/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperpreviewprovider.h"
#include "dynamicwallpaperpreviewjob.h"

#include <QGuiApplication>

class AsyncImageResponse : public QQuickImageResponse
{
public:
    AsyncImageResponse(const QString &fileName, const QSize &requestedSize);

    QString errorString() const override;
    QQuickTextureFactory *textureFactory() const override;

private Q_SLOTS:
    void handleFinished(const QImage &image);
    void handleFailed(const QString &errorString);

private:
    QString m_errorString;
    QImage m_image;
};

AsyncImageResponse::AsyncImageResponse(const QString &fileName, const QSize &requestedSize)
{
    QSize desiredSize = requestedSize;
    if (desiredSize.isEmpty())
        desiredSize = QSize(400, 250) * qApp->devicePixelRatio();

    DynamicWallpaperPreviewJob *job = new DynamicWallpaperPreviewJob(fileName, desiredSize);

    connect(job, &DynamicWallpaperPreviewJob::finished, this, &AsyncImageResponse::handleFinished);
    connect(job, &DynamicWallpaperPreviewJob::failed, this, &AsyncImageResponse::handleFailed);
}

QString AsyncImageResponse::errorString() const
{
    return m_errorString;
}

QQuickTextureFactory *AsyncImageResponse::textureFactory() const
{
    return QQuickTextureFactory::textureFactoryForImage(m_image);
}

void AsyncImageResponse::handleFinished(const QImage &image)
{
    m_image = image;
    Q_EMIT finished();
}

void AsyncImageResponse::handleFailed(const QString &errorString)
{
    m_errorString = errorString;
    Q_EMIT finished();
}

static QString fileNameFromBase64(const QString &base64)
{
    return QString::fromUtf8(QByteArray::fromBase64(base64.toUtf8()));
}

QQuickImageResponse *DynamicWallpaperPreviewProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    return new AsyncImageResponse(fileNameFromBase64(id), requestedSize);
}
