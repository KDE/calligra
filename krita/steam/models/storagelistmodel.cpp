#include "storagelistmodel.h"

#include <QDebug>
#include <QList>
#include <QString>
#include "../kritasteamclient.h"
#include "../kis_steamcloudstorage.h"

class StorageListModel::Private
{
public:
    Private()
        : kritaSteamClient(0)
    {
    };

    QList<RemoteStorageEntry> listData;
    KritaSteamClient* kritaSteamClient;
};

StorageListModel::StorageListModel(QObject *parent) :
    QAbstractListModel(parent)
    , d(new Private)
{
    QHash<int, QByteArray> roles;
    roles[ImageRole] = "image";
    roles[FilePathRole] = "filePath";
    roles[FileNameRole] = "fileName";
    roles[FileSizeRole] = "fileSizeMb";
    roles[CheckedRole] = "checked";
    setRoleNames(roles);
}

StorageListModel::~StorageListModel()
{
    delete d;
}

QVariant StorageListModel::data(const QModelIndex& index, int role) const
{
    QVariant data;
    if (index.isValid())
    {
        switch(role) {
        case FilePathRole:
            data = d->listData[index.row()].filePath;
            break;
        case FileNameRole:
            data = d->listData[index.row()].fileName;
            break;
        case FileSizeRole:
            data = d->listData[index.row()].fileSizeString();
            break;
        case CheckedRole:
            data = d->listData[index.row()].isSynced;
            break;
        }
    }
    return data;
}

int StorageListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return d->listData.count();
}

QObject* StorageListModel::kritaSteamClient()
{
    return d->kritaSteamClient;
}

void StorageListModel::setKritaSteamClient(QObject* kritaSteamClient)
{
    d->kritaSteamClient = qobject_cast<KritaSteamClient*>(kritaSteamClient);

    if (d->kritaSteamClient) {
        KisSteamCloudStorage* storage = d->kritaSteamClient->remoteStorage();
        storage->syncFiles();

        QList<RemoteStorageEntry> completeRemoteFilesList = storage->getFileList();

        if (storage) {
            beginResetModel();
            d->listData.clear();

            for(int index=0; index < completeRemoteFilesList.size(); index++) {
                if (completeRemoteFilesList[index].filePath.startsWith("workingcopies/")) {
                    d->listData.append(completeRemoteFilesList[index]);
                }
            }
            endResetModel();

            emit maxCapacityChanged();
            emit usedCapacityChanged();
        }
    }

    emit kritaSteamClientChanged();
}

void StorageListModel::requestIndexForFilename(const QString& filename)
{
    int foundIndex = -1;
    for (int i=0; foundIndex == -1 && i < d->listData.size(); i++) {
        if (d->listData.at(i).fileName.compare(filename, Qt::CaseInsensitive)==0) {
            foundIndex = i;
        }
    }
    emit filenameFound(foundIndex);
}

qint64 StorageListModel::maxCapacity()
{
    qint64 result = 0;
    if (d->kritaSteamClient) {
        if (d->kritaSteamClient->isInitialised()) {
            result = d->kritaSteamClient->remoteStorage()->capacity();
        }
    }
    return result;
}

qint64 StorageListModel::usedCapacity()
{
    qint64 result = 0;
    if (d->kritaSteamClient) {
        if (d->kritaSteamClient->isInitialised()) {
            result = d->kritaSteamClient->remoteStorage()->usedSpace();
        }
    }
    return result;
}
