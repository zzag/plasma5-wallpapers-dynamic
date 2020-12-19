# SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

add_definitions(-DTRANSLATION_DOMAIN=\"plasma_wallpaper_com.github.zzag.dynamic\")

set(packagestructure_SOURCES
    dynamicwallpaperpackagestructure.cpp
)

add_library(packagestructure_dynamicwallpaper MODULE ${packagestructure_SOURCES})

target_link_libraries(packagestructure_dynamicwallpaper
    Qt5::Core

    KF5::I18n
    KF5::Package
)

install(TARGETS packagestructure_dynamicwallpaper DESTINATION ${KDE_INSTALL_PLUGINDIR}/kpackage/packagestructure)
