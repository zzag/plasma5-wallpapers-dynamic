# kdynamicwallpaperbuilder(1) completion

# SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: CC0-1.0

complete -f -c kdynamicwallpaperbuilder
complete -c kdynamicwallpaperbuilder -s v -l version -d "Print the version information and quit"
complete -c kdynamicwallpaperbuilder -s h -l help -d "Show help message and quit"
complete -c kdynamicwallpaperbuilder -l help-all -d "Show help message including Qt specific options and quit"

complete -c kdynamicwallpaperbuilder -l output -d "Specify the file where the output will be written" -r
complete -c kdynamicwallpaperbuilder -l max-threads -d "Maximum number of threads that can be used when encoding a wallpaper" -r
