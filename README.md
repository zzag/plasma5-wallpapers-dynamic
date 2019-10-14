# dynamic-wallpaper

This is a simple dynamic wallpaper plugin for KDE Plasma.
[Demo](https://www.youtube.com/watch?v=UIMM6DpEpqA)


## Additional wallpapers

This plugin ships with only one dynamic wallpaper. If you'd like to install more,
please visit [Additional Wallpapers](https://github.com/zzag/dynamic-wallpaper/wiki/Additional-Wallpapers)
page.


## How to install

Arch Linux:

```sh
yay -S plasma5-wallpapers-dynamic
```

openSUSE Tumbleweed:

The dynamic-wallpaper plugin is available from [home:KAMiKAZOW:KDE](https://software.opensuse.org//download.html?project=home%3AKAMiKAZOW%3AKDE&package=plasma5-dynamic-wallpaper) of openSUSE Build Service.

## Build from source code

### Prerequisites

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


### Build and install

Open the terminal and run the following command

```sh
git clone https://github.com/zzag/dynamic-wallpaper.git
```

Once you have the source code, you can build the plugin

```sh
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DCMAKE_INSTALL_LIBDIR=lib
make
sudo make install
```

## How to use it

Open desktop configuration dialog and select "Dynamic" wallpaper type.


## How to create custom dynamic wallpaper

A dynamic wallpaper is composed of a bunch of images and the associated metadata, e.g. the position of the Sun.

A dynamic wallpaper must have the following filesystem structure

```
foobar
├── contents
│   └── images
│       ├── image.png
│       └── preview.png
└── metadata.json
```

This plugin supports two types of dynamic wallpapers - solar and timed.

With a solar dynamic wallpaper, the position of the Sun is provided along each image. However, do keep in mind that such wallpapers won't work for you if you live near the North or South pole. See sample metadata.json file [metadata.json.solar](metadata.json.solar).

With a timed dynamic wallpaper, a time value is provided along each image. The time value is calculated as follows

    time = the number of seconds since the start of the day / 86400

See sample metadata.json file [metadata.json.timed](metadata.json.timed).

In addition to normal wallpaper images, one can provide the preview image, which is optional.


## How to install a dynamic wallpaper

```sh
kpackagetool5 --type Wallpaper/Dynamic --install foobar
```
