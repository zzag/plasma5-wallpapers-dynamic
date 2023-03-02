# SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

add_definitions(-DTRANSLATION_DOMAIN=\"plasma_wallpaper_com.github.zzag.dynamic\")

kcoreaddons_add_plugin(packagestructure_dynamicwallpaper INSTALL_NAMESPACE "kpackage/packagestructure")

set_target_properties(packagestructure_dynamicwallpaper PROPERTIES
    OUTPUT_NAME kdynamicwallpaper
)

target_sources(packagestructure_dynamicwallpaper PRIVATE
    dynamicwallpaperpackagestructure.cpp
)

target_link_libraries(packagestructure_dynamicwallpaper
    Qt6::Core

    KF6::I18n
    KF6::Package
)
