set(packagestructure_SRCS
    DynamicWallpaperPackageStructure.cc
)

add_library(packagestructure_dynamicwallpaper MODULE ${packagestructure_SRCS})

target_link_libraries(packagestructure_dynamicwallpaper
    Qt5::Core

    KF5::I18n
    KF5::Package
)

install(TARGETS packagestructure_dynamicwallpaper DESTINATION ${KDE_INSTALL_PLUGINDIR}/kpackage/packagestructure)
