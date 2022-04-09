/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperengine_timed.h"

TimedDynamicWallpaperEngine *TimedDynamicWallpaperEngine::create()
{
    return new TimedDynamicWallpaperEngine();
}

qreal TimedDynamicWallpaperEngine::progressForMetaData(const KSolarDynamicWallpaperMetaData &metaData) const
{
    return metaData.time();
}

qreal TimedDynamicWallpaperEngine::progressForDateTime(const QDateTime &dateTime) const
{
    QDateTime midnight = dateTime;
    midnight.setTime(QTime());

    return midnight.secsTo(dateTime) / 86400.0;
}
