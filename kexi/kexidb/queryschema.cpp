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
		{
			aliases.setAutoDelete(true);
			asterisks.setAutoDelete(true);
			relations.setAutoDelete(true);
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
		
		/*! Used to mapping Fields to its aliases for this query */
		QIntDict<QCString> aliases;
		
		/*! Helper */
		int maxIndexWithAlias;

		/*! Used to store visibility flag for every field */
		QBitArray visibility;

		/*! List of asterisks defined for this query  */
		Field::List asterisks;

		/*! Temporary field vector for using in fieldsExpanded() */
//		Field::Vector *fieldsExpanded;
		QueryFieldInfo::Vector *fieldsExpanded;

		/*! A cache for autoIncrementFields(). */
		QueryFieldInfo::List *autoincFields;
		
		/*! A cache for autoIncrementSQLFieldsList(). */
		QString autoIncrementSQLFieldsList;

		/*! A map for fast lookup of query fields' order.
		 This is exactly opposite information compared to vector returned 
		 by fieldsExpanded() */
		QMap<QueryFieldInfo*,uint> *fieldsOrder;

//		QValueList<bool> detailedVisibility;

		/*! order of PKEY fields (e.g. for updateRow() ) */
		QValueVector<uint> *pkeyFieldsOrder;

		/*! forced (predefined) statement */
		QString statement;

		/*! Relationships defined for this query. */
		Relationship::List relations;
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
}

//KexiDB::FieldList& QuerySchema::addField(KexiDB::Field* field, bool visible)
FieldList& QuerySchema::insertField(uint index, Field *field, bool visible)
{
	if (!field)
		return *this;
	if (index>m_fields.count()) {
		kdWarning() << "QuerySchema::insertField(): index (" << index << ") out of range" << endl;
		return *this;
	}
	if (fieldCount()>=d->visibility.size())
		d->visibility.resize(d->visibility.size()*2);
	d->clearCachedData();
	if (!field->isQueryAsterisk() && !field->table()) {
		KexiDBDbg << "QuerySchema::addField(): WARNING: field '"<<field->name()<<"' must contain table information!" <<endl;
		return *this;
	}
	FieldList::insertField(index, field);
	if (field->isQueryAsterisk()) {
		d->asterisks.append(field);
		//if this is single-table asterisk,
		//add a table to list if not exists there:
		if (field->table() && (d->tables.findRef(field->table())==-1))
			d->tables.append(field->table());
	}
	else {
		//add a table to list if not exists there:
		if (d->tables.findRef(field->table())==-1)
			d->tables.append(field->table());
	}
//	//visible by default
//	setFieldVisible(field, true);
//	d->visibility.setBit(fieldCount()-1, visible);
	//update visibility (move bits to make a place for new one)
	for (uint i=fieldCount()-1; i>index; i--)
		d->visibility.setBit(i, d->visibility.testBit(i-1));
	d->visibility.setBit(index, visible);
	return *this;
}

FieldList& QuerySchema::insertField(uint index, Field *field)
{
	return insertField( index, field, true );
}

KexiDB::FieldList& QuerySchema::addField(KexiDB::Field* field, bool visible)
{
	return insertField(m_fields.count(), field, visible);
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

bool QuerySchema::isFieldVisible(uint number) const
{
	return (number < fieldCount()) ? d->visibility.testBit(number) : false;
}

void QuerySchema::setFieldVisible(uint number, bool v)
{
	d->visibility.setBit(number, v);
}

#if 0
bool QuerySchema::isFieldVisible(KexiDB::Field *f) const
{
	return d->visibility[f]!=0;
}

void QuerySchema::setFieldVisible(KexiDB::Field *f, bool v)
{
	d->visibility.take(f);
	if (!v)
		return;
	d->visibility.insert(f, f);
}
#endif

FieldList& QuerySchema::addAsterisk(QueryAsterisk *asterisk, bool visible)
{
	if (!asterisk)
		return *this;
	//make unique name
	asterisk->m_name = (asterisk->table() ? asterisk->table()->name() + ".*" : "*") 
		+ QString::number(asterisks()->count());
	return addField(asterisk, visible);
}

Connection* QuerySchema::connection()
{
	return d->parent_table ? d->parent_table->connection() : 0;
}

QString QuerySchema::debugString()
{
	QString dbg;
	dbg.reserve(1024);
	dbg = QString("QUERY ") + schemaDataDebugString() + "\n"
		+ "-PARENT_TABLE=" + (d->parent_table ? d->parent_table->name() :"(NULL)")
		+ "\n-COLUMNS:\n"
		+ FieldList::debugString();

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
	dbg += (QString("\n-TABLES:\n") + table_names);
	QString aliases;
	Field::ListIterator it( m_fields );
	if (d->aliases.isEmpty())
		aliases = "<NONE>";
	else {
		for (int i=0; it.current(); ++it, i++) {
			QCString *alias = d->aliases[i];
			if (alias)
				aliases += (QString("field #%1: ").arg(i) 
					+ (it.current()->name().isEmpty() ? "<noname>" : it.current()->name())
					+ " -> " + (const char*)*alias + "\n");
		}
	}
	dbg += QString("\n-ALIASES:\n" + aliases);
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

void QuerySchema::addTable(TableSchema *table)
{
	kdDebug() << "QuerySchema::addTable() " << (void *)table << endl;
	if (!table)
		return;
	if (d->tables.findRef(table)==-1)
		d->tables.append(table);
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

bool QuerySchema::contains(TableSchema *table) const
{
	return d->tables.find(table)!=-1;
}

QCString QuerySchema::alias(uint index) const
{
	QCString *a = d->aliases[index];
	return a ? *a : QCString();
}

bool QuerySchema::hasAlias(uint index) const
{
	return d->aliases[index]!=0;
}

void QuerySchema::setAlias(uint index, const QCString& alias)
{
	if (index >= m_fields.count()) {
		KexiDBWarning << "QuerySchema::setAlias(): index ("  << index << ") out of range!" << endl;
		return;
	}
	QCString fixedAlias = alias.lower().stripWhiteSpace();
	if (fixedAlias.isEmpty()) {
		d->aliases.remove(index);
		d->maxIndexWithAlias = -1;
	}
	else {
		d->aliases.replace(index, new QCString(fixedAlias));
		d->maxIndexWithAlias = QMAX( d->maxIndexWithAlias, (int)index );
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

QueryFieldInfo::Vector QuerySchema::fieldsExpanded()
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
	QueryFieldInfo::List list;
	int i = 0;
	int fieldNumber = 0;
	for (Field *f = m_fields.first(); f; f = m_fields.next(), fieldNumber++) {
		if (f->isQueryAsterisk()) {
			if (static_cast<QueryAsterisk*>(f)->isSingleTableAsterisk()) {
				Field::List *ast_fields = static_cast<QueryAsterisk*>(f)->table()->fields();
				for (Field *ast_f = ast_fields->first(); ast_f; ast_f=ast_fields->next()) {
//					d->detailedVisibility += isFieldVisible(fieldNumber);
					list.append( new QueryFieldInfo(ast_f, QCString()/*no field for asterisk!*/,
						isFieldVisible(fieldNumber)) 
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
//						d->detailedVisibility += isFieldVisible(fieldNumber);
//						list.append(tab_f);
						list.append( new QueryFieldInfo(tab_f, QCString()/*no field for asterisk!*/,
							isFieldVisible(fieldNumber)) 
						);
					}
				}
			}
		}
		else {
			//a single field
//			d->detailedVisibility += isFieldVisible(fieldNumber);
			list.append( new QueryFieldInfo(f, alias(fieldNumber), isFieldVisible(fieldNumber)) );
//			list.append(f);
		}
	}
	//prepare clean vector for expanded list, and a map for order information
	if (!d->fieldsExpanded) {
		d->fieldsExpanded = new QueryFieldInfo::Vector( list.count() );// Field::Vector( list.count() );
		d->fieldsExpanded->setAutoDelete(true);
		d->fieldsOrder = new QMap<QueryFieldInfo*,uint>();
	}
	else {//for future:
		d->fieldsExpanded->clear();
		d->fieldsExpanded->resize( list.count() );
		d->fieldsOrder->clear();
	}
	//fill the vector and the map
	QueryFieldInfo::ListIterator it(list);
	for (i=0; it.current(); ++it, i++)
	{
		d->fieldsExpanded->insert(i,it.current());
		d->fieldsOrder->insert(it.current(),i);
	}
//	if (detailedVisibility)
//		*detailedVisibility = d->detailedVisibility;
	return *d->fieldsExpanded;
}

QMap<QueryFieldInfo*,uint> QuerySchema::fieldsOrder()
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
		QueryFieldInfo *fi = d->fieldsExpanded->at(i);
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

QueryFieldInfo::List* QuerySchema::autoIncrementFields()
{
	bool noCache = d->autoincFields;
	if (!d->autoincFields) {
		d->autoincFields = new QueryFieldInfo::List();
	}
	if (!d->parent_table) {
		KexiDBWarning << "QuerySchema::autoIncrementFields(): no parent table!" << endl;
		return d->autoincFields;
	}
	if (noCache) {
		QueryFieldInfo::Vector fexp = fieldsExpanded();
		for (int i=0; i<(int)fexp.count(); i++) {
			QueryFieldInfo *fi = fexp[i];
			if (fi->field->table() == d->parent_table && fi->field->isAutoIncrement()) {
				d->autoincFields->append( fi );
			}
		}
	}
	return d->autoincFields;
}

QString QuerySchema::sqlFieldsList(QueryFieldInfo::List* infolist)
{
	if (!infolist)
		return QString::null;
	QString result;
	result.reserve(256);
	QueryFieldInfo::ListIterator it( *infolist );
	bool start = true;
	for (; it.current(); ++it) {
		if (!start)
			result += ",";
		else
			start = false;
		result += it.current()->field->name();
	}
	return result;
}

QString QuerySchema::autoIncrementSQLFieldsList()
{
	if (d->autoIncrementSQLFieldsList.isEmpty())
		d->autoIncrementSQLFieldsList = QuerySchema::sqlFieldsList( autoIncrementFields() );
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

