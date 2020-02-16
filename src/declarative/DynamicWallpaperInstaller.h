/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
