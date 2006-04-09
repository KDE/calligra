/* This file is part of the KDE project
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>

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

#include <kexidb/queryschema.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/expression.h>
#include <kexidb/parser/sqlparser.h>
#include <kexidb/utils.h>

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

//=======================================
namespace KexiDB {
//! @internal
class QuerySchemaPrivate
{
	public:
		QuerySchemaPrivate(QuerySchema* q)
		 : query(q)
		 , masterTable(0)
		 , maxIndexWithAlias(-1)
		 , visibility(64)
		 , fieldsExpanded(0)
		 , orderByColumnList(0)
		 , autoincFields(0)
		 , fieldsOrder(0)
		 , pkeyFieldsOrder(0)
		 , pkeyFieldsCount(0)
		 , tablesBoundToColumns(64, -1)
		 , tablePositionsForAliases(67, false)
		 , columnPositionsForAliases(67, false)
		 , whereExpr(0)
		 , regenerateExprAliases(false)
		{
			columnAliases.setAutoDelete(true);
			tableAliases.setAutoDelete(true);
			asterisks.setAutoDelete(true);
			relations.setAutoDelete(true);
			tablePositionsForAliases.setAutoDelete(true);
			columnPositionsForAliases.setAutoDelete(true);
			visibility.fill(false);
		}
		~QuerySchemaPrivate()
		{
			delete fieldsExpanded;
			delete orderByColumnList;
			delete autoincFields;
			delete fieldsOrder;
			delete pkeyFieldsOrder;
			delete whereExpr;
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
			if (fieldsExpanded) {
				delete fieldsExpanded;
				fieldsExpanded = 0;
				delete fieldsOrder;
				fieldsOrder = 0;
				delete autoincFields;
				autoincFields = 0;
				autoIncrementSQLFieldsList = QString::null;
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
						columnAlias = (i18n("short for 'expression' word (only latin letters, please)", "expr") 
							+ QString::number(colNum)).latin1();
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

		/*! A list of fields for ORDER BY section. @see QuerySchema::orderByColumnList(). */
		QueryColumnInfo::Vector *orderByColumnList;

		/*! A cache for autoIncrementFields(). */
		QueryColumnInfo::List *autoincFields;
		
		/*! A cache for autoIncrementSQLFieldsList(). */
		QString autoIncrementSQLFieldsList;
		QPointer<Driver> lastUsedDriverForAutoIncrementSQLFieldsList;

		/*! A map for fast lookup of query fields' order.
		 This is exactly opposite information compared to vector returned 
		 by fieldsExpanded() */
		QMap<QueryColumnInfo*,int> *fieldsOrder;

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

		Q3Dict<QueryColumnInfo> columnInfosByName;

		/*! Set by insertField(): true, if aliases for expression columns should 
		 be generated on next columnAlias() call. */
		bool regenerateExprAliases : 1;
};
}

//=======================================

QuerySchema::QuerySchema()
	: FieldList(false)//fields are not owned by QuerySchema object
	, SchemaData(KexiDB::QueryObjectType)
	, d( new QuerySchemaPrivate(this) )
{
	init();
}

QuerySchema::QuerySchema(TableSchema* tableSchema)
	: FieldList(false)
	, SchemaData(KexiDB::QueryObjectType)
	, d( new QuerySchemaPrivate(this) )
{
	d->masterTable = tableSchema;
//	assert(d->masterTable);
	init();
	if (!d->masterTable) {
		KexiDBWarn << "QuerySchema(TableSchema*): !d->masterTable" << endl;
		m_name = QString::null;
		return;
	}
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
		KexiDBWarn << "QuerySchema::addField(): WARNING: field '"<<field->name()
			<<"' must contain table information!" <<endl;
		return *this;
	}
	if (fieldCount()>=d->visibility.size()) {
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
	if (columnPosition > d->tablesBoundToColumns.count()) {
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
			dbg1 += (ci->field->name() + 
				(ci->alias.isEmpty() ? QString::null : (QString::fromLatin1(" AS ") + QString(ci->alias))));
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
	QString where = d->whereExpr ? d->whereExpr->debugString() : QString::null;
	if (!where.isEmpty())
		dbg += QString("\n-WHERE EXPRESSION:\n" + where);
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
		if (!tableNameLower.isEmpty() && it.current()->name().lower()!=tableNameLower) {
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
		const QString& tableNameLower = table->name().lower();
		const QString& aliasLower = QString(alias.lower());
		int num = 0;
		for (TableSchema::ListIterator it(d->tables); it.current(); ++it, num++) {
			if (it.current()->name().lower()==tableNameLower) {
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
		if (it.current()->name().lower()==tableName.lower())
			return it.current();
	}
	return 0;
}

bool QuerySchema::contains(TableSchema *table) const
{
	return d->tables.findRef(table)!=-1;
}

Field* QuerySchema::findTableField(const QString &tableDotFieldName) const
{
	QString tableName, fieldName;
	if (!KexiDB::splitToTableAndFieldParts(tableDotFieldName, tableName, fieldName))
		return 0;
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
		if (it.current()->name().lower()==tableName.lower())
			return num;
	}
	return -1;
}

Q3ValueList<int> QuerySchema::tablePositions(const QString& tableName) const
{
	int num = 0;
	Q3ValueList<int> result;
	const QString& tableNameLower = tableName.lower();
	for (TableSchema::ListIterator it(d->tables); it.current(); ++it, num++) {
		if (it.current()->name().lower()==tableNameLower) {
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

Field* QuerySchema::field(const QString& name)
{
	computeFieldsExpanded();
	QueryColumnInfo *ci = d->columnInfosByName[name];
	return ci ? ci->field : 0;
}

QueryColumnInfo* QuerySchema::columnInfo(const QString& name)
{
	computeFieldsExpanded();
	return d->columnInfosByName[name];
}

QueryColumnInfo::Vector QuerySchema::fieldsExpanded(bool unique)
{
	computeFieldsExpanded();
	if (!unique)
		return *d->fieldsExpanded;

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

void QuerySchema::computeFieldsExpanded()
{
	if (d->fieldsExpanded) {
//		if (detailedVisibility)
//			*detailedVisibility = d->detailedVisibility;
		return;
	}

//	if (detailedVisibility)
//		detailedVisibility->clear();

//	d->detailedVisibility.clear();
//	m_fields_by_name.clear();

	//collect all fields in a list (not a vector yet, because we do not know its size)
	QueryColumnInfo::List list;
	int i = 0;
	int fieldPosition = 0;
	for (Field *f = m_fields.first(); f; f = m_fields.next(), fieldPosition++) {
		if (f->isQueryAsterisk()) {
			if (static_cast<QueryAsterisk*>(f)->isSingleTableAsterisk()) {
				Field::List *ast_fields = static_cast<QueryAsterisk*>(f)->table()->fields();
				for (Field *ast_f = ast_fields->first(); ast_f; ast_f=ast_fields->next()) {
//					d->detailedVisibility += isFieldVisible(fieldPosition);
					list.append( new QueryColumnInfo(ast_f, Q3CString()/*no field for asterisk!*/,
						isColumnVisible(fieldPosition)) 
					);
//					list.append(ast_f);
				}
			}
			else {//all-tables asterisk: itereate through table list
				for (TableSchema *table = d->tables.first(); table; table = d->tables.next()) {
					//add all fields from this table
					Field::List *tab_fields = table->fields();
					for (Field *tab_f = tab_fields->first(); tab_f; tab_f = tab_fields->next()) {
//! \todo (js): perhaps not all fields should be appended here
//						d->detailedVisibility += isFieldVisible(fieldPosition);
//						list.append(tab_f);
						list.append( new QueryColumnInfo(tab_f, Q3CString()/*no field for asterisk!*/,
							isColumnVisible(fieldPosition)) 
						);
					}
				}
			}
		}
		else {
			//a single field
//			d->detailedVisibility += isFieldVisible(fieldPosition);
			list.append(
				new QueryColumnInfo(f, columnAlias(fieldPosition), isColumnVisible(fieldPosition)) );
//			list.append(f);
		}
	}
	//prepare clean vector for expanded list, and a map for order information
	if (!d->fieldsExpanded) {
		d->fieldsExpanded = new QueryColumnInfo::Vector( list.count() );// Field::Vector( list.count() );
		d->fieldsExpanded->setAutoDelete(true);
		d->fieldsOrder = new QMap<QueryColumnInfo*,int>();
	}
	else {//for future:
		d->fieldsExpanded->clear();
		d->fieldsExpanded->resize( list.count() );
		d->fieldsOrder->clear();
	}

	/*fill:
	 -the vector
	 -the map
	 -"fields by name" dictionary
	*/
	d->columnInfosByName.clear();
	QueryColumnInfo::ListIterator it(list);
	for (i=0; it.current(); ++it, i++)
	{
		d->fieldsExpanded->insert(i,it.current());
		d->fieldsOrder->insert(it.current(),i);
		//remember field by name/alias/table.name if there's no such string yet in d->columnInfosByName
		if (!it.current()->alias.isEmpty()) {
			//alias
			if (!d->columnInfosByName[ it.current()->alias ])
				d->columnInfosByName.insert( it.current()->alias, it.current() );
		}
		else {
			//no alias: store name and table.name
			if (!d->columnInfosByName[ it.current()->field->name() ])
				d->columnInfosByName.insert( it.current()->field->name(), it.current() );
			QString tableAndName( it.current()->field->table()->name() + "." + it.current()->field->name() );
			if (!d->columnInfosByName[ tableAndName ])
				d->columnInfosByName.insert( tableAndName, it.current() );
		}
	}
//	if (detailedVisibility)
//		*detailedVisibility = d->detailedVisibility;
}

QMap<QueryColumnInfo*,int> QuerySchema::fieldsOrder()
{
	if (!d->fieldsOrder)
		computeFieldsExpanded();
	return *d->fieldsOrder;
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
		return QString::null;
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
		new VariableExpr((field->table() ? (field->table()->name()+".") : QString::null)+field->name())
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

void QuerySchema::setOrderByColumnList(const QStringList& columnNames)
{
	Q_UNUSED(columnNames);
//! @todo implement this:
// all field names should be fooun, exit otherwise ..........

	// OK
//TODO	if (!d->orderByColumnList)
//TODO
}

/*! Convenience method, similar to setOrderBy(const QStringList&). */
void QuerySchema::setOrderByColumnList(const QString& column1, const QString& column2, 
	const QString& column3, const QString& column4, const QString& column5)
{
	Q_UNUSED(column1);
	Q_UNUSED(column2);
	Q_UNUSED(column3);
	Q_UNUSED(column4);
	Q_UNUSED(column5);
//! @todo implement this, like above
//! @todo add ORDER BY info to debugString()
}

QueryColumnInfo::Vector QuerySchema::orderByColumnList() const
{
	return d->orderByColumnList ? *d->orderByColumnList: QueryColumnInfo::Vector();
}


/*
	new field1, Field *field2
	if (!field1 || !field2) {
		kWarning() << "QuerySchema::addRelationship(): !masterField || !detailsField" << endl;
		return;
	}
	if (field1->isQueryAsterisk() || field2->isQueryAsterisk()) {
		kWarning() << "QuerySchema::addRelationship(): relationship's fields cannot be asterisks" << endl;
		return;
	}
	if (!hasField(field1) && !hasField(field2)) {
		kWarning() << "QuerySchema::addRelationship(): fields do not belong to this query" << endl;
		return;
	}
	if (field1->table() == field2->table()) {
		kWarning() << "QuerySchema::addRelationship(): fields cannot belong to the same table" << endl;
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
		TableSchema::List *tables = query()->tables();
		for ( table = tables->first(); table; table = tables->next() ) {
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

