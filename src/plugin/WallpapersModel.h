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

class WallpapersModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum AdditionalRoles {
        /**
         * Human readable name of the wallpaper.
         */
        Name = Qt::UserRole + 1,
        /**
         * The id of the wallpaper.
         */
        Id,
        /**
         * Path to the preview of the wallpaper. Optional.
         */
        PreviewUrl
    };

    explicit WallpapersModel(QObject* parent = nullptr);

    // Reimplemented from QAbstractListModel.
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index = {}, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE int indexOf(const QString& id) const;

private:
    void load();

    struct Wallpaper {
        QString name;
        QString id;
        QUrl previewUrl;
    };

    QVector<Wallpaper> m_wallpapers;

    Q_DISABLE_COPY(WallpapersModel)
};
