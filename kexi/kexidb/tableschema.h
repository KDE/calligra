/* This file is part of the KDE project
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIDB_TABLE_H
#define KEXIDB_TABLE_H

#include <qvaluelist.h>
#include <qptrlist.h>
#include <qstring.h>

#include <kexidb/fieldlist.h>
#include <kexidb/schemadata.h>
#include <kexidb/indexschema.h>
#include <kexidb/relationship.h>

namespace KexiDB {

class Connection;

/*! KexiDB::TableSchema provides information about native database table 
	that can be stored using SQL database engine. 
*/	

class KEXI_DB_EXPORT TableSchema : public FieldList, public SchemaData
{
	public:
		typedef QPtrList<TableSchema> List; //!< Type of tables list
		typedef QPtrListIterator<TableSchema> ListIterator; //!< Iterator for tables list

		TableSchema(const QString & name);
		TableSchema(const SchemaData& sdata);
		TableSchema();

		/*! Copy constructor. */
		TableSchema(const TableSchema& ts);
		
		virtual ~TableSchema();
		
		/*! Inserts \a field into a specified position (\a index).
		 'order' property of \a field is set automatically. */
		virtual FieldList& insertField(uint index, Field *field);

		/*! Reimplemented for internal reasons. */
		virtual void removeField(KexiDB::Field *field);

		/*! \return list of fields that are primary key of this table.
		 This method never returns NULL value,
		 if there is no primary key, empty IndexSchema object is returned.
		 IndexSchema object is owned by the table schema. */
		IndexSchema* primaryKey() const { return m_pkey; }

		/*! Sets table's primary key index to \a pkey. Pass pkey==NULL if you want to unassign
		 existing primary key ("primary" property of given IndexSchema object will be
		 cleared then so this index becomes ordinary index, still existing on table indeices list). 
		 
		 If this table already has primary key assigned, 
		 it is unassigned using setPrimaryKey(NULL) call.
		 
		 Before assigning as primary key, you should add the index to indices list 
		 with addIndex() (this is not done automatically!).
		*/
		void setPrimaryKey(IndexSchema *pkey);

		const IndexSchema::ListIterator indicesIterator() const 
			{ return IndexSchema::ListIterator(m_indices); }

		const IndexSchema::List* indices() { return &m_indices; }

		/*! Removes all fields from the list, clears name and all other properties. 
			\sa FieldList::clear() */
		virtual void clear();

		/*! \return String for debugging purposes. */
		virtual QString debugString();

		/*! if table was created using a connection, 
			returns this connection object, otherwise NULL. */
		Connection* connection() const { return m_conn; }

		/*! \return true if this is KexiDB storage system's table 
		 (used internally by KexiDB). This helps in hiding such tables
		 in applications (if desired) and will also enable lookup of system 
		 tables for schema export/import functionality. 
		 
		 Any internal KexiDB system table's schema (kexi__*) has 
		 cleared its SchemaData part, e.g. id=-1 for such table,
		 and no description, caption and so on. This is because
		 it represents a native database table rather that extended Kexi table.
		 
		 isKexiDBSystem()==true implies isNative()==true.
		 
		 By default (after allocation), TableSchema object 
		 has this property set to false. */
		bool isKexiDBSystem() const { return m_isKexiDBSystem; }

		/*! Sets KexiDBSystem flag to on or off. When on, native flag is forced to be on.
		 When off, native flag is not affected.
		 \sa isKexiDBSystem() */
		void setKexiDBSystem(bool set);

		/*! \return true if this is schema of native database object,
		 When this is kexiDBSystem table, native flag is forced to be on. */
		virtual bool isNative() const { return m_native || m_isKexiDBSystem; }
		
		/* Sets native flag. Does not allow to set this off for system KexiDB table. */
		virtual void setNative(bool set);

		/*! \return query schema object that is defined by "select * from <this_table_name>"
		 This query schema object is owned by the table schema object.
		 It is convenient way to get such a query when it is available otherwise.
		 Always return non-null.
		*/
		QuerySchema* query();

		/*! */
		Field* anyNonPKField();

	protected:
		/*! Automatically retrieves table schema via connection. */
		TableSchema(Connection *conn, const QString & name = QString::null);

		void init();

		IndexSchema::List m_indices;

		Connection *m_conn;
		
		IndexSchema *m_pkey;

		QuerySchema *m_query; //!< cached query schema that is defined by "select * from <this_table_name>"

		class Private;
		Private *d;

	private:
		bool m_isKexiDBSystem : 1;

	friend class Connection;
};

} //namespace KexiDB

#endif
