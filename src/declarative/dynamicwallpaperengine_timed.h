/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "dynamicwallpaperdescription.h"
#include "dynamicwallpaperengine.h"

class TimedDynamicWallpaperEngine : public DynamicWallpaperEngine
{
public:
    void update() override;

    static TimedDynamicWallpaperEngine *create(const DynamicWallpaperDescription &description);

private:
    explicit TimedDynamicWallpaperEngine(const DynamicWallpaperDescription &description);

    qreal progressForMetaData(const KDynamicWallpaperMetaData &metaData) const;
    qreal progressForDateTime(const QDateTime &dateTime) const;

    DynamicWallpaperDescription m_description;
    QMap<qreal, int> m_progressToImageIndex;
};
