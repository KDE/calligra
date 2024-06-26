/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef FILECOLLECTOR_H
#define FILECOLLECTOR_H

#include <KoFilter.h>
#include <QHash>

class QString;
class QByteArray;

class KoStore;

class FileCollectorPrivate;

class FileCollector
{
public:
    struct FileInfo {
        FileInfo(const QString &id, const QString &fileName, const QByteArray &mimetype, const QByteArray &fileContents, const QString &label);

        QString id;
        QString fileName;
        QByteArray mimetype;
        QByteArray fileContents;
        QString label;
    };

    FileCollector();
    virtual ~FileCollector();

    void setFilePrefix(const QString &prefix);
    QString filePrefix() const;
    void setFileSuffix(const QString &suffix);
    QString fileSuffix() const;
    void setPathPrefix(const QString &prefix);
    QString pathPrefix() const;

    void addContentFile(const QString &id, const QString &fileName, const QByteArray &mimetype, const QByteArray &fileContents);

    void addContentFile(const QString &id, const QString &fileName, const QByteArray &mimetype, const QByteArray &fileContents, const QString &label);

    QList<FileInfo *> files() const; // Embedded files

protected:
    // When you have created all the content and added it using
    // addContentFile(), call this function once and it will write
    // them into the result file or directory depending on which type
    // of KoStore that is used.
    virtual KoFilter::ConversionStatus writeFiles(KoStore *store);

private:
    FileCollectorPrivate *const d;
};

#endif // FILECOLLECTOR_H
