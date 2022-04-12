/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "dynamicwallpaperengine.h"

#include <KDynamicWallpaperMetaData>
#include <KSunPath>
#include <KSunPosition>

class SolarDynamicWallpaperEngine : public DynamicWallpaperEngine
{
public:
    bool isExpired() const override;
    void update() override;

    static SolarDynamicWallpaperEngine *create(const QList<KDynamicWallpaperMetaData> &metadata,
                                               const QUrl &source,
                                               const QGeoCoordinate &location);

private:
    SolarDynamicWallpaperEngine(const QList<KDynamicWallpaperMetaData> &metadata,
                                const QUrl &source,
                                const KSunPath &sunPath, const KSunPosition &midnight,
                                const QGeoCoordinate &location, const QDateTime &dateTime);
    SolarDynamicWallpaperEngine(const QList<KDynamicWallpaperMetaData> &metadata,
                                const QUrl &source, const QDateTime &dateTime);

    qreal progressForPosition(const KSunPosition &position) const;
    qreal progressForMetaData(const KSolarDynamicWallpaperMetaData &metaData) const;
    qreal progressForDateTime(const QDateTime &dateTime) const;

    enum class Mode {
        Normal,
        Fallback,
    };

    Mode m_mode;
    QUrl m_source;
    QMap<qreal, KSolarDynamicWallpaperMetaData> m_progressToMetaData;
    KSunPath m_sunPath;
    KSunPosition m_midnight;
    QGeoCoordinate m_location;
    QDateTime m_dateTime;
};
