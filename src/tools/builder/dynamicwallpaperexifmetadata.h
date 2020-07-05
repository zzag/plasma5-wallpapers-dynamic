/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QDateTime>
#include <QSharedDataPointer>
#include <QString>

class DynamicWallpaperExifMetaDataPrivate;

class DynamicWallpaperExifMetaData
{
public:
    enum MetaDataField {
        BirthDateTimeField = 0x1,
        SolarCoordinatesField = 0x2,
    };
    Q_DECLARE_FLAGS(MetaDataFields, MetaDataField)

    explicit DynamicWallpaperExifMetaData(const QString &fileName);
    DynamicWallpaperExifMetaData(const DynamicWallpaperExifMetaData &other);
    ~DynamicWallpaperExifMetaData();

    DynamicWallpaperExifMetaData &operator=(const DynamicWallpaperExifMetaData &other);

    QDateTime birthDateTime() const;
    qreal solarAzimuth() const;
    qreal solarElevation() const;

    MetaDataFields fields() const;
    bool isValid() const;

private:
    QSharedDataPointer<DynamicWallpaperExifMetaDataPrivate> d;
};
