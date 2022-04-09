/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpapercrawler.h"

#include <KSolarDynamicWallpaperMetaData>
#include <KDynamicWallpaperReader>

#include <QDir>

/*!
 * \class DynamicWallpaperCrawler
 * \brief The DynamicWallpaperCrawler class discovers dynamic wallpapers.
 */

/*!
 * Constructs an DynamicWallpaperCrawler object with the given \p parent.
 */
DynamicWallpaperCrawler::DynamicWallpaperCrawler(QObject *parent)
    : QThread(parent)
    , m_token(QUuid::createUuid())
{
}

/*!
 * Destructs the DynamicWallpaperCrawler object.
 */
DynamicWallpaperCrawler::~DynamicWallpaperCrawler()
{
    wait();
}

/*!
 * Returns the UUID that uniquely identifies this crawler.
 */
QUuid DynamicWallpaperCrawler::token() const
{
    return m_token;
}

/*!
 * Sets the list of directories where dynamic wallpapers should be searched to \p roots.
 *
 * Dynamic wallpapers will be searched recursively.
 */
void DynamicWallpaperCrawler::setSearchRoots(const QStringList &roots)
{
    m_searchRoots = roots;
}

/*!
 * Returns the list of directories where dynamic wallpapers should be searched recursively.
 */
QStringList DynamicWallpaperCrawler::searchRoots() const
{
    return m_searchRoots;
}

/*!
 * Sets the package structure for dynamic wallpaper packages to \p structure.
 */
void DynamicWallpaperCrawler::setPackageStructure(KPackage::PackageStructure *structure)
{
    m_packageStructure = structure;
}

/*!
 * Returns the package structure for dynamic wallpaper packages.
 */
KPackage::PackageStructure *DynamicWallpaperCrawler::packageStructure() const
{
    return m_packageStructure;
}

void DynamicWallpaperCrawler::run()
{
    for (const QString &candidate : qAsConst(m_searchRoots))
        visitFolder(candidate);

    deleteLater();
}

void DynamicWallpaperCrawler::visitFolder(const QString &filePath)
{
    QDir currentFolder(filePath);
    currentFolder.setFilter(QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Readable | QDir::AllDirs | QDir::Files);
    currentFolder.setNameFilters({ QStringLiteral("*.avif") });

    const QFileInfoList fileInfos = currentFolder.entryInfoList();
    for (const QFileInfo &fileInfo : fileInfos) {
        if (fileInfo.isDir()) {
            if (checkPackage(fileInfo.filePath())) {
                emit foundPackage(fileInfo.filePath(), token());
            } else {
                visitFolder(fileInfo.filePath());
            }
        } else {
            visitFile(fileInfo.filePath());
        }
    }
}

void DynamicWallpaperCrawler::visitFile(const QString &filePath)
{
    // Not every avif file is a dynamic wallpaper, we need to read the file contents to
    // determine whether filePath actually points to a dynamic wallpaper file.
    KDynamicWallpaperReader reader(filePath);
    if (reader.error() == KDynamicWallpaperReader::NoError)
        emit foundFile(filePath, token());
}

bool DynamicWallpaperCrawler::checkPackage(const QString &filePath) const
{
    if (!QFile::exists(filePath + QLatin1String("/metadata.desktop")) &&
            !QFile::exists(filePath + QLatin1String("/metadata.json")))
        return false;

    KPackage::Package package(packageStructure());
    package.setPath(filePath);

    const QUrl imageUrl = package.fileUrl(QByteArrayLiteral("dynamic"));
    return imageUrl.isValid();
}
