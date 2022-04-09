/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "dynamicwallpaperdescription.h"

#include <QDateTime>
#include <QMap>

class DynamicWallpaperEngine
{
public:
    virtual ~DynamicWallpaperEngine();

    void setDescription(const DynamicWallpaperDescription &description);
    DynamicWallpaperDescription description() const;

    void update();

    QUrl bottomLayer() const;
    QUrl topLayer() const;
    qreal blendFactor() const;

    virtual bool isExpired() const;

protected:
    virtual qreal progressForMetaData(const KSolarDynamicWallpaperMetaData &metaData) const = 0;
    virtual qreal progressForDateTime(const QDateTime &dateTime) const = 0;

private:
    DynamicWallpaperDescription m_description;
    QMap<qreal, int> m_progressToImageIndex;
    QUrl m_topLayer;
    QUrl m_bottomLayer;
    qreal m_blendFactor;
};
