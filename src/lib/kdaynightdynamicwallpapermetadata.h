/*
 * SPDX-FileCopyrightText: 2022 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#pragma once

#include "kdynamicwallpaper_export.h"

#include <QByteArray>
#include <QJsonObject>
#include <QSharedDataPointer>

class KDayNightDynamicWallpaperMetaDataPrivate;

class KDYNAMICWALLPAPER_EXPORT KDayNightDynamicWallpaperMetaData
{
public:
    enum class TimeOfDay {
        Day = 1,
        Night = 2,
    };

    enum MetaDataField {
        TimeOfDayField = 1 << 0,
        IndexField = 1 << 1,
    };
    Q_DECLARE_FLAGS(MetaDataFields, MetaDataField)

    KDayNightDynamicWallpaperMetaData();
    KDayNightDynamicWallpaperMetaData(TimeOfDay timeOfDay, int index);
    KDayNightDynamicWallpaperMetaData(const KDayNightDynamicWallpaperMetaData &other);
    ~KDayNightDynamicWallpaperMetaData();

    KDayNightDynamicWallpaperMetaData &operator=(const KDayNightDynamicWallpaperMetaData &other);

    MetaDataFields fields() const;
    bool isValid() const;

    void setTimeOfDay(TimeOfDay tod);
    TimeOfDay timeOfDay() const;

    void setIndex(int index);
    int index() const;

    QJsonObject toJson() const;

    static KDayNightDynamicWallpaperMetaData fromJson(const QJsonObject &object);

private:
    QSharedDataPointer<KDayNightDynamicWallpaperMetaDataPrivate> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KDayNightDynamicWallpaperMetaData::MetaDataFields)
