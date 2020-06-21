/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperdescriptionreader.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImageReader>
#include <QJsonDocument>
#include <QTime>

/*!
 * Constructs the DynamicWallpaperDescriptionReader with the file name \p fileName.
 */
DynamicWallpaperDescriptionReader::DynamicWallpaperDescriptionReader(const QString &fileName)
    : m_fileName(fileName)
{
}

/*!
 * Returns \c true if the reader has read until the last descriptor, or if an error has occurred
 * and reading has been aborted. Otherwise, it returns \c false.
 */
bool DynamicWallpaperDescriptionReader::atEnd() const
{
    return m_hasError || m_currentDescriptorIndex >= m_descriptors.count();
}

/*!
 * Attempts to read the next wallpaper image descriptor and returns \c true if successful;
 * otherwise returns \c false if the end of file has been reached or an error occurred.
 */
bool DynamicWallpaperDescriptionReader::readNext()
{
    if (m_hasError)
        return false;

    if (m_descriptors.isEmpty() && !readFile())
        return false;

    m_currentDescriptorIndex++;

    if (m_currentDescriptorIndex >= m_descriptors.count())
        return false;

    const QJsonObject descriptor = m_descriptors[m_currentDescriptorIndex].toObject();

    return readImage(descriptor) && readMetaData(descriptor) && readPrimary(descriptor);
}

/*!
 * Returns the image for the current descriptor.
 *
 * This method will return an invalid QImage if an error has occurred.
 */
QImage DynamicWallpaperDescriptionReader::image() const
{
    if (m_hasError)
        return QImage();
    return m_currentImage;
}

/*!
 * Returns the KDynamicWallpaperMetaData object for the current descriptor.
 *
 * This method will return an invalid KDynamicWallpaperMetaData if an error has occurred.
 */
KDynamicWallpaperMetaData DynamicWallpaperDescriptionReader::metaData() const
{
    if (m_hasError)
        return KDynamicWallpaperMetaData();
    return m_currentMetaData;
}

/*!
 * Returns \c true if the current descriptor describes the primary image; otherwise returns \c false.
 *
 * This method will return \c false if an error has occurred.
 */
bool DynamicWallpaperDescriptionReader::isPrimary() const
{
    return !m_hasError && m_isPrimary;
}

/*!
 * Returns \c true if an error occurred; otherwise returns \c false.
 */
bool DynamicWallpaperDescriptionReader::hasError() const
{
    return m_hasError;
}

/*!
 * Returns the human readable description of the last error that occurred.
 */
QString DynamicWallpaperDescriptionReader::errorString() const
{
    return m_errorString;
}

bool DynamicWallpaperDescriptionReader::readFile()
{
    QFile file(m_fileName);
    if (!file.open(QFile::ReadOnly)) {
        setError(QStringLiteral("Failed to open %1: %2").arg(file.fileName(), file.errorString()));
        return false;
    }

    QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    if (document.isNull()) {
        setError(QStringLiteral("Invalid JSON file"));
        return false;
    }

    m_descriptors = document.array();
    if (m_descriptors.isEmpty()) {
        setError(QStringLiteral("JSON document is empty"));
        return false;
    }

    return true;
}

bool DynamicWallpaperDescriptionReader::readImage(const QJsonObject &descriptor)
{
    const QJsonValue fileName = descriptor[QLatin1String("FileName")];
    if (!fileName.isString()) {
        setError(QStringLiteral("FileName must be a string"));
        return false;
    }

    QString absoluteFileName = fileName.toString();
    if (!QFileInfo(fileName.toString()).isAbsolute())
        absoluteFileName = resolveFileName(fileName.toString());

    QImageReader reader(absoluteFileName);
    if (!reader.read(&m_currentImage))
        setError(QStringLiteral("Failed to read %1: %2").arg(reader.fileName(), reader.errorString()));

    return !hasError();
}

QString DynamicWallpaperDescriptionReader::resolveFileName(const QString &fileName)
{
    const QFileInfo descriptionFileInfo(m_fileName);
    const QDir descriptionDirectory = descriptionFileInfo.dir();
    return descriptionDirectory.absoluteFilePath(fileName);
}

bool DynamicWallpaperDescriptionReader::readMetaData(const QJsonObject &descriptor)
{
    m_currentMetaData = KDynamicWallpaperMetaData();

    const QJsonValue solarElevation = descriptor[QLatin1String("SolarElevation")];
    const QJsonValue solarAzimuth = descriptor[QLatin1String("SolarAzimuth")];
    const QJsonValue crossFadeMode = descriptor[QLatin1String("CrossFade")];
    const QJsonValue time = descriptor[QLatin1String("Time")];

    if (solarAzimuth.isUndefined() ^ solarElevation.isUndefined()) {
        if (solarAzimuth.isUndefined())
            setError(QStringLiteral("SolarElevation was specified but SolarAzimuth was not"));
        else
            setError(QStringLiteral("SolarAzimuth was specified but SolarElevation was not"));
        return false;
    }

    if (!solarElevation.isUndefined()) {
        if (!solarElevation.isDouble()) {
            setError(QStringLiteral("SolarElevation must be a real number"));
            return false;
        }
        m_currentMetaData.setSolarElevation(solarElevation.toDouble());
    }

    if (!solarAzimuth.isUndefined()) {
        if (!solarAzimuth.isDouble()) {
            setError(QStringLiteral("SolarAzimuth must be a real number"));
            return false;
        }
        m_currentMetaData.setSolarAzimuth(solarAzimuth.toDouble());
    }

    if (!crossFadeMode.isUndefined()) {
        if (!crossFadeMode.isBool()) {
            setError(QStringLiteral("CrossFade must be a boolean"));
            return false;
        }
        if (crossFadeMode.toBool())
            m_currentMetaData.setCrossFadeMode(KDynamicWallpaperMetaData::CrossFade);
        else
            m_currentMetaData.setCrossFadeMode(KDynamicWallpaperMetaData::NoCrossFade);
    }

    if (!time.isUndefined()) {
        if (!time.isString()) {
            setError(QStringLiteral("Time must be a string"));
            return false;
        }
        const QTime parsedTime = QTime::fromString(time.toString());
        if (!parsedTime.isValid()) {
            setError(QStringLiteral("Failed to parse Time"));
            return false;
        }
        m_currentMetaData.setTime(parsedTime.msecsSinceStartOfDay() / 86400000.0);
    } else {
        setError(QStringLiteral("Missing Time"));
        return false;
    }

    return true;
}

bool DynamicWallpaperDescriptionReader::readPrimary(const QJsonObject &descriptor)
{
    m_isPrimary = descriptor[QLatin1String("Primary")].toBool();
    return true;
}

void DynamicWallpaperDescriptionReader::setError(const QString &text)
{
    m_errorString = text;
    m_hasError = true;
}
