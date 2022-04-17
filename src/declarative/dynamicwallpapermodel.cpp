/*
 * SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dynamicwallpapermodel.h"
#include "dynamicwallpapercrawler.h"
#include "dynamicwallpaperprober.h"

#include <KAboutData>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KPackage/PackageLoader>
#include <KSharedConfig>

#include <QFileInfo>
#include <QStandardPaths>

// TODO: The model can be implemented better.

class DynamicWallpaper
{
public:
    static DynamicWallpaper *fromFile(const QUrl &fileUrl);
    static DynamicWallpaper *fromPackage(const QUrl &packageUrl);

    QUrl imageUrl;
    QUrl folderUrl;
    QUrl previewUrl;
    QString name;
    QString packageName;
    QString license;
    QString author;
    bool isPackage = false;
    bool isCustom = false;
    bool isRemovable = false;
    bool isZombie = false;
};

static QUrl folderUrlForImageUrl(const QUrl &url)
{
    const QString fileName = url.toLocalFile();
    const QFileInfo fileInfo(fileName);
    return QUrl::fromLocalFile(fileInfo.path());
}

static QUrl previewUrlForImageUrl(const QUrl &url)
{
    const QString fileName = url.toLocalFile();
    const QString base64 = fileName.toUtf8().toBase64();
    return QLatin1String("image://dynamicpreview/") + base64;
}

static bool checkRemovable(const QUrl &url)
{
    const QString fileName = url.toLocalFile();
    const QString dataDirectory = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    return fileName.startsWith(dataDirectory);
}

DynamicWallpaper *DynamicWallpaper::fromFile(const QUrl &fileUrl)
{
    DynamicWallpaper *wallpaper = new DynamicWallpaper;
    wallpaper->imageUrl = fileUrl;
    wallpaper->folderUrl = folderUrlForImageUrl(fileUrl);
    wallpaper->previewUrl = previewUrlForImageUrl(fileUrl);
    wallpaper->name = fileUrl.fileName(QUrl::PrettyDecoded);
    return wallpaper;
}

DynamicWallpaper *DynamicWallpaper::fromPackage(const QUrl &packageUrl)
{
    KPackage::Package package =
            KPackage::PackageLoader::self()->loadPackage(QStringLiteral("Wallpaper/Dynamic"));
    package.setPath(packageUrl.toLocalFile());

    const QUrl fileUrl = package.fileUrl(QByteArrayLiteral("dynamic"));
    const KPluginMetaData metaData = package.metadata();

    DynamicWallpaper *wallpaper = new DynamicWallpaper;
    wallpaper->imageUrl = fileUrl;
    wallpaper->folderUrl = folderUrlForImageUrl(fileUrl);
    wallpaper->previewUrl = previewUrlForImageUrl(fileUrl);
    wallpaper->name = metaData.name();
    wallpaper->packageName = metaData.pluginId();
    wallpaper->license = metaData.license();
    wallpaper->isPackage = true;

    if (!metaData.authors().isEmpty())
        wallpaper->author = metaData.authors().first().name();

    return wallpaper;
}

class DynamicWallpaperModelPrivate : public QObject
{
    Q_OBJECT

public:
    DynamicWallpaperModelPrivate(DynamicWallpaperModel *model);

    DynamicWallpaper *wallpaperForIndex(const QModelIndex &index) const;

    void internalAppend(DynamicWallpaper *wallpaper);
    void internalPrepend(DynamicWallpaper *wallpaper);
    void internalScheduleRemove(const QModelIndex &index, bool set);
    void internalRemove(const QModelIndex &index);
    void internalReset();

    bool contains(const QUrl &fileUrl) const;
    QModelIndex find(const QUrl &fileUrl) const;

    bool registerFileName(const QString &fileName);
    void unregisterFileName(const QString &fileName);

    void addCustomWallpaper(const QUrl &fileUrl);
    void addFileWallpaper(const QUrl &fileUrl);
    void addPackageWallpaper(const QUrl &folderUrl);

    void removeCustomWallpaper(const QModelIndex &index);
    void removeFileWallpaper(const QModelIndex &index);
    void removePackageWallpaper(const QModelIndex &index);

    void loadCustomWallpapers();
    void loadGenericWallpapers();

    void handleFoundPackage(const QString &packagePath, const QUuid &token);
    void handleFoundFile(const QString &filePath, const QUuid &token);

    DynamicWallpaperModel *q;
    QVector<DynamicWallpaper *> wallpapers;
    KSharedConfigPtr config;
    QPointer<DynamicWallpaperCrawler> crawler;
    QUuid lastToken;
};

DynamicWallpaperModelPrivate::DynamicWallpaperModelPrivate(DynamicWallpaperModel *model)
    : q(model)
    , config(KSharedConfig::openConfig(QStringLiteral("kdynamicwallpaperrc")))
{
}

DynamicWallpaper *DynamicWallpaperModelPrivate::wallpaperForIndex(const QModelIndex &index) const
{
    if (!index.isValid())
        return nullptr;
    return wallpapers.value(index.row());
}

void DynamicWallpaperModelPrivate::internalAppend(DynamicWallpaper *wallpaper)
{
    const int row = wallpapers.count();

    q->beginInsertRows(QModelIndex(), row, row);
    wallpapers.append(wallpaper);
    q->endInsertRows();
}

void DynamicWallpaperModelPrivate::internalPrepend(DynamicWallpaper *wallpaper)
{
    q->beginInsertRows(QModelIndex(), 0, 0);
    wallpapers.prepend(wallpaper);
    q->endInsertRows();
}

void DynamicWallpaperModelPrivate::internalScheduleRemove(const QModelIndex &index, bool set)
{
    const int row = index.row();

    if (wallpapers[row]->isZombie == set)
        return;
    wallpapers[row]->isZombie = set;
    Q_EMIT q->dataChanged(index, index, { DynamicWallpaperModel::WallpaperIsZombieRole });
}

void DynamicWallpaperModelPrivate::internalRemove(const QModelIndex &index)
{
    const int row = index.row();

    q->beginRemoveRows(QModelIndex(), row, row);
    delete wallpapers.takeAt(row);
    q->endRemoveRows();
}

void DynamicWallpaperModelPrivate::internalReset()
{
    q->beginResetModel();
    qDeleteAll(wallpapers);
    wallpapers.clear();
    q->endResetModel();
}

bool DynamicWallpaperModelPrivate::contains(const QUrl &fileUrl) const
{
    return find(fileUrl).isValid();
}

QModelIndex DynamicWallpaperModelPrivate::find(const QUrl &fileUrl) const
{
    for (int i = 0; i < wallpapers.count(); ++i) {
        if (wallpapers[i]->imageUrl == fileUrl)
            return q->createIndex(i, 0);
    }

    return QModelIndex();
}

bool DynamicWallpaperModelPrivate::registerFileName(const QString &fileName)
{
    KConfigGroup group(config, QStringLiteral("General"));
    QStringList wallpapers = group.readEntry(QStringLiteral("Wallpapers"), QStringList());

    if (wallpapers.contains(fileName))
        return false;

    wallpapers.prepend(fileName);

    group.writeEntry(QStringLiteral("Wallpapers"), wallpapers);
    group.sync();

    return true;
}

void DynamicWallpaperModelPrivate::unregisterFileName(const QString &fileName)
{
    KConfigGroup group(config, QStringLiteral("General"));
    QStringList wallpapers = group.readEntry(QStringLiteral("Wallpapers"), QStringList());

    wallpapers.removeOne(fileName);

    group.writeEntry(QStringLiteral("Wallpapers"), wallpapers);
    group.sync();
}

void DynamicWallpaperModelPrivate::addCustomWallpaper(const QUrl &fileUrl)
{
    const QString fileName = fileUrl.toLocalFile();
    if (fileName.isEmpty())
        return;

    if (!registerFileName(fileName))
        return;

    DynamicWallpaper *wallpaper = DynamicWallpaper::fromFile(fileUrl);
    wallpaper->isRemovable = true;
    wallpaper->isCustom = true;

    internalPrepend(wallpaper);
}

void DynamicWallpaperModelPrivate::addFileWallpaper(const QUrl &fileUrl)
{
    DynamicWallpaper *wallpaper = DynamicWallpaper::fromFile(fileUrl);
    wallpaper->isRemovable = checkRemovable(fileUrl);

    internalAppend(wallpaper);
}

void DynamicWallpaperModelPrivate::addPackageWallpaper(const QUrl &folderUrl)
{
    DynamicWallpaper *wallpaper = DynamicWallpaper::fromPackage(folderUrl);
    wallpaper->isRemovable = checkRemovable(folderUrl);

    internalAppend(wallpaper);
}

void DynamicWallpaperModelPrivate::removeCustomWallpaper(const QModelIndex &index)
{
    const DynamicWallpaper *wallpaper = wallpaperForIndex(index);

    unregisterFileName(wallpaper->imageUrl.toLocalFile());
    internalRemove(index);
}

void DynamicWallpaperModelPrivate::removeFileWallpaper(const QModelIndex &index)
{
    const DynamicWallpaper *wallpaper = wallpaperForIndex(index);

    if (!QFile::remove(wallpaper->imageUrl.toLocalFile()))
        return;

    internalRemove(index);
}

void DynamicWallpaperModelPrivate::removePackageWallpaper(const QModelIndex &index)
{
    const DynamicWallpaper *wallpaper = wallpaperForIndex(index);
    const QUrl imageUrl = wallpaper->imageUrl;

    const QString dataLocation = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    const QString wallpaperPackageRoot = dataLocation + QStringLiteral("/wallpapers/");
    KPackage::Package package =
            KPackage::PackageLoader::self()->loadPackage(QStringLiteral("Wallpaper/Dynamic"));

    KJob *uninstallJob = package.uninstall(wallpaper->packageName, wallpaperPackageRoot);

    connect(uninstallJob, &KJob::finished, this, [this, imageUrl](KJob *job) {
        if (job->error() != KJob::NoError)
            return;
        const QModelIndex index = find(imageUrl);
        if (!index.isValid())
            return;
        internalRemove(index);
    });
}

void DynamicWallpaperModelPrivate::loadCustomWallpapers()
{
    KConfigGroup group(config, QStringLiteral("General"));
    const QStringList wallpaperFileNames = group.readEntry("Wallpapers", QStringList());

    for (const QString &wallpaperFileName : wallpaperFileNames) {
        const QUrl wallpaperUrl = QUrl::fromUserInput(wallpaperFileName);
        if (contains(wallpaperUrl))
            continue;

        DynamicWallpaper *wallpaper = DynamicWallpaper::fromFile(wallpaperUrl);
        wallpaper->isRemovable = true;
        wallpaper->isCustom = true;

        internalAppend(wallpaper);
    }
}

void DynamicWallpaperModelPrivate::loadGenericWallpapers()
{
    QStringList candidates = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                       QStringLiteral("wallpapers"),
                                                       QStandardPaths::LocateDirectory);

    // Load the package structure in the main thread because it seems like the PackageLoader
    // class is not thread-safe. Notice that system wallpapers are discovered in another thread
    // since we may need to read file contents in order to determine whether a given file is
    // actually a dynamic wallpaper and not just some random avif file.
    KPackage::PackageStructure *packageStructure =
            KPackage::PackageLoader::self()->loadPackageStructure(QStringLiteral("Wallpaper/Dynamic"));

    DynamicWallpaperCrawler *crawler = new DynamicWallpaperCrawler(this);
    connect(crawler, &DynamicWallpaperCrawler::foundFile,
            this, &DynamicWallpaperModelPrivate::handleFoundFile);
    connect(crawler, &DynamicWallpaperCrawler::foundPackage,
            this, &DynamicWallpaperModelPrivate::handleFoundPackage);

    crawler->setSearchRoots(candidates);
    crawler->setPackageStructure(packageStructure);
    crawler->start(QThread::LowPriority);

    // Queued events are delivered no matter what, except the case where the receiver object
    // is destroyed. So each crawler has a token that uniquely identifies it. We use the token
    // to filter out dynamic wallpapers that are discovered by the previous crawler, if there
    // is any.
    lastToken = crawler->token();
}

void DynamicWallpaperModelPrivate::handleFoundFile(const QString &packagePath, const QUuid &token)
{
    if (lastToken == token)
        addFileWallpaper(QUrl::fromLocalFile(packagePath));
}

void DynamicWallpaperModelPrivate::handleFoundPackage(const QString &filePath, const QUuid &token)
{
    if (lastToken == token)
        addPackageWallpaper(QUrl::fromLocalFile(filePath));
}

/*!
 * Constructs an empty DynamicWallpaperModel object.
 */
DynamicWallpaperModel::DynamicWallpaperModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new DynamicWallpaperModelPrivate(this))
{
}

/*!
 * Destructs the DynamicWallpaperModel object.
 */
DynamicWallpaperModel::~DynamicWallpaperModel()
{
    qDeleteAll(d->wallpapers);
}

QHash<int, QByteArray> DynamicWallpaperModel::roleNames() const
{
    QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
    roleNames.insert(WallpaperNameRole, QByteArrayLiteral("name"));
    roleNames.insert(WallpaperFolderRole, QByteArrayLiteral("folder"));
    roleNames.insert(WallpaperLicenseRole, QByteArrayLiteral("license"));
    roleNames.insert(WallpaperAuthorRole, QByteArrayLiteral("author"));
    roleNames.insert(WallpaperIsPackageRole, QByteArrayLiteral("package"));
    roleNames.insert(WallpaperIsCustomRole, QByteArrayLiteral("custom"));
    roleNames.insert(WallpaperIsRemovableRole, QByteArrayLiteral("removable"));
    roleNames.insert(WallpaperIsZombieRole, QByteArrayLiteral("zombie"));
    roleNames.insert(WallpaperImageRole, QByteArrayLiteral("image"));
    roleNames.insert(WallpaperPreviewRole, QByteArrayLiteral("preview"));
    return roleNames;
}

/*!
 * Reimplemented to return the total number of dynamic wallpapers in the model.
 */
int DynamicWallpaperModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return d->wallpapers.count();
}

QVariant DynamicWallpaperModel::data(const QModelIndex &index, int role) const
{
    const DynamicWallpaper *wallpaper = d->wallpaperForIndex(index);
    if (!wallpaper)
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
    case WallpaperNameRole:
        return wallpaper->name;
    case WallpaperFolderRole:
        return wallpaper->folderUrl;
    case WallpaperLicenseRole:
        return wallpaper->license;
    case WallpaperAuthorRole:
        return wallpaper->author;
    case WallpaperIsPackageRole:
        return wallpaper->isPackage;
    case WallpaperIsCustomRole:
        return wallpaper->isCustom;
    case WallpaperIsRemovableRole:
        return wallpaper->isRemovable;
    case WallpaperIsZombieRole:
        return wallpaper->isZombie;
    case WallpaperImageRole:
        return wallpaper->imageUrl;
    case WallpaperPreviewRole:
        return wallpaper->previewUrl;
    }

    return QVariant();
}

/*!
 * Returns the index of the dynamic wallpaper identified by the specified image url \p fileUrl.
 *
 * This method will return -1 if dynamic wallpaper with the given image url doesn't exist.
 */
int DynamicWallpaperModel::find(const QUrl &fileUrl) const
{
    const QModelIndex index = d->find(fileUrl);
    return index.isValid() ? index.row() : -1;
}

/*!
 * Returns a QModelIndex for the specified \p index.
 */
QModelIndex DynamicWallpaperModel::modelIndex(int index) const
{
    return createIndex(index, 0);
}

/*!
 * Reloads the dynamic wallpaper model.
 */
void DynamicWallpaperModel::reload()
{
    d->config->markAsClean();
    d->config->reparseConfiguration();

    d->internalReset();
    d->loadCustomWallpapers();
    d->loadGenericWallpapers();
}

/*!
 * Removes all dynamic wallpapers that are scheduled to be removed.
 */
void DynamicWallpaperModel::purge()
{
    for (int i = rowCount() - 1; i >= 0; --i) {
        const QModelIndex wallpaperIndex = createIndex(i, 0);
        if (!wallpaperIndex.data(WallpaperIsZombieRole).toBool())
            continue;
        remove(wallpaperIndex);
    }
}

/*!
 * Adds the dynamic wallpaper with the given url \p fileUrl to the model.
 */
void DynamicWallpaperModel::add(const QUrl &fileUrl)
{
    DynamicWallpaperProber *prober = new DynamicWallpaperProber(fileUrl, this);
    connect(prober, &DynamicWallpaperProber::finished,
            this, &DynamicWallpaperModel::handleProberFinished);
    connect(prober, &DynamicWallpaperProber::failed,
            this, &DynamicWallpaperModel::handleProberFailed);
    prober->start(QThread::LowestPriority);
}

void DynamicWallpaperModel::handleProberFinished(const QUrl &fileUrl)
{
    d->addCustomWallpaper(fileUrl);
}

void DynamicWallpaperModel::handleProberFailed(const QUrl &fileUrl)
{
    Q_EMIT errorOccurred(i18n("%1 is not a dynamic wallpaper", fileUrl.toLocalFile()));
}

/*!
 * Schedules a dynamic wallpaper with the specified model index \p index for removal.
 *
 * The dynamic wallpaper will be removed from the model when purge() is called.
 */
void DynamicWallpaperModel::scheduleRemove(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    d->internalScheduleRemove(index, true);
}

/*!
 * Unschedules a dynamic wallpaper with the specified model index \p index for removal.
 */
void DynamicWallpaperModel::unscheduleRemove(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    d->internalScheduleRemove(index, false);
}

/*!
 * Removes the dynamic wallpaper with the specified model index \p index.
 */
void DynamicWallpaperModel::remove(const QModelIndex &index)
{
    const DynamicWallpaper *wallpaper = d->wallpaperForIndex(index);
    if (!wallpaper)
        return;

    if (wallpaper->isCustom)
        d->removeCustomWallpaper(index);
    else if (wallpaper->isPackage)
        d->removePackageWallpaper(index);
    else
        d->removeFileWallpaper(index);
}

#include "dynamicwallpapermodel.moc"
