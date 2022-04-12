/*
 * SPDX-FileCopyrightText: 2022 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperengine_daynight.h"
#include "dynamicwallpaperimagehandle.h"

#include <KSunPosition>

DayNightDynamicWallpaperEngine *DayNightDynamicWallpaperEngine::create(const QList<KDynamicWallpaperMetaData> &metadata,
                                                                       const QUrl &source,
                                                                       const QGeoCoordinate &location)
{
    return new DayNightDynamicWallpaperEngine(metadata, source, location);
}

DayNightDynamicWallpaperEngine::DayNightDynamicWallpaperEngine(const QList<KDynamicWallpaperMetaData> &metadata,
                                                               const QUrl &source,
                                                               const QGeoCoordinate &location)
    : m_location(location)
{
    for (const KDynamicWallpaperMetaData &md : metadata) {
        const auto &dayNight = std::get<KDayNightDynamicWallpaperMetaData>(md);
        if (dayNight.timeOfDay() == KDayNightDynamicWallpaperMetaData::TimeOfDay::Day) {
            m_topLayer = DynamicWallpaperImageHandle(source.toLocalFile(), dayNight.index()).toUrl();
        } else if (dayNight.timeOfDay() == KDayNightDynamicWallpaperMetaData::TimeOfDay::Night) {
            m_bottomLayer = DynamicWallpaperImageHandle(source.toLocalFile(), dayNight.index()).toUrl();
        }
    }
}

void DayNightDynamicWallpaperEngine::update()
{
    if (m_location.isValid()) {
        const KSunPosition sunPosition(QDateTime::currentDateTime(), m_location);
        if (sunPosition.isValid()) {
            m_blendFactor = (qBound(-6.0, sunPosition.elevation(), 6.0) + 6.0) / 12.0;
            return;
        }
    }

    const QTime currentTime = QTime::currentTime();
    if (currentTime < QTime(6, 0)) {
        m_blendFactor = 0;
    } else if (currentTime < QTime(7, 0)) {
        m_blendFactor = QTime(6, 0).secsTo(currentTime) / 3600.0;
    } else if (currentTime < QTime(18, 0)) {
        m_blendFactor = 1;
    } else if (currentTime < QTime(19, 0)) {
        m_blendFactor = currentTime.secsTo(QTime(19, 0)) / 3600.0;
    } else {
        m_blendFactor = 0;
    }
}
