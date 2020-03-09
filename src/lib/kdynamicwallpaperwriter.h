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

    enum Codec {
        HEVC,
        AVC,
    };

    enum WriteOption {
        PreserveColorProfile = 1 << 0,
        Primary              = 1 << 1,
    };
    Q_DECLARE_FLAGS(WriteOptions, WriteOption)

    KDynamicWallpaperWriter();
    ~KDynamicWallpaperWriter();

    void setQuality(int quality);
    int quality() const;

    void setLossless(bool lossless);
    bool isLossless() const;

    void setCodec(Codec codec);
    Codec codec() const;

    bool begin(QIODevice *device);
    bool begin(const QString &fileName);
    void end();
    bool isActive() const;

    bool write(const QImage &image, const KDynamicWallpaperMetaData &metaData,
               WriteOptions options = WriteOptions());

    WallpaperWriterError error() const;
    QString errorString() const;

    static bool canWrite(QIODevice *device);
    static bool canWrite(const QString &fileName);

private:
    QScopedPointer<KDynamicWallpaperWriterPrivate> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KDynamicWallpaperWriter::WriteOptions)
