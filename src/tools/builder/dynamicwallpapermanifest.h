/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <KDynamicWallpaperMetaData>
#include <KDynamicWallpaperWriter>

#include <QJsonArray>
#include <QJsonObject>
#include <QString>

class DynamicWallpaperManifest
{
public:
    explicit DynamicWallpaperManifest(const QString &fileName);
    ~DynamicWallpaperManifest();

    QList<KDynamicWallpaperMetaData> metaData() const;
    QList<KDynamicWallpaperWriter::ImageView> images() const;

    bool hasError() const;
    QString errorString() const;

private:
    QString resolveFileName(const QString &fileName) const;

    void init();
    void setError(const QString &text);
    void parseSolar(const QJsonArray &entries);
    void parseDayNight(const QJsonArray &entries);

    QString m_manifestFileName;
    QList<KDynamicWallpaperMetaData> m_metaDataList;
    QList<KDynamicWallpaperWriter::ImageView> m_imageList;
    QString m_errorString;
    bool m_hasError = false;
};
