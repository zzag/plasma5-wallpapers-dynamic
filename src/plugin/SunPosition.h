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

// Qt
#include <QDateTime>
#include <QVector3D>

class SunPosition {
public:
    SunPosition();
    SunPosition(qreal elevation, qreal azimuth);
    SunPosition(const QDateTime& time, qreal latitude, qreal longitude);

    /**
     * Elevation of the Sun above the local horizon.
     */
    qreal elevation() const;

    /**
     * Sun's relative direction along the local horizon, where 0 degrees is north,
     * 90 degrees is east, 180 degrees is south, and 270 degrees is west.
     */
    qreal azimuth() const;

    /**
     * Converts the sun position (elevation, azimuth) to Cartesian coordinates.
     */
    QVector3D toVector() const;

private:
    qreal m_elevation;
    qreal m_azimuth;
};
