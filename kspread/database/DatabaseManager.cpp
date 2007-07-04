/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "DatabaseManager.h"

#include <QHash>

#include <KoDom.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

#include "CellStorage.h"
#include "Database.h"
#include "Map.h"
#include "Region.h"
#include "Sheet.h"

using namespace KSpread;

class DatabaseManager::Private
{
public:
    const Map* map;
    static int s_id;
};

int DatabaseManager::Private::s_id = 0;


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
    const KoXmlNode databaseRanges = KoDom::namedItemNS(body, KoXmlNS::table, "database-ranges");
    KoXmlElement element;
    forEachElement(element, databaseRanges)
    {
        if (element.namespaceURI() != KoXmlNS::table)
            continue;
        if (element.localName() == "database-range")
        {
            Database database;
            if (!database.loadOdf(element, d->map))
                return false;
            const Region region = database.range();
            if (region.isEmpty() || !region.isValid())
                continue;
            const Sheet* sheet = (*region.constBegin())->sheet();
            if (!sheet)
                continue;
            if (database.name().isEmpty())
                database.setName(createUniqueName());
            sheet->cellStorage()->setDatabase(region, database);
        }
    }
    return true;
}

void DatabaseManager::saveOdf(KoXmlWriter& xmlWriter) const
{
    xmlWriter.startElement("table:database-ranges");
    const QList<Sheet*>& sheets = d->map->sheetList();
    for (int i = 0; i < sheets.count(); ++i)
        sheets[i]->cellStorage()->saveOdfDatabases(xmlWriter);
    xmlWriter.endElement();
}

#include "DatabaseManager.moc"
