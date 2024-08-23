/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000-2002 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoTarStore.h"
#include "KoStore_p.h"

#include <QBuffer>

#include <QByteArray>

#include <QUrl>
#include <StoreDebug.h>
#include <ktar.h>

#include <KoNetAccess.h>

KoTarStore::KoTarStore(const QString &_filename, Mode mode, const QByteArray &appIdentification, bool writeMimetype)
    : KoStore(mode, writeMimetype)
{
    debugStore << "KoTarStore Constructor filename =" << _filename << " mode = " << int(mode) << Qt::endl;

    d->localFileName = _filename;

    m_pTar = new KTar(_filename, "application/x-gzip");

    init(appIdentification); // open the targz file and init some vars
}

KoTarStore::KoTarStore(QIODevice *dev, Mode mode, const QByteArray &appIdentification, bool writeMimetype)
    : KoStore(mode, writeMimetype)
{
    m_pTar = new KTar(dev);

    init(appIdentification);
}

KoTarStore::KoTarStore(QWidget *window, const QUrl &_url, const QString &_filename, Mode mode, const QByteArray &appIdentification, bool writeMimetype)
    : KoStore(mode, writeMimetype)
{
    debugStore << "KoTarStore Constructor url=" << _url.url(QUrl::PreferLocalFile) << " filename = " << _filename << " mode = " << int(mode) << Qt::endl;

    d->url = _url;
    d->window = window;

    if (mode == KoStore::Read) {
        d->fileMode = KoStorePrivate::RemoteRead;
        d->localFileName = _filename;

    } else {
        d->fileMode = KoStorePrivate::RemoteWrite;
        d->localFileName = "/tmp/kozip"; // ### FIXME with KTempFile
    }

    m_pTar = new KTar(d->localFileName, "application/x-gzip");

    init(appIdentification); // open the targz file and init some vars
}

KoTarStore::~KoTarStore()
{
    if (!d->finalized)
        finalize(); // ### no error checking when the app forgot to call finalize itself
    delete m_pTar;

    // Now we have still some job to do for remote files.
    if (d->fileMode == KoStorePrivate::RemoteRead) {
        KIO::NetAccess::removeTempFile(d->localFileName);
    } else if (d->fileMode == KoStorePrivate::RemoteWrite) {
        KIO::NetAccess::upload(d->localFileName, d->url, d->window);
        // ### FIXME: delete temp file
    }
}

QStringList KoTarStore::directoryList() const
{
    QStringList retval;
    const KArchiveDirectory *directory = m_pTar->directory();
    foreach (const QString &name, directory->entries()) {
        const KArchiveEntry *fileArchiveEntry = m_pTar->directory()->entry(name);
        if (fileArchiveEntry->isDirectory()) {
            retval << name;
        }
    }
    return retval;
}

QByteArray KoTarStore::completeMagic(const QByteArray &appMimetype)
{
    debugStore << "QCString KoTarStore::completeMagic( const QCString& appMimetype )********************";
    QByteArray res("Calligra ");
    res += appMimetype;
    res += '\004'; // Two magic bytes to make the identification
    res += '\006'; // more reliable (DF)
    debugStore << "sssssssssssssssssssssxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    debugStore << " return :!!!!!!!!!!!!!!! :" << res;
    return res;
}

void KoTarStore::init(const QByteArray &appIdentification)
{
    m_currentDir = nullptr;
    d->good = m_pTar->open(d->mode == Write ? QIODevice::WriteOnly : QIODevice::ReadOnly);

    if (!d->good)
        return;

    if (d->mode == Write) {
        debugStore << "appIdentification :" << appIdentification;
        m_pTar->setOrigFileName(completeMagic(appIdentification));
    } else {
        d->good = m_pTar->directory() != nullptr;
    }
}

bool KoTarStore::doFinalize()
{
    return m_pTar->close();
}

// When reading, d->stream comes directly from KArchiveFile::device()
// When writing, d->stream buffers the data into m_byteArray

bool KoTarStore::openWrite(const QString & /*name*/)
{
    // Prepare memory buffer for writing
    m_byteArray.resize(0);
    d->stream = std::unique_ptr<QIODevice>(new QBuffer(&m_byteArray));
    d->stream->open(QIODevice::WriteOnly);
    return true;
}

bool KoTarStore::openRead(const QString &name)
{
    const KArchiveEntry *entry = m_pTar->directory()->entry(name);
    if (entry == nullptr) {
        return false;
    }
    if (entry->isDirectory()) {
        warnStore << name << " is a directory !";
        return false;
    }
    auto f = (KArchiveFile *)entry;
    m_byteArray.resize(0);
    d->stream = std::unique_ptr<QIODevice>(f->createDevice());
    d->size = f->size();
    return true;
}

bool KoTarStore::closeWrite()
{
    // write the whole bytearray at once into the tar file

    debugStore << "Writing file" << d->fileName << " into TAR archive. size" << d->size;
    m_byteArray.resize(d->size); // TODO: check if really needed
    if (!m_pTar->writeFile(d->fileName, m_byteArray, 0100644, QStringLiteral("user"), QStringLiteral("group")))
        warnStore << "Failed to write " << d->fileName;
    m_byteArray.resize(0); // save memory
    return true;
}

bool KoTarStore::enterRelativeDirectory(const QString &dirName)
{
    if (d->mode == Read) {
        if (!m_currentDir) {
            m_currentDir = m_pTar->directory(); // initialize
            Q_ASSERT(d->currentPath.isEmpty());
        }
        const KArchiveEntry *entry = m_currentDir->entry(dirName);
        if (entry && entry->isDirectory()) {
            m_currentDir = dynamic_cast<const KArchiveDirectory *>(entry);
            return m_currentDir != nullptr;
        }
        return false;
    }
    return true;
}

bool KoTarStore::enterAbsoluteDirectory(const QString &path)
{
    if (path.isEmpty()) {
        m_currentDir = nullptr;
        return true;
    }
    if (d->mode == Read) {
        m_currentDir = dynamic_cast<const KArchiveDirectory *>(m_pTar->directory()->entry(path));
        Q_ASSERT(m_currentDir);
        return m_currentDir != nullptr;
    }
    return true;
}

bool KoTarStore::fileExists(const QString &absPath) const
{
    return m_pTar->directory()->entry(absPath) != nullptr;
}
