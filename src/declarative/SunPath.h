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
     * Returns the coordinates of the center of this path of the Sun.
     */
    QVector3D center() const;

    /**
     * Returns the normal of the plane that contains this path of the Sun.
     */
    QVector3D normal() const;

    /**
     * Projects given position of the Sun onto this path of the Sun.
     */
    QVector3D project(const SunPosition &position) const;

    /**
     * Creates a path of the Sun for the given time and location.
     */
    static SunPath create(const QDateTime &dateTime, const QGeoCoordinate &location);

private:
    SunPath(const QVector3D &center, const QVector3D &normal, float radius);

    QVector3D m_center;
    QVector3D m_normal;
    float m_radius;
};
