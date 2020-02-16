/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <KPackage/PackageStructure>

class DynamicWallpaperPackageStructure final : public KPackage::PackageStructure
{
    Q_OBJECT

public:
    explicit DynamicWallpaperPackageStructure(QObject *parent = nullptr, const QVariantList &args = QVariantList());

    void initPackage(KPackage::Package *package) override;
};
