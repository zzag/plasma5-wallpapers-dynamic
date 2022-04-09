/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "dynamicwallpaperdescription.h"
#include "dynamicwallpaperengine.h"

#include <KSunPath>
#include <KSunPosition>

class SolarDynamicWallpaperEngine : public DynamicWallpaperEngine
{
public:
    bool isExpired() const override;
    void update() override;

    static SolarDynamicWallpaperEngine *create(const DynamicWallpaperDescription &description,
                                               const QGeoCoordinate &location);

private:
    SolarDynamicWallpaperEngine(const DynamicWallpaperDescription &description,
                                const KSunPath &sunPath, const KSunPosition &midnight,
                                const QGeoCoordinate &location, const QDateTime &dateTime);
    qreal progressForPosition(const KSunPosition &position) const;
    qreal progressForMetaData(const KDynamicWallpaperMetaData &metaData) const;
    qreal progressForDateTime(const QDateTime &dateTime) const;

    DynamicWallpaperDescription m_description;
    QMap<qreal, int> m_progressToImageIndex;
    KSunPath m_sunPath;
    KSunPosition m_midnight;
    QGeoCoordinate m_location;
    QDateTime m_dateTime;
};
