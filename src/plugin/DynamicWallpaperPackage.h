/*
 * Copyright (C) 2019 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#pragma once

// Own
#include "SunPosition.h"

// Qt
#include <QUrl>
#include <QVector>

// std
#include <memory>

/**
 * This enum type is used to specify type of the dynamic wallpaper.
 */
enum class WallpaperType {
    /**
     * A solar dynamic wallpaper uses the position of the Sun provided along each
     * image to determine what image(s) reflect the user's light situation most
     * accurately.
     *
     * Beware that a solar dynamic wallpaper may not work if the user lives close
     * to the North or the South pole.
     */
    Solar,
    /**
     * A timed dynamic wallpaper uses the current time to determine what images
     * reflect the user's light situation most accurately. The dynamic wallpaper
     * must provide a real number along each image, which is computed as follows
     *
     * @code
     * time = the number of seconds since the start of the day / 86400
     * @endcode
     */
    Timed,
};

struct WallpaperImage {
    /**
     * Position of the Sun when the picture was taken.
     */
    SunPosition position = SunPosition();
    /**
     * A number between 0 and 1 that specifies how much the day has advanced.
     *
     * 0 and 1 correspond to midnight, 0.5 corresponds to noon.
     */
    qreal time = 0;
    /**
     * Path to the picture.
     */
    QUrl url;
};

class DynamicWallpaperPackage {
public:
    /**
     * Returns the type of the dynamic wallpaper.
     */
    WallpaperType type() const;

    /**
     * Returns all images stored in the dynamic wallpaper.
     */
    QVector<WallpaperImage> images() const;

private:
    QVector<WallpaperImage> m_images;
    WallpaperType m_type;

    friend class DynamicWallpaperLoader;
};

class DynamicWallpaperLoader {
public:
    /**
     * Loads a dynamic wallpaper with the given @p id.
     *
     * This method returns @c true if the wallpaper has been loaded successfully;
     * otherwise @c false. If an error has occurred during the loading process,
     * you can use errorText() method to retrieve the error message.
     */
    bool load(const QString& id);

    /**
     * Returns the error text if an error has occurred during the loading process.
     */
    QString errorText() const;

    /**
     * Returns the loaded dynamic wallpaper package object.
     *
     * This method will return @c null if an error has occured during the loading process.
     */
    std::shared_ptr<DynamicWallpaperPackage> wallpaper() const;

private:
    void reset();

    QString m_errorText;
    std::shared_ptr<DynamicWallpaperPackage> m_wallpaper;
};
