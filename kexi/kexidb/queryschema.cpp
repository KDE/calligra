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

QuerySchema::QuerySchema()
	: FieldList(false)//fields are not owned by QuerySchema object
	, SchemaData(KexiDB::QueryObjectType)
//	, m_conn(0)
	, m_parent_table(0)
	, m_visibility(64)
{
	init();
}

QuerySchema::QuerySchema(TableSchema* tableSchema)
	: FieldList(false)
	, SchemaData(KexiDB::QueryObjectType)
//	, m_conn(0)
	, m_parent_table(tableSchema)
	, m_visibility(64)
{
	init();
	assert(m_parent_table);
	if (!m_parent_table) {
		m_name = QString::null;
		return;
	}
	addTable(m_parent_table);
	//defaults:
	//inherit name from a table
	m_name = m_parent_table->name();
	//inherit caption from a table
	m_caption = m_parent_table->caption();
	//add all fields of the table as asterisk:
	addField( new QueryAsterisk(this) );
}

QuerySchema::~QuerySchema()
{
	delete m_fieldsExpanded;
	delete m_pkeyFieldsOrder;
}

void QuerySchema::init()
{
	m_type = KexiDB::QueryObjectType;
	m_tables.setAutoDelete(false);
	m_asterisks.setAutoDelete(true);
	m_fieldsExpanded=0;
	m_fieldsOrder=0;
	m_pkeyFieldsOrder=0;
	m_visibility.fill(false);
}

void QuerySchema::clear()
{
	FieldList::clear();
	SchemaData::clear();
	m_aliases.clear();
	m_asterisks.clear();
	m_parent_table = 0;
	m_tables.clear();
	if (m_fieldsExpanded) {
		delete m_fieldsExpanded;
		m_fieldsExpanded=0;
		delete m_fieldsOrder;
		m_fieldsOrder=0;
	}
	if (m_pkeyFieldsOrder) {
		delete m_pkeyFieldsOrder;
		m_pkeyFieldsOrder=0;
	}
	m_visibility.fill(false);
}

KexiDB::FieldList& QuerySchema::addField(KexiDB::Field* field, bool visible)
{
	if (!field)
		return *this;
	if (fieldCount()>=m_visibility.size())
		m_visibility.resize(m_visibility.size()*2);
	if (m_fieldsExpanded) {
		delete m_fieldsExpanded;
		m_fieldsExpanded = 0;
		delete m_fieldsOrder;
		m_fieldsOrder = 0;
		m_detailedVisibility.clear();
	}
	if (!field->isQueryAsterisk() && !field->table()) {
		KexiDBDbg << "QuerySchema::addField(): WARNING: field '"<<field->name()<<"' must contain table information!" <<endl;
		return *this;
	}
	FieldList::addField(field);
	if (field->isQueryAsterisk()) {
		m_asterisks.append(field);
		//if this is single-table asterisk,
		//add a table to list if not exists there:
		if (field->table() && (m_tables.findRef(field->table())==-1))
			m_tables.append(field->table());
	}
	else {
		//add a table to list if not exists there:
		if (m_tables.findRef(field->table())==-1)
			m_tables.append(field->table());
	}
//	//visible by default
//	setFieldVisible(field, true);
	m_visibility.setBit(fieldCount()-1, visible);
	return *this;
}

bool QuerySchema::isFieldVisible(uint number) const
{
	return m_visibility.testBit(number);
}

void QuerySchema::setFieldVisible(uint number, bool v)
{
	m_visibility.setBit(number, v);
}

#if 0
bool QuerySchema::isFieldVisible(KexiDB::Field *f) const
{
	return m_visibility[f]!=0;
}

void QuerySchema::setFieldVisible(KexiDB::Field *f, bool v)
{
	m_visibility.take(f);
	if (!v)
		return;
	m_visibility.insert(f, f);
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
	return m_parent_table ? m_parent_table->connection() : 0;
}

void QuerySchema::debug()
{
	KexiDBDbg << "QUERY " << schemaDataDebugString() << endl;
	KexiDBDbg << "  -PARENT_TABLE=" << (m_parent_table ? m_parent_table->name() :"(NULL)") << endl;
	KexiDBDbg << "  -FIELDS/ASTERISKS: " << endl;
	FieldList::debug();

	TableSchema *table;
	QString table_names;
	for ( table = m_tables.first(); table; table = m_tables.next() ) {
		if (!table_names.isEmpty())
			table_names += ", ";
		table_names += table->name();
	}
	if (m_tables.isEmpty())
		table_names = "<NONE>";
	KexiDBDbg << "  -TABLES: " << table_names << endl;
	QMap<Field*, QString>::Iterator it;
	QString aliases;
	for ( it = m_aliases.begin(); it != m_aliases.end(); ++it ) {
		aliases += (it.key()->name() + " -> " + it.data() + "\n");
	}
	if (m_aliases.isEmpty())
		aliases = "<NONE>";
	KexiDBDbg << "  -ALIASES: " << aliases << endl;
}

TableSchema* QuerySchema::parentTable() const
{
	return m_parent_table;
}

void QuerySchema::setParentTable(TableSchema *table)
{ 
	if (table)
		m_parent_table=table; 
}

void QuerySchema::addTable(TableSchema *table)
{
	kdDebug() << "QuerySchema::addTable() " << (void *)table << endl;
	if (!table)
		return;
	if (m_tables.findRef(table)==-1)
		m_tables.append(table);
}

void QuerySchema::removeTable(TableSchema *table)
{
	if (!table)
		return;
	if (m_parent_table == table)
		m_parent_table = 0;
	m_tables.remove(table);
}

void QuerySchema::setAlias(Field *field, const QString& alias)
{
	if (!field)
		return;
	if (alias.isEmpty()) {
		m_aliases.remove(field);
		return;
	}
	m_aliases[field] = alias;
}

Field::Vector QuerySchema::fieldsExpanded(QValueList<bool> *detailedVisibility)
{
	if (m_fieldsExpanded) {
		if (detailedVisibility)
			*detailedVisibility = m_detailedVisibility;
		return *m_fieldsExpanded;
	}

	if (detailedVisibility)
		detailedVisibility->clear();

	m_detailedVisibility.clear();

	Field::List list;
	int i = 0;
	Field *f;
	int fieldNumber = 0;
	for (f = m_fields.first(); f; f = m_fields.next(), fieldNumber++) {
		if (f->isQueryAsterisk()) {
			if (static_cast<QueryAsterisk*>(f)->isSingleTableAsterisk()) {
				Field::List *ast_fields = static_cast<QueryAsterisk*>(f)->table()->fields();
				for (Field *ast_f = ast_fields->first(); ast_f; ast_f=ast_fields->next()) {
					m_detailedVisibility += isFieldVisible(fieldNumber);
					list.append(ast_f);
				}
			}
			else {//all-tables asterisk: itereate through table list
				for (TableSchema *table = m_tables.first(); table; table = m_tables.next()) {
					//add all fields from this table
					Field::List *tab_fields = table->fields();
					for (Field *tab_f = tab_fields->first(); tab_f; tab_f = tab_fields->next()) {
//! \todo (js): perhaps not all fields should be appended here
						m_detailedVisibility += isFieldVisible(fieldNumber);
						list.append(tab_f);
					}
				}
			}
		}
		else {
			m_detailedVisibility += isFieldVisible(fieldNumber);
			list.append(f);
		}
	}
	if (!m_fieldsExpanded) {
		m_fieldsExpanded = new Field::Vector( list.count() );
		m_fieldsOrder = new QMap<Field*,uint>();
	}
	else {//for future:
		m_fieldsExpanded->clear();
		m_fieldsExpanded->resize( list.count() );
		m_fieldsOrder->clear();
	}
	for (i=0, f = list.first(); f; f = list.next(), i++) {
		m_fieldsExpanded->insert(i,f);
		m_fieldsOrder->insert(f,i);
	}
	if (detailedVisibility)
		*detailedVisibility = m_detailedVisibility;
	return *m_fieldsExpanded;
}

QMap<Field*,uint> QuerySchema::fieldsOrder()
{
	if (!m_fieldsOrder)
		(void)fieldsExpanded();
	return *m_fieldsOrder;
}

QValueVector<uint> QuerySchema::pkeyFieldsOrder()
{
	if (m_pkeyFieldsOrder)
		return *m_pkeyFieldsOrder;

	TableSchema *tbl = parentTable();
	if (!tbl || !tbl->primaryKey())
		return QValueVector<uint>();

	//get order of PKEY fields (e.g. for save() )
	IndexSchema *pkey = tbl->primaryKey();
	if (!m_pkeyFieldsOrder) {
		m_pkeyFieldsOrder = new QValueVector<uint>( pkey->fieldCount() );
	}
//			m_pkeyFieldsOrder->reserve(pkey->fieldCount());
	const uint fCount = fieldsExpanded().count();
	for (uint i=0, j=0; i<fCount; i++) {
		Field *f = m_fieldsExpanded->at(i);
		if (f->table()==tbl && pkey->field(f->name())!=0) {
			KexiDBDbg << "Cursor::init(): FIELD " << f->name() << " IS IN PKEY" << endl;
			(*m_pkeyFieldsOrder)[j]=i;
			j++;
		}
	}
	return *m_pkeyFieldsOrder;
}

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

QString QueryAsterisk::debugString() const
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

