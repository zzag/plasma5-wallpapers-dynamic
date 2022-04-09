/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpapermanifest.h"
#include "dynamicwallpaperexifmetadata.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImageReader>
#include <QJsonDocument>
#include <QTime>

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

QString DynamicWallpaperManifest::resolveFileName(const QString &fileName) const
{
    const QFileInfo manifestFileInfo(m_manifestFileName);
    const QDir descriptionDirectory = manifestFileInfo.dir();
    return descriptionDirectory.absoluteFilePath(fileName);
}

void DynamicWallpaperManifest::init()
{
    QFile file(m_manifestFileName);
    if (!file.open(QFile::ReadOnly)) {
        setError(QStringLiteral("Failed to open ") + file.fileName() + QStringLiteral(": ") + file.errorString());
        return;
    }

    QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    if (document.isNull()) {
        setError(QStringLiteral("Not JSON file"));
        return;
    }

    if (document.isObject()) {
        const QJsonObject rootObject = document.object();
        const QString type = rootObject["Type"].toString();
        if (type == QLatin1String("solar")) {
            parseSolar(rootObject["Meta"].toArray());
        } else if (type == QLatin1String("day-night")) {
            parseDayNight(rootObject["Meta"].toArray());
        } else {
            setError(QStringLiteral("Unknown manifest type. Available types: solar"));
        }
    } else {
        parseSolar(document.array()); // Fallback to legacy v3 manifest file format.
    }
}

void DynamicWallpaperManifest::parseSolar(const QJsonArray &entries)
{
    if (entries.isEmpty()) {
        setError(QStringLiteral("No manifest image entries"));
        return;
    }

    QMap<int, QString> indexToFileName;
    QList<KDynamicWallpaperMetaData> metaDataList;
    QList<KDynamicWallpaperWriter::ImageView> imageList;

    for (int i = 0; i < entries.size(); ++i) {
        const QJsonObject descriptor = entries[i].toObject();
        const QJsonValue solarElevation = descriptor[QLatin1String("SolarElevation")];
        const QJsonValue solarAzimuth = descriptor[QLatin1String("SolarAzimuth")];
        const QJsonValue crossFadeMode = descriptor[QLatin1String("CrossFade")];
        const QJsonValue time = descriptor[QLatin1String("Time")];
        const QJsonValue fileName = descriptor[QLatin1String("FileName")];

        QString absoluteFileName = fileName.toString();
        if (absoluteFileName.isEmpty()) {
            setError(QStringLiteral("FileName value was not specified for one or more of the images. Check your json file!"));
            return;
        }

        if (!QFileInfo(fileName.toString()).isAbsolute()) {
            absoluteFileName = resolveFileName(fileName.toString());
        }

        int index = indexToFileName.key(absoluteFileName, -1);
        if (index == -1) {
            if (indexToFileName.isEmpty()) {
                index = 0;
            } else {
                index = indexToFileName.lastKey() + 1;
            }
            indexToFileName.insert(index, absoluteFileName);
        }

        KSolarDynamicWallpaperMetaData::MetaDataFields placeholderFields;

        KSolarDynamicWallpaperMetaData metaData;
        metaData.setIndex(index);

        if (solarAzimuth.isUndefined() ^ solarElevation.isUndefined()) {
            if (solarAzimuth.isUndefined())
                setError(QStringLiteral("SolarElevation was specified but SolarAzimuth was not"));
            else
                setError(QStringLiteral("SolarAzimuth was specified but SolarElevation was not"));
            return;
        }

        if (!solarElevation.isUndefined()) {
            if (isPlaceholderValue(solarElevation)) {
                placeholderFields |= KSolarDynamicWallpaperMetaData::SolarElevationField;
            } else if (solarElevation.isDouble()) {
                metaData.setSolarElevation(solarElevation.toDouble());
            } else {
                setError(QStringLiteral("Invalid solar elevation value has been specified for ") + absoluteFileName);
                return;
            }
        }

        if (!solarAzimuth.isUndefined()) {
            if (isPlaceholderValue(solarAzimuth)) {
                placeholderFields |= KSolarDynamicWallpaperMetaData::SolarAzimuthField;
            } else if (solarAzimuth.isDouble()) {
                metaData.setSolarAzimuth(solarAzimuth.toDouble());
            } else {
                setError(QStringLiteral("Invalid solar azimuth value has been specified for ") + absoluteFileName);
                return;
            }
        }

        if (!crossFadeMode.isUndefined()) {
            if (crossFadeMode.toBool())
                metaData.setCrossFadeMode(KSolarDynamicWallpaperMetaData::CrossFade);
            else
                metaData.setCrossFadeMode(KSolarDynamicWallpaperMetaData::NoCrossFade);
        }

        if (!time.isUndefined()) {
            if (isPlaceholderValue(time)) {
                placeholderFields |= KSolarDynamicWallpaperMetaData::TimeField;
            } else {
                const QTime parsedTime = QTime::fromString(time.toString());
                if (parsedTime.isValid()) {
                    metaData.setTime(timeToReal(parsedTime));
                } else {
                    setError(QStringLiteral("Failed to parse time for image ") + absoluteFileName);
                    return;
                }
            }
        } else {
            setError(QStringLiteral("No time has been provided for ") + absoluteFileName);
            return;
        }

        if (placeholderFields) {
            DynamicWallpaperExifMetaData exifMetaData(absoluteFileName);
            if (placeholderFields & KSolarDynamicWallpaperMetaData::SolarAzimuthField) {
                if (exifMetaData.fields() & DynamicWallpaperExifMetaData::SolarCoordinatesField) {
                    metaData.setSolarAzimuth(exifMetaData.solarAzimuth());
                } else {
                    setError(QStringLiteral("%1: Failed to compute the position of the Sun based on GPS "
                                            "coordinates and the time when the photo was taken. Please check "
                                            "that the specified image actually has GPS coordinates in its "
                                            "Exif metadata. You can do that with a tool such as exiftool.")
                                 .arg(absoluteFileName));
                    return;
                }
            }
            if (placeholderFields & KSolarDynamicWallpaperMetaData::SolarElevationField) {
                if (exifMetaData.fields() & DynamicWallpaperExifMetaData::SolarCoordinatesField) {
                    metaData.setSolarElevation((exifMetaData.solarElevation()));
                } else {
                    setError(QStringLiteral("%1: Failed to compute the position of the Sun based on GPS "
                                            "coordinates and the time when the photo was taken. Please check "
                                            "that the specified image actually has GPS coordinates in its "
                                            "Exif metadata. You can do that with a tool such as exiftool.")
                                 .arg(absoluteFileName));
                    return;
                }
            }
            if (placeholderFields & KSolarDynamicWallpaperMetaData::TimeField) {
                if (exifMetaData.fields() & DynamicWallpaperExifMetaData::BirthDateTimeField) {
                    metaData.setTime(timeToReal(exifMetaData.birthDateTime().time()));
                } else {
                    setError(QStringLiteral("Failed to determine the time when %1 was taken from its Exif metadata").arg(absoluteFileName));
                    return;
                }
            }
        }

        metaDataList.append(metaData);
    }

    for (const QString &fileName : indexToFileName)
        imageList.append(KDynamicWallpaperWriter::ImageView(fileName));

    m_metaDataList = metaDataList;
    m_imageList = imageList;
}

void DynamicWallpaperManifest::parseDayNight(const QJsonArray &entries)
{
    if (entries.isEmpty()) {
        setError(QStringLiteral("No manifest image entries"));
        return;
    }

    QString dayFileName;
    QString nightFileName;

    for (int i = 0; i < entries.size(); ++i) {
        const QJsonObject descriptor = entries[i].toObject();
        QString *fileName;

        const QString timeOfDay = descriptor["TimeOfDay"].toString();
        if (timeOfDay == QLatin1String("day")) {
            fileName = &dayFileName;
        } else if (timeOfDay == QLatin1String("night")) {
            fileName = &nightFileName;
        } else {
            setError(QStringLiteral("Unknown TimeOfDay value. Possible values: day, night"));
            return;
        }

        *fileName = descriptor["FileName"].toString();
        if (fileName->isEmpty()) {
            setError(QStringLiteral("No image file name has been specified"));
            return;
        }
        if (!QFileInfo(*fileName).isAbsolute()) {
            *fileName = resolveFileName(*fileName);
        }
    }

    if (dayFileName.isEmpty()) {
        setError(QStringLiteral("No day picture has been specified"));
        return;
    }
    if (nightFileName.isEmpty()) {
        setError(QStringLiteral("No night picture has been specified"));
        return;
    }

    m_imageList = {
        KDynamicWallpaperWriter::ImageView(dayFileName),
        KDynamicWallpaperWriter::ImageView(nightFileName),
    };

    m_metaDataList = {
        KDayNightDynamicWallpaperMetaData(KDayNightDynamicWallpaperMetaData::TimeOfDay::Day, 0),
        KDayNightDynamicWallpaperMetaData(KDayNightDynamicWallpaperMetaData::TimeOfDay::Night, 1),
    };
}

void DynamicWallpaperManifest::setError(const QString &text)
{
    m_errorString = text;
    m_hasError = true;
}

/*!
 * Constructs the DynamicWallpaperDescriptionReader with the file name \p fileName.
 */
DynamicWallpaperManifest::DynamicWallpaperManifest(const QString &fileName)
    : m_manifestFileName(fileName)
{
    init();
}

/*!
 * Destructs the DynamicWallpaperDescriptionReader object.
 */
DynamicWallpaperManifest::~DynamicWallpaperManifest()
{
}

QList<KDynamicWallpaperMetaData> DynamicWallpaperManifest::metaData() const
{
    return m_metaDataList;
}

QList<KDynamicWallpaperWriter::ImageView> DynamicWallpaperManifest::images() const
{
    return m_imageList;
}

/*!
 * Returns \c true if an error occurred; otherwise returns \c false.
 */
bool DynamicWallpaperManifest::hasError() const
{
    return m_hasError;
}

/*!
 * Returns the human readable description of the last error that occurred.
 */
QString DynamicWallpaperManifest::errorString() const
{
    return m_errorString;
}
