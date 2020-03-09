/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperengine_solar.h"

#include <cmath>

SolarDynamicWallpaperEngine::SolarDynamicWallpaperEngine(const KSunPath &sunPath,
                                                         const KSunPosition &midnight,
                                                         const QGeoCoordinate &location,
                                                         const QDateTime &dateTime)
    : m_sunPath(sunPath)
    , m_midnight(midnight)
    , m_location(location)
    , m_dateTime(dateTime)
{
}

bool SolarDynamicWallpaperEngine::isExpired() const
{
    return m_dateTime.date() != QDate::currentDate();
}

SolarDynamicWallpaperEngine *SolarDynamicWallpaperEngine::create(const QGeoCoordinate &location)
{
    const QDateTime dateTime = QDateTime::currentDateTime();

    const KSunPosition midnight = KSunPosition::midnight(dateTime, location);
    if (!midnight.isValid())
        return nullptr;

    const KSunPath path = KSunPath::create(dateTime, location);
    if (!path.isValid())
        return nullptr;

    return new SolarDynamicWallpaperEngine(path, midnight, location, dateTime);
}

qreal SolarDynamicWallpaperEngine::progressForMetaData(const KDynamicWallpaperMetaData &metaData) const
{
    const KSunPosition position(metaData.solarElevation(), metaData.solarAzimuth());
    return progressForPosition(position);
}

qreal SolarDynamicWallpaperEngine::progressForDateTime(const QDateTime &dateTime) const
{
    const KSunPosition position(dateTime, m_location);
    return progressForPosition(position);
}

qreal SolarDynamicWallpaperEngine::progressForPosition(const KSunPosition &position) const
{
    const QVector3D projectedMidnight = m_sunPath.project(m_midnight);
    const QVector3D projectedPosition = m_sunPath.project(position);

    const QVector3D v1 = (projectedMidnight - m_sunPath.center()).normalized();
    const QVector3D v2 = (projectedPosition - m_sunPath.center()).normalized();

    const QVector3D cross = QVector3D::crossProduct(v1, v2);
    const float dot = QVector3D::dotProduct(v1, v2);
    const float det = QVector3D::dotProduct(m_sunPath.normal(), cross);

    qreal angle = std::atan2(det, dot);
    if (angle < 0)
        angle += 2 * M_PI;

    return angle / (2 * M_PI);
}
