/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QImage>

class DynamicWallpaperImageAsyncResult
{
public:
    DynamicWallpaperImageAsyncResult() {}
    explicit DynamicWallpaperImageAsyncResult(const QImage &image) : image(image) {}
    explicit DynamicWallpaperImageAsyncResult(const QString &text) : errorString(text) {}

    QImage image;
    QString errorString;
};
