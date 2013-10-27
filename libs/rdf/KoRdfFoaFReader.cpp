/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <ben.martin@kogmbh.com>
   Copyright (C) 2013 Friedrich W. H. Kossebau <kossebau@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#include "KoRdfFoaFReader.h"

// lib
#include "KoRdfFoaF.h"
// KDE
#include <kdebug.h>


KoRdfFoaFReader::KoRdfFoaFReader()
{
}

KoRdfFoaFReader::~KoRdfFoaFReader()
{
}

void KoRdfFoaFReader::updateSemanticItems(QList<hKoRdfSemanticItem> &semanticItems, KoDocumentRdf *rdf, QSharedPointer<Soprano::Model> m)
{
    const QString sparqlQuery = QLatin1String(
        "prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> \n"
        "prefix foaf: <http://xmlns.com/foaf/0.1/> \n"
        "prefix pkg: <http://docs.oasis-open.org/opendocument/meta/package/common#> \n"
        "select distinct ?graph ?person ?name ?nick ?homepage ?img ?phone \n"
        "where { \n"
        "  GRAPH ?graph { \n"
        "    ?person rdf:type foaf:Person . \n"
        "    ?person foaf:name ?name \n"
        "    OPTIONAL { ?person foaf:phone ?phone } \n"
        "    OPTIONAL { ?person foaf:nick ?nick } \n"
        "    OPTIONAL { ?person foaf:homepage ?homepage } \n"
        "    OPTIONAL { ?person foaf:img ?img } \n"
        "    }\n"
        "}\n");

    Soprano::QueryResultIterator it =
        m->executeQuery(sparqlQuery,
                        Soprano::Query::QueryLanguageSparql);

    // lastKnownObjects is used to perform a sematic set diff
    // at return time d->foafObjects will have any new objects and
    // ones that are no longer available will be removed.
    QList<hKoRdfSemanticItem> oldSemanticItems = semanticItems;

    // uniqfilter is needed because soprano is not honouring
    // the DISTINCT sparql keyword
    QSet<QString> uniqfilter;
    while (it.next()) {

        QString name = it.binding("name").toString();
        if (uniqfilter.contains(name)) {
            continue;
        }
        uniqfilter += name;

        hKoRdfSemanticItem newSemanticItem(new KoRdfFoaF(0, rdf, it));

        const QString newItemLs = newSemanticItem->linkingSubject().toString();
        foreach (hKoRdfSemanticItem semItem, oldSemanticItems) {
            if (newItemLs == semItem->linkingSubject().toString()) {
                oldSemanticItems.removeAll(semItem);
                newSemanticItem = 0;
                break;
            }
        }

        if (newSemanticItem) {
            semanticItems << newSemanticItem;
        }
    }

    foreach (hKoRdfSemanticItem semItem, oldSemanticItems) {
        semanticItems.removeAll(semItem);
    }

#ifndef NDEBUG
    if (semanticItems.empty() && m->statementCount())
    {
        kDebug(30015) << "foaf() have data, but no foafs!" << endl;
        QList<Soprano::Statement> allStatements = m->listStatements().allElements();
        foreach (Soprano::Statement s, allStatements)
        {
            kDebug(30015) << s;
        }
    }
#endif
}

hKoRdfSemanticItem KoRdfFoaFReader::createSemanticItem(const KoDocumentRdf* rdf, QObject* parent)
{
    return hKoRdfSemanticItem(new KoRdfFoaF(parent, rdf));

}
