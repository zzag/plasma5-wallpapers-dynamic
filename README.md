# Dynamic Wallpaper Engine

[![Packaging status](https://repology.org/badge/vertical-allrepos/plasma5-wallpapers-dynamic.svg)](https://repology.org/project/plasma5-wallpapers-dynamic/versions)

A wallpaper plugin for KDE Plasma that continuously updates the desktop background
based on the current time in your location.


## Additional Wallpapers

More dynamic wallpapers can be found at https://github.com/karmanyaahm/awesome-plasma5-dynamic-wallpapers.


## Building From Git

In order to build this wallpaper plugin from source code, you need to install a
couple of prerequisites

Arch Linux:

```sh
sudo pacman -S cmake extra-cmake-modules git plasma-framework qt5-base qt5-declarative \
    qt5-location libexif libheif
```

Fedora:

```sh
sudo dnf install cmake extra-cmake-modules git kf5-kpackage-devel kf5-plasma-devel \
    kf5-ki18n-devel qt5-qtbase-devel qt5-qtdeclarative-devel qt5-qtlocation-devel \
    libexif-devel libheif-devel
```

Ubuntu:

```sh
sudo apt install cmake extra-cmake-modules git libkf5package-dev libkf5plasma-dev \
    libkf5i18n-dev qtbase5-dev qtdeclarative5-dev qtpositioning5-dev gettext \
    qml-module-qtpositioning libexif-dev libheif-dev
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
./dynamicwallpaperconverter --crossfade file.heic
```

After the command above has finished its execution, you should see a file in the current working
directory named 'wallpaper.heic', which can be fed into this plugin.
