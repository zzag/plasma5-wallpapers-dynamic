/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#pragma once

#include "kdynamicwallpaper_export.h"
#include "kdynamicwallpapermetadata.h"

#include <QIODevice>

class KDynamicWallpaperReaderPrivate;

class KDYNAMICWALLPAPER_EXPORT KDynamicWallpaperReader
{
public:
    enum WallpaperReaderError {
        NoError,
        OpenError,
        ReadError,
    };

    KDynamicWallpaperReader();
    explicit KDynamicWallpaperReader(QIODevice *device);
    explicit KDynamicWallpaperReader(const QString &fileName);
    ~KDynamicWallpaperReader();

    void setDevice(QIODevice *device);
    QIODevice *device() const;

    void setFileName(const QString &fileName);
    QString fileName() const;

    QList<KDynamicWallpaperMetaData> metaData() const;

    int imageCount() const;
    QImage image(int imageIndex) const;

    WallpaperReaderError error() const;
    QString errorString() const;

    static bool canRead(QIODevice *device);
    static bool canRead(const QString &fileName);

private:
    QScopedPointer<KDynamicWallpaperReaderPrivate> d;
};
