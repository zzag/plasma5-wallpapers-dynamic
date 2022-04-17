/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperimageprovider.h"
#include "dynamicwallpaperglobals.h"
#include "dynamicwallpaperimagehandle.h"

#include <KDynamicWallpaperReader>

#include <QFutureWatcher>
#include <QtConcurrent>

static DynamicWallpaperImageAsyncResult load(const QString &fileName,
                                             int index,
                                             const QSize &requestedSize,
                                             const QQuickImageProviderOptions &options)
{
    const KDynamicWallpaperReader reader(fileName);
    if (reader.error() != KDynamicWallpaperReader::NoError)
        return DynamicWallpaperImageAsyncResult(reader.errorString());

    const QImage image = reader.image(index);
    const QSize effectiveSize = QQuickImageProviderWithOptions::loadSize(image.size(),
                                                                         requestedSize,
                                                                         QByteArrayLiteral("avif"),
                                                                         options);

    return DynamicWallpaperImageAsyncResult(image.scaled(effectiveSize,
                                                         Qt::IgnoreAspectRatio,
                                                         Qt::SmoothTransformation));
}

class DynamicWallpaperAsyncImageResponse : public QQuickImageResponse
{
public:
    DynamicWallpaperAsyncImageResponse(const QString &fileName,
                                       int index,
                                       const QSize &requestedSize,
                                       const QQuickImageProviderOptions &options);

    QQuickTextureFactory *textureFactory() const override;
    QString errorString() const override;

private Q_SLOTS:
    void handleFinished();

private:
    QFutureWatcher<DynamicWallpaperImageAsyncResult> *m_watcher;
    QImage m_image;
    QString m_errorString;
};

DynamicWallpaperAsyncImageResponse::DynamicWallpaperAsyncImageResponse(const QString &fileName,
                                                                       int index,
                                                                       const QSize &requestedSize,
                                                                       const QQuickImageProviderOptions &options)
{
    m_watcher = new QFutureWatcher<DynamicWallpaperImageAsyncResult>(this);
    connect(m_watcher, &QFutureWatcher<DynamicWallpaperImageAsyncResult>::finished,
            this, &DynamicWallpaperAsyncImageResponse::handleFinished);
    m_watcher->setFuture(QtConcurrent::run(load, fileName, index, requestedSize, options));
}

void DynamicWallpaperAsyncImageResponse::handleFinished()
{
    const DynamicWallpaperImageAsyncResult result = m_watcher->result();

    if (result.errorString.isEmpty())
        m_image = result.image;
    else
        m_errorString = result.errorString;

    Q_EMIT finished();
}

QQuickTextureFactory *DynamicWallpaperAsyncImageResponse::textureFactory() const
{
    return QQuickTextureFactory::textureFactoryForImage(m_image);
}

QString DynamicWallpaperAsyncImageResponse::errorString() const
{
    return m_errorString;
}

DynamicWallpaperImageProvider::DynamicWallpaperImageProvider()
    : QQuickImageProviderWithOptions(ImageType::ImageResponse)
{
}

QQuickImageResponse *DynamicWallpaperImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize, const QQuickImageProviderOptions &options)
{
    const DynamicWallpaperImageHandle handle = DynamicWallpaperImageHandle::fromString(id);
    return new DynamicWallpaperAsyncImageResponse(handle.fileName(), handle.imageIndex(), requestedSize, options);
}
