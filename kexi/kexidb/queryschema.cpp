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
{
	init();
}

QuerySchema::QuerySchema(TableSchema* tableSchema)
	: FieldList(false)
	, SchemaData(KexiDB::QueryObjectType)
//	, m_conn(0)
	, m_parent_table(tableSchema)
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
}

void QuerySchema::init()
{
	m_type = KexiDB::QueryObjectType;
	m_tables.setAutoDelete(false);
	m_asterisks.setAutoDelete(true);
	m_fieldsExpanded.setAutoDelete(false); //it is temporary
}

void QuerySchema::clear()
{
	FieldList::clear();
	SchemaData::clear();
	m_aliases.clear();
	m_asterisks.clear();
	m_parent_table = 0;
	m_tables.clear();
//	m_conn = 0;
}

KexiDB::FieldList& QuerySchema::addField(KexiDB::Field* field)
{
	if (!field || (!field->isQueryAsterisk() && !field->table()))
		return *this;
	FieldList::addField(field);
	if (field->isQueryAsterisk()) {
		m_asterisks.append(field);
		//if this is single-table asterisk,
		//add a table to list if not exists there:
		if (field->table() && (m_tables.find(field->table())==-1))
			m_tables.append(field->table());
	}
	else {
		//add a table to list if not exists there:
		if (m_tables.find(field->table())==-1)
			m_tables.append(field->table());
	}
	
	return *this;
}

Connection* QuerySchema::connection()
{
	return m_parent_table ? m_parent_table->connection() : 0;
}

void QuerySchema::debug()
{
	KexiDBDbg << "QUERY " << schemaDataDebugString() << endl;
	KexiDBDbg << "  PARENT_TABLE=" << (m_parent_table ? m_parent_table->name() :"(NULL)") << endl;
	FieldList::debug();
	TableSchema *table;
	QString table_names;
	for ( table = m_tables.first(); table; table = m_tables.next() ) {
		if (!table_names.isEmpty())
			table_names += ", ";
		table_names += table->name();
	}
	KexiDBDbg << "  TABLES: " << table_names << endl;
	QMap<Field*, QString>::Iterator it;
	QString aliases;
	for ( it = m_aliases.begin(); it != m_aliases.end(); ++it ) {
		aliases += (it.key()->name() + " -> " + it.data() + "\n");
	}
	KexiDBDbg << "  ALIASES: " << aliases << endl;
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
	if (!table)
		return;
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

Field::List* QuerySchema::fieldsExpanded()
{
	m_fieldsExpanded.clear();
	for (Field *f = m_fields.first(); f; f = m_fields.next()) {
		if (f->isQueryAsterisk()) {
			if (static_cast<QueryAsterisk*>(f)->isSingleTableAsterisk()) {
				Field::List *ast_fields = static_cast<QueryAsterisk*>(f)->table()->fields();
				for (Field *ast_f = ast_fields->first(); ast_f; ast_f=ast_fields->next()) {
					m_fieldsExpanded.append(ast_f);
				}
			}
			else {//all-tables asterisk: itereate through table list
				for (TableSchema *table = m_tables.first(); table; table = m_tables.next()) {
					//add all fields from this table
					Field::List *tab_fields = table->fields();
					for (Field *tab_f = tab_fields->first(); tab_f; tab_f = tab_fields->next()) {
//! \todo (js): perhaps not all fields should be appended here
						m_fieldsExpanded.append(tab_f);
					}
				}
			}
		}
		else {
			m_fieldsExpanded.append(f);
		}
	}
	return &m_fieldsExpanded;
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
		dbg += ("SINGLE-TABLE ASTERISK (*." + table()->name() + ")");
	}
	return dbg;
}

