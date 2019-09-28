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

// Own
#include "SunPosition.h"

// Qt
#include <QtMath>

static qreal degrees(qreal radians)
{
    return radians * 180 / M_PI;
}

static qreal radians(qreal degrees)
{
    return degrees * M_PI / 180;
}

static qreal eccentricityEarthOrbit(qreal t)
{
    return 0.016708634 - t * (0.000042037 + 0.0000001267 * t);
}

static qreal solarGeometricMeanAnomaly(qreal t)
{
    return radians(357.52911 + t * (35999.05029 - 0.0001537 * t));
}

static qreal obliquityCorrection(qreal t)
{
    const qreal temp1 = 23 + (26 + (21.448 - t * (46.815 + t * (0.00059 - t * 0.001813))) / 60) / 60;
    const qreal temp2 = 0.00256 * std::cos(radians(125.04 - 1934.136 * t));
    return radians(temp1 + temp2);
}

static qreal solarEquationOfCenter(qreal t)
{
    const qreal anomaly = solarGeometricMeanAnomaly(t);
    return radians(std::sin(anomaly) * (1.914602 - t * (0.004817 + 0.000014 * t))
        + std::sin(2 * anomaly) * (0.019993 - 0.000101 * t)
        + std::sin(3 * anomaly) * 0.000289);
}

static qreal solarGeometricMeanLongitude(qreal t)
{
    const qreal l = std::fmod(280.46646 + t * (36000.76983 + t * 0.0003032), 360);
    return radians(l < 0 ? l + 360 : l);
}

static qreal solarTrueLongitude(qreal t)
{
    return solarGeometricMeanLongitude(t) + solarEquationOfCenter(t);
}

static qreal solarApparentLongitude(qreal t)
{
    return solarTrueLongitude(t) - radians(0.00569 + 0.00478 * std::sin(radians(125.04 - 1934.136 * t)));
}

static qreal solarDeclination(qreal t)
{
    return std::asin(std::sin(obliquityCorrection(t)) * std::sin(solarApparentLongitude(t)));
}

static qreal equationOfTime(qreal t)
{
    const qreal e = eccentricityEarthOrbit(t);
    const qreal m = solarGeometricMeanAnomaly(t);
    const qreal l = solarGeometricMeanLongitude(t);
    const qreal y = std::pow(std::tan(obliquityCorrection(t) / 2), 2);
    return y * std::sin(2 * l) - 2 * e * std::sin(m) + 4 * e * y * std::sin(m) * std::cos(2 * l)
        - 0.5 * y * y * std::sin(4 * l) - 1.25 * e * e * std::sin(2 * m);
}

SunPosition::SunPosition()
    : SunPosition(0, 0)
{
}

SunPosition::SunPosition(qreal elevation, qreal azimuth)
    : m_elevation(elevation)
    , m_azimuth(azimuth)
{
}

SunPosition::SunPosition(const QDateTime& dateTime, qreal latitude, qreal longitude)
{
    const qreal t = (dateTime.date().toJulianDay() - 2451545.0) / 36525.0;

    const qreal progress = (QDateTime(dateTime.date()).secsTo(dateTime) - dateTime.offsetFromUtc()) / 86400.0;
    qreal azimuth = std::fmod(2 * M_PI * progress + equationOfTime(t) + radians(longitude), 2 * M_PI) - M_PI;

    const qreal d = solarDeclination(t);
    qreal zenith = std::acos(std::max(-1.0, std::min(1.0, std::sin(radians(latitude)) * std::sin(radians(d)) + std::cos(radians(latitude)) * std::cos(radians(d)) * std::cos(azimuth))));

    if (azimuth < -M_PI)
        azimuth += 2 * M_PI;

    const qreal azimuthDenominator = std::cos(radians(latitude)) * std::sin(zenith);
    if (azimuthDenominator > 1e-6) {
        const qreal temp = (std::sin(radians(latitude)) * std::cos(zenith) - std::sin(radians(d))) / azimuthDenominator;
        azimuth = (azimuth > 0 ? -1 : 1) * (M_PI - std::acos(std::max(-1.0, std::min(1.0, temp))));
    }

    if (azimuth < 0)
        azimuth += 2 * M_PI;

    const qreal refraction = 90 - degrees(zenith);
    if (refraction <= 85) {
        qreal correction = 0;
        if (refraction > 5) {
            const qreal temp = std::tan(radians(refraction));
            correction = 58.1 / temp - 0.07 / std::pow(temp, 3) + 0.000086 / std::pow(temp, 5);
        } else if (refraction > -0.575) {
            correction = 1735 + refraction * (-518.2 + refraction * (103.4 + refraction * (-12.79 + refraction * 0.711)));
        } else {
            correction = -20.774 / std::tan(radians(refraction));
        }
        zenith -= radians(correction / 3600);
    }

    m_elevation = 90.0 - degrees(zenith);
    m_azimuth = degrees(azimuth);
}

qreal SunPosition::elevation() const
{
    return m_elevation;
}

qreal SunPosition::azimuth() const
{
    return m_azimuth;
}

QVector3D SunPosition::toVector() const
{
    const float x = static_cast<float>(std::cos(radians(m_elevation)) * std::cos(radians(m_azimuth)));
    const float y = static_cast<float>(std::cos(radians(m_elevation)) * std::sin(radians(m_azimuth)));
    const float z = static_cast<float>(std::sin(radians(m_elevation)));
    return QVector3D(x, y, z);
}
