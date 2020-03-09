/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "dynamicwallpaperengine.h"

#include <KSunPath>
#include <KSunPosition>

class SolarDynamicWallpaperEngine : public DynamicWallpaperEngine
{
public:
    bool isExpired() const override;

    static SolarDynamicWallpaperEngine *create(const QGeoCoordinate &location);

protected:
    qreal progressForMetaData(const KDynamicWallpaperMetaData &metaData) const override;
    qreal progressForDateTime(const QDateTime &dateTime) const override;

private:
    SolarDynamicWallpaperEngine(const KSunPath &sunPath, const KSunPosition &midnight,
                                const QGeoCoordinate &location, const QDateTime &dateTime);
    qreal progressForPosition(const KSunPosition &position) const;

    KSunPath m_sunPath;
    KSunPosition m_midnight;
    QGeoCoordinate m_location;
    QDateTime m_dateTime;
};
