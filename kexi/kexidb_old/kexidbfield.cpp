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

#include "kexidbfield.h"

KexiDBField::KexiDBField()
{
	m_table = "";
	m_name = "";
	m_constraints = KexiDBField::CCNone;
	m_reference = "";
	m_sqlType = KexiDBField::SQLInvalid;
	m_length = 0;
	m_precision = 0;
	m_unsigned = false;
	m_binary = false;
	m_defaultValue = QVariant("");
}


KexiDBField::KexiDBField(const QString& t)
{
	m_table = t;
	m_name = "";
	m_constraints = KexiDBField::CCNone;
	m_reference = "";
	m_sqlType = KexiDBField::SQLInvalid;
	m_length = 0;
	m_precision = 0;
	m_unsigned = false;
	m_binary = false;
	m_defaultValue = QVariant("");
}

KexiDBField::KexiDBField(const QString& name, enum KexiDBField::ColumnType ctype,
	int cconst,int length, int precision, bool isUnsigned,
	bool isBinary, QVariant defaultValue):
	m_name(name),m_constraints(cconst),m_sqlType(ctype),m_length(length),
	m_precision(precision),m_unsigned(isUnsigned),m_binary(isBinary),
	m_defaultValue(defaultValue)
{
}

QVariant::Type
KexiDBField::sql2qt(KexiDBField::ColumnType sqltype)
{
	switch(sqltype)
	{
		case SQLLongVarchar:
		case SQLVarchar:
			return QVariant::String;
		case SQLInteger:
		case SQLSmallInt:
		case SQLTinyInt:
		case SQLNumeric:
			return QVariant::Int;
		case SQLDouble:
		case SQLBigInt:
		case SQLDecimal:
		case SQLFloat:
			return QVariant::Double;
		case SQLBinary:
		case SQLLongVarBinary:
		case SQLVarBinary:
			return QVariant::ByteArray;
		case SQLDate:
			return QVariant::Date;
		case SQLTime:
			return QVariant::Time;
		case SQLTimeStamp:
			return QVariant::DateTime;
		case SQLBoolean:
			return QVariant::Bool;
		case SQLInvalid:
		default:
			return QVariant::Invalid;
	}

	return QVariant::Invalid;
}

QString
KexiDBField::typeName(ColumnType sqltype)
{
	switch(sqltype)
	{
		case SQLLongVarchar:
			return "Long Varchar";
		case SQLVarchar:
			return "Varchar";
		case SQLInteger:
			return "Integer";
		case SQLSmallInt:
			return "Small Integer";
		case SQLTinyInt:
			return "Tiny Integer";
		case SQLNumeric:
			return "Numeric";
		case SQLDouble:
			return "Double";
		case SQLBigInt:
			return "Big Integer";
		case SQLDecimal:
			return "Decimal";
		case SQLFloat:
			return "Float";
		case SQLBinary:
			return "Binary";
		case SQLLongVarBinary:
			return "Long Var Binary";
		case SQLVarBinary:
			return "Var Binary";
		case SQLDate:
			return "Date";
		case SQLTime:
			return "Time";
		case SQLTimeStamp:
			return "Time Stamp";
		case SQLBoolean:
			return "Bool";
		case SQLInterval:
			return "Interval";
		case SQLInvalid:
		case SQLLastType:
			return QString::null;
	}

	return QString::null;
}

QString
KexiDBField::name() const
{
	return m_name;
}

QString
KexiDBField::table() const
{
	return m_table;
}

QVariant::Type
KexiDBField::type() const
{
	return sql2qt(sqlType());
}

KexiDBField::ColumnType
KexiDBField::sqlType() const
{
	return m_sqlType;
}

QVariant
KexiDBField::defaultValue() const
{
	return m_defaultValue;
}

int
KexiDBField::length() const
{
	return m_length;
}

int KexiDBField::constraints() const
{
	return m_constraints;
}

QString
KexiDBField::references() const
{
	return m_reference;
}

int
KexiDBField::precision() const
{
	return m_precision;
}

bool
KexiDBField::unsignedType() const
{
	return m_unsigned;
}

bool
KexiDBField::binary() const
{
	return m_binary;
}

void
KexiDBField::setTable(const QString& t)
{
	m_table = t;
}

void
KexiDBField::setName(const QString& n)
{
	m_name = n;
}

void
KexiDBField::setColumnType(ColumnType t)
{
	m_sqlType = t;
}

void
KexiDBField::setConstraints(int c)
{
	m_constraints = c;
}

void
KexiDBField::setLength(int l)
{
	m_length = l;
}

void
KexiDBField::setPrecision(int p)
{
	m_precision = p;
}

void
KexiDBField::setUnsigned(bool u)
{
	m_unsigned = u;
}

void
KexiDBField::setBinary(bool b)
{
	m_binary = b;
}

void
KexiDBField::setDefaultValue(const QVariant& d)
{
	m_defaultValue = d;
}

void
KexiDBField::setAutoIncrement(bool a)
{
	if(a)
	{
		m_constraints = static_cast<KexiDBField::ColumnConstraints>(m_constraints | KexiDBField::CCAutoInc);
	}
	else if(!a && (m_constraints & KexiDBField::CCAutoInc))
	{
		m_constraints = static_cast<KexiDBField::ColumnConstraints>(m_constraints ^ KexiDBField::CCAutoInc);
	}
}

void
KexiDBField::setPrimaryKey(bool p)
{
	if(p)
	{
		m_constraints = static_cast<KexiDBField::ColumnConstraints>(m_constraints | KexiDBField::CCPrimaryKey);
	}
	else if(!p && (m_constraints & KexiDBField::CCPrimaryKey))
	{
		m_constraints = static_cast<KexiDBField::ColumnConstraints>(m_constraints ^ KexiDBField::CCPrimaryKey);
	}
}

void
KexiDBField::setUniqueKey(bool u)
{
	if(u)
	{
		m_constraints = static_cast<KexiDBField::ColumnConstraints>(m_constraints | KexiDBField::CCUnique);
	}
	else if(!u && (m_constraints & KexiDBField::CCUnique))
	{
		m_constraints = static_cast<KexiDBField::ColumnConstraints>(m_constraints ^ KexiDBField::CCUnique);
	}
}

void
KexiDBField::setForeignKey(bool f)
{
	if(f)
	{
		m_constraints = static_cast<KexiDBField::ColumnConstraints>(m_constraints | KexiDBField::CCForeignKey);
	}
	else if(!f && (m_constraints & KexiDBField::CCForeignKey))
	{
		m_constraints = static_cast<KexiDBField::ColumnConstraints>(m_constraints ^ KexiDBField::CCForeignKey);
	}
}

void
KexiDBField::setNotNull(bool n)
{
	if(n)
	{
		m_constraints = static_cast<KexiDBField::ColumnConstraints>(m_constraints | KexiDBField::CCNotNull);
	}
	else if(!n && (m_constraints & KexiDBField::CCNotNull))
	{
		m_constraints = static_cast<KexiDBField::ColumnConstraints>(m_constraints ^ KexiDBField::CCNotNull);
	}
}
