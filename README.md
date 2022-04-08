<!--
SPDX-FileCopyrightText: none

SPDX-License-Identifier: CC0-1.0
-->

# Dynamic Wallpaper Engine

**NOTE**: The master branch contains the next major version of this plugin, which doesn't
use the HEIF file format and it's still in development. If you want to build this plugin
from source code, checkout 3.3.9 tag!

A wallpaper plugin for KDE Plasma that continuously updates the desktop background
based on the current time in your location.


## Additional Wallpapers

More dynamic wallpapers can be found at https://github.com/karmanyaahm/awesome-plasma5-dynamic-wallpapers.


## Installation

#### Arch Linux

```
yay -S plasma5-wallpapers-dynamic
```

#### Fedora

```
sudo dnf install plasma-wallpapers-dynamic
```

In order to use the dynamic wallpaper builder tool, install `plasma-wallpapers-dynamic-builder` package.

#### Ubuntu 20.10

```
sudo apt install plasma-wallpaper-dynamic
```


## Building From Git

In order to build this wallpaper plugin from source code, you need to install a
couple of prerequisites

Arch Linux:

```sh
sudo pacman -S cmake extra-cmake-modules git plasma-framework qt5-base qt5-declarative \
    qt5-location libexif libavif
```

Fedora:

```sh
sudo dnf install cmake extra-cmake-modules git kf5-kpackage-devel kf5-plasma-devel \
    kf5-ki18n-devel qt5-qtbase-devel qt5-qtdeclarative-devel qt5-qtlocation-devel \
    libexif-devel libavif-devel
```

Ubuntu:

```sh
sudo apt install cmake extra-cmake-modules git libkf5package-dev libkf5plasma-dev \
    libkf5i18n-dev qtbase5-dev qtdeclarative5-dev qtpositioning5-dev gettext \
    qml-module-qtpositioning libexif-dev libavif-dev build-essential
```

Once all prerequisites are installed, you need to grab the source code

```sh
git clone https://github.com/zzag/plasma5-wallpapers-dynamic.git
cd plasma5-wallpapers-dynamic
```

Configure the build

```sh
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DCMAKE_INSTALL_LIBDIR=lib \
    -DBUILD_TESTING=OFF
```

Now trigger the build by running the following command

```sh
make
```

To install run

```sh
sudo make install
```


## Components

This project consists of the following components:

* A C++ library that provides an easy way to read and write dynamic wallpapers
* A command line tool for creating dynamic wallpapers
* A plugin for KDE Plasma 5 that is responsible for displaying dynamic wallpapers


## How to Use It

Right-click a blank area of the desktop and choose "Configure Desktop...", select
"Dynamic" wallpaper type and click the Apply button.


## How to Create a Dynamic Wallpaper

This engine ships with a command-line tool that one could use to create a dynamic wallpaper from a
collection of jpeg or png images. See [kdynamicwallpaperbuilder](src/tools/builder/README.md).


## How to Use Dynamic Wallpapers for macOS

Since dynamic wallpapers for macOS and this plugin are incompatible, you need to use a script to
convert dynamic wallpapers.

```sh
curl https://git.io/JJkjd -sL > dynamicwallpaperconverter
chmod +x dynamicwallpaperconverter
```

Once you've downloaded the dynamicwallpaperconverter script, you can start converting wallpapers

```sh
./dynamicwallpaperconverter --crossfade file.avif
```

After the command above has finished its execution, you should see a file in the current working
directory named 'wallpaper.avif', which can be fed into this plugin.
