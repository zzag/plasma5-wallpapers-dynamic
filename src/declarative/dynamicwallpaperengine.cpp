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
