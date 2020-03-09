/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <KPackage/PackageStructure>

#include <QThread>
#include <QUuid>

class DynamicWallpaperCrawler : public QThread
{
    Q_OBJECT

public:
    explicit DynamicWallpaperCrawler(QObject *parent = nullptr);
    ~DynamicWallpaperCrawler() override;

    QUuid token() const;

    void setSearchRoots(const QStringList &candidates);
    QStringList searchRoots() const;

    void setPackageStructure(KPackage::PackageStructure *structure);
    KPackage::PackageStructure *packageStructure() const;

Q_SIGNALS:
    void foundPackage(const QString &packagePath, const QUuid &token);
    void foundFile(const QString &filePath, const QUuid &token);

protected:
    void run() override;

private:
    void visitFolder(const QString &filePath);
    void visitFile(const QString &filePath);

    bool checkPackage(const QString &filePath) const;

    KPackage::PackageStructure *m_packageStructure = nullptr;
    QStringList m_searchRoots;
    QUuid m_token;
};
