/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000-2002 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoZipStore.h"
#include "KoStore_p.h"

#include <QBuffer>
#include <QByteArray>

#include <StoreDebug.h>
#include <kzip.h>

#include <KoNetAccess.h>
#include <QUrl>

KoZipStore::KoZipStore(const QString &_filename, Mode mode, const QByteArray &appIdentification, bool writeMimetype)
    : KoStore(mode, writeMimetype)
{
    debugStore << "KoZipStore Constructor filename =" << _filename << " mode = " << int(mode) << " mimetype = " << appIdentification << Qt::endl;
    Q_D(KoStore);

    d->localFileName = _filename;

    m_pZip = new KZip(_filename);

    init(appIdentification); // open the zip file and init some vars
}

KoZipStore::KoZipStore(QIODevice *dev, Mode mode, const QByteArray &appIdentification, bool writeMimetype)
    : KoStore(mode, writeMimetype)
{
    m_pZip = new KZip(dev);
    init(appIdentification);
}

KoZipStore::KoZipStore(QWidget *window, const QUrl &_url, const QString &_filename, Mode mode, const QByteArray &appIdentification, bool writeMimetype)
    : KoStore(mode, writeMimetype)
{
    debugStore << "KoZipStore Constructor url" << _url.url(QUrl::PreferLocalFile) << " filename = " << _filename << " mode = " << int(mode)
               << " mimetype = " << appIdentification << Qt::endl;
    Q_D(KoStore);

    d->url = _url;
    d->window = window;

    if (mode == KoStore::Read) {
        d->fileMode = KoStorePrivate::RemoteRead;
        d->localFileName = _filename;

    } else {
        d->fileMode = KoStorePrivate::RemoteWrite;
        d->localFileName = QStringLiteral("/tmp/kozip"); // ### FIXME with KTempFile
    }

    m_pZip = new KZip(d->localFileName);
    init(appIdentification); // open the zip file and init some vars
}

KoZipStore::~KoZipStore()
{
    Q_D(KoStore);
    debugStore << "KoZipStore::~KoZipStore";
    if (!d->finalized)
        finalize(); // ### no error checking when the app forgot to call finalize itself
    delete m_pZip;

    // Now we have still some job to do for remote files.
    if (d->fileMode == KoStorePrivate::RemoteRead) {
        KIO::NetAccess::removeTempFile(d->localFileName);
    } else if (d->fileMode == KoStorePrivate::RemoteWrite) {
        KIO::NetAccess::upload(d->localFileName, d->url, d->window);
        // ### FIXME: delete temp file
    }
}

void KoZipStore::init(const QByteArray &appIdentification)
{
    Q_D(KoStore);

    m_currentDir = nullptr;
    d->good = m_pZip->open(d->mode == Write ? QIODevice::WriteOnly : QIODevice::ReadOnly);

    if (!d->good)
        return;

    if (d->mode == Write) {
        // debugStore <<"KoZipStore::init writing mimetype" << appIdentification;

        m_pZip->setCompression(KZip::NoCompression);
        m_pZip->setExtraField(KZip::NoExtraField);

        // Write identification
        if (d->writeMimetype) {
            (void)m_pZip->writeFile(QStringLiteral("mimetype"), appIdentification);
        }

        m_pZip->setCompression(KZip::DeflateCompression);
        // We don't need the extra field in Calligra - so we leave it as "no extra field".
    } else {
        d->good = m_pZip->directory() != nullptr;
    }
}

void KoZipStore::setCompressionEnabled(bool e)
{
    if (e) {
        m_pZip->setCompression(KZip::DeflateCompression);
    } else {
        m_pZip->setCompression(KZip::NoCompression);
    }
}

bool KoZipStore::doFinalize()
{
    return m_pZip->close();
}

bool KoZipStore::openWrite(const QString &name)
{
    Q_D(KoStore);
    d->stream = nullptr; // Don't use!
    return m_pZip->prepareWriting(name, "", "" /*m_pZip->rootDir()->user(), m_pZip->rootDir()->group()*/, 0);
}

bool KoZipStore::openRead(const QString &name)
{
    Q_D(KoStore);
    const KArchiveEntry *entry = m_pZip->directory()->entry(name);
    if (entry == nullptr) {
        return false;
    }
    if (entry->isDirectory()) {
        warnStore << name << " is a directory !";
        return false;
    }
    // Must cast to KZipFileEntry, not only KArchiveFile, because device() isn't virtual!
    const KZipFileEntry *f = static_cast<const KZipFileEntry *>(entry);
    delete d->stream;
    d->stream = f->createDevice();
    d->size = f->size();
    return true;
}

qint64 KoZipStore::write(const char *_data, qint64 _len)
{
    Q_D(KoStore);
    if (_len == 0)
        return 0;
    // debugStore <<"KoZipStore::write" << _len;

    if (!d->isOpen) {
        errorStore << "KoStore: You must open before writing" << Qt::endl;
        return 0;
    }
    if (d->mode != Write) {
        errorStore << "KoStore: Can not write to store that is opened for reading" << Qt::endl;
        return 0;
    }

    d->size += _len;
    if (m_pZip->writeData(_data, _len)) // writeData returns a bool!
        return _len;
    return 0;
}

QStringList KoZipStore::directoryList() const
{
    QStringList retval;
    const KArchiveDirectory *directory = m_pZip->directory();
    foreach (const QString &name, directory->entries()) {
        const KArchiveEntry *fileArchiveEntry = m_pZip->directory()->entry(name);
        if (fileArchiveEntry->isDirectory()) {
            retval << name;
        }
    }
    return retval;
}

bool KoZipStore::closeWrite()
{
    Q_D(KoStore);
    debugStore << "Wrote file" << d->fileName << " into ZIP archive. size" << d->size;
    return m_pZip->finishWriting(d->size);
}

bool KoZipStore::enterRelativeDirectory(const QString &dirName)
{
    Q_D(KoStore);
    if (d->mode == Read) {
        if (!m_currentDir) {
            m_currentDir = m_pZip->directory(); // initialize
            Q_ASSERT(d->currentPath.isEmpty());
        }
        const KArchiveEntry *entry = m_currentDir->entry(dirName);
        if (entry && entry->isDirectory()) {
            m_currentDir = dynamic_cast<const KArchiveDirectory *>(entry);
            return m_currentDir != nullptr;
        }
        return false;
    } else // Write, no checking here
        return true;
}

bool KoZipStore::enterAbsoluteDirectory(const QString &path)
{
    if (path.isEmpty()) {
        m_currentDir = nullptr;
        return true;
    }
    m_currentDir = dynamic_cast<const KArchiveDirectory *>(m_pZip->directory()->entry(path));
    Q_ASSERT(m_currentDir);
    return m_currentDir != nullptr;
}

bool KoZipStore::fileExists(const QString &absPath) const
{
    const KArchiveEntry *entry = m_pZip->directory()->entry(absPath);
    return entry && entry->isFile();
}
