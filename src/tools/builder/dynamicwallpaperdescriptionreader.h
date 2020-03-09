/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <KDynamicWallpaperMetaData>

#include <QJsonArray>
#include <QJsonObject>
#include <QImage>
#include <QString>

class DynamicWallpaperDescriptionReader
{
public:
    explicit DynamicWallpaperDescriptionReader(const QString &fileName);

    bool atEnd() const;
    bool readNext();

    QImage image() const;
    KDynamicWallpaperMetaData metaData() const;
    bool isPrimary() const;

    bool hasError() const;
    QString errorString() const;

private:
    bool readFile();
    bool readImage(const QJsonObject &descriptor);
    bool readMetaData(const QJsonObject &descriptor);
    bool readPrimary(const QJsonObject &descriptor);
    void setError(const QString &text);
    QString resolveFileName(const QString &fileName);

    KDynamicWallpaperMetaData m_currentMetaData;
    QImage m_currentImage;
    QString m_errorString;
    QString m_fileName;
    QJsonArray m_descriptors;
    int m_currentDescriptorIndex = -1;
    bool m_isPrimary = false;
    bool m_atEnd = false;
    bool m_hasError = false;
};
