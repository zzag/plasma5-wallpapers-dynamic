/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QAbstractListModel>
#include <QUrl>

class DynamicWallpaperModelPrivate;

class DynamicWallpaperModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        WallpaperNameRole = Qt::UserRole + 1,
        WallpaperFolderRole = Qt::UserRole + 2,
        WallpaperLicenseRole = Qt::UserRole + 3,
        WallpaperAuthorRole = Qt::UserRole + 4,
        WallpaperIsPackageRole = Qt::UserRole + 5,
        WallpaperIsCustomRole = Qt::UserRole + 6,
        WallpaperIsRemovableRole = Qt::UserRole + 7,
        WallpaperIsZombieRole = Qt::UserRole + 8,
        WallpaperImageRole = Qt::UserRole + 9,
        WallpaperPreviewRole = Qt::UserRole + 10,
    };

    explicit DynamicWallpaperModel(QObject *parent = nullptr);
    ~DynamicWallpaperModel() override;

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE int find(const QUrl &url) const;
    Q_INVOKABLE QModelIndex modelIndex(int index) const;

public Q_SLOTS:
    void reload();
    void purge();

    void add(const QUrl &fileUrl);
    void scheduleRemove(const QModelIndex &index);
    void unscheduleRemove(const QModelIndex &index);
    void remove(const QModelIndex &index);

private:
    friend class DynamicWallpaperModelPrivate;
    QScopedPointer<DynamicWallpaperModelPrivate> d;
};
