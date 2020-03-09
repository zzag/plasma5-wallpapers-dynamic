/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperimageprovider.h"
#include "dynamicwallpaperimagehandle.h"

#include <KDynamicWallpaperReader>

#include <QDebug>

DynamicWallpaperImageProvider::DynamicWallpaperImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage DynamicWallpaperImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    const DynamicWallpaperImageHandle handle = DynamicWallpaperImageHandle::fromString(id);
    if (!handle.isValid()) {
        qWarning() << "Failed to decode a DynamicWallpaperImageHandle from" << id;
        return QImage();
    }

    KDynamicWallpaperReader reader(handle.fileName());

    QImage image = reader.imageAt(handle.imageIndex());
    if (reader.error() != KDynamicWallpaperReader::NoError) {
        qWarning("Failed to read %s: %s", qPrintable(reader.fileName()), qPrintable(reader.errorString()));
        return QImage();
    }

    *size = image.size();

    // If the sourceSize is set, scale the image to that size and ignore the aspect ratio.
    if (requestedSize.isValid())
        image = image.scaled(requestedSize);

    return image;
}
