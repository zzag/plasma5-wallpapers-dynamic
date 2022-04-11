/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QtQuick/private/qquickpixmapcache_p.h> // TODO: Use stable API in Qt 6

class DynamicWallpaperImageProvider : public QQuickImageProviderWithOptions
{
public:
    DynamicWallpaperImageProvider();

    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize,
                                              const QQuickImageProviderOptions &options) override;
};
