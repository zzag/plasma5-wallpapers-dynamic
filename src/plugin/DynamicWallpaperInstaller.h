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

// KF
#include <KJob>

// Qt
#include <QObject>

class DynamicWallpaperInstaller : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString error READ error WRITE setError NOTIFY errorChanged)

public:
    explicit DynamicWallpaperInstaller(QObject *parent = nullptr);

    QString error() const;
    void setError(const QString &error);

    static QString locatePackageRoot();

public slots:
    void install(const QUrl &fileUrl);
    void uninstall(const QString &packageName);
    void uninstall(const QStringList &packageNames);

signals:
    void installed();
    void uninstalled();
    void errorChanged();

private slots:
    void slotPackageInstalled(KJob *job);
    void slotPackageUninstalled(KJob *job);

private:
    QString m_error;

    Q_DISABLE_COPY(DynamicWallpaperInstaller)
};
