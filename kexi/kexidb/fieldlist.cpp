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
#include <kexidb/object.h>

#include <kdebug.h>

using namespace KexiDB;

FieldList::FieldList(bool owner)
{
	m_fields.setAutoDelete( owner );
}

FieldList::~FieldList()
{
}

void FieldList::clear()
{
//	m_name = QString::null;
	m_fields.clear();
}

FieldList& FieldList::addField(KexiDB::Field *field)
{
//	field.setTable(m_name);
	m_fields.append(field);
	return *this;
}

bool FieldList::isOwner() const
{
	return m_fields.autoDelete();
}

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

void FieldList::debug() const
{
	QString dbg;
	Field::ListIterator it( m_fields );
	Field *field;
	bool start = true;
	for (; (field = it.current())!=0; ++it) {
		if (!start)
			dbg += ",\n";
		else
			start = false;
		dbg += "  ";
		dbg += field->debugString();
	}
	KexiDBDbg << dbg;
}

