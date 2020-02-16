/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QGeoCoordinate>
#include <QObject>
#include <QTimer>
#include <QUrl>

#include <memory>

class DynamicWallpaperModel;
class DynamicWallpaperPackage;

class DynamicWallpaperHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl bottomLayer READ bottomLayer NOTIFY bottomLayerChanged)
    Q_PROPERTY(QUrl topLayer READ topLayer NOTIFY topLayerChanged)
    Q_PROPERTY(qreal blendFactor READ blendFactor NOTIFY blendFactorChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString wallpaperId READ wallpaperId WRITE setWallpaperId NOTIFY wallpaperIdChanged)
    Q_PROPERTY(QGeoCoordinate location READ location WRITE setLocation NOTIFY locationChanged)

public:
    enum Status {
        Ok,
        Error
    };
    Q_ENUM(Status)

    explicit DynamicWallpaperHandler(QObject *parent = nullptr);
    ~DynamicWallpaperHandler() override;

    QUrl bottomLayer() const;
    QUrl topLayer() const;
    qreal blendFactor() const;
    Status status() const;
    QString error() const;

    QString wallpaperId() const;
    void setWallpaperId(const QString &id);

    QGeoCoordinate location() const;
    void setLocation(const QGeoCoordinate &location);

Q_SIGNALS:
    void bottomLayerChanged();
    void topLayerChanged();
    void blendFactorChanged();
    void statusChanged();
    void errorChanged();
    void wallpaperIdChanged();
    void locationChanged();

public Q_SLOTS:
    void update();

private:
    void setBottomLayer(const QUrl &url);
    void setTopLayer(const QUrl &url);
    void setBlendFactor(qreal factor);
    void setStatus(Status status);
    void setError(const QString &error);
    void reloadModel();
    void reloadWallpaper();
    void scheduleUpdate();

    QTimer *m_scheduleTimer = nullptr;
    Status m_status = Status::Ok;
    QString m_error;
    QString m_wallpaperId;
    std::shared_ptr<DynamicWallpaperPackage> m_wallpaper;
    std::unique_ptr<DynamicWallpaperModel> m_model;
    QUrl m_bottomLayer;
    QUrl m_topLayer;
    QGeoCoordinate m_location;
    qreal m_blendFactor = 0.0;

    Q_DISABLE_COPY(DynamicWallpaperHandler)
};
