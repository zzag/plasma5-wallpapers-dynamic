add_definitions(-DTRANSLATION_DOMAIN=\"plasma_wallpaper_com.github.zzag.wallpaper\")

set(wallpaperplugin_SRCS
    clockskewnotifier.cpp
    clockskewnotifierengine.cpp
    dynamicwallpaper.cpp
    dynamicwallpaperextensionplugin.cpp
    dynamicwallpaperinstaller.cpp
    dynamicwallpapermodel.cpp
    dynamicwallpaperpackage.cpp
    sunpath.cpp
    sunposition.cpp
    wallpapersmodel.cpp
)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(wallpaperplugin_SRCS
        clockskewnotifierengine_linux.cpp
        ${wallpaperplugin_SRCS}
    )
endif()

add_library(wallpaperplugin SHARED ${wallpaperplugin_SRCS})

target_compile_options(wallpaperplugin PRIVATE -fno-rtti)

target_link_libraries(wallpaperplugin
    Qt5::Gui
    Qt5::Positioning
    Qt5::Qml

    KF5::I18n
    KF5::Package
)

install(TARGETS wallpaperplugin DESTINATION ${KDE_INSTALL_QMLDIR}/com/github/zzag/private/wallpaper)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/com/github/zzag/private/wallpaper)
