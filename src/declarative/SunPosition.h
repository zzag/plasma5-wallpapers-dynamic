/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

// Qt
#include <QDateTime>
#include <QGeoCoordinate>
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
    SunPosition(const QDateTime &dateTime, const QGeoCoordinate &location);

    /**
     * Returns @c true if the position of the Sun is valid; otherwise @c false.
     */
    bool isValid() const;

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

    /**
     * Determines the position of the Sun (elevation, azimuth) at midnight.
     *
     * The returned value is the lowest position of the Sun in the sky at the
     * given time and at the given location.
     */
    static SunPosition midnight(const QDateTime &dateTime, const QGeoCoordinate &location);

private:
    void init(qreal jcent, const QGeoCoordinate &location, qreal hourAngle);

    qreal m_elevation;
    qreal m_azimuth;
};
