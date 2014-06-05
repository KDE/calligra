#include "kis_steamcloudstorage.h"
#include <QDebug>
#include <QFile>
#include <steam/steam_api.h>
#include <steam/isteamremotestorage.h>

const int32 STEAM_CLOUD_MAX_FILES = 1000;

// File threshold - above this size we push
// buffered to the cloud at runtime, rather than letting
// Steam synchronise for us
const qint64 FILE_SIZE_BUFFERED_THRESHOLD = 20000000; // 20MB

class KisSteamCloudStorage::Private
{
public:
    Private(KisSteamCloudStorage* qq)
    : storageTotalBytes(-1)
      , storageAvailableBytes(-1)
      , storageUsedBytes(-1)
      , fileCount(0)
      , maxFileCount(STEAM_CLOUD_MAX_FILES)
    {
    }

    qint64 storageTotalBytes;
    qint64 storageAvailableBytes;
    qint64 storageUsedBytes;

    // Note filecount relates to files not under legacy cloud support
    // Hence, you can have limited capacity even with "0" files
    int32 fileCount;
    int32 maxFileCount;

    QList<RemoteStorageEntry> fileList;
};

KisSteamCloudStorage::KisSteamCloudStorage()
    : d(new Private(this))
{
    checkStorage();
}

bool KisSteamCloudStorage::hasPendingTransfers()
{
    return false;
}

bool KisSteamCloudStorage::pushFile(const QString &filepath, const QString &destPath)
{
    bool success = false;
    // Simple file-write
    // We may want to have a "immediate/background" flag or size check
    // to trigger the immediate upload behaviour (Rather than waiting until the app closes)
    QFile file(filepath);
    qDebug() << "Saving to cloud: " << filepath;
    if(file.exists()) {
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray fileData = file.readAll();
            QByteArray filePathArray = destPath.toLocal8Bit();
            qint64 fileSize = file.size();

            qDebug() << "pushFile: fileSize: " << fileSize;

            if (fileData.size() != 0) {
                success = SteamRemoteStorage()->FileWrite( filePathArray.data(), fileData.constData(), fileData.size() );
            }
            if (success) {
                qDebug("pushFile: file written successfully");
            } else {
                qDebug("pushFile: file not written successfully");
            }
        } else {
            qDebug("pushFile: Error opening file");
        }
    } else {
        qDebug("pushFile: source file does not exist");
    }

    return success;
}

qint64 KisSteamCloudStorage::capacity()
{
    return d->storageTotalBytes;
}

qint64 KisSteamCloudStorage::freeSpace()
{
    return d->storageAvailableBytes;
}

qint64 KisSteamCloudStorage::usedSpace()
{
    return d->storageUsedBytes;
}

void KisSteamCloudStorage::checkStorage()
{
    int32 totalBytes;
    int32 availableBytes;

    bool success = SteamRemoteStorage()->GetQuota(&totalBytes, &availableBytes);

    if(success) {
        d->storageTotalBytes = totalBytes;
        d->storageAvailableBytes = availableBytes;
        d->storageUsedBytes = totalBytes - availableBytes;
        d->fileCount = SteamRemoteStorage()->GetFileCount();

        qDebug() << d->storageAvailableBytes << " of " << d->storageTotalBytes << "bytes free.";
        qDebug() << "in " << d->fileCount << " of " << d->maxFileCount << "files.";
        qDebug() << "(" << d->storageUsedBytes << "bytes used)";

        syncFiles();
    } else {
        d->storageTotalBytes = -1;
        d->storageAvailableBytes = 0;
        d->storageUsedBytes = -1;

        qDebug("Steam Cloud: Couldn't get quota");
    }
}

void KisSteamCloudStorage::syncFiles()
{
    const char* filename;
    int32 fileSize;

    d->fileCount = SteamRemoteStorage()->GetFileCount();

    d->fileList.clear();

    qDebug() << "Syncing " << d->fileCount << " files...";

    for(int fileIndex=0; fileIndex<d->fileCount; fileIndex++) {
        filename = SteamRemoteStorage()->GetFileNameAndSize(fileIndex, &fileSize);

        d->fileList.append(RemoteStorageEntry(filename, fileSize, true));
        qDebug() << "  " << fileIndex << ": " << filename << "   (size: " << fileSize << " bytes)";
    }
}

QList<RemoteStorageEntry> KisSteamCloudStorage::getFileList()
{
    return d->fileList;
}
