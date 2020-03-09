add_definitions(-DTRANSLATION_DOMAIN=\"plasma_wallpaper_com.github.zzag.dynamic\")

add_subdirectory(completions)

set(builder_SOURCES
    dynamicwallpaperdescriptionreader.cpp
    main.cpp
)

add_executable(kdynamicwallpaperbuilder ${builder_SOURCES})
target_link_libraries(kdynamicwallpaperbuilder KDynamicWallpaper::KDynamicWallpaper)

install(TARGETS kdynamicwallpaperbuilder ${INSTALL_TARGETS_DEFAULT_ARGS})
