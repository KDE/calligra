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

MySqlRecord::MySqlRecord(MYSQL_RES *result, QObject *p, bool buffer, MySqlRecord *parent)
 : KexiDBRecord(), MySqlResult(result, p)
{
}

bool
MySqlRecord::findKey()
{
}

bool
MySqlRecord::readOnly()
{
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
MySqlRecord::commit(bool insertBuffer)
{
	if(!m_updateBuffer.empty())
	{
		QString query = QString("update " + m_table);
		for(UpdateBuffer::Iterator it = m_updateBuffer.begin(); it != m_updateBuffer.end(); it++)
		{
			query += QString(QString(" set ") + it.key() + "='" + it.data().toString() + "'");
		}
		query += QString(" where " + m_keyField + "=" + value(m_keyField).toString());
	}
}

QVariant
MySqlRecord::value(unsigned int column)
{
//	kdDebug() << "MySqlRecord::value(uint)" << endl;
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


bool
MySqlRecord::update(unsigned int, QVariant)
{
	return false;
}

bool
MySqlRecord::update(QString, QVariant)
{
	return false;
}

KexiDBRecord*
MySqlRecord::insert()
{
	return 0;
}

bool
MySqlRecord::deleteRecord()
{
	return false;
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
	return MySqlResult::next();
}

MySqlRecord::~MySqlRecord()
{
}

