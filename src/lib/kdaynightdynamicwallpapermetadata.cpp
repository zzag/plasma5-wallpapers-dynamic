/*
 * SPDX-FileCopyrightText: 2022 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "kdaynightdynamicwallpapermetadata.h"

#include <QSharedData>

/*!
 * \class KDayNightDynamicWallpaperMetaData
 * \brief The KDayNightDynamicWallpaperMetaData class represents metadata associated with images
 * in the dynamic wallpaper.
 */

static QJsonValue timeOfDayToJson(KDayNightDynamicWallpaperMetaData::TimeOfDay tod)
{
    switch (tod) {
    case KDayNightDynamicWallpaperMetaData::TimeOfDay::Day:
        return QJsonValue(QStringLiteral("day"));
    case KDayNightDynamicWallpaperMetaData::TimeOfDay::Night:
        return QJsonValue(QStringLiteral("night"));
    default:
        Q_UNREACHABLE();
    }
}

static KDayNightDynamicWallpaperMetaData::TimeOfDay timeOfDayFromJson(const QJsonValue &value)
{
    static QHash<QString, KDayNightDynamicWallpaperMetaData::TimeOfDay> lookup = {
        { QStringLiteral("day"), KDayNightDynamicWallpaperMetaData::TimeOfDay::Day },
        { QStringLiteral("night"), KDayNightDynamicWallpaperMetaData::TimeOfDay::Night },
    };
    return lookup.value(value.toString());
}

class KDayNightDynamicWallpaperMetaDataPrivate : public QSharedData
{
public:
    KDayNightDynamicWallpaperMetaDataPrivate();

    KDayNightDynamicWallpaperMetaData::MetaDataFields presentFields;
    KDayNightDynamicWallpaperMetaData::TimeOfDay timeOfDay;
    int index;
};

KDayNightDynamicWallpaperMetaDataPrivate::KDayNightDynamicWallpaperMetaDataPrivate()
    : timeOfDay(KDayNightDynamicWallpaperMetaData::TimeOfDay::Day)
    , index(-1)
{
}

/*!
 * Constructs an empty KDayNightDynamicWallpaperMetaData object.
 */
KDayNightDynamicWallpaperMetaData::KDayNightDynamicWallpaperMetaData()
    : d(new KDayNightDynamicWallpaperMetaDataPrivate)
{
}

/*!
 * Constructs a KDayNightDynamicWallpaperMetaData object with the given \a timeOfDay and image \a index.
 */
KDayNightDynamicWallpaperMetaData::KDayNightDynamicWallpaperMetaData(TimeOfDay timeOfDay, int index)
    : KDayNightDynamicWallpaperMetaData()
{
    setTimeOfDay(timeOfDay);
    setIndex(index);
}

/*!
 * Constructs a copy of the KDayNightDynamicWallpaperMetaData object.
 */
KDayNightDynamicWallpaperMetaData::KDayNightDynamicWallpaperMetaData(const KDayNightDynamicWallpaperMetaData &other)
    : d(other.d)
{
}

/*!
 * Destructs the KDayNightDynamicWallpaperMetaData object.
 */
KDayNightDynamicWallpaperMetaData::~KDayNightDynamicWallpaperMetaData()
{
}

/*!
 * Assigns the value of \p other to a dynamic wallpaper metadata object.
 */
KDayNightDynamicWallpaperMetaData &KDayNightDynamicWallpaperMetaData::operator=(const KDayNightDynamicWallpaperMetaData &other)
{
    d = other.d;
    return *this;
}

/*!
 * Returns a bitmask that indicates which fields are present in the metadata.
 */
KDayNightDynamicWallpaperMetaData::MetaDataFields KDayNightDynamicWallpaperMetaData::fields() const
{
    return d->presentFields;
}

/*!
 * Returns \c true if the KDayNightDynamicWallpaperMetaData contains valid metadata; otherwise \c false.
 */
bool KDayNightDynamicWallpaperMetaData::isValid() const
{
    const MetaDataFields requiredFields = TimeOfDayField | IndexField;
    return (d->presentFields & requiredFields) == requiredFields;
}

/*!
 * Sets the time of day to \p tod.
 */
void KDayNightDynamicWallpaperMetaData::setTimeOfDay(TimeOfDay tod)
{
    d->timeOfDay = tod;
    d->presentFields |= TimeOfDayField;
}

/*!
 * Returns the value of the time of day field in the dynamic wallpaper metadata.
 */
KDayNightDynamicWallpaperMetaData::TimeOfDay KDayNightDynamicWallpaperMetaData::timeOfDay() const
{
    return d->timeOfDay;
}

/*!
 * Sets the index of the associated wallpaper image to \p index.
 */
void KDayNightDynamicWallpaperMetaData::setIndex(int index)
{
    d->index = index;
    d->presentFields |= IndexField;
}

/*!
 * Returns the index of the associated wallpaper image.
 */
int KDayNightDynamicWallpaperMetaData::index() const
{
    return d->index;
}

/*!
 * Converts the KDayNightDynamicWallpaperMetaData to a UTF-8 encoded JSON document.
 *
 * This method returns an empty QJsonObject if the metadata is invalid.
 */
QJsonObject KDayNightDynamicWallpaperMetaData::toJson() const
{
    if (!isValid())
        return QJsonObject();

    QJsonObject object;

    object[QLatin1String("TimeOfDay")] = timeOfDayToJson(d->timeOfDay);
    object[QLatin1String("Index")] = d->index;

    return object;
}

/*!
 * Decodes a JSON-encoded KDayNightDynamicWallpaperMetaData object.
 */
KDayNightDynamicWallpaperMetaData KDayNightDynamicWallpaperMetaData::fromJson(const QJsonObject &object)
{
    KDayNightDynamicWallpaperMetaData metaData;

    const QJsonValue index = object[QLatin1String("Index")];
    if (index.isDouble())
        metaData.setIndex(index.toInt());

    const QJsonValue timeOfDay = object[QLatin1String("TimeOfDay")];
    if (timeOfDay.isString())
        metaData.setTimeOfDay(timeOfDayFromJson(timeOfDay));

    return metaData;
}
