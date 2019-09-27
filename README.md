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
sudo pacman -S cmake extra-cmake-modules git plasma-framework qt5-base \
    qt5-declarative
```

Ubuntu:

```sh
sudo apt install cmake extra-cmake-modules git libkf5plasma-dev qtbase5-dev \
    qtdeclarative5-dev
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

Each dynamic wallpaper is composed of a bunch of images and associated metadata,
e.g. sun positions, name, etc. The metadata is stored in a json file, which looks
something like this

```json
{
    "KPlugin": {
        "Id": "fancy-wallpaper",
        "Name": "My Fancy Wallpaper"
    },
    "Wallpaper": {
        "Preview": "preview.png",
        "Type": "solar",
        "MetaData": [
            {
                "Azimuth": 2.73,
                "Elevation": -45.17,
                "FileName": "image_1.png"
            },
            {
                "Azimuth": 56.83,
                "Elevation": -24.75,
                "FileName": "image_2.png"
            },
            {
                "Azimuth": 92.15,
                "Elevation": 0.98,
                "FileName": "image_3.png"
            },
            {
                "Azimuth": 128.26,
                "Elevation": 29.67,
                "FileName": "image_4.png"
            },
            {
                "Azimuth": 182.68,
                "Elevation": 43.0,
                "FileName": "image_5.png"
            },
            {
                "Azimuth": 235.51,
                "Elevation": 27.56,
                "FileName": "image_6.png"
            },
            {
                "Azimuth": 270.81,
                "Elevation": -1.41,
                "FileName": "image_7.png"
            },
            {
                "Azimuth": 307.09,
                "Elevation": -31.25,
                "FileName": "image_8.png"
            }
        ]
    }
}
```

All images(including the preview image) have to be stored in a folder called
`images` (see below). The preview image is optional, it's used only in the
configuration dialog.

```
foobar
├── contents
│   └── images
│       ├── image_1.png
│       ├── image_2.png
│       ├── image_3.png
│       ├── image_4.png
│       ├── image_5.png
│       ├── image_6.png
│       ├── image_7.png
│       ├── image_8.png
│       └── preview.png
└── metadata.json
```

Once you have all images and the metadata prepared, you can install the wallpaper

```sh
# Install the wallpaper user-wide.
mkdir -p ~/.local/share/dynamicwallpapers
cp -pr foobar ~/.local/share/dynamicwallpapers/

# Install the wallpaper system-wide.
sudo cp -pr foobar /usr/share/dynamicwallpapers/
```
