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

#include "mysqlrecord.h"

MySqlRecord::MySqlRecord(MySqlResult *result, unsigned int record, MySqlRecord *parent)
 : KexiDBRecord(result, record)
{
	m_parent = parent;
}

void
MySqlRecord::reset()
{
}

bool
MySqlRecord::commit(bool)
{
	return false;
}

QVariant
MySqlRecord::value(unsigned int)
{
	return QVariant();
}

QVariant
MySqlRecord::value(QString)
{
	return QVariant();
}

QVariant::Type
MySqlRecord::type(unsigned int)
{
	return QVariant::Invalid;
}

QVariant::Type
MySqlRecord::type(QString)
{
	return QVariant::Invalid;
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

bool
MySqlRecord::deleteRecord()
{
	return false;
}

MySqlRecord*
MySqlRecord::gotoRecord(unsigned int)
{
	return 0;
}

MySqlRecord*
MySqlRecord::insert()
{
	return 0;
}

MySqlRecord*
MySqlRecord::operator++()
{
	return 0;
}

MySqlRecord*
MySqlRecord::operator--()
{
	return 0;
}

MySqlRecord::~MySqlRecord()
{
}

