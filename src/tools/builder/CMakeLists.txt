# SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

add_definitions(-DTRANSLATION_DOMAIN=\"plasma_wallpaper_com.github.zzag.dynamic\")

add_subdirectory(completions)

set(builder_SOURCES
    dynamicwallpaperdescription.cpp
    dynamicwallpaperexifmetadata.cpp
    main.cpp
)

set(builder_LIBRARIES
    KF5::I18n
    libexif::libexif
    KDynamicWallpaper::KDynamicWallpaper
)

add_executable(kdynamicwallpaperbuilder ${builder_SOURCES})
target_link_libraries(kdynamicwallpaperbuilder ${builder_LIBRARIES})

install(TARGETS kdynamicwallpaperbuilder ${INSTALL_TARGETS_DEFAULT_ARGS})
