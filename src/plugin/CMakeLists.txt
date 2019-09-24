set(wallpaperplugin_SRCS
    DateTimeWatcher.cc
    DynamicWallpaper.cc
    DynamicWallpaperModel.cc
    DynamicWallpaperPackage.cc
    PlatformDateTimeWatcher.cc
    SunPath.cc
    SunPosition.cc
    WallpapersModel.cc
    plugin.cc
)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(wallpaperplugin_SRCS
        DateTimeWatcher_linux.cc
        ${wallpaperplugin_SRCS}
    )
endif()

add_library(wallpaperplugin SHARED ${wallpaperplugin_SRCS})

target_link_libraries(wallpaperplugin
    Qt5::Gui
    Qt5::Qml
)

install(TARGETS wallpaperplugin DESTINATION ${KDE_INSTALL_QMLDIR}/com/github/zzag/private/wallpaper)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/com/github/zzag/private/wallpaper)
