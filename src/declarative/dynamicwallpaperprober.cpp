/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpaperprober.h"

#include <KDynamicWallpaperMetaData>
#include <KDynamicWallpaperReader>

/*!
 * \class DynamicWallpaperProber
 * \brief The DynamicWallpaperProper class provides a convenient way to asynchronously
 * check whether the specified file url corresponds to a dynamic wallpaper.
 *
 * If the specified file url is not a dynamic wallpaper, the failed() signal is emitted;
 * otherwise the finished() signal is emitted.
 *
 * After either the failed() or the finished() signal has been emitted, the prober object
 * will be destroyed automatically.
 */

/*!
 * Constructs a dynamic wallpaper prober with the specified \a fileUrl and \a parent.
 */
DynamicWallpaperProber::DynamicWallpaperProber(const QUrl &fileUrl, QObject *parent)
    : QThread(parent)
    , m_fileUrl(fileUrl)
{
}

/*!
 * Destructs the DynamicWallpaperProber object.
 */
DynamicWallpaperProber::~DynamicWallpaperProber()
{
    wait();
}

void DynamicWallpaperProber::run()
{
    const KDynamicWallpaperReader reader(m_fileUrl.toLocalFile());
    const KDynamicWallpaperMetaData metadata = reader.metaDataAt(0);

    if (metadata.isValid())
        emit finished(m_fileUrl);
    else
        emit failed(m_fileUrl);

    deleteLater();
}
