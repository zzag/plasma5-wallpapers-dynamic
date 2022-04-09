/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperdescription.h"
#include "dynamicwallpaperimagehandle.h"

#include <KDynamicWallpaperReader>

/*!
 * Constructs an invalid DynamicWallpaperDescription object.
 */
DynamicWallpaperDescription::DynamicWallpaperDescription()
{
}

/*!
 * Returns \c true if the DynamicWallpaperDescription is valid; otherwise returns \c false.
 */
bool DynamicWallpaperDescription::isValid() const
{
    return !m_imageUrls.isEmpty();
}

/*!
 * Returns a bitmask that indicates which wallpaper engines can display this wallpaper.
 */
DynamicWallpaperDescription::EngineTypes DynamicWallpaperDescription::supportedEngines() const
{
    EngineTypes types = SolarEngine | TimedEngine;
    for (const KSolarDynamicWallpaperMetaData &metaData : m_metaData) {
        // Exclude the solar engine if there's at least one image without solar metadata.
        if (!(metaData.fields() & KSolarDynamicWallpaperMetaData::SolarAzimuthField))
            types &= ~SolarEngine;
    }
    return types;
}

/*!
 * Returns the total number of images in the dynamic wallpaper.
 *
 * This method will return \c 0 if the DynamicWallpaperDescription object is invalid.
 */
int DynamicWallpaperDescription::imageCount() const
{
    return m_imageUrls.count();
}

/*!
 * Returns the url for the image with the specified index \p imageIndex.
 *
 * This method will return an invalid QImage if the DynamicWallpaperDescription is invalid
 * or if the provided index is outside the valid range.
 */
QUrl DynamicWallpaperDescription::imageUrlAt(int imageIndex) const
{
    return m_imageUrls.value(imageIndex);
}

/*!
 * Returns the metadata for the image with the specified index \p imageIndex.
 *
 * This method will return an invalid KSolarDynamicWallpaperMetaData if the DynamicWallpaperDescription
 * is invalid or if the provided index is outside the valid range.
 */
KSolarDynamicWallpaperMetaData DynamicWallpaperDescription::metaDataAt(int imageIndex) const
{
    return m_metaData.value(imageIndex);
}

/*!
 * Attempts to load the DynamicWallpaperDescription for the given file name \p fileName.
 *
 * Returns a valid DynamicWallpaperDescription if the loading succeeds; otherwise, the returned
 * description will be invalid.
 */
DynamicWallpaperDescription DynamicWallpaperDescription::fromFile(const QString &fileName)
{
    KDynamicWallpaperReader reader(fileName);
    if (reader.error() != KDynamicWallpaperReader::NoError)
        return DynamicWallpaperDescription();

    DynamicWallpaperDescription description;

    const QList<KSolarDynamicWallpaperMetaData> metaDataList = reader.metaData();
    for (const KSolarDynamicWallpaperMetaData &metaData : metaDataList) {
        if (!metaData.isValid())
            return DynamicWallpaperDescription();

        DynamicWallpaperImageHandle handle;
        handle.setFileName(fileName);
        handle.setImageIndex(metaData.index());

        description.addImage(handle.toUrl(), metaData);
    }

    return description;
}

/*!
 * \internal
 */
void DynamicWallpaperDescription::addImage(const QUrl &url, const KSolarDynamicWallpaperMetaData &metaData)
{
    m_imageUrls << url;
    m_metaData << metaData;
}
