/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QImage>

class DynamicWallpaperPreviewCache
{
public:
    static QImage load(const QString &fileName);
    static void store(const QImage &image, const QString &fileName);
};
