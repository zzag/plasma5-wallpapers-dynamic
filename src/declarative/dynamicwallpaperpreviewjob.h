/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QImage>
#include <QSize>
#include <QString>

class DynamicWallpaperPreviewJobPrivate;

class DynamicWallpaperPreviewJob : public QObject
{
    Q_OBJECT

public:
    DynamicWallpaperPreviewJob(const QString &fileName, const QSize &size);
    ~DynamicWallpaperPreviewJob() override;

Q_SIGNALS:
    void finished(const QImage &image);
    void failed(const QString &errorString);

private Q_SLOTS:
    void handleFinished();

private:
    QScopedPointer<DynamicWallpaperPreviewJobPrivate> d;
};
