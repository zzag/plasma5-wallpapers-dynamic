/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperpreviewprovider.h"

#include <KIO/PreviewJob>

#include <QGuiApplication>

class AsyncImageResponse : public QQuickImageResponse
{
public:
    AsyncImageResponse(const QString &fileName, const QSize &requestedSize);

    QString errorString() const override;
    QQuickTextureFactory *textureFactory() const override;

private Q_SLOTS:
    void handleResult(const KFileItem &fileItem, const QPixmap &pixmap);
    void handleError();

private:
    QString m_errorString;
    QImage m_image;
};

AsyncImageResponse::AsyncImageResponse(const QString &fileName, const QSize &requestedSize)
{
    KFileItemList fileItems;
    fileItems << QUrl::fromLocalFile(fileName);

    const QStringList enabledPlugins = KIO::PreviewJob::availablePlugins();

    QSize desiredSize = requestedSize;
    if (desiredSize.isEmpty())
        desiredSize = QSize(400, 250) * qApp->devicePixelRatio();

    KIO::PreviewJob *jerb = KIO::filePreview(fileItems, desiredSize, &enabledPlugins);
    jerb->setIgnoreMaximumSize(true);

    connect(jerb, &KIO::PreviewJob::gotPreview, this, &AsyncImageResponse::handleResult);
    connect(jerb, &KIO::PreviewJob::failed, this, &AsyncImageResponse::handleError);
}

QString AsyncImageResponse::errorString() const
{
    return m_errorString;
}

QQuickTextureFactory *AsyncImageResponse::textureFactory() const
{
    return QQuickTextureFactory::textureFactoryForImage(m_image);
}

void AsyncImageResponse::handleResult(const KFileItem &fileItem, const QPixmap &pixmap)
{
    Q_UNUSED(fileItem)
    m_image = pixmap.toImage();
    emit finished();
}

void AsyncImageResponse::handleError()
{
    m_errorString = QStringLiteral("Unknown error");
    emit finished();
}

static QString fileNameFromBase64(const QString &base64)
{
    return QByteArray::fromBase64(base64.toUtf8());
}

QQuickImageResponse *DynamicWallpaperPreviewProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    return new AsyncImageResponse(fileNameFromBase64(id), requestedSize);
}
