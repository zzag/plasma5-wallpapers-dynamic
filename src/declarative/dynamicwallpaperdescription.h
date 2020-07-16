/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <KDynamicWallpaperMetaData>

#include <QString>
#include <QUrl>
#include <QVector>

class DynamicWallpaperDescription
{
public:
    enum EngineType {
        SolarEngine = 1 << 0,
        TimedEngine = 1 << 1,
    };
    Q_DECLARE_FLAGS(EngineTypes, EngineType)

    DynamicWallpaperDescription();

    bool isValid() const;
    EngineTypes supportedEngines() const;

    int imageCount() const;

    QUrl imageUrlAt(int imageIndex) const;
    KDynamicWallpaperMetaData metaDataAt(int imageIndex) const;

    static DynamicWallpaperDescription fromFile(const QString &fileName);

private:
    void addImage(const QUrl &url, const KDynamicWallpaperMetaData &metaData);

    QVector<KDynamicWallpaperMetaData> m_metaData;
    QVector<QUrl> m_imageUrls;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DynamicWallpaperDescription::EngineTypes)
