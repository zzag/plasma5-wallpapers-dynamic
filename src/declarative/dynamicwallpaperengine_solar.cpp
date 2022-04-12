/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperengine_solar.h"
#include "dynamicwallpaperimagehandle.h"

#include <cmath>

SolarDynamicWallpaperEngine::SolarDynamicWallpaperEngine(const QList<KDynamicWallpaperMetaData> &metadata,
                                                         const QUrl &source,
                                                         const KSunPath &sunPath,
                                                         const KSunPosition &midnight,
                                                         const QGeoCoordinate &location,
                                                         const QDateTime &dateTime)
    : m_mode(Mode::Normal)
    , m_source(source)
    , m_sunPath(sunPath)
    , m_midnight(midnight)
    , m_location(location)
    , m_dateTime(dateTime)
{
    for (const KDynamicWallpaperMetaData &md : metadata) {
        const auto &solar = std::get<KSolarDynamicWallpaperMetaData>(md);
        m_progressToMetaData.insert(progressForMetaData(solar), solar);
    }
}

SolarDynamicWallpaperEngine::SolarDynamicWallpaperEngine(const QList<KDynamicWallpaperMetaData> &metadata,
                                                         const QUrl &source,
                                                         const QDateTime &dateTime)
    : m_mode(Mode::Fallback)
    , m_source(source)
    , m_dateTime(dateTime)
{
    for (const KDynamicWallpaperMetaData &md : metadata) {
        const auto &solar = std::get<KSolarDynamicWallpaperMetaData>(md);
        m_progressToMetaData.insert(progressForMetaData(solar), solar);
    }
}

bool SolarDynamicWallpaperEngine::isExpired() const
{
    return m_dateTime.date() != QDate::currentDate();
}

static bool checkSolarMetadata(const QList<KDynamicWallpaperMetaData> &metadata)
{
    return std::all_of(metadata.begin(), metadata.end(), [](auto md) {
        const auto &solar = std::get<KSolarDynamicWallpaperMetaData>(md);
        return solar.fields() & (KSolarDynamicWallpaperMetaData::SolarAzimuthField | KSolarDynamicWallpaperMetaData::SolarElevationField);
    });
}

SolarDynamicWallpaperEngine *SolarDynamicWallpaperEngine::create(const QList<KDynamicWallpaperMetaData> &metadata,
                                                                 const QUrl &source,
                                                                 const QGeoCoordinate &location)
{
    const QDateTime dateTime = QDateTime::currentDateTime();

    if (location.isValid() && checkSolarMetadata(metadata)) {
        const KSunPosition midnight = KSunPosition::midnight(dateTime, location);
        if (midnight.isValid()) {
            const KSunPath path = KSunPath::create(dateTime, location);
            if (path.isValid())
                return new SolarDynamicWallpaperEngine(metadata, source, path, midnight, location, dateTime);
        }
    }

    return new SolarDynamicWallpaperEngine(metadata, source, dateTime);
}

qreal SolarDynamicWallpaperEngine::progressForMetaData(const KSolarDynamicWallpaperMetaData &metaData) const
{
    if (m_mode == Mode::Fallback) {
        return metaData.time();
    } else {
        const KSunPosition position(metaData.solarElevation(), metaData.solarAzimuth());
        return progressForPosition(position);
    }
}

qreal SolarDynamicWallpaperEngine::progressForDateTime(const QDateTime &dateTime) const
{
    if (m_mode == Mode::Fallback) {
        QDateTime midnight = dateTime;
        midnight.setTime(QTime());
        return midnight.secsTo(dateTime) / 86400.0;
    } else {
        const KSunPosition position(dateTime, m_location);
        return progressForPosition(position);
    }
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

    QMap<qreal, KSolarDynamicWallpaperMetaData>::iterator nextImage;
    QMap<qreal, KSolarDynamicWallpaperMetaData>::iterator currentImage;

    nextImage = m_progressToMetaData.upperBound(progress);
    if (nextImage == m_progressToMetaData.end())
        nextImage = m_progressToMetaData.begin();

    if (nextImage == m_progressToMetaData.begin())
        currentImage = std::prev(m_progressToMetaData.end());
    else
        currentImage = std::prev(nextImage);

    if (currentImage->crossFadeMode() == KSolarDynamicWallpaperMetaData::CrossFade) {
        m_blendFactor = computeBlendFactor(currentImage.key(), nextImage.key(), progress);
    } else {
        m_blendFactor = 0;
    }

    m_topLayer = DynamicWallpaperImageHandle(m_source.toLocalFile(), nextImage->index()).toUrl();
    m_bottomLayer = DynamicWallpaperImageHandle(m_source.toLocalFile(), currentImage->index()).toUrl();
}
