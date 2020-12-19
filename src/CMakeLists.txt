# SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

plasma_install_package(package com.github.zzag.dynamic wallpapers wallpaper)

add_subdirectory(declarative)
add_subdirectory(lib)
add_subdirectory(plugins)
add_subdirectory(tools)
add_subdirectory(translations)
