/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "kdynamicwallpaperreader.h"
#include "kdynamicwallpapermetadata.h"

#include <KLocalizedString>

#include <QColorSpace>
#include <QFile>
#include <QImage>

#include <libheif/heif.h>

/*!
 * \class KDynamicWallpaperReader
 * \brief The KDynamicWallpaperReader class provides a convenient way for reading dynamic
 * wallpapers.
 *
 * If any error occurs when reading an image or metadata, imageAt() or metaDataAt() will
 * return a null QImage or an invalid KDynamicWallpaperMetaData, respectively. You can
 * then call error() to find out the type of the error that occurred, or errorString() to
 * get a human readable description of what went wrong.
 */

static QIODevice *deviceFromUserData(void *userData)
{
    return static_cast<QIODevice *>(userData);
}

static int64_t readerGetPositionCallback(void *userData)
{
    const QIODevice *device = deviceFromUserData(userData);
    return device->pos();
}

static int readerReadCallback(void *data, size_t size, void *userData)
{
    QIODevice *device = deviceFromUserData(userData);
    const qint64 readCount = device->read(static_cast<char *>(data), size);
    if (readCount == -1)
        return 1;
    return size_t(readCount) != size; // 0 indicates success
}

static int readerSeekCallback(int64_t position, void *userData)
{
    QIODevice *device = deviceFromUserData(userData);
    const bool ok = device->seek(position);
    return !ok; // 0 indicates success
}

static heif_reader_grow_status readerWaitForFileSizeCallback(int64_t size, void *userData)
{
    const QIODevice *device = deviceFromUserData(userData);
    if (device->size() < size)
        return heif_reader_grow_status_size_beyond_eof;
    return heif_reader_grow_status_size_reached;
}

static const heif_reader s_reader = {
    /* .reader_api_version = */ 1,
    /* .get_position = */ readerGetPositionCallback,
    /* .read = */ readerReadCallback,
    /* .seek = */ readerSeekCallback,
    /* .wait_for_file_size = */ readerWaitForFileSizeCallback,
};

class KDynamicWallpaperReaderPrivate
{
public:
    KDynamicWallpaperReaderPrivate();

    bool ensureOpen();
    bool open();
    void close();

    bool checkImageIndex(int imageIndex) const;

    KDynamicWallpaperMetaData metaDataAt(int imageIndex);
    QImage imageAt(int imageIndex);

    QIODevice *device;
    heif_context *context;
    KDynamicWallpaperReader::WallpaperReaderError wallpaperReaderError;
    QString errorString;
    QVector<heif_item_id> images;
    bool isDeviceForeign;
};

KDynamicWallpaperReaderPrivate::KDynamicWallpaperReaderPrivate()
    : device(nullptr)
    , context(nullptr)
    , wallpaperReaderError(KDynamicWallpaperReader::NoError)
    , isDeviceForeign(false)
{
}

bool KDynamicWallpaperReaderPrivate::ensureOpen()
{
    if (context)
        return true;
    return open();
}

bool KDynamicWallpaperReaderPrivate::open()
{
    if (!device) {
        wallpaperReaderError = KDynamicWallpaperReader::DeviceError;
        errorString = i18n("No assigned device");
        return false;
    }

    if (device->isOpen()) {
        if (!(device->openMode() & QIODevice::ReadOnly)) {
            wallpaperReaderError = KDynamicWallpaperReader::DeviceError;
            errorString = i18n("The device is not open for reading");
            return false;
        }
    } else {
        if (!device->open(QIODevice::ReadOnly)) {
            wallpaperReaderError = KDynamicWallpaperReader::DeviceError;
            errorString = device->errorString();
            return false;
        }
    }

    context = heif_context_alloc();
    if (!context) {
        wallpaperReaderError = KDynamicWallpaperReader::UnknownError;
        errorString = i18n("Failed to allocate HEIF context");
        return false;
    }

    const heif_error error = heif_context_read_from_reader(context, &s_reader, device, nullptr);
    if (error.code != heif_error_Ok) {
        wallpaperReaderError = KDynamicWallpaperReader::InvalidDataError;
        errorString = i18n("Invalid HEIF file: %1", error.message);
        heif_context_free(context);
        context = nullptr;
        return false;
    }

    const int imageCount = heif_context_get_number_of_top_level_images(context);
    images.resize(imageCount);
    heif_context_get_list_of_top_level_image_IDs(context, images.data(), imageCount);

    return true;
}

void KDynamicWallpaperReaderPrivate::close()
{
    if (context)
        heif_context_free(context);
    if (!isDeviceForeign)
        device->deleteLater();

    context = nullptr;
    device = nullptr;
    isDeviceForeign = false;
    images.clear();
}

bool KDynamicWallpaperReaderPrivate::checkImageIndex(int imageIndex) const
{
    return imageIndex >= 0 && imageIndex < images.count();
}

KDynamicWallpaperMetaData KDynamicWallpaperReaderPrivate::metaDataAt(int imageIndex)
{
    heif_image_handle *handle;

    heif_error error = heif_context_get_image_handle(context, images[imageIndex], &handle);
    if (error.code != heif_error_Ok) {
        wallpaperReaderError = KDynamicWallpaperReader::UnknownError;
        errorString = i18n("Failed to get image handle: %1", error.message);
        return KDynamicWallpaperMetaData();
    }

    const char *filter = "mime";
    const int blockCount = heif_image_handle_get_number_of_metadata_blocks(handle, filter);
    if (!blockCount) {
        heif_image_handle_release(handle);
        return KDynamicWallpaperMetaData();
    }

    QVector<heif_item_id> blockIds(blockCount);
    heif_image_handle_get_list_of_metadata_block_IDs(handle, filter, blockIds.data(), blockCount);

    KDynamicWallpaperMetaData metaData;

    for (const heif_item_id &blockId : blockIds) {
        const char *contentType = heif_image_handle_get_metadata_content_type(handle, blockId);
        if (contentType != QByteArrayLiteral("application/rdf+xml"))
            continue;

        const size_t blockSize = heif_image_handle_get_metadata_size(handle, blockId);
        QByteArray block(blockSize, 0);

        const heif_error error = heif_image_handle_get_metadata(handle, blockId, block.data());
        if (error.code != heif_error_Ok)
            continue;

        metaData = KDynamicWallpaperMetaData::fromXmp(block);
        if (metaData.isValid())
            break;
    }

    heif_image_handle_release(handle);

    return metaData;
}

static QColorSpace colorProfileForImage(heif_image *image)
{
    QColorSpace colorSpace;

    switch (heif_image_get_color_profile_type(image)) {
    case heif_color_profile_type_not_present:
    case heif_color_profile_type_nclx:
        break;
    case heif_color_profile_type_prof:
    case heif_color_profile_type_rICC: {
        const size_t iccProfileSize = heif_image_get_color_profile_type(image);
        QByteArray iccProfile(iccProfileSize, 0);
        const heif_error error = heif_image_get_raw_color_profile(image, iccProfile.data());
        if (error.code != heif_error_Ok)
            colorSpace = QColorSpace::fromIccProfile(iccProfile);
        break; }
    }

    return colorSpace;
}

QImage KDynamicWallpaperReaderPrivate::imageAt(int imageIndex)
{
    heif_image *image;
    heif_image_handle *handle;
    heif_error error;
    heif_chroma chroma;

    error = heif_context_get_image_handle(context, images[imageIndex], &handle);
    if (error.code != heif_error_Ok) {
        wallpaperReaderError = KDynamicWallpaperReader::UnknownError;
        errorString = i18n("Failed to get image handle: %1", error.message);
        return QImage();
    }

    if (heif_image_handle_has_alpha_channel(handle))
        chroma = heif_chroma_interleaved_RGBA;
    else
        chroma = heif_chroma_interleaved_RGB;

    error = heif_decode_image(handle, &image, heif_colorspace_RGB, chroma, nullptr);
    if (error.code != heif_error_Ok) {
        wallpaperReaderError = KDynamicWallpaperReader::UnknownError;
        errorString = i18n("Failed to decode image: %1", error.message);
        heif_image_handle_release(handle);
        return QImage();
    }

    int stride;
    const uint8_t *data = heif_image_get_plane_readonly(image, heif_channel_interleaved, &stride);
    const int width = heif_image_get_width(image, heif_channel_interleaved);
    const int height = heif_image_get_height(image, heif_channel_interleaved);

    QImage::Format format = QImage::Format_RGB888;
    if (chroma == heif_chroma_interleaved_RGBA)
        format = QImage::Format_RGBA8888;

    auto cleanupFunc = [](void *data) { heif_image_release(static_cast<heif_image *>(data)); };
    QImage decodedImage(data, width, height, stride, format, cleanupFunc, image);

    const QColorSpace colorSpace = colorProfileForImage(image);
    if (colorSpace.isValid())
        decodedImage.setColorSpace(colorSpace);

    heif_image_handle_release(handle); // heif_image will be destroyed by QImage

    return decodedImage;
}

/*!
 * Constructs an empty KDynamicWallpaperReader object.
 */
KDynamicWallpaperReader::KDynamicWallpaperReader()
    : d(new KDynamicWallpaperReaderPrivate)
{
}

/*!
 * Constructs the KDynamicWallpaperReader with the device \p device.
 */
KDynamicWallpaperReader::KDynamicWallpaperReader(QIODevice *device)
    : d(new KDynamicWallpaperReaderPrivate)
{
    setDevice(device);
}

/*!
 * Constructs the KDynamicWallpaperReader with the file name \p fileName.
 */
KDynamicWallpaperReader::KDynamicWallpaperReader(const QString &fileName)
    : d(new KDynamicWallpaperReaderPrivate)
{
    setFileName(fileName);
}

/*!
 * Destructs the KDynamicWallpaperReader object.
 */
KDynamicWallpaperReader::~KDynamicWallpaperReader()
{
    if (d->device)
        d->close();
}

/*!
 * Sets the device of the reader to the specified \p device.
 *
 * If the device is not already open, KDynamicWallpaperReader will attempt to open the device
 * in QIODevice::ReadOnly mode by calling open().
 */
void KDynamicWallpaperReader::setDevice(QIODevice *device)
{
    if (d->device)
        d->close();
    d->device = device;
    d->isDeviceForeign = true;
}

/*!
 * Returns the device assigned to the reader, or \c nullptr if no device has been assigned.
 */
QIODevice *KDynamicWallpaperReader::device() const
{
    return d->device;
}

/*!
 * Sets the file name of the file to be read to \p fileName. Internally, KDynamicWallpaperReader
 * will create a QFile object and open it in QIODevice::ReadOnly mode, and use it when reading
 * dynamic wallpapers.
 */
void KDynamicWallpaperReader::setFileName(const QString &fileName)
{
    if (d->device)
        d->close();
    d->device = new QFile(fileName);
    d->isDeviceForeign = false;
}

/*!
 * If the currently assigned device is a QFile, or if setFileName() has been called, this
 * function returns the name of the file KDynamicWallpaperReader reads from; otherwise an empty
 * QString object is returned.
 */
QString KDynamicWallpaperReader::fileName() const
{
    const QFile *file = qobject_cast<QFile *>(d->device);
    return file ? file->fileName() : QString();
}

/*!
 * Returns the total number of images in the dynamic wallpaper.
 */
int KDynamicWallpaperReader::imageCount() const
{
    if (!d->ensureOpen())
        return 0;
    return d->images.count();
}

/*!
 * Returns the KDynamicWallpaperMetaData object associated with image \p imageIndex.
 *
 * This method will return an invalid KDynamicWallpaperMetaData object if image \p imageIndex
 * has no metadata associated with it or if \p imageIndex is outside of the valid range.
 */
KDynamicWallpaperMetaData KDynamicWallpaperReader::metaDataAt(int imageIndex) const
{
    if (!d->ensureOpen() || !d->checkImageIndex(imageIndex))
        return KDynamicWallpaperMetaData();
    return d->metaDataAt(imageIndex);
}

/*!
 * Returns the image with the specified index \p imageIndex.
 *
 * This method will return a null QImage object if \p imageIndex is outside of the valid range.
 */
QImage KDynamicWallpaperReader::imageAt(int imageIndex) const
{
    if (!d->ensureOpen() || !d->checkImageIndex(imageIndex))
        return QImage();
    return d->imageAt(imageIndex);
}

/*!
 * Returns the type of the last error that occurred.
 */
KDynamicWallpaperReader::WallpaperReaderError KDynamicWallpaperReader::error() const
{
    return d->wallpaperReaderError;
}

/*!
 * Returns the human readable description of the last error that occurred.
 */
QString KDynamicWallpaperReader::errorString() const
{
    if (d->wallpaperReaderError == NoError)
        return i18n("No error");
    return d->errorString;
}

/*!
 * Returns \c true if a dynamic wallpaper can be read for the device; otherwise \c false.
 */
bool KDynamicWallpaperReader::canRead(QIODevice *device)
{
    if (device->isSequential())
        return false;

    const QByteArray header = device->peek(12);
    if (header.size() != 12)
        return false;

    const uint8_t *data = reinterpret_cast<const uint8_t *>(header.data());
    const char *mime = heif_get_file_mime_type(data, header.size());
    if (qstrcmp(mime, "image/heic") && qstrcmp(mime, "image/heif"))
        return false;

    return true;
}

/*!
 * Returns \c true if a dynamic wallpaper can be read for the file name; otherwise \c false.
 */
bool KDynamicWallpaperReader::canRead(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return false;
    return canRead(&file);
}
