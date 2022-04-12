/*
 * SPDX-FileCopyrightText: 2022 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "dynamicwallpaperengine.h"

#include <KDynamicWallpaperMetaData>

#include <QGeoCoordinate>

class DayNightDynamicWallpaperEngine : public DynamicWallpaperEngine
{
public:
    void update() override;

    static DayNightDynamicWallpaperEngine *create(const QList<KDynamicWallpaperMetaData> &metadata,
                                                  const QUrl &source,
                                                  const QGeoCoordinate &location);

private:
    DayNightDynamicWallpaperEngine(const QList<KDynamicWallpaperMetaData> &metadata,
                                   const QUrl &source,
                                   const QGeoCoordinate &location);

    QGeoCoordinate m_location;
};
