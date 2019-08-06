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

// Qt
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

QString DynamicWallpaperPackage::name() const
{
    return m_name;
}

QVector<Image> DynamicWallpaperPackage::images() const
{
    return m_images;
}

struct Context {
    QString packagePath;
};

static QString imagePath(const Context& context, const QString& fileName)
{
    return context.packagePath + QLatin1String("/images/") + fileName;
}

static bool validateImage(const Context& context, const QJsonObject& object)
{
    if (object.isEmpty())
        return false;

    if (!object.contains(QLatin1String("elevation")))
        return false;

    if (!object.value(QLatin1String("elevation")).isDouble())
        return false;

    if (!object.contains(QLatin1String("azimuth")))
        return false;

    if (!object.value(QLatin1String("azimuth")).isDouble())
        return false;

    if (!object.contains(QLatin1String("filename")))
        return false;

    if (!object.value(QLatin1String("filename")).isString())
        return false;

    const QString fileName = object.value(QLatin1String("filename")).toString();
    if (!QFileInfo(imagePath(context, fileName)).exists())
        return false;

    return true;
}

static bool validateImages(const Context& context, const QJsonArray& array)
{
    if (array.isEmpty())
        return false;

    for (const QJsonValue& value : array) {
        if (!value.isObject())
            return false;

        const QJsonObject& image = value.toObject();
        if (!validateImage(context, image))
            return false;
    }

    return true;
}

static bool validateMetadata(const Context& context, const QJsonObject& object)
{
    if (object.isEmpty())
        return false;

    if (!object.contains(QLatin1String("name")))
        return false;

    if (!object.value(QLatin1String("name")).isString())
        return false;

    if (!object.contains(QLatin1String("images")))
        return false;

    if (!object.value(QLatin1String("images")).isArray())
        return false;

    const QJsonArray images = object.value(QLatin1String("images")).toArray();
    if (!validateImages(context, images))
        return false;

    return true;
}

static bool validate(const Context& context, const QJsonDocument& document)
{
    if (document.isEmpty())
        return false;

    if (!document.isObject())
        return false;

    const QJsonObject& object = document.object();
    if (!validateMetadata(context, object))
        return false;

    return true;
}

static Image parseImage(const Context& context, const QJsonValue& value)
{
    const QJsonObject& object = value.toObject();

    const qreal elevation = object.value(QLatin1String("elevation")).toDouble();
    const qreal azimuth = object.value(QLatin1String("azimuth")).toDouble();
    const SunPosition position(elevation, azimuth);

    const QString fileName = object.value(QLatin1String("filename")).toString();
    const QUrl url = QUrl::fromLocalFile(imagePath(context, fileName));

    return { position, url };
}

static QVector<Image> parseImages(const Context& context, const QJsonValue& value)
{
    const QJsonArray& array = value.toArray();

    QVector<Image> images;
    images.reserve(array.count());

    for (const QJsonValue& image : array)
        images << parseImage(context, image);

    return images;
}

std::unique_ptr<DynamicWallpaperPackage> DynamicWallpaperPackage::load(const QString& id)
{
    const QString fileName = QStandardPaths::locate(
        QStandardPaths::GenericDataLocation,
        QStringLiteral("dynamicwallpapers/%1/metadata.json").arg(id));
    if (fileName.isEmpty())
        return nullptr;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return nullptr;

    const QJsonDocument metadata = QJsonDocument::fromJson(file.readAll());
    if (metadata.isNull())
        return nullptr;

    Context context;
    context.packagePath = QFileInfo(fileName).absolutePath();

    if (!validate(context, metadata))
        return nullptr;

    const QJsonObject& rootObject = metadata.object();

    std::unique_ptr<DynamicWallpaperPackage> wallpaper = std::make_unique<DynamicWallpaperPackage>();
    wallpaper->m_images = parseImages(context, rootObject.value(QLatin1String("images")));
    wallpaper->m_name = rootObject.value(QLatin1String("name")).toString();

    return wallpaper;
}