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

#ifndef KEXIWEBFORMS_MODEL_DATABASE_H
#define KEXIWEBFORMS_MODEL_DATABASE_H

#include <QPair>
#include <QHash>

class QString;

namespace KexiWebForms {
    namespace Model {

        class Database {
        public:
            Database() {}
            virtual ~Database() {}
            
            /**
             * Get all table names
             * @param KexiDB::ObjectTypes specify for which type of object we should retrieve names
             * @return QHash with captions as keys and object names as values
             */
            QHash<QString, QString> getNames(KexiDB::ObjectTypes);

            /**
             * Get the schema of a table
             * @param QString& table name
             * @return a QHash with a QPair representing field caption and field name as key and
             *    a QPair representing the field value (if any) and the type as value 
             */
            QMap< QPair<QString, QString>, QPair<QString, KexiDB::Field::Type> > getSchema(const QString&,
                                                                                     const QString& pkey = "",
                                                                                     const uint pkeyValue = 0);

            /**
             * Despite its name, this method is useful when creating new rows, too
             * Create/Update a row in a given table
             * @param QString& the table name
             * @param QHash<QString, QVariant> a Hash with Name/Value pairs
             * @param create create a new row, instead of trying to update it
             */
            bool updateRow(const QString&, const QHash<QString, QVariant>, bool create = false, int pkeyValue = -1);
        };
        
    }
}

#endif /* KEXIWEBFORMS_MODEL_DATABASE_H */
