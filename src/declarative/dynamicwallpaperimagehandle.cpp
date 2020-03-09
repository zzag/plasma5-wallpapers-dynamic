/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperimagehandle.h"

#include <QVector>

/*!
 * Constructs an invalid DynamicWallpaperImageHandle object.
 */
DynamicWallpaperImageHandle::DynamicWallpaperImageHandle()
    : m_imageIndex(-1)
{
}

/*!
 * Returns \c true if the image handle is valid; otherwise returns \c false.
 */
bool DynamicWallpaperImageHandle::isValid() const
{
    return !m_fileName.isEmpty() && m_imageIndex != -1;
}

/*!
 * Sets the file name of the image handle to \p fileName.
 */
void DynamicWallpaperImageHandle::setFileName(const QString &fileName)
{
    m_fileName = fileName;
}

/*!
 * Returns the file name of the image handle.
 */
QString DynamicWallpaperImageHandle::fileName() const
{
    return m_fileName;
}

/*!
 * Sets the image index of the image handle to \p index.
 */
void DynamicWallpaperImageHandle::setImageIndex(int index)
{
    m_imageIndex = index;
}

/*!
 * Returns the image index of the image handle.
 */
int DynamicWallpaperImageHandle::imageIndex() const
{
    return m_imageIndex;
}

static QString fileNameFromBase64(const QStringRef &base64)
{
    return QByteArray::fromBase64(base64.toUtf8());
}

static QString base64FromFileName(const QString &fileName)
{
    return fileName.toUtf8().toBase64();
}

static int imageIndexFromString(const QStringRef &string)
{
    bool ok;
    const int imageIndex = string.toInt(&ok);
    if (ok)
        return imageIndex;
    return -1;
}

static QString stringFromImageIndex(int imageIndex)
{
    return QString::number(imageIndex);
}

/*!
 * Converts the value of the image handle to a QString and returns it.
 */
QString DynamicWallpaperImageHandle::toString() const
{
    const QString fileName = base64FromFileName(m_fileName);
    const QString imageIndex = stringFromImageIndex(m_imageIndex);
    return fileName + '#' + imageIndex;
}

/*!
 * Converts the value of the image handle to a QUrl which can be passed to the Image QML component.
 */
QUrl DynamicWallpaperImageHandle::toUrl() const
{
    return QLatin1String("image://dynamic/") + toString();
}

/*!
 * Creates a DynamicWallpaperImageHandle from the specified string \p string.
 */
DynamicWallpaperImageHandle DynamicWallpaperImageHandle::fromString(const QString &string)
{
    DynamicWallpaperImageHandle handle;

    const QVector<QStringRef> parts = string.splitRef('#', Qt::SkipEmptyParts);
    if (parts.count() != 2)
        return handle;

    // Encoding and decoding a file name to/from base64 is definitely an overkill, but I don't
    // want to deal with os-specific file path conventions. Sue me. :/

    handle.setFileName(fileNameFromBase64(parts[0]));
    handle.setImageIndex(imageIndexFromString(parts[1]));

    return handle;
}
