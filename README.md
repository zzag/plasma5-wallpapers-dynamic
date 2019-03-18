# dynamic-wallpaper

This is a simple dynamic wallpaper plugin for KDE Plasma.


## Build and install

First of all, you have to get the source code. Open the terminal and run the following
command

```sh
git clone https://github.com/zzag/dynamic-wallpaper.git
```

Once you have the source code, you can build the plugin

```sh
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr
make
sudo make install
```

## How to use it

Open desktop configuration dialog and select "Dynamic" wallpaper type.

Please notice that you have to specify your location(latitude and longitude) in
order to use this plugin.


## How to create custom dynamic wallpaper

Each dynamic wallpaper is composed of a bunch of images and associated metadata,
e.g. sun positions, name, etc. The metadata is stored in a json file, which looks
something like this

```json
{
  "name": "Eyecandy Wallpaper",
  "preview": "preview.png",
  "images": [
    {
      "azimuth": 2.73,
      "elevation": -45.17,
      "filename": "image_1.png"
    },
    {
      "azimuth": 56.83,
      "elevation": -24.75,
      "filename": "image_2.png"
    },
    {
      "azimuth": 92.15,
      "elevation": 0.98,
      "filename": "image_3.png"
    },
    {
      "azimuth": 128.26,
      "elevation": 29.67,
      "filename": "image_4.png"
    },
    {
      "azimuth": 182.68,
      "elevation": 43.0,
      "filename": "image_5.png"
    },
    {
      "azimuth": 235.51,
      "elevation": 27.56,
      "filename": "image_6.png"
    },
    {
      "azimuth": 270.81,
      "elevation": -1.41,
      "filename": "image_7.png"
    },
    {
      "azimuth": 307.09,
      "elevation": -31.25,
      "filename": "image_8.png"
    }
  ]
}
```

All images(including the preview image) have to be stored in a folder called
`images` (see below). The preview image is optional, it's used only in the
configuration dialog.

```
└── foobar
    ├── images
    │   ├── image_1.png
    │   ├── image_2.png
    │   ├── image_3.png
    │   ├── image_4.png
    │   ├── image_5.png
    │   ├── image_6.png
    │   ├── image_7.png
    │   ├── image_8.png
    │   └── preview.png
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
