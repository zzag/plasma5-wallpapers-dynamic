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

class SunPosition;

class SunPath {
public:
    SunPath();
    SunPath(const QDateTime& dateTime, qreal latitude, qreal longitude);

    /**
     * Whether this sun path is valid.
     */
    bool isValid() const;

    /**
     * Returns coordinates of the center of the sun path.
     */
    QVector3D center() const;

    /**
     * Returns normal of the plane that contains this sun path.
     */
    QVector3D normal() const;

    /**
     * Returns Cartesian coordinates of the Sun during midnight.
     */
    QVector3D midnight() const;

    /**
     * Projects the given sun position onto this sun path.
     */
    QVector3D project(const SunPosition& position) const;

private:
    QVector3D m_center;
    QVector3D m_normal;
    QVector3D m_midnight;
};
