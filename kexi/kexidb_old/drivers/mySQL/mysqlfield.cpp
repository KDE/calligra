/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Joseph Wenninger <jowenn@kde.org>

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

#include "mysqlfield.h"

#include "../../kexiDB/kexidbfield.h"

MySqlField::MySqlField(MYSQL_FIELD *field, unsigned int column)
 : KexiDBField(QString::fromLatin1(field->table), column)
{
	m_field = field;
}

QString
MySqlField::name() const
{
	return QString::fromLatin1(m_field->name);
}

bool
MySqlField::primary_key()
{
	if(m_field->flags & PRI_KEY_FLAG)
		return true;
	else
		return false;

}

bool
MySqlField::unique_key()
{
	if(m_field->flags & PRI_KEY_FLAG)
		return true;
	else
		return false;
}

bool
MySqlField::forign_key()
{
	return false; //mySQL doen't know forign keys
}

QVariant::Type
MySqlField::type()
{
	return sql2qt(sqlType());
}

KexiDBField::ColumnType
MySqlField::sqlType()
{
	switch(m_field->type)
	{
		case FIELD_TYPE_NULL:
			return KexiDBField::SQLInvalid;
		case FIELD_TYPE_INT24:
		case FIELD_TYPE_LONGLONG:
			return KexiDBField::SQLBigInt;
			return KexiDBField::SQLBinary;
			return KexiDBField::SQLBit;
		case FIELD_TYPE_NEWDATE:
		case FIELD_TYPE_DATE:
			return KexiDBField::SQlDate;
		case FIELD_TYPE_DECIMAL:
			return KexiDBField::SQLDecimal;
		case FIELD_TYPE_DOUBLE:
			return KexiDBField::SQLDouble;
		case FIELD_TYPE_FLOAT:
			return KexiDBField::SQLFloat;
		case FIELD_TYPE_LONG:
		case FIELD_TYPE_YEAR:
			return KexiDBField::SQLInteger;
		case FIELD_TYPE_SHORT:
			return KexiDBField::SQLSmallInt;
		case FIELD_TYPE_TIME:
			return KexiDBField::SQLTime;
		case FIELD_TYPE_DATETIME:
		case FIELD_TYPE_TIMESTAMP:
			return KexiDBField::SQLTimeStamp;
		case FIELD_TYPE_TINY:
			return KexiDBField::SQLTinyInt;
		case FIELD_TYPE_TINY_BLOB:
		case FIELD_TYPE_MEDIUM_BLOB:
		case FIELD_TYPE_LONG_BLOB:
		case FIELD_TYPE_BLOB:
			return KexiDBField::SQLVarBinary;
		case FIELD_TYPE_VAR_STRING:
		case FIELD_TYPE_STRING:
		case FIELD_TYPE_SET:
		case FIELD_TYPE_ENUM:
			return KexiDBField::SQLVarchar;

	}
	return KexiDBField::SQLInvalid;
}

MySqlField::~MySqlField()
{
}
