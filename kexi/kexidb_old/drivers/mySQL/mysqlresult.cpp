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
#include <qdatetime.h>

#include <kdebug.h>

#include <mysqlresult.h>
#include <mysqlfield.h>

MySqlResult::MySqlResult(MYSQL_RES *result, QObject *parent) : KexiDBResult(parent)
{
	//various initialisations...
	m_result = result;
	m_row = 0;
	m_lengths = 0;
	m_field = 0;
	m_currentRecord = 0;
	m_numFields = mysql_num_fields(m_result);
	kdDebug()<<"m_numFields:"<<m_numFields<<endl;
	//creating field-index
	int i=0;
	while((m_field = mysql_fetch_field(m_result)))
	{
		//field name...
		m_fieldNames.insert(QString::fromLatin1(m_field->name), i);
		//field infos...
		MySqlField *f = new MySqlField(m_field, i);
		m_fields.insert(i, f);
		
		//incrementing...
		i++;
	}
}

unsigned int
MySqlResult::numFields()
{
	return m_numFields;
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
	m_lengths = mysql_fetch_lengths(m_result);
	
	if(!m_row)
	{
		return false;
	}
	
	m_currentRecord++;
	return true;
}

QVariant
MySqlResult::value(unsigned int field)
{
	if(!m_row)
	{
		return 0;
	}
	
	switch(fieldInfo(field)->type())
	{
		case QVariant::Date:
			return QVariant(QDate::fromString((m_row)[field], Qt::ISODate));
		case QVariant::Int:
			return QVariant(QString((m_row)[field]).toInt());
		case QVariant::ByteArray:
		{
			QByteArray ba;
			ba.duplicate((m_row)[field], m_lengths[field]);
			return QVariant(ba);
		}
		default:
			return QVariant((m_row)[field]);
	}

	return 0;
}

QVariant
MySqlResult::value(QString field)
{
	FieldNames::Iterator it;
	it = m_fieldNames.find(field);
	return value(it.data());
}

KexiDBField*
MySqlResult::fieldInfo(unsigned int field)
{
	return m_fields[field];
}

KexiDBField*
MySqlResult::fieldInfo(QString field)
{
	FieldNames::Iterator it;
	it = m_fieldNames.find(field);
	return fieldInfo(it.data());
}

unsigned int
MySqlResult::currentRecord()
{
	return m_currentRecord;
}

MySqlResult::~MySqlResult()
{
	mysql_free_result(m_result);
}

#include "mysqlresult.moc"
