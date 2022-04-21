/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "kdynamicwallpaperwriter.h"

#include <QFile>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QScopeGuard>
#include <QThread>

#include <avif/avif.h>

/*!
 * \class KDynamicWallpaperWriter
 * \brief The KDynamicWallpaperWriter class provides a convenient way for writing dynamic
 * wallpapers.
 *
 * If any error occurs when writing an image, write() will return false. You can then call
 * error() to find the type of the error that occurred, or errorString() to get a human
 * readable description of what went wrong.
 */

class KDynamicWallpaperWriterPrivate
{
public:
    KDynamicWallpaperWriterPrivate();

    bool flush(QIODevice *device);

    KDynamicWallpaperWriter::WallpaperWriterError wallpaperWriterError;
    QString errorString;
    QList<KDynamicWallpaperWriter::ImageView> images;
    QList<KDynamicWallpaperMetaData> metaData;
    std::optional<int> maxThreadCount;
};

KDynamicWallpaperWriterPrivate::KDynamicWallpaperWriterPrivate()
    : wallpaperWriterError(KDynamicWallpaperWriter::NoError)
{
}

static QByteArray serializeMetaData(const QList<KDynamicWallpaperMetaData> &metaData)
{
    QJsonArray array;
    QByteArray type;

    for (const KDynamicWallpaperMetaData &md : metaData) {
        if (auto solar = std::get_if<KSolarDynamicWallpaperMetaData>(&md)) {
            type = QByteArrayLiteral("solar");
            array.append(solar->toJson());
        } else if (auto dayNight = std::get_if<KDayNightDynamicWallpaperMetaData>(&md)) {
            type = QByteArrayLiteral("day-night");
            array.append(dayNight->toJson());
        } else {
            Q_UNREACHABLE();
        }
    }

    QJsonDocument document;
    document.setArray(array);

    const QByteArray base64 = document.toJson(QJsonDocument::Compact).toBase64();
    QFile templateFile(QStringLiteral(":/kdynamicwallpaper/xmp/metadata.xml"));
    templateFile.open(QFile::ReadOnly);

    QByteArray xmp = templateFile.readAll();
    xmp.replace(QByteArrayLiteral("{{type}}"), type);
    xmp.replace(QByteArrayLiteral("{{base64}}"), base64);
    return xmp;
}

bool KDynamicWallpaperWriterPrivate::flush(QIODevice *device)
{
    if (metaData.isEmpty()) {
        wallpaperWriterError = KDynamicWallpaperWriter::UnknownError;
        errorString = QStringLiteral("No metadata have been specified");
        return false;
    }

    if (images.isEmpty()) {
        wallpaperWriterError = KDynamicWallpaperWriter::UnknownError;
        errorString = QStringLiteral("No images have been specified");
        return false;
    }

    const QByteArray xmp = serializeMetaData(metaData);
    avifEncoder *encoder = avifEncoderCreate();
    encoder->maxThreads = maxThreadCount.value_or(QThread::idealThreadCount());
    auto encoderCleanup = qScopeGuard([&encoder]() {
        avifEncoderDestroy(encoder);
    });

    for (const auto &view : images) {
        const QImage image = view.data().convertToFormat(QImage::Format_RGB888);
        if (image.isNull())
            return false;

        avifImage *avif = avifImageCreate(image.width(), image.height(), 8, AVIF_PIXEL_FORMAT_YUV444);
        avifImageSetMetadataXMP(avif, reinterpret_cast<const uint8_t *>(xmp.constData()), xmp.size());

        avifRGBImage rgb;
        avifRGBImageSetDefaults(&rgb, avif);

        rgb.format = AVIF_RGB_FORMAT_RGB;
        rgb.depth = 8;
        rgb.rowBytes = image.bytesPerLine();
        rgb.pixels = const_cast<uint8_t *>(image.constBits());

        // TODO: color space

        avifResult result = avifImageRGBToYUV(avif, &rgb);
        if (result != AVIF_RESULT_OK) {
            wallpaperWriterError = KDynamicWallpaperWriter::UnknownError;
            errorString = avifResultToString(result);
            return false;
        }

        result = avifEncoderAddImage(encoder, avif, 0, AVIF_ADD_IMAGE_FLAG_NONE);
        if (result != AVIF_RESULT_OK) {
            wallpaperWriterError = KDynamicWallpaperWriter::UnknownError;
            errorString = avifResultToString(result);
            return false;
        }

        avifImageDestroy(avif);
    }

    avifRWData output = AVIF_DATA_EMPTY;
    avifResult result = avifEncoderFinish(encoder, &output);
    if (result == AVIF_RESULT_OK) {
        device->write(reinterpret_cast<const char *>(output.data), output.size);
    } else {
        wallpaperWriterError = KDynamicWallpaperWriter::EncoderError;
        errorString = avifResultToString(result);
    }

    avifRWDataFree(&output);
    return true;
}

/*!
 * Constructs an empty KDynamicWallpaperWriter object.
 */
KDynamicWallpaperWriter::KDynamicWallpaperWriter()
    : d(new KDynamicWallpaperWriterPrivate)
{
}

/*!
 * Destructs the KDynamicWallpaperWriter object.
 */
KDynamicWallpaperWriter::~KDynamicWallpaperWriter()
{
}

void KDynamicWallpaperWriter::setMetaData(const QList<KDynamicWallpaperMetaData> &metaData)
{
    d->metaData = metaData;
}

QList<KDynamicWallpaperMetaData> KDynamicWallpaperWriter::metaData() const
{
    return d->metaData;
}

void KDynamicWallpaperWriter::setImages(const QList<ImageView> &images)
{
    d->images = images;
}

QList<KDynamicWallpaperWriter::ImageView> KDynamicWallpaperWriter::images() const
{
    return d->images;
}

/*!
 * Sets the desired maximum number of threads that can be used during the encoding step.
 * If not set, QThread::idealThreadCount() will be used.
 */
void KDynamicWallpaperWriter::setMaxThreadCount(int max)
{
    d->maxThreadCount = max;
}

/*!
 * Returns the maximum number of threads that can be used during encoding. If nullopt is
 * returned, the writer is free to choose the optimal number of threads at its will.
 */
std::optional<int> KDynamicWallpaperWriter::maxThreadCount() const
{
    return d->maxThreadCount;
}

/*!
 * Begins a write sequence to the device and returns \c true if successful; otherwise \c false is
 * returned. You must call this method before calling write() method.
 *
 * If the device is not already open, KDynamicWallpaperWriter will attempt to open the device
 * in QIODevice::WriteOnly mode by calling open().
 */
bool KDynamicWallpaperWriter::flush(QIODevice *device)
{
    if (device->isOpen()) {
        if (!(device->openMode() & QIODevice::WriteOnly)) {
            d->wallpaperWriterError = KDynamicWallpaperWriter::DeviceError;
            d->errorString = QStringLiteral("The device is not open for writing");
            return false;
        }
    } else {
        if (!device->open(QIODevice::WriteOnly)) {
            d->wallpaperWriterError = KDynamicWallpaperWriter::DeviceError;
            d->errorString = device->errorString();
            return false;
        }
    }

    return d->flush(device);
}

/*!
 * Begins a write sequence to the file \p fileName and returns \c true if successful; otherwise
 * \c false is returned. Internally, KDynamicWallpaperWriter will create a QFile object and open
 * it in QIODevice::WriteOnly mode, and use it when writing dynamic wallpapers.
 *
 * You must call this method before calling write() method.
 */
bool KDynamicWallpaperWriter::flush(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        d->wallpaperWriterError = KDynamicWallpaperWriter::DeviceError;
        d->errorString = file.errorString();
        return false;
    }

    return d->flush(&file);
}

/*!
 * Returns the type of the last error that occurred.
 */
KDynamicWallpaperWriter::WallpaperWriterError KDynamicWallpaperWriter::error() const
{
    return d->wallpaperWriterError;
}

/*!
 * Returns the human readable description of the last error that occurred.
 */
QString KDynamicWallpaperWriter::errorString() const
{
    if (d->wallpaperWriterError == NoError)
        return QStringLiteral("No error");
    return d->errorString;
}

/*!
 * Returns \c true if a dynamic wallpaper can be written to the specified @device; otherwise \c false
 * is returned.
 */
bool KDynamicWallpaperWriter::canWrite(QIODevice *device)
{
    return device->isWritable();
}

/*!
 * Returns \c true if a dynamic wallpaper can be written to a file with the specified file name
 * \p fileName; otherwise \c false is returned.
 */
bool KDynamicWallpaperWriter::canWrite(const QString &fileName)
{
    QFile file(fileName);
    return file.isWritable();
}
