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

#include <iostream.h>

#include <klocale.h>

#include <kexidberror.h>

#include "cqldb.h"
#include "cqlrecord.h"

CqlRecord::CqlRecord(SqlHandle *handle, const QString statement)
 : KexiDBRecord()
{
	try
	{
		m_cursor = handle->declareCursor(statement.latin1());

		try
		{
			m_cursor->open();
		}
		catch(CqlException &err)
		{
			cerr << err << endl;
		}
	}
	catch(CqlException &err)
	{
		cerr << err << endl;
		throw KexiDBError(0, CqlDB::errorText(err));
	}
}

bool
CqlRecord::readOnly()
{
	return false;
}

void
CqlRecord::reset()
{
	return false;
}

bool
CqlRecord::commit(unsigned int record, bool insertBuffer=false)
{
	return false;
}

QVariant
CqlRecord::value(unsigned int field)
{
}

QVariant
CqlRecord::value(QString field)
{
}

QVariant::Type
CqlRecord::type(unsigned int field)
{
}

QVariant::Type
CqlRecord::type(QString field)
{
}

KexiDBField::ColumnType
CqlRecord::sqlType(unsigned int field)
{
}

KexiDBField::ColumnType
CqlRecord::sqlType(QString field)
{
}

KexiDBField*
CqlRecord::fieldInfo(unsigned int column)
{
}

KexiDBField*
CqlRecord::fieldInfo(QString column)
{
}

bool
CqlRecord::update(unsigned int record, unsigned int field, QVariant value)
{
}

bool
CqlRecord::update(unsigned int record, QString field, QVariant value)
{
}

bool
CqlRecord::deleteRecord(uint record)
{
}

int
CqlRecord::insert()
{
}

void
CqlRecord::gotoRecord(unsigned int record)
{
}

unsigned int
CqlRecord::fieldCount()
{
}

QString
CqlRecord::fieldName(unsigned int field)
{
}

bool
CqlRecord::next()
{
}

unsigned long
CqlRecord::last_id()
{
}

CqlRecord::~CqlRecord()
{
}

//#include "cqlrecord.moc"
