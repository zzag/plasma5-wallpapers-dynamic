#.rst:
# Findlibexif
# -------
#
# Try to find libexif on a Unix system.
#
# This will define the following variables:
#
# ``libexif_FOUND``
#     True if (the requested version of) libexif is available
# ``libexif_VERSION``
#     The version of libexif
# ``libexif_LIBRARIES``
#     This should be passed to target_compile_options() if the target is not
#     used for linking
# ``libexif_INCLUDE_DIRS``
#     This should be passed to target_include_directories() if the target is not
#     used for linking
# ``libexif_DEFINITIONS``
#     This should be passed to target_compile_options() if the target is not
#     used for linking
#
# If ``libexif_FOUND`` is TRUE, it will also define the following imported target:
#
# ``libexif::libexif``
#     The libexif library
#
# In general we recommend using the imported target, as it is easier to use.
# Bear in mind, however, that if the target is in the link interface of an
# exported library, it must be made available by the package config file.

# SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

find_package(PkgConfig)
pkg_check_modules(PKG_libexif QUIET libexif)

set(libexif_VERSION ${PKG_libexif_VERSION})
set(libexif_DEFINITIONS ${PKG_libexif_CFLAGS_OTHER})

find_path(libexif_INCLUDE_DIR
    NAMES libexif/exif-data.h
    HINTS ${PKG_libexif_INCLUDE_DIRS}
)

find_library(libexif_LIBRARY
    NAMES exif
    HINTS ${PKG_libexif_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libexif
    FOUND_VAR libexif_FOUND
    REQUIRED_VARS libexif_LIBRARY
                  libexif_INCLUDE_DIR
    VERSION_VAR libexif_VERSION
)

if (libexif_FOUND AND NOT TARGET libexif::libexif)
    add_library(libexif::libexif UNKNOWN IMPORTED)
    set_target_properties(libexif::libexif PROPERTIES
        IMPORTED_LOCATION "${libexif_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${libexif_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${libexif_INCLUDE_DIR}"
    )
endif()

set(libexif_INCLUDE_DIRS ${libexif_INCLUDE_DIR})
set(libexif_LIBRARIES ${libexif_LIBRARY})

mark_as_advanced(libexif_INCLUDE_DIR)
mark_as_advanced(libexif_LIBRARY)
