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
	m_type = KexiDB::QueryObjectType;
//	m_indices.setAutoDelete( true );
}

QuerySchema::QuerySchema(TableSchema* tableSchema)
	: FieldList(false)
	, SchemaData(KexiDB::QueryObjectType)
//	, m_conn(0)
	, m_parent_table(tableSchema)
{
	m_type = KexiDB::QueryObjectType;
	assert(m_parent_table);
	if (!m_parent_table) {
		m_name = QString::null;
		return;
	}
	//defaults:
	//inherit name from a table
	m_name = m_parent_table->name();
	//inherit caption from a table
	m_caption = m_parent_table->caption();
}

/*QuerySchema::QuerySchema(Connection *conn)
	: FieldList(false)
	, m_conn( conn )
{
	assert(conn);
}*/

QuerySchema::~QuerySchema()
{
}

void QuerySchema::clear()
{
	FieldList::clear();
	SchemaData::clear();
	m_parent_table = 0;
//	m_conn = 0;
}

KexiDB::FieldList& QuerySchema::addField(KexiDB::Field* field)
{
	FieldList::addField(field);
	//Check for auto-generated indices:

	//TODO?
	return *this;
}

Connection* QuerySchema::connection()
{
	return m_parent_table ? m_parent_table->connection() : 0;
}

void QuerySchema::debug() const
{
	KexiDBDbg << "QUERY " << schemaDataDebugString() << endl;
	FieldList::debug();
}

TableSchema* QuerySchema::parentTable() const
{
	return m_parent_table;
}


