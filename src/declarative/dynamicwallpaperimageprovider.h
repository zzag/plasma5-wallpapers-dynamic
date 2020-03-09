/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QQuickImageProvider>

class DynamicWallpaperImageProvider : public QQuickImageProvider
{
public:
    DynamicWallpaperImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
};
