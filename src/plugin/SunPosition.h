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

/**
 * The SunPosition class provides a convenient way for determining the position
 * of the Sun at the given time and location.
 */
class SunPosition
{
public:
    SunPosition();
    SunPosition(qreal elevation, qreal azimuth);
    SunPosition(const QDateTime &dateTime, qreal latitude, qreal longitude);

    /**
     * Returns the elevation angle of the Sun, in decimal degrees.
     */
    qreal elevation() const;

    /**
     * Returns the azimuth angle of the Sun, in decimal degrees.
     *
     * The azimuth angle specifies the Sun's relative direction along the local
     * horizon, where 0 degrees corresponds to north, 90 degrees corresponds to
     * east, 180 degrees corresponds to south, and 270 degrees corresponds to
     * west.
     */
    qreal azimuth() const;

    /**
     * Converts the position of the Sun (elevation, azimuth) to the Cartesian
     * coordinates.
     *
     * The returned value is a unit vector, i.e. it has a magnitude of 1.
     */
    QVector3D toVector() const;

private:
    qreal m_elevation;
    qreal m_azimuth;
};
