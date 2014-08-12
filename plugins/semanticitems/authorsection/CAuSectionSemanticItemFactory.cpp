/* This file is part of the Calligra project, made with-in the KDE community

   Copyright (C) 2014 Denis Kuplaykov <dener.kup@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "CAuSectionSemanticItemFactory.h"

// plugin
#include <CAuSectionRdf.h>
// KDE
#include <KLocale>
// Qt
#include <QMimeData>

CAuSectionSemanticItemFactory::CAuSectionSemanticItemFactory()
    : KoRdfSemanticItemFactoryBase("AuthorSection")
{
}

QString CAuSectionSemanticItemFactory::className() const
{
    return QLatin1String("AuthorSection");
}

QString CAuSectionSemanticItemFactory::classDisplayName() const
{
    return i18nc("displayname of the semantic item type AuthorSection", "AuthorSection");
}

void CAuSectionSemanticItemFactory::updateSemanticItems(QList<hKoRdfBasicSemanticItem> &semanticItems, const KoDocumentRdf *rdf, QSharedPointer<Soprano::Model> m)
{
    const QString sparqlQuery = QLatin1String(
        "PREFIX rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> \n"
        "PREFIX cau: <http://www.calligra.org/author/> \n"
        "PREFIX causec: <http://www.calligra.org/author/Section#> \n"
        "SELECT DISTINCT ?graph ?section ?magicid ?descr\n"
        "WHERE { \n"
        "  GRAPH ?graph { \n"
        "    ?section rdf:type       cau:Section . \n"
        "    ?section causec:magicid ?magicid    . \n"
        "    OPTIONAL { ?section causec:descr ?descr } \n"
        "    }\n"
        "}\n"
    );

    Soprano::QueryResultIterator it = m->executeQuery(
        sparqlQuery,
        Soprano::Query::QueryLanguageSparql
    );

    // lastKnownObjects is used to perform a sematic set diff
    // at return time d->foafObjects will have any new objects and
    // ones that are no longer available will be removed.
    QList<hKoRdfBasicSemanticItem> oldSemanticItems = semanticItems;

    // uniqfilter is needed because soprano is not honouring
    // the DISTINCT sparql keyword
    QSet<QString> uniqfilter;
    while (it.next()) {

        QString magicid = it.binding("magicid").toString();
        if (uniqfilter.contains(magicid)) {
            continue;
        }
        uniqfilter += magicid;

        hKoRdfBasicSemanticItem newSemanticItem(new CAuSectionRdf(0, rdf, it));

        const QString newItemLs = newSemanticItem->linkingSubject().toString();
        foreach (hKoRdfBasicSemanticItem semItem, oldSemanticItems) {
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

    foreach (hKoRdfBasicSemanticItem semItem, oldSemanticItems) {
        semanticItems.removeAll(semItem);
    }
}

hKoRdfBasicSemanticItem CAuSectionSemanticItemFactory::createSemanticItem(const KoDocumentRdf* rdf, QObject* parent)
{
    return hKoRdfBasicSemanticItem(new CAuSectionRdf(parent, rdf));

}

bool CAuSectionSemanticItemFactory::canCreateSemanticItemFromMimeData(const QMimeData *mimeData) const
{
    Q_UNUSED(mimeData);
    return false;
}

hKoRdfBasicSemanticItem CAuSectionSemanticItemFactory::createSemanticItemFromMimeData(
    const QMimeData *mimeData,
    KoCanvasBase *host,
    const KoDocumentRdf *rdf,
    QObject *parent) const
{
    Q_UNUSED(mimeData);
    Q_UNUSED(host);
    Q_UNUSED(rdf);
    Q_UNUSED(parent);

    return hKoRdfBasicSemanticItem(0);
}

bool CAuSectionSemanticItemFactory::isBasic() const
{
    return true;
}
