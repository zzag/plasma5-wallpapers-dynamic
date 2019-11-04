/*
 * Copyright (C) 2019 Vlad Zahorodnii <vladzzag@gmail.com>
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
#include "DynamicWallpaperModel.h"
#include "DynamicWallpaperPackage.h"
#include "SunPosition.h"

// KF
#include <KLocalizedString>

// Qt
#include <QtMath>

// std
#include <algorithm>

const static qreal ARC_LENGTH = 2 * M_PI;

static qreal computeTime(const SunPath &path, const SunPosition &position)
{
    const QVector3D projected = path.project(position);

    const QVector3D v1 = path.midnight() - path.center();
    const QVector3D v2 = projected - path.center();

    const QVector3D cross = QVector3D::crossProduct(v1, v2);
    const float dot = QVector3D::dotProduct(v1, v2);
    const float det = QVector3D::dotProduct(path.normal(), cross);

    qreal angle = std::atan2(det, dot);
    if (angle < 0)
        angle += ARC_LENGTH;

    return angle / ARC_LENGTH;
}

SolarDynamicWallpaperModel::SolarDynamicWallpaperModel(const DynamicWallpaperPackage *wallpaper, qreal latitude, qreal longitude)
    : m_dateTime(QDateTime::currentDateTime())
    , m_latitude(latitude)
    , m_longitude(longitude)
{
    m_sunPath = SunPath(m_dateTime, latitude, longitude);
    if (!m_sunPath.isValid())
        return;

    const QVector<WallpaperImage> images = wallpaper->images();
    for (const WallpaperImage &image : images) {
        const qreal time = computeTime(m_sunPath, image.position);
        m_knots << Knot { time, image.url };
    }

    std::sort(m_knots.begin(), m_knots.end());
}

bool SolarDynamicWallpaperModel::isExpired() const
{
    // Rebuild the model each hour.
    const QDateTime now = QDateTime::currentDateTime();
    return qAbs(now.secsTo(m_dateTime)) > 3600;
}

bool SolarDynamicWallpaperModel::isValid() const
{
    return m_sunPath.isValid();
}

QString SolarDynamicWallpaperModel::errorText() const
{
    if (!m_sunPath.isValid())
        return i18n("Could not construct the path of the Sun");
    return QString();
}

void SolarDynamicWallpaperModel::update()
{
    const QDateTime now(QDateTime::currentDateTime());
    const SunPosition position(now, m_latitude, m_longitude);
    m_time = computeTime(m_sunPath, position);
}

TimedDynamicWallpaperModel::TimedDynamicWallpaperModel(const DynamicWallpaperPackage *package)
{
    const QVector<WallpaperImage> images = package->images();
    for (const WallpaperImage &image : images) {
        m_knots << Knot { image.time, image.url };
    }

    std::sort(m_knots.begin(), m_knots.end());
}

void TimedDynamicWallpaperModel::update()
{
    const int elapsedMsecs = QTime::currentTime().msecsSinceStartOfDay();
    const int msecsPerDay = 86400000;
    m_time = qreal(elapsedMsecs) / msecsPerDay;
}

DynamicWallpaperModel::~DynamicWallpaperModel()
{
}

bool DynamicWallpaperModel::isExpired() const
{
    return false;
}

bool DynamicWallpaperModel::isValid() const
{
    return true;
}

QString DynamicWallpaperModel::errorText() const
{
    return QString();
}

QUrl DynamicWallpaperModel::bottomLayer() const
{
    return currentBottomKnot().url;
}

QUrl DynamicWallpaperModel::topLayer() const
{
    return currentTopKnot().url;
}

static qreal computeTimeSpan(qreal from, qreal to)
{
    if (to < from)
        return (1 - from) + to;

    return to - from;
}

qreal DynamicWallpaperModel::blendFactor() const
{
    const qreal from = currentBottomKnot().time;
    const qreal to = currentTopKnot().time;

    const qreal reflectedFrom = 1 - from;
    const qreal reflectedTo = 1 - to;

    const qreal totalDuration = computeTimeSpan(from, to);
    const qreal totalElapsed = computeTimeSpan(from, m_time);

    if ((reflectedFrom < from) ^ (reflectedTo < to)) {
        if (reflectedFrom < to) {
            const qreal threshold = computeTimeSpan(from, reflectedFrom);
            if (totalElapsed < threshold)
                return 0;
            return (totalElapsed - threshold) / (totalDuration - threshold);
        }
        if (from < reflectedTo) {
            const qreal threshold = computeTimeSpan(from, reflectedTo);
            if (threshold < totalElapsed)
                return 1;
            return totalElapsed / threshold;
        }
    }

    return totalElapsed / totalDuration;
}

DynamicWallpaperModel::Knot DynamicWallpaperModel::currentBottomKnot() const
{
    const Knot dummy { m_time, QUrl() };

    if (dummy < m_knots.first())
        return m_knots.last();

    if (m_knots.last() <= dummy)
        return m_knots.last();

    auto it = std::lower_bound(m_knots.begin(), m_knots.end(), dummy);
    if (dummy < *it)
        return *std::prev(it);

    return *it;
}

DynamicWallpaperModel::Knot DynamicWallpaperModel::currentTopKnot() const
{
    const Knot dummy { m_time, QUrl() };

    if (dummy < m_knots.first())
        return m_knots.first();

    if (m_knots.last() <= dummy)
        return m_knots.first();

    auto it = std::lower_bound(m_knots.begin(), m_knots.end(), dummy);
    if (dummy < *it)
        return *it;

    return *std::next(it);
}
