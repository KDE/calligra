#ifndef KIS_STEAMCLOUDSTORAGE_H
#define KIS_STEAMCLOUDSTORAGE_H

#include "kis_remotestorage.h"

class KisSteamCloudStorage : public KisRemoteStorage
{
public:
    KisSteamCloudStorage();

    virtual bool connect() { return true; }
    virtual void disconnect() {}

    virtual bool pushFile(const QString &path, const QString &destPath);

    virtual bool hasPendingTransfers();
    virtual qint64 capacity();
    virtual qint64 freeSpace();
    virtual qint64 usedSpace();

    virtual void checkStorage();
    virtual void syncFiles();

    virtual QList<RemoteStorageEntry> getFileList();

private:
    class Private;
    Private * const d;
};

#endif // KIS_STEAMCLOUDSTORAGE_H
