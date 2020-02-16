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
#include "DynamicWallpaperInstaller.h"

// KF
#include <KPackage/Package>
#include <KPackage/PackageLoader>

// Qt
#include <QDir>
#include <QStandardPaths>

DynamicWallpaperInstaller::DynamicWallpaperInstaller(QObject *parent)
    : QObject(parent)
{
}

QString DynamicWallpaperInstaller::error() const
{
    return m_error;
}

void DynamicWallpaperInstaller::setError(const QString &error)
{
    m_error = error;
    emit errorChanged();
}

QString DynamicWallpaperInstaller::locatePackageRoot()
{
    const QString dataFolder = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    return dataFolder + QLatin1String("/dynamicwallpapers");
}

static KPackage::PackageStructure *locatePackageStructure()
{
    return KPackage::PackageLoader::self()->loadPackageStructure(QStringLiteral("Wallpaper/Dynamic"));
}

void DynamicWallpaperInstaller::install(const QUrl &fileUrl)
{
    KPackage::PackageStructure *structure = locatePackageStructure();
    if (!structure)
        return;

    const QString metaDataFilePath = fileUrl.toLocalFile();
    const QString sourcePackage = QFileInfo(metaDataFilePath).path();

    KPackage::Package package(structure);
    KJob *job = package.install(sourcePackage, locatePackageRoot());

    connect(job, &KJob::finished, this, &DynamicWallpaperInstaller::slotPackageInstalled);
}

void DynamicWallpaperInstaller::uninstall(const QString &packageName)
{
    KPackage::PackageStructure *structure = locatePackageStructure();
    if (!structure)
        return;

    KPackage::Package package(structure);
    KJob *job = package.uninstall(packageName, locatePackageRoot());

    connect(job, &KJob::finished, this, &DynamicWallpaperInstaller::slotPackageUninstalled);
}

void DynamicWallpaperInstaller::uninstall(const QStringList &packageNames)
{
    for (const QString &packageName : packageNames)
        uninstall(packageName);
}

void DynamicWallpaperInstaller::slotPackageInstalled(KJob *job)
{
    if (job->error() != KJob::NoError)
        setError(job->errorText());
    else
        emit installed();
}

void DynamicWallpaperInstaller::slotPackageUninstalled(KJob *job)
{
    if (job->error() != KJob::NoError)
        setError(job->errorText());
    else
        emit uninstalled();
}
