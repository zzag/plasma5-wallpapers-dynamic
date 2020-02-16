/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperhandler.h"
#include "dynamicwallpapermodel.h"
#include "dynamicwallpaperpackage.h"

#include <KLocalizedString>

DynamicWallpaperHandler::DynamicWallpaperHandler(QObject *parent)
    : QObject(parent)
{
    // The purpose of this timer is to compress multiple scheduled update requests.
    m_scheduleTimer = new QTimer(this);
    m_scheduleTimer->setSingleShot(true);
    m_scheduleTimer->setInterval(0);
    connect(m_scheduleTimer, &QTimer::timeout, this, &DynamicWallpaperHandler::update);
}

DynamicWallpaperHandler::~DynamicWallpaperHandler()
{
}

QUrl DynamicWallpaperHandler::bottomLayer() const
{
    return m_bottomLayer;
}

QUrl DynamicWallpaperHandler::topLayer() const
{
    return m_topLayer;
}

qreal DynamicWallpaperHandler::blendFactor() const
{
    return m_blendFactor;
}

DynamicWallpaperHandler::Status DynamicWallpaperHandler::status() const
{
    return m_status;
}

QString DynamicWallpaperHandler::error() const
{
    return m_error;
}

QString DynamicWallpaperHandler::wallpaperId() const
{
    return m_wallpaperId;
}

void DynamicWallpaperHandler::setWallpaperId(const QString &id)
{
    if (m_wallpaperId == id)
        return;
    m_wallpaperId = id;
    emit wallpaperIdChanged();
    reloadWallpaper();
    reloadModel();
    scheduleUpdate();
}

QGeoCoordinate DynamicWallpaperHandler::location() const
{
    return m_location;
}

void DynamicWallpaperHandler::setLocation(const QGeoCoordinate &location)
{
    if (m_location == location)
        return;
    m_location = location;
    emit locationChanged();
    reloadModel();
    scheduleUpdate();
}

void DynamicWallpaperHandler::update()
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

void DynamicWallpaperHandler::setBottomLayer(const QUrl &url)
{
    if (m_bottomLayer == url)
        return;
    m_bottomLayer = url;
    emit bottomLayerChanged();
}

void DynamicWallpaperHandler::setTopLayer(const QUrl &url)
{
    if (m_topLayer == url)
        return;
    m_topLayer = url;
    emit topLayerChanged();
}

void DynamicWallpaperHandler::setBlendFactor(qreal factor)
{
    if (m_blendFactor == factor)
        return;
    m_blendFactor = factor;
    emit blendFactorChanged();
}

void DynamicWallpaperHandler::setStatus(Status status)
{
    if (m_status == status)
        return;
    m_status = status;
    emit statusChanged();
}

void DynamicWallpaperHandler::setError(const QString &error)
{
    if (m_error == error)
        return;
    m_error = error;
    emit errorChanged();
}

void DynamicWallpaperHandler::reloadModel()
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

void DynamicWallpaperHandler::reloadWallpaper()
{
    m_wallpaper.reset();

    DynamicWallpaperLoader loader;
    if (!loader.load(m_wallpaperId)) {
        setError(i18n("Could not load dynamic wallpaper '%1': %2", m_wallpaperId, loader.errorText()));
        setStatus(Status::Error);
        return;
    }

    m_wallpaper = loader.wallpaper();
    setStatus(Status::Ok);
}

void DynamicWallpaperHandler::scheduleUpdate()
{
    m_scheduleTimer->start();
}
