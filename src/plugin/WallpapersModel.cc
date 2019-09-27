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
#include "WallpapersModel.h"

// KF
#include <KPackage/PackageLoader>

// Qt
#include <QJsonObject>

WallpapersModel::WallpapersModel(QObject* parent)
    : QAbstractListModel(parent)
{
    load();
}

QHash<int, QByteArray> WallpapersModel::roleNames() const
{
    const QHash<int, QByteArray> roles {
        { Name, QByteArrayLiteral("name") },
        { Id, QByteArrayLiteral("id") },
        { PreviewUrl, QByteArrayLiteral("previewUrl") }
    };
    return roles;
}

int WallpapersModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_wallpapers.count();
}

QVariant WallpapersModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (m_wallpapers.count() <= index.row())
        return QVariant();

    const Wallpaper& wallpaper = m_wallpapers.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
    case Name:
        return wallpaper.name;
    case Id:
        return wallpaper.id;
    case PreviewUrl:
        return wallpaper.previewUrl;
    default:
        return QVariant();
    }
}

int WallpapersModel::indexOf(const QString& id) const
{
    auto it = std::find_if(m_wallpapers.begin(), m_wallpapers.end(),
        [id](const Wallpaper& wallpaper) {
            return wallpaper.id == id;
        });
    if (it == m_wallpapers.end())
        return -1;

    return std::distance(m_wallpapers.begin(), it);
}

static void forEachPackage(const QString& packageFormat, std::function<void(const KPackage::Package&)> callback)
{
    const QList<KPluginMetaData> packages = KPackage::PackageLoader::self()->listPackages(packageFormat);
    for (const KPluginMetaData& metaData : packages) {
        const QString pluginId = metaData.pluginId();
        const KPackage::Package package = KPackage::PackageLoader::self()->loadPackage(packageFormat, pluginId);
        if (package.isValid())
            callback(package);
    }
}

static QString previewFromMetaData(const KPluginMetaData& metaData)
{
    const QJsonObject wallpaperObject = metaData.rawData().value(QLatin1String("Wallpaper")).toObject();
    if (wallpaperObject.isEmpty())
        return QString();

    return wallpaperObject.value(QLatin1String("Preview")).toString();
}

void WallpapersModel::load()
{
    QVector<Wallpaper> wallpapers;

    forEachPackage(QStringLiteral("Wallpaper/Dynamic"), [&](const KPackage::Package& package) {
        Wallpaper wallpaper = {};
        wallpaper.id = package.metadata().pluginId();
        wallpaper.name = package.metadata().name();

        const QString previewFileName = previewFromMetaData(package.metadata());
        if (!previewFileName.isEmpty())
            wallpaper.previewUrl = package.fileUrl(QByteArrayLiteral("images"), previewFileName);

        wallpapers << wallpaper;
    });

    beginResetModel();
    m_wallpapers = wallpapers;
    endResetModel();
}
