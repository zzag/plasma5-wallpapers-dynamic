/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperpreviewjob.h"
#include "dynamicwallpaperglobals.h"
#include "dynamicwallpaperpreviewcache.h"

#include <KDynamicWallpaperReader>
#include <KLocalizedString>
#include <KSolarDynamicWallpaperMetaData>

#include <QFutureWatcher>
#include <QtConcurrent>
#include <QtMath>

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
    const int blue = qBlue(a) * (1 - blendFactor) + qBlue(b) * blendFactor;
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
    const auto &solar = std::get<KSolarDynamicWallpaperMetaData>(metadata);
    if (solar.fields() & KSolarDynamicWallpaperMetaData::SolarElevationField)
        return solar.solarElevation() / 90;
    return std::cos(M_PI * (2 * solar.time() + 1));
}

static bool score_compare(const KDynamicWallpaperMetaData &a, const KDynamicWallpaperMetaData &b)
{
    return scoreForMetaData(a) < scoreForMetaData(b);
}

static bool isSolar(const QList<KDynamicWallpaperMetaData> &metadata)
{
    return std::all_of(metadata.constBegin(), metadata.constEnd(), [](auto md) {
        return std::holds_alternative<KSolarDynamicWallpaperMetaData>(md);
    });
}

static bool isDayNight(const QList<KDynamicWallpaperMetaData> &metadata)
{
    return std::all_of(metadata.constBegin(), metadata.constEnd(), [](auto md) {
        return std::holds_alternative<KDayNightDynamicWallpaperMetaData>(md);
    });
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

        const QList<KDynamicWallpaperMetaData> metadata = reader.metaData();
        if (metadata.isEmpty())
            return DynamicWallpaperImageAsyncResult(i18n("Not a dynamic wallpaper"));

        int darkIndex = 0;
        int lightIndex = 0;

        if (isSolar(metadata)) {
            auto dark = std::min_element(metadata.begin(), metadata.end(), score_compare);
            auto light = std::max_element(metadata.begin(), metadata.end(), score_compare);
            darkIndex = std::get<KSolarDynamicWallpaperMetaData>(*dark).index();
            lightIndex = std::get<KSolarDynamicWallpaperMetaData>(*light).index();
        } else if (isDayNight(metadata)) {
            for (const KDynamicWallpaperMetaData &md : metadata) {
                const auto &dayNight = std::get<KDayNightDynamicWallpaperMetaData>(md);
                switch (dayNight.timeOfDay()) {
                case KDayNightDynamicWallpaperMetaData::TimeOfDay::Day:
                    lightIndex = dayNight.index();
                    break;
                case KDayNightDynamicWallpaperMetaData::TimeOfDay::Night:
                    darkIndex = dayNight.index();
                    break;
                }
            }
        }

        const QImage darkImage = reader.image(darkIndex);
        if (darkImage.isNull())
            return DynamicWallpaperImageAsyncResult(reader.errorString());

        const QImage lightImage = reader.image(lightIndex);
        if (lightImage.isNull())
            return DynamicWallpaperImageAsyncResult(reader.errorString());

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
    connect(d->watcher, &QFutureWatcher<DynamicWallpaperImageAsyncResult>::finished,
            this, &DynamicWallpaperPreviewJob::handleFinished);
    d->watcher->setFuture(QtConcurrent::run(makePreview, fileName, requestedSize));
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
        Q_EMIT finished(response.image);
    else
        Q_EMIT failed(response.errorString);

    deleteLater();
}
