/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <KDynamicWallpaperMetaData>

#include <QJsonArray>
#include <QJsonObject>
#include <QImage>
#include <QString>

class DynamicWallpaperDescriptionReaderPrivate;

class DynamicWallpaperDescriptionReader
{
public:
    explicit DynamicWallpaperDescriptionReader(const QString &fileName);
    ~DynamicWallpaperDescriptionReader();

    bool atEnd() const;
    bool readNext();

    QImage image() const;
    KDynamicWallpaperMetaData metaData() const;
    bool isPrimary() const;

    bool hasError() const;
    QString errorString() const;

private:
    QScopedPointer<DynamicWallpaperDescriptionReaderPrivate> d;
};
