/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperengine_solar.h"

#include <cmath>

SolarDynamicWallpaperEngine::SolarDynamicWallpaperEngine(const DynamicWallpaperDescription &description,
                                                         const KSunPath &sunPath,
                                                         const KSunPosition &midnight,
                                                         const QGeoCoordinate &location,
                                                         const QDateTime &dateTime)
    : m_description(description)
    , m_sunPath(sunPath)
    , m_midnight(midnight)
    , m_location(location)
    , m_dateTime(dateTime)
{
    for (int i = 0; i < m_description.imageCount(); ++i) {
        const KDynamicWallpaperMetaData metaData = m_description.metaDataAt(i);
        m_progressToImageIndex.insert(progressForMetaData(metaData), i);
    }
}

bool SolarDynamicWallpaperEngine::isExpired() const
{
    return m_dateTime.date() != QDate::currentDate();
}

SolarDynamicWallpaperEngine *SolarDynamicWallpaperEngine::create(const DynamicWallpaperDescription &description,
                                                                 const QGeoCoordinate &location)
{
    const QDateTime dateTime = QDateTime::currentDateTime();

    const KSunPosition midnight = KSunPosition::midnight(dateTime, location);
    if (!midnight.isValid())
        return nullptr;

    const KSunPath path = KSunPath::create(dateTime, location);
    if (!path.isValid())
        return nullptr;

    return new SolarDynamicWallpaperEngine(description, path, midnight, location, dateTime);
}

qreal SolarDynamicWallpaperEngine::progressForMetaData(const KDynamicWallpaperMetaData &metaData) const
{
    const auto &solar = std::get<KSolarDynamicWallpaperMetaData>(metaData);
    const KSunPosition position(solar.solarElevation(), solar.solarAzimuth());
    return progressForPosition(position);
}

qreal SolarDynamicWallpaperEngine::progressForDateTime(const QDateTime &dateTime) const
{
    const KSunPosition position(dateTime, m_location);
    return progressForPosition(position);
}

qreal SolarDynamicWallpaperEngine::progressForPosition(const KSunPosition &position) const
{
    const QVector3D projectedMidnight = m_sunPath.project(m_midnight);
    const QVector3D projectedPosition = m_sunPath.project(position);

    const QVector3D v1 = (projectedMidnight - m_sunPath.center()).normalized();
    const QVector3D v2 = (projectedPosition - m_sunPath.center()).normalized();

    const QVector3D cross = QVector3D::crossProduct(v1, v2);
    const float dot = QVector3D::dotProduct(v1, v2);
    const float det = QVector3D::dotProduct(m_sunPath.normal(), cross);

    qreal angle = std::atan2(det, dot);
    if (angle < 0)
        angle += 2 * M_PI;

    return angle / (2 * M_PI);
}

static qreal computeTimeSpan(qreal from, qreal to)
{
    if (to < from)
        return (1 - from) + to;

    return to - from;
}

static qreal computeBlendFactor(qreal from, qreal to, qreal now)
{
    const qreal reflectedFrom = 1 - from;
    const qreal reflectedTo = 1 - to;

    const qreal totalDuration = computeTimeSpan(from, to);
    const qreal totalElapsed = computeTimeSpan(from, now);

    if ((reflectedFrom < from) ^ (reflectedTo < to)) {
        if (reflectedFrom < to) {
            const qreal threshold = computeTimeSpan(from, reflectedFrom);
            if (totalElapsed < threshold)
                return 0;
            return (totalElapsed - threshold) / (totalDuration - threshold);
        }
        if (from < reflectedTo) {
            const qreal threshold = computeTimeSpan(from, reflectedTo);
            if (threshold < totalElapsed)
                return 1;
            return totalElapsed / threshold;
        }
    }

    return totalElapsed / totalDuration;
}

void SolarDynamicWallpaperEngine::update()
{
    const qreal progress = progressForDateTime(QDateTime::currentDateTime());

    QMap<qreal, int>::iterator nextImage;
    QMap<qreal, int>::iterator currentImage;

    nextImage = m_progressToImageIndex.upperBound(progress);
    if (nextImage == m_progressToImageIndex.end())
        nextImage = m_progressToImageIndex.begin();

    if (nextImage == m_progressToImageIndex.begin())
        currentImage = std::prev(m_progressToImageIndex.end());
    else
        currentImage = std::prev(nextImage);

    const auto metadata = m_description.metaDataAt(*currentImage);
    if (const auto solar = std::get_if<KSolarDynamicWallpaperMetaData>(&metadata); solar && solar->crossFadeMode() == KSolarDynamicWallpaperMetaData::CrossFade) {
        m_topLayer = m_description.imageUrlAt(*nextImage);
        m_blendFactor = computeBlendFactor(currentImage.key(), nextImage.key(), progress);
    } else {
        m_topLayer = QUrl();
        m_blendFactor = 0;
    }

    m_bottomLayer = m_description.imageUrlAt(*currentImage);
}
