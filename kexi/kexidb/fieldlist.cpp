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

#include <kexidb/fieldlist.h>

using namespace KexiDB;

FieldList::FieldList()
{
	m_fields.setAutoDelete( true );
}

FieldList::~FieldList()
{
}

void FieldList::addField(KexiDB::Field *field)
{
//	field.setTable(m_name);
	m_fields.append(field);
}
/*
void Table::addPrimaryKey(const QString& key)
{
	m_primaryKeys.append(key);
}*/

/*QStringList Table::primaryKeys() const
{
	return m_primaryKeys;
}

bool Table::hasPrimaryKeys() const
{
	return !m_primaryKeys.isEmpty();
}
*/

KexiDB::Field* FieldList::field(unsigned int id)
{
	if (id < m_fields.count())
		return m_fields.at(id);
	return 0;
}

unsigned int FieldList::fieldCount() const
{
	return m_fields.count();
}

