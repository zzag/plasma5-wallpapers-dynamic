# SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

add_definitions(-DTRANSLATION_DOMAIN=\"plasma_wallpaper_com.github.zzag.dynamic\")

set(dynamicwallpaperlib_SOURCES
    kdynamicwallpaperreader.cpp
    kdynamicwallpaperwriter.cpp
    ksolardynamicwallpapermetadata.cpp
    ksunpath.cpp
    ksunposition.cpp
    ksystemclockmonitor.cpp
    ksystemclockmonitorengine.cpp
)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(dynamicwallpaperlib_SOURCES
        ksystemclockmonitorengine_linux.cpp
        ${dynamicwallpaperlib_SOURCES}
    )
endif()

ecm_generate_headers(dynamicwallpaperlib_HEADERS
    HEADER_NAMES
        KDynamicWallpaperReader
        KDynamicWallpaperWriter
        KSolarDynamicWallpaperMetaData
        KSunPath
        KSunPosition
        KSystemClockMonitor
    REQUIRED_HEADERS dynamicwallpaperlib_HEADERS
)

qt5_add_resources(dynamicwallpaperlib_SOURCES resources.qrc)

add_library(kdynamicwallpaper ${dynamicwallpaperlib_SOURCES})
add_library(KDynamicWallpaper::KDynamicWallpaper ALIAS kdynamicwallpaper)

generate_export_header(kdynamicwallpaper
    BASE_NAME KDynamicWallpaper
    EXPORT_MACRO_NAME KDYNAMICWALLPAPER_EXPORT
)

set_target_properties(kdynamicwallpaper PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME KDynamicWallpaper
)

target_include_directories(kdynamicwallpaper PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KDynamicWallpaper>"
)

target_link_libraries(kdynamicwallpaper
    PUBLIC
        Qt5::Core
        Qt5::Gui
        Qt5::Positioning

    PRIVATE
        Qt5::Xml
        KF5::I18n
        avif
)

set(CMAKECONFIG_INSTALL_DIR ${KDE_INSTALL_LIBDIR}/cmake/KDynamicWallpaper)

write_basic_config_version_file(${CMAKE_CURRENT_BINARY_DIR}/KDynamicWallpaperConfigVersion.cmake
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY AnyNewerVersion)

configure_package_config_file(KDynamicWallpaperConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/KDynamicWallpaperConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR})

install(TARGETS kdynamicwallpaper EXPORT KDynamicWallpaperTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/KDynamicWallpaperConfig.cmake
    DESTINATION ${CMAKECONFIG_INSTALL_DIR}
    COMPONENT Devel)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/KDynamicWallpaperConfigVersion.cmake
    DESTINATION ${CMAKECONFIG_INSTALL_DIR}
    COMPONENT Devel)

install(EXPORT KDynamicWallpaperTargets
    NAMESPACE KDynamicWallpaper::
    DESTINATION ${CMAKECONFIG_INSTALL_DIR}
    FILE KDynamicWallpaperTargets.cmake
    COMPONENT Devel)

install(FILES ${dynamicwallpaperlib_HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/kdynamicwallpaper_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KDynamicWallpaper
    COMPONENT Devel)
