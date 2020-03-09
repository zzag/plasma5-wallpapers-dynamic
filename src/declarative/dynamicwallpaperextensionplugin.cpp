/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperextensionplugin.h"
#include "dynamicwallpaperhandler.h"
#include "dynamicwallpaperimageprovider.h"
#include "dynamicwallpapermodel.h"
#include "dynamicwallpaperpreviewprovider.h"

#include <KSystemClockMonitor>

#include <QQmlEngine>

void DynamicWallpaperExtensionPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<DynamicWallpaperHandler>(uri, 1, 0, "DynamicWallpaperHandler");
    qmlRegisterType<DynamicWallpaperModel>(uri, 1, 0, "DynamicWallpaperModel");
    qmlRegisterType<KSystemClockMonitor>(uri, 1, 0, "SystemClockMonitor");
}

void DynamicWallpaperExtensionPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(uri)
    engine->addImageProvider(QLatin1String("dynamic"), new DynamicWallpaperImageProvider);
    engine->addImageProvider(QLatin1String("dynamicpreview"), new DynamicWallpaperPreviewProvider);
}
