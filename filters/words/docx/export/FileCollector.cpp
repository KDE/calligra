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
#include <KoStoreDevice.h>

// This filter
#include "DocxExportDebug.h"

// ================================================================
//                     class FileCollector::FileInfo


FileCollector::FileInfo::FileInfo(const QString &id, const QString &fileName,
                                  const QByteArray &mimetype, const QByteArray &fileContents,
                                  const QString &label)
    : id(id)
    , fileName(fileName)
    , mimetype(mimetype)
    , fileContents(fileContents)
    , label(label)
{
}


// ================================================================
//                     class FileCollectorPrivate


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
                                   const QByteArray &mimetype, const QByteArray &fileContents,
                                   const QString &label)
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
    foreach (FileInfo *file, d->m_files) {
        // Zip contents do not work with absolute value for lo/msoff
        QString fileName = file->fileName;
        if (fileName.at(0) == '/') {
            fileName.remove(0, 1);
        }
        if (!store->open(fileName)) {
            debugDocx << "Can not create" << file->fileName;
            return KoFilter::CreationError;
        }

        // Write contents and check if it went well.
        qint64 writeLen = store->write(file->fileContents);
        store->close();
        if (writeLen != file->fileContents.size()) {
            // FIXME: There isn't a simple KoFilter::WriteError but there should be!
            return KoFilter::EmbeddedDocError;
        }

    }

    return KoFilter::OK;
}


// ----------------------------------------------------------------
//                         Private functions

