#! /usr/bin/env bash

# SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: CC0-1.0

$XGETTEXT `find . -name \*.qml -o -name \*.cpp` -o $podir/plasma_wallpaper_com.github.zzag.dynamic.pot
