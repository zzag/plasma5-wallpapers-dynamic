<!--
SPDX-FileCopyrightText: none

SPDX-License-Identifier: CC0-1.0
-->

# kdynamicwallpaperbuilder

kdynamicwallpaperbuilder is a command line tool designed for creating dynamic wallpapers from a
collection of images.


## Getting Started

In order to create a dynamic wallpaper, you need to provide a manifest json file with the following structure

```json
[
    {
        "SolarAzimuth": "*",
        "SolarElevation": "*",
        "CrossFade": true,
        "Time": "18:00",
        "FileName": "0.png"
    },
    {
        "SolarAzimuth": 0,
        "SolarElevation": -90,
        "CrossFade": true,
        "Time": "00:00",
        "FileName": "1.png"
    },
    {
        "SolarAzimuth": 90,
        "SolarElevation": 0,
        "CrossFade": true,
        "Time": "06:00",
        "FileName": "2.png"
    },
    {
        "SolarAzimuth": 180,
        "SolarElevation": 90,
        "CrossFade": true,
        "Time": "12:00",
        "FileName": "3.png"
    }
]
```

It might look like you must provide a lot of data, but don't be scared. Let's break it down.

The `SolarAzimuth` field and the `SolarElevation` field specify the position of the Sun when the
associated picture was taken. The `Time` field specifies the time, which is in 24-hour format, when
the picture was taken. If the user is not located near the North or the South Pole, the dynamic
wallpaper engine will try to show images based on the current position of the Sun; otherwise it will
fallback to using time metadata. Note that only the `Time` field is required, the position of the
Sun is optional.

If `SolarAzimuth` or `SolarElevation` has a special value of `"*"`, then the position of the Sun
will be computed based on GPS coordinates and the time when the picture was taken.

The `CrossFade` field indicates whether the current image can be blended with the next one. The
cross-fading is used to make transitions between images smooth. By default, the `CrossFade` field is
set to `true`. Last, but not least, the `FileName` field specifies the file path of the image
relative to the manifest json file.

Now that you have prepared all images and a manifest file, it's time pull out big guns. Run the
following command

```sh
kdynamicwallpaperbuilder path/to/manifest.json
```

It may take some time before the command completes, so be patient. If everything goes well, you
should see a new file in the current working directory `wallpaper.avif`, which can be used as a
dynamic wallpaper.
