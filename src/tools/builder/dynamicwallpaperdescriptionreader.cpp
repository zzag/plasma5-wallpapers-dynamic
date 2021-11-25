/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperdescriptionreader.h"
#include "dynamicwallpaperexifmetadata.h"

#include <KLocalizedString>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImageReader>
#include <QJsonDocument>
#include <QTime>

class DynamicWallpaperReaderState
{
public:
    KDynamicWallpaperMetaData metaData;
    QImage image;
    bool isPrimary;
};

class DynamicWallpaperDescriptionReaderPrivate
{
public:
    bool open();
    void setError(const QString &text);
    QString resolveFileName(const QString &fileName);
    bool read(int index);

    DynamicWallpaperReaderState state;
    QString errorString;
    QString metaDataFileName;
    QJsonArray descriptors;
    int descriptorIndex = -1;
    bool atEnd = false;
    bool hasError = false;
};

bool DynamicWallpaperDescriptionReaderPrivate::open()
{
    QFile file(metaDataFileName);
    if (!file.open(QFile::ReadOnly)) {
        setError(i18n("Failed to open %1: %2", file.fileName(), file.errorString()));
        return false;
    }

    QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    if (document.isNull()) {
        setError(i18n("Invalid JSON file"));
        return false;
    }

    descriptors = document.array();
    if (descriptors.isEmpty()) {
        setError(i18n("JSON document is empty"));
        return false;
    }

    return true;
}

/*!
 * \internal
 *
 * Returns \c true if the specified value is a placeholder; otherwise returns \c false.
 *
 * A placeholder indicates that the correponding value must be computed by the builder.
 */
static bool isPlaceholderValue(const QJsonValue &value)
{
    return value.toString().trimmed() == QStringLiteral("*");
}

/*!
 * \internal
 *
 * Returns a real number between 0 and 1, where 0 and 1 correspond to midnight, and 0.5
 * corresponds to noon.
 */
static qreal timeToReal(const QTime &time)
{
    return time.msecsSinceStartOfDay() / 86400000.0;
}

bool DynamicWallpaperDescriptionReaderPrivate::read(int index)
{
    state = DynamicWallpaperReaderState();

    const QJsonObject descriptor = descriptors[index].toObject();
    const QJsonValue solarElevation = descriptor[QLatin1String("SolarElevation")];
    const QJsonValue solarAzimuth = descriptor[QLatin1String("SolarAzimuth")];
    const QJsonValue crossFadeMode = descriptor[QLatin1String("CrossFade")];
    const QJsonValue time = descriptor[QLatin1String("Time")];
    const QJsonValue fileName = descriptor[QLatin1String("FileName")];

    QString absoluteFileName = fileName.toString();

    if (absoluteFileName.isEmpty()) {
        setError(i18n("FileName value was not specified for one or more of the images. Check your json file!"));
        return false;
    }

    if (!QFileInfo(fileName.toString()).isAbsolute())
        absoluteFileName = resolveFileName(fileName.toString());

    QImageReader reader(absoluteFileName);
    if (!reader.read(&state.image)) {
        setError(i18n("Failed to read %1: %2", reader.fileName(), reader.errorString()));
        return false;
    }

    KDynamicWallpaperMetaData::MetaDataFields placeholderFields;

    if (solarAzimuth.isUndefined() ^ solarElevation.isUndefined()) {
        if (solarAzimuth.isUndefined())
            setError(i18n("SolarElevation was specified but SolarAzimuth was not"));
        else
            setError(i18n("SolarAzimuth was specified but SolarElevation was not"));
        return false;
    }

    if (!solarElevation.isUndefined()) {
        if (isPlaceholderValue(solarElevation)) {
            placeholderFields |= KDynamicWallpaperMetaData::SolarElevationField;
        } else if (solarElevation.isDouble()) {
            state.metaData.setSolarElevation(solarElevation.toDouble());
        } else {
            setError(i18n("Invalid solar elevation value has been specified for %1",
                          absoluteFileName));
            return false;
        }
    }

    if (!solarAzimuth.isUndefined()) {
        if (isPlaceholderValue(solarAzimuth)) {
            placeholderFields |= KDynamicWallpaperMetaData::SolarAzimuthField;
        } else if (solarAzimuth.isDouble()) {
            state.metaData.setSolarAzimuth(solarAzimuth.toDouble());
        } else {
            setError(i18n("Invalid solar azimuth value has been specified for %1",
                          absoluteFileName));
            return false;
        }
    }

    if (!crossFadeMode.isUndefined()) {
        if (crossFadeMode.toBool())
            state.metaData.setCrossFadeMode(KDynamicWallpaperMetaData::CrossFade);
        else
            state.metaData.setCrossFadeMode(KDynamicWallpaperMetaData::NoCrossFade);
    }

    if (!time.isUndefined()) {
        if (isPlaceholderValue(time)) {
            placeholderFields |= KDynamicWallpaperMetaData::TimeField;
        } else {
            const QTime parsedTime = QTime::fromString(time.toString());
            if (parsedTime.isValid()) {
                state.metaData.setTime(timeToReal(parsedTime));
            } else {
                setError(i18n("Failed to parse time for image %1", absoluteFileName));
                return false;
            }
        }
    } else {
        setError(i18n("No time has been provided for %1", absoluteFileName));
        return false;
    }

    if (placeholderFields) {
        DynamicWallpaperExifMetaData exifMetaData(absoluteFileName);
        if (placeholderFields & KDynamicWallpaperMetaData::SolarAzimuthField) {
            if (exifMetaData.fields() & DynamicWallpaperExifMetaData::SolarCoordinatesField) {
                state.metaData.setSolarAzimuth(exifMetaData.solarAzimuth());
            } else {
                setError(i18n("%1: Failed to compute the position of the Sun based on GPS "
                              "coordinates and the time when the photo was taken. Please check "
                              "that the specified image actually has GPS coordinates in its "
                              "Exif metadata. You can do that with a tool such as exiftool.",
                              absoluteFileName));
                return false;
            }
        }
        if (placeholderFields & KDynamicWallpaperMetaData::SolarElevationField) {
            if (exifMetaData.fields() & DynamicWallpaperExifMetaData::SolarCoordinatesField) {
                state.metaData.setSolarElevation((exifMetaData.solarElevation()));
            } else {
                setError(i18n("%1: Failed to compute the position of the Sun based on GPS "
                              "coordinates and the time when the photo was taken. Please check "
                              "that the specified image actually has GPS coordinates in its "
                              "Exif metadata. You can do that with a tool such as exiftool.",
                              absoluteFileName));
                return false;
            }
        }
        if (placeholderFields & KDynamicWallpaperMetaData::TimeField) {
            if (exifMetaData.fields() & DynamicWallpaperExifMetaData::BirthDateTimeField) {
                state.metaData.setTime(timeToReal(exifMetaData.birthDateTime().time()));
            } else {
                setError(i18n("Failed to determine the time when %1 was taken from its Exif "
                              "metadata", absoluteFileName));
                return false;
            }
        }
    }

    state.isPrimary = descriptor[QLatin1String("Primary")].toBool();

    return true;
}

QString DynamicWallpaperDescriptionReaderPrivate::resolveFileName(const QString &fileName)
{
    const QFileInfo descriptionFileInfo(metaDataFileName);
    const QDir descriptionDirectory = descriptionFileInfo.dir();
    return descriptionDirectory.absoluteFilePath(fileName);
}

void DynamicWallpaperDescriptionReaderPrivate::setError(const QString &text)
{
    errorString = text;
    hasError = true;
}

/*!
 * Constructs the DynamicWallpaperDescriptionReader with the file name \p fileName.
 */
DynamicWallpaperDescriptionReader::DynamicWallpaperDescriptionReader(const QString &fileName)
    : d(new DynamicWallpaperDescriptionReaderPrivate)
{
    d->metaDataFileName = fileName;
}

/*!
 * Destructs the DynamicWallpaperDescriptionReader object.
 */
DynamicWallpaperDescriptionReader::~DynamicWallpaperDescriptionReader()
{
}

/*!
 * Returns \c true if the reader has read until the last descriptor, or if an error has occurred
 * and reading has been aborted. Otherwise, it returns \c false.
 */
bool DynamicWallpaperDescriptionReader::atEnd() const
{
    return d->hasError || d->descriptorIndex >= d->descriptors.count();
}

/*!
 * Attempts to read the next wallpaper image descriptor and returns \c true if successful;
 * otherwise returns \c false if the end of file has been reached or an error occurred.
 */
bool DynamicWallpaperDescriptionReader::readNext()
{
    if (d->hasError)
        return false;

    if (d->descriptors.isEmpty() && !d->open())
        return false;

    d->descriptorIndex++;

    if (d->descriptorIndex >= d->descriptors.count())
        return false;

    return d->read(d->descriptorIndex);
}

/*!
 * Returns the image for the current descriptor.
 *
 * This method will return an invalid QImage if an error has occurred.
 */
QImage DynamicWallpaperDescriptionReader::image() const
{
    if (d->hasError)
        return QImage();
    return d->state.image;
}

/*!
 * Returns the KDynamicWallpaperMetaData object for the current descriptor.
 *
 * This method will return an invalid KDynamicWallpaperMetaData if an error has occurred.
 */
KDynamicWallpaperMetaData DynamicWallpaperDescriptionReader::metaData() const
{
    if (d->hasError)
        return KDynamicWallpaperMetaData();
    return d->state.metaData;
}

/*!
 * Returns \c true if the current descriptor describes the primary image; otherwise returns \c false.
 *
 * This method will return \c false if an error has occurred.
 */
bool DynamicWallpaperDescriptionReader::isPrimary() const
{
    return !d->hasError && d->state.isPrimary;
}

/*!
 * Returns \c true if an error occurred; otherwise returns \c false.
 */
bool DynamicWallpaperDescriptionReader::hasError() const
{
    return d->hasError;
}

/*!
 * Returns the human readable description of the last error that occurred.
 */
QString DynamicWallpaperDescriptionReader::errorString() const
{
    return d->errorString;
}
