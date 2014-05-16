#ifndef KIS_REMOTESTORAGE_H
#define KIS_REMOTESTORAGE_H

#include <QObject>
#include "kurl.h"

class RemoteStorageEntry
{
public:
    RemoteStorageEntry(const QString& filepath, qint32 size, bool synced)
        : filePath(filepath)
        , fileSizeBytes(size)
        , isSynced(synced)
    {
        KUrl url(filepath);
        fileName = url.fileName();
    }

    QString fileSizeString() {
        QString result;
        float sizeInMb = (float) fileSizeBytes / 1000000.0f;
        result.setNum(sizeInMb, 'f', 1);
        result += " MB";
        return result;
    }

    QString filePath;
    QString fileName;
    QString fileSize;
    qint32 fileSizeBytes;
    bool isSynced;
};

class KisRemoteStorage : public QObject
{
    Q_OBJECT
public:
    explicit KisRemoteStorage(QObject *parent = 0);

    virtual bool connect() = 0;
    virtual void disconnect() = 0;

    virtual bool pushFile(const QString &path, const QString &destPath) = 0;

    virtual bool hasPendingTransfers() = 0;
    virtual qint64 capacity() = 0;
    virtual qint64 freeSpace() = 0;
    virtual qint64 usedSpace() = 0;

    virtual QList<RemoteStorageEntry> getFileList() = 0;

signals:

public slots:
};

#endif // KIS_REMOTESTORAGE_H
