/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperpackagestructure.h"

#include <KLocalizedString>

#include <QFileInfo>

DynamicWallpaperPackageStructure::DynamicWallpaperPackageStructure(QObject *parent, const QVariantList &args)
    : KPackage::PackageStructure(parent, args)
{
}

void DynamicWallpaperPackageStructure::initPackage(KPackage::Package *package)
{
    package->addDirectoryDefinition(QByteArrayLiteral("images"),
                                    QStringLiteral("images/"),
                                    i18n("Dynamic wallpaper files"));
    package->setRequired(QByteArrayLiteral("images"), true);
}

void DynamicWallpaperPackageStructure::pathChanged(KPackage::Package *package)
{
    package->removeDefinition(QByteArrayLiteral("dynamic"));

    const QStringList fileFormats { QStringLiteral(".heic"), QStringLiteral(".heif") };

    for (const QString &fileFormat : fileFormats) {
        const QFileInfo fileInfo(package->path() + QLatin1String("contents/images/dynamic") + fileFormat);
        if (!fileInfo.exists())
            continue;
        package->addFileDefinition(QByteArrayLiteral("dynamic"),
                                   QStringLiteral("images/dynamic") + fileFormat,
                                   i18n("Dynamic wallpaper file"));
        package->setRequired(QByteArrayLiteral("dynamic"), true);
        break;
    }
}

K_EXPORT_KPACKAGE_PACKAGE_WITH_JSON(DynamicWallpaperPackageStructure, "dynamicwallpaperpackagestructure.json")

#include "dynamicwallpaperpackagestructure.moc"
