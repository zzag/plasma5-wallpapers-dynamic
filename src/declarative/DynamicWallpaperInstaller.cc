/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
