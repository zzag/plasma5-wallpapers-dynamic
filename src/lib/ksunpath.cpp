/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "ksunpath.h"
#include "ksunposition.h"

#include <QVector>

#include <cmath>

/*!
 * \class KSunPath
 * \brief The KSunPath class represents a path of the Sun at the given time and location.
 */

static QVector3D computeNormal(const QVector3D &center, const QVector3D &v1, const QVector3D &v2)
{
    const QVector3D cross = QVector3D::crossProduct(v1 - center, v2 - center);
    return cross.normalized();
}

static QVector3D positionToVector(const KSunPosition &position)
{
    return position.toVector();
}

/*!
 * Creates a path of the Sun at the specified date and location.
 */
KSunPath KSunPath::create(const QDateTime &dateTime, const QGeoCoordinate &location)
{
    // I bet there is a nice formula to determine the normal and the center of
    // the sun path. Sampling the position of the Sun is not that bad, however
    // having the computed results as accurate as possible is still something we
    // have to strive for.
    const int sampleCount = 24;

    const QDateTime utcDateTime = dateTime.toUTC();
    const QDate utcDate = utcDateTime.date();

    QVector<KSunPosition> positions;
    positions.reserve(sampleCount);

    for (int i = 0; i < sampleCount; ++i) {
        const QTime utcTime(i, 0);
        const QDateTime sampleDataTime(utcDate, utcTime, Qt::UTC);
        const KSunPosition position(sampleDataTime, location);
        if (!position.isValid())
            return KSunPath();
        positions << position;
    }

    QVector<QVector3D> samples;
    samples.reserve(sampleCount);
    std::transform(positions.constBegin(), positions.constEnd(),
                   std::back_inserter(samples), positionToVector);

    QVector3D center = std::accumulate(samples.constBegin(), samples.constEnd(), QVector3D());
    center /= sampleCount;

    float radius = 0;
    for (const QVector3D &sample : samples)
        radius += (sample - center).length();
    radius /= sampleCount;

    QVector3D normal;
    for (int i = 1; i < samples.count(); ++i) {
        const QVector3D v1 = samples.at(i - 1);
        const QVector3D v2 = samples.at(i);
        normal += computeNormal(center, v1, v2);
    }
    normal.normalize();

    return KSunPath(center, normal, radius);
}

/*!
 * Constructs a KSunPath with \p center, \p normal, and \p radius.
 */
KSunPath::KSunPath(const QVector3D &center, const QVector3D &normal, float radius)
    : m_center(center)
    , m_normal(normal)
    , m_radius(radius)
{
}

/*!
 * Returns \c true if the path of the Sun is valid; otherwise returns \c false.
 */
bool KSunPath::isValid() const
{
    return !qFuzzyIsNull(m_normal.x());
}

/*!
 * Returns the coordinates of the center of this path of the Sun.
 */
QVector3D KSunPath::center() const
{
    return m_center;
}

/*!
 * Returns the normal of the plane that contains this path of the Sun.
 */
QVector3D KSunPath::normal() const
{
    return m_normal;
}

/*!
 * Projects the specified KSunPosition onto this KSunPath.
 */
QVector3D KSunPath::project(const KSunPosition &position) const
{
    if (!isValid())
        return QVector3D();

    const QVector3D axis = QVector3D(1, 0, 0);
    const QVector3D origin = QVector3D(0, 0, 0);
    const QVector3D normal = computeNormal(origin, axis, position.toVector());
    if (normal.isNull())
        return QVector3D();

    const QVector3D direction = QVector3D::crossProduct(m_normal, normal).normalized();
    const QVector3D point = QVector3D::crossProduct(normal, direction) * QVector3D::dotProduct(m_normal, m_center) +
        QVector3D::crossProduct(direction, m_normal) * QVector3D::dotProduct(normal, origin);

    const QVector3D delta = point - m_center;
    const float dot = QVector3D::dotProduct(direction, delta);
    const float discriminator = dot * dot - delta.lengthSquared() + m_radius * m_radius;
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
