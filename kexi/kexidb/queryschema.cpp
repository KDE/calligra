/* This file is part of the KDE project
   Copyright (C) 2003-2007 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexidb/queryschema.h"
#include "kexidb/driver.h"
#include "kexidb/connection.h"
#include "kexidb/expression.h"
#include "kexidb/parser/sqlparser.h"
#include "utils.h"
#include "lookupfieldschema.h"

#include <assert.h>

#include <q3valuelist.h>
#include <q3asciidict.h>
#include <q3ptrdict.h>
#include <q3intdict.h>
#include <qbitarray.h>
//Added by qt3to4:
#include <Q3CString>

#include <kdebug.h>
#include <klocale.h>

using namespace KexiDB;

QueryColumnInfo::QueryColumnInfo(Field *f, const Q3CString& _alias, bool _visible, 
	QueryColumnInfo *foreignColumn)
 : field(f), alias(_alias), visible(_visible), m_indexForVisibleLookupValue(-1)
 , m_foreignColumn(foreignColumn)
{
}

QueryColumnInfo::~QueryColumnInfo()
{
}

QString QueryColumnInfo::debugString() const
{
	return field->name() + 
		( alias.isEmpty() ? QString() 
			: (QString::fromLatin1(" AS ") + QString(alias)) );
}

//=======================================
namespace KexiDB {
//! @internal
class QuerySchemaPrivate
{
	public:
		QuerySchemaPrivate(QuerySchema* q, QuerySchemaPrivate* copy = 0)
		 : query(q)
		 , masterTable(0)
		 , fakeRowIDField(0)
		 , fakeRowIDCol(0)
		 , maxIndexWithAlias(-1)
		 , visibility(64)
		 , fieldsExpanded(0)
		 , internalFields(0)
		 , fieldsExpandedWithInternalAndRowID(0)
		 , fieldsExpandedWithInternal(0)
		 , autoincFields(0)
		 , columnsOrder(0)
		 , columnsOrderWithoutAsterisks(0)
		 , columnsOrderExpanded(0)
		 , pkeyFieldsOrder(0)
		 , pkeyFieldsCount(0)
		 , tablesBoundToColumns(64, -1)
		 , tablePositionsForAliases(67, false)
		 , columnPositionsForAliases(67, false)
		 , whereExpr(0)
		 , ownedVisibleColumns(0)
		 , regenerateExprAliases(false)
		{
			columnAliases.setAutoDelete(true);
			tableAliases.setAutoDelete(true);
			asterisks.setAutoDelete(true);
			relations.setAutoDelete(true);
			tablePositionsForAliases.setAutoDelete(true);
			columnPositionsForAliases.setAutoDelete(true);
			visibility.fill(false);
			if (copy) {
				// deep copy
				*this = *copy;
				if (copy->fieldsExpanded)
					fieldsExpanded = new QueryColumnInfo::Vector(*copy->fieldsExpanded);
				if (copy->internalFields)
					internalFields = new QueryColumnInfo::Vector(*copy->internalFields);
				if (copy->fieldsExpandedWithInternalAndRowID)
					fieldsExpandedWithInternalAndRowID = new QueryColumnInfo::Vector(
						*copy->fieldsExpandedWithInternalAndRowID);
				if (copy->fieldsExpandedWithInternal)
					fieldsExpandedWithInternal = new QueryColumnInfo::Vector(
						*copy->fieldsExpandedWithInternal);
				if (copy->autoincFields)
					autoincFields = new QueryColumnInfo::List(*copy->autoincFields);
				if (copy->columnsOrder)
					columnsOrder = new QMap<QueryColumnInfo*,int>(*copy->columnsOrder);
				if (copy->columnsOrderWithoutAsterisks)
					columnsOrderWithoutAsterisks = new QMap<QueryColumnInfo*,int>(
						*copy->columnsOrderWithoutAsterisks);
				if (copy->columnsOrderExpanded)
					columnsOrderExpanded = new QMap<QueryColumnInfo*,int>(*copy->columnsOrderExpanded);
				if (copy->pkeyFieldsOrder)
					pkeyFieldsOrder = new Q3ValueVector<int>(*copy->pkeyFieldsOrder);
				if (copy->whereExpr)
					whereExpr = copy->whereExpr->copy();
				if (copy->fakeRowIDCol)
					fakeRowIDCol = new QueryColumnInfo(*copy->fakeRowIDCol);
				if (copy->fakeRowIDField)
					fakeRowIDField = new Field(*copy->fakeRowIDField);
				if (copy->ownedVisibleColumns)
					ownedVisibleColumns = new Field::List(*copy->ownedVisibleColumns);
			}
		}
		~QuerySchemaPrivate()
		{
			delete fieldsExpanded;
			delete internalFields;
			delete fieldsExpandedWithInternalAndRowID;
			delete fieldsExpandedWithInternal;
			delete autoincFields;
			delete columnsOrder;
			delete columnsOrderWithoutAsterisks;
			delete columnsOrderExpanded;
			delete pkeyFieldsOrder;
			delete whereExpr;
			delete fakeRowIDCol;
			delete fakeRowIDField;
			delete ownedVisibleColumns;
		}

		void clear()
		{
			columnAliases.clear();
			tableAliases.clear();
			asterisks.clear();
			relations.clear();
			masterTable = 0;
			tables.clear();
			clearCachedData();
			delete pkeyFieldsOrder;
			pkeyFieldsOrder=0;
			visibility.fill(false);
			tablesBoundToColumns = Q3ValueVector<int>(64,-1);
			tablePositionsForAliases.clear();
			columnPositionsForAliases.clear();
		}

		void clearCachedData()
		{
			orderByColumnList.clear();
			if (fieldsExpanded) {
				delete fieldsExpanded;
				fieldsExpanded = 0;
				delete internalFields;
				internalFields = 0;
				delete columnsOrder;
				columnsOrder = 0;
				delete columnsOrderWithoutAsterisks;
				columnsOrderWithoutAsterisks = 0;
				delete columnsOrderExpanded;
				columnsOrderExpanded = 0;
				delete autoincFields;
				autoincFields = 0;
				autoIncrementSQLFieldsList.clear();
				columnInfosByNameExpanded.clear();
				columnInfosByName.clear();
				delete ownedVisibleColumns;
				ownedVisibleColumns = 0;
			}
		}

		void setColumnAliasInternal(uint position, const Q3CString& alias)
		{
			columnAliases.replace(position, new Q3CString(alias));
			columnPositionsForAliases.replace(alias, new int(position));
			maxIndexWithAlias = qMax( maxIndexWithAlias, (int)position );
		}

		void setColumnAlias(uint position, const Q3CString& alias)
		{
			Q3CString *oldAlias = columnAliases.take(position);
			if (oldAlias) {
				tablePositionsForAliases.remove(*oldAlias);
				delete oldAlias;
			}
			if (alias.isEmpty()) {
				maxIndexWithAlias = -1;
			}
			else {
				setColumnAliasInternal(position, alias);
			}
		}

		bool hasColumnAliases()
		{
			tryRegenerateExprAliases();
			return !columnAliases.isEmpty();
		}

		Q3CString* columnAlias(uint position)
		{
			tryRegenerateExprAliases();
			return columnAliases[position];
		}

		QuerySchema *query;

		/*! Master table of the query. (may be NULL)
			Any data modifications can be performed if we know master table.
			If null, query's records cannot be modified. */
		TableSchema *masterTable;
		
		/*! List of tables used in this query */
		TableSchema::List tables;

		Field *fakeRowIDField; //! used to mark a place for ROWID
		QueryColumnInfo *fakeRowIDCol; //! used to mark a place for ROWID

	protected:
		void tryRegenerateExprAliases()
		{
			if (!regenerateExprAliases)
				return;
			//regenerate missing aliases for experessions
			Field *f;
			uint p=0;
			uint colNum=0; //used to generate a name
			Q3CString columnAlias;
			for (Field::ListIterator it(query->fieldsIterator()); (f = it.current()); ++it, p++) {
				if (f->isExpression() && !columnAliases[p]) {
					//missing
					for (;;) { //find 1st unused
						colNum++;
						columnAlias = (i18nc("short for 'expression' word (only latin letters, please)", "expr").toLatin1() 
							+ QString::number(colNum)).toLatin1();
						if (!tablePositionsForAliases[columnAlias])
							break;
					}
					setColumnAliasInternal(p, columnAlias);
				}
			}
			regenerateExprAliases = false;
		}

		/*! Used to mapping columns to its aliases for this query */
		Q3IntDict<Q3CString> columnAliases;

	public:
		/*! Used to mapping tables to its aliases for this query */
		Q3IntDict<Q3CString> tableAliases;
		
		/*! Helper used with aliases */
		int maxIndexWithAlias;

		/*! Helper used with tableAliases */
		int maxIndexWithTableAlias;
		
		/*! Used to store visibility flag for every field */
		QBitArray visibility;

		/*! List of asterisks defined for this query  */
		Field::List asterisks;

		/*! Temporary field vector for using in fieldsExpanded() */
//		Field::Vector *fieldsExpanded;
		QueryColumnInfo::Vector *fieldsExpanded;

		/*! Temporary field vector containing internal fields used for lookup columns. */
		QueryColumnInfo::Vector *internalFields;

		/*! Temporary, used to cache sum of expanded fields and internal fields (+rowid) used for lookup columns. 
		 Contains not auto-deleted items.*/
		QueryColumnInfo::Vector *fieldsExpandedWithInternalAndRowID;

		/*! Temporary, used to cache sum of expanded fields and internal fields used for lookup columns. 
		 Contains not auto-deleted items.*/
		QueryColumnInfo::Vector *fieldsExpandedWithInternal;

		/*! A list of fields for ORDER BY section. @see QuerySchema::orderByColumnList(). */
		OrderByColumnList orderByColumnList;

		/*! A cache for autoIncrementFields(). */
		QueryColumnInfo::List *autoincFields;
		
		/*! A cache for autoIncrementSQLFieldsList(). */
		QString autoIncrementSQLFieldsList;
		QPointer<Driver> lastUsedDriverForAutoIncrementSQLFieldsList;

		/*! A map for fast lookup of query columns' order (unexpanded version). */
		QMap<QueryColumnInfo*,int> *columnsOrder;

		/*! A map for fast lookup of query columns' order (unexpanded version without asterisks). */
		QMap<QueryColumnInfo*,int> *columnsOrderWithoutAsterisks;

		/*! A map for fast lookup of query columns' order.
		 This is exactly opposite information compared to vector returned 
		 by fieldsExpanded() */
		QMap<QueryColumnInfo*,int> *columnsOrderExpanded;

//		QValueList<bool> detailedVisibility;

		/*! order of PKEY fields (e.g. for updateRow() ) */
		Q3ValueVector<int> *pkeyFieldsOrder;

		/*! number of PKEY fields within the query */
		uint pkeyFieldsCount;

		/*! forced (predefined) statement */
		QString statement;

		/*! Relationships defined for this query. */
		Relationship::List relations;

		/*! Information about columns bound to tables.
		 Used a table is used in FROM section more than once
		 (using table aliases).

		 This list is updated by insertField(uint position, Field *field, 
		 int bindToTable, bool visible), using bindToTable parameter.
		 
		 Example: for this statement: 
		 SELECT t1.a, othertable.x, t2.b FROM table t1, table t2, othertable; 
		 tablesBoundToColumns list looks like this:
		 [ 0, -1, 1 ]
		 - first column is bound to table 0 "t1"
		 - second coulmn is not specially bound (othertable.x isn't ambiguous)
		 - third column is bound to table 1 "t2"
		*/
		Q3ValueVector<int> tablesBoundToColumns;
		
		/*! Collects table positions for aliases: used in tablePositionForAlias(). */
		Q3AsciiDict<int> tablePositionsForAliases;

		/*! Collects column positions for aliases: used in columnPositionForAlias(). */
		Q3AsciiDict<int> columnPositionsForAliases;

		/*! WHERE expression */
		BaseExpr *whereExpr;

		Q3Dict<QueryColumnInfo> columnInfosByNameExpanded;

		Q3Dict<QueryColumnInfo> columnInfosByName; //!< Same as columnInfosByNameExpanded but asterisks are skipped

		//! field schemas created for multiple joined columns like a||' '||b||' '||c
		Field::List *ownedVisibleColumns;

		/*! Set by insertField(): true, if aliases for expression columns should 
		 be generated on next columnAlias() call. */
		bool regenerateExprAliases : 1;
};
}

//=======================================

OrderByColumn::OrderByColumn()
 : m_column(0)
 , m_pos(-1)
 , m_field(0)
 , m_ascending(true)
{
}

OrderByColumn::OrderByColumn(QueryColumnInfo& column, bool ascending, int pos)
 : m_column(&column)
 , m_pos(pos)
 , m_field(0)
 , m_ascending(ascending)
{
}

OrderByColumn::OrderByColumn(Field& field, bool ascending)
 : m_column(0)
 , m_pos(-1)
 , m_field(&field)
 , m_ascending(ascending)
{
}

OrderByColumn::~OrderByColumn()
{
}

QString OrderByColumn::debugString() const
{
	QString orderString( m_ascending ? "ascending" : "descending" );
	if (m_column) {
		if (m_pos>-1)
			return QString("COLUMN_AT_POSITION_%1(%2, %3)")
				.arg(m_pos+1).arg(m_column->debugString()).arg(orderString);
		else
			return QString("COLUMN(%1, %2)").arg(m_column->debugString()).arg(orderString);
	}
	return m_field ? QString("FIELD(%1, %2)").arg(m_field->debugString()).arg(orderString)
	 : QString("NONE");
}

QString OrderByColumn::toSQLString(bool includeTableName, Driver *drv, int identifierEscaping) const
{
	const QString orderString( m_ascending ? "" : " DESC" );
	QString fieldName, tableName;
	if (m_column) {
		if (m_pos>-1)
			return QString::number(m_pos+1) + orderString;
		else {
			if (includeTableName && m_column->alias.isEmpty()) {
				tableName = m_column->field->table()->name();
				if (drv)
					tableName = drv->escapeIdentifier(tableName, identifierEscaping);
				tableName += ".";
			}
			fieldName = m_column->aliasOrName();
			if (drv)
				fieldName = drv->escapeIdentifier(fieldName, identifierEscaping);
		}
	}
	else {
		if (includeTableName) {
			tableName = m_field->table()->name();
			if (drv)
				tableName = drv->escapeIdentifier(tableName, identifierEscaping);
			tableName += ".";
		}
		fieldName = m_field ? m_field->name() : "??"/*error*/;
		if (drv)
			fieldName = drv->escapeIdentifier(fieldName, identifierEscaping);
	}
	return tableName + fieldName + orderString;
}

//=======================================

OrderByColumnList::OrderByColumnList()
 : OrderByColumnListBase()
{
}

bool OrderByColumnList::appendFields(QuerySchema& querySchema,
	const QString& field1, bool ascending1, 
	const QString& field2, bool ascending2, 
	const QString& field3, bool ascending3, 
	const QString& field4, bool ascending4, 
	const QString& field5, bool ascending5)
{
	uint numAdded = 0;
#define ADD_COL(fieldName, ascending) \
	if (ok && !fieldName.isEmpty()) { \
		if (!appendField( querySchema, fieldName, ascending )) \
			ok = false; \
		else \
			numAdded++; \
	}
	bool ok = true;
	ADD_COL(field1, ascending1);
	ADD_COL(field2, ascending2);
	ADD_COL(field3, ascending3);
	ADD_COL(field4, ascending4);
	ADD_COL(field5, ascending5);
#undef ADD_COL
	if (ok)
		return true;
	for (uint i=0; i<numAdded; i++)
		pop_back();
	return false;
}

OrderByColumnList::~OrderByColumnList()
{
}

void OrderByColumnList::appendColumn(QueryColumnInfo& columnInfo, bool ascending)
{
	appendColumn( OrderByColumn(columnInfo, ascending) );
}

bool OrderByColumnList::appendColumn(QuerySchema& querySchema, bool ascending, int pos)
{
	QueryColumnInfo::Vector fieldsExpanded( querySchema.fieldsExpanded() );
	QueryColumnInfo* ci = (pos >= (int)fieldsExpanded.size()) ? 0 : fieldsExpanded[pos];
	if (!ci)
		return false;
	appendColumn( OrderByColumn(*ci, ascending, pos) );
	return true;
}

void OrderByColumnList::appendField(Field& field, bool ascending)
{
	appendColumn( OrderByColumn(field, ascending) );
}

bool OrderByColumnList::appendField(QuerySchema& querySchema, 
	const QString& fieldName, bool ascending)
{
	QueryColumnInfo *columnInfo = querySchema.columnInfo( fieldName );
	if (columnInfo) {
		appendColumn( OrderByColumn(*columnInfo, ascending) );
		return true;
	}
	Field *field = querySchema.findTableField(fieldName);
	if (field) {
		appendColumn( OrderByColumn(*field, ascending) );
		return true;
	}
	KexiDBWarn << "OrderByColumnList::addColumn(QuerySchema& querySchema, "
		"const QString& column, bool ascending): no such field \"" << fieldName << "\"" << endl;
	return false;
}
		
void OrderByColumnList::appendColumn(const OrderByColumn& column)
{
	append( column );
}

QString OrderByColumnList::debugString() const
{
	if (isEmpty())
		return "NONE";
	QString dbg;
	for (OrderByColumn::ListConstIterator it=constBegin(); it!=constEnd(); ++it) {
		if (!dbg.isEmpty())
			dbg += "\n";
		dbg += (*it).debugString();
	}
	return dbg;
}

QString OrderByColumnList::toSQLString(bool includeTableNames, Driver *drv, int identifierEscaping) const
{
	QString string;
	for (OrderByColumn::ListConstIterator it=constBegin(); it!=constEnd(); ++it) {
		if (!string.isEmpty())
			string += ", ";
		string += (*it).toSQLString(includeTableNames, drv, identifierEscaping);
	}
	return string;
}

//=======================================

QuerySchema::QuerySchema()
	: FieldList(false)//fields are not owned by QuerySchema object
	, SchemaData(KexiDB::QueryObjectType)
	, d( new QuerySchemaPrivate(this) )
{
	init();
}

QuerySchema::QuerySchema(TableSchema& tableSchema)
	: FieldList(false)
	, SchemaData(KexiDB::QueryObjectType)
	, d( new QuerySchemaPrivate(this) )
{
	d->masterTable = &tableSchema;
	init();
	/*if (!d->masterTable) {
		KexiDBWarn << "QuerySchema(TableSchema*): !d->masterTable" << endl;
		m_name.clear();
		return;
	}*/
	addTable(d->masterTable);
	//defaults:
	//inherit name from a table
	m_name = d->masterTable->name();
	//inherit caption from a table
	m_caption = d->masterTable->caption();
	
//replaced by explicit field list: //add all fields of the table as asterisk:
//replaced by explicit field list:	addField( new QueryAsterisk(this) );

	// add explicit field list to avoid problems (e.g. with fields added outside of Kexi):
	for (Field::ListIterator it( d->masterTable->fieldsIterator() ); it.current(); ++it) {
		addField( it.current() );
	}
}

QuerySchema::QuerySchema(const QuerySchema& querySchema)
	: FieldList(querySchema, false /* !deepCopyFields */)
	, SchemaData(querySchema)
	, d( new QuerySchemaPrivate(this, querySchema.d) )
{
	//only deep copy query asterisks
	for (Field::ListIterator f_it(querySchema.m_fields); f_it.current(); ++f_it) {
		Field *f;
		if (dynamic_cast<QueryAsterisk*>( f_it.current() )) {
			f = f_it.current()->copy();
			if (static_cast<const KexiDB::FieldList *>(f_it.current()->m_parent) == &querySchema)
				f->m_parent = this;
		}
		else
			f = f_it.current();
		addField( f );
	}
}

QuerySchema::~QuerySchema()
{
	delete d;
}

void QuerySchema::init()
{
	m_type = KexiDB::QueryObjectType;
//m_fields_by_name.setAutoDelete( true ); //because we're using QueryColumnInfoEntry objects
}

void QuerySchema::clear()
{
	FieldList::clear();
	SchemaData::clear();
	d->clear();
}

FieldList& QuerySchema::insertField(uint position, Field *field, bool visible)
{
	return insertField(position, field, -1/*don't bind*/, visible);
}

/*virtual*/
FieldList& QuerySchema::insertField(uint position, Field *field)
{
	return insertField( position, field, -1/*don't bind*/, true );
}

FieldList& QuerySchema::insertField(uint position, Field *field, 
	int bindToTable, bool visible)
{
	if (!field) {
		KexiDBWarn << "QuerySchema::insertField(): !field" << endl;
		return *this;
	}

	if (position>m_fields.count()) {
		KexiDBWarn << "QuerySchema::insertField(): position (" << position << ") out of range" << endl;
		return *this;
	}
	if (!field->isQueryAsterisk() && !field->isExpression() && !field->table()) {
		KexiDBWarn << "QuerySchema::insertField(): WARNING: field '"<<field->name()
			<<"' must contain table information!" <<endl;
		return *this;
	}
	if ((int)fieldCount()>=d->visibility.size()) {
		d->visibility.resize(d->visibility.size()*2);
		d->tablesBoundToColumns.resize(d->tablesBoundToColumns.size()*2);
	}
	d->clearCachedData();
	FieldList::insertField(position, field);
	if (field->isQueryAsterisk()) {
		d->asterisks.append(field);
		//if this is single-table asterisk,
		//add a table to list if doesn't exist there:
		if (field->table() && (d->tables.findRef(field->table())==-1))
			d->tables.append(field->table());
	}
	else if (field->table()) {
		//add a table to list if doesn't exist there:
		if (d->tables.findRef(field->table())==-1)
			d->tables.append(field->table());
	}
//	//visible by default
//	setFieldVisible(field, true);
//	d->visibility.setBit(fieldCount()-1, visible);
	//update visibility
	//--move bits to make a place for a new one
	for (uint i=fieldCount()-1; i>position; i--)
		d->visibility.setBit(i, d->visibility.testBit(i-1));
	d->visibility.setBit(position, visible);

	//bind to table
	if (bindToTable < -1 && bindToTable>(int)d->tables.count()) {
		KexiDBWarn << "QuerySchema::insertField(): bindToTable (" << bindToTable 
			<< ") out of range" << endl;
		bindToTable = -1;
	}
	//--move items to make a place for a new one
	for (uint i=fieldCount()-1; i>position; i--)
		d->tablesBoundToColumns[i] = d->tablesBoundToColumns[i-1];
	d->tablesBoundToColumns[ position ] = bindToTable;
	
	KexiDBDbg << "QuerySchema::insertField(): bound to table (" << bindToTable << "): " <<endl;
	if (bindToTable==-1)
		KexiDBDbg << " <NOT SPECIFIED>" << endl; 
	else
		KexiDBDbg << " name=" << d->tables.at(bindToTable)->name() 
			<< " alias=" << tableAlias(bindToTable) <<  endl;
	QString s;
	for (uint i=0; i<fieldCount();i++)
		s+= (QString::number(d->tablesBoundToColumns[i]) + " ");
	KexiDBDbg << "tablesBoundToColumns == [" << s << "]" <<endl;

	if (field->isExpression())
		d->regenerateExprAliases = true;

	return *this;
}

int QuerySchema::tableBoundToColumn(uint columnPosition) const
{
	if ((int)columnPosition > d->tablesBoundToColumns.count()) {
		KexiDBWarn << "QuerySchema::tableBoundToColumn(): columnPosition (" << columnPosition
			<< ") out of range" << endl;
		return -1;
	}
	return d->tablesBoundToColumns[columnPosition];
}

KexiDB::FieldList& QuerySchema::addField(KexiDB::Field* field, bool visible)
{
	return insertField(m_fields.count(), field, visible);
}

KexiDB::FieldList& QuerySchema::addField(KexiDB::Field* field, int bindToTable, 
	bool visible)
{
	return insertField(m_fields.count(), field, bindToTable, visible);
}

void QuerySchema::removeField(KexiDB::Field *field)
{
	if (!field)
		return;
	d->clearCachedData();
	if (field->isQueryAsterisk()) {
		d->asterisks.remove(field); //this will destroy this asterisk
	}
//TODO: should we also remove table for this field or asterisk?
	FieldList::removeField(field);
}

FieldList& QuerySchema::addExpression(BaseExpr* expr, bool visible)
{
	return addField( new Field(this, expr), visible );
}

bool QuerySchema::isColumnVisible(uint position) const
{
	return (position < fieldCount()) ? d->visibility.testBit(position) : false;
}

void QuerySchema::setColumnVisible(uint position, bool v)
{
	if (position < fieldCount())
		d->visibility.setBit(position, v);
}

FieldList& QuerySchema::addAsterisk(QueryAsterisk *asterisk, bool visible)
{
	if (!asterisk)
		return *this;
	//make unique name
	asterisk->m_name = (asterisk->table() ? asterisk->table()->name() + ".*" : "*") 
		+ QString::number(asterisks()->count());
	return addField(asterisk, visible);
}

Connection* QuerySchema::connection() const
{
	TableSchema *mt = masterTable();
	return mt ? mt->connection() : 0;
}

QString QuerySchema::debugString()
{
	QString dbg;
	dbg.reserve(1024);
	//fields
	TableSchema *mt = masterTable();
	dbg = QString("QUERY ") + schemaDataDebugString() + "\n"
		+ "-masterTable=" + (mt ? mt->name() :"<NULL>")
		+ "\n-COLUMNS:\n"
		+ ((fieldCount()>0) ? FieldList::debugString() : "<NONE>") + "\n"
		+ "-FIELDS EXPANDED ";

	QString dbg1;
	uint fieldsExpandedCount = 0;
	if (fieldCount()>0) {
		QueryColumnInfo::Vector fe( fieldsExpanded() );
		fieldsExpandedCount = fe.size();
		for ( uint i=0; i < fieldsExpandedCount; i++ ) {
			QueryColumnInfo *ci = fe[i];
			if (!dbg1.isEmpty())
				dbg1 += ",\n";
			dbg1 += ci->debugString();
		}
		dbg1 += "\n";
	}
	else {
		dbg1 = "<NONE>\n";
	}
	dbg1.prepend( QString("(%1):\n").arg(fieldsExpandedCount) );
	dbg += dbg1;

	//it's safer to delete fieldsExpanded for now 
	// (debugString() could be called before all fields are added)
//causes a crash	d->clearCachedData();

	//bindings
	QString dbg2;
	dbg2.reserve(512);
	for (uint i = 0; i<fieldCount(); i++) {
		int tablePos = tableBoundToColumn(i);
		if (tablePos>=0) {
			Q3CString tAlias = tableAlias(tablePos);
			if (!tAlias.isEmpty()) {
				dbg2 += (QString::fromLatin1(" field \"") + FieldList::field(i)->name() 
					+ "\" uses alias \"" + QString(tAlias) + "\" of table \""
					+ d->tables.at(tablePos)->name() + "\"\n");
			}
		}
	}
	if (!dbg2.isEmpty()) {
		dbg += "\n-BINDINGS:\n";
		dbg += dbg2;
	}
	
	//tables	
	TableSchema *table;
	QString table_names;
	table_names.reserve(512);
	for ( table = d->tables.first(); table; table = d->tables.next() ) {
		if (!table_names.isEmpty())
			table_names += ", ";
		table_names += (QString("'") + table->name() + "'");
	}
	if (d->tables.isEmpty())
		table_names = "<NONE>";
	dbg += (QString("-TABLES:\n") + table_names);
	QString aliases;
	if (!d->hasColumnAliases())
		aliases = "<NONE>\n";
	else {
		Field::ListIterator it( m_fields );
		for (int i=0; it.current(); ++it, i++) {
			Q3CString *alias = d->columnAlias(i);
			if (alias)
				aliases += (QString("field #%1: ").arg(i) 
					+ (it.current()->name().isEmpty() ? "<noname>" : it.current()->name())
					+ " -> " + (const char*)*alias + "\n");
		}
	}
	//aliases
	dbg += QString("\n-COLUMN ALIASES:\n" + aliases);
	if (d->tableAliases.isEmpty())
		aliases = "<NONE>";
	else {
		aliases = "";
		TableSchema::ListIterator t_it(d->tables);
		for (int i=0; t_it.current(); ++t_it, i++) {
			Q3CString *alias = d->tableAliases[i];
			if (alias)
				aliases += (QString("table #%1: ").arg(i) 
					+ (t_it.current()->name().isEmpty() ? "<noname>" : t_it.current()->name())
					+ " -> " + (const char*)*alias + "\n");
		}
	}
	dbg += QString("-TABLE ALIASES:\n" + aliases);
	QString where = d->whereExpr ? d->whereExpr->debugString() : QString();
	if (!where.isEmpty())
		dbg += (QString("\n-WHERE EXPRESSION:\n") + where);
	if (!orderByColumnList().isEmpty())
		dbg += (QString("\n-ORDER BY (%1):\n").arg(orderByColumnList().count()) 
			+ orderByColumnList().debugString());
	return dbg;
}

TableSchema* QuerySchema::masterTable() const
{
	if (d->masterTable)
		return d->masterTable;
	if (d->tables.isEmpty())
		return 0;

	//try to find master table if there's only one table (with possible aliasses)
	int num = 0;
	QString tableNameLower;
	for (TableSchema::ListIterator it(d->tables); it.current(); ++it, num++) {
		if (!tableNameLower.isEmpty() && it.current()->name().toLower()!=tableNameLower) {
			//two or more different tables
			return 0;
		}
		tableNameLower = tableAlias(num);
	}
	return d->tables.first();
}

void QuerySchema::setMasterTable(TableSchema *table)
{ 
	if (table)
		d->masterTable=table; 
}

TableSchema::List* QuerySchema::tables() const
{
	return &d->tables;
}

void QuerySchema::addTable(TableSchema *table, const Q3CString& alias)
{
	KexiDBDbg << "QuerySchema::addTable() " << (void *)table 
		<< " alias=" << alias << endl;
	if (!table)
		return;
	
	//only append table if:
	//-it has alias
	//-it has no alias but there is no such table on the list
	if (alias.isEmpty() && d->tables.findRef(table)!=-1) {
		const QString& tableNameLower = table->name().toLower();
		const QString& aliasLower = QString(alias.toLower());
		int num = 0;
		for (TableSchema::ListIterator it(d->tables); it.current(); ++it, num++) {
			if (it.current()->name().toLower()==tableNameLower) {
				const QString& tAlias = tableAlias(num);
				if (tAlias == aliasLower) {
					KexiDBWarn << "QuerySchema::addTable(): table with \"" 
						<< tAlias << "\" alias already added!" << endl;
					return;
				}
			}
		}
	}
	
	d->tables.append(table);
	
	if (!alias.isEmpty())
		setTableAlias(d->tables.count()-1, alias);
}

void QuerySchema::removeTable(TableSchema *table)
{
	if (!table)
		return;
	if (d->masterTable == table)
		d->masterTable = 0;
	d->tables.remove(table);
	//todo: remove fields!
}

TableSchema* QuerySchema::table(const QString& tableName) const
{
//TODO: maybe use tables_byname?
	for (TableSchema::ListIterator it(d->tables); it.current(); ++it) {
		if (it.current()->name().toLower()==tableName.toLower())
			return it.current();
	}
	return 0;
}

bool QuerySchema::contains(TableSchema *table) const
{
	return d->tables.findRef(table)!=-1;
}

Field* QuerySchema::findTableField(const QString &tableOrTableAndFieldName) const
{
	QString tableName, fieldName;
	if (!KexiDB::splitToTableAndFieldParts(tableOrTableAndFieldName, 
		tableName, fieldName, KexiDB::SetFieldNameIfNoTableName)) {
		return 0;
	}
	if (tableName.isEmpty()) {
		for (TableSchema::ListIterator it(d->tables); it.current(); ++it) {
			if (it.current()->field(fieldName))
				return it.current()->field(fieldName);
		}
		return 0;
	}
	TableSchema *tableSchema = table(tableName);
	if (!tableSchema)
		return 0;
	return tableSchema->field(fieldName);
}

Q3CString QuerySchema::columnAlias(uint position) const
{
	Q3CString *a = d->columnAlias(position);
	return a ? *a : Q3CString();
}

bool QuerySchema::hasColumnAlias(uint position) const
{
	return d->columnAlias(position)!=0;
}

void QuerySchema::setColumnAlias(uint position, const Q3CString& alias)
{
	if (position >= m_fields.count()) {
		KexiDBWarn << "QuerySchema::setColumnAlias(): position ("  << position 
			<< ") out of range!" << endl;
		return;
	}
	Q3CString fixedAlias = alias.trimmed();
	Field *f = FieldList::field(position);
	if (f->captionOrName().isEmpty() && fixedAlias.isEmpty()) {
		KexiDBWarn << "QuerySchema::setColumnAlias(): position ("  << position 
			<< ") could not remove alias when no name is specified for expression column!" << endl;
		return;
	}
	d->setColumnAlias(position, fixedAlias);
}

Q3CString QuerySchema::tableAlias(uint position) const
{
	Q3CString *a = d->tableAliases[position];
	return a ? *a : Q3CString();
}

int QuerySchema::tablePositionForAlias(const Q3CString& name) const
{
	int *num = d->tablePositionsForAliases[name];
	if (!num)
		return -1;
	return *num;
}

int QuerySchema::tablePosition(const QString& tableName) const
{
	int num = 0;
	for (TableSchema::ListIterator it(d->tables); it.current(); ++it, num++) {
		if (it.current()->name().toLower()==tableName.toLower())
			return num;
	}
	return -1;
}

Q3ValueList<int> QuerySchema::tablePositions(const QString& tableName) const
{
	int num = 0;
	Q3ValueList<int> result;
	const QString& tableNameLower = tableName.toLower();
	for (TableSchema::ListIterator it(d->tables); it.current(); ++it, num++) {
		if (it.current()->name().toLower()==tableNameLower) {
			result += num;
		}
	}
	return result;
}

bool QuerySchema::hasTableAlias(uint position) const
{
	return d->tableAliases[position]!=0;
}

int QuerySchema::columnPositionForAlias(const Q3CString& name) const
{
	int *num = d->columnPositionsForAliases[name];
	if (!num)
		return -1;
	return *num;
}

void QuerySchema::setTableAlias(uint position, const Q3CString& alias)
{
	if (position >= d->tables.count()) {
		KexiDBWarn << "QuerySchema::setTableAlias(): position ("  << position 
			<< ") out of range!" << endl;
		return;
	}
	Q3CString fixedAlias = alias.trimmed();
	if (fixedAlias.isEmpty()) {
		Q3CString *oldAlias = d->tableAliases.take(position);
		if (oldAlias) {
			d->tablePositionsForAliases.remove(*oldAlias);
			delete oldAlias;
		}
//			d->maxIndexWithTableAlias = -1;
	}
	else {
		d->tableAliases.replace(position, new Q3CString(fixedAlias));
		d->tablePositionsForAliases.replace(fixedAlias, new int(position));
//		d->maxIndexWithTableAlias = qMax( d->maxIndexWithTableAlias, (int)index );
	}
}

Relationship::List* QuerySchema::relationships() const
{
	return &d->relations;
}

Field::List* QuerySchema::asterisks() const
{
	return &d->asterisks;
}
		
QString QuerySchema::statement() const
{
	return d->statement;
}

void QuerySchema::setStatement(const QString &s)
{
	d->statement = s;
}

Field* QuerySchema::field(const QString& identifier, bool expanded)
{
	QueryColumnInfo *ci = columnInfo(identifier, expanded);
	return ci ? ci->field : 0;
}

QueryColumnInfo* QuerySchema::columnInfo(const QString& identifier, bool expanded)
{
	computeFieldsExpanded();
	return expanded ? d->columnInfosByNameExpanded[identifier] : d->columnInfosByName[identifier];
}

QueryColumnInfo::Vector QuerySchema::fieldsExpanded(FieldsExpandedOptions options)
{
	computeFieldsExpanded();
	if (options == WithInternalFields || options == WithInternalFieldsAndRowID) {
		//a ref to a proper pointer (as we cache the vector for two cases)
		QueryColumnInfo::Vector*& tmpFieldsExpandedWithInternal = 
			(options == WithInternalFields) ? d->fieldsExpandedWithInternal : d->fieldsExpandedWithInternalAndRowID;
		//special case
		if (!tmpFieldsExpandedWithInternal) {
			//glue expanded and internal fields and cache it
			const uint size = d->fieldsExpanded->count()
				+ (d->internalFields ? d->internalFields->count() : 0)
				+ ((options == WithInternalFieldsAndRowID) ? 1 : 0) /*ROWID*/;
			tmpFieldsExpandedWithInternal = new QueryColumnInfo::Vector( size );
			const uint fieldsExpandedVectorSize = d->fieldsExpanded->size();
			for (uint i=0; i<fieldsExpandedVectorSize; i++)
				tmpFieldsExpandedWithInternal->insert(i, d->fieldsExpanded->at(i));
			const uint internalFieldsCount = d->internalFields ? d->internalFields->size() : 0;
			if (internalFieldsCount > 0) {
				for (uint i=0; i < internalFieldsCount; i++)
					tmpFieldsExpandedWithInternal->insert(
						fieldsExpandedVectorSize + i, d->internalFields->at(i));
			}
			if (options == WithInternalFieldsAndRowID) {
				if (!d->fakeRowIDField) {
					d->fakeRowIDField = new Field("rowID", Field::BigInteger);
					d->fakeRowIDCol = new QueryColumnInfo(d->fakeRowIDField, Q3CString(), true);
				}
				tmpFieldsExpandedWithInternal->insert( 
					fieldsExpandedVectorSize + internalFieldsCount, d->fakeRowIDCol );
			}
		}
		return *tmpFieldsExpandedWithInternal;
	}

	if (options == Default)
		return *d->fieldsExpanded;

	//options == Unique:
	Q3Dict<char> columnsAlreadyFound;
	QueryColumnInfo::Vector result( d->fieldsExpanded->count() ); //initial size is set
//	QMapConstIterator<QueryColumnInfo*, bool> columnsAlreadyFoundIt;
	//compute unique list
	uint uniqueListCount = 0;
	for (uint i=0; i<d->fieldsExpanded->count(); i++) {
		QueryColumnInfo *ci = (*d->fieldsExpanded)[i];
//		columnsAlreadyFoundIt = columnsAlreadyFound.find(ci);
//		uint foundColumnIndex = -1;
		if (!columnsAlreadyFound[ci->aliasOrName()]) {// columnsAlreadyFoundIt==columnsAlreadyFound.constEnd())
			columnsAlreadyFound.insert(ci->aliasOrName(), (char*)1);
			result.insert(uniqueListCount++, ci);
		}
	}
	result.resize(uniqueListCount); //update result size
	return result;
}

QueryColumnInfo::Vector QuerySchema::internalFields()
{
	computeFieldsExpanded();
	return d->internalFields ? *d->internalFields : QueryColumnInfo::Vector();
}

QueryColumnInfo* QuerySchema::expandedOrInternalField(uint index)
{
	QueryColumnInfo::Vector vector = fieldsExpanded(WithInternalFields);
	return (index < vector.size()) ? vector[index] : 0;
}

inline QString lookupColumnKey(Field *foreignField, Field* field)
{
	QString res;
	if (field->table()) // can be 0 for anonymous fields built as joined multiple visible columns
		res = field->table()->name() + ".";
	return res + field->name() + "_" + foreignField->table()->name() + "." + foreignField->name();
}

void QuerySchema::computeFieldsExpanded()
{
	if (d->fieldsExpanded)
		return;

	if (!d->columnsOrder) {
		d->columnsOrder = new QMap<QueryColumnInfo*,int>();
		d->columnsOrderWithoutAsterisks = new QMap<QueryColumnInfo*,int>();
	}
	else {
		d->columnsOrder->clear();
		d->columnsOrderWithoutAsterisks->clear();
	}
	if (d->ownedVisibleColumns)
		d->ownedVisibleColumns->clear();

	//collect all fields in a list (not a vector yet, because we do not know its size)
	QueryColumnInfo::List list; //temporary
	QueryColumnInfo::List lookup_list; //temporary, for collecting additional fields related to lookup fields
	QMap<QueryColumnInfo*, bool> columnInfosOutsideAsterisks; //helper for filling d->columnInfosByName
	uint i = 0;
	uint fieldPosition = 0;
	uint numberOfColumnsWithMultipleVisibleFields = 0; //used to find an unique name for anonymous field
	Field *f;
	for (Field::ListIterator it = fieldsIterator(); (f = it.current()); ++it, fieldPosition++) {
		if (f->isQueryAsterisk()) {
			if (static_cast<QueryAsterisk*>(f)->isSingleTableAsterisk()) {
				Field::List *ast_fields = static_cast<QueryAsterisk*>(f)->table()->fields();
				for (Field *ast_f = ast_fields->first(); ast_f; ast_f=ast_fields->next()) {
//					d->detailedVisibility += isFieldVisible(fieldPosition);
					QueryColumnInfo *ci = new QueryColumnInfo(ast_f, Q3CString()/*no field for asterisk!*/,
						isColumnVisible(fieldPosition));
					list.append( ci );
					KexiDBDbg << "QuerySchema::computeFieldsExpanded(): caching (unexpanded) columns order: "
						<< ci->debugString() << " at position " << fieldPosition << endl;
					d->columnsOrder->insert(ci, fieldPosition);
//					list.append(ast_f);
				}
			}
			else {//all-tables asterisk: iterate through table list
				for (TableSchema *table = d->tables.first(); table; table = d->tables.next()) {
					//add all fields from this table
					Field::List *tab_fields = table->fields();
					for (Field *tab_f = tab_fields->first(); tab_f; tab_f = tab_fields->next()) {
//! \todo (js): perhaps not all fields should be appended here
//						d->detailedVisibility += isFieldVisible(fieldPosition);
//						list.append(tab_f);
						QueryColumnInfo *ci = new QueryColumnInfo(tab_f, Q3CString()/*no field for asterisk!*/,
							isColumnVisible(fieldPosition));
						list.append( ci );
						KexiDBDbg << "QuerySchema::computeFieldsExpanded(): caching (unexpanded) columns order: "
							<< ci->debugString() << " at position " << fieldPosition << endl;
						d->columnsOrder->insert(ci, fieldPosition);
					}
				}
			}
		}
		else {
			//a single field
//			d->detailedVisibility += isFieldVisible(fieldPosition);
			QueryColumnInfo *ci = new QueryColumnInfo(f, columnAlias(fieldPosition), isColumnVisible(fieldPosition));
			list.append( ci );
			columnInfosOutsideAsterisks.insert( ci, true );
			KexiDBDbg << "QuerySchema::computeFieldsExpanded(): caching (unexpanded) column's order: "
				<< ci->debugString() << " at position " << fieldPosition << endl;
			d->columnsOrder->insert(ci, fieldPosition);
			d->columnsOrderWithoutAsterisks->insert(ci, fieldPosition);

			//handle lookup field schema
			LookupFieldSchema *lookupFieldSchema = f->table() ? f->table()->lookupFieldSchema( *f ) : 0;
			if (!lookupFieldSchema || lookupFieldSchema->boundColumn()<0)
				continue;
			// Lookup field schema found:
			// Now we also need to fetch "visible" value from the lookup table, not only the value of binding.
			// -> build LEFT OUTER JOIN clause for this purpose (LEFT, not INNER because the binding can be broken)
			// "LEFT OUTER JOIN lookupTable ON thisTable.thisField=lookupTable.boundField"
			LookupFieldSchema::RowSource& rowSource = lookupFieldSchema->rowSource();
			if (rowSource.type() == LookupFieldSchema::RowSource::Table) {
				TableSchema *lookupTable = connection()->tableSchema( rowSource.name() );
				FieldList* visibleColumns = 0;
				Field *boundField = 0;
				if (lookupTable 
					&& (uint)lookupFieldSchema->boundColumn() < lookupTable->fieldCount()
					&& (visibleColumns = lookupTable->subList( lookupFieldSchema->visibleColumns() ))
					&& (boundField = lookupTable->field( lookupFieldSchema->boundColumn() )))
				{
					Field *visibleColumn = 0;
					// for single visible column, just add it as-is
					if (visibleColumns->fieldCount() == 1) {
						visibleColumn = visibleColumns->fields()->first();
					}
					else {
						// for multiple visible columns, build an expression column
						// (the expression object will be owned by column info)
						visibleColumn = new Field();
						visibleColumn->setName(
							QString::fromLatin1("[multiple_visible_fields_%1]")
							.arg( ++numberOfColumnsWithMultipleVisibleFields ));
						visibleColumn->setExpression( 
							new ConstExpr(CHARACTER_STRING_LITERAL, QVariant()/*not important*/));
						if (!d->ownedVisibleColumns) {
							d->ownedVisibleColumns = new Field::List();
							d->ownedVisibleColumns->setAutoDelete(true);
						}
						d->ownedVisibleColumns->append( visibleColumn ); // remember to delete later
					}

					lookup_list.append( 
						new QueryColumnInfo(visibleColumn, Q3CString(), true/*visible*/, ci/*foreign*/) );
/*
					//add visibleField to the list of SELECTed fields if it is not yes present there
					if (!findTableField( visibleField->table()->name()+"."+visibleField->name() )) {
						if (!table( visibleField->table()->name() )) {
						}
						if (!sql.isEmpty())
							sql += QString::fromLatin1(", ");
						sql += (escapeIdentifier(visibleField->table()->name(), drvEscaping) + "."
							+ escapeIdentifier(visibleField->name(), drvEscaping));
					}*/
				}
				delete visibleColumns;
			}
			else if (rowSource.type() == LookupFieldSchema::RowSource::Query) {
				QuerySchema *lookupQuery = connection()->querySchema( rowSource.name() );
				if (!lookupQuery)
					continue;
				const QueryColumnInfo::Vector lookupQueryFieldsExpanded( lookupQuery->fieldsExpanded() );
				if ((uint)lookupFieldSchema->boundColumn() >= lookupQueryFieldsExpanded.count())
					continue;
				QueryColumnInfo *boundColumnInfo = 0;
				if (!(boundColumnInfo = lookupQueryFieldsExpanded[ lookupFieldSchema->boundColumn() ]))
					continue;
				Field *boundField = boundColumnInfo->field;
				if (!boundField)
					continue;
				const Q3ValueList<uint> visibleColumns( lookupFieldSchema->visibleColumns() );
				bool ok = true;
				// all indices in visibleColumns should be in [0..lookupQueryFieldsExpanded.size()-1]
				for (Q3ValueList<uint>::ConstIterator
					visibleColumnsIt(visibleColumns.constBegin());
					visibleColumnsIt!=visibleColumns.constEnd(); ++visibleColumnsIt)
				{
					if ((*visibleColumnsIt) >= lookupQueryFieldsExpanded.count()) {
						ok = false;
						break;
					}
				}
				if (!ok)
					continue;
				Field *visibleColumn = 0;
				// for single visible column, just add it as-is
				if (visibleColumns.count() == 1) {
					visibleColumn = lookupQueryFieldsExpanded[ visibleColumns.first() ]->field;
				}
				else {
					// for multiple visible columns, build an expression column
					// (the expression object will be owned by column info)
					visibleColumn = new Field();
					visibleColumn->setName(
						QString::fromLatin1("[multiple_visible_fields_%1]")
						.arg( ++numberOfColumnsWithMultipleVisibleFields ));
					visibleColumn->setExpression( 
						new ConstExpr(CHARACTER_STRING_LITERAL, QVariant()/*not important*/));
					if (!d->ownedVisibleColumns) {
						d->ownedVisibleColumns = new Field::List();
						d->ownedVisibleColumns->setAutoDelete(true);
					}
					d->ownedVisibleColumns->append( visibleColumn ); // remember to delete later
				}

				lookup_list.append( 
					new QueryColumnInfo(visibleColumn, Q3CString(), true/*visible*/, ci/*foreign*/) );
/*
				//add visibleField to the list of SELECTed fields if it is not yes present there
				if (!findTableField( visibleField->table()->name()+"."+visibleField->name() )) {
					if (!table( visibleField->table()->name() )) {
					}
					if (!sql.isEmpty())
						sql += QString::fromLatin1(", ");
					sql += (escapeIdentifier(visibleField->table()->name(), drvEscaping) + "."
						+ escapeIdentifier(visibleField->name(), drvEscaping));
				}*/
			}
		}
	}
	//prepare clean vector for expanded list, and a map for order information
	if (!d->fieldsExpanded) {
		d->fieldsExpanded = new QueryColumnInfo::Vector( list.count() );// Field::Vector( list.count() );
		d->fieldsExpanded->setAutoDelete(true);
		d->columnsOrderExpanded = new QMap<QueryColumnInfo*,int>();
	}
	else {//for future:
		d->fieldsExpanded->clear();
		d->fieldsExpanded->resize( list.count() );
		d->columnsOrderExpanded->clear();
	}

	/*fill (based on prepared 'list' and 'lookup_list'):
	 -the vector
	 -the map
	 -"fields by name" dictionary
	*/
	d->columnInfosByName.clear();
	d->columnInfosByNameExpanded.clear();
	i=0;
	QueryColumnInfo *ci;
	for (QueryColumnInfo::ListIterator it(list); (ci = it.current()); ++it, i++) {
		d->fieldsExpanded->insert(i, ci);
		d->columnsOrderExpanded->insert(ci, i);
		//remember field by name/alias/table.name if there's no such string yet in d->columnInfosByNameExpanded
		if (!ci->alias.isEmpty()) {
			//store alias and table.alias
			if (!d->columnInfosByNameExpanded[ ci->alias ])
				d->columnInfosByNameExpanded.insert( ci->alias, ci );
			QString tableAndAlias( ci->alias );
			if (ci->field->table())
				tableAndAlias.prepend(ci->field->table()->name() + ".");
			if (!d->columnInfosByNameExpanded[ tableAndAlias ])
				d->columnInfosByNameExpanded.insert( tableAndAlias, ci );
			//the same for "unexpanded" list
			if (columnInfosOutsideAsterisks.contains(ci)) {
				if (!d->columnInfosByName[ ci->alias ])
					d->columnInfosByName.insert( ci->alias, ci );
				if (!d->columnInfosByName[ tableAndAlias ])
					d->columnInfosByName.insert( tableAndAlias, ci );
			}
		}
		else {
			//no alias: store name and table.name
			if (!d->columnInfosByNameExpanded[ ci->field->name() ])
				d->columnInfosByNameExpanded.insert( ci->field->name(), ci );
			QString tableAndName( ci->field->name() );
			if (ci->field->table())
				tableAndName.prepend(ci->field->table()->name() + ".");
			if (!d->columnInfosByNameExpanded[ tableAndName ])
				d->columnInfosByNameExpanded.insert( tableAndName, ci );
			//the same for "unexpanded" list
			if (columnInfosOutsideAsterisks.contains(ci)) {
				if (!d->columnInfosByName[ ci->field->name() ])
					d->columnInfosByName.insert( ci->field->name(), ci );
				if (!d->columnInfosByName[ tableAndName ])
					d->columnInfosByName.insert( tableAndName, ci );
			}
		}
	}

	//remove duplicates for lookup fields
	Q3Dict<uint> lookup_dict(101); //used to fight duplicates and to update QueryColumnInfo::indexForVisibleLookupValue()
	                         // (a mapping from table.name string to uint* lookupFieldIndex
	lookup_dict.setAutoDelete(true);
	i=0;
	for (QueryColumnInfo::ListIterator it(lookup_list); (ci = it.current());)
	{
		const QString key( lookupColumnKey(ci->foreignColumn()->field, ci->field) );
		if ( /* not needed   columnInfo( tableAndFieldName ) || */
			lookup_dict[ key ]) {
			// this table.field is already fetched by this query
			++it;
			lookup_list.removeRef( ci );
		}
		else {
			lookup_dict.replace( key, new uint( i ) );
			++it;
			i++;
		}
	}

	//create internal expanded list with lookup fields
	if (d->internalFields) {
		d->internalFields->clear();
		d->internalFields->resize( lookup_list.count() );
	}
	delete d->fieldsExpandedWithInternal; //clear cache
	delete d->fieldsExpandedWithInternalAndRowID; //clear cache
	d->fieldsExpandedWithInternal = 0;
	d->fieldsExpandedWithInternalAndRowID = 0;
	if (!lookup_list.isEmpty() && !d->internalFields) {//create on demand
		d->internalFields = new QueryColumnInfo::Vector( lookup_list.count() );
		d->internalFields->setAutoDelete(true);
	}
	i=0;
	for (QueryColumnInfo::ListIterator it(lookup_list); it.current();i++, ++it)
	{
		//add it to the internal list
		d->internalFields->insert(i, it.current());
		d->columnsOrderExpanded->insert(it.current(), list.count()+i);
	}

	//update QueryColumnInfo::indexForVisibleLookupValue() cache for columns
	numberOfColumnsWithMultipleVisibleFields = 0;
	for (i=0; i < d->fieldsExpanded->size(); i++) {
		QueryColumnInfo* ci = d->fieldsExpanded->at(i);
//! @todo QuerySchema itself will also support lookup fields...
		LookupFieldSchema *lookupFieldSchema 
			= ci->field->table() ? ci->field->table()->lookupFieldSchema( *ci->field ) : 0;
		if (!lookupFieldSchema || lookupFieldSchema->boundColumn()<0)
			continue;
		LookupFieldSchema::RowSource& rowSource = lookupFieldSchema->rowSource();
		if (rowSource.type() == LookupFieldSchema::RowSource::Table) {
			TableSchema *lookupTable = connection()->tableSchema( rowSource.name() );
			FieldList* visibleColumns = 0;
			if (lookupTable 
				&& (uint)lookupFieldSchema->boundColumn() < lookupTable->fieldCount()
				&& (visibleColumns = lookupTable->subList( lookupFieldSchema->visibleColumns() )))
			{
				Field *visibleColumn = 0;
				// for single visible column, just add it as-is
				if (visibleColumns->fieldCount() == 1) 
				{
					visibleColumn = visibleColumns->fields()->first();
					const QString key( lookupColumnKey(ci->field, visibleColumn) );
					uint *index = lookup_dict[ key ];
					if (index)
						ci->setIndexForVisibleLookupValue( d->fieldsExpanded->size() + *index );
				}
				else {
					const QString key( QString::fromLatin1("[multiple_visible_fields_%1]_%2.%3")
						.arg( ++numberOfColumnsWithMultipleVisibleFields )
						.arg(ci->field->table()->name()).arg(ci->field->name()) );
					uint *index = lookup_dict[ key ];
					if (index)
						ci->setIndexForVisibleLookupValue( d->fieldsExpanded->size() + *index );
				}
			}
			delete visibleColumns;
		}
		else if (rowSource.type() == LookupFieldSchema::RowSource::Query) {
			QuerySchema *lookupQuery = connection()->querySchema( rowSource.name() );
			if (!lookupQuery)
				continue;
			const QueryColumnInfo::Vector lookupQueryFieldsExpanded( lookupQuery->fieldsExpanded() );
			if ((uint)lookupFieldSchema->boundColumn() >= lookupQueryFieldsExpanded.count())
				continue;
			QueryColumnInfo *boundColumnInfo = 0;
			if (!(boundColumnInfo = lookupQueryFieldsExpanded[ lookupFieldSchema->boundColumn() ]))
				continue;
			Field *boundField = boundColumnInfo->field;
			if (!boundField)
				continue;
			const Q3ValueList<uint> visibleColumns( lookupFieldSchema->visibleColumns() );
			Field *visibleColumn = 0;
			// for single visible column, just add it as-is
			if (visibleColumns.count() == 1) {
				visibleColumn = lookupQueryFieldsExpanded[ visibleColumns.first() ]->field;
				const QString key( lookupColumnKey(ci->field, visibleColumn) );
				uint *index = lookup_dict[ key ];
				if (index)
					ci->setIndexForVisibleLookupValue( d->fieldsExpanded->size() + *index );
			}
			else {
				const QString key( QString::fromLatin1("[multiple_visible_fields_%1]_%2.%3")
					.arg( ++numberOfColumnsWithMultipleVisibleFields )
					.arg(ci->field->table()->name()).arg(ci->field->name()) );
				uint *index = lookup_dict[ key ];
				if (index)
					ci->setIndexForVisibleLookupValue( d->fieldsExpanded->size() + *index );
			}
		}
		else {
			KexiDBWarn << "QuerySchema::computeFieldsExpanded(): unsupported row source type " 
				<< rowSource.typeName() << endl;
		}
	}
}

QMap<QueryColumnInfo*,int> QuerySchema::columnsOrder(ColumnsOrderOptions options)
{
	if (!d->columnsOrder)
		computeFieldsExpanded();
	if (options == UnexpandedList)
		return *d->columnsOrder;
	else if (options == UnexpandedListWithoutAsterisks)
		return *d->columnsOrderWithoutAsterisks;
	return *d->columnsOrderExpanded;
}

Q3ValueVector<int> QuerySchema::pkeyFieldsOrder()
{
	if (d->pkeyFieldsOrder)
		return *d->pkeyFieldsOrder;

	TableSchema *tbl = masterTable();
	if (!tbl || !tbl->primaryKey())
		return Q3ValueVector<int>();

	//get order of PKEY fields (e.g. for rows updating or inserting )
	IndexSchema *pkey = tbl->primaryKey();
	pkey->debug();
	debug();
	d->pkeyFieldsOrder = new Q3ValueVector<int>( pkey->fieldCount(), -1 );

	const uint fCount = fieldsExpanded().count();
	d->pkeyFieldsCount = 0;
	for (uint i = 0; i<fCount; i++) {
		QueryColumnInfo *fi = d->fieldsExpanded->at(i);
		const int fieldIndex = fi->field->table()==tbl ? pkey->indexOf(fi->field) : -1;
		if (fieldIndex!=-1/* field found in PK */ 
			&& d->pkeyFieldsOrder->at(fieldIndex)==-1 /* first time */)
		{
			KexiDBDbg << "QuerySchema::pkeyFieldsOrder(): FIELD " << fi->field->name() 
				<< " IS IN PKEY AT POSITION #" << fieldIndex << endl;
//			(*d->pkeyFieldsOrder)[j]=i;
			(*d->pkeyFieldsOrder)[fieldIndex]=i;
			d->pkeyFieldsCount++;
//			j++;
		}
	}
	KexiDBDbg << "QuerySchema::pkeyFieldsOrder(): " << d->pkeyFieldsCount
		<< " OUT OF " << pkey->fieldCount() << " PKEY'S FIELDS FOUND IN QUERY " << name() << endl;
	return *d->pkeyFieldsOrder;
}

uint QuerySchema::pkeyFieldsCount()
{
	(void)pkeyFieldsOrder(); /* rebuild information */
	return d->pkeyFieldsCount;
}

Relationship* QuerySchema::addRelationship( Field *field1, Field *field2 )
{
//@todo: find existing global db relationships
	Relationship *r = new Relationship(this, field1, field2);
	if (r->isEmpty()) {
		delete r;
		return 0;
	}

	d->relations.append( r );
	return r;
}

QueryColumnInfo::List* QuerySchema::autoIncrementFields()
{
	if (!d->autoincFields) {
		d->autoincFields = new QueryColumnInfo::List();
	}
	TableSchema *mt = masterTable();
	if (!mt) {
		KexiDBWarn << "QuerySchema::autoIncrementFields(): no master table!" << endl;
		return d->autoincFields;
	}
	if (d->autoincFields->isEmpty()) {//no cache
		QueryColumnInfo::Vector fexp = fieldsExpanded();
		for (int i=0; i<(int)fexp.count(); i++) {
			QueryColumnInfo *fi = fexp[i];
			if (fi->field->table() == mt && fi->field->isAutoIncrement()) {
				d->autoincFields->append( fi );
			}
		}
	}
	return d->autoincFields;
}

QString QuerySchema::sqlColumnsList(QueryColumnInfo::List* infolist, Driver *driver)
{
	if (!infolist)
		return QString();
	QString result;
	result.reserve(256);
	QueryColumnInfo::ListIterator it( *infolist );
	bool start = true;
	for (; it.current(); ++it) {
		if (!start)
			result += ",";
		else
			start = false;
		result += driver->escapeIdentifier( it.current()->field->name() );
	}
	return result;
}

QString QuerySchema::autoIncrementSQLFieldsList(Driver *driver)
{
	if ((Driver *)d->lastUsedDriverForAutoIncrementSQLFieldsList != driver
		|| d->autoIncrementSQLFieldsList.isEmpty())
	{
		d->autoIncrementSQLFieldsList = QuerySchema::sqlColumnsList( autoIncrementFields(), driver );
		d->lastUsedDriverForAutoIncrementSQLFieldsList = driver;
	}
	return d->autoIncrementSQLFieldsList;
}

void QuerySchema::setWhereExpression(BaseExpr *expr)
{
	delete d->whereExpr;
	d->whereExpr = expr;
}

void QuerySchema::addToWhereExpression(KexiDB::Field *field, const QVariant& value, int relation)
{
	int token;
	if (value.isNull())
		token = SQL_NULL;
	else if (field->isIntegerType()) {
		token = INTEGER_CONST;
	}
	else if (field->isFPNumericType()) {
		token = REAL_CONST;
	}
	else {
		token = CHARACTER_STRING_LITERAL;
//! @todo date, time			
	}
	
	BinaryExpr * newExpr = new BinaryExpr(
		KexiDBExpr_Relational, 
		new ConstExpr( token, value ),
		relation,
		new VariableExpr((field->table() ? (field->table()->name()+".") : QString())+field->name())
	);
	if (d->whereExpr) {
		d->whereExpr = new BinaryExpr(
			KexiDBExpr_Logical, 
			d->whereExpr,
			AND,
			newExpr
		);
	}
	else {
		d->whereExpr = newExpr;
	}
}

/*
void QuerySchema::addToWhereExpression(KexiDB::Field *field, const QVariant& value)
		switch (value.type()) {
		case Int: case UInt: case Bool: case LongLong: case ULongLong:
			token = INTEGER_CONST;
			break;
		case Double:
			token = REAL_CONST;
			break;
		default:
			token = CHARACTER_STRING_LITERAL;
		}
//! @todo date, time			
				
*/

BaseExpr *QuerySchema::whereExpression() const
{
	return d->whereExpr;
}

void QuerySchema::setOrderByColumnList(const OrderByColumnList& list)
{
	d->orderByColumnList = list;
// all field names should be found, exit otherwise ..........?
}

OrderByColumnList& QuerySchema::orderByColumnList() const
{
	return d->orderByColumnList;
}

QuerySchemaParameterList QuerySchema::parameters()
{
	if (!whereExpression())
		return QuerySchemaParameterList();
	QuerySchemaParameterList params;
	whereExpression()->getQueryParameters(params);
	return params;
}

/*
	new field1, Field *field2
	if (!field1 || !field2) {
		KexiDBWarn << "QuerySchema::addRelationship(): !masterField || !detailsField" << endl;
		return;
	}
	if (field1->isQueryAsterisk() || field2->isQueryAsterisk()) {
		KexiDBWarn << "QuerySchema::addRelationship(): relationship's fields cannot be asterisks" << endl;
		return;
	}
	if (!hasField(field1) && !hasField(field2)) {
		KexiDBWarn << "QuerySchema::addRelationship(): fields do not belong to this query" << endl;
		return;
	}
	if (field1->table() == field2->table()) {
		KexiDBWarn << "QuerySchema::addRelationship(): fields cannot belong to the same table" << endl;
		return;
	}
//@todo: check more things: -types
//@todo: find existing global db relationships

	Field *masterField = 0, *detailsField = 0;
	IndexSchema *masterIndex = 0, *detailsIndex = 0;
	if (field1->isPrimaryKey() && field2->isPrimaryKey()) {
		//2 primary keys
		masterField = field1;
		masterIndex = masterField->table()->primaryKey();
		detailsField = field2;
		detailsIndex = masterField->table()->primaryKey();
	}
	else if (field1->isPrimaryKey()) {
		masterField = field1;
		masterIndex = masterField->table()->primaryKey();
		detailsField = field2;
//@todo: check if it already exists
		detailsIndex = new IndexSchema(detailsField->table());
		detailsIndex->addField(detailsField);
		detailsIndex->setForeigKey(true);
	//		detailsField->setForeignKey(true);
	}
	else if (field2->isPrimaryKey()) {
		detailsField = field1;
		masterField = field2;
		masterIndex = masterField->table()->primaryKey();
//@todo
	}

	if (!masterIndex || !detailsIndex)
		return; //failed

	Relationship *rel = new Relationship(masterIndex, detailsIndex);

	d->relations.append( rel );
}*/

//---------------------------------------------------

QueryAsterisk::QueryAsterisk( QuerySchema *query, TableSchema *table )
	:Field()
	,m_table(table)
{
	assert(query);
	m_parent = query;
	setType(Field::Asterisk);
}

QueryAsterisk::~QueryAsterisk()
{
}

Field* QueryAsterisk::copy() const
{
	return new QueryAsterisk(*this);
}

void QueryAsterisk::setTable(TableSchema *table)
{
	KexiDBDbg << "QueryAsterisk::setTable()" << endl;
	m_table=table;
}

QString QueryAsterisk::debugString()
{
	QString dbg;
	if (isAllTableAsterisk()) {
		dbg += "ALL-TABLES ASTERISK (*) ON TABLES(";
		TableSchema *table;
		QString table_names;
		for (TableSchema::ListIterator it( *query()->tables() ); (table = it.current()); ++it) {
			if (!table_names.isEmpty())
				table_names += ", ";
			table_names += table->name();
		}
		dbg += (table_names + ")");
	}
	else {
		dbg += ("SINGLE-TABLE ASTERISK (" + table()->name() + ".*)");
	}
	return dbg;
}

