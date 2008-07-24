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

#include <QHash>

class QString;
//class QMap;

namespace KexiWebForms {
    namespace Model {

        class Database {
        public:
            Database() {}
            virtual ~Database() {}
            
            /**
             * Get all table names
             * @param KexiDB::ObjectTypes specify for which type of object we should retrieve names
             * @return QHash<QString, QString> a QHash with captions as keys and object names as values
             */
            QHash<QString, QString> getNames(KexiDB::ObjectTypes);
            
            /**
             * Create a new record
             * @param QString& the table name
             * @param QMap<const QString&, const QString&>& 
             */
            bool createRow(const QString&, const QMap<const QString&, const QString&>&);
        };
        
    }
}

#endif /* KEXIWEBFORMS_MODEL_DATABASE_H */
