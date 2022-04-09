/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperengine_timed.h"

TimedDynamicWallpaperEngine *TimedDynamicWallpaperEngine::create(const DynamicWallpaperDescription &description)
{
    return new TimedDynamicWallpaperEngine(description);
}

TimedDynamicWallpaperEngine::TimedDynamicWallpaperEngine(const DynamicWallpaperDescription &description)
    : m_description(description)
{
    for (int i = 0; i < m_description.imageCount(); ++i) {
        const KDynamicWallpaperMetaData metaData = m_description.metaDataAt(i);
        m_progressToImageIndex.insert(progressForMetaData(metaData), i);
    }
}

qreal TimedDynamicWallpaperEngine::progressForMetaData(const KDynamicWallpaperMetaData &metaData) const
{
    return std::get<KSolarDynamicWallpaperMetaData>(metaData).time();
}

qreal TimedDynamicWallpaperEngine::progressForDateTime(const QDateTime &dateTime) const
{
    QDateTime midnight = dateTime;
    midnight.setTime(QTime());

    return midnight.secsTo(dateTime) / 86400.0;
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

void TimedDynamicWallpaperEngine::update()
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
