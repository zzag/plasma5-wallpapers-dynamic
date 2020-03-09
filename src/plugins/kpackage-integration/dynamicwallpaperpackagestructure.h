/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
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
    void pathChanged(KPackage::Package *package) override;
};
