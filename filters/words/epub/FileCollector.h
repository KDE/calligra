/* This file is part of the KDE project
   Copyright (C) 2012 Inge Wallin <inge@lysator.liu.se>

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

#ifndef FILECOLLECTOR_H
#define FILECOLLECTOR_H

#include <QHash>
#include <KoFilter.h>


class QString;
class QByteArray;

class KoStore;

class FileCollectorPrivate;

class FileCollector
{
public:
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

    FileCollector();
    virtual ~FileCollector();

    void setFilePrefix(const QString &prefix);
    QString filePrefix() const;
    void setFileSuffix(const QString &suffix);
    QString fileSuffix() const;
    void setPathPrefix(const QString &prefix);
    QString pathPrefix() const;

    void addContentFile(const QString &id, const QString &fileName,
                        const QByteArray &mimetype, const QByteArray &fileContents);

    void addContentFile(const QString &id, const QString &fileName,
                        const QByteArray &mimetype, const QByteArray &fileContents, const QString &label);

    QList<FileInfo*>  files() const;   // Embedded files

    
protected:

    // When you have created all the content and added it using
    // addContentFile(), call this function once and it will write
    // them into the result file or directory depending on which type
    // of KoStore that is used.
    virtual KoFilter::ConversionStatus  writeFiles(KoStore *store);

private:
    FileCollectorPrivate * const d;
};

#endif // FILECOLLECTOR_H
