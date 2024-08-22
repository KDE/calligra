/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "FileCollector.h"

// Qt
#include <QByteArray>
#include <QList>
#include <QString>

// Calligra
#include <KoStore.h>

#include "SharedExportDebug.h"

// ================================================================
//                     class FileCollectorPrivate

// Struct FileInfo holds the information about a file inside the EPUB store.

struct FileInfo {
    FileInfo(const QString &id, const QString &fileName, const QByteArray &mimetype, const QByteArray &fileContents, const QString &label)
        : m_id(id)
        , m_fileName(fileName)
        , m_mimetype(mimetype)
        , m_fileContents(fileContents)
        , m_label(label)
    {
    }

    QString m_id;
    QString m_fileName;
    QByteArray m_mimetype;
    QByteArray m_fileContents;
    QString m_label;
};

class FileCollectorPrivate
{
public:
    FileCollectorPrivate();
    ~FileCollectorPrivate();

    QString filePrefix; // default: "chapter"
    QString fileSuffix; // default: ".xhtml"
    QString pathPrefix; // default: "OEBPS/"

    QList<FileCollector::FileInfo *> m_files; // Embedded files
};

FileCollectorPrivate::FileCollectorPrivate()
    : filePrefix("chapter")
    , fileSuffix(".xhtml")
    , pathPrefix("OEBPS/")
{
}

FileCollectorPrivate::~FileCollectorPrivate() = default;

// ================================================================
//                         class FileCollector

FileCollector::FileCollector()
    : d(new FileCollectorPrivate)
{
}

FileCollector::~FileCollector()
{
    qDeleteAll(d->m_files);

    delete d;
}

void FileCollector::setFilePrefix(const QString &prefix)
{
    d->filePrefix = prefix;
}

QString FileCollector::filePrefix() const
{
    return d->filePrefix;
}

void FileCollector::setFileSuffix(const QString &suffix)
{
    d->fileSuffix = suffix;
}

QString FileCollector::fileSuffix() const
{
    return d->fileSuffix;
}

void FileCollector::setPathPrefix(const QString &prefix)
{
    d->pathPrefix = prefix;
}

QString FileCollector::pathPrefix() const
{
    return d->pathPrefix;
}

// ----------------------------------------------------------------

void FileCollector::addContentFile(const QString &id, const QString &fileName, const QByteArray &mimetype, const QByteArray &fileContents)
{
    addContentFile(id, fileName, mimetype, fileContents, "");
}

void FileCollector::addContentFile(const QString &id, const QString &fileName, const QByteArray &mimetype, const QByteArray &fileContents, const QString &label)
{
    FileInfo *newFile = new FileInfo(id, fileName, mimetype, fileContents, label);
    d->m_files.append(newFile);
}

QList<FileCollector::FileInfo *> FileCollector::files() const
{
    return d->m_files;
}

KoFilter::ConversionStatus FileCollector::writeFiles(KoStore *store)
{
    // Write contents of added files.
    foreach (FileInfo *file, d->m_files) {
        if (!store->open(file->m_fileName)) {
            debugSharedExport << "Can not create" << file->m_fileName;
            return KoFilter::CreationError;
        }
        store->write(file->m_fileContents);
        store->close();
    }

    return KoFilter::OK;
}

// ----------------------------------------------------------------
//                         Private functions
