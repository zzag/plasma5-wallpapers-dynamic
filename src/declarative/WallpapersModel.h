/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

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
