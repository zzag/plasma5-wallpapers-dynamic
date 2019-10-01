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

// std
#include <limits>

static qreal julianCenturiesToJulianDay(qreal t)
{
    return 36525.0 * t + 2451545.0;
}

static qreal julianDayToJulianCenturies(qreal t)
{
    return (t - 2451545.0) / 36525.0;
}

static qreal julianCenturies(const QDateTime& dateTime)
{
    const qreal jd = dateTime.toSecsSinceEpoch() / 86400.0 + 2440587.5;
    return julianDayToJulianCenturies(jd);
}

static qreal sind(qreal value)
{
    return std::sin(qDegreesToRadians(value));
}

static qreal cosd(qreal value)
{
    return std::cos(qDegreesToRadians(value));
}

static qreal tand(qreal value)
{
    return std::tan(qDegreesToRadians(value));
}

static qreal eccentricityEarthOrbit(qreal t)
{
    return 0.016708634 - t * (0.000042037 + 0.0000001267 * t);
}

static qreal solarGeometricMeanAnomaly(qreal t)
{
    return 357.52911 + t * (35999.05029 - 0.0001537 * t);
}

static qreal obliquityCorrection(qreal t)
{
    const qreal temp1 = 23 + (26 + (21.448 - t * (46.815 + t * (0.00059 - t * 0.001813))) / 60) / 60;
    const qreal temp2 = 0.00256 * cosd(125.04 - t * 1934.136);
    return temp1 + temp2;
}

static qreal solarEquationOfCenter(qreal t)
{
    const qreal anomaly = solarGeometricMeanAnomaly(t);
    return sind(anomaly) * (1.914602 - t * (0.004817 + t * 0.000014))
        + sind(2 * anomaly) * (0.019993 - t * 0.000101)
        + sind(3 * anomaly) * 0.000289;
}

static qreal solarGeometricMeanLongitude(qreal t)
{
    const qreal l = std::fmod(280.46646 + t * (36000.76983 + t * 0.0003032), 360);
    return l < 0 ? l + 360 : l;
}

static qreal solarTrueLongitude(qreal t)
{
    return solarGeometricMeanLongitude(t) + solarEquationOfCenter(t);
}

static qreal solarApparentLongitude(qreal t)
{
    return solarTrueLongitude(t) - 0.00569 - 0.00478 * sind(125.04 - t * 1934.136);
}

static qreal solarDeclination(qreal t)
{
    return std::asin(sind(obliquityCorrection(t)) * sind(solarApparentLongitude(t)));
}

static qreal equationOfTime(qreal t)
{
    const qreal e = eccentricityEarthOrbit(t);
    const qreal m = solarGeometricMeanAnomaly(t);
    const qreal l = solarGeometricMeanLongitude(t);
    const qreal y = std::pow(tand(0.5 * obliquityCorrection(t)), 2);
    const qreal equation = y * sind(2 * l) - 2 * e * sind(m)
        + 4 * e * y * sind(m) * cosd(2 * l) - 0.5 * y * y * sind(4 * l)
        - 1.25 * e * e * sind(2 * m);
    return qRadiansToDegrees(equation);
}

static qreal hourAngle(qreal t, qreal longitude)
{
    const qreal jd = julianCenturiesToJulianDay(t);
    const qreal offset = (jd - std::round(jd) - 0.5) * 1440;

    qreal angle = std::fmod(longitude + equationOfTime(t) - (720 - offset) / 4, 360);
    if (angle < -180)
        angle += 360;
    if (angle > 180)
        angle -= 360;

    return angle;
}

static qreal solarZenith(qreal t, qreal latitude, qreal longitude)
{
    const qreal angle = hourAngle(t, longitude);
    const qreal declination = solarDeclination(t);

    const qreal zenith = std::acos(sind(latitude) * std::sin(declination)
        + cosd(latitude) * std::cos(declination) * cosd(angle));

    return qRadiansToDegrees(zenith);
}

static qreal solarAzimuth(qreal t, qreal latitude, qreal longitude, qreal zenith)
{
    const qreal denominator = cosd(latitude) * sind(zenith);
    if (qFuzzyIsNull(denominator))
        return std::numeric_limits<qreal>::quiet_NaN();

    const qreal declination = solarDeclination(t);
    const qreal numerator = sind(latitude) * cosd(zenith) - std::sin(declination);

    qreal azimuth = std::acos(qBound(-1.0, numerator / denominator, 1.0));

    if (hourAngle(t, longitude) < 0)
        azimuth = M_PI - azimuth;
    else
        azimuth = azimuth + M_PI;

    return qRadiansToDegrees(azimuth);
}

static qreal atmosphericRefractionCorrection(qreal e)
{
    if (e > 85)
        return 0;

    const qreal te = tand(e);
    qreal correction = 0;

    if (e > 5)
        correction = 58.1 / te - 0.07 / (te * te * te) + 0.000086 / (te * te * te * te * te);
    else if (e > -0.575)
        correction = 1735 + e * (-518.2 + e * (103.4 + e * (-12.79 + e * 0.711)));
    else
        correction = -20.774 / te;

    return correction / 3600;
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
    const qreal jcent = julianCenturies(dateTime);
    const qreal zenith = solarZenith(jcent, latitude, longitude);

    m_azimuth = solarAzimuth(jcent, latitude, longitude, zenith);

    m_elevation = 90 - zenith;
    m_elevation += atmosphericRefractionCorrection(m_elevation);
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
    const float x = static_cast<float>(cosd(m_elevation) * cosd(m_azimuth));
    const float y = static_cast<float>(cosd(m_elevation) * sind(m_azimuth));
    const float z = static_cast<float>(sind(m_elevation));

    return QVector3D(x, y, z);
}
