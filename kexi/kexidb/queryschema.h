/* This file is part of the KDE project
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

#ifndef KEXIDB_QUERY_H
#define KEXIDB_QUERY_H

#include <qvaluelist.h>
#include <qvaluevector.h>
#include <qvaluevector.h>
#include <qstring.h>
#include <qmap.h>
#include <qptrlist.h>
#include <qptrdict.h>
#include <qintdict.h>
#include <qbitarray.h>

#include <kexidb/fieldlist.h>
#include <kexidb/schemadata.h>
#include <kexidb/tableschema.h>
#include <kexidb/relationship.h>

namespace KexiDB {

class Connection;
class QueryAsterisk;
class QuerySchemaPrivate;

/*! Helper class that assigns additional information for the column in a query:
	- alias
	- visibility
	QueryColumnInfo::Vector is created and returned by QuerySchema::fieldsExpanded().
	It's efficiently cached there.
*/
class KEXI_DB_EXPORT QueryColumnInfo
{
	public:
		typedef QPtrVector<QueryColumnInfo> Vector;
		typedef QPtrList<QueryColumnInfo> List;
		typedef QPtrListIterator<QueryColumnInfo> ListIterator;
		
		QueryColumnInfo(Field *f, QCString _alias, bool _visible)
		 : field(f), alias(_alias), visible(_visible)
		{
		}
		Field *field;
		QCString alias;
		bool visible : 1;
};

/*! KexiDB::QuerySchema provides information about database query
	that can be executed using SQL database engine. 
*/
class KEXI_DB_EXPORT QuerySchema : public FieldList, public SchemaData
{
	public:
		/*! Creates empty query object (without columns). */
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
		
		/*! Inserts \a field to the columns list at \a index position.
		 Inserted field will not be owned by this QuerySchema object,
		 but still by corresponding TableSchema. 
		 
		 As \a field object you can also pass KexiDB::QueryAsterisk,
		 (see QueryAsterisk class description).
		 
		 Note: After inserting a field, corresponding table will be automatically 
		 added to query's tables list if it is not present there (see tables()).
		 Field must have its table assigned. 
		 
		 Added field will be visible. Use insertField(index, field, false)
		 to add invisible field.
		*/
		virtual FieldList& insertField(uint index, Field *field);

		/* Like above method, but you can also set column's visibility. */
		virtual FieldList& insertField(uint index, Field *field, bool visible);
		
		/*! Adds \a field to the columns list.
		 \sa insertField() */
		virtual KexiDB::FieldList& addField(KexiDB::Field* field, bool visible = true);

		/*! Removes field from the columns list. Use with care. */
		virtual void removeField(KexiDB::Field *field);

		/*! \return column's \a number visibility. By default column is visible. */
		bool isColumnVisible(uint number) const;

		//! Sets column's \a number visibility to \a v.
		void setColumnVisible(uint number, bool v);

		/*! Adds \a asterisk at the and of columns list. */
		FieldList& addAsterisk(QueryAsterisk *asterisk, bool visible = true);

//		int id() { return m_id; }
//		Field::List::iterator fields() { return m_fields.begin(); }
//js		void addPrimaryKey(const QString& key);

		/*! Removes all columns and their aliases from the columns list, 
		 removes all tables and their aliases from the tables list within this query.
		 Sets parent table information to NULL.
		 Does not destroy any objects though. Clears name and all other properties. 
		 \sa FieldList::clear() */
		virtual void clear();

		/*! \return string for debugging purposes. */
		virtual QString debugString();

		/*! If query was created using a connection, 
			returns this connection object, otherwise NULL. */
		Connection* connection();
		
		/*! \return table that is parent to this query. 
		 All potentially-editable columns within this query belong just to this table.
		 This method also can return NULL if there are no tables at all,
		 or if previously assigned parent table schema has been removed 
		 with removeTable(). 
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
		TableSchema::List* tables() const;

		/*! Adds \a table schema as one of tables used in a query. */
		void addTable(TableSchema *table);

		/*! Removes \a table schema from this query. 
		 This does not destroy \a table object but only takes it out of the list. 
		 If this table was parent for the query, parent table information is also
		 invalidated. */
		void removeTable(TableSchema *table);

		/*! \return table with name \a tableName or 0 if this query has no such table */
		TableSchema* table(const QString& tableName) const;
		
		/*! \return true if the query uses \a table. */
		bool contains(TableSchema *table) const;

		/*! \return alias of a column at \a index or null string 
		 If there is no alias for this column
		 or if there is no such column within the query defined */
		QCString columnAlias(uint index) const;
		
		/*! Provided for convenience. 
		 \return true if a column at \a index has non empty alias defined 
		 within the query.
		 If there is no alias for this column,
		 or if there is no such column in the query defined, false is returned. */
		bool hasColumnAlias(uint index) const;

		/*! Sets \a alias for a column at \a index, within the query. 
		 Passing empty sting to \a alias clears alias for a given column. */
		void setColumnAlias(uint index, const QCString& alias);

		/*! \return alias of a table at \a index or null string (within FROM section)
		 If there is no alias for this table
		 or if there is no such table within the query defined */
		QCString tableAlias(uint index) const;
		
		/*! Provided for convenience. 
		 \return true if a table at \a index (within FROM section of the the query)
		 has non empty alias defined.
		 If there is no alias for this table,
		 or if there is no such table in the query defined, false is returned. */
		bool hasTableAlias(uint index) const;

		/*! Sets \a alias for a table at \a index (within FROM section of the the query).
		 Passing empty sting to \a alias clears alias for a given table
		 (only for specified \a index). */
		void setTableAlias(uint index, const QCString& alias);

		/*! \return a list of relationships defined for this query */
		Relationship::List* relationships() const;

		/*! Adds a new relationship defined by \a field1 and \a field2.
		 Both fields should belong to two different tables of this query.
		 This is convenience function useful for a typical cases. 
		 It automatically creates Relationship object for this query. 
		 If one of the fields are primary keys, it will be detected 
		 and appropriate master-detail relation will be established.
		 This functiuon does nothing if the arguments are invalid. */
		Relationship* addRelationship( Field *field1, Field *field2 );

		/*! \return list of QueryAsterisk objects defined for this query */
		Field::List* asterisks() const;

		/*! QuerySchema::fields() returns vector of fields used for the query columns,
		 but in a case when there are asterisks defined for the query,
		 it does not expand QueryAsterisk objects to field lists but return every
		 asterisk as-is.
		 This could be inconvenient when you need just full expanded list of fields,
		 so this method does the work for you. 

		 Note: You should assign the resulted vector in your space - it will be shared 
		 and implicity copied on any modification.
		 This method's result is cached by QuerySchema object.
@todo js: UPDATE CACHE!
		*/
		QueryColumnInfo::Vector fieldsExpanded();//QValueList<bool> *detailedVisibility = 0);

		/*! \return a map for fast lookup of query columns' order.
		 This is exactly opposite information compared to vector returned 
		 by fieldsExpanded(). This method's result is cached by QuerySchema object.
@todo js: UPDATE CACHE!
		*/
		QMap<QueryColumnInfo*,uint> fieldsOrder();

		/*! \return table describing order of PKEY fields within the query.
		 It is usable foe e.g. Conenction::updateRow(), when we need 
		 to locate each PKEY's field in a constant time.
		 Returned vector is owned by QuerySchema object, when you assign it, 
		 it is implicity shared.
		 Its size if equal to number of PKEY fields, i.e. 
		 == parentTable()->primaryKey()->fieldCount().
		 Returns empty vector if there is neither parent table nor parent table's pkey.
		 This method's result is cached by QuerySchema object.
@todo js: UPDATE CACHE!
		*/
		QValueVector<uint> pkeyFieldsOrder();

		/*! \return a list of field infos for all auto-incremented fields
		 from parent table of this query. This result is cached for efficiency. 
		 fieldsExpanded() is used for that.
		*/
		QueryColumnInfo::List* autoIncrementFields();

		/*! \return a preset statement (if any). */
		QString statement() const;

		//! forces a query statement (i.e. no statement is composed from QuerySchema's content)
		void setStatement(const QString &s);

		/*! \return a string that is a result of concatenating all column names 
		 for \a infolist, with "," between each one. 
		 This is usable e.g. as argument like "field1,field2" 
		 for "INSERT INTO (xxx) ..". The result of this method is effectively cached,
		 and it is invalidated when set of fields changes (e.g. using clear() 
		 or addField()).
		 
		 This method is similar to FieldList::sqlFieldsList() it just uses
		 QueryColumnInfo::List instead of Field::List.
		*/
		static QString sqlColumnsList(QueryColumnInfo::List* infolist, Driver *driver);

		/*! \return cached sql list created using sqlColumnsList() on a list returned
		 by autoIncrementFields(). */
		QString autoIncrementSQLFieldsList(Driver *driver);
		
	protected:
		void init();

	//		/*! Automatically retrieves query schema via connection. */
//		QuerySchema(Connection *conn);

		QuerySchemaPrivate *d;
		
	friend class Connection;
};

/*! This class encapsulates information about single asterisk in query definition.
 There are two types of query asterisks:
 
 1. "Single-table" asterisk, that references all fields of given table used
 in the query.
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

		/*! \return Query object for that this asterisk object is defined */
		QuerySchema *query() const { return static_cast<QuerySchema*>(m_parent); }

		/*! \return Table schema for this asterisk 
		 if it has "single-table" type (1st type) 
		 or NULL if it has "all-tables" type (2nd type) defined. */
		virtual TableSchema* table() const { return m_table; }

		/*! Sets table schema for this asterisk. 
		 \a table may be NULL - then the asterisk becames "all-tables" type asterisk. */
		virtual void setTable(TableSchema *table);

		/*! This is convenience method that returns true 
		 if the asterisk has "all-tables" type (2nd type).*/
		bool isSingleTableAsterisk() const { return m_table!=NULL; }
		
		/*! This is convenience method that returns true 
		 if the asterisk has "single-tables" type (2nd type).*/
		bool isAllTableAsterisk() const { return m_table==NULL; }
		
		/*! \return String for debugging purposes. */
		virtual QString debugString();

	protected:
		/*! Table schema for this asterisk */
		TableSchema* m_table;

	friend class QuerySchema;
};

} //namespace KexiDB

#endif
