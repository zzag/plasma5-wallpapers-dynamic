find_package(KF5 REQUIRED COMPONENTS
    Plasma
)

find_package(Qt5 CONFIG REQUIRED COMPONENTS
    Core
    Gui
    Qml
)

add_subdirectory(plugin)

plasma_install_package(package com.github.zzag.wallpaper wallpapers wallpaper)
