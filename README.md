# plasma5-wallpapers-dynamic

[![Packaging status](https://repology.org/badge/vertical-allrepos/plasma5-wallpapers-dynamic.svg)](https://repology.org/project/plasma5-wallpapers-dynamic/versions)

plasma5-wallpapers-dynamic is a simple dynamic wallpaper plugin for KDE Plasma.
[Demo](https://www.youtube.com/watch?v=UIMM6DpEpqA)


## Additional wallpapers

This plugin ships with only one dynamic wallpaper. If you'd like to install more,
please visit [Additional Wallpapers](https://github.com/zzag/plasma5-wallpapers-dynamic/wiki/Additional-Wallpapers)
page.


## Building plasma5-wallpapers-dynamic from Git

Before building plasma5-wallpapers-dynamic from source code, you need to install
a couple of prerequisites.

Arch Linux:

```sh
sudo pacman -S cmake extra-cmake-modules git plasma-framework qt5-base qt5-declarative
```

Fedora:

```sh
sudo dnf install cmake extra-cmake-modules git kf5-kpackage-devel kf5-plasma-devel \
    kf5-ki18n-devel qt5-qtbase-devel qt5-qtdeclarative-devel
```

Ubuntu:

```sh
sudo apt install cmake extra-cmake-modules git libkf5package-dev libkf5plasma-dev \
    qtbase5-dev qtdeclarative5-dev
```

Once all prerequisites are installed, you need to grab the source code

```sh
git clone https://github.com/zzag/plasma5-wallpapers-dynamic.git
cd plasma5-wallpapers-dynamic
```

If you want to build some specific version of this plugin, run the following command
from terminal

```sh
git checkout <version>
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


## Install plasma5-wallpapers-dynamic using a package manager

Arch Linux:

```sh
yay -S plasma5-wallpapers-dynamic
```

openSUSE Tumbleweed:

```sh
zypper addrepo https://download.opensuse.org/repositories/home:KAMiKAZOW:KDE/openSUSE_Tumbleweed/home:KAMiKAZOW:KDE.repo
zypper refresh
zypper install plasma5-dynamic-wallpaper
```


## How to start using this wallpaper plugin

Right-click a blank area of the desktop and choose "Configure Desktop...", select
"Dynamic" wallpaper type and click the Apply button.


## Dynamic wallpaper structure

A dynamic wallpaper is composed of a bunch of images and the associated metadata.
The metadata contains all relevant information about the wallpaper, for example
the user visible name, the type (solar or timed), per image data that can be used
to determine what images reflect the user's light situation most accurately, etc.

A dynamic wallpaper must have the following filesystem structure

```
package
├── contents
│   └── images
│       ├── image.png
│       └── preview.png
└── metadata.json
```

All images, including the preview image, must be stored in contents/images folder.
There must be at least two images, excluding the preview image. The metadata.json
file is located in the toplevel wallpaper directory. This file contains all useful
information about the dynamic wallpaper.

You can find two sample metadata.json files in this repository. One is for a solar
dynamic wallpaper, see [metadata.json.solar.sample](metadata.json.solar.sample),
and the other one is for a timed dynamic wallpaper, see [metadata.json.timed.sample](metadata.json.timed.sample).

For more details, see
* [metadata.json keys](https://github.com/zzag/plasma5-wallpapers-dynamic/wiki/metadata.json-keys)
* [Difference between solar dynamic wallpapers and timed dynamic wallpapers](https://github.com/zzag/plasma5-wallpapers-dynamic/wiki/Difference-between-solar-dynamic-wallpapers-and-timed-dynamic-wallpapers)
* [Importing .heic dynamic wallpapers](https://github.com/zzag/plasma5-wallpapers-dynamic/wiki/Importing-.heic-dynamic-wallpapers)


## How to install a dynamic wallpaper

There are several ways one could install a dynamic wallpaper.

The most preferred way is to use the "Add Wallpaper..." button in the configuration
module of this plugin.

The other way is to use `kpackagetool5` tool

```sh
kpackagetool5 --type Wallpaper/Dynamic --install <path to the wallpaper>
```
