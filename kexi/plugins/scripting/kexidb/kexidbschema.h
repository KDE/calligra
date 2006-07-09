/***************************************************************************
 * kexidbschema.h
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

#ifndef KROSS_KEXIDB_KEXIDBSCHEMA_H
#define KROSS_KEXIDB_KEXIDBSCHEMA_H

#include <qstring.h>

#include <api/object.h>
#include <api/class.h>

#include <kexidb/drivermanager.h>
#include <kexidb/schemadata.h>
#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>

namespace Kross { namespace KexiDB {

    /**
     * The KexiDBSchema template class spends a base to wrap from
     * \a ::KexiDB::SchemaData and \a ::KexiDB::FieldList inherited
     * classes.
     */
    template<class T>
    class KexiDBSchema : public Kross::Api::Class<T>
    {
        public:
            KexiDBSchema(const QString& name, ::KexiDB::SchemaData* schema, ::KexiDB::FieldList* fieldlist);
            virtual ~KexiDBSchema();

        protected:
            ::KexiDB::SchemaData* m_schema;
            ::KexiDB::FieldList* m_fieldlist;

        private:
            /// \return the name of the Schema.
            Kross::Api::Object::Ptr name(Kross::Api::List::Ptr);
            /// Set the name of the Schema.
            Kross::Api::Object::Ptr setName(Kross::Api::List::Ptr);

            /// \return the caption of the Schema.
            Kross::Api::Object::Ptr caption(Kross::Api::List::Ptr);
            /// Set the caption of the Schema.
            Kross::Api::Object::Ptr setCaption(Kross::Api::List::Ptr);

            /// \return a description of the Schema.
            Kross::Api::Object::Ptr description(Kross::Api::List::Ptr);
            /// Set a description of the Schema.
            Kross::Api::Object::Ptr setDescription(Kross::Api::List::Ptr);

            /// \return the KexiDBFieldList object this Schema holds.
            Kross::Api::Object::Ptr fieldlist(Kross::Api::List::Ptr);
    };

    /**
     * The KexiDBTableSchema class wraps \a ::KexiDB::TableSchema
     * objects.
     */
    class KexiDBTableSchema : public KexiDBSchema<KexiDBTableSchema>
    {
        public:
            KexiDBTableSchema(::KexiDB::TableSchema* tableschema);
            virtual ~KexiDBTableSchema();
            virtual const QString getClassName() const;
            ::KexiDB::TableSchema* tableschema();

        private:
            /** \return the \a KexiDBQuerySchema object that represents a
            "SELECT * FROM this_KexiDBTableSchema_object". */
            Kross::Api::Object::Ptr query(Kross::Api::List::Ptr);
    };

    /**
     * The KexiDBQuerySchema class wraps \a ::KexiDB::QuerySchema
     * objects.
     */
    class KexiDBQuerySchema : public KexiDBSchema<KexiDBQuerySchema>
    {
        public:
            KexiDBQuerySchema(::KexiDB::QuerySchema* queryschema);
            virtual ~KexiDBQuerySchema();
            virtual const QString getClassName() const;
            ::KexiDB::QuerySchema* queryschema();

        private:
            /// \return the SQL statement of this query schema.
            Kross::Api::Object::Ptr statement(Kross::Api::List::Ptr);
            /// Set the SQL statement of this query schema.
            Kross::Api::Object::Ptr setStatement(Kross::Api::List::Ptr);

            Kross::Api::Object::Ptr setWhereExpression(Kross::Api::List::Ptr);
    };

}}

#endif

