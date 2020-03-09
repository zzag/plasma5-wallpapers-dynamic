/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#pragma once

#include "kdynamicwallpaper_export.h"

#include <QDateTime>
#include <QGeoCoordinate>
#include <QVector3D>

class KSunPosition;

class KDYNAMICWALLPAPER_EXPORT KSunPath
{
public:
    KSunPath() = default;

    bool isValid() const;
    QVector3D center() const;
    QVector3D normal() const;

    QVector3D project(const KSunPosition &position) const;

    static KSunPath create(const QDateTime &dateTime, const QGeoCoordinate &location);

private:
    KSunPath(const QVector3D &center, const QVector3D &normal, float radius);

    QVector3D m_center;
    QVector3D m_normal;
    float m_radius;
};
