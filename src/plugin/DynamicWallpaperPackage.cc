/*
 * Copyright (C) 2019 Vlad Zagorodniy <vladzzag@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// Own
#include "DynamicWallpaperPackage.h"

// KF
#include <KPackage/PackageLoader>

// Qt
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>

WallpaperType DynamicWallpaperPackage::type() const
{
    return m_type;
}

QVector<WallpaperImage> DynamicWallpaperPackage::images() const
{
    return m_images;
}

static bool parseSolarMetaData(const KPackage::Package& package, const QJsonObject& rootObject, QVector<WallpaperImage>& images)
{
    const QJsonArray metaData = rootObject.value(QLatin1String("MetaData")).toArray();
    if (metaData.isEmpty()) {
        qWarning() << "A dynamic wallpaper should contain at least one image";
        return false;
    }

    images.reserve(metaData.count());

    for (int i = 0; i < metaData.count(); ++i) {
        const QJsonObject rawImage = metaData.at(i).toObject();
        if (rawImage.isEmpty()) {
            qWarning("Image with index %d doesn't have any associated metadata", i);
            return false;
        }

        if (!rawImage.value(QLatin1String("Azimuth")).isDouble()) {
            qWarning("Image with index %d has invalid azimuth value type", i);
            return false;
        }

        const qreal azimuth = rawImage.value(QLatin1String("Azimuth")).toDouble();
        if (std::abs(azimuth) > 360) {
            qWarning("Image with index %d has invalid azimuth value", i);
            return false;
        }

        if (!rawImage.value(QLatin1String("Elevation")).isDouble()) {
            qWarning("Image with index %d has invalid elevation value type", i);
            return false;
        }

        const qreal elevation = rawImage.value(QLatin1String("Elevation")).toDouble();
        if (std::abs(elevation) > 90) {
            qWarning("Image with index %d has invalid elevation value", i);
            return false;
        }

        if (!rawImage.value(QLatin1String("FileName")).isString()) {
            qWarning("Image with index %d has invalid filename", i);
            return false;
        }

        const QString fileName = rawImage.value(QLatin1String("FileName")).toString();
        const QUrl url = package.fileUrl(QByteArrayLiteral("images"), fileName);
        if (!url.isValid()) {
            qWarning("Couldn't locate image with index %d", i);
            return false;
        }

        WallpaperImage image = {};
        image.position = SunPosition(elevation, azimuth);
        image.url = url;

        images << image;
    }

    return true;
}

static bool parseTimedMetaData(const KPackage::Package& package, const QJsonObject& rootObject, QVector<WallpaperImage>& images)
{
    const QJsonArray metaData = rootObject.value(QLatin1String("MetaData")).toArray();
    if (metaData.isEmpty()) {
        qWarning() << "A dynamic wallpaper should contain at least one image";
        return false;
    }

    images.reserve(metaData.count());

    for (int i = 0; i < metaData.count(); ++i) {
        const QJsonObject rawImage = metaData.at(i).toObject();
        if (rawImage.isEmpty()) {
            qWarning("Image with index %d doesn't have any associated metadata", i);
            return false;
        }

        if (!rawImage.value(QLatin1String("Time")).isDouble()) {
            qWarning("Image with index %d has invalid time value type", i);
            return false;
        }

        const qreal time = rawImage.value(QLatin1String("Time")).toDouble();
        if (time < 0 || time > 1) {
            qWarning("Image with index %d has invalid time value", i);
            return false;
        }

        if (!rawImage.value(QLatin1String("FileName")).isString()) {
            qWarning("Image with index %d has invalid filename", i);
            return false;
        }

        const QString fileName = rawImage.value(QLatin1String("FileName")).toString();
        const QUrl url = package.fileUrl(QByteArrayLiteral("images"), fileName);
        if (!url.isValid()) {
            qWarning("Couldn't locate image with index %d", i);
            return false;
        }

        WallpaperImage image = {};
        image.time = time;
        image.url = url;

        images << image;
    }

    return true;
}

static bool parseWallpaperType(const QJsonObject& rootObject, WallpaperType& type)
{
    const QJsonValue value = rootObject.value(QStringLiteral("Type"));
    if (value == QStringLiteral("solar")) {
        type = WallpaperType::Solar;
        return true;
    }
    if (value == QStringLiteral("timed")) {
        type = WallpaperType::Timed;
        return true;
    }
    if (value.isUndefined()) {
        type = WallpaperType::Solar;
        return true;
    }
    qWarning() << "Unknown wallpaper type:" << value;
    return false;
}

std::unique_ptr<DynamicWallpaperPackage> DynamicWallpaperPackage::load(const QString& id)
{
    const KPackage::Package package = KPackage::PackageLoader::self()->loadPackage(QStringLiteral("Wallpaper/Dynamic"), id);
    if (!package.isValid())
        return nullptr;

    const QJsonObject rootObject = package.metadata().rawData().value(QLatin1String("Wallpaper")).toObject();
    if (rootObject.isEmpty())
        return nullptr;

    std::unique_ptr<DynamicWallpaperPackage> wallpaper = std::make_unique<DynamicWallpaperPackage>();
    if (!parseWallpaperType(rootObject, wallpaper->m_type))
        return nullptr;

    switch (wallpaper->type()) {
    case WallpaperType::Solar:
        if (!parseSolarMetaData(package, rootObject, wallpaper->m_images))
            return nullptr;
        break;
    case WallpaperType::Timed:
        if (!parseTimedMetaData(package, rootObject, wallpaper->m_images))
            return nullptr;
        break;
    default:
        Q_UNREACHABLE();
    }

    return wallpaper;
}
