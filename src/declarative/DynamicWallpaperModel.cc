/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

// Own
#include "DynamicWallpaperModel.h"
#include "DynamicWallpaperPackage.h"

// Qt
#include <QtMath>

// std
#include <algorithm>

const static qreal ARC_LENGTH = 2 * M_PI;

static qreal computeTime(const SunPath &path, const SunPosition &midnight, const SunPosition &position)
{
    const QVector3D projectedMidnight = path.project(midnight);
    const QVector3D projectedPosition = path.project(position);

    const QVector3D v1 = (projectedMidnight - path.center()).normalized();
    const QVector3D v2 = (projectedPosition - path.center()).normalized();

    const QVector3D cross = QVector3D::crossProduct(v1, v2);
    const float dot = QVector3D::dotProduct(v1, v2);
    const float det = QVector3D::dotProduct(path.normal(), cross);

    qreal angle = std::atan2(det, dot);
    if (angle < 0)
        angle += ARC_LENGTH;

    return angle / ARC_LENGTH;
}

SolarDynamicWallpaperModel *SolarDynamicWallpaperModel::create(std::shared_ptr<DynamicWallpaperPackage> wallpaper,
                                                               const QGeoCoordinate &location)
{
    const QDateTime dateTime = QDateTime::currentDateTime();

    const SunPosition midnight = SunPosition::midnight(dateTime, location);
    if (!midnight.isValid())
        return nullptr;

    const SunPath path = SunPath::create(dateTime, location);
    if (!path.isValid())
        return nullptr;

    return new SolarDynamicWallpaperModel(wallpaper, dateTime, location, path, midnight);
}

SolarDynamicWallpaperModel::SolarDynamicWallpaperModel(std::shared_ptr<DynamicWallpaperPackage> wallpaper,
                                                       const QDateTime &dateTime,
                                                       const QGeoCoordinate &location,
                                                       const SunPath &path,
                                                       const SunPosition &midnight)
    : DynamicWallpaperModel(wallpaper)
    , m_sunPath(path)
    , m_midnight(midnight)
    , m_dateTime(dateTime)
    , m_location(location)
{
    const QVector<WallpaperImage> images = wallpaper->images();
    for (const WallpaperImage &image : images) {
        const qreal time = computeTime(m_sunPath, m_midnight, image.position);
        m_knots << Knot { time, image.url };
    }
    std::sort(m_knots.begin(), m_knots.end());
}

bool SolarDynamicWallpaperModel::isExpired() const
{
    // Rebuild the model every hour.
    const QDateTime now = QDateTime::currentDateTime();
    return std::abs(now.secsTo(m_dateTime)) > 3600;
}

void SolarDynamicWallpaperModel::update()
{
    const QDateTime now(QDateTime::currentDateTime());
    const SunPosition position(now, m_location);
    m_time = computeTime(m_sunPath, m_midnight, position);
}

TimedDynamicWallpaperModel *TimedDynamicWallpaperModel::create(std::shared_ptr<DynamicWallpaperPackage> wallpaper)
{
    return new TimedDynamicWallpaperModel(wallpaper);
}

TimedDynamicWallpaperModel::TimedDynamicWallpaperModel(std::shared_ptr<DynamicWallpaperPackage> wallpaper)
    : DynamicWallpaperModel(wallpaper)
{
    const QVector<WallpaperImage> images = wallpaper->images();
    for (const WallpaperImage &image : images)
        m_knots << Knot { image.time, image.url };
    std::sort(m_knots.begin(), m_knots.end());
}

void TimedDynamicWallpaperModel::update()
{
    const int elapsedMilliSeconds = QTime::currentTime().msecsSinceStartOfDay();
    const int milliSecondsPerDay = 86400000;
    m_time = qreal(elapsedMilliSeconds) / milliSecondsPerDay;
}

DynamicWallpaperModel::DynamicWallpaperModel(std::shared_ptr<DynamicWallpaperPackage> wallpaper)
    : m_wallpaper(std::move(wallpaper))
{
}

DynamicWallpaperModel::~DynamicWallpaperModel()
{
}

bool DynamicWallpaperModel::isExpired() const
{
    return false;
}

QUrl DynamicWallpaperModel::bottomLayer() const
{
    return currentBottomKnot().url;
}

QUrl DynamicWallpaperModel::topLayer() const
{
    return currentTopKnot().url;
}

DynamicWallpaperPackage *DynamicWallpaperModel::wallpaper() const
{
    return m_wallpaper.get();
}

static qreal computeTimeSpan(qreal from, qreal to)
{
    if (to < from)
        return (1 - from) + to;

    return to - from;
}

static qreal computeBlendFactor(qreal from, qreal to, qreal now)
{
    const qreal reflectedFrom = 1 - from;
    const qreal reflectedTo = 1 - to;

    const qreal totalDuration = computeTimeSpan(from, to);
    const qreal totalElapsed = computeTimeSpan(from, now);

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

qreal DynamicWallpaperModel::blendFactor() const
{
    const qreal from = currentBottomKnot().time;
    const qreal to = currentTopKnot().time;

    const qreal blendFactor = computeBlendFactor(from, to, m_time);

    if (!wallpaper()->isSmooth())
        return std::round(blendFactor);

    return blendFactor;
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
