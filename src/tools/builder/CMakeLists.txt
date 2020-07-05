add_definitions(-DTRANSLATION_DOMAIN=\"plasma_wallpaper_com.github.zzag.dynamic\")

add_subdirectory(completions)

set(builder_SOURCES
    dynamicwallpaperdescriptionreader.cpp
    dynamicwallpaperexifmetadata.cpp
    main.cpp
)

set(builder_LIBRARIES
    libexif::libexif
    KDynamicWallpaper::KDynamicWallpaper
)

add_executable(kdynamicwallpaperbuilder ${builder_SOURCES})
target_link_libraries(kdynamicwallpaperbuilder ${builder_LIBRARIES})

install(TARGETS kdynamicwallpaperbuilder ${INSTALL_TARGETS_DEFAULT_ARGS})
