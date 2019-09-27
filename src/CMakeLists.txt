find_package(KF5 REQUIRED COMPONENTS
    I18n
    Package
    Plasma
)

find_package(Qt5 CONFIG REQUIRED COMPONENTS
    Core
    Gui
    Qml
)

add_subdirectory(kpackage-integration)
add_subdirectory(plugin)
add_subdirectory(translations)

plasma_install_package(package com.github.zzag.wallpaper wallpapers wallpaper)
