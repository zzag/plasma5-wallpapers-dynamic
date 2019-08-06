set(wallpaperplugin_SRCS
    DynamicWallpaper.cc
    DynamicWallpaperModel.cc
    DynamicWallpaperPackage.cc
    SunPath.cc
    SunPosition.cc
    WallpapersModel.cc
    plugin.cc
)

add_library(wallpaperplugin SHARED ${wallpaperplugin_SRCS})

target_link_libraries(wallpaperplugin
    Qt5::Gui
    Qt5::Qml
)

install(TARGETS wallpaperplugin DESTINATION ${KDE_INSTALL_QMLDIR}/com/github/zzag/private/wallpaper)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/com/github/zzag/private/wallpaper)
