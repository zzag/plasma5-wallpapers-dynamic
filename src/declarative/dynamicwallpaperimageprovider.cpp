/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperimageprovider.h"
#include "dynamicwallpaperglobals.h"
#include "dynamicwallpaperimagehandle.h"

#include <KDynamicWallpaperReader>

#include <QtConcurrent>
#include <QFutureWatcher>

static DynamicWallpaperImageAsyncResult load(const QString &fileName, int index, const QSize &requestedSize)
{
    const KDynamicWallpaperReader reader(fileName);
    QImage image = reader.imageAt(index);

    if (reader.error() != KDynamicWallpaperReader::NoError)
        return DynamicWallpaperImageAsyncResult(reader.errorString());

    // If the requested image size is valid, scale the image without preserving the
    // aspect ratio.
    if (requestedSize.isValid())
        image = image.scaled(requestedSize);

    // QtQuick wants images to have the format of ARGB32_Premultiplied, so perform
    // format conversion in the worker thread right away.
    if (image.format() != QImage::Format_ARGB32_Premultiplied)
        image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    return DynamicWallpaperImageAsyncResult(image);
}

class DynamicWallpaperAsyncImageResponse : public QQuickImageResponse
{
public:
    DynamicWallpaperAsyncImageResponse(const QString &fileName, int index, const QSize &requestedSize);

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
                                                                       const QSize &requestedSize)
{
    m_watcher = new QFutureWatcher<DynamicWallpaperImageAsyncResult>(this);
    m_watcher->setFuture(QtConcurrent::run(load, fileName, index, requestedSize));

    connect(m_watcher, &QFutureWatcher<DynamicWallpaperImageAsyncResult>::finished,
            this, &DynamicWallpaperAsyncImageResponse::handleFinished);
}

void DynamicWallpaperAsyncImageResponse::handleFinished()
{
    const DynamicWallpaperImageAsyncResult result = m_watcher->result();

    if (result.errorString.isEmpty())
        m_image = result.image;
    else
        m_errorString = result.errorString;

    emit finished();
}

QQuickTextureFactory *DynamicWallpaperAsyncImageResponse::textureFactory() const
{
    return QQuickTextureFactory::textureFactoryForImage(m_image);
}

QString DynamicWallpaperAsyncImageResponse::errorString() const
{
    return m_errorString;
}

QQuickImageResponse *DynamicWallpaperImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    const DynamicWallpaperImageHandle handle = DynamicWallpaperImageHandle::fromString(id);
    return new DynamicWallpaperAsyncImageResponse(handle.fileName(), handle.imageIndex(), requestedSize);
}
