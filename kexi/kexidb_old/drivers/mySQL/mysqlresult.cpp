/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
*/

#include <qvariant.h>

#include <kdebug.h>

#include "mysqlresult.h"

MySqlResult::MySqlResult(MYSQL_RES *result, QObject *parent) : KexiDBResult(parent)
{
	//various initialisations...
	m_result = result;
//	m_row = new MYSQL_ROW;
	m_row = 0;
	m_currentRecord = 0;
//	m_fieldNames = 0;
	m_numFields = mysql_num_fields(m_result);
	
	//creating field-index
	MYSQL_FIELD *field;
	int i=0;
	while((field = mysql_fetch_field(m_result)))
	{
		m_fieldNames.insert(QString::fromLatin1(field->name), i);
		i++;
	}
}

unsigned int
MySqlResult::numRows()
{
	return mysql_num_rows(m_result);
}

bool
MySqlResult::next()
{
	m_row = mysql_fetch_row(m_result);
	if(!m_row)
		return false;
	return true;
}

QVariant
MySqlResult::value(unsigned int field)
{
	if(!m_row)
		return 0;
	QVariant v((m_row)[field]);
	return v;
}

QVariant
MySqlResult::value(QString field)
{
	FieldNames::Iterator it;
	it = m_fieldNames.find(field);
	QVariant v((m_row)[it.data()]);
	return v;
}

MySqlResult::~MySqlResult()
{
	mysql_free_result(m_result);
}

#include "mysqlresult.moc"
