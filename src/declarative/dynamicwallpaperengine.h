/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QDateTime>
#include <QMap>
#include <QUrl>

class DynamicWallpaperEngine
{
public:
    virtual ~DynamicWallpaperEngine();

    virtual void update() = 0;
    virtual bool isExpired() const;

    QUrl bottomLayer() const;
    QUrl topLayer() const;
    qreal blendFactor() const;

protected:
    QUrl m_topLayer;
    QUrl m_bottomLayer;
    qreal m_blendFactor;
};
