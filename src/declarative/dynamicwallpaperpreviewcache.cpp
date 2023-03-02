/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperpreviewcache.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>

static QString cacheKey(const QString &fileName)
{
    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(QFile::encodeName(fileName));
    return QString::fromLatin1(hash.result().toHex()) + QStringLiteral(".png");
}

static QString cacheRoot()
{
    QString cache = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
    return cache + QLatin1String("/kdynamicwallpaper/");
}

static QString cacheFileName(const QString &fileName)
{
    return cacheRoot() + cacheKey(fileName);
}

/*!
 * Loads the preview for a wallpaper with the specified \a fileName from the cache.
 *
 * If the cache has no preview for a wallpaper with the given \a fileName or the cached preview
 * image is outdated, this method will return a null QImage object.
 *
 * This function can be called from multiple threads simultaneously.
 */
QImage DynamicWallpaperPreviewCache::load(const QString &fileName)
{
    QImage image(cacheFileName(fileName));
    if (image.isNull())
        return QImage();

    const qint64 lastCreated = image.text(QStringLiteral("Preview:Timestamp")).toLongLong();
    const qint64 lastModified = QFileInfo(fileName).lastModified().toSecsSinceEpoch();

    if (lastModified > lastCreated)
        return QImage();

    return image;
}

/*!
 * Stores the preview \a image for a wallpaper with the specified \a fileName in the cache.
 *
 * This function can be called from multiple threads simultaneously.
 */
void DynamicWallpaperPreviewCache::store(const QImage &image, const QString &fileName)
{
    const QDir cache(cacheRoot());
    if (!cache.exists())
        cache.mkpath(QStringLiteral("."));

    const qint64 modifiedTimestamp = QFileInfo(fileName).lastModified().toSecsSinceEpoch();

    QImage scaled = image.scaled(512, 512, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    scaled.setText(QStringLiteral("Preview:Timestamp"), QString::number(modifiedTimestamp));
    scaled.save(cacheFileName(fileName));
}
