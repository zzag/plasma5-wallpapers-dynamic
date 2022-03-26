/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#pragma once

#include "kdynamicwallpaper_export.h"

#include <QIODevice>

class KDynamicWallpaperMetaData;
class KDynamicWallpaperWriterPrivate;

class KDYNAMICWALLPAPER_EXPORT KDynamicWallpaperWriter
{
public:
    enum WallpaperWriterError {
        NoError,
        DeviceError,
        EncoderError,
        UnknownError,
    };

    KDynamicWallpaperWriter();
    ~KDynamicWallpaperWriter();

    void setMetaData(const QList<KDynamicWallpaperMetaData> &metaData);
    QList<KDynamicWallpaperMetaData> metaData() const;

    void setImages(const QList<QImage> &images);
    QList<QImage> images() const;

    bool flush(QIODevice *device);
    bool flush(const QString &fileName);

    WallpaperWriterError error() const;
    QString errorString() const;

    static bool canWrite(QIODevice *device);
    static bool canWrite(const QString &fileName);

private:
    QScopedPointer<KDynamicWallpaperWriterPrivate> d;
};
