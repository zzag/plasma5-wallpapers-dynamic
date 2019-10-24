add_definitions(-DTRANSLATION_DOMAIN=\"plasma_wallpaper_com.github.zzag.wallpaper\")

set(wallpaperplugin_SRCS
    ClockSkewNotifier.cc
    ClockSkewNotifierEngine.cc
    DynamicWallpaper.cc
    DynamicWallpaperInstaller.cc
    DynamicWallpaperModel.cc
    DynamicWallpaperPackage.cc
    SunPath.cc
    SunPosition.cc
    WallpapersModel.cc
    plugin.cc
)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(wallpaperplugin_SRCS
        ClockSkewNotifierEngine_linux.cc
        ${wallpaperplugin_SRCS}
    )
endif()

add_library(wallpaperplugin SHARED ${wallpaperplugin_SRCS})

target_link_libraries(wallpaperplugin
    Qt5::Gui
    Qt5::Qml

    KF5::I18n
    KF5::Package
)

install(TARGETS wallpaperplugin DESTINATION ${KDE_INSTALL_QMLDIR}/com/github/zzag/private/wallpaper)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/com/github/zzag/private/wallpaper)
