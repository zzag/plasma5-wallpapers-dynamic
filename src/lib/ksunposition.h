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

class KDYNAMICWALLPAPER_EXPORT KSunPosition
{
public:
    KSunPosition();
    KSunPosition(qreal elevation, qreal azimuth);
    KSunPosition(const QDateTime &dateTime, const QGeoCoordinate &location);

    bool isValid() const;
    qreal elevation() const;
    qreal azimuth() const;
    QVector3D toVector() const;

    static KSunPosition midnight(const QDateTime &dateTime, const QGeoCoordinate &location);

private:
    void init(qreal jcent, const QGeoCoordinate &location, qreal hourAngle);

    qreal m_elevation;
    qreal m_azimuth;
};
