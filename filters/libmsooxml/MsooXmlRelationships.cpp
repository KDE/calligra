/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <KoOdfExporter.h>
#include <QSet>
#include <KDebug>

using namespace MSOOXML;

class MsooXmlRelationships::Private
{
public:
    Private()
    {
    }
    ~Private() {
    }
    KoFilter::ConversionStatus loadRels(const QString& path, const QString& file);

    MsooXmlImport* importer;
    KoOdfWriters* writers;
    QString *errorMessage;
    QMap<QString, QString> rels;
    QMap<QString, QString> targetsForTypes;
    QSet<QString> loadedFiles;
};

KoFilter::ConversionStatus MsooXmlRelationships::Private::loadRels(const QString& path, const QString& file)
{
    kDebug() << (path + '/' + file) << "...";
    loadedFiles.insert(path + '/' + file);
    MsooXmlRelationshipsReaderContext context(path, file, rels, targetsForTypes);
    MsooXmlRelationshipsReader reader(writers);

    const QString realPath(path + "/_rels/" + file + ".rels");
    return importer->loadAndParseDocument(
               &reader, realPath, *errorMessage, &context);
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

QString MsooXmlRelationships::target(const QString& path, const QString& file, const QString& id)
{
    const QString key(MsooXmlRelationshipsReader::relKey(path, file, id));
    const QString result(d->rels.value(key));
    if (!result.isEmpty())
        return result;
    if (d->loadedFiles.contains(path + '/' + file)) {
        *d->errorMessage = i18n("Could not find target for id \"%1\" in file \"%2\"", id, path + "/" + file);
        return QString(); // cannot be found
    }
    if (d->loadRels(path, file) != KoFilter::OK) {
        *d->errorMessage = i18n("Could not find relationships file \"%1\"", path + "/" + file);
        return QString();
    }
    return d->rels.value(key);
}

QString MsooXmlRelationships::targetForType(const QString& path, const QString& file, const QString& relType)
{
    const QString key(MsooXmlRelationshipsReader::targetKey(path + "/" + file, relType));
    //kDebug() << key;
    const QString target(d->targetsForTypes.value(key));
    if (!target.isEmpty())
        return target;
    if (d->loadedFiles.contains(path + "/" + file)) {
        *d->errorMessage = i18n("Could not find target for relationship \"%1\" in file \"%2\"",
                                relType, path + "/" + file);
        return QString(); // cannot be found
    }
    if (d->loadRels(path, file) != KoFilter::OK) {
        *d->errorMessage = i18n("Could not find relationships file \"%1\"", path + "/" + file);
        return QString();
    }
    return d->targetsForTypes.value(key);
}
