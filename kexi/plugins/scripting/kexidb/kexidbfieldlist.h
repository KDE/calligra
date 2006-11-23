/***************************************************************************
 * kexidbfieldlist.h
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#ifndef KROSS_KEXIDB_KEXIDBFIELDLIST_H
#define KROSS_KEXIDB_KEXIDBFIELDLIST_H

#include <qstring.h>
#include <qobject.h>

#include <kexidb/drivermanager.h>
#include <kexidb/fieldlist.h>

namespace Kross { namespace KexiDB {

    // Forward declarations.
    class KexiDBField;
    class KexiDBFieldList;

    /**
    * A list of fields. The KexiDBFieldList can be used to handle KexiDBField objects
    * in a backend-independend way.
    *
    * Example (in Python) ;
    * @code
    * # Get the tableschema for the "dept" table.
    * table = connection.tableSchema("dept")
    * # Create a KexiDBFieldList based on the table and filled with the selected fields.
    * subfields = ["deptno","name","loc"]
    * fieldlist = table.fieldlist().subList(subfields)
    * # Create the "SELECT * from dept;" queryschema.
    * query = table.query()
    * # We change the queryschema to "SELECT deptno,name,loc FROM dept;" now.
    * query.fieldlist().setFields(fieldlist)
    * # and change the query to "SELECT deptno,name,loc FROM dept WHERE deptno=5;"
    * query.setWhereExpression("deptno=5")
    * # Execute the query and get a KexiDBCursor object as result which could be used to iterate through the result.
    * cursor = connection.executeQuerySchema(query)
    * @endcode
    */
    class KexiDBFieldList : public QObject
    {
        public:
            KexiDBFieldList(::KexiDB::FieldList* fieldlist);
            virtual ~KexiDBFieldList();
            ::KexiDB::FieldList* fieldlist() { return m_fieldlist; }

#if 0
        private:

            /** Returns the number of fields. */
            uint fieldCount();
            /** Return the field specified by the index-number passed as an argument. */
            KexiDBField* field(uint index);
            /** Return the field specified by the as an argument passed fieldname. */
            KexiDBField* fieldByName(const QString& name);

            /** Returns a list of all fields. */
            Kross::Api::List* fields();
            /** Returns true if the KexiDBField object passed as an argument is in the field list. */
            bool hasField(KexiDBField* field);
            /** Return a list of field names. */
            const QStringList names() const;

            /** Adds the KexiDBField object passed as an argument to the field list. */
            void addField(KexiDBField* field);
            /** Inserts the KexiDBField object passed as the second argument
            into the field list at the position defined by the first argument. */
            void insertField(uint index, KexiDBField* field);
            /** Removes the KexiDBField object passed as an argument from the field list. */
            void removeField(KexiDBField* field);
            /** Removes all KexiDBField objects from the fieldlist. */
            void clear();
            /** Set the fieldlist to the as argument passed list of fields. */
            void setFields(KexiDBFieldList* fieldlist);
            /** Creates and returns list that contain fields selected by name. */
            KexiDBFieldList* subList(Q3ValueList<QVariant> list);
#endif

        private:
            ::KexiDB::FieldList* m_fieldlist;
    };

}}

#endif

