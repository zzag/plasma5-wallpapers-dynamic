/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "sunposition.h"

#include <QUrl>
#include <QVector>

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

struct WallpaperImage
{
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

class DynamicWallpaperPackage
{
public:
    /**
     * Returns the type of the dynamic wallpaper.
     */
    WallpaperType type() const;

    /**
     * Returns @c true if the dynamic wallpaper wants to have a smooth transition
     * between individual images; otherwise @c false.
     */
    bool isSmooth() const;

    /**
     * Returns all images stored in the dynamic wallpaper.
     */
    QVector<WallpaperImage> images() const;

private:
    QVector<WallpaperImage> m_images;
    WallpaperType m_type;
    bool m_isSmooth;

    friend class DynamicWallpaperLoader;
};

class DynamicWallpaperLoader
{
public:
    /**
     * Loads a dynamic wallpaper with the given @p id.
     *
     * This method returns @c true if the wallpaper has been loaded successfully;
     * otherwise @c false. If an error has occurred during the loading process,
     * you can use errorText() method to retrieve the error message.
     */
    bool load(const QString &id);

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
