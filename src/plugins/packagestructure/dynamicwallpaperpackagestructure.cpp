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
