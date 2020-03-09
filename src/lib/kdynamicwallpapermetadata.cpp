/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "kdynamicwallpapermetadata.h"

#include <QDomDocument>
#include <QDomNode>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSharedData>

/*!
 * \class KDynamicWallpaperMetaData
 * \brief The KDynamicWallpaperMetaData class represents metadata associated with images
 * in the dynamic wallpaper.
 *
 * KDynamicWallpaperMetaData provides information about images in the dynamic wallpaper,
 * for example, solar position, etc. Some fields may not be specified. In order to check
 * whether the given field is set, test the corresponding bit in fields().
 */

static QJsonValue crossFadeModeToJson(KDynamicWallpaperMetaData::CrossFadeMode crossFadeMode)
{
    switch (crossFadeMode) {
    case KDynamicWallpaperMetaData::NoCrossFade:
        return QJsonValue(false);
    case KDynamicWallpaperMetaData::CrossFade:
        return QJsonValue(true);
    default:
        Q_UNREACHABLE();
    }
}

static KDynamicWallpaperMetaData::CrossFadeMode crossFadeModeFromJson(const QJsonValue &value)
{
    return value.toBool() ? KDynamicWallpaperMetaData::CrossFade : KDynamicWallpaperMetaData::NoCrossFade;
}

class KDynamicWallpaperMetaDataPrivate : public QSharedData
{
public:
    KDynamicWallpaperMetaDataPrivate();

    KDynamicWallpaperMetaData::MetaDataFields presentFields;
    KDynamicWallpaperMetaData::CrossFadeMode crossFadeMode;
    qreal solarAzimuth;
    qreal solarElevation;
    qreal time;
};

KDynamicWallpaperMetaDataPrivate::KDynamicWallpaperMetaDataPrivate()
    : crossFadeMode(KDynamicWallpaperMetaData::NoCrossFade)
    , solarAzimuth(0.0)
    , solarElevation(0.0)
    , time(0.0)
{
}

/*!
 * Constructs an empty KDynamicWallpaperMetaData object.
 */
KDynamicWallpaperMetaData::KDynamicWallpaperMetaData()
    : d(new KDynamicWallpaperMetaDataPrivate)
{
}

/*!
 * Constructs a copy of the KDynamicWallpaperMetaData object.
 */
KDynamicWallpaperMetaData::KDynamicWallpaperMetaData(const KDynamicWallpaperMetaData &other)
    : d(other.d)
{
}

/*!
 * Destructs the KDynamicWallpaperMetaData object.
 */
KDynamicWallpaperMetaData::~KDynamicWallpaperMetaData()
{
}

/*!
 * Assigns the value of \p other to a dynamic wallpaper metadata object.
 */
KDynamicWallpaperMetaData &KDynamicWallpaperMetaData::operator=(const KDynamicWallpaperMetaData &other)
{
    d = other.d;
    return *this;
}

/*!
 * Returns a bitmask that indicates which fields are present in the metadata.
 */
KDynamicWallpaperMetaData::MetaDataFields KDynamicWallpaperMetaData::fields() const
{
    return d->presentFields;
}

/*!
 * Returns \c true if the KDynamicWallpaperMetaData contains valid metadata; otherwise \c false.
 */
bool KDynamicWallpaperMetaData::isValid() const
{
    const MetaDataFields requiredFields = TimeField;
    if ((d->presentFields & requiredFields) != requiredFields)
        return false;

    if (bool(d->presentFields & SolarAzimuthField) ^ bool(d->presentFields & SolarElevationField))
        return false;

    if (d->time < 0 || d->time > 1)
        return false;

    return true;
}

/*!
 * Sets the value of the cross-fade mode field to \p mode.
 */
void KDynamicWallpaperMetaData::setCrossFadeMode(CrossFadeMode mode)
{
    d->crossFadeMode = mode;
    d->presentFields |= CrossFadeField;
}

/*!
 * Returns the value of the cross-fade mode field in the dynamic wallpaper metadata.
 */
KDynamicWallpaperMetaData::CrossFadeMode KDynamicWallpaperMetaData::crossFadeMode() const
{
    return d->crossFadeMode;
}

/*!
 * Sets the value of the time field to \p time.
 */
void KDynamicWallpaperMetaData::setTime(qreal time)
{
    d->time = time;
    d->presentFields |= TimeField;
}

/*!
 * Returns the value of the time field in the dynamic wallpaper metadata.
 */
qreal KDynamicWallpaperMetaData::time() const
{
    return d->time;
}

/*!
 * Sets the value of the solar elevation field to \p elevation.
 */
void KDynamicWallpaperMetaData::setSolarElevation(qreal elevation)
{
    d->solarElevation = elevation;
    d->presentFields |= SolarElevationField;
}

/*!
 * Returns the value of solar elevation stored in the dynamic wallpaper metadata.
 *
 * Note that this method will return \c 0 if SolarElevationField is not set in fields().
 */
qreal KDynamicWallpaperMetaData::solarElevation() const
{
    return d->solarElevation;
}

/*!
 * Sets the value of the solar azimuth field to \p azimuth.
 */
void KDynamicWallpaperMetaData::setSolarAzimuth(qreal azimuth)
{
    d->solarAzimuth = azimuth;
    d->presentFields |= SolarAzimuthField;
}

/*!
 * Returns the value of solar azimuth stored in the dynamic wallpaper metadata.
 *
 * Note that this method will return \c 0 if SolarAzimuthField is not set in fields().
 */
qreal KDynamicWallpaperMetaData::solarAzimuth() const
{
    return d->solarAzimuth;
}

/*!
 * Converts the KDynamicWallpaperMetaData to a UTF-8 encoded JSON document.
 *
 * This method returns an empty QByteArray if the metadata is invalid.
 */
QByteArray KDynamicWallpaperMetaData::toJson() const
{
    if (!isValid())
        return QByteArray();

    QJsonObject rootObject;

    if (d->presentFields & CrossFadeField)
        rootObject[QLatin1String("CrossFade")] = crossFadeModeToJson(d->crossFadeMode);
    if (d->presentFields & SolarElevationField)
        rootObject[QLatin1String("Elevation")] = d->solarElevation;
    if (d->presentFields & SolarAzimuthField)
        rootObject[QLatin1String("Azimuth")] = d->solarAzimuth;
    rootObject[QLatin1String("Time")] = d->time;

    QJsonDocument document;
    document.setObject(rootObject);

    return document.toJson(QJsonDocument::Compact);
}

/*!
 * Converts the KDynamicWallpaperMetaData to a Base64 string.
 */
QByteArray KDynamicWallpaperMetaData::toBase64() const
{
    return toJson().toBase64();
}

/*!
 * Converts the KDynamicWallpaperMetaData to a Base64 string, and stores it in XMP metadata.
 */
QByteArray KDynamicWallpaperMetaData::toXmp() const
{
    if (!isValid())
        return QByteArray();

    QFile xmpTemplateFile(QStringLiteral(":/kdynamicwallpaper/xmp/metadata.xml"));
    xmpTemplateFile.open(QFile::ReadOnly);

    QByteArray xmpMetaData = xmpTemplateFile.readAll();
    xmpMetaData.replace(QByteArrayLiteral("base64"), toBase64());

    return xmpMetaData;
}

/*!
 * Decodes a JSON-encoded KDynamicWallpaperMetaData object.
 */
KDynamicWallpaperMetaData KDynamicWallpaperMetaData::fromJson(const QByteArray &json)
{
    KDynamicWallpaperMetaData metaData;

    const QJsonDocument document = QJsonDocument::fromJson(json);
    if (document.isNull())
        return metaData;

    const QJsonObject rootObject = document.object();
    if (rootObject.isEmpty())
        return metaData;

    const QJsonValue crossFadeMode = rootObject[QLatin1String("CrossFade")];
    if (crossFadeMode.isBool())
        metaData.setCrossFadeMode(crossFadeModeFromJson(crossFadeMode));

    const QJsonValue time = rootObject[QLatin1String("Time")];
    if (time.isDouble())
        metaData.setTime(time.toDouble());

    const QJsonValue solarElevation = rootObject[QLatin1String("Elevation")];
    if (solarElevation.isDouble())
        metaData.setSolarElevation(solarElevation.toDouble());

    const QJsonValue solarAzimuth = rootObject[QLatin1String("Azimuth")];
    if (solarAzimuth.isDouble())
        metaData.setSolarAzimuth(solarAzimuth.toDouble());

    return metaData;
}

/*!
 * Decodes a Base64-encoded KDynamicWallpaperMetaData object.
 */
KDynamicWallpaperMetaData KDynamicWallpaperMetaData::fromBase64(const QByteArray &base64)
{
    return fromJson(QByteArray::fromBase64(base64));
}

/*!
 * Creates a KDynamicWallpaperMetaData object from the specified \p xmp metadata.
 */
KDynamicWallpaperMetaData KDynamicWallpaperMetaData::fromXmp(const QByteArray &xmp)
{
    QDomDocument xmpDocument;
    xmpDocument.setContent(xmp);
    if (xmpDocument.isNull())
        return KDynamicWallpaperMetaData();

    const QString attributeName = QStringLiteral("plasma:DynamicWallpaper");

    const QDomNodeList descriptionNodes = xmpDocument.elementsByTagName(QStringLiteral("rdf:Description"));
    for (int i = 0; i < descriptionNodes.count(); ++i) {
        QDomElement descriptionNode = descriptionNodes.at(i).toElement();
        const QString encodedMetaData = descriptionNode.attribute(attributeName);
        if (encodedMetaData.isEmpty())
            continue;
        KDynamicWallpaperMetaData metaData = fromBase64(encodedMetaData.toUtf8());
        if (metaData.isValid())
            return metaData;
    }

    return KDynamicWallpaperMetaData();
}
