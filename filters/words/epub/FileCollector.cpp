/* This file is part of the KDE project

   Copyright (C) 2012 Inge Wallin            <inge@lysator.liu.se>
   Copyright (C) 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


// Own
#include "FileCollector.h"

// Qt
#include <QString>
#include <QByteArray>
#include <QList>

// Calligra
#include <KoStore.h>

#include "SharedExportDebug.h"

// ================================================================
//                     class FileCollectorPrivate


// Struct FileInfo holds the information about a file inside the EPUB store.

struct FileInfo
{
    FileInfo(const QString &id, const QString &fileName, const QByteArray &mimetype, const QByteArray &fileContents, const QString &label)
        : m_id(id), m_fileName(fileName), m_mimetype(mimetype), m_fileContents(fileContents), m_label(label)
    {}

    QString     m_id;
    QString     m_fileName;
    QByteArray  m_mimetype;
    QByteArray  m_fileContents;
    QString     m_label;
};


class FileCollectorPrivate
{
public:
    FileCollectorPrivate();
    ~FileCollectorPrivate();

    QString  filePrefix;        // default: "chapter"
    QString  fileSuffix;        // default: ".xhtml"
    QString  pathPrefix;        // default: "OEBPS/"

    QList<FileCollector::FileInfo*>  m_files;  // Embedded files
};

FileCollectorPrivate::FileCollectorPrivate()
    : filePrefix("chapter")
    , fileSuffix(".xhtml")
    , pathPrefix("OEBPS/")
{
}

FileCollectorPrivate::~FileCollectorPrivate()
{
}


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

void FileCollector::addContentFile(const QString &id, const QString &fileName,
                                   const QByteArray &mimetype, const QByteArray &fileContents)
{
    addContentFile(id, fileName, mimetype, fileContents, "");
}

void FileCollector::addContentFile(const QString &id, const QString &fileName,
                                   const QByteArray &mimetype, const QByteArray &fileContents, const QString &label)
{
    FileInfo *newFile = new FileInfo(id, fileName, mimetype, fileContents, label);
    d->m_files.append(newFile);
}

QList<FileCollector::FileInfo*>  FileCollector::files() const
{
    return d->m_files;
}

KoFilter::ConversionStatus FileCollector::writeFiles(KoStore *store)
{
    // Write contents of added files.
    foreach(FileInfo *file, d->m_files) {
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

