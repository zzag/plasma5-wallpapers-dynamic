/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "dynamicwallpaperengine.h"

class TimedDynamicWallpaperEngine : public DynamicWallpaperEngine
{
public:
    static TimedDynamicWallpaperEngine *create();

protected:
    qreal progressForMetaData(const KDynamicWallpaperMetaData &metaData) const override;
    qreal progressForDateTime(const QDateTime &dateTime) const override;
};
