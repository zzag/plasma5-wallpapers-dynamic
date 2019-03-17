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
#include "WallpapersModel.h"

// Qt
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

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

void WallpapersModel::load()
{
    const QStringList dirs = QStandardPaths::locateAll(
        QStandardPaths::GenericDataLocation,
        QStringLiteral("dynamicwallpapers/"),
        QStandardPaths::LocateDirectory);
    if (dirs.isEmpty())
        return;

    QHash<QString, Wallpaper> wallpapers;
    for (const QString& root : dirs) {
        const QDir dir(root);
        const QStringList ids = dir.entryList(QDir::Dirs);

        for (const QString& id : ids) {
            const QStringList parts { root, id, QStringLiteral("metadata.json") };
            QFile file(parts.join('/'));
            if (!file.open(QIODevice::ReadOnly))
                continue;

            const QJsonDocument metadata = QJsonDocument::fromJson(file.readAll());
            if (metadata.isNull())
                continue;

            if (!metadata.isObject())
                continue;

            const QJsonObject rootObject = metadata.object();
            if (!rootObject.contains(QLatin1String("name")))
                continue;

            Wallpaper wallpaper;

            wallpaper.name = rootObject.value(QLatin1String("name")).toString();
            wallpaper.id = id;

            const QString previewFileName = rootObject.value(QLatin1String("preview")).toString();
            if (!previewFileName.isEmpty()) {
                const QStringList parts { root, id, QStringLiteral("images"), previewFileName };
                wallpaper.previewUrl = QUrl::fromLocalFile(parts.join('/'));
            }

            wallpapers.insert(id, wallpaper);
        }
    }

    beginResetModel();
    m_wallpapers = wallpapers.values().toVector();
    endResetModel();
}