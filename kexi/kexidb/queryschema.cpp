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

#include <kexidb/queryschema.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>

#include <assert.h>

#include <qvaluelist.h>
#include <qasciidict.h>
#include <qptrdict.h>
#include <qintdict.h>
#include <qbitarray.h>

#include <kdebug.h>

using namespace KexiDB;

//=======================================
namespace KexiDB {
class QuerySchemaPrivate
{
	public:
		QuerySchemaPrivate()
		 : parent_table(0)
		 , maxIndexWithAlias(-1)
		 , visibility(64)
		 , fieldsExpanded(0)
		 , autoincFields(0)
		 , fieldsOrder(0)
		 , pkeyFieldsOrder(0)
		 , tablesBoundToColumns(64, -1)
		 , tablePositionsForAliases(67, false)
		{
			aliases.setAutoDelete(true);
			tableAliases.setAutoDelete(true);
			asterisks.setAutoDelete(true);
			relations.setAutoDelete(true);
			tablePositionsForAliases.setAutoDelete(true);
			visibility.fill(false);
		}
		~QuerySchemaPrivate()
		{
			delete fieldsExpanded;
			delete autoincFields;
			delete fieldsOrder;
			delete pkeyFieldsOrder;
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
		
		/*! Parent table of the query. (may be NULL)
			Any data modifications can be performed if we know parent table.
			If null, query's records cannot be modified. */
		TableSchema *parent_table;
		
		/*! List of tables used in this query */
		TableSchema::List tables;
		
		/*! Used to mapping columns to its aliases for this query */
		QIntDict<QCString> aliases;
		
		/*! Used to mapping tables to its aliases for this query */
		QIntDict<QCString> tableAliases;
		
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

		/*! A cache for autoIncrementFields(). */
		QueryColumnInfo::List *autoincFields;
		
		/*! A cache for autoIncrementSQLFieldsList(). */
		QString autoIncrementSQLFieldsList;
		QGuardedPtr<Driver> lastUsedDriverForAutoIncrementSQLFieldsList;

		/*! A map for fast lookup of query fields' order.
		 This is exactly opposite information compared to vector returned 
		 by fieldsExpanded() */
		QMap<QueryColumnInfo*,uint> *fieldsOrder;

//		QValueList<bool> detailedVisibility;

		/*! order of PKEY fields (e.g. for updateRow() ) */
		QValueVector<uint> *pkeyFieldsOrder;

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
		QValueVector<int> tablesBoundToColumns;
		
		/* Collects table positions for aliases: used in tablePositionsForAlias(). */
		QAsciiDict<int> tablePositionsForAliases;
};
}

//=======================================

//=======================================

QuerySchema::QuerySchema()
	: FieldList(false)//fields are not owned by QuerySchema object
	, SchemaData(KexiDB::QueryObjectType)
	, d( new QuerySchemaPrivate() )
{
	init();
}

QuerySchema::QuerySchema(TableSchema* tableSchema)
	: FieldList(false)
	, SchemaData(KexiDB::QueryObjectType)
	, d( new QuerySchemaPrivate() )
{
	d->parent_table = tableSchema;
	assert(d->parent_table);
	init();
	if (!d->parent_table) {
		m_name = QString::null;
		return;
	}
	addTable(d->parent_table);
	//defaults:
	//inherit name from a table
	m_name = d->parent_table->name();
	//inherit caption from a table
	m_caption = d->parent_table->caption();
	//add all fields of the table as asterisk:
	addField( new QueryAsterisk(this) );
}

QuerySchema::~QuerySchema()
{
	delete d;
}

void QuerySchema::init()
{
	m_type = KexiDB::QueryObjectType;
}

void QuerySchema::clear()
{
	FieldList::clear();
	SchemaData::clear();
	d->aliases.clear();
	d->tableAliases.clear();
	d->asterisks.clear();
	d->relations.clear();
	d->parent_table = 0;
	d->tables.clear();
	d->clearCachedData();
	if (d->pkeyFieldsOrder) {
		delete d->pkeyFieldsOrder;
		d->pkeyFieldsOrder=0;
	}
	d->visibility.fill(false);
	d->tablesBoundToColumns = QValueVector<int>(64,-1);
	d->tablePositionsForAliases.clear();
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
	if (!field)
		return *this;
	if (position>m_fields.count()) {
		kdWarning() << "QuerySchema::insertField(): position (" << position << ") out of range" << endl;
		return *this;
	}
	if (fieldCount()>=d->visibility.size()) {
		d->visibility.resize(d->visibility.size()*2);
		d->tablesBoundToColumns.resize(d->tablesBoundToColumns.size()*2);
	}
	d->clearCachedData();
	if (!field->isQueryAsterisk() && !field->table()) {
		KexiDBDbg << "QuerySchema::addField(): WARNING: field '"<<field->name()
			<<"' must contain table information!" <<endl;
		return *this;
	}
	FieldList::insertField(position, field);
	if (field->isQueryAsterisk()) {
		d->asterisks.append(field);
		//if this is single-table asterisk,
		//add a table to list if doesn't exist there:
		if (field->table() && (d->tables.findRef(field->table())==-1))
			d->tables.append(field->table());
	}
	else {
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
		kdWarning() << "QuerySchema::insertField(): bindToTable (" << bindToTable 
			<< ") out of range" << endl;
		bindToTable = -1;
	}
	//--move items to make a place for a new one
	for (uint i=fieldCount()-1; i>position; i--)
		d->tablesBoundToColumns[i] = d->tablesBoundToColumns[i-1];
	d->tablesBoundToColumns[ position ] = bindToTable;
	
	kdDebug() << "QuerySchema::insertField(): bound to table (" << bindToTable << "): " <<endl;
	if (bindToTable==-1)
		kdDebug() << " <NOT SPECIFIED>" << endl; 
	else
		kdDebug() << " name=" << d->tables.at(bindToTable)->name() 
			<< " alias=" << tableAlias(bindToTable) <<  endl;
	QString s;
	for (uint i=0; i<fieldCount();i++)
		s+= (QString::number(d->tablesBoundToColumns[i]) + " ");
	kdDebug() << "tablesBoundToColumns == [" << s << "]" <<endl;
	return *this;
}

int QuerySchema::tableBoundToColumn(uint columnPosition) const
{
	if (columnPosition > d->tablesBoundToColumns.count()) {
		kdWarning() << "QuerySchema::tableBoundToColumn(): columnPosition (" << columnPosition
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
	return d->parent_table ? d->parent_table->connection() : 0;
}

QString QuerySchema::debugString()
{
	QString dbg;
	dbg.reserve(1024);
	//fields
	dbg = QString("QUERY ") + schemaDataDebugString() + "\n"
		+ "-PARENT_TABLE=" + (d->parent_table ? d->parent_table->name() :"(NULL)")
		+ "\n-COLUMNS:\n"
		+ FieldList::debugString();

	//bindings
	QString dbg2;
	dbg2.reserve(512);
	for (uint i = 0; i<fieldCount(); i++) {
		int tablePos = tableBoundToColumn(i);
		if (tablePos>=0) {
			QCString tAlias = tableAlias(tablePos);
			if (!tAlias.isEmpty()) {
				dbg2 += (QString::fromLatin1(" field \"") + field(i)->name() 
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
	Field::ListIterator it( m_fields );
	if (d->aliases.isEmpty())
		aliases = "<NONE>\n";
	else {
		for (int i=0; it.current(); ++it, i++) {
			QCString *alias = d->aliases[i];
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
			QCString *alias = d->tableAliases[i];
			if (alias)
				aliases += (QString("table #%1: ").arg(i) 
					+ (t_it.current()->name().isEmpty() ? "<noname>" : t_it.current()->name())
					+ " -> " + (const char*)*alias + "\n");
		}
	}
	dbg += QString("-TABLE ALIASES:\n" + aliases);
	return dbg;
}

TableSchema* QuerySchema::parentTable() const
{
	return d->parent_table;
}

void QuerySchema::setParentTable(TableSchema *table)
{ 
	if (table)
		d->parent_table=table; 
}

TableSchema::List* QuerySchema::tables() const
{
	return &d->tables;
}

void QuerySchema::addTable(TableSchema *table, const QCString& alias)
{
	kdDebug() << "QuerySchema::addTable() " << (void *)table 
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
					kdWarning() << "QuerySchema::addTable(): table with \"" 
						<<tAlias<<"\" alias already added!" << endl;
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
	if (d->parent_table == table)
		d->parent_table = 0;
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
	return d->tables.find(table)!=-1;
}

QCString QuerySchema::columnAlias(uint position) const
{
	QCString *a = d->aliases[position];
	return a ? *a : QCString();
}

bool QuerySchema::hasColumnAlias(uint position) const
{
	return d->aliases[position]!=0;
}

void QuerySchema::setColumnAlias(uint position, const QCString& alias)
{
	if (position >= m_fields.count()) {
		KexiDBWarning << "QuerySchema::setColumnAlias(): position ("  << position 
			<< ") out of range!" << endl;
		return;
	}
	QCString fixedAlias = alias.lower().stripWhiteSpace();
	if (fixedAlias.isEmpty()) {
		d->aliases.remove(position);
		d->maxIndexWithAlias = -1;
	}
	else {
		d->aliases.replace(position, new QCString(fixedAlias));
		d->maxIndexWithAlias = QMAX( d->maxIndexWithAlias, (int)position );
	}
}

QCString QuerySchema::tableAlias(uint position) const
{
	QCString *a = d->tableAliases[position];
	return a ? *a : QCString();
}

int QuerySchema::tablePositionForAlias(const QCString& name) const
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

QValueList<int> QuerySchema::tablePositions(const QString& tableName) const
{
	int num = 0;
	QValueList<int> result;
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

void QuerySchema::setTableAlias(uint position, const QCString& alias)
{
	if (position >= d->tables.count()) {
		KexiDBWarning << "QuerySchema::setTableAlias(): position ("  << position 
			<< ") out of range!" << endl;
		return;
	}
	QCString fixedAlias = alias.lower().stripWhiteSpace();
	if (fixedAlias.isEmpty()) {
		QCString *oldAlias = d->tableAliases.take(position);
		if (oldAlias) {
			d->tablePositionsForAliases.remove(*oldAlias);
			delete oldAlias;
		}
//			d->maxIndexWithTableAlias = -1;
	}
	else {
		d->tableAliases.replace(position, new QCString(fixedAlias));
		d->tablePositionsForAliases.replace(fixedAlias, new int(position));
//		d->maxIndexWithTableAlias = QMAX( d->maxIndexWithTableAlias, (int)index );
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

QueryColumnInfo::Vector QuerySchema::fieldsExpanded()
{
	if (d->fieldsExpanded) {
//		if (detailedVisibility)
//			*detailedVisibility = d->detailedVisibility;
		return *d->fieldsExpanded;
	}

//	if (detailedVisibility)
//		detailedVisibility->clear();

//	d->detailedVisibility.clear();

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
					list.append( new QueryColumnInfo(ast_f, QCString()/*no field for asterisk!*/,
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
						list.append( new QueryColumnInfo(tab_f, QCString()/*no field for asterisk!*/,
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
		d->fieldsOrder = new QMap<QueryColumnInfo*,uint>();
	}
	else {//for future:
		d->fieldsExpanded->clear();
		d->fieldsExpanded->resize( list.count() );
		d->fieldsOrder->clear();
	}
	//fill the vector and the map
	QueryColumnInfo::ListIterator it(list);
	for (i=0; it.current(); ++it, i++)
	{
		d->fieldsExpanded->insert(i,it.current());
		d->fieldsOrder->insert(it.current(),i);
	}
//	if (detailedVisibility)
//		*detailedVisibility = d->detailedVisibility;
	return *d->fieldsExpanded;
}

QMap<QueryColumnInfo*,uint> QuerySchema::fieldsOrder()
{
	if (!d->fieldsOrder)
		(void)fieldsExpanded();
	return *d->fieldsOrder;
}

QValueVector<uint> QuerySchema::pkeyFieldsOrder()
{
	if (d->pkeyFieldsOrder)
		return *d->pkeyFieldsOrder;

	TableSchema *tbl = parentTable();
	if (!tbl || !tbl->primaryKey())
		return QValueVector<uint>();

	//get order of PKEY fields (e.g. for save() )
	IndexSchema *pkey = tbl->primaryKey();
	if (!d->pkeyFieldsOrder) {
		d->pkeyFieldsOrder = new QValueVector<uint>( pkey->fieldCount() );
	}
//			d->pkeyFieldsOrder->reserve(pkey->fieldCount());
	const uint fCount = fieldsExpanded().count();
	for (uint i=0, j=0; i<fCount; i++) {
		QueryColumnInfo *fi = d->fieldsExpanded->at(i);
		if (fi->field->table()==tbl && pkey->field(fi->field->name())!=0) {
			KexiDBDbg << "Cursor::init(): FIELD " << fi->field->name() << " IS IN PKEY" << endl;
			(*d->pkeyFieldsOrder)[j]=i;
			j++;
		}
	}
	return *d->pkeyFieldsOrder;
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
	if (!d->parent_table) {
		KexiDBWarning << "QuerySchema::autoIncrementFields(): no parent table!" << endl;
		return d->autoincFields;
	}
	if (d->autoincFields->isEmpty()) {//no cache
		QueryColumnInfo::Vector fexp = fieldsExpanded();
		for (int i=0; i<(int)fexp.count(); i++) {
			QueryColumnInfo *fi = fexp[i];
			if (fi->field->table() == d->parent_table && fi->field->isAutoIncrement()) {
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

/*
	new field1, Field *field2
	if (!field1 || !field2) {
		kdWarning() << "QuerySchema::addRelationship(): !masterField || !detailsField" << endl;
		return;
	}
	if (field1->isQueryAsterisk() || field2->isQueryAsterisk()) {
		kdWarning() << "QuerySchema::addRelationship(): relationship's fields cannot be asterisks" << endl;
		return;
	}
	if (!hasField(field1) && !hasField(field2)) {
		kdWarning() << "QuerySchema::addRelationship(): fields do not belong to this query" << endl;
		return;
	}
	if (field1->table() == field2->table()) {
		kdWarning() << "QuerySchema::addRelationship(): fields cannot belong to the same table" << endl;
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
	m_type = Field::Asterisk;
}

QueryAsterisk::~QueryAsterisk()
{
}

void QueryAsterisk::setTable(TableSchema *table)
{
	kdDebug() << "QueryAsterisk::setTable()" << endl;
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

