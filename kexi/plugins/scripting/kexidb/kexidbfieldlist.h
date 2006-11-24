/***************************************************************************
 * kexidbfieldlist.h
 * This file is part of the KDE project
 * copyright (C)2004-2006 by Sebastian Sauer (mail@dipe.org)
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

#ifndef SCRIPTING_KEXIDBFIELDLIST_H
#define SCRIPTING_KEXIDBFIELDLIST_H

#include <qstring.h>
#include <qobject.h>

#include <kexidb/drivermanager.h>
#include <kexidb/fieldlist.h>

namespace Scripting {

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
            Q_OBJECT
        public:
            KexiDBFieldList(QObject* parent, ::KexiDB::FieldList* fieldlist, bool owner);
            virtual ~KexiDBFieldList();
            ::KexiDB::FieldList* fieldlist() { return m_fieldlist; }

        public slots:

            /** Returns the number of fields. */
            uint fieldCount();
            /** Return the \a KexiDBField specified by the index-number passed as an argument. */
            QObject* field(uint index);
            /** Return the \a KexiDBField specified by the as an argument passed fieldname. */
            QObject* fieldByName(const QString& name);

            /** Returns true if the passed \a KexiDBField \p field object is in the field list. */
            bool hasField(QObject* field);
            /** Return a list of field names. */
            const QStringList names() const;

            /** Adds the \a KexiDBField object passed as an argument to the field list. */
            bool addField(QObject* field);
            /** Inserts the \a KexiDBField object passed as the second argument
            into the field list at the position defined by the first argument. */
            bool insertField(uint index, QObject* field);
            /** Removes the \a KexiDBField object passed as an argument from the field list. */
            bool removeField(QObject* field);
            /** Removes all KexiDBField objects from the fieldlist. */
            void clear();

            /** Set the list of field to the as argument passed \a KexiDBFieldList \p fieldlist . */
            bool setFields(QObject* fieldlist);

            /** Creates and returns a \a KexiDBFieldList object that contain fields selected by name. */
            QObject* subList(QVariantList list);

        private:
            ::KexiDB::FieldList* m_fieldlist;
            bool m_owner;
    };

}

#endif

