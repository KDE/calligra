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

#include <QHash>
#include <QString>
#include <QtAlgorithms>

#include <KDebug>

#include <kexidb/field.h>
#include <kexidb/cursor.h>
#include <kexidb/connection.h>
#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>

#include "DataProvider.h"

#include "Database.h"

namespace KexiWebForms {
    namespace Model {

        static bool caseInsensitiveLessThan(const QString &s1, const QString &s2) {
            return s1.toLower() < s2.toLower();
        }
        
        QHash<QString, QString> Database::getNames(KexiDB::ObjectTypes objectType) {
            QList<int> objectIds(gConnection->objectIds( objectType ));
            QHash<QString, QString> objectNamesForCaptions;
            
            foreach (const int id, objectIds) {
                KexiDB::SchemaData schema;
                tristate res = gConnection->loadObjectSchemaData( id, schema );
                if (res != true)
                    continue;
                objectNamesForCaptions.insertMulti( 
                    schema.captionOrName(), schema.name() ); //insertMulti() because there can be many objects with the same caption
            }
            return objectNamesForCaptions;
        }
        
        /*bool Database::create(const QString& table, const QMap<const QString&, const QString&> data) {
            return true;
            }*/

    }
}
