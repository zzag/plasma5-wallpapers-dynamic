/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QString>
#include <QUrl>

class DynamicWallpaperImageHandle
{
public:
    DynamicWallpaperImageHandle();
    DynamicWallpaperImageHandle(const QString &fileName, int index);

    bool isValid() const;

    void setFileName(const QString &fileName);
    QString fileName() const;

    void setImageIndex(int index);
    int imageIndex() const;

    QString toString() const;
    QUrl toUrl() const;

    static DynamicWallpaperImageHandle fromString(const QString &string);

private:
    QString m_fileName;
    int m_imageIndex;
};
