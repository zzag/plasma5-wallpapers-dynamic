set(wallpaperplugin_SRCS
    ClockSkewWatcher.cc
    DynamicWallpaper.cc
    DynamicWallpaperModel.cc
    DynamicWallpaperPackage.cc
    PlatformClockSkewWatcher.cc
    SunPath.cc
    SunPosition.cc
    WallpapersModel.cc
    plugin.cc
)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(wallpaperplugin_SRCS
        ClockSkewWatcher_linux.cc
        ${wallpaperplugin_SRCS}
    )
endif()

add_library(wallpaperplugin SHARED ${wallpaperplugin_SRCS})

target_link_libraries(wallpaperplugin
    Qt5::Gui
    Qt5::Qml

    KF5::Package
)

install(TARGETS wallpaperplugin DESTINATION ${KDE_INSTALL_QMLDIR}/com/github/zzag/private/wallpaper)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/com/github/zzag/private/wallpaper)
