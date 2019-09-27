/*
 * Copyright (C) 2019 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// Own
#include "DynamicWallpaperModel.h"
#include "DynamicWallpaperPackage.h"
#include "SunPosition.h"

// Qt
#include <QtMath>

// std
#include <algorithm>

const static qreal ARC_LENGTH = 2 * M_PI;

static qreal computeTime(const SunPath& path, const SunPosition& position)
{
    const QVector3D projected = path.project(position);

    const QVector3D v1 = path.midnight() - path.center();
    const QVector3D v2 = projected - path.center();

    const QVector3D cross = QVector3D::crossProduct(v1, v2);
    const float dot = QVector3D::dotProduct(v1, v2);
    const float det = QVector3D::dotProduct(path.normal(), cross);

    qreal angle = std::atan2(det, dot);
    if (angle < 0)
        angle += ARC_LENGTH;

    return angle / ARC_LENGTH;
}

DynamicWallpaperModel::DynamicWallpaperModel(const DynamicWallpaperPackage* wallpaper, qreal latitude, qreal longitude)
    : m_dateTime(QDateTime::currentDateTime())
    , m_latitude(latitude)
    , m_longitude(longitude)
    , m_time(0)
{
    m_sunPath = SunPath(m_dateTime, latitude, longitude);
    if (!m_sunPath.isValid())
        return;

    const QVector<WallpaperImage> images = wallpaper->images();
    for (const WallpaperImage& image : images) {
        qreal time;

        switch (wallpaper->type()) {
        case WallpaperType::Solar:
            time = computeTime(m_sunPath, image.position);
            break;
        case WallpaperType::Timed:
            time = image.time;
            break;
        default:
            Q_UNREACHABLE();
        }

        m_knots << Knot { time, image.url };
    }

    std::sort(m_knots.begin(), m_knots.end());

    update();
}

DynamicWallpaperModel::~DynamicWallpaperModel()
{
}

bool DynamicWallpaperModel::isExpired() const
{
    // Rebuild the model each hour.
    const QDateTime now = QDateTime::currentDateTime();
    return qAbs(now.secsTo(m_dateTime)) > 3600;
}

bool DynamicWallpaperModel::isValid() const
{
    return m_sunPath.isValid();
}

QUrl DynamicWallpaperModel::bottomLayer() const
{
    return currentBottomKnot().url;
}

QUrl DynamicWallpaperModel::topLayer() const
{
    return currentTopKnot().url;
}

qreal DynamicWallpaperModel::blendFactor() const
{
    const Knot from = currentBottomKnot();
    const Knot to = currentTopKnot();

    if (from < to)
        return (m_time - from.time) / (to.time - from.time);

    const qreal duration = 1 - from.time + to.time;

    if (from.time <= m_time)
        return (m_time - from.time) / duration;

    return (1 - from.time + m_time) / duration;
}

DynamicWallpaperModel::Knot DynamicWallpaperModel::currentBottomKnot() const
{
    const Knot dummy { m_time, QUrl() };

    if (dummy < m_knots.first())
        return m_knots.last();

    if (m_knots.last() <= dummy)
        return m_knots.last();

    auto it = std::lower_bound(m_knots.begin(), m_knots.end(), dummy);
    if (dummy < *it)
        return *std::prev(it);

    return *it;
}

DynamicWallpaperModel::Knot DynamicWallpaperModel::currentTopKnot() const
{
    const Knot dummy { m_time, QUrl() };

    if (dummy < m_knots.first())
        return m_knots.first();

    if (m_knots.last() <= dummy)
        return m_knots.first();

    auto it = std::lower_bound(m_knots.begin(), m_knots.end(), dummy);
    if (dummy < *it)
        return *it;

    return *std::next(it);
}

void DynamicWallpaperModel::update()
{
    const QDateTime now(QDateTime::currentDateTime());
    const SunPosition position(now, m_latitude, m_longitude);
    m_time = computeTime(m_sunPath, position);
}
