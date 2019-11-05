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
#include <QGeoCoordinate>
#include <QVector3D>

class SunPosition;

/**
 * The SunPath class represents a path of the Sun at the given time and location.
 */
class SunPath
{
public:
    SunPath() = default;

    /**
     * Returns @c true if the path of the Sun is valid; otherwise @c false.
     */
    bool isValid() const;

    /**
     * Returns the coordinates of the center of the sun path.
     */
    QVector3D center() const;

    /**
     * Returns the normal of the plane that contains this sun path.
     */
    QVector3D normal() const;

    /**
     * Projects the given sun position onto this sun path.
     */
    QVector3D project(const SunPosition &position) const;

    /**
     * Creates a path of the Sun for the given time and location.
     */
    static SunPath create(const QDateTime &dateTime, const QGeoCoordinate &location);

private:
    SunPath(const QVector3D &center, const QVector3D &normal);

    QVector3D m_center;
    QVector3D m_normal;
};
