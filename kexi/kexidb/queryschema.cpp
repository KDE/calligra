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

#include <kexidb/query.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>

#include <assert.h>

#include <kdebug.h>

using namespace KexiDB;

Query::Query(const QString& name)
	: FieldList(name)
	, m_id(0)
	, m_conn(0)
	, m_parent_table(0)
{
//	m_indices.setAutoDelete( true );
}

Query::Query()
	: FieldList()
	, m_name("")
	, m_id(0)
	, m_conn(0)
	, m_parent_table(0)
{
}

Query::Query(Table* table, const QString & name)
	: FieldList(name)
	, m_id(0)
	, m_conn(0)
	, m_parent_table(table)
{
	assert(m_parent_table);
	if (!m_parent_table) {
		m_name = QString::null;
		return;
	}
	//parent table
	if (m_name.isEmpty()) //inherit name from a table
		m_name = m_parent_table->name();
	
}

Query::~Query()
{
}

void Query::clear()
{
	FieldList::clear();
	m_parent_table = 0;
	m_conn = 0;
}

void Query::addField(KexiDB::Field* field)
{
	FieldList::addField(field);
	//Check for auto-generated indices:

	//TODO?
}

Query::Query(const QString & name, Connection *conn)
	: m_name( name )
	, m_conn( conn )
{
	assert(conn);
}

Connection* Query::connection()
{
	return m_conn;
}

void Query::debug()
{
	kdDebug() << "QUERY " << m_name << endl;
	FieldList::debug();
}

