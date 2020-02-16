/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperextensionplugin.h"
#include "clockskewnotifier.h"
#include "dynamicwallpaper.h"
#include "dynamicwallpaperinstaller.h"
#include "dynamicwallpaperpreviewprovider.h"
#include "wallpapersmodel.h"

#include <QQmlEngine>

void Plugin::registerTypes(const char *uri)
{
    qmlRegisterType<DynamicWallpaper>(uri, 1, 0, "DynamicWallpaper");
    qmlRegisterType<WallpapersModel>(uri, 1, 0, "WallpapersModel");
    qmlRegisterType<ClockSkewNotifier>(uri, 1, 1, "ClockSkewNotifier");
    qmlRegisterType<DynamicWallpaperInstaller>(uri, 1, 2, "DynamicWallpaperInstaller");
}

void Plugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(uri)
    engine->addImageProvider(QLatin1String("dynamicpreview"), new DynamicWallpaperPreviewProvider);
}
