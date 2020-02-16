/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vladzzag@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "wallpapersmodel.h"
#include "dynamicwallpaperinstaller.h"

#include <KAboutData>
#include <KPackage/PackageLoader>

#include <QJsonObject>

WallpapersModel::WallpapersModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QHash<int, QByteArray> WallpapersModel::roleNames() const
{
    QHash<int, QByteArray> additionalRoles {
        { NameRole, QByteArrayLiteral("name") },
        { IdRole, QByteArrayLiteral("id") },
        { PreviewUrlRole, QByteArrayLiteral("previewUrl") },
        { FolderRole, QByteArrayLiteral("folder") },
        { IsRemovableRole, QByteArrayLiteral("removable") },
        { IsZombieRole, QByteArrayLiteral("zombie") },
        { AuthorRole, QByteArrayLiteral("author") },
        { LicenseRole, QByteArrayLiteral("license") },
    };
    return additionalRoles.unite(QAbstractListModel::roleNames());
}

int WallpapersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_wallpapers.count();
}

QVariant WallpapersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (m_wallpapers.count() <= index.row())
        return QVariant();

    const Wallpaper &wallpaper = m_wallpapers.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
    case NameRole:
        return wallpaper.name;
    case IdRole:
        return wallpaper.id;
    case PreviewUrlRole:
        return wallpaper.previewUrl;
    case FolderRole:
        return wallpaper.folderUrl;
    case IsRemovableRole:
        return wallpaper.isRemovable;
    case IsZombieRole:
        return wallpaper.isZombie;
    case AuthorRole:
        return wallpaper.author;
    case LicenseRole:
        return wallpaper.license;
    default:
        return QVariant();
    }
}

bool WallpapersModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    Wallpaper &wallpaper = m_wallpapers[index.row()];
    QVector<int> modifiedRoles;

    if (role == IsZombieRole) {
        const bool zombie = value.toBool();
        if (wallpaper.isZombie != zombie) {
            wallpaper.isZombie = zombie;
            modifiedRoles << IsZombieRole;
        }
    }

    if (modifiedRoles.isEmpty())
        return false;

    emit dataChanged(index, index, modifiedRoles);

    return true;
}

QStringList WallpapersModel::zombies() const
{
    QStringList zombies;

    for (const Wallpaper &wallpaper : m_wallpapers) {
        if (!wallpaper.isZombie)
            continue;
        zombies << wallpaper.id;
    }

    return zombies;
}

int WallpapersModel::indexOf(const QString &id) const
{
    auto it = std::find_if(m_wallpapers.begin(), m_wallpapers.end(),
        [id](const Wallpaper &wallpaper) {
            return wallpaper.id == id;
        });
    if (it == m_wallpapers.end())
        return -1;

    return std::distance(m_wallpapers.begin(), it);
}

static void forEachPackage(const QString &packageFormat, std::function<void(const KPackage::Package &)> callback)
{
    const QList<KPluginMetaData> packages = KPackage::PackageLoader::self()->listPackages(packageFormat);
    for (const KPluginMetaData &metaData : packages) {
        const QString pluginId = metaData.pluginId();
        const KPackage::Package package = KPackage::PackageLoader::self()->loadPackage(packageFormat, pluginId);
        if (package.isValid())
            callback(package);
    }
}

static QString previewFromMetaData(const KPluginMetaData &metaData)
{
    const QJsonObject wallpaperObject = metaData.rawData().value(QLatin1String("Wallpaper")).toObject();
    if (wallpaperObject.isEmpty())
        return QString();

    return wallpaperObject.value(QLatin1String("Preview")).toString();
}

static QUrl previewUrlForFilePath(const QString &filePath)
{
    // Using base64 because I don't want to deal with file name/path conventions. :/
    const QString base64 = filePath.toUtf8().toBase64();
    return QStringLiteral("image://dynamicpreview/") + base64;
}

void WallpapersModel::reload()
{
    const QString localWallpapersRoot = DynamicWallpaperInstaller::locatePackageRoot();

    QVector<Wallpaper> wallpapers;

    forEachPackage(QStringLiteral("Wallpaper/Dynamic"), [&](const KPackage::Package &package) {
        const KPluginMetaData metaData = package.metadata();

        Wallpaper wallpaper = {};
        wallpaper.id = metaData.pluginId();
        wallpaper.name = metaData.name();
        wallpaper.license = metaData.license();

        const QString folder = package.path();
        wallpaper.folderUrl = QUrl::fromLocalFile(folder);
        wallpaper.isRemovable = folder.startsWith(localWallpapersRoot);

        if (!metaData.authors().isEmpty())
            wallpaper.author = metaData.authors().first().name();

        const QString previewFileName = previewFromMetaData(metaData);
        if (!previewFileName.isEmpty()) {
            const QString previewFilePath = package.filePath("images", previewFileName);
            wallpaper.previewUrl = previewUrlForFilePath(previewFilePath);
        }

        wallpapers << wallpaper;
    });

    beginResetModel();
    m_wallpapers = wallpapers;
    endResetModel();
}
