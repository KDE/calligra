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

#include "cqldb.h"
#include "cqlfield.h"

#include <CqlConstants.h>

CqlField::CqlField(ColumnMetadata *meta)
 : KexiDBField("", meta->columnNumber())
{
	m_meta = meta;
}

QString
CqlField::name() const
{
	return CqlDB::cqlString(m_meta->name());
}

QString
CqlField::table() const
{
	return QString("ask seth");
}

KexiDBField::ColumnConstraints
CqlField::constraints()
{
	return KexiDBField::None;
}

QString
CqlField::references()
{
	return QString::null;
}

QVariant::Type
CqlField::type()
{
	return sql2qt(sqlType());
}

KexiDBField::ColumnType
CqlField::sqlType()
{
	switch(m_meta->columnType().typeType())
	{
		case CqlConstants::CQL_CHAR:
			return SQLVarchar;

		default:
			return SQLVarchar;
	}
}

QVariant
CqlField::defaultValue()
{
	return QVariant("");
}

int
CqlField::length()
{
	return 0;
}

int
CqlField::precision()
{
	return 0;
}

bool
CqlField::usignedType()
{
	return false;
}

bool
CqlField::binary()
{
	return false;
}

CqlField::~CqlField()
{
}
