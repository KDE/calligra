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

#include <kexidbfield.h>

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

QString
MySqlField::table() const
{
	return QString::fromLatin1(m_field->table);
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
		case FIELD_TYPE_NEWDATE:
		case FIELD_TYPE_DATE:
			return KexiDBField::SQLDate;
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

QVariant 
MySqlField::defaultValue()
{
	return QVariant(m_field->def);
}

int 
MySqlField::length()
{
	return m_field->length;
}

QString
MySqlField::sql2string(KexiDBField::ColumnType sqltype)
{
	switch(sqltype)
	{
		case SQLLongVarchar:
			return "TEXT";
		case SQLVarchar:
			return "VARCHAR";
		case SQLInteger:
			return "INTEGER";
		case SQLSmallInt:
			return "SMALLINT";
		case SQLTinyInt:
			return "TINYINT";
		case SQLNumeric:
			return "NUMERIC";
		case SQLDouble:
			return "DOUBLE";
		case SQLBigInt:
			return "BIGINT";
		case SQLDecimal:
			return "DECIMAL";
		case SQLFloat:
			return "FLOAT";
		case SQLBinary:
			return "BLOB";
		case SQLLongVarBinary:
			return "LONGBLOB";
		case SQLVarBinary:
			return "BLOB";
		case SQLDate:
			return "DATE";
		case SQLTime:
			return "TIME";
		case SQLTimeStamp:
			return "TIMESTAMP";
		case SQLBoolean:
			return "BOOL";
		case SQLInterval:
			return "ENUM";
		case SQLInvalid:
			return QString::null;
	}

	return QString::null;
}

KexiDBField::ColumnConstraints
MySqlField::constraints()
{
	KexiDBField::ColumnConstraints constraints = KexiDBField::None;
	
	if(m_field->flags & NOT_NULL_FLAG)
	{
		constraints = static_cast<KexiDBField::ColumnConstraints>(constraints + KexiDBField::NotNull);
	}
	
	if(m_field->flags & UNIQUE_KEY_FLAG)
	{
		constraints = static_cast<KexiDBField::ColumnConstraints>(constraints + KexiDBField::Unique);
	}
	
	if(m_field->flags & PRI_KEY_FLAG)
	{
		constraints = static_cast<KexiDBField::ColumnConstraints>(constraints + KexiDBField::PrimaryKey);
	}
	
	if(m_field->flags & AUTO_INCREMENT_FLAG)
	{
		constraints = static_cast<KexiDBField::ColumnConstraints>(constraints + KexiDBField::AutoInc);
	}
	
	return constraints;
}

QString
MySqlField::references()
{
	return QString::null;
}

int
MySqlField::precision()
{
	return m_field->decimals;
}

bool
MySqlField::unsignedType()
{
	return m_field->flags & UNSIGNED_FLAG;
}

bool
MySqlField::binary()
{
	return m_field->flags & BINARY_FLAG;
}

MySqlField::~MySqlField()
{
}
