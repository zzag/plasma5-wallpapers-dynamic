/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <KSolarDynamicWallpaperMetaData>
#include <KDynamicWallpaperWriter>

#include <QJsonArray>
#include <QJsonObject>
#include <QString>

class DynamicWallpaperDescription
{
public:
    explicit DynamicWallpaperDescription(const QString &fileName);
    ~DynamicWallpaperDescription();

    QList<KSolarDynamicWallpaperMetaData> metaData() const;
    QList<KDynamicWallpaperWriter::ImageView> images() const;

    bool hasError() const;
    QString errorString() const;

private:
    void init(const QString &metaDataFileName);
    void setError(const QString &text);

    QList<KSolarDynamicWallpaperMetaData> m_metaDataList;
    QList<KDynamicWallpaperWriter::ImageView> m_imageList;
    QString m_errorString;
    bool m_hasError = false;
};
