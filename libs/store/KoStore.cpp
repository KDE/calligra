/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2000-2002 David Faure <faure@kde.org>, Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoStore.h"
#include "KoStore_p.h"

#include "KoDirectoryStore.h"
#include "KoEncryptedStore.h"
#include "KoTarStore.h"
#include "KoZipStore.h"

#include <QBuffer>
#include <QFile>
#include <QFileInfo>

#include <QUrl>
#include <StoreDebug.h>

#include <KLocalizedString>
#include <KMessageBox>
#include <KoNetAccess.h>

#define DefaultFormat KoStore::Zip

static KoStore::Backend determineBackend(QIODevice *dev)
{
    unsigned char buf[5];
    if (dev->read((char *)buf, 4) < 4) {
        return DefaultFormat; // will create a "bad" store (bad()==true)
    }
    if (buf[0] == 0037 && buf[1] == 0213) { // gzip -> tar.gz
        return KoStore::Tar;
    }
    if (buf[0] == 'P' && buf[1] == 'K' && buf[2] == 3 && buf[3] == 4) {
        return KoStore::Zip;
    }
    return DefaultFormat; // fallback
}

KoStore *KoStore::createStore(const QString &fileName, Mode mode, const QByteArray &appIdentification, Backend backend, bool writeMimetype)
{
    bool automatic = false;
    if (backend == Auto) {
        automatic = true;
        if (mode == KoStore::Write) {
            backend = DefaultFormat;
        } else {
            QFileInfo inf(fileName);
            if (inf.isDir()) {
                backend = Directory;
            } else {
                QFile file(fileName);
                if (file.open(QIODevice::ReadOnly)) {
                    backend = determineBackend(&file);
                } else {
                    backend = DefaultFormat; // will create a "bad" store (bad()==true)
                }
            }
        }
    }
    switch (backend) {
    case Tar:
        return new KoTarStore(fileName, mode, appIdentification, writeMimetype);
    case Zip:
        if (automatic && mode == Read) {
            // When automatically detecting, this might as well be an encrypted file. We'll need to check anyway, so we'll just use the encrypted store.
            return new KoEncryptedStore(fileName, Read, appIdentification, writeMimetype);
        }
        return new KoZipStore(fileName, mode, appIdentification, writeMimetype);
    case Directory:
        return new KoDirectoryStore(fileName /* should be a dir name.... */, mode, writeMimetype);
    case Encrypted:
        return new KoEncryptedStore(fileName, mode, appIdentification, writeMimetype);
    default:
        warnStore << "Unsupported backend requested for KoStore : " << backend;
        return nullptr;
    }
}

KoStore *KoStore::createStore(QIODevice *device, Mode mode, const QByteArray &appIdentification, Backend backend, bool writeMimetype)
{
    bool automatic = false;
    if (backend == Auto) {
        automatic = true;
        if (mode == KoStore::Write) {
            backend = DefaultFormat;
        } else if (device->open(QIODevice::ReadOnly)) {
            backend = determineBackend(device);
            device->close();
        }
    }
    switch (backend) {
    case Tar:
        return new KoTarStore(device, mode, appIdentification, writeMimetype);
    case Directory:
        errorStore << "Can't create a Directory store for a memory buffer!" << Qt::endl;
        /* fall through */
    case Zip:
        if (automatic && mode == Read) {
            // When automatically detecting, this might as well be an encrypted file. We'll need to check anyway, so we'll just use the encrypted store.
            return new KoEncryptedStore(device, Read, appIdentification, writeMimetype);
        }
        return new KoZipStore(device, mode, appIdentification, writeMimetype);
    case Encrypted:
        return new KoEncryptedStore(device, mode, appIdentification, writeMimetype);
    default:
        warnStore << "Unsupported backend requested for KoStore : " << backend;
        return nullptr;
    }
}

KoStore *KoStore::createStore(QWidget *window, const QUrl &url, Mode mode, const QByteArray &appIdentification, Backend backend, bool writeMimetype)
{
    const bool automatic = (backend == Auto);
    if (url.isLocalFile())
        return createStore(url.toLocalFile(), mode, appIdentification, backend, writeMimetype);

    QString tmpFile;
    if (mode == KoStore::Write) {
        if (automatic)
            backend = DefaultFormat;
    } else {
        const bool downloaded = KIO::NetAccess::download(url, tmpFile, window);

        if (!downloaded) {
            errorStore << "Could not download file!" << Qt::endl;
            backend = DefaultFormat; // will create a "bad" store (bad()==true)
        } else if (automatic) {
            QFile file(tmpFile);
            if (file.open(QIODevice::ReadOnly)) {
                backend = determineBackend(&file);
                file.close();
            }
        }
    }
    switch (backend) {
    case Tar:
        return new KoTarStore(window, url, tmpFile, mode, appIdentification);
    case Zip:
        if (automatic && mode == Read) {
            // When automatically detecting, this might as well be an encrypted file. We'll need to check anyway, so we'll just use the encrypted store.
            return new KoEncryptedStore(window, url, tmpFile, Read, appIdentification, writeMimetype);
        }
        return new KoZipStore(window, url, tmpFile, mode, appIdentification, writeMimetype);
    case Encrypted:
        return new KoEncryptedStore(window, url, tmpFile, mode, appIdentification, writeMimetype);
    default:
        warnStore << "Unsupported backend requested for KoStore (QUrl) : " << backend;
        KMessageBox::error(window, i18n("The directory mode is not supported for remote locations."), i18n("Calligra Storage"));
        return nullptr;
    }
}

namespace
{
const char ROOTPART[] = "root";
const char MAINNAME[] = "maindoc.xml";
}

KoStore::KoStore(Mode mode, bool writeMimetype)
    : d_ptr(new KoStorePrivate(this, mode, writeMimetype))
{
}

KoStore::~KoStore()
{
    Q_D(KoStore);
    delete d->stream;
    delete d_ptr;
}

QUrl KoStore::urlOfStore() const
{
    Q_D(const KoStore);
    if (d->fileMode == KoStorePrivate::RemoteRead || d->fileMode == KoStorePrivate::RemoteWrite) {
        return d->url;
    }
    return QUrl(d->localFileName);
}

bool KoStore::open(const QString &_name)
{
    Q_D(KoStore);
    // This also converts from relative to absolute, i.e. merges the currentPath()
    d->fileName = d->toExternalNaming(_name);

    if (d->isOpen) {
        warnStore << "Store is already opened, missing close";
        return false;
    }

    if (d->fileName.length() > 512) {
        errorStore << "KoStore: Filename " << d->fileName << " is too long" << Qt::endl;
        return false;
    }

    if (d->mode == Write) {
        debugStore << "opening for writing" << d->fileName;
        if (d->filesList.contains(d->fileName)) {
            warnStore << "KoStore: Duplicate filename" << d->fileName;
            return false;
        }

        d->filesList.append(d->fileName);

        d->size = 0;
        if (!openWrite(d->fileName)) {
            return false;
        }
    } else if (d->mode == Read) {
        debugStore << "Opening for reading" << d->fileName;
        if (!openRead(d->fileName)) {
            return false;
        }
    }
    return false;
}

bool KoStore::isOpen() const
{
    Q_D(const KoStore);
    return d->isOpen;
}

bool KoStore::close()
{
    Q_D(KoStore);
    debugStore << "Closing";

    if (!d->isOpen) {
        warnStore << "You must open before closing";
        return false;
    }

    bool ret = d->mode == Write ? closeWrite() : closeRead();

    delete d->stream;
    d->stream = nullptr;
    d->isOpen = false;
    return ret;
}

QIODevice *KoStore::device() const
{
    Q_D(const KoStore);
    if (!d->isOpen) {
        warnStore << "You must open before asking for a device";
    }
    if (d->mode != Read) {
        warnStore << "Can not get device from store that is opened for writing";
    }
    return d->stream;
}

QByteArray KoStore::read(qint64 max)
{
    Q_D(KoStore);
    QByteArray data;

    if (!d->isOpen) {
        warnStore << "You must open before reading";
        return data;
    }
    if (d->mode != Read) {
        errorStore << "KoStore: Can not read from store that is opened for writing" << Qt::endl;
        return data;
    }

    return d->stream->read(max);
}

qint64 KoStore::write(const QByteArray &data)
{
    return write(data.constData(), data.size()); // see below
}

qint64 KoStore::read(char *buffer, qint64 len)
{
    Q_D(KoStore);
    if (!d->isOpen) {
        errorStore << "KoStore: You must open before reading" << Qt::endl;
        return -1;
    }
    if (d->mode != Read) {
        errorStore << "KoStore: Can not read from store that is opened for writing" << Qt::endl;
        return -1;
    }

    return d->stream->read(buffer, len);
}

qint64 KoStore::write(const char *data, qint64 len)
{
    Q_D(KoStore);
    if (len == 0) {
        return 0;
    }

    if (!d->isOpen) {
        errorStore << "KoStore: You must open before writing" << Qt::endl;
        return 0;
    }
    if (d->mode != Write) {
        errorStore << "KoStore: Can not write to store that is opened for reading" << Qt::endl;
        return 0;
    }

    qint64 nwritten = d->stream->write(data, len);
    Q_ASSERT(nwritten == (int)len);
    d->size += nwritten;

    return nwritten;
}

qint64 KoStore::size() const
{
    Q_D(const KoStore);
    if (!d->isOpen) {
        warnStore << "You must open before asking for a size";
        return static_cast<qint64>(-1);
    }
    if (d->mode != Read) {
        warnStore << "Can not get size from store that is opened for writing";
        return static_cast<qint64>(-1);
    }
    return d->size;
}

bool KoStore::enterDirectory(const QString &directory)
{
    Q_D(KoStore);
    // debugStore <<"enterDirectory" << directory;
    qint64 pos;
    bool success = true;
    QString tmp(directory);

    while ((pos = tmp.indexOf('/')) != -1 && (success = d->enterDirectoryInternal(tmp.left(pos)))) {
        tmp.remove(0, pos + 1);
    }

    if (success && !tmp.isEmpty()) {
        return d->enterDirectoryInternal(tmp);
    }
    return success;
}

bool KoStore::leaveDirectory()
{
    Q_D(KoStore);
    if (d->currentPath.isEmpty()) {
        return false;
    }

    d->currentPath.pop_back();

    return enterAbsoluteDirectory(currentPath());
}

QString KoStore::currentPath() const
{
    Q_D(const KoStore);
    QString path;
    QStringList::ConstIterator it = d->currentPath.begin();
    QStringList::ConstIterator end = d->currentPath.end();
    for (; it != end; ++it) {
        path += *it;
        path += '/';
    }
    return path;
}

void KoStore::pushDirectory()
{
    Q_D(KoStore);
    d->directoryStack.push(currentPath());
}

void KoStore::popDirectory()
{
    Q_D(KoStore);
    d->currentPath.clear();
    enterAbsoluteDirectory(QString());
    enterDirectory(d->directoryStack.pop());
}

bool KoStore::addLocalFile(const QString &fileName, const QString &destName)
{
    QFileInfo fi(fileName);
    uint size = fi.size();
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    if (!open(destName)) {
        return false;
    }

    QByteArray data;
    data.resize(8L * 1024);

    qint64 total = 0;
    for (qint64 block = 0; (block = file.read(data.data(), data.size())) > 0; total += block) {
        data.resize(block);
        if (write(data) != block) {
            return false;
        }
        data.resize(8L * 1024);
    }
    Q_ASSERT(total == size);
    if (total != size) {
        warnStore << "Did not write enough bytes. Expected: " << size << ", wrote" << total;
        return false;
    }

    close();
    file.close();

    return true;
}

bool KoStore::addDataToFile(QByteArray &buffer, const QString &destName)
{
    QBuffer file(&buffer);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    if (!open(destName)) {
        return false;
    }

    QByteArray data;
    data.resize(8L * 1024);

    qint64 block = 0;
    while ((block = file.read(data.data(), data.size())) > 0) {
        data.resize(block);
        if (write(data) != block) {
            return false;
        }
        data.resize(8L * 1024);
    }

    close();
    file.close();

    return true;
}

bool KoStore::extractFile(const QString &sourceName, const QString &fileName)
{
    Q_D(KoStore);
    QFile file(fileName);
    return d->extractFile(sourceName, file);
}

bool KoStore::extractFile(const QString &sourceName, QByteArray &data)
{
    Q_D(KoStore);
    QBuffer buffer(&data);
    return d->extractFile(sourceName, buffer);
}

bool KoStorePrivate::extractFile(const QString &sourceName, QIODevice &buffer)
{
    if (!q->open(sourceName)) {
        return false;
    }

    if (!buffer.open(QIODevice::WriteOnly)) {
        q->close();
        return false;
    }
    // ### This could use KArchive::copy or something, no?

    QByteArray data;
    data.resize(8L * 1024);
    uint total = 0;
    for (qint64 block = 0; (block = q->read(data.data(), data.size())) > 0; total += block) {
        buffer.write(data.data(), block);
    }

    if (q->size() != static_cast<qint64>(-1)) {
        Q_ASSERT(total == q->size());
    }

    buffer.close();
    q->close();

    return true;
}

bool KoStore::seek(qint64 pos)
{
    Q_D(KoStore);
    return d->stream->seek(pos);
}

qint64 KoStore::pos() const
{
    Q_D(const KoStore);
    return d->stream->pos();
}

bool KoStore::atEnd() const
{
    Q_D(const KoStore);
    return d->stream->atEnd();
}

// See the specification for details of what this function does.
QString KoStorePrivate::toExternalNaming(const QString &_internalNaming) const
{
    if (_internalNaming == ROOTPART) {
        return q->currentPath() + MAINNAME;
    }

    QString intern;
    if (_internalNaming.startsWith("tar:/")) { // absolute reference
        intern = _internalNaming.mid(5); // remove protocol
    } else {
        intern = q->currentPath() + _internalNaming;
    }

    return intern;
}

bool KoStorePrivate::enterDirectoryInternal(const QString &directory)
{
    if (q->enterRelativeDirectory(directory)) {
        currentPath.append(directory);
        return true;
    }
    return false;
}

bool KoStore::hasFile(const QString &fileName) const
{
    Q_D(const KoStore);
    return fileExists(d->toExternalNaming(fileName));
}

bool KoStore::finalize()
{
    Q_D(KoStore);
    Q_ASSERT(!d->finalized); // call this only once!
    d->finalized = true;
    return doFinalize();
}

void KoStore::setCompressionEnabled(bool /*e*/)
{
}

bool KoStore::isEncrypted()
{
    return false;
}

bool KoStore::setPassword(const QString & /*password*/)
{
    return false;
}

QString KoStore::password()
{
    return QString();
}

bool KoStore::bad() const
{
    Q_D(const KoStore);
    return !d->good;
}

KoStore::Mode KoStore::mode() const
{
    Q_D(const KoStore);
    return d->mode;
}

QStringList KoStore::directoryList() const
{
    return QStringList();
}
