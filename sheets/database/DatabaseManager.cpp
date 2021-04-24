/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DatabaseManager.h"

#include <QHash>

#include <KoXmlNS.h>
#include <KoXmlWriter.h>

#include "CellStorage.h"
#include "Database.h"
#include "Map.h"
#include "Region.h"
#include "Sheet.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN DatabaseManager::Private
{
public:
    const Map* map;
    static int s_id;
};

int DatabaseManager::Private::s_id = 1;


DatabaseManager::DatabaseManager(const Map* map)
        : d(new Private)
{
    d->map = map;
}

DatabaseManager::~DatabaseManager()
{
    delete d;
}

QString DatabaseManager::createUniqueName() const
{
    return "database-" + QString::number(Private::s_id++);
}

bool DatabaseManager::loadOdf(const KoXmlElement& body)
{
    const KoXmlNode databaseRanges = KoXml::namedItemNS(body, KoXmlNS::table, "database-ranges");
    KoXmlElement element;
    forEachElement(element, databaseRanges) {
        if (element.namespaceURI() != KoXmlNS::table)
            continue;
        if (element.localName() == "database-range") {
            Database database;
            if (!database.loadOdf(element, d->map))
                return false;
            const Region region = database.range();
            if (!region.isValid())
                continue;
            const Sheet* sheet = region.lastSheet();
            if (!sheet)
                continue;
            sheet->cellStorage()->setDatabase(region, database);
        }
    }
    return true;
}

void DatabaseManager::saveOdf(KoXmlWriter& xmlWriter) const
{
    QList< QPair<QRectF, Database> > databases;
    const Region region(QRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax)));
    const QList<Sheet*>& sheets = d->map->sheetList();
    for (int i = 0; i < sheets.count(); ++i)
        databases << sheets[i]->cellStorage()->databases(region);
    if (databases.isEmpty())
        return;

    xmlWriter.startElement("table:database-ranges");
    for (int i = 0; i < databases.count(); ++i) {
        Database database = databases[i].second;
        database.setRange(Region(databases[i].first.toRect(), database.range().firstSheet()));
        if (!database.range().isValid())
            continue;
        database.saveOdf(xmlWriter);
    }
    xmlWriter.endElement();
}
