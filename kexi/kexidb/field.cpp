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

#include <kexidb/field.h>

#include <kexidb/connection.h>
#include <kexidb/driver.h>
#include <kexidb/expression.h>

// we use here I18N_NOOP() but this depends on kde libs: TODO: add #ifdefs
#include <kdebug.h>
#include <klocale.h>

#include <qdatetime.h>

using namespace KexiDB;

Field::FieldTypeNames Field::m_typeNames;
Field::FieldTypeGroupNames Field::m_typeGroupNames;

Field::Field()
	:m_parent(0)
	,m_name("")
	,m_type(InvalidType)
	,m_length(0)
	,m_precision(0)
	,m_options(NoOptions)
	,m_defaultValue( QVariant(QString::null) )
	,m_order(-1)
	,m_width(0)
	,m_expr(0)
{
	setConstraints(NoConstraints);
}


Field::Field(TableSchema *tableSchema)
	:m_parent(tableSchema)
	,m_name("")
	,m_type(InvalidType)
	,m_length(0)
	,m_precision(0)
	,m_options(NoOptions)
	,m_defaultValue( QVariant(QString::null) )
	,m_order(tableSchema->fieldCount())
	,m_width(0)
	,m_expr(0)
{
	setConstraints(NoConstraints);
}

Field::Field(const QString& name, Type ctype,
 uint cconst, uint options, uint length, uint precision,
 QVariant defaultValue, const QString& caption, const QString& description,
 uint width)
	: m_parent(0)
	,m_name(name)
	,m_type(ctype)
	,m_length(length)
	,m_precision(precision)
	,m_options(options)
	,m_defaultValue(defaultValue)
	,m_order(-1)
	,m_caption(caption)
	,m_desc(description)
	,m_width(width)
	,m_expr(0)
{
	setConstraints(cconst);
	if (m_length==0) {//0 means default length:
		if (m_type==Field::Text)
			m_length = defaultTextLength();
	}
}

/*! Copy constructor. */
Field::Field(const Field& f)
{
	(*this) = f;
	if (f.m_expr) {//deep copy the expresion
		m_expr = new Expression(*f.m_expr);
		m_expr->m_field = this;
	} else
		m_expr = 0;
}

Field::~Field()
{
	delete m_expr;
}

QVariant::Type Field::variantType(uint type)
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

QString Field::typeName(uint type)
{
	return (type <= LastType) ? m_typeNames.at(type) : QString::number(type);
}

QString Field::typeString(uint type)
{
	return (type <= LastType) ? m_typeNames.at((int)LastType+1 + type) : QString("Type%1").arg(type);
}

QString Field::typeGroupName(uint typeGroup)
{
	return (typeGroup <= LastTypeGroup) ? m_typeGroupNames.at(typeGroup) : typeGroupString(typeGroup);
}

QString Field::typeGroupString(uint typeGroup)
{
	return (typeGroup <= LastTypeGroup) ? m_typeGroupNames.at((int)LastTypeGroup+1 + typeGroup) : QString("TypeGroup%1").arg(typeGroup);
}

bool Field::isIntegerType( uint type )
{
	switch (type) {
	case Field::ShortInteger:
	case Field::Integer:
	case Field::BigInteger:
		return true;
	default:;
	}
	return false;
}

bool Field::isNumericType( uint type )
{
	switch (type) {
	case Field::ShortInteger:
	case Field::Integer:
	case Field::BigInteger:
	case Field::Float:
	case Field::Double:
		return true;
	default:;
	}
	return false;
}

bool Field::isFPNumericType( uint type )
{
	return type==Field::Float || type==Field::Double;
}

bool Field::isDateTimeType(uint type)
{
	switch (type) {
	case Field::Date:
	case Field::DateTime:
	case Field::Time:
		return true;
	default:;
	}
	return false;
}

bool Field::isTextType( uint type )
{
	switch (type) {
	case Field::Text:
	case Field::LongText:
		return true;
	default:;
	}
	return false;
}

bool Field::hasEmptyProperty(uint type)
{
	return Field::isTextType(type) || type==BLOB;
}

Field::TypeGroup Field::typeGroup(uint type)
{
	if (Field::isTextType(type))
		return TextGroup;
	else if (Field::isIntegerType(type))
		return IntegerGroup;
	else if (Field::isFPNumericType(type))
		return FloatGroup;
	else if (type==Boolean)
		return BooleanGroup;
	else if (Field::isDateTimeType(type))
		return DateTimeGroup;
	else if (type==BLOB)
		return BLOBGroup;

	return InvalidGroup; //unknown
}

TableSchema*
Field::table() const
{
	return static_cast<TableSchema*>(m_parent);
}

/*QString
Field::references() const
{
	return m_reference;
}*/

/*
bool
Field::binary() const
{
	return m_binary;
}*/

void
Field::setTable(TableSchema *tableSchema)
{
	m_parent = tableSchema;
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
Field::setConstraints(uint c)
{
	m_constraints = c;
	//pkey must be unique notnull
	if (isPrimaryKey()) {
		m_constraints |= Unique;
		m_constraints |= NotNull;
		m_constraints |= NotEmpty;
	}
}

void
Field::setLength(uint l)
{
	m_length = l;
}

void
Field::setPrecision(uint p)
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
//js: FIXME			if (!ok || (!(m_options & Unsigned) && (-v > 0x080000000 || v > (0x080000000-1))) || ((m_options & Unsigned) && (v < 0 || v > 0x100000000)))
			if (!ok || (!(m_options & Unsigned) && (-v > (int)0x07FFFFFFF || v > (int)(0x080000000-1))))
				m_defaultValue = QVariant();
			else
#if (QT_VERSION >= 0x030200) //TMP
				m_defaultValue = QVariant((Q_LLONG)v);
#else
				m_defaultValue = QVariant(); //do not works
#endif
			break;
		}case BigInteger: {//8 bytes
#ifndef Q_WS_WIN
#warning fixme
#endif
/*
			Q_LLONG long v = def.toLongLong(&ok);
//TODO: 2-part decoding
			if (!ok || (!(m_options & Unsigned) && (-v > 0x080000000 || v > (0x080000000-1))))
				m_defaultValue = QVariant();
			else
				if (m_options & Unsigned)
					m_defaultValue=QVariant((Q_ULLONG) v);
				else
					m_defaultValue = QVariant((Q_LLONG)v);*/
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
	if (isAutoIncrement() != a)
		m_constraints = static_cast<Field::Constraints>(m_constraints ^ Field::AutoInc);
}

void
Field::setPrimaryKey(bool p)
{
	if(isPrimaryKey() != p)
		m_constraints = static_cast<Field::Constraints>(m_constraints ^ Field::PrimaryKey);
	if (p) {//also set implied constraints
		setUniqueKey(true);
		setNotNull(true);
		setNotEmpty(true);
	}
}

void
Field::setUniqueKey(bool u)
{
	if(isUniqueKey() != u)
		m_constraints = static_cast<Field::Constraints>(m_constraints ^ Field::Unique);
}

void
Field::setForeignKey(bool f)
{
	if (isForeignKey() != f)
		m_constraints = static_cast<Field::Constraints>(m_constraints ^ Field::ForeignKey);
}

void
Field::setNotNull(bool n)
{
	if (isNotNull() != n)
		m_constraints = static_cast<Field::Constraints>(m_constraints ^ Field::NotNull);
}

void Field::setNotEmpty(bool n)
{
	if (isNotEmpty() != n)
		m_constraints = static_cast<Field::Constraints>(m_constraints ^ Field::NotEmpty);
}


QString Field::debugString() const
{
	KexiDB::Connection *conn = table() ? table()->connection() : 0;
	QString dbg = m_name + " ";
	if (m_options & Field::Unsigned)
		dbg += " UNSIGNED ";
	dbg += (conn && conn->driver()) ? conn->driver()->sqlTypeName(m_type) : Driver::defaultSQLTypeName(m_type);
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
	if (m_constraints & Field::NotEmpty)
		dbg += " NOTEMPTY";
	return dbg;
}

void Field::setExpression(KexiDB::Expression *expr)
{
	if (m_expr==expr)
		return;
	if (m_expr) {
		delete m_expr;
	}
	m_expr = expr;
	if (m_expr) {
		m_expr->m_field = this;
	}
}

//-------------------------------------------------------
#define ADDTYPE(type, i18, str) this->at(Field::type) = i18; \
	this->at(Field::type+Field::LastType+1) = str
#define ADDGROUP(type, i18, str) this->at(Field::type) = i18; \
	this->at(Field::type+Field::LastTypeGroup+1) = str

Field::FieldTypeNames::FieldTypeNames()
 : QValueVector<QString>()
{
	resize((Field::LastType + 1)*2);

	ADDTYPE( InvalidType, I18N_NOOP("Invalid type"), "InvalidType" );
	ADDTYPE( Byte, I18N_NOOP("Byte"), "Byte" );
	ADDTYPE( ShortInteger, I18N_NOOP("Short integer number"), "ShortInteger" );
	ADDTYPE( Integer, I18N_NOOP("Integer number"), "Integer" );
	ADDTYPE( BigInteger, I18N_NOOP("Big integer number"), "BigInteger" );
	ADDTYPE( Boolean, I18N_NOOP("Yes/No value"), "Boolean" );
	ADDTYPE( Date, I18N_NOOP("Date"), "Date" );
	ADDTYPE( DateTime, I18N_NOOP("Date and time"), "DateTime" );
	ADDTYPE( Time, I18N_NOOP("Time"), "Time" );
	ADDTYPE( Float, I18N_NOOP("Single precision number"), "Float" );
	ADDTYPE( Double, I18N_NOOP("Double precision number"), "Double" );
	ADDTYPE( Text, I18N_NOOP("Text"), "Text" );
	ADDTYPE( LongText, I18N_NOOP("Long text"), "LongText" );
	ADDTYPE( BLOB, I18N_NOOP("Object"), "BLOB" );
}

//-------------------------------------------------------

Field::FieldTypeGroupNames::FieldTypeGroupNames()
 : QValueVector<QString>()
{
	resize((Field::LastTypeGroup + 1)*2);
	ADDGROUP( InvalidGroup, I18N_NOOP("Invalid group"), "InvalidGroup" );
	ADDGROUP( TextGroup, I18N_NOOP("Text"), "TextGroup" );
	ADDGROUP( IntegerGroup, I18N_NOOP("Integer number"), "IntegerGroup" );
	ADDGROUP( FloatGroup, I18N_NOOP("Floating point number"), "FloatGroup" );
	ADDGROUP( BooleanGroup, I18N_NOOP("Yes/No"), "BooleanGroup" );
	ADDGROUP( DateTimeGroup, I18N_NOOP("Date/Time"), "DateTimeGroup" );
	ADDGROUP( BLOBGroup, I18N_NOOP("Object"), "BLOBGroup" );
}

//-------------------------------------------------------

