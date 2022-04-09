/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperengine.h"

/*!
 * Destructs the DynamicWallpaperEngine object.
 */
DynamicWallpaperEngine::~DynamicWallpaperEngine()
{
}

/*!
 * Sets the DynamicWallpaperEngine's wallpaper description to \p description.
 *
 * You must call this method before starting using the engine.
 */
void DynamicWallpaperEngine::setDescription(const DynamicWallpaperDescription &description)
{
    m_progressToImageIndex.clear();
    m_description = description;

    for (int i = 0; i < m_description.imageCount(); ++i) {
        const KSolarDynamicWallpaperMetaData metaData = m_description.metaDataAt(i);
        m_progressToImageIndex.insert(progressForMetaData(metaData), i);
    }
}

/*!
 * Returns the DynamicWallpaperDescription object assigned to this engine.
 */
DynamicWallpaperDescription DynamicWallpaperEngine::description() const
{
    return m_description;
}

/*!
 * Returns \c true if the engine has been expired and must be rebuilt; otherwise returns \c false.
 */
bool DynamicWallpaperEngine::isExpired() const
{
    return false;
}

/*!
 * Returns the QUrl of the image that is currently being displayed in the top layer.
 */
QUrl DynamicWallpaperEngine::topLayer() const
{
    return m_topLayer;
}

/*!
 * Returns the QUrl of the image that is currently beind displayed in the bottom layer.
 */
QUrl DynamicWallpaperEngine::bottomLayer() const
{
    return m_bottomLayer;
}

/*!
 * Returns the blend factor between the bottom layer and the top layer.
 */
qreal DynamicWallpaperEngine::blendFactor() const
{
    return m_blendFactor;
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

/*!
 * Updates the internal state of the DynamicWallpaperEngine.
 */
void DynamicWallpaperEngine::update()
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

    const KSolarDynamicWallpaperMetaData currentMetaData = description().metaDataAt(*currentImage);
    if (currentMetaData.crossFadeMode() == KSolarDynamicWallpaperMetaData::CrossFade) {
        m_topLayer = description().imageUrlAt(*nextImage);
        m_blendFactor = computeBlendFactor(currentImage.key(), nextImage.key(), progress);
    } else {
        m_topLayer = QUrl();
        m_blendFactor = 0;
    }

    m_bottomLayer = description().imageUrlAt(*currentImage);
}
