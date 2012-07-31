/*
 * This file is part of the KDE project
 *
 * (C) Copyright 2008 by Lorenzo Villani <lvillani@binaryhelix.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <KDebug>

#include <db/field.h>
#include <db/cursor.h>
#include <db/connection.h>
#include <db/queryschema.h>
#include <db/tableschema.h>

#include "DataProvider.h"

#include "Cache.h"

namespace KexiWebForms { // begin namespace KexiWebForms
namespace Model {        // begin namespace Model

Cache* Cache::m_instance = 0;

Cache* Cache::getInstance() {
    if (!m_instance)
        m_instance = new Cache();
    return m_instance;
}

// Following stuff is ugly
bool Cache::updateCachedPkeys(const QString& requestedTable) {
    // FIXME: Check for errors
    if (cachedPkeys[requestedTable].isEmpty()) {
        kDebug() << "Cached Pkeys is empty, updating";
        KexiDB::TableSchema tableSchema(*gConnection->tableSchema(requestedTable));
        KexiDB::QuerySchema idSchema;
        idSchema.addField(tableSchema.primaryKey()->field(0));
        KexiDB::Cursor* cursor = gConnection->executeQuery(idSchema);
        while (cursor->moveNext()) {
            kDebug() << "Appending " << cursor->value(0).toUInt() << " to cache";
            cachedPkeys[requestedTable].append(cursor->value(0).toUInt());
        }
        if (cursor) {
            cursor->close();
            gConnection->deleteCursor(cursor);
        }
    }
    return true;
}

const QList<uint>& Cache::getCachedPkeys(const QString& requestedTable) {
    return cachedPkeys[requestedTable];
}

uint Cache::getCurrentCachePosition(const QString& requestedTable, uint pkeyValueUInt) {
    for (int i = 0; i < cachedPkeys[requestedTable].size(); i++) {
        if (cachedPkeys[requestedTable].at(i) == pkeyValueUInt)
            return i;
    }
    return 0;
}

} // end namespace Model
} // end namespace KexiWebForms
