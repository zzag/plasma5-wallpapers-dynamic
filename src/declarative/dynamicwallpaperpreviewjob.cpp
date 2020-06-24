/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperpreviewjob.h"
#include "dynamicwallpaperglobals.h"
#include "dynamicwallpaperpreviewcache.h"

#include <KDynamicWallpaperMetaData>
#include <KDynamicWallpaperReader>
#include <KLocalizedString>

#include <QtConcurrent>
#include <QtMath>
#include <QFutureWatcher>

/*!
 * \class DynamicWallpaperPreviewJob
 * \brief The DynamicWallpaperPreviewJob provides a convenient way for getting dynamic
 * wallpaper thumbnail images.
 *
 * The DynamicWallpaperPreviewJob allows to load or generate preview images for wallpapers
 * asynchronously. After a preview image has been generated, the finished() signal will be
 * emitted.
 *
 * If for whatever reason, the preview job wasn't able to successfully get a preview image,
 * the failed() signal will be emitted.
 *
 * After the finished() or the failed() signal has been emitted, the preview job object will
 * be destroyed automatically.
 */

class DynamicWallpaperPreviewJobPrivate
{
public:
    QFutureWatcher<DynamicWallpaperImageAsyncResult> *watcher;
};

static QRgb blend(QRgb a, QRgb b, qreal blendFactor)
{
    const int alpha = qAlpha(a) * (1 - blendFactor) + qAlpha(b) * blendFactor;
    const int red = qRed(a) * (1 - blendFactor) + qRed(b) * blendFactor;
    const int blue = qBlue(a) * (1 - blendFactor) + qBlue(b)* blendFactor;
    const int green = qGreen(a) * (1 - blendFactor) + qGreen(b) * blendFactor;

    return qRgba(red, green, blue, alpha);
}

static QImage ensureArgb32(const QImage &image)
{
    return image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
}

static QImage blend(const QImage &dark, const QImage &light, qreal delta)
{
    // Note that the dark and the light images may have different dimensions.
    const int width = std::max(dark.width(), light.width());
    const int height = std::max(dark.height(), light.height());

    const QImage a = ensureArgb32(dark.scaled(width, height));
    const QImage b = ensureArgb32(light.scaled(width, height));

    const QEasingCurve blendCurve(QEasingCurve::InOutQuad);
    const int blendFrom = std::floor(width * (1 - delta) / 2);
    const int blendTo = std::ceil(width * (1 + delta) / 2);

    QVector<qreal> blendFactorTable(width);
    for (int i = 0; i < width; ++i) {
        const qreal progress = qreal(i - blendFrom) / (blendTo - blendFrom);
        blendFactorTable[i] = blendCurve.valueForProgress(progress);
    }

    QImage result(width, height, QImage::Format_ARGB32_Premultiplied);

    for (int i = 0; i < height; ++i) {
        const uint32_t *in0 = reinterpret_cast<const uint32_t *>(a.scanLine(i));
        const uint32_t *in1 = reinterpret_cast<const uint32_t *>(b.scanLine(i));
        uint32_t *out = reinterpret_cast<uint32_t *>(result.scanLine(i));

        for (int j = 0; j < width; ++j)
            *(out++) = blend(*(in0++), *(in1++), blendFactorTable[j]);
    }

    return result;
}

/*!
 * \internal
 *
 * Returns the approximate solar elevation for the specified wallpaper \a metadata.
 */
static qreal scoreForMetaData(const KDynamicWallpaperMetaData &metadata)
{
    if (metadata.fields() & KDynamicWallpaperMetaData::SolarElevationField)
        return metadata.solarElevation() / 90;
    return std::cos(M_PI * (2 * metadata.time() + 1));
}

static bool score_compare(const KDynamicWallpaperMetaData &a, const KDynamicWallpaperMetaData &b)
{
    return scoreForMetaData(a) < scoreForMetaData(b);
}

/*!
 * \internal
 *
 * Creates a preview image for the given wallpaper \a fileName with the specified \a size.
 *
 * Note that this function runs off the main thread.
 */
static DynamicWallpaperImageAsyncResult makePreview(const QString &fileName, const QSize &size)
{
    QImage preview = DynamicWallpaperPreviewCache::load(fileName);

    if (preview.isNull()) {
        // The cache has no preview for the specified wallpaper yet, so generate one...
        KDynamicWallpaperReader reader(fileName);
        if (reader.error() != KDynamicWallpaperReader::NoError)
            return DynamicWallpaperImageAsyncResult(reader.errorString());
        if (reader.imageCount() < 2)
            return DynamicWallpaperImageAsyncResult(i18n("Not enough images"));

        QVector<KDynamicWallpaperMetaData> metadata;
        for (int i = 0; i < reader.imageCount(); ++i)
            metadata.append(reader.metaDataAt(i));

        auto dark = std::min_element(metadata.begin(), metadata.end(), score_compare);
        auto light = std::max_element(metadata.begin(), metadata.end(), score_compare);

        const QImage darkImage = reader.imageAt(std::distance(metadata.begin(), dark));
        const QImage lightImage = reader.imageAt(std::distance(metadata.begin(), light));

        preview = blend(darkImage, lightImage, 0.5);

        DynamicWallpaperPreviewCache::store(preview, fileName);
    }

    return DynamicWallpaperImageAsyncResult(preview.scaled(size, Qt::KeepAspectRatio));
}

/*!
 * Constructs a DynamicWallpaperPreviewJob with the specified \a fileName and \a requestedSize.
 */
DynamicWallpaperPreviewJob::DynamicWallpaperPreviewJob(const QString &fileName, const QSize &requestedSize)
    : d(new DynamicWallpaperPreviewJobPrivate)
{
    d->watcher = new QFutureWatcher<DynamicWallpaperImageAsyncResult>(this);
    d->watcher->setFuture(QtConcurrent::run(makePreview, fileName, requestedSize));

    connect(d->watcher, &QFutureWatcher<DynamicWallpaperImageAsyncResult>::finished,
            this, &DynamicWallpaperPreviewJob::handleFinished);
}

/*!
 * Destructs the DynamicWallpaperPreviewJob object.
 */
DynamicWallpaperPreviewJob::~DynamicWallpaperPreviewJob()
{
}

/*!
 * \fn void DynamicWallpaperPreviewJob::finished(const QImage &image)
 *
 * This signal is emitted when the preview job has successfully created a preview \a image.
 *
 * The DynamicWallpaperPreviewJob object will be destroyed after the finished() signal has
 * been emitted.
 */

/*!
 * \fn void DynamicWallpaperPreviewJob::failed(const QString &errorString)
 *
 * This signal is emitted to indicate that the preview job has failed to generate a preview
 * image. The specified \a errorString is the description of the error that occurred.
 *
 * The DynamicWallpaperPreviewJob object will be destroyed after the failed() signal has been
 * emitted.
 */

void DynamicWallpaperPreviewJob::handleFinished()
{
    const DynamicWallpaperImageAsyncResult response = d->watcher->result();
    if (response.errorString.isNull())
        emit finished(response.image);
    else
        emit failed(response.errorString);

    deleteLater();
}
