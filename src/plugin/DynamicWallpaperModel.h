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

// Qt
#include <QDateTime>
#include <QUrl>
#include <QVector>

// std
#include <memory>

class DynamicWallpaperData;
class SunPath;

class DynamicWallpaperModel {
public:
    explicit DynamicWallpaperModel(const DynamicWallpaperData* wallpaper,
        qreal latitude, qreal longitude);
    ~DynamicWallpaperModel();

    /**
     * Whether the model is no longer actual.
     */
    bool isExpired() const;

    /**
     * Whether the model is valid.
     */
    bool isValid() const;

    /**
     * Returns path to image that has to be displayed in the bottom layer.
     */
    QUrl bottomLayer() const;

    /**
     * Returns path to image that has to be displayed in the top layer.
     */
    QUrl topLayer() const;

    /**
     * Returns the blend factor between the bottom and the top layer.
     *
     * A value of 0.0 is equivalent to displaying only the bottom layer.
     *
     * A value of 1.0 is equivalent to displaying only the top layer.
     */
    qreal blendFactor() const;

    /**
     * Updates the state of the model.
     */
    void update();

private:
    struct Knot {
        bool operator<(const Knot& other) const { return time < other.time; }
        bool operator<=(const Knot& other) const { return time <= other.time; }

        qreal time;
        QUrl url;
    };

    Knot currentBottomKnot() const;
    Knot currentTopKnot() const;

    QDateTime m_dateTime;
    qreal m_latitude;
    qreal m_longitude;
    std::unique_ptr<SunPath> m_sunPath;
    QVector<Knot> m_knots;
    qreal m_time;
};