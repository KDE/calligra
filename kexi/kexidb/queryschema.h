/* This file is part of the KDE project
   Copyright (C) 2003-2006 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIDB_QUERY_H
#define KEXIDB_QUERY_H

#include <qvaluevector.h>
#include <qstring.h>
#include <qmap.h>
#include <qptrlist.h>

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

		QueryColumnInfo(Field *f, QCString _alias, bool _visible);
		~QueryColumnInfo();

		//! \return alias if it's not empty, field's name otherwise.
		inline QCString aliasOrName() const { 
			return alias.isEmpty() ? field->name().latin1() : (const char*)alias; 
		}

		//! \return field's caption if it's not empty, field's alias otherwise.
		//! If alias is also empty - returns field's name.
		inline QString captionOrAliasOrName() const {
			return field->caption().isEmpty() ? QString(aliasOrName()) : field->caption(); }

		Field *field;
		QCString alias;

		/*! Index of column with visible lookup value within the 'fields expanded' vector.
		 -1 means no visible lookup value is available because there is no lookup for the column.
		 Cached for efficiency as we use this information frequently.
		 @see LookupFieldSchema::visibleVolumn() */
		int indexForVisibleLookupValue;

		//! true if this column is visible to the user
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
		 it is defined just adding all the fields to the query in natural order.
		 To avoid problems (e.g. with fields added outside of Kexi using ALTER TABLE) 
		 we do not use "all-tables query asterisk" (see QueryAsterisk) item to achieve 
		 this effect. 

		 Properties such as the name and caption of the query are inherited 
		 from table schema.

		 We consider that query schema based on \a table is not (a least yet) stored 
		 in a system table, so query connection is set to NULL
		 (even if \a tableSchema's connection is not NULL).
		 Id of the created query is set to 0. */
		QuerySchema(TableSchema* tableSchema);
		
		virtual ~QuerySchema();
		
		/*! Inserts \a field to the columns list at \a position.
		 Inserted field will not be owned by this QuerySchema object,
		 but still by corresponding TableSchema. 
		 
		 As \a field object you can also pass KexiDB::QueryAsterisk,
		 (see QueryAsterisk class description).
		 
		 Note: After inserting a field, corresponding table will be automatically 
		 added to query's tables list if it is not present there (see tables()).
		 Field must have its table assigned. 
		 
		 Added field will be visible. Use insertField(position, field, false)
		 to add invisible field.
		*/
		virtual FieldList& insertField(uint position, Field *field);

		/* Like above method, but you can also set column's visibility. 
		 New column isn't bound explicity to any table.
		*/
		FieldList& insertField(uint position, Field *field, bool visible);

		/* Like above method, but you can also explicity bound the new column
		 to specific position on tables list. 
		 If bindToTable==-1, no particular table should be bound.
		 @see tableBoundToColumn(uint columnPosition) */
		FieldList& insertField(uint position, Field *field, 
			int bindToTable, bool visible = true);
		
		/*! Adds \a field to the columns list.
		 \sa insertField() */
		FieldList& addField(Field* field, bool visible = true);
		
		/*! Adds \a field to the columns list. Also binds to a table 
		 at \a bindToTable position. Use bindToTable==-1 if no table should be bound.
		 \sa insertField()
		 \sa tableBoundToColumn(uint columnPosition)
		*/
		FieldList& addField(Field* field, int bindToTable, 
			bool visible = true);

		/*! Removes field from the columns list. Use with care. */
		virtual void removeField(Field *field);

		/*! \return visibility flag for column at \a position. 
		 By default column is visible. */
		bool isColumnVisible(uint position) const;

		//! Sets visibility flag for column at \a position to \a v.
		void setColumnVisible(uint position, bool v);

		/*! Adds \a asterisk at the and of columns list. */
		FieldList& addAsterisk(QueryAsterisk *asterisk, bool visible = true);

		/*! Removes all columns and their aliases from the columns list, 
		 removes all tables and their aliases from the tables list within this query.
		 Sets master table information to NULL.
		 Does not destroy any objects though. Clears name and all other properties. 
		 \sa FieldList::clear() */
		virtual void clear();

		/*! \return string for debugging purposes. */
		virtual QString debugString();

		/*! If query was created using a connection, 
			returns this connection object, otherwise NULL. */
		Connection* connection() const;
		
		/*! \return table that is master to this query. 
		 All potentially-editable columns within this query belong just to this table.
		 This method also can return NULL if there are no tables at all,
		 or if previously assigned master table schema has been removed 
		 with removeTable(). 
		 Every query that has at least one table defined, should have 
		 assigned a master table. 
		 If no master table is assigned explicitym but this method there is only 
		 one table used for this query even if there are table aliases,
		 a single table is returned here.
		 (e.g. "T" table is returned for "SELECT T1.A, T2.B FROM T T1, T T2" statement). */
		TableSchema* masterTable() const;

		/*! Sets master table of this query to \a table.
			This table should be also added to query's tables list
			using addTable(). If \a table equals NULL, nothing is performed.
			\sa masterTable() */
		void setMasterTable(TableSchema *table);
		
		/*! \return list of tables used in a query. 
		 This also includes master table. 
		 \sa masterTable() */
		TableSchema::List* tables() const;

		/*! Adds \a table schema as one of tables used in a query.
		 if \a alias is not empty, it will be assigned to this table
		 using setTableAlias(position, alias)
		*/
		void addTable(TableSchema *table, const QCString& alias = QCString());

		/*! Removes \a table schema from this query. 
		 This does not destroy \a table object but only takes it out of the list. 
		 If this table was master for the query, master table information is also
		 invalidated. */
		void removeTable(TableSchema *table);

		/*! \return table with name \a tableName or 0 if this query has no such table. */
		TableSchema* table(const QString& tableName) const;
		
		/*! \return true if the query uses \a table. */
		bool contains(TableSchema *table) const;

		/*! Convenience function. 
		 \return table field by searching through all tables in this query.
		 The field does not need to be included on the list of query columns.
		 Similarly, query aliases are not taken into account.

		 \a tableDotFieldName string must contain table name and field name 
		 with '.' character between them, e.g. "mytable.myfield". 
		 0 is returned if the query has no such 
		 table defined of the table has no such field defined.

		 QuerySchema::table("mytable")->field("myfield") could be 
		 alternative for findTableField("mytable.myfield") but it can crash
		 if "mytable" is not defined in the query.
		*/
		Field* findTableField(const QString &tableDotFieldName) const;

		/*! \return alias of a column at \a position or null string 
		 If there is no alias for this column
		 or if there is no such column within the query defined. 
		 If the column is an expression and has no alias defined, 
		 a new unique alias will be generated automatically on this call.
		*/
		QCString columnAlias(uint position) const;
		
		/*! Provided for convenience. 
		 \return true if a column at \a position has non empty alias defined 
		 within the query.
		 If there is no alias for this column,
		 or if there is no such column in the query defined, false is returned. */
		bool hasColumnAlias(uint position) const;

		/*! Sets \a alias for a column at \a position, within the query. 
		 Passing empty string to \a alias clears alias for a given column. */
		void setColumnAlias(uint position, const QCString& alias);

		/*! \return a table position (within FROM section), 
		 that is bound to column at \a columnPosition (within SELECT section).
		 This information can be used to find if there is alias defined for 
		 a table that is referenced by a given column.
		 
		 For example, for "SELECT t2.id FROM table1 t1, table2 t2" query statement,
		 columnBoundToTable(0) returns 1, what means that table at position 1 
		 (within FROM section) is bound to column at position 0, so we can 
		 now call tableAlias(1) to see if we have used alias for this column (t2.d)
		 or just a table name (table2.d). 
		 
		 These checkings are performed e.g. by Connection::queryStatement() 
		 to construct a statement string maximally identical to originally 
		 defined query statement.
		 
		 -1 is returned if:
			- \a columnPosition is out of range (i.e. < 0 or >= fieldCount())
			- a column at \a columnPosition is not bound to any table (i.e. 
				no database field is used for this column, 
				e.g. "1" constant for "SELECT 1 from table" query statement)
		*/
		int tableBoundToColumn(uint columnPosition) const;

		/*! \return alias of a table at \a position (within FROM section) 
		 or null string if there is no alias for this table
		 or if there is no such table within the query defined. */
		QCString tableAlias(uint position) const;
		
		/*! \return table position (within FROM section) that has attached 
		 alias \a name.
		 If there is no such alias, -1 is returned.
		 Only first table's position attached for this alias is returned.
		 It is not especially bad, since aliases rarely can be duplicated, 
		 what leads to ambiguity.
		 Duplicated aliases are only allowed for trivial queries that have 
		 no database fields used within their columns, 
		 e.g. "SELECT 1 from table1 t, table2 t" is ok
		 but "SELECT t.id from table1 t, table2 t" is not.
		*/
		int tablePositionForAlias(const QCString& name) const;

		/*! \return table position (within FROM section) for \a tableName.
		 -1 is returend if there's no such table declared in the FROM section.
		 \sa tablePositions()
		*/
		int tablePosition(const QString& tableName) const;
		
		/*! \return a list of all \a tableName table occurences (within FROM section).
		 E.g. for "SELECT * FROM table t, table t2" [0, 1] list is returned.
		 Empty list is returned there's no such table declared 
		 in the FROM section at all.
		 \sa tablePosition()
		*/
		QValueList<int> tablePositions(const QString& tableName) const;

		/*! Provided for convenience. 
		 \return true if a table at \a position (within FROM section of the the query)
		 has non empty alias defined.
		 If there is no alias for this table,
		 or if there is no such table in the query defined, false is returned. */
		bool hasTableAlias(uint position) const;

		/*! \return column position that has defined alias \a name.
		 If there is no such alias, -1 is returned. */
		int columnPositionForAlias(const QCString& name) const;

		/*! Sets \a alias for a table at \a position (within FROM section 
		 of the the query).
		 Passing empty sting to \a alias clears alias for a given table
		 (only for specified \a position). */
		void setTableAlias(uint position, const QCString& alias);

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

		/*! \return field for \a name or 0 if no field for this name 
		 was found within the query. fieldsExpanded() method is used 
		 to lookup expanded list of the query fields, so queries with asterisks 
		 are processed well.
		 If a field has alias defined, name is not taken into account, 
		 but only it's alias. If a field has no alias:
		 - field's name is checked
		 - field's table and field's name are checked in a form of "tablename.fieldname",
		   so you can provide \a name of this form to avoid ambiguity.

		 If there are more than one fields with the same name equal to \a name,
		 first-found is returned (checking is performed from first to last query field).
		 Structures needed to compute result of this method are cached, 
		 so only first usage costs o(n) - another usages cost o(1). 

		 Example:
		 Let query be defined by "SELECT T.B AS X, * FROM T" statement and T 
		 is table containing fields A, B, C. 
		 Expanded list of columns for the query is: T.B AS X, T.A, T.B, T.C.
		 - Calling field("B") will return a pointer to third query column (not first, 
		   because it is covered by "X" alias). Additionally, calling field("X") 
		   will return the same pointer.
		 - Calling field("T.A") will return the same pointer as field("A").
		 */
		virtual Field* field(const QString& name);

		/*! Like QuerySchema::field(const QString& name) but returns not only Field
		 object for \a name but entire QueryColumnInfo object. */
		QueryColumnInfo* columnInfo(const QString& name);

		/*! Options used in fieldsExpanded(). */
		enum FieldsExpandedOptions {
			Default, //!< All fields are returned even if duplicated
			Unique, //!< Unique list of fields is returned
			WithInternalFields,        //!< Like Default but internal fields (for lookup) are appended
			WithInternalFieldsAndRowID //!< Like WithInternalFields but RowID (big int type) field 
			                           //!< is appended after internal fields
		};

		/*! \return fully expanded list of fields. 
		 QuerySchema::fields() returns vector of fields used for the query columns,
		 but in a case when there are asterisks defined for the query,
		 it does not expand QueryAsterisk objects to field lists but return every
		 asterisk as-is.
		 This could be inconvenient when you need just a fully expanded list of fields,
		 so this method does the work for you. 

		 If \a options is Unique, each field is returned in the vector only once 
		 (first found field is selected).
		 Note however, that the same field can be returned more than once if it has attached 
		 a different alias.
		 For example, let t be TABLE( a, b ) and let query be defined 
		 by "SELECT *, a AS alfa FROM t" statement. Either fieldsExpanded(true) and fieldsExpanded(false)
		 will return [ a, b, a (alfa) ] list.
		 On the other hand, for query defined by "SELECT *, a FROM t" statement,
		 fieldsExpanded(true) will return [ a, b ] list 
		 and fieldsExpanded(false) will return [ a, b, a ] list.

		 If \a options is WithInternalFields or WithInternalFieldsAndRowID, 
		 additional internal fields are also appended to the vector.
		 If \a options is WithRowIDAndInternalFields, 
		 one fake BigInteger column is appended to make space for ROWID column used 
		 by KexiDB::Cursor implementations. For example, let persons be TABLE( surname, city_id ), 
		 let city_number reference cities.is in TABLE cities( id, name ) and let query q be defined 
		 by "SELECT * FROM t" statement. If we want to display persons' city names instead of city_id's.
		 To do this, cities.name has to be retrieved as well, so the following statement should be used:
		 "SELECT * FROM persons, cities.name LEFT OUTER JOIN cities ON persons.city_id=cities.id".
		 Thus, calling fieldsExpanded(false, true, true) will return 4 elements instead of 2:
		 persons.surname, persons.city_id, cities.name, {ROWID}. The {ROWID} item is the placeholder 
		 used for fetching ROWID by KexiDB cursors.

		 By default, all fields are returned in the vector even 
		 if there are multiple occurences of one or more (options == Default). 

		 Note: You should assign the resulted vector in your space - it will be shared 
		 and implicity copied on any modification.
		 This method's result is cached by QuerySchema object.
@todo js: UPDATE CACHE!
		*/
		QueryColumnInfo::Vector fieldsExpanded(FieldsExpandedOptions options = Default);

		/*! \return list of fields internal fields used for lookup columns. */
		QueryColumnInfo::Vector internalFields();

		/*! \return info for expanded of internal field at index \a index. 
		 The returned field can be either logical or internal (for lookup),
		 the latter case is true if \a index &gt;= fieldsExpanded().count(). 
		 Equivalent of QuerySchema::fieldsExpanded(WithInternalFields).at(index). */
		QueryColumnInfo* expandedOrInternalField(uint index);

		/*! \return a map for fast lookup of query columns' order.
		 This is exactly opposite information compared to vector returned 
		 by fieldsExpanded(). This method's result is cached by QuerySchema object.
		 Note: indices of internal fields (see internalFields()) are also returned 
		 here - in this case the index is counted as a sum of size(e) + i (where "e" is 
		 the list of expanded fields and i is the column index within internal fields list).
		 This feature is used eg. at the end of Connection::updateRow() where need indices of
		 fields (including internal) to update all the values in memory.
@todo js: UPDATE CACHE!
		*/
		QMap<QueryColumnInfo*,int> fieldsOrder();

		/*! \return table describing order of primary key (PKEY) fields within the query.
		 Indexing is performed against vector returned by fieldsExpanded().
		 It is usable for e.g. Conenction::updateRow(), when we need 
		 to locate each primary key's field in a constant time.

		 Returned vector is owned and cached by QuerySchema object. When you assign it, 
		 it is implicity shared. Its size is equal to number of primary key
		 fields defined for master table (masterTable()->primaryKey()->fieldCount()).

		 Each element of the returned vector:
		 - can belong to [0..fieldsExpanded().count()-1] if there is such 
		   primary key's field in the fieldsExpanded() list.
		 - can be equal to -1 if there is no such primary key's field 
		   in the fieldsExpanded() list.

		 If there are more than one primary key's field included in the query, 
		 only first-found column (oin the fieldsExpanded() list) for each pkey's field is included.

		 Returns empty vector if there is no master table or no master table's pkey.
		 @see example for pkeyFieldsCount().
@todo js: UPDATE CACHE!
		*/
		QValueVector<int> pkeyFieldsOrder();

		/*! \return number of master table's primary key fields included in this query.
		 This method is useful to quickly check whether the vector returned by pkeyFieldsOrder()
		 if filled completely.

		 User e.g. in Connection::updateRow() to check if entire primary 
		 key information is specified.

		 Examples: let table T has (ID1 INTEGER, ID2 INTEGER, A INTEGER) fields, 
		 and let (ID1, ID2) is T's primary key.
		 -# The query defined by "SELECT * FROM T" statement contains all T's 
		    primary key's fields as T is the master table, and thus pkeyFieldsCount() 
		    will return 2 (both primary key's fields are in the fieldsExpanded() list),
				and pkeyFieldsOrder() will return vector {0, 1}. 
		 -# The query defined by "SELECT A, ID2 FROM T" statement, and thus pkeyFieldsCount()
		    will return 1 (only one primary key's field is in the fieldsExpanded() list),
				and pkeyFieldsOrder() will return vector {-1, 1}, as second primary key's field 
				is at position #1 and first field is not specified at all within the query. 
		*/
		uint pkeyFieldsCount();

		/*! \return a list of field infos for all auto-incremented fields
		 from master table of this query. This result is cached for efficiency. 
		 fieldsExpanded() is used for that.
		*/
		QueryColumnInfo::List* autoIncrementFields();

		/*! \return a preset statement (if any). */
		QString statement() const;

		/*! Forces a query statement (i.e. no statement is composed from QuerySchema's content) */
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

		/*! Sets a WHERE expression \a exp. It will be owned by this query, 
		 so you can forget about it. Previously set WHERE expression will be deleted.
		 You can pass 0 to remove expresssion. */
		void setWhereExpression(BaseExpr *expr);

		/*! \return WHERE expression or 0 if this query has no WHERE expression */
		BaseExpr *whereExpression() const;
	
		/*! Adds a part to WHERE expression. 
		 Simplifies creating of WHERE expression, if used instead 
		 of setWhereExpression(BaseExpr *expr). */
		void addToWhereExpression(KexiDB::Field *field, const QVariant& value, int relation = '=');

		/*! Sets a list of columns for ORDER BY section of the query. 
		 Each name on the list must be a field or alias present within the query 
		 and must not be covered by aliases. If one or more names cannot be found within 
		 the query, the method will have no effect. Any previous ORDER BY settings will be removed. 
		 */
		void setOrderByColumnList(const QStringList& columnNames);

		/*! Convenience method, similar to setOrderByColumnList(const QStringList&). */
		void setOrderByColumnList(const QString& column1, const QString& column2 = QString::null, 
			const QString& column3 = QString::null, const QString& column4 = QString::null, 
			const QString& column5 = QString::null);

		/*! \return a list of columns contained in ORDER BY section of the query. 
		 @see setOrderBy(const QStringList&) */
		QueryColumnInfo::Vector orderByColumnList() const;

	protected:
		void init();

		void computeFieldsExpanded();

	//		/*! Automatically retrieves query schema via connection. */
//		QuerySchema(Connection *conn);

		QuerySchemaPrivate *d;
		
	friend class Connection;
	friend class QuerySchemaPrivate;
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
