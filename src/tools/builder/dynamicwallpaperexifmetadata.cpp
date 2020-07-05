/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperexifmetadata.h"

#include <KSunPosition>

#include <QGeoCoordinate>
#include <QTimeZone>

#include <libexif/exif-data.h>

/*!
 * \class DynamicWallpaperExifMetaData
 * \brief The DynamicWallpaperExifMetaData class provides a convenient way for computing the position
 * of the Sun from the Exif metadata in an image.
 */

class DynamicWallpaperExifMetaDataPrivate : public QSharedData
{
public:
    void initialize(const QString &fileName);

    DynamicWallpaperExifMetaData::MetaDataFields fields;
    QDateTime birthDateTime;
    KSunPosition solarCoordinates;
};

static ExifEntry *readEntry(ExifData *data, ExifIfd ifd, ExifTag tag)
{
    return exif_content_get_entry(data->ifd[ifd], tag);
}

static QByteArray readAscii(ExifData *data, ExifIfd ifd, ExifTag tag)
{
    ExifEntry *entry = readEntry(data, ifd, tag);
    if (!entry)
        return QByteArray();

    QByteArray buffer(1024, 0);
    exif_entry_get_value(entry, buffer.data(), buffer.size());

    return buffer;
}

/*!
 * \internal
 *
 * Converts \a degrees, \a minutes, and \a seconds to the decimal degrees.
 */
static qreal degreesMinutesSecondsToDecimalDegrees(const ExifRational &degrees,
                                                   const ExifRational &minutes,
                                                   const ExifRational &seconds)
{
    const qreal decimalDegrees = qreal(degrees.numerator) / degrees.denominator;
    const qreal decimalMinutes = qreal(minutes.numerator) / minutes.denominator / 60;
    const qreal decimalSeconds = qreal(seconds.numerator) / seconds.denominator / 3600;
    return decimalDegrees + decimalMinutes + decimalSeconds;
}

static bool readGpsLatitude(ExifData *data, qreal *result)
{
    const ExifEntry *latitude = readEntry(data, EXIF_IFD_GPS, ExifTag(EXIF_TAG_GPS_LATITUDE));
    if (!latitude)
        return false;

    const QByteArray reference = readAscii(data, EXIF_IFD_GPS, ExifTag(EXIF_TAG_GPS_LATITUDE_REF));
    if (reference.isNull())
        return false;

    const ExifByteOrder byteOrder = exif_data_get_byte_order(data);
    const uint stride = exif_format_get_size(latitude->format);

    const ExifRational degrees = exif_get_rational(latitude->data + stride * 0, byteOrder);
    const ExifRational minutes = exif_get_rational(latitude->data + stride * 1, byteOrder);
    const ExifRational seconds = exif_get_rational(latitude->data + stride * 2, byteOrder);

    *result = degreesMinutesSecondsToDecimalDegrees(degrees, minutes, seconds);
    if (reference[0] == 'S')
        *result *= -1;

    return true;
}

static bool readGpsLongitude(ExifData *data, qreal *result)
{
    const ExifEntry *longitude = readEntry(data, EXIF_IFD_GPS, ExifTag(EXIF_TAG_GPS_LONGITUDE));
    if (!longitude)
        return false;

    const QByteArray reference = readAscii(data, EXIF_IFD_GPS, ExifTag(EXIF_TAG_GPS_LONGITUDE_REF));
    if (reference.isNull())
        return false;

    const ExifByteOrder byteOrder = exif_data_get_byte_order(data);
    const uint stride = exif_format_get_size(longitude->format);

    const ExifRational degrees = exif_get_rational(longitude->data + stride * 0, byteOrder);
    const ExifRational minutes = exif_get_rational(longitude->data + stride * 1, byteOrder);
    const ExifRational seconds = exif_get_rational(longitude->data + stride * 2, byteOrder);

    *result = degreesMinutesSecondsToDecimalDegrees(degrees, minutes, seconds);
    if (reference[0] == 'W')
        *result *= -1;

    return true;
}

static bool readGpsCoordinates(ExifData *data, QGeoCoordinate *coordinates)
{
    qreal latitude, longitude;
    if (!readGpsLatitude(data, &latitude))
        return false;
    if (!readGpsLongitude(data, &longitude))
        return false;
    *coordinates = QGeoCoordinate(latitude, longitude);
    return true;
}

/*!
 * \internal
 *
 * Determines the date and the time when the picture was taken. Note that the TimeZoneOffset tag
 * is required because the position of the Sun is computed based on time and location.
 */
static bool readExifDateTime(ExifData *data, QDateTime *result)
{
    QByteArray dateTimeAscii = readAscii(data, EXIF_IFD_EXIF, EXIF_TAG_DATE_TIME_ORIGINAL);
    if (dateTimeAscii.isNull())
        dateTimeAscii = readAscii(data, EXIF_IFD_EXIF, EXIF_TAG_DATE_TIME);
    if (dateTimeAscii.isNull())
        return false;

    const ExifEntry *timeZoneOffset = readEntry(data, EXIF_IFD_EXIF, EXIF_TAG_TIME_ZONE_OFFSET);
    if (!timeZoneOffset)
        return false;

    const QDateTime localDateTime = QDateTime::fromString(dateTimeAscii,
                                                          QStringLiteral("yyyy:MM:dd hh:mm:ss"));
    if (!localDateTime.isValid())
        return false;

    const ExifByteOrder byteOrder = exif_data_get_byte_order(data);
    const QTimeZone timeZone(exif_get_sshort(timeZoneOffset->data, byteOrder) * 86400);

    *result = QDateTime(localDateTime.date(), localDateTime.time(), timeZone);

    return true;
}

/*!
 * \internal
 *
 * Determines the date and the time when the picture was taken based on the GPS timestamp.
 */
static bool readGpsDateTime(ExifData *data, QDateTime *result)
{
    const QByteArray dateAscii = readAscii(data, EXIF_IFD_GPS, ExifTag(EXIF_TAG_GPS_DATE_STAMP));
    if (dateAscii.isNull())
        return false;
    const ExifEntry *timeEntry = readEntry(data, EXIF_IFD_GPS, ExifTag(EXIF_TAG_GPS_TIME_STAMP));
    if (!timeEntry)
        return false;

    const ExifByteOrder byteOrder = exif_data_get_byte_order(data);
    const uint stride = exif_format_get_size(timeEntry->format);

    const ExifRational hoursRational = exif_get_rational(timeEntry->data + stride * 0, byteOrder);
    const ExifRational minutesRational = exif_get_rational(timeEntry->data + stride * 1, byteOrder);
    const ExifRational secondsRational = exif_get_rational(timeEntry->data + stride * 2, byteOrder);

    const QTime time = QTime(hoursRational.numerator / hoursRational.denominator,
                             minutesRational.numerator / minutesRational.denominator,
                             secondsRational.numerator / secondsRational.denominator);
    const QDate date = QDate::fromString(dateAscii, QStringLiteral("yyyy:MM:dd"));

    *result = QDateTime(date, time, QTimeZone::utc());

    return true;
}

static bool readDateTimeOriginal(ExifData *data, QDateTime *result)
{
    return readExifDateTime(data, result) || readGpsDateTime(data, result);
}

void DynamicWallpaperExifMetaDataPrivate::initialize(const QString &fileName)
{
    QGeoCoordinate coordinates;

    ExifData *data = exif_data_new_from_file(fileName.toUtf8());
    if (!data)
        return;

    if (readDateTimeOriginal(data, &birthDateTime))
        fields |= DynamicWallpaperExifMetaData::BirthDateTimeField;

    if (birthDateTime.isValid() && readGpsCoordinates(data, &coordinates)) {
        solarCoordinates = KSunPosition(birthDateTime, coordinates);
        if (solarCoordinates.isValid())
            fields |= DynamicWallpaperExifMetaData::SolarCoordinatesField;
    }

    exif_data_unref(data);
}

/*!
 * Constructs a DynamicWallpaperExifMetaData for an image with the specified \a fileName.
 */
DynamicWallpaperExifMetaData::DynamicWallpaperExifMetaData(const QString &fileName)
    : d(new DynamicWallpaperExifMetaDataPrivate)
{
    d->initialize(fileName);
}

/*!
 * Constructs a copy of the DynamicWallpaperExifMetaData object.
 */
DynamicWallpaperExifMetaData::DynamicWallpaperExifMetaData(const DynamicWallpaperExifMetaData &other)
    : d(other.d)
{
}

/*!
 * Destructs the DynamicWallpaperExifMetaData object.
 */
DynamicWallpaperExifMetaData::~DynamicWallpaperExifMetaData()
{
}

/*!
 * Assigns the value of \p other to a dynamic wallpaper exif metadata object.
 */
DynamicWallpaperExifMetaData &DynamicWallpaperExifMetaData::operator=(const DynamicWallpaperExifMetaData &other)
{
    d = other.d;
    return *this;
}

/*!
 * Returns the date and the time when the picture was taken or created.
 */
QDateTime DynamicWallpaperExifMetaData::birthDateTime() const
{
    return d->birthDateTime;
}

/*!
 * Returns the Sun's azimuth angle when the picture was taken or created, in decimal degrees.
 *
 * If the picture comes with no geo-coordinates metadata, this method will return \c 0.
 */
qreal DynamicWallpaperExifMetaData::solarAzimuth() const
{
    return d->solarCoordinates.azimuth();
}

/*!
 * Returns the Sun's elevation angle when the picture was taken or created, in decimal degrees.
 *
 * If the pictures with with no geo-coordinates metadata, this method will return \c 0.
 */
qreal DynamicWallpaperExifMetaData::solarElevation() const
{
    return d->solarCoordinates.elevation();
}

/*!
 * Returns a bitmask that indicates which fields are present in the metadata.
 */
DynamicWallpaperExifMetaData::MetaDataFields DynamicWallpaperExifMetaData::fields() const
{
    return d->fields;
}

/*!
 * Returns \c true if the DynamicWallpaperExifMetaData is valid; otherwise returns \c false.
 */
bool DynamicWallpaperExifMetaData::isValid() const
{
    return d->fields & BirthDateTimeField;
}
