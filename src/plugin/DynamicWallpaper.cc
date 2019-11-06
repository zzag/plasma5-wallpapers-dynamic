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
#include "DynamicWallpaper.h"
#include "DynamicWallpaperModel.h"
#include "DynamicWallpaperPackage.h"

// KF
#include <KLocalizedString>

DynamicWallpaper::DynamicWallpaper(QObject *parent)
    : QObject(parent)
{
    // The purpose of this timer is to compress multiple scheduled update requests.
    m_scheduleTimer = new QTimer(this);
    m_scheduleTimer->setSingleShot(true);
    m_scheduleTimer->setInterval(0);
    connect(m_scheduleTimer, &QTimer::timeout, this, &DynamicWallpaper::update);
}

DynamicWallpaper::~DynamicWallpaper()
{
}

QUrl DynamicWallpaper::bottomLayer() const
{
    return m_bottomLayer;
}

QUrl DynamicWallpaper::topLayer() const
{
    return m_topLayer;
}

qreal DynamicWallpaper::blendFactor() const
{
    return m_blendFactor;
}

DynamicWallpaper::Status DynamicWallpaper::status() const
{
    return m_status;
}

QString DynamicWallpaper::error() const
{
    return m_error;
}

QString DynamicWallpaper::wallpaperId() const
{
    return m_wallpaperId;
}

void DynamicWallpaper::setWallpaperId(const QString &id)
{
    if (m_wallpaperId == id)
        return;
    m_wallpaperId = id;
    emit wallpaperIdChanged();
    reloadWallpaper();
    reloadModel();
    scheduleUpdate();
}

qreal DynamicWallpaper::latitude() const
{
    return m_location.latitude();
}

void DynamicWallpaper::setLatitude(qreal latitude)
{
    if (m_location.latitude() == latitude)
        return;
    m_location.setLatitude(latitude);
    emit latitudeChanged();
    reloadModel();
    scheduleUpdate();
}

qreal DynamicWallpaper::longitude() const
{
    return m_location.longitude();
}

void DynamicWallpaper::setLongitude(qreal longitude)
{
    if (m_location.longitude() == longitude)
        return;
    m_location.setLongitude(longitude);
    emit longitudeChanged();
    reloadModel();
    scheduleUpdate();
}

void DynamicWallpaper::update()
{
    if (m_status == Status::Error)
        return;
    if (m_model->isExpired())
        reloadModel();
    if (m_status == Status::Error)
        return;
    m_model->update();
    setBottomLayer(m_model->bottomLayer());
    setTopLayer(m_model->topLayer());
    setBlendFactor(m_model->blendFactor());
}

void DynamicWallpaper::setBottomLayer(const QUrl &url)
{
    if (m_bottomLayer == url)
        return;
    m_bottomLayer = url;
    emit bottomLayerChanged();
}

void DynamicWallpaper::setTopLayer(const QUrl &url)
{
    if (m_topLayer == url)
        return;
    m_topLayer = url;
    emit topLayerChanged();
}

void DynamicWallpaper::setBlendFactor(qreal factor)
{
    if (m_blendFactor == factor)
        return;
    m_blendFactor = factor;
    emit blendFactorChanged();
}

void DynamicWallpaper::setStatus(Status status)
{
    if (m_status == status)
        return;
    m_status = status;
    emit statusChanged();
}

void DynamicWallpaper::setError(const QString &error)
{
    if (m_error == error)
        return;
    m_error = error;
    emit errorChanged();
}

void DynamicWallpaper::reloadModel()
{
    m_model.reset();

    if (!m_wallpaper)
        return;

    std::unique_ptr<DynamicWallpaperModel> model;

    switch (m_wallpaper->type()) {
    case WallpaperType::Solar:
        model.reset(SolarDynamicWallpaperModel::create(m_wallpaper, m_location));
        break;
    case WallpaperType::Timed:
        model.reset(TimedDynamicWallpaperModel::create(m_wallpaper));
        break;
    }

    if (!model) {
        setError(i18n("Not able to display the dynamic wallpaper. If you live close to the North or "
                      "the South geographic pole, try using another dynamic wallpaper."));
        setStatus(Status::Error);
        return;
    }

    m_model = std::move(model);
    m_model->update();

    setStatus(Status::Ok);
}

void DynamicWallpaper::reloadWallpaper()
{
    m_wallpaper.reset();

    DynamicWallpaperLoader loader;
    if (!loader.load(m_wallpaperId)) {
        setError(i18n("Could not load dynamic wallpaper: %1", loader.errorText()));
        setStatus(Status::Error);
        return;
    }

    m_wallpaper = loader.wallpaper();
    setStatus(Status::Ok);
}

void DynamicWallpaper::scheduleUpdate()
{
    m_scheduleTimer->start();
}
