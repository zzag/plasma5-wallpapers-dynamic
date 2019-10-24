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

#pragma once

// Qt
#include <QObject>
#include <QTimer>
#include <QUrl>

// std
#include <memory>

class DynamicWallpaperModel;
class DynamicWallpaperPackage;

class DynamicWallpaper : public QObject {
    Q_OBJECT
    Q_PROPERTY(QUrl bottomLayer READ bottomLayer NOTIFY bottomLayerChanged)
    Q_PROPERTY(QUrl topLayer READ topLayer NOTIFY topLayerChanged)
    Q_PROPERTY(qreal blendFactor READ blendFactor NOTIFY blendFactorChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString wallpaperId READ wallpaperId WRITE setWallpaperId NOTIFY wallpaperIdChanged)
    Q_PROPERTY(qreal latitude READ latitude WRITE setLatitude NOTIFY latitudeChanged)
    Q_PROPERTY(qreal longitude READ longitude WRITE setLongitude NOTIFY longitudeChanged)

public:
    enum Status {
        Ok,
        Error
    };
    Q_ENUM(Status)

    explicit DynamicWallpaper(QObject* parent = nullptr);
    ~DynamicWallpaper() override;

    QUrl bottomLayer() const;
    QUrl topLayer() const;
    qreal blendFactor() const;
    Status status() const;
    QString error() const;

    QString wallpaperId() const;
    void setWallpaperId(const QString& id);

    qreal latitude() const;
    void setLatitude(qreal latitude);

    qreal longitude() const;
    void setLongitude(qreal longitude);

Q_SIGNALS:
    void bottomLayerChanged();
    void topLayerChanged();
    void blendFactorChanged();
    void statusChanged();
    void errorChanged();
    void wallpaperIdChanged();
    void latitudeChanged();
    void longitudeChanged();

public Q_SLOTS:
    void update();

private:
    void setBottomLayer(const QUrl& url);
    void setTopLayer(const QUrl& url);
    void setBlendFactor(qreal factor);
    void setStatus(Status status);
    void setError(const QString& error);
    void reloadModel();
    void reloadWallpaper();
    void scheduleUpdate();

    QTimer* m_scheduleTimer = nullptr;
    Status m_status = Status::Ok;
    QString m_error;
    QString m_wallpaperId;
    std::shared_ptr<DynamicWallpaperPackage> m_wallpaper;
    std::unique_ptr<DynamicWallpaperModel> m_model;
    QUrl m_bottomLayer;
    QUrl m_topLayer;
    qreal m_blendFactor = 0.0;
    qreal m_latitude = 0.0;
    qreal m_longitude = 0.0;

    Q_DISABLE_COPY(DynamicWallpaper)
};