/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "MsooXmlRelationships.h"
#include "MsooXmlRelationshipsReader.h"
#include "MsooXmlImport.h"
#include "MsooXmlCommentsReader.h"
#include <KoOdfExporter.h>
#include <QSet>
#include <KDebug>

using namespace MSOOXML;

class MsooXmlRelationships::Private
{
public:
    Private() {
    }
    ~Private() {
    }
    KoFilter::ConversionStatus loadRels(const QString& path, const QString& file);
    MsooXmlImport* importer;
    KoOdfWriters* writers;
    QString *errorMessage;
    QMap<QString, QString> rels;
    QSet<QString> loadedFiles;
    
    KoFilter::ConversionStatus loadComments();
    QMap<QString, QStringList> comments;    
};

KoFilter::ConversionStatus MsooXmlRelationships::Private::loadRels(const QString& path, const QString& file)
{
    kDebug() << (path + '/' + file) << "...";
    loadedFiles.insert(path + '/' + file);
    MsooXmlRelationshipsReaderContext context(path, file, rels);
    MsooXmlRelationshipsReader reader(writers);

    const QString realPath(path + "/_rels/" + file + ".rels");
    return importer->loadAndParseDocument(
               &reader, realPath, *errorMessage, &context);
}

KoFilter::ConversionStatus MsooXmlRelationships::Private::loadComments()
{       
    MsooXmlCommentsReaderContext context(comments);
    MsooXmlCommentsReader reader(writers);        
    return importer->loadAndParseDocument(&reader, "word/comments.xml", *errorMessage, &context);;
}


MsooXmlRelationships::MsooXmlRelationships(MsooXmlImport& importer, KoOdfWriters *writers, QString& errorMessage)
        : d(new Private)
{
    d->importer = &importer;
    d->writers = writers;
    d->errorMessage = &errorMessage;
}

MsooXmlRelationships::~MsooXmlRelationships()
{
    delete d;
}


bool MsooXmlRelationships::get_comment(const QString id, QString &author, QString &date, QString &text) {
    if (d->comments.count() == 0)
        d->loadComments();
    
    if (d->comments.contains(id)) {
        QStringList list = d->comments[id];
        author = list[0];
        date   = list[1];
        text   = list[2];
    }
   
    return true;
}


QString MsooXmlRelationships::link_target(const QString& id)
{
    if (!d->loadedFiles.contains("word/document.xml"))
        d->loadRels("word", "document.xml");

    // try to find link target from rels. Only data at right side of target is needed.
    foreach(QString key, d->rels.keys()) {
        if (key.endsWith(id)) {
            int from_right = d->rels[key].length() - 5;
            return d->rels[key].right(from_right);
        }
    }

    return "";
}

QString MsooXmlRelationships::target(const QString& path, const QString& file, const QString& id)
{
    const QString key(MsooXmlRelationshipsReader::relKey(path, file, id));
    const QString result(d->rels.value(key));
    if (!result.isEmpty())
        return result;
    if (d->loadedFiles.contains(path + '/' + file)) {
        *d->errorMessage = i18n("Could not find target \"%1\" in file \"%2\"", id, path + "/" + file);
        return QString(); // cannot be found
    }
    if (d->loadRels(path, file) != KoFilter::OK) {
        *d->errorMessage = i18n("Could not find relationships file \"%1\"", path + "/" + file);
        return QString();
    }
    return d->rels.value(key);
}
