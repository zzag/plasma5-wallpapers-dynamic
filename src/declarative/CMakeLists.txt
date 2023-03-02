# SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

add_definitions(-DTRANSLATION_DOMAIN=\"plasma_wallpaper_com.github.zzag.dynamic\")

set(FALLBACK_WALLPAPER "Dynamic")
configure_file(config-dynamicwallpaper.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-dynamicwallpaper.h)

set(dynamicwallpaperplugin_SOURCES
    dynamicwallpapercrawler.cpp
    dynamicwallpaperengine.cpp
    dynamicwallpaperengine_daynight.cpp
    dynamicwallpaperengine_solar.cpp
    dynamicwallpaperextensionplugin.cpp
    dynamicwallpaperhandler.cpp
    dynamicwallpaperimagehandle.cpp
    dynamicwallpaperimageprovider.cpp
    dynamicwallpapermodel.cpp
    dynamicwallpaperpreviewcache.cpp
    dynamicwallpaperpreviewjob.cpp
    dynamicwallpaperpreviewprovider.cpp
    dynamicwallpaperprober.cpp
)

add_library(plasma_wallpaper_dynamicplugin ${dynamicwallpaperplugin_SOURCES})

target_link_libraries(plasma_wallpaper_dynamicplugin
    Qt6::Concurrent
    Qt6::Core
    Qt6::Gui
    Qt6::Positioning
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickPrivate

    KF6::ConfigCore
    KF6::I18n
    KF6::Package

    KDynamicWallpaper::KDynamicWallpaper
)

install(TARGETS plasma_wallpaper_dynamicplugin DESTINATION ${KDE_INSTALL_QMLDIR}/com/github/zzag/plasma/wallpapers/dynamic)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/com/github/zzag/plasma/wallpapers/dynamic)
