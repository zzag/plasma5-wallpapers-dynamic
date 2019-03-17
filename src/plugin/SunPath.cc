/*
 * Copyright (C) 2019 Vlad Zagorodniy <vladzzag@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// Own
#include "SunPath.h"
#include "SunPosition.h"
#include "utils.h"

// Qt
#include <QVector>

// std
#include <cmath>

SunPath::SunPath(const QDateTime& now, qreal latitude, qreal longitude)
{
    auto at = [&](const QTime& time) {
        return SunPosition(QDateTime(now.date(), time), latitude, longitude);
    };

    const int numSamples = 24;
    QVector<QVector3D> samples;
    samples.reserve(numSamples);
    for (int i = 0; i < numSamples; ++i) {
        samples << at(QTime(i, 0)).toVector();
    }

    for (const QVector3D& sample : samples)
        m_center += sample;
    m_center /= samples.count();

    for (int i = 1; i < samples.count(); ++i) {
        const QVector3D v1 = samples.at(i - 1);
        const QVector3D v2 = samples.at(i);
        m_normal += computeNormal(m_center, v1, v2);
    }
    m_normal /= samples.count() - 1;

    m_midnight = project(at(QTime(0, 0)));
}

bool SunPath::isValid() const
{
    return !qFuzzyIsNull(m_normal.x());
}

QVector3D SunPath::center() const
{
    return m_center;
}

QVector3D SunPath::normal() const
{
    return m_normal;
}

QVector3D SunPath::midnight() const
{
    return m_midnight;
}

QVector3D SunPath::project(const SunPosition& position) const
{
    if (!isValid())
        return QVector3D();

    const QVector3D axis = QVector3D(1, 0, 0);
    const QVector3D origin = QVector3D(0, 0, 0);
    const QVector3D normal = computeNormal(origin, axis, position.toVector());
    if (normal.isNull())
        return QVector3D();

    const QVector3D direction = QVector3D::crossProduct(m_normal, normal).normalized();
    const QVector3D point = QVector3D::crossProduct(normal, direction) * QVector3D::dotProduct(m_normal, m_center)
        + QVector3D::crossProduct(direction, m_normal) * QVector3D::dotProduct(normal, origin);

    const float dot = QVector3D::dotProduct(direction, point);
    const float discriminator = dot * dot - point.lengthSquared() + 1;
    if (discriminator < 0)
        return QVector3D();

    if (qFuzzyIsNull(discriminator))
        return point - dot * direction;

    const QVector3D a = point + direction * (-dot - std::sqrt(discriminator));
    const QVector3D b = point + direction * (-dot + std::sqrt(discriminator));

    if (qFuzzyIsNull(position.elevation())) {
        if (position.azimuth() < 180)
            return (a.y() < b.y()) ? b : a;
        return (a.y() < b.y()) ? a : b;
    }

    if (position.elevation() < 0)
        return (a.z() < b.z()) ? a : b;

    return (a.z() < b.z()) ? b : a;
}
