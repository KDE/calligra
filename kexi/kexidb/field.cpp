/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#include "field.h"

#include <kexidb/table.h>
#include <kexidb/connection.h>
#include <kexidb/driver.h>

#include <qdatetime.h>

using namespace KexiDB;

Field::Field()
{
	m_table = 0;
	m_name = "";
	setConstraints(NoConstraints);
	m_reference = "";
	m_type = InvalidType;
	m_length = 0;
	m_precision = 0;
	m_options = NoOptions;
//	m_unsigned = false;
//	m_binary = false;
	m_defaultValue = QVariant(QString::null);
}


Field::Field(Table *table)
{
	m_table = table;
	m_name = "";
	setConstraints(NoConstraints);
	m_reference = "";
	m_type = InvalidType;
	m_length = 0;
	m_precision = 0;
	m_options = NoOptions;
//	m_unsigned = false;
//	m_binary = false;
	m_defaultValue = QVariant(QString::null);
}

Field::Field(const QString& name, enum Type ctype,
	int cconst,int length, int precision, int options,
	QVariant defaultValue)
	: m_table(0)
	,m_name(name)
	,m_type(ctype)
	,m_options(options)
	,m_length(length)
	,m_precision(precision)
	,m_defaultValue(defaultValue)
{
	setConstraints(cconst);
	if (m_length==0) {//0 means default length:
		if (m_type==Field::Text)
			m_length = defaultTextLength();
	}
}

QVariant::Type
Field::variantType(Type type)
{
	switch(type)
	{
		case Byte:
		case ShortInteger:
		case Integer:
		case BigInteger:
//		case AutoIncrement:
			return QVariant::Int;
		case Boolean:
			return QVariant::Bool;
		case Date:
			return QVariant::Date;
		case DateTime:
			return QVariant::DateTime;
		case Time:
			return QVariant::Time;
		case Float:
		case Double:
			return QVariant::Double;
		case Text:
		case LongText:
			return QVariant::String;
		case BLOB:
			return QVariant::ByteArray;
		default:
			return QVariant::Invalid;
	}

	return QVariant::Invalid;
}


Field::~Field() {
}


// ** NOTE: we'd like use here I18N_NOOP() but this depends on kde libs!...
QString
Field::typeName(Type type)
{
	switch(type)
	{
/*		case SQLLongVarchar:
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
			return "Interval";*/
		case Byte:
			return "Byte";
		case ShortInteger:
			return "Short integer number";
		case Integer:
			return "Integer number";
		case BigInteger:
			return "Big integer number";
//		case AutoIncrement:
//			return "Auto increment number";
		case Boolean:
			return "True/False value";
		case Date:
			return "Date";
		case DateTime:
			return "Date/Time";
		case Time:
			return "Time";
		case Float:
			return "Single precision number";
		case Double:
			return "Double precision number";
		case Text:
			return "Text";
		case LongText:
			return "Long text";
		case BLOB:
			return "Object";
		default:
			;
	}
	return QString::null;
}

QString
Field::name() const
{
	return m_name;
}

Table*
Field::table() const
{
	return m_table;
}

QVariant::Type
Field::variantType() const
{
	return variantType(m_type);
}

QString
Field::typeName() const
{
	return typeName(m_type);
}

Field::Type
Field::type() const
{
	return m_type;
}

QVariant
Field::defaultValue() const
{
	return m_defaultValue;
}

int
Field::length() const
{
	return m_length;
}

int Field::constraints() const
{
	return m_constraints;
}

QString
Field::references() const
{
	return m_reference;
}

int
Field::precision() const
{
	return m_precision;
}

bool
Field::isUnsigned() const
{
	return m_options & Unsigned;
}

/*
bool
Field::binary() const
{
	return m_binary;
}*/

void
Field::setTable(Table *table)
{
	m_table = table;
}

void
Field::setName(const QString& n)
{
	m_name = n;
}

void
Field::setType(Type t)
{
	m_type = t;
}

void
Field::setConstraints(int c)
{
	m_constraints = c;
	//pkey must be unique notnull
	if (m_constraints & Field::PrimaryKey ) {
		m_constraints |= Unique;
		m_constraints |= NotNull;
	}
}

void
Field::setLength(int l)
{
	m_length = l;
}

void
Field::setPrecision(int p)
{
	m_precision = p;
}

void
Field::setUnsigned(bool u)
{
	m_options |= Unsigned;
	m_options ^= (!u * Unsigned);
}

void
Field::setDefaultValue(const QVariant& def)
{
	m_defaultValue = def;
}

bool
Field::setDefaultValue(const QCString& def)
{
	if (def.isNull()) {
		m_defaultValue = QVariant();
		return true;
	}
	
	bool ok;
	switch(m_type)
	{
		case Byte: {
			unsigned int v = def.toUInt(&ok);
			if (!ok || v > 255)
				m_defaultValue = QVariant();
			else
				m_defaultValue = QVariant(v);
			break;
		}case ShortInteger: {
			int v = def.toInt(&ok);
			if (!ok || (!(m_options & Unsigned) && (v < -32768 || v > 32768)) || ((m_options & Unsigned) && (v < 0 || v > 65535)))
				m_defaultValue = QVariant();
			else
				m_defaultValue = QVariant(v);
			break;
		}case Integer: {//4 bytes
			long v = def.toLong(&ok);
			if (!ok || (!(m_options & Unsigned) && (-v > 0x080000000 || v > (0x080000000-1))) || ((m_options & Unsigned) && (v < 0 || v > 0x100000000)))
				m_defaultValue = QVariant();
			else
				m_defaultValue = QVariant(v);
			break;
		}case BigInteger: {//8 bytes
			long v = def.toLong(&ok);
//TODO: 2-part decoding
			if (!ok || (!(m_options & Unsigned) && (-v > 0x080000000 || v > (0x080000000-1))) || ((m_options & Unsigned) && (v < 0 || v > 0x100000000)))
				m_defaultValue = QVariant();
			else
				m_defaultValue = QVariant(v);
			break;
		}case Boolean: {
			unsigned short v = def.toUShort(&ok);
			if (!ok || v > 1)
				m_defaultValue = QVariant();
			else
				m_defaultValue = QVariant((bool)v);
			break;
		}case Date: {//YYYY-MM-DD
			QDate date = QDate::fromString( def, Qt::ISODate );
			if (!date.isValid())
				m_defaultValue = QVariant();
			else
				m_defaultValue = QVariant(date);
			break;
		}case DateTime: {//YYYY-MM-DDTHH:MM:SS
			QDateTime dt = QDateTime::fromString( def, Qt::ISODate );
			if (!dt.isValid())
				m_defaultValue = QVariant();
			else
				m_defaultValue = QVariant(dt);
			break;
		}case Time: {//HH:MM:SS
			QTime time = QTime::fromString( def, Qt::ISODate );
			if (!time.isValid())
				m_defaultValue = QVariant();
			else
				m_defaultValue = QVariant(time);
			break;
		}case Float: {
			float v = def.toFloat(&ok);
			if (!ok || ((m_options & Unsigned) && (v < 0.0)))
				m_defaultValue = QVariant();
			else
				m_defaultValue = QVariant(v);
			break;
		}case Double: {
			double v = def.toDouble(&ok);
			if (!ok || ((m_options & Unsigned) && (v < 0.0)))
				m_defaultValue = QVariant();
			else
				m_defaultValue = QVariant(v);
			break;
		}case Text: {
			if (def.isNull() || (def.length() > 255))
				m_defaultValue = QVariant();
			else
				m_defaultValue = QVariant((QString)def);
			break;
		}case LongText: {
			if (def.isNull())
				m_defaultValue = QVariant();
			else
				m_defaultValue = QVariant((QString)def);
			break;
		}case BLOB: {
//TODO
			if (def.isNull())
				m_defaultValue = QVariant();
			else
				m_defaultValue = QVariant(def);
			break;
		}default:
			m_defaultValue = QVariant();
	}
	return m_defaultValue.isNull();
}

void
Field::setAutoIncrement(bool a)
{
	if(a)
	{
		m_constraints = static_cast<Field::Constraints>(m_constraints | Field::AutoInc);
	}
	else if(!a && (m_constraints & Field::AutoInc))
	{
		m_constraints = static_cast<Field::Constraints>(m_constraints ^ Field::AutoInc);
	}
}

void
Field::setPrimaryKey(bool p)
{
	if(p)
	{
		m_constraints = static_cast<Field::Constraints>(m_constraints | Field::PrimaryKey);
	}
	else if(!p && (m_constraints & Field::PrimaryKey))
	{
		m_constraints = static_cast<Field::Constraints>(m_constraints ^ Field::PrimaryKey);
	}
}

void
Field::setUniqueKey(bool u)
{
	if(u)
	{
		m_constraints = static_cast<Field::Constraints>(m_constraints | Field::Unique);
	}
	else if(!u && (m_constraints & Field::Unique))
	{
		m_constraints = static_cast<Field::Constraints>(m_constraints ^ Field::Unique);
	}
}

void
Field::setForeignKey(bool f)
{
	if(f)
	{
		m_constraints = static_cast<Field::Constraints>(m_constraints | Field::ForeignKey);
	}
	else if(!f && (m_constraints & Field::ForeignKey))
	{
		m_constraints = static_cast<Field::Constraints>(m_constraints ^ Field::ForeignKey);
	}
}

void
Field::setNotNull(bool n)
{
	if(n)
	{
		m_constraints = static_cast<Field::Constraints>(m_constraints | Field::NotNull);
	}
	else if(!n && (m_constraints & Field::NotNull))
	{
		m_constraints = static_cast<Field::Constraints>(m_constraints ^ Field::NotNull);
	}
}

QString Field::debugString() const
{
	KexiDB::Connection *conn = m_table ? m_table->connection() : 0;
	QString dbg = m_name + " ";
	if (m_options & Field::Unsigned)
		dbg += " UNSIGNED ";
	dbg += conn ? conn->driver()->sqlTypeName(m_type) : Driver::defaultSQLTypeName(m_type);
	if (m_length > 0)
		dbg += "(" + QString::number(m_length) + ")";
	if (m_precision > 0)
		dbg += " PRECISION(" + QString::number(m_precision) + ")";
	if (m_constraints & Field::AutoInc)
		dbg += " AUTOINC";
	if (m_constraints & Field::Unique)
		dbg += " UNIQUE";
	if (m_constraints & Field::PrimaryKey)
		dbg += " PKEY";
	if (m_constraints & Field::ForeignKey)
		dbg += " FKEY";
	if (m_constraints & Field::NotNull)
		dbg += " NOTNULL";
	return dbg;
}
