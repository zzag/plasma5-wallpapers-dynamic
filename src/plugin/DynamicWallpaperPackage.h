/*
 * Copyright (C) 2019 Vlad Zagorodniy <vladzzag@gmail.com>
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

struct Image {
    /**
     * Position of the Sun when the picture was taken.
     */
    SunPosition position;
    /**
     * Path to the picture.
     */
    QUrl url;
};

class DynamicWallpaperPackage {
public:
    /**
     * Returns the name of the dynamic wallpaper.
     */
    QString name() const;

    /**
     * Returns all images stored in the dynamic wallpaper.
     */
    QVector<Image> images() const;

    /**
     * Loads dynamic wallpaper with the given id.
     *
     * If there is no such wallpaper, @c null is returned.
     */
    static std::unique_ptr<DynamicWallpaperPackage> load(const QString& id);

private:
    QString m_name;
    QVector<Image> m_images;
};