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

KexiDBField::KexiDBField(QString, unsigned int)
{
}

QVariant::Type
KexiDBField::sql2qt(KexiDBField::ColumnType sqltype)
{
	switch(sqltype)
	{
		case SQLInvalid:
			return QVariant::Invalid;
		case SQLLongVarChar:
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
			return QVariant::CString;
		case SQlDate:
			return QVariant::Date;
		case SQLTime:
			return QVariant::Time;
		case SQLTimeStamp:
			return QVariant::DateTime;
		case SQLBit:
			return QVariant::Bool;
	}
}

QString
KexiDBField::typeName(ColumnType sqltype)
{
	switch(sqltype)
	{
		case SQLLongVarChar:
			return "Long varchar";
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
		case SQlDate:
			return "Date";
		case SQLTime:
			return "Time";
		case SQLTimeStamp:
			return "Time Stamp";
		case SQLBit:
			return "Bool";
	}
	
	return QString::null;
}

QString
KexiDBField::name() const
{
	return QString::null;
}

QString
KexiDBField::table() const
{
	return QString::null;
}

bool
KexiDBField::auto_increment()
{
	return false;
}

bool
KexiDBField::primary_key()
{
	return false;
}

bool
KexiDBField::unique_key()
{
	return false;
}

bool
KexiDBField::forign_key()
{
	return false;
}

bool
KexiDBField::not_null()
{
	return false;
}

QVariant::Type
KexiDBField::type()
{
	return QVariant::Invalid;
}

KexiDBField::ColumnType
KexiDBField::sqlType()
{
	return SQLInvalid;
}

QVariant 
KexiDBField::defaultValue()
{
	return QVariant::Invalid;
}

int 
KexiDBField::length()
{
	return 0;
}

QString
KexiDBField::sql2string(KexiDBField::ColumnType sqltype)
{
	return QString::null;
}

KexiDBField::~KexiDBField()
{
}

