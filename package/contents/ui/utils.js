.pragma library

/**
 * Calculate sunrise and sunset time
 * @param now Current date
 * @param lat The latitude (south is negative, north is positive)
 * @param lon The longitude (west is negative, east is positive)
 **/
function calcSunriseSunset(date, latitude, longitude) {
    function toJulian(date) {
        return date.getTime() / 86400000 + 2440587.5;
    }

    function julianCentury(date) {
        return (toJulian(date) - 2451545) / 36525;
    }

    function radians(degrees) {
        return degrees * Math.PI / 180;
    }

    function degrees(radians) {
        return radians * 180 / Math.PI;
    }

    function sin(angle) {
        return Math.sin(radians(angle));
    }

    function cos(angle) {
        return Math.cos(radians(angle));
    }

    function asin(value) {
        return degrees(Math.asin(value));
    }

    function acos(value) {
        return degrees(Math.acos(value));
    }

    function tan(angle) {
        return Math.tan(radians(angle));
    }

    function meanObliquityOfEcliptic(j) {
        return 23 + (26 + (21.448 - j * (46.815 + j * (0.00059 - j * 0.001813))) / 60) / 60;
    }

    function obliquityCorrection(j) {
        return meanObliquityOfEcliptic(j) + 0.00256 * cos(125.04 - 1934.136 * j);
    }

    function geometricMeanLongitude(j) {
        var lon = (280.46646 + j * (36000.76983 + j * 0.0003032)) % 360;
        return lon < 0 ? lon + 360 : lon;
    }

    function geometricMeanAnomaly(j) {
        return 357.52911 + j * (35999.05029 - 0.0001537 * j);
    }

    function equationOfCenter(j) {
        var anomaly = geometricMeanAnomaly(j);
        return sin(anomaly) * (1.914602 - j * (0.004817 + 0.000014 * j)) +
            sin(2 * anomaly) * (0.019993 - 0.000101 * j) +
            sin(3 * anomaly) * 0.000289;
    }

    function trueLongitude(j) {
        return geometricMeanLongitude(j) + equationOfCenter(j);
    }

    function apparentLongitude(j) {
        return trueLongitude(j) - 0.00569 - 0.00478 * sin(125.04 - 1934.136 * j);
    }

    function declination(j) {
        return asin(sin(obliquityCorrection(j)) * sin(apparentLongitude(j)));
    }

    function eccentricityOfEarthOrbit(j) {
        return 0.016708634 - j * (0.000042037 + 0.0000001267 * j);
    }

    function equationOfTime(j) {
        var correction = obliquityCorrection(j);
        var anomaly = geometricMeanAnomaly(j);
        var longitude = geometricMeanLongitude(j);
        var eccentricity = eccentricityOfEarthOrbit(j);
        var y = Math.pow(tan(correction / 2), 2);
        return 4 * degrees(y * sin(2 * longitude) - 2 * eccentricity * sin(anomaly) +
            4 * eccentricity * y * sin(anomaly) * cos(2 * longitude) -
            0.5 * Math.pow(y, 2) * sin(4 * longitude) -
            1.25 * Math.pow(eccentricity, 2) * sin(2 * anomaly));
    }

    function solarNoon(j, longitude) {
        return 720 - 4 * longitude - equationOfTime(j, longitude) - date.getTimezoneOffset();
    }

    function hourAngle(j, latitude, azimuth) {
        var d = declination(j);
        return acos(cos(azimuth) / (cos(latitude) * cos(d)) - tan(latitude) * tan(d));
    }

    function withStrippedTime(date) {
        var stripped = new Date(date);
        stripped.setHours(0, 0, 0, 0);
        return stripped;
    }

    var jd = julianCentury(date);
    var noon = new Date(withStrippedTime(date).getTime() + solarNoon(jd, longitude) * 1000 * 60);

    var sunsetHourAngle = hourAngle(jd, latitude, 90.833);
    var sunrise = new Date(noon.getTime() - 4 * sunsetHourAngle * 1000 * 60);
    var sunset = new Date(noon.getTime() + 4 * sunsetHourAngle * 1000 * 60);

    var dawnHourAngle = hourAngle(jd, latitude, 96);
    var dawn = new Date(noon.getTime() - 4 * dawnHourAngle * 1000 * 60);
    var dusk = new Date(noon.getTime() + 4 * dawnHourAngle * 1000 * 60);

    return {
        sunrise: sunrise,
        sunset: sunset,
        dawn: dawn,
        dusk: dusk
    };
}
