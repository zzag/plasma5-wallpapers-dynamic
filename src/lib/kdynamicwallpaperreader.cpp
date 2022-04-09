/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "kdynamicwallpaperreader.h"

#include <QDomDocument>
#include <QDomNode>
#include <QFile>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QScopeGuard>
#include <QThread>

#include <avif/avif.h>

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

class KDynamicWallpaperReaderPrivate
{
public:
    KDynamicWallpaperReaderPrivate();

    bool open();
    void close();

    QImage fetch(int imageIndex);

    QIODevice *device;
    QByteArray buffer;
    avifDecoder *decoder;
    KDynamicWallpaperReader::WallpaperReaderError wallpaperReaderError;
    QString errorString;
    QList<KDynamicWallpaperMetaData> metaData;
    bool isDeviceForeign;
};

KDynamicWallpaperReaderPrivate::KDynamicWallpaperReaderPrivate()
    : device(nullptr)
    , decoder(nullptr)
    , wallpaperReaderError(KDynamicWallpaperReader::NoError)
    , isDeviceForeign(false)
{
}

static QList<KDynamicWallpaperMetaData> parseSolarMetaData(const QByteArray &xmp)
{
    QDomDocument xmpDocument;
    xmpDocument.setContent(xmp);
    if (xmpDocument.isNull())
        return QList<KDynamicWallpaperMetaData>();

    const QString attributeName = QStringLiteral("plasma:dynamic-wallpaper-solar");
    const QDomNodeList descriptionNodes = xmpDocument.elementsByTagName(QStringLiteral("rdf:Description"));
    for (int i = 0; i < descriptionNodes.count(); ++i) {
        QDomElement descriptionNode = descriptionNodes.at(i).toElement();
        const QByteArray base64 = descriptionNode.attribute(attributeName).toUtf8();
        if (base64.isEmpty())
            continue;

        const QJsonArray array = QJsonDocument::fromJson(QByteArray::fromBase64(base64)).array();
        QList<KDynamicWallpaperMetaData> result;
        for (int i = 0; i < array.size(); ++i) {
            KSolarDynamicWallpaperMetaData metaData = KSolarDynamicWallpaperMetaData::fromJson(array[i].toObject());
            if (metaData.isValid())
                result.append(metaData);
        }
        return result;
    }

    return QList<KDynamicWallpaperMetaData>();
}

bool KDynamicWallpaperReaderPrivate::open()
{
    if (!device) {
        wallpaperReaderError = KDynamicWallpaperReader::OpenError;
        errorString = QStringLiteral("No assigned device");
        return false;
    }

    if (device->isOpen()) {
        if (!(device->openMode() & QIODevice::ReadOnly)) {
            wallpaperReaderError = KDynamicWallpaperReader::OpenError;
            errorString = QStringLiteral("The device is not open for reading");
            return false;
        }
    } else {
        if (!device->open(QIODevice::ReadOnly)) {
            wallpaperReaderError = KDynamicWallpaperReader::OpenError;
            errorString = device->errorString();
            return false;
        }
    }

    decoder = avifDecoderCreate();
    decoder->maxThreads = QThread::idealThreadCount();

    auto cleanup = qScopeGuard([this]() {
        avifDecoderDestroy(decoder);
        decoder = nullptr;
    });

    buffer = device->readAll();
    avifResult result = avifDecoderSetIOMemory(decoder, reinterpret_cast<const uint8_t *>(buffer.constData()), buffer.size());
    if (result != AVIF_RESULT_OK) {
        wallpaperReaderError = KDynamicWallpaperReader::OpenError;
        errorString = QString::fromUtf8(avifResultToString(result));
        return false;
    }

    result = avifDecoderParse(decoder);
    if (result != AVIF_RESULT_OK) {
        wallpaperReaderError = KDynamicWallpaperReader::OpenError;
        errorString = QString::fromUtf8(avifResultToString(result));
        return false;
    }

    if (!decoder->image->xmp.size) {
        wallpaperReaderError = KDynamicWallpaperReader::OpenError;
        errorString = QStringLiteral("No metadata");
        return false;
    }

    const QByteArray rawMetaData = QByteArray::fromRawData(reinterpret_cast<const char *>(decoder->image->xmp.data), decoder->image->xmp.size);

    metaData = parseSolarMetaData(rawMetaData);
    if (metaData.isEmpty()) {
        wallpaperReaderError = KDynamicWallpaperReader::OpenError;
        errorString = QStringLiteral("No metadata");
        return false;
    }

    cleanup.dismiss();
    return true;
}

void KDynamicWallpaperReaderPrivate::close()
{
    if (decoder)
        avifDecoderDestroy(decoder);
    if (!isDeviceForeign)
        device->deleteLater();

    decoder = nullptr;
    device = nullptr;
    isDeviceForeign = false;
    buffer.clear();
}

QImage KDynamicWallpaperReaderPrivate::fetch(int index)
{
    avifResult result = avifDecoderNthImage(decoder, index);
    if (result != AVIF_RESULT_OK) {
        wallpaperReaderError = KDynamicWallpaperReader::ReadError;
        errorString = QString::fromUtf8(avifResultToString(result));
        return QImage();
    }

    const QImage::Format qtFormat = QImage::Format_RGB32;
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    const avifRGBFormat avifFormat = AVIF_RGB_FORMAT_BGRA;
#else
    const avifRGBFormat avifFormat = AVIF_RGB_FORMAT_ARGB;
#endif

    QImage image(decoder->image->width, decoder->image->height, qtFormat);

    avifRGBImage rgb;
    avifRGBImageSetDefaults(&rgb, decoder->image);
    rgb.format = avifFormat;
    rgb.rowBytes = image.bytesPerLine();
    rgb.pixels = image.bits();

    result = avifImageYUVToRGB(decoder->image, &rgb);
    if (result != AVIF_RESULT_OK) {
        wallpaperReaderError = KDynamicWallpaperReader::ReadError;
        errorString = QString::fromUtf8(avifResultToString(result));
        return QImage();
    }

    // TODO: color space

    return image;
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
    d->open();
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
    d->open();
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
    return d->decoder->imageCount;
}

/*!
 * Returns the KDynamicWallpaperMetaData objects for the current wallpaper.
 */
QList<KDynamicWallpaperMetaData> KDynamicWallpaperReader::metaData() const
{
    return d->metaData;
}

/*!
 * Returns the image with the specified index \p imageIndex.
 *
 * This method will return a null QImage object if \p imageIndex is outside of the valid range.
 */
QImage KDynamicWallpaperReader::image(int imageIndex) const
{
    if (!d->decoder)
        return QImage();
    return d->fetch(imageIndex);
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
        return QStringLiteral("No error");
    return d->errorString;
}

/*!
 * Returns \c true if a dynamic wallpaper can be read for the device; otherwise \c false.
 */
bool KDynamicWallpaperReader::canRead(QIODevice *device)
{
    if (device->isSequential())
        return false;

    QByteArray header = device->peek(144);
    if (header.size() < 12)
        return false;

    avifROData input;
    input.data = reinterpret_cast<const uint8_t *>(header.constData());
    input.size = header.size();

    return avifPeekCompatibleFileType(&input);
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
