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

#include <kdebug.h>

#include "../../kexiDB/kexidb.h"

#include "mysqldb.h"
#include "mysqlresult.h"
#include "mysqlfield.h"
#include "mysqlrecord.h"

MySqlRecord::MySqlRecord(MYSQL_RES *result, MySqlDB *db, bool buffer, MySqlRecord *parent)
 : KexiDBRecord(), MySqlResult(result, db)
{
	m_db = db;
	m_lastItem = 0;
	m_readOnly = findKey();
}

bool
MySqlRecord::findKey()
{
	for(uint i=0; i < fieldCount(); i++)
	{
		if(MySqlResult::fieldInfo(i)->primary_key() || MySqlResult::fieldInfo(i)->unique_key())
		{
			m_keyField = MySqlResult::fieldInfo(i)->name();
			m_table = MySqlResult::fieldInfo(i)->table();
			return false;
		}
	}

	kdDebug() << "MySqlRecrod::findKey(): name is: " << m_keyField << endl;

	return true;
}

bool
MySqlRecord::readOnly()
{
	return m_readOnly;
}

void
MySqlRecord::reset()
{
	for(UpdateBuffer::Iterator it = m_updateBuffer.begin(); it != m_updateBuffer.end(); it++)
	{
		m_updateBuffer.remove(it);
	}
}

bool
MySqlRecord::commit(unsigned int record, bool insertBuffer)
{
	kdDebug() << "MySqlRecord::commit()" << endl;
	for(UpdateBuffer::Iterator it = m_updateBuffer.begin(); it != m_updateBuffer.end(); it++)
	{
		if((*it).record == record && (*it).done == false)
		{
			QString value = m_db->escape((*it).value.toString());
			QString key = m_db->escape(m_keyBuffer.find(record).data().toString());
			
			int index = m_insertList.findIndex(record);
			if(!insertBuffer)
			{
				kdDebug() << "MySqlRecord::commit: committing update" << endl;
				QString statement("update " + m_table + " set " + (*it).field + "='" + value + "' where " + m_keyField + "='" + key + "'");
				kdDebug() << "MySqlRecord::commit(): query: " << statement << endl;
				m_db->query(statement);
				(*it).done = true;
			}
			else if(insertBuffer && index != -1)
			{
				kdDebug() << "MySqlRecord::commit: committing suicide" << endl;
				QString statement("insert into " + m_table + " set " + (*it).field + " = '" + value + "'");
				kdDebug() << "MySqlRecord::commit(insert): " << statement << endl;
				m_db->query(statement);
				m_insertList.remove(m_insertList.at(index));
				(*it).done = true;
				
				// hopefully asign magic data (no metter how :)
				if(!m_keyBuffer.contains(record))
				{
					if((*it).field == m_keyField)
					{
						m_keyBuffer.insert(record, QVariant(value));
					}
					if(fieldInfo(m_keyField)->auto_increment())
					{
						m_keyBuffer.insert(record, QVariant((unsigned int)m_db->lastAuto()));
					}
					// else, maybe we should wait, and don't insert it by now...
					
					m_lastID = (unsigned int)m_db->lastAuto();
				}
			}
		}
	}
}

QVariant
MySqlRecord::value(unsigned int column)
{
	return MySqlResult::value(column);
}

QVariant
MySqlRecord::value(QString column)
{
	if(m_contentBuffer.count() > 0)
	{
		ContentBuffer::Iterator it = m_contentBuffer.find(column);
		return it.data();
	}
	else
	{
		return MySqlResult::value(column);
	}
}

QVariant::Type
MySqlRecord::type(unsigned int column)
{
	return MySqlResult::fieldInfo(column)->type();
}

QVariant::Type
MySqlRecord::type(QString column)
{
	return MySqlResult::fieldInfo(column)->type();
}

KexiDBField::ColumnType
MySqlRecord::sqlType(unsigned int)
{
	return KexiDBField::SQLInvalid;
}

KexiDBField::ColumnType
MySqlRecord::sqlType(QString)
{
	return KexiDBField::SQLInvalid;
}

KexiDBField*
MySqlRecord::fieldInfo(unsigned int column)
{
	return MySqlResult::fieldInfo(column);
}

KexiDBField*
MySqlRecord::fieldInfo(QString column)
{
	return MySqlResult::fieldInfo(column);
}

bool
MySqlRecord::update(unsigned int record, unsigned int field, QVariant value)
{
	kdDebug() << "MySqlRecord::update(uint): holding field '" << fieldInfo(field)->name() << "' for update" << endl;
	return update(record, MySqlResult::fieldInfo(field)->name(), value);
}

bool
MySqlRecord::update(unsigned int record, QString field, QVariant value)
{
	if(readOnly())
	{
		kdDebug() << "MySqlRecord::update(): record is read only, abroating..." << endl;
		return false;
	}

	UpdateItem i;
	i.record = record;
	i.field = field;
	i.value = value;
	i.done = false;

	m_updateBuffer.append(i);
	kdDebug() << "MySqlRecord::update(): we have now " << m_updateBuffer.count() << " items" << endl;
	return true;
}

int
MySqlRecord::insert()
{
//	return 0;
	if(readOnly())
		return -1;

	m_lastItem++;
	
	m_insertList.append(m_lastItem);
	return m_lastItem;
}

bool
MySqlRecord::deleteRecord(uint record)
{
	if(readOnly())
		return false;

	QString key = m_db->escape(m_keyBuffer.find(record).data().toString());
	QString statement("delete form " + m_table + " where " + m_keyField + " = '" + key + "'"); 
}

void
MySqlRecord::gotoRecord(unsigned int)
{
	return 0;
}

unsigned int
MySqlRecord::fieldCount()
{
	return MySqlResult::numFields();
}

QString
MySqlRecord::fieldName(unsigned int field)
{
	return MySqlResult::fieldInfo(field)->name();
}

bool
MySqlRecord::next()
{
	if(MySqlResult::next())
	{
//		m_keyContent = value(m_keyField);
		if(readOnly())
			return true;
		
		kdDebug() << "MySqlRecord::next(): hint: " << MySqlResult::currentRecord() - 1<< ", " << value(m_keyField).toString() << endl;
		m_keyBuffer.insert(MySqlResult::currentRecord() - 1, value(m_keyField));
		m_lastItem++;
		kdDebug() << "MySqlRecord::next(): but: " << m_keyBuffer[MySqlResult::currentRecord() - 1].toString() << " found" << endl;
		return true;
	}
	return false;
}

unsigned long
MySqlRecord::last_id()
{
	return m_lastID;
}

MySqlRecord::~MySqlRecord()
{
}

