/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QThread>
#include <QUrl>

class DynamicWallpaperProber : public QThread
{
    Q_OBJECT

public:
    explicit DynamicWallpaperProber(const QUrl &fileUrl, QObject *parent = nullptr);
    ~DynamicWallpaperProber() override;

protected:
    void run() override;

Q_SIGNALS:
    void finished(const QUrl &fileUrl);
    void failed(const QUrl &fileUrl);

private:
    QUrl m_fileUrl;
};
