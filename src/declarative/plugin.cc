/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "plugin.h"
#include "ClockSkewNotifier.h"
#include "DynamicWallpaper.h"
#include "DynamicWallpaperInstaller.h"
#include "WallpapersModel.h"

#include <QQmlEngine>

void Plugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("com.github.zzag.private.wallpaper"));
    qmlRegisterType<DynamicWallpaper>(uri, 1, 0, "DynamicWallpaper");
    qmlRegisterType<WallpapersModel>(uri, 1, 0, "WallpapersModel");
    qmlRegisterType<ClockSkewNotifier>(uri, 1, 1, "ClockSkewNotifier");
    qmlRegisterType<DynamicWallpaperInstaller>(uri, 1, 2, "DynamicWallpaperInstaller");
}
