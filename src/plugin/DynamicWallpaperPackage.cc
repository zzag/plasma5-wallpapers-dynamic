/*
 * Copyright (C) 2019 Vlad Zahorodnii <vladzzag@gmail.com>
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
#include <KLocalizedString>
#include <KPackage/PackageLoader>

// Qt
#include <QJsonArray>
#include <QJsonObject>

WallpaperType DynamicWallpaperPackage::type() const
{
    return m_type;
}

bool DynamicWallpaperPackage::isSmooth() const
{
    return m_isSmooth;
}

QVector<WallpaperImage> DynamicWallpaperPackage::images() const
{
    return m_images;
}

static QJsonObject extractWallpaperObject(const KPackage::Package &package)
{
    const QJsonObject &rootObject = package.metadata().rawData();
    if (rootObject.isEmpty())
        return QJsonObject();

    return rootObject.value(QLatin1String("Wallpaper")).toObject();
}

static QJsonArray extractMetaData(const KPackage::Package &package)
{
    const QJsonObject &wallpaperObject = extractWallpaperObject(package);
    if (wallpaperObject.isEmpty())
        return QJsonArray();

    return wallpaperObject.value(QLatin1String("MetaData")).toArray();
}

static bool parseWallpaperType(const KPackage::Package &package, WallpaperType &type, QString &errorText)
{
    const QJsonObject wallpaperObject = extractWallpaperObject(package);
    if (wallpaperObject.isEmpty()) {
        errorText = i18n("Could not find 'Wallpaper' object");
        return false;
    }

    const QJsonValue value = wallpaperObject.value(QStringLiteral("Type"));
    if (value.isUndefined()) {
        // Type is not set explicitly, assume that's a solar dynamic wallpaper.
        type = WallpaperType::Solar;
        return true;
    }

    const static QHash<QString, WallpaperType> knownWallpaperTypes {
        { QStringLiteral("solar"), WallpaperType::Solar },
        { QStringLiteral("timed"), WallpaperType::Timed },
    };

    const auto typeIt = knownWallpaperTypes.constFind(value.toString());
    if (typeIt == knownWallpaperTypes.constEnd()) {
        errorText = i18n("Unknown wallpaper type: %1", value.toString());
        return false;
    }

    type = *typeIt;
    return true;
}

static bool parseSolarMetaData(const KPackage::Package &package, QVector<WallpaperImage> &images, QString &errorText)
{
    const QJsonArray metaData = extractMetaData(package);

    images.reserve(metaData.count());

    for (int i = 0; i < metaData.count(); ++i) {
        const QJsonObject rawImage = metaData.at(i).toObject();
        if (rawImage.isEmpty()) {
            errorText = i18n("Image %1 has no metadata associated with it", i);
            return false;
        }

        const QJsonValue rawAzimuth = rawImage.value(QLatin1String("Azimuth"));
        if (!rawAzimuth.isDouble()) {
            errorText = i18n("Image %1 has invalid azimuth value: %2", i, rawAzimuth.toString());
            return false;
        }
        const QJsonValue rawElevation = rawImage.value(QLatin1String("Elevation"));
        if (!rawElevation.isDouble()) {
            errorText = i18n("Image %1 has invalid elevation value: %2", i, rawElevation.toString());
            return false;
        }

        WallpaperImage image = {};
        image.position = SunPosition(rawElevation.toDouble(), rawAzimuth.toDouble());

        const QString fileName = rawImage.value(QLatin1String("FileName")).toString();
        const QUrl url = package.fileUrl(QByteArrayLiteral("images"), fileName);
        image.url = url;

        images << image;
    }

    return true;
}

static bool parseTimedMetaData(const KPackage::Package &package, QVector<WallpaperImage> &images, QString &errorText)
{
    const QJsonArray metaData = extractMetaData(package);

    images.reserve(metaData.count());

    for (int i = 0; i < metaData.count(); ++i) {
        const QJsonObject rawImage = metaData.at(i).toObject();
        if (rawImage.isEmpty()) {
            errorText = i18n("Image %1 has no metadata associated with it", i);
            return false;
        }

        const QJsonValue rawTime = rawImage.value(QLatin1String("Time"));
        if (!rawTime.isDouble()) {
            errorText = i18n("Image %1 has invalid time value: %2", i, rawTime.toString());
            return false;
        }
        const qreal time = rawTime.toDouble();
        if (time < 0 || time > 1) {
            errorText = i18n("Image %1 has invalid time value: %2", i, time);
            return false;
        }

        WallpaperImage image = {};
        image.time = time;

        const QString fileName = rawImage.value(QLatin1String("FileName")).toString();
        const QUrl url = package.fileUrl(QByteArrayLiteral("images"), fileName);
        image.url = url;

        images << image;
    }

    return true;
}

static void parseSmoothMode(const KPackage::Package &package, bool &smooth)
{
    const QJsonObject wallpaperObject = extractWallpaperObject(package);
    const QJsonValue smoothValue = wallpaperObject.value(QLatin1String("Smooth"));

    // If the Smooth key is not present, assume that it's implicitly set to true.
    smooth = smoothValue.isUndefined() || smoothValue.toBool();
}

bool DynamicWallpaperLoader::load(const QString &id)
{
    reset();

    const KPackage::Package package = KPackage::PackageLoader::self()->loadPackage(QStringLiteral("Wallpaper/Dynamic"), id);
    if (!package.isValid()) {
        m_errorText = i18n("Could not load wallpaper package: %1", id);
        return false;
    }

    auto wallpaper = std::make_shared<DynamicWallpaperPackage>();
    if (!parseWallpaperType(package, wallpaper->m_type, m_errorText))
        return false;

    parseSmoothMode(package, wallpaper->m_isSmooth);

    switch (wallpaper->type()) {
    case WallpaperType::Solar:
        if (!parseSolarMetaData(package, wallpaper->m_images, m_errorText))
            return false;
        break;
    case WallpaperType::Timed:
        if (!parseTimedMetaData(package, wallpaper->m_images, m_errorText))
            return false;
        break;
    default:
        Q_UNREACHABLE();
    }

    if (wallpaper->m_images.count() < 2) {
        m_errorText = i18n("Wallpaper does not have enough images");
        return false;
    }

    m_wallpaper = wallpaper;

    return true;
}

QString DynamicWallpaperLoader::errorText() const
{
    return m_errorText;
}

std::shared_ptr<DynamicWallpaperPackage> DynamicWallpaperLoader::wallpaper() const
{
    return m_wallpaper;
}

void DynamicWallpaperLoader::reset()
{
    m_errorText = QString();
    m_wallpaper = {};
}
