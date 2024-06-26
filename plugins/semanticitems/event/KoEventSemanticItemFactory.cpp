/* This file is part of the Calligra project, made with-in the KDE community

   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>
   SPDX-FileCopyrightText: 2013 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoEventSemanticItemFactory.h"

// lib
#include "KoRdfCalendarEvent.h"
// KF5
#include <KLocalizedString>
#include <kdebug.h>
// Qt
#include <QMimeData>

KoEventSemanticItemFactory::KoEventSemanticItemFactory()
    : KoRdfSemanticItemFactoryBase("Event")
{
}

QString KoEventSemanticItemFactory::className() const
{
    return QLatin1String("Event");
}

QString KoEventSemanticItemFactory::classDisplayName() const
{
    return i18nc("displayname of the semantic item type Event", "Event");
}

void KoEventSemanticItemFactory::updateSemanticItems(QList<hKoRdfBasicSemanticItem> &semanticItems, const KoDocumentRdf *rdf, QSharedPointer<Soprano::Model> m)
{
    const QString sparqlQuery = QLatin1String(
        " prefix rdf:  <http://www.w3.org/1999/02/22-rdf-syntax-ns#> \n"
        " prefix cal:  <http://www.w3.org/2002/12/cal/icaltzd#>  \n"
        " select distinct ?graph ?ev ?uid ?dtstart ?dtend ?summary ?location ?geo ?long ?lat \n"
        " where {  \n"
        "  GRAPH ?graph { \n"
        "    ?ev rdf:type cal:Vevent . \n"
        "    ?ev cal:uid      ?uid . \n"
        "    ?ev cal:dtstart  ?dtstart . \n"
        "    ?ev cal:dtend    ?dtend \n"
        "    OPTIONAL { ?ev cal:summary  ?summary  } \n"
        "    OPTIONAL { ?ev cal:location ?location } \n"
        "    OPTIONAL {  \n"
        "               ?ev cal:geo ?geo . \n"
        "               ?geo rdf:first ?lat . \n"
        "               ?geo rdf:rest ?joiner . \n"
        "               ?joiner rdf:first ?long \n"
        "              } \n"
        "    } \n"
        "  } \n");

    Soprano::QueryResultIterator it = m->executeQuery(sparqlQuery, Soprano::Query::QueryLanguageSparql);

    QList<hKoRdfBasicSemanticItem> oldSemanticItems = semanticItems;
    // uniqfilter is needed because soprano is not honouring
    // the DISTINCT sparql keyword
    QSet<QString> uniqfilter;
    while (it.next()) {
        const QString name = it.binding("uid").toString();
        if (uniqfilter.contains(name)) {
            continue;
        }
        uniqfilter += name;

        hKoRdfBasicSemanticItem newSemanticItem(new KoRdfCalendarEvent(0, rdf, it));

        const QString newSemanticItemLinkingSubject = newSemanticItem->linkingSubject().toString();
        foreach (hKoRdfBasicSemanticItem semItem, oldSemanticItems) {
            if (newSemanticItemLinkingSubject == semItem->linkingSubject().toString()) {
                oldSemanticItems.removeAll(semItem);
                newSemanticItem = 0;
                break;
            }
        }

        if (newSemanticItem) {
            semanticItems << newSemanticItem;
        }
    }

    foreach (hKoRdfBasicSemanticItem semItem, oldSemanticItems) {
        semanticItems.removeAll(semItem);
    }
}

hKoRdfBasicSemanticItem KoEventSemanticItemFactory::createSemanticItem(const KoDocumentRdf *rdf, QObject *parent)
{
    return hKoRdfBasicSemanticItem(new KoRdfCalendarEvent(parent, rdf));
}

bool KoEventSemanticItemFactory::canCreateSemanticItemFromMimeData(const QMimeData *mimeData) const
{
    return mimeData->hasFormat(QLatin1String("text/calendar"));
}

hKoRdfBasicSemanticItem
KoEventSemanticItemFactory::createSemanticItemFromMimeData(const QMimeData *mimeData, KoCanvasBase *host, const KoDocumentRdf *rdf, QObject *parent) const
{
    const QByteArray ba = mimeData->data(QLatin1String("text/calendar"));
    hKoRdfSemanticItem semanticItem = hKoRdfSemanticItem(new KoRdfCalendarEvent(parent, rdf));
    semanticItem->importFromData(ba, rdf, host);
    return semanticItem;
}

bool KoEventSemanticItemFactory::isBasic() const
{
    return false;
}
