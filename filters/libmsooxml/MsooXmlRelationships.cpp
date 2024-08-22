/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2009-2010 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#include "MsooXmlRelationships.h"
#include "MsooXmlImport.h"
#include "MsooXmlRelationshipsReader.h"

#include <KoOdfExporter.h>

#include <KLocalizedString>

#include "MsooXmlDebug.h"
#include <QSet>

using namespace MSOOXML;

class Q_DECL_HIDDEN MsooXmlRelationships::Private
{
public:
    Private() = default;
    ~Private() = default;
    KoFilter::ConversionStatus loadRels(const QString &path, const QString &file);

    MsooXmlImport *importer;
    KoOdfWriters *writers;
    QString *errorMessage;
    QMap<QString, QString> rels;
    QMap<QString, QString> targetsForTypes;
    QSet<QString> loadedFiles;
};

KoFilter::ConversionStatus MsooXmlRelationships::Private::loadRels(const QString &path, const QString &file)
{
    debugMsooXml << (path + '/' + file) << "...";
    loadedFiles.insert(path + '/' + file);
    MsooXmlRelationshipsReaderContext context(path, file, rels, targetsForTypes);
    MsooXmlRelationshipsReader reader(writers);

    const QString realPath(path + "/_rels/" + file + ".rels");
    return importer->loadAndParseDocument(&reader, realPath, *errorMessage, &context);
}

MsooXmlRelationships::MsooXmlRelationships(MsooXmlImport &importer, KoOdfWriters *writers, QString &errorMessage)
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

unsigned MsooXmlRelationships::targetCountWithWord(const QString &searchTerm)
{
    unsigned count = 0;
    QMapIterator<QString, QString> i(d->rels);
    while (i.hasNext()) {
        i.next();
        if (i.value().contains(searchTerm)) {
            ++count;
        }
    }
    return count;
}

QString MsooXmlRelationships::target(const QString &path, const QString &file, const QString &id)
{
    const QString key(MsooXmlRelationshipsReader::relKey(path, file, id));
    const QString result(d->rels.value(key));
    if (!result.isEmpty())
        return result;
    const QString filePath = path + QLatin1Char('/') + file;
    if (d->loadedFiles.contains(filePath)) {
        *d->errorMessage = i18n("Could not find target for id \"%1\" in file \"%2\"", id, filePath);
        return QString(); // cannot be found
    }
    if (d->loadRels(path, file) != KoFilter::OK) {
        *d->errorMessage = i18n("Could not find relationships file \"%1\"", filePath);
        return QString();
    }
    return d->rels.value(key);
}

QString MsooXmlRelationships::targetForType(const QString &path, const QString &file, const QString &relType)
{
    const QString filePath = path + QLatin1Char('/') + file;
    const QString key(MsooXmlRelationshipsReader::targetKey(filePath, relType));
    // debugMsooXml << key;
    const QString target(d->targetsForTypes.value(key));
    if (!target.isEmpty())
        return target;
    if (d->loadedFiles.contains(filePath)) {
        *d->errorMessage = i18n("Could not find target for relationship \"%1\" in file \"%2\"", relType, filePath);
        return QString(); // cannot be found
    }
    if (d->loadRels(path, file) != KoFilter::OK) {
        *d->errorMessage = i18n("Could not find relationships file \"%1\"", filePath);
        return QString();
    }
    return d->targetsForTypes.value(key);
}
