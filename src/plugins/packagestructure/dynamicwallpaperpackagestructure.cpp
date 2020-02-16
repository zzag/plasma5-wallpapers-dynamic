/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperpackagestructure.h"

#include <KLocalizedString>

DynamicWallpaperPackageStructure::DynamicWallpaperPackageStructure(QObject *parent, const QVariantList &args)
    : KPackage::PackageStructure(parent, args)
{
}

void DynamicWallpaperPackageStructure::initPackage(KPackage::Package *package)
{
    const QStringList supportedImageFormats {
        QStringLiteral("image/jpeg"),
        QStringLiteral("image/png"),
    };

    package->setDefaultPackageRoot(QStringLiteral("dynamicwallpapers/"));
    package->addDirectoryDefinition(QByteArrayLiteral("images"), QStringLiteral("images"), i18n("Images"));
    package->setMimeTypes(QByteArrayLiteral("images"), supportedImageFormats);
    package->setRequired(QByteArrayLiteral("images"), true);
}

K_EXPORT_KPACKAGE_PACKAGE_WITH_JSON(DynamicWallpaperPackageStructure, "dynamicwallpaperpackagestructure.json")

#include "dynamicwallpaperpackagestructure.moc"
