/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperdescription.h"
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

void DynamicWallpaperDescription::init(const QString &metaDataFileName)
{
    auto resolveFileName = [&metaDataFileName](const QString &fileName) {
        const QFileInfo descriptionFileInfo(metaDataFileName);
        const QDir descriptionDirectory = descriptionFileInfo.dir();
        return descriptionDirectory.absoluteFilePath(fileName);
    };

    QFile file(metaDataFileName);
    if (!file.open(QFile::ReadOnly)) {
        setError(QStringLiteral("Failed to open ") + file.fileName() + QStringLiteral(": ") + file.errorString());
        return;
    }

    QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    if (document.isNull()) {
        setError(QStringLiteral("Not JSON file"));
        return;
    }

    const QJsonArray descriptors = document.array();
    if (descriptors.isEmpty()) {
        setError(QStringLiteral("JSON document is empty"));
        return;
    }

    QMap<int, QString> uniqueFileNames;
    QList<KSolarDynamicWallpaperMetaData> metaDataList;
    QList<KDynamicWallpaperWriter::ImageView> imageList;

    for (int i = 0; i < descriptors.size(); ++i) {
        const QJsonObject descriptor = descriptors[i].toObject();
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

        int index = uniqueFileNames.key(absoluteFileName, -1);
        if (index == -1) {
            if (uniqueFileNames.isEmpty()) {
                index = 0;
            } else {
                index = uniqueFileNames.lastKey() + 1;
            }
            uniqueFileNames.insert(index, absoluteFileName);
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

    for (const QString &fileName : uniqueFileNames)
        imageList.append(KDynamicWallpaperWriter::ImageView(fileName));

    m_metaDataList = metaDataList;
    m_imageList = imageList;
}

void DynamicWallpaperDescription::setError(const QString &text)
{
    m_errorString = text;
    m_hasError = true;
}

/*!
 * Constructs the DynamicWallpaperDescriptionReader with the file name \p fileName.
 */
DynamicWallpaperDescription::DynamicWallpaperDescription(const QString &fileName)
{
    init(fileName);
}

/*!
 * Destructs the DynamicWallpaperDescriptionReader object.
 */
DynamicWallpaperDescription::~DynamicWallpaperDescription()
{
}

QList<KSolarDynamicWallpaperMetaData> DynamicWallpaperDescription::metaData() const
{
    return m_metaDataList;
}

QList<KDynamicWallpaperWriter::ImageView> DynamicWallpaperDescription::images() const
{
    return m_imageList;
}

/*!
 * Returns \c true if an error occurred; otherwise returns \c false.
 */
bool DynamicWallpaperDescription::hasError() const
{
    return m_hasError;
}

/*!
 * Returns the human readable description of the last error that occurred.
 */
QString DynamicWallpaperDescription::errorString() const
{
    return m_errorString;
}
