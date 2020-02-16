/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

// Own
#include "SunPosition.h"

// Qt
#include <QtMath>

const static qreal s_midnightHourAngle = -180;

static qreal julianCenturiesToJulianDay(qreal jcent)
{
    return 36525.0 * jcent + 2451545.0;
}

static qreal julianDayToJulianCenturies(qreal jd)
{
    return (jd - 2451545.0) / 36525.0;
}

static qreal dateTimeToJulianDay(const QDateTime &dateTime)
{
    return dateTime.toSecsSinceEpoch() / 86400.0 + 2440587.5;
}

static qreal sind(qreal value)
{
    return std::sin(qDegreesToRadians(value));
}

static qreal cosd(qreal value)
{
    return std::cos(qDegreesToRadians(value));
}

static qreal eccentricityEarthOrbit(qreal jcent)
{
    return 0.016708634 - jcent * (0.000042037 + 0.0000001267 * jcent);
}

static qreal solarGeometricMeanAnomaly(qreal jcent)
{
    return qDegreesToRadians(357.52911 + jcent * (35999.05029 - 0.0001537 * jcent));
}

static qreal solarMeanEclipticObliquity(const qreal jcent)
{
    const qreal seconds = 21.448 - jcent * (46.815 + jcent * (0.00059 - jcent * 0.001813));
    return 23.0 + (26.0 + seconds / 60.0) / 60.0;
}

static qreal obliquityCorrection(qreal jcent)
{
    const qreal meanEclipticObliquity = solarMeanEclipticObliquity(jcent);
    const qreal omega = qDegreesToRadians(125.04 - jcent * 1934.136);
    return qDegreesToRadians(meanEclipticObliquity + 0.00256 * std::cos(omega));
}

static qreal solarEquationOfCenter(qreal jcent)
{
    const qreal anomaly = solarGeometricMeanAnomaly(jcent);
    const qreal equation = std::sin(2 * anomaly) * (0.019993 - jcent * 0.000101) +
        std::sin(anomaly) * (1.914602 - jcent * (0.004817 + jcent * 0.000014)) +
        std::sin(3 * anomaly) * 0.000289;
    return qDegreesToRadians(equation);
}

static qreal solarGeometricMeanLongitude(qreal jcent)
{
    qreal l = std::fmod(280.46646 + jcent * (36000.76983 + jcent * 0.0003032), 360);
    if (l < 0)
        l += 360;
    return qDegreesToRadians(l);
}

static qreal solarTrueLongitude(qreal jcent)
{
    return solarGeometricMeanLongitude(jcent) + solarEquationOfCenter(jcent);
}

static qreal solarApparentLongitude(qreal jcent)
{
    const qreal omega = qDegreesToRadians(125.04 - jcent * 1934.136);
    const qreal correction = qDegreesToRadians(-0.00569 - 0.00478 * std::sin(omega));
    return solarTrueLongitude(jcent) + correction;
}

static qreal solarDeclination(qreal jcent)
{
    return std::asin(std::sin(obliquityCorrection(jcent)) * std::sin(solarApparentLongitude(jcent)));
}

static qreal equationOfTime(qreal jcent)
{
    const qreal e = eccentricityEarthOrbit(jcent);
    const qreal m = solarGeometricMeanAnomaly(jcent);
    const qreal l = solarGeometricMeanLongitude(jcent);
    const qreal y = std::pow(std::tan(0.5 * obliquityCorrection(jcent)), 2);
    const qreal equation = y * std::sin(2 * l) - 2 * e * std::sin(m) +
        4 * e * y * std::sin(m) * std::cos(2 * l) - 0.5 * y * y * std::sin(4 * l) -
        1.25 * e * e * std::sin(2 * m);
    return 4 * qRadiansToDegrees(equation);
}

static qreal julianCenturiesToMinutesFromMidnight(qreal jcent)
{
    const qreal jd = julianCenturiesToJulianDay(jcent);
    const qreal minutes = jd - std::round(jd) - 0.5;
    return minutes * 1440.0;
}

static qreal solarHourAngle(qreal jcent, const QGeoCoordinate &location)
{
    const qreal minutes = julianCenturiesToMinutesFromMidnight(jcent);

    const qreal angle = std::fmod(location.longitude() + (equationOfTime(jcent) + minutes - 720) / 4, 360);
    if (angle < -180.0)
        return angle + 360.0;
    if (angle > 180.0)
        return angle - 360.0;

    return angle;
}

static qreal solarZenith(qreal jcent, const QGeoCoordinate &location, qreal hourAngle)
{
    const qreal declination = solarDeclination(jcent);

    const qreal zenith = std::acos(sind(location.latitude()) * std::sin(declination) +
        cosd(location.latitude()) * std::cos(declination) * cosd(hourAngle));

    return qRadiansToDegrees(zenith);
}

static qreal solarAzimuth(qreal jcent, const QGeoCoordinate &location, qreal hourAngle)
{
    const qreal zenith = solarZenith(jcent, location, hourAngle);

    const qreal denominator = cosd(location.latitude()) * sind(zenith);
    if (qFuzzyIsNull(denominator))
        return std::nan("");

    const qreal declination = solarDeclination(jcent);
    const qreal numerator = sind(location.latitude()) * cosd(zenith) - std::sin(declination);

    qreal azimuth = std::acos(qBound(-1.0, numerator / denominator, 1.0));

    if (hourAngle < 0)
        azimuth = M_PI - azimuth;
    else
        azimuth = azimuth + M_PI;

    return qRadiansToDegrees(azimuth);
}

static qreal atmosphericRefractionCorrection(qreal e)
{
    if (e > 85)
        return 0;

    const qreal te = std::tan(qDegreesToRadians(e));
    qreal correction = 0;

    if (e > 5)
        correction = 58.1 / te - 0.07 / (te * te * te) + 0.000086 / (te * te * te * te * te);
    else if (e > -0.575)
        correction = 1735 + e * (-518.2 + e * (103.4 + e * (-12.79 + e * 0.711)));
    else
        correction = -20.774 / te;

    return correction / 3600.0;
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

SunPosition::SunPosition(const QDateTime &dateTime, const QGeoCoordinate &location)
{
    const qreal jd = dateTimeToJulianDay(dateTime);
    const qreal jcent = julianDayToJulianCenturies(jd);
    const qreal hourAngle = solarHourAngle(jcent, location);

    init(jcent, location, hourAngle);
}

bool SunPosition::isValid() const
{
    return !(std::isnan(m_azimuth) || std::isnan(m_elevation));
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

static qreal solarNoonCorrection(qreal jcent, const QGeoCoordinate &location)
{
    return (720 - 4 * location.longitude() - equationOfTime(jcent)) / 1440;
}

SunPosition SunPosition::midnight(const QDateTime &dateTime, const QGeoCoordinate &location)
{
    qreal jdNoon = std::round(dateTimeToJulianDay(dateTime));
    qreal jcentNoon = julianDayToJulianCenturies(jdNoon);

    const qreal correction = solarNoonCorrection(jcentNoon, location);
    jdNoon += correction - 0.5;
    jcentNoon = julianDayToJulianCenturies(jdNoon);

    const qreal jdMidnight = jdNoon + 0.5;
    const qreal jcentMidnight = julianDayToJulianCenturies(jdMidnight);

    SunPosition position;
    position.init(jcentMidnight, location, s_midnightHourAngle);

    return position;
}

void SunPosition::init(qreal jcent, const QGeoCoordinate &location, qreal hourAngle)
{
    const qreal zenith = solarZenith(jcent, location, hourAngle);

    m_elevation = 90 - zenith;
    m_elevation += atmosphericRefractionCorrection(m_elevation);

    m_azimuth = solarAzimuth(jcent, location, hourAngle);
}
