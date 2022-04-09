/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <KDynamicWallpaperMetaData>

#include <QGeoCoordinate>
#include <QTimer>
#include <QUrl>

class DynamicWallpaperEngine;

class DynamicWallpaperHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QGeoCoordinate location READ location WRITE setLocation NOTIFY locationChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QUrl topLayer READ topLayer WRITE setTopLayer NOTIFY topLayerChanged)
    Q_PROPERTY(QUrl bottomLayer READ bottomLayer WRITE setBottomLayer NOTIFY bottomLayerChanged)
    Q_PROPERTY(qreal blendFactor READ blendFactor WRITE setBlendFactor NOTIFY blendFactorChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

public:
    enum Status { Null, Ready, Error };
    Q_ENUM(Status)

    explicit DynamicWallpaperHandler(QObject *parent = nullptr);
    ~DynamicWallpaperHandler() override;

    void setLocation(const QGeoCoordinate &coordinate);
    QGeoCoordinate location() const;

    void setSource(const QUrl &url);
    QUrl source() const;

    void setTopLayer(const QUrl &url);
    QUrl topLayer() const;

    void setBottomLayer(const QUrl &url);
    QUrl bottomLayer() const;

    void setBlendFactor(qreal blendFactor);
    qreal blendFactor() const;

    void setStatus(Status status);
    Status status() const;

    void setErrorString(const QString &text);
    QString errorString() const;

public Q_SLOTS:
    void scheduleUpdate();
    void update();

Q_SIGNALS:
    void locationChanged();
    void sourceChanged();
    void topLayerChanged();
    void bottomLayerChanged();
    void blendFactorChanged();
    void statusChanged();
    void errorStringChanged();

private:
    void reloadDescription();
    void reloadEngine();

    DynamicWallpaperEngine *m_engine = nullptr;
    QList<KDynamicWallpaperMetaData> m_metadata;
    QTimer *m_updateTimer;
    QGeoCoordinate m_location;
    QString m_errorString;
    QUrl m_source;
    QUrl m_topLayer;
    QUrl m_bottomLayer;
    qreal m_blendFactor = 0;
    Status m_status = Null;
};
