/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "kdynamicwallpapermetadata.h"

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
    int index;
};

KDynamicWallpaperMetaDataPrivate::KDynamicWallpaperMetaDataPrivate()
    : crossFadeMode(KDynamicWallpaperMetaData::NoCrossFade)
    , solarAzimuth(0.0)
    , solarElevation(0.0)
    , time(0.0)
    , index(-1)
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
    const MetaDataFields requiredFields = TimeField | IndexField;
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
 * Sets the index of the associated wallpaper image to \p index.
 */
void KDynamicWallpaperMetaData::setIndex(int index)
{
    d->index = index;
    d->presentFields |= IndexField;
}

/*!
 * Returns the index of the associated wallpaper image.
 */
int KDynamicWallpaperMetaData::index() const
{
    return d->index;
}

/*!
 * Converts the KDynamicWallpaperMetaData to a UTF-8 encoded JSON document.
 *
 * This method returns an empty QJsonObject if the metadata is invalid.
 */
QJsonObject KDynamicWallpaperMetaData::toJson() const
{
    if (!isValid())
        return QJsonObject();

    QJsonObject object;

    if (d->presentFields & CrossFadeField)
        object[QLatin1String("CrossFade")] = crossFadeModeToJson(d->crossFadeMode);
    if (d->presentFields & SolarElevationField)
        object[QLatin1String("Elevation")] = d->solarElevation;
    if (d->presentFields & SolarAzimuthField)
        object[QLatin1String("Azimuth")] = d->solarAzimuth;
    object[QLatin1String("Time")] = d->time;
    object[QLatin1String("Index")] = d->index;

    return object;
}

/*!
 * Decodes a JSON-encoded KDynamicWallpaperMetaData object.
 */
KDynamicWallpaperMetaData KDynamicWallpaperMetaData::fromJson(const QJsonObject &object)
{
    KDynamicWallpaperMetaData metaData;

    const QJsonValue index = object[QLatin1String("Index")];
    if (index.isDouble())
        metaData.setIndex(index.toInt());

    const QJsonValue crossFadeMode = object[QLatin1String("CrossFade")];
    if (crossFadeMode.isBool())
        metaData.setCrossFadeMode(crossFadeModeFromJson(crossFadeMode));

    const QJsonValue time = object[QLatin1String("Time")];
    if (time.isDouble())
        metaData.setTime(time.toDouble());

    const QJsonValue solarElevation = object[QLatin1String("Elevation")];
    if (solarElevation.isDouble())
        metaData.setSolarElevation(solarElevation.toDouble());

    const QJsonValue solarAzimuth = object[QLatin1String("Azimuth")];
    if (solarAzimuth.isDouble())
        metaData.setSolarAzimuth(solarAzimuth.toDouble());

    return metaData;
}
