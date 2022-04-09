/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#pragma once

#include "kdynamicwallpaper_export.h"
#include "kdynamicwallpapermetadata.h"

#include <QIODevice>
#include <QImage>

#include <optional>

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

    class ImageView
    {
    public:
        explicit ImageView(const QString &fileName)
            : m_fileName(fileName)
        {
        }

        QImage data() const
        {
            return QImage(m_fileName);
        }

    private:
        QImage m_fileName;
    };

    KDynamicWallpaperWriter();
    ~KDynamicWallpaperWriter();

    void setMetaData(const QList<KDynamicWallpaperMetaData> &metaData);
    QList<KDynamicWallpaperMetaData> metaData() const;

    void setImages(const QList<ImageView> &views);
    QList<ImageView> images() const;

    bool flush(QIODevice *device);
    bool flush(const QString &fileName);

    void setMaxThreadCount(int max);
    std::optional<int> maxThreadCount() const;

    WallpaperWriterError error() const;
    QString errorString() const;

    static bool canWrite(QIODevice *device);
    static bool canWrite(const QString &fileName);

private:
    QScopedPointer<KDynamicWallpaperWriterPrivate> d;
};
