/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDB_QUERY_H
#define KEXIDB_QUERY_H

#include <qvaluelist.h>
#include <qstring.h>
#include <qmap.h>
#include <qptrlist.h>

#include <kexidb/fieldlist.h>
#include <kexidb/schemadata.h>
#include <kexidb/tableschema.h>

namespace KexiDB {

class Connection;
class QueryAsterisk;

/*! KexiDB::QuerySchema provides information about database query
	that can be executed using SQL database engine. 
	
*/

class KEXI_DB_EXPORT QuerySchema : public FieldList, public SchemaData
{
	public:
		/*! Creates empty query object (without fields). */
		QuerySchema();

		/*! Creates query schema object that is equivalent to "SELECT * FROM table" 
		 sql command. Schema of \a table is used to contruct this query -- 
		 it is defined just by defining "all-tables query asterisk" (see QueryAsterisk)
		 item. Properties such as the name and caption of the query are inherited 
		 from table schema.

		 We consider that query schema based on \a table is not (a least yet) stored 
		 in a system table, so query connection is set to NULL
		 (even if \a tableSchema's connection is not NULL).
		 Id of the created query is set to 0. */
		QuerySchema(TableSchema* tableSchema);
		
		virtual ~QuerySchema();
		
		/*! Adds \a field to 
		 Added field will not be owned by this QuerySchema object,
		 but still by corresponding TableSchema. 
		 
		 As \a field object you can also pass KexiDB::QueryAsterisk,
		 (see QueryAsterisk class description).
		 
		 Note: After adding a field, corresponding table will be automatically 
		 added to query's tables list if it is not present there (see tables()).
		 Field bust have its table assigned. 
		 */
		virtual KexiDB::FieldList& addField(KexiDB::Field* field);

		/*! Adds \a asterisk at the and of field list. */
		FieldList& addAsterisk(QueryAsterisk *asterisk);

//		int id() { return m_id; }
//		Field::List::iterator fields() { return m_fields.begin(); }
//js		void addPrimaryKey(const QString& key);

		/*! Removes all fields and aliases from the list.
		 Removes all tables. Sets parent table information to NULL.
		 Does not destroy any objects though.
		 clears name and all other properties. 
		 \sa FieldList::clear() */
		virtual void clear();

		//! Outputs debug information.
		virtual void debug();

		/*! If query was created using a connection, 
			returns this connection object, otherwise NULL. */
		Connection* connection();
		
		/*! \return table that is parent to this query. 
		 Only potentially-editable fields 
		 in this query belong to this table.
		 This method also can return NULL if there is no tables at all,
		 or if previously parent table schema was removed with removeTable(). 
		 Every query that have at least one table defined, should have 
		 assigned a parent table. */
		TableSchema* parentTable() const;

		/*! Sets parent table of this query to \a table.
			This table should be also added to query's tables list
			using addTable(). If \a table equals NULL, nothing is performed.
			\sa parentTable() */
		void setParentTable(TableSchema *table);
		
		/*! \return list of tables used in a query. 
		 This also includes parent table. 
		 \sa parentTable() */
		TableSchema::List* tables() { return &m_tables; }

		/*! Adds \a table schema as one of tables used in a query. */
		void addTable(TableSchema *table);

		/*! Removes \a table schema from this query. 
		 This does not destroy \a table object but only takes it out of the list. 
		 If this table was parent for the query, parent table information is also
		 invalidated. */
		void removeTable(TableSchema *table);
		
		/*! \return true if the query uses \a table. */
		bool contains(TableSchema *table) { return m_tables.find(table)!=-1; }
		
		/*! \return alias of \a field ot empty string 
		 if there is no alias for \a field 
		 or if there is no such field within the query defined */
		QString alias(Field *field) const { return m_aliases[field]; }
		
		/*! This is convenience method. 
		 \return true if \a field has non empty alias defined within the query.
		 if there is no alias for \a field
		 or if there is no such field in the query defined, false is returned. */
		bool hasAlias(Field *field) const { return !m_aliases[field].isEmpty(); }

		/*! Sets \a alias for \a field within the query. 
		 Passing empty sting to \a alias clears alias for given field. */
		void setAlias(Field *field, const QString& alias);
		
		/*! \return list of QueryAsterisk objects defined for this query */
		Field::List* asterisks() { return &m_asterisks; }

		/*! QuerySchema::fields() returns list of fields used in query, but 
		 in a case when there are asterisks defined for the query, it return
		 does not expand QueryAsterisk objects to field lists but return asterisk as-is.
		 This could be inconvenient when you need just full expanede list of fields,
		 so this method does the work for you. 

		 Note: You should not permanently store a pointer to returned field list. 
		 Make a deep copy if the list if you need.
		*/
		Field::List* fieldsExpanded();

	protected:
		void init();

	//		/*! Automatically retrieves query schema via connection. */
//		QuerySchema(Connection *conn);

	//js	QStringList m_primaryKeys;
//		Index::List m_indices;
//		QString m_name;

//		int m_id; //! unique identifier used in kexi__objects for this query

//		/*! Connection that was used to retrieve this query schema (may be NULL). */
//js: conn from m_parent_table will be reused		Connection *m_conn; 
		/*! Parent table of the query. (may be NULL)
			Any data modifications can be performed if we know parent table.
			If null, query's records cannot be modified. */
		TableSchema *m_parent_table;
		
		/*! List of tables used in this query */
		TableSchema::List m_tables;
		
		/*! Used to mapping Fields to its aliases for this query */
		QMap<Field*, QString> m_aliases;
		
		/*! List of asterisks defined for this query  */
		Field::List m_asterisks;

		/*! Temporary field list for using in fieldsExpanded() */
		Field::List m_fieldsExpanded;

	friend class Connection;
};

/*! This class encapsulates information about single asterisk in query definition.
 There are two types of query asterisks:
 
 1. "Single-table" asterisk, that references all fields of given table used in the query.
 Example SQL statement: 
 \code
 SELECT staff.*, cars.model from staff, cars WHERE staff.car = cars.number;
 \endcode
 The "staff.*" element is our "single-table" asterisk;
 this tells us that we want to get all fields of table "staff".
 
 2. "All-tables" asterisk, that references all fields of all tables used in the query.
 Example SQL statement: 
 \code
 SELECT * from staff, cars WHERE staff.car = cars.number;
 \endcode
 The "*" is our "all-tables" asterisk;
 this tells us that we want to get all fields of all used tables (here: "staff" and "cars").
 
 There can be many asterisks of 1st type defined for given single query.
 There can be one asterisk of 2nd type defined for given single query.
*/

class KEXI_DB_EXPORT QueryAsterisk : protected Field
{
	public:
		/*! Constructs query asterisk definition object.
		 Pass table schema to \a table if this asterisk should be 
		 of type "single-table", otherwise (if you want to define 
		 "all-tables" type asterisk), omit this parameter. 
		 
		 QueryAsterisk objects are owned by QuerySchema object
		 (not by TableSchema object like for ordinary Field objects)
		 for that the QueryAsterisk object was added (using QuerySchema::addField()).
		 */
		QueryAsterisk( QuerySchema *query, TableSchema *table = 0 );
		
		virtual ~QueryAsterisk();

		/*! \return query object for that this asterisk object is defined */
		QuerySchema *query() const { return static_cast<QuerySchema*>(m_parent); }

		/*! \return table schema for this asterisk 
		 if it has "single-table" type (1st type) 
		 or NULL if it has "all-tables" type (2nd type) defined. */
		virtual TableSchema* table() const { return m_table; }

		/*! This is convenience method that returns true 
		 if the asterisk has "all-tables" type (2nd type).*/
		bool isSingleTableAsterisk() const { return m_table!=NULL; }
		
		/*! This is convenience method that returns true 
		 if the asterisk has "single-tables" type (2nd type).*/
		bool isAllTableAsterisk() const { return m_table==NULL; }
		
		//! \return string for debugging purposes.
		virtual QString debugString() const;
	
	protected:
		/*! Table schema for this asterisk */
		TableSchema* m_table;

	friend class QuerySchema;
};

} //namespace KexiDB

#endif
