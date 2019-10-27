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

#pragma once

// Qt
#include <QAbstractListModel>
#include <QUrl>
#include <QVector>

class WallpapersModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum AdditionalRoles {
        NameRole = Qt::UserRole + 1, ///< Human readable name of the wallpaper.
        IdRole, ///< The id of the wallpaper.
        PreviewUrlRole, ///< Path to the preview of the wallpaper. Optional.
        FolderRole, ///< Path to the root of the wallpaper.
        IsRemovableRole, ///< Whether the wallpaper can be removed.
        IsZombieRole, ///< Whether the wallpaper is about to be removed.
        AuthorRole, ///< The author of the wallpaper.
        LicenseRole, ///< License under which the wallpaper is distributed.
    };

    explicit WallpapersModel(QObject *parent = nullptr);

    // Reimplemented from QAbstractListModel.
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index = {}, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Q_INVOKABLE QStringList zombies() const;
    Q_INVOKABLE int indexOf(const QString &id) const;
    Q_INVOKABLE void reload();

private:
    struct Wallpaper
    {
        QString name;
        QString id;
        QString author;
        QString license;
        QUrl folderUrl;
        QUrl previewUrl;
        bool isRemovable = false;
        bool isZombie = false;
    };

    QVector<Wallpaper> m_wallpapers;

    Q_DISABLE_COPY(WallpapersModel)
};
