#.rst:
# Findlibheif
# -------
#
# Try to find libheif on a Unix system.
#
# This will define the following variables:
#
# ``libheif_FOUND``
#     True if (the requested version of) libheif is available
# ``libheif_VERSION``
#     The version of libheif
# ``libheif_LIBRARIES``
#     This should be passed to target_compile_options() if the target is not
#     used for linking
# ``libheif_INCLUDE_DIRS``
#     This should be passed to target_include_directories() if the target is not
#     used for linking
# ``libheif_DEFINITIONS``
#     This should be passed to target_compile_options() if the target is not
#     used for linking
#
# If ``libheif_FOUND`` is TRUE, it will also define the following imported target:
#
# ``libheif::libheif``
#     The libheif library
#
# In general we recommend using the imported target, as it is easier to use.
# Bear in mind, however, that if the target is in the link interface of an
# exported library, it must be made available by the package config file.

# Copyright (C) 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the University nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

find_package(PkgConfig)
pkg_check_modules(PKG_libheif QUIET libheif)

set(libheif_VERSION ${PKG_libheif_VERSION})
set(libheif_DEFINITIONS ${PKG_libheif_CFLAGS_OTHER})

find_path(libheif_INCLUDE_DIR
    NAMES libheif/heif.h
    HINTS ${PKG_libheif_INCLUDE_DIRS}
)

find_library(libheif_LIBRARY
    NAMES heif
    HINTS ${PKG_libheif_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libheif
    FOUND_VAR libheif_FOUND
    REQUIRED_VARS libheif_LIBRARY
                  libheif_INCLUDE_DIR
    VERSION_VAR libheif_VERSION
)

if (libheif_FOUND AND NOT TARGET libheif::libheif)
    add_library(libheif::libheif UNKNOWN IMPORTED)
    set_target_properties(libheif::libheif PROPERTIES
        IMPORTED_LOCATION "${libheif_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${libheif_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${libheif_INCLUDE_DIR}"
    )
endif()

set(libheif_INCLUDE_DIRS ${libheif_INCLUDE_DIR})
set(libheif_LIBRARIES ${libheif_LIBRARY})

mark_as_advanced(libheif_INCLUDE_DIR)
mark_as_advanced(libheif_LIBRARY)
