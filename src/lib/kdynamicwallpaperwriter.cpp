/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "kdynamicwallpaperwriter.h"
#include "kdynamicwallpapermetadata.h"

#include <KLocalizedString>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
#include <QColorSpace>
#endif
#include <QFile>
#include <QImage>

#include <libheif/heif.h>

/*!
 * \class KDynamicWallpaperWriter
 * \brief The KDynamicWallpaperWriter class provides a convenient way for writing dynamic
 * wallpapers.
 *
 * If any error occurs when writing an image, write() will return false. You can then call
 * error() to find the type of the error that occurred, or errorString() to get a human
 * readable description of what went wrong.
 */

static heif_error writerWriteCallback(heif_context *, const void *data, size_t size, void *userData)
{
    QIODevice *device = static_cast<QIODevice *>(userData);

    const qint64 writtenCount = device->write(static_cast<const char *>(data), size);
    if (writtenCount == -1)
        return { heif_error_Encoding_error, heif_suberror_Cannot_write_output_data, nullptr };

    if (size_t(writtenCount) != size)
        return { heif_error_Encoding_error, heif_suberror_Cannot_write_output_data, nullptr };

    return { heif_error_Ok, heif_suberror_Unspecified, "ok" };
}

static heif_writer s_writer {
    /* .writer_api_version = */ 1,
    /* .write = */ writerWriteCallback,
};

class KDynamicWallpaperWriterPrivate
{
public:
    KDynamicWallpaperWriterPrivate();

    bool begin();
    void end();
    bool write(const QImage &image, const KDynamicWallpaperMetaData &metaData,
               KDynamicWallpaperWriter::WriteOptions options);

    QIODevice *device;
    heif_context *context;
    heif_encoder *encoder;
    KDynamicWallpaperWriter::WallpaperWriterError wallpaperWriterError;
    QString errorString;
    KDynamicWallpaperWriter::Codec codec;
    int quality;
    bool isLossless;
    bool isActive;
    bool isDeviceForeign;
};

KDynamicWallpaperWriterPrivate::KDynamicWallpaperWriterPrivate()
    : device(nullptr)
    , context(nullptr)
    , encoder(nullptr)
    , wallpaperWriterError(KDynamicWallpaperWriter::NoError)
    , codec(KDynamicWallpaperWriter::HEVC)
    , quality(100)
    , isLossless(true)
    , isActive(false)
    , isDeviceForeign(false)
{
}

static heif_compression_format formatForCodec(KDynamicWallpaperWriter::Codec codec)
{
    switch (codec) {
    case KDynamicWallpaperWriter::HEVC:
        return heif_compression_HEVC;
    case KDynamicWallpaperWriter::AVC:
        return heif_compression_AVC;
    default:
        Q_UNREACHABLE();
    }
}

bool KDynamicWallpaperWriterPrivate::begin()
{
    if (device->isOpen()) {
        if (!(device->openMode() & QIODevice::WriteOnly)) {
            wallpaperWriterError = KDynamicWallpaperWriter::DeviceError;
            errorString = i18n("The device is not open for writing");
            return false;
        }
    } else {
        if (!device->open(QIODevice::WriteOnly)) {
            wallpaperWriterError = KDynamicWallpaperWriter::DeviceError;
            errorString = device->errorString();
            return false;
        }
    }

    context = heif_context_alloc();
    if (!context) {
        wallpaperWriterError = KDynamicWallpaperWriter::UnknownError;
        errorString = i18n("Failed to allocate HEIF context");
        return false;
    }

    const heif_compression_format compressionFormat = formatForCodec(codec);
    heif_error error = heif_context_get_encoder_for_format(context, compressionFormat, &encoder);
    if (error.code != heif_error_Ok) {
        wallpaperWriterError = KDynamicWallpaperWriter::EncoderError;
        errorString = i18n("No suitable encoder");
        goto error_context;
    }

    error = heif_encoder_set_lossless(encoder, isLossless);
    if (error.code != heif_error_Ok) {
        wallpaperWriterError = KDynamicWallpaperWriter::EncoderError;
        errorString = i18n("Failed to set the lossless parameter: %1", error.message);
        goto error_encoder;
    }

    error = heif_encoder_set_lossy_quality(encoder, quality);
    if (error.code != heif_error_Ok) {
        wallpaperWriterError = KDynamicWallpaperWriter::EncoderError;
        errorString = i18n("Failed to set the quality parameter: %1", error.message);
        goto error_encoder;
    }

    return true;

    // Unfortunately, there's no any sane way to wrap heif_encoder or heif_context into a smart
    // pointer. One could argue that we could use a deleter or something in order to automatically
    // release resources, but I don't find this argument convincing because we'll end up with a
    // bunch of raw and "guarded" pointers, which isn't better than just using the goto keyword
    // if you ask me. I know it's terrible, but that's the most sanest choice.

error_encoder:
    heif_encoder_release(encoder);
    encoder = nullptr;

error_context:
    heif_context_free(context);
    context = nullptr;

    return false;
}

void KDynamicWallpaperWriterPrivate::end()
{
    // It appears like libheif can't handle incremental writes, so everything must be written
    // when KDynamicWallpaperWriter::end() is called. For the sake of keeping the amount of
    // used memory as low as possible, it would be really nice if we could flush encoded data
    // to the QIODevice in write() rather than here.
    heif_context_write(context, &s_writer, device);

    if (!isDeviceForeign)
        device->close();

    if (encoder)
        heif_encoder_release(encoder);
    if (context)
        heif_context_free(context);
    if (!isDeviceForeign)
        delete device;

    context = nullptr;
    encoder = nullptr;
    device = nullptr;
}

static QImage convertToCompatibleFormat(const QImage &image)
{
    if (image.format() == QImage::Format_RGB888 || image.format() == QImage::Format_RGBA8888)
        return image;

    if (image.hasAlphaChannel())
        return image.convertToFormat(QImage::Format_RGBA8888);

    return image.convertToFormat(QImage::Format_RGB888);
}

bool KDynamicWallpaperWriterPrivate::write(const QImage &image, const KDynamicWallpaperMetaData &metaData,
                                           KDynamicWallpaperWriter::WriteOptions options)
{
    const QImage convertedImage = convertToCompatibleFormat(image);

    const int width = convertedImage.width();
    const int height = convertedImage.height();
    const int depth = convertedImage.depth();

    heif_chroma chroma;
    heif_image_handle *handle;
    heif_image *heifImage;

    int stride;
    uint8_t *data;
    QByteArray xmpMetaData;

    if (convertedImage.hasAlphaChannel())
        chroma = heif_chroma_interleaved_RGBA;
    else
        chroma = heif_chroma_interleaved_RGB;

    heif_error error = heif_image_create(width, height, heif_colorspace_RGB, chroma, &heifImage);
    if (error.code != heif_error_Ok) {
        wallpaperWriterError = KDynamicWallpaperWriter::UnknownError;
        errorString = i18n("Failed to create a heif_image: %1", error.message);
        return false;
    }

    error = heif_image_add_plane(heifImage, heif_channel_interleaved, width, height, depth);
    if (error.code != heif_error_Ok) {
        wallpaperWriterError = KDynamicWallpaperWriter::UnknownError;
        errorString = i18n("Failed to add a color plane: %1", error.message);
        goto error_image;
    }

    if (options & KDynamicWallpaperWriter::PreserveColorProfile) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        const QByteArray iccProfile = image.colorSpace().iccProfile();
        error = heif_image_set_raw_color_profile(heifImage, "prof", iccProfile.data(), iccProfile.size());
        if (error.code != heif_error_Ok) {
            wallpaperWriterError = KDynamicWallpaperWriter::EncoderError;
            errorString = i18n("Failed to set ICC profile: %1", error.message);
            goto error_image;
        }
#else
        qWarning() << "You need at least Qt 5.14 to preserve color profiles";
#endif
    }

    data = heif_image_get_plane(heifImage, heif_channel_interleaved, &stride);

    for (int i = 0; i < height; ++i) {
        const uint8_t *source = reinterpret_cast<const uint8_t *>(convertedImage.scanLine(i));
        uint8_t *target = data + stride * i;
        std::copy_n(source, convertedImage.bytesPerLine(), target);
    }

    error = heif_context_encode_image(context, heifImage, encoder, nullptr, &handle);
    if (error.code != heif_error_Ok) {
        wallpaperWriterError = KDynamicWallpaperWriter::UnknownError;
        errorString = i18n("Failed to encode image: %1", error.message);
        goto error_image;
    }

    xmpMetaData = metaData.toXmp();
    error = heif_context_add_XMP_metadata(context, handle, xmpMetaData.data(), xmpMetaData.size());
    if (error.code != heif_error_Ok) {
        wallpaperWriterError = KDynamicWallpaperWriter::UnknownError;
        errorString = i18n("Failed to add XMP metadata: %1", error.message);
        goto error_handle;
    }

    if (options & KDynamicWallpaperWriter::Primary) {
        error = heif_context_set_primary_image(context, handle);
        if (error.code != heif_error_Ok) {
            wallpaperWriterError = KDynamicWallpaperWriter::UnknownError;
            errorString = i18n("Failed to set the primary image: %1", error.message);
            goto error_handle;
        }
    }

    heif_image_handle_release(handle);
    heif_image_release(heifImage);

    return true;

    // Unfortunately, there's no any sane way to wrap heif_image_handle or heif_image into a smart
    // pointer. One could argue that we could use a deleter or something in order to automatically
    // release resources, but I don't find this argument convincing because we'll end up with a
    // bunch of raw and "guarded" pointers, which isn't better than just using the goto keyword
    // if you ask me. I know it's terrible, but that's the most sanest choice.

error_handle:
    heif_image_handle_release(handle);

error_image:
    heif_image_release(heifImage);

    return false;
}

/*!
 * Constructs an empty KDynamicWallpaperWriter object.
 */
KDynamicWallpaperWriter::KDynamicWallpaperWriter()
    : d(new KDynamicWallpaperWriterPrivate)
{
    if (d->isActive)
        d->end();
}

/*!
 * Destructs the KDynamicWallpaperWriter object.
 */
KDynamicWallpaperWriter::~KDynamicWallpaperWriter()
{
    if (d->isActive)
        d->end();
}

/*!
 * Sets the quality parameter of the encoder to \p quality.
 */
void KDynamicWallpaperWriter::setQuality(int quality)
{
    d->quality = quality;
}

/*!
 * Returns the quality parameter of the encoder.
 */
int KDynamicWallpaperWriter::quality() const
{
    return d->quality;
}

/*!
 * Sets the lossless parameter of the encoder to \p lossless.
 */
void KDynamicWallpaperWriter::setLossless(bool lossless)
{
    d->isLossless = lossless;
}

/*!
 * Returns the lossless parameter of the encoder.
 */
bool KDynamicWallpaperWriter::isLossless() const
{
    return d->isLossless;
}

/*!
 * Sets the codec for this writer. The codec is used for encoding any data that is written.
 */
void KDynamicWallpaperWriter::setCodec(Codec codec)
{
    d->codec = codec;
}

/*!
 * Returns the code that is assigned to this writer.
 */
KDynamicWallpaperWriter::Codec KDynamicWallpaperWriter::codec() const
{
    return d->codec;
}

/*!
 * Begins a write sequence to the device and returns \c true if successful; otherwise \c false is
 * returned. You must call this method before calling write() method.
 *
 * If the device is not already open, KDynamicWallpaperWriter will attempt to open the device
 * in QIODevice::WriteOnly mode by calling open().
 */
bool KDynamicWallpaperWriter::begin(QIODevice *device)
{
    if (d->isActive)
        return false;
    d->device = device;
    d->isDeviceForeign = true;
    d->isActive = d->begin();
    return d->isActive;
}

/*!
 * Begins a write sequence to the file \p fileName and returns \c true if successful; otherwise
 * \c false is returned. Internally, KDynamicWallpaperWriter will create a QFile object and open
 * it in QIODevice::WriteOnly mode, and use it when writing dynamic wallpapers.
 *
 * You must call this method before calling write() method.
 */
bool KDynamicWallpaperWriter::begin(const QString &fileName)
{
    if (d->isActive)
        return false;
    d->device = new QFile(fileName);
    d->isDeviceForeign = false;
    d->isActive = d->begin();
    if (!d->isActive) {
        delete d->device;
        d->device = nullptr;
    }
    return d->isActive;
}

/*!
 * Finalizes the write sequence to the device.
 */
void KDynamicWallpaperWriter::end()
{
    if (!d->isActive)
        return;
    d->isActive = false;
    d->end();
}

/*!
 * Returns \c true if begin() has been called and end() has not been called yet; otherwise \c false
 * is returned.
 */
bool KDynamicWallpaperWriter::isActive() const
{
    return d->isActive;
}

bool KDynamicWallpaperWriter::write(const QImage &image, const KDynamicWallpaperMetaData &metaData,
                                    WriteOptions options)
{
    if (!d->isActive)
        return false;
    return d->write(image, metaData, options);
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
        return i18n("No error");
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
