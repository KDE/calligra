/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

// ** bits of SQLiteConnection related to table altering **

#include "sqliteconnection.h"
#include <kexidb/utils.h>

#include <kstaticdeleter.h>

#include <qmap.h>

using namespace KexiDB;

enum SQLiteTypeAffinity { //as defined here: 2.1 Determination Of Column Affinity (http://sqlite.org/datatype3.html)
	NoAffinity = 0, IntAffinity = 1, TextAffinity = 2, BLOBAffinity = 3
};

//! helper for affinityForType()
static KStaticDeleter< QMap<int,int> > KexiDB_SQLite_affinityForType_deleter;
QMap<int,int> *KexiDB_SQLite_affinityForType = 0;

//! \return SQLite type affinity for \a type
//! See doc/dev/alter_table_type_conversions.ods, page 2 for more info
static SQLiteTypeAffinity affinityForType(Field::Type type)
{
	if (!KexiDB_SQLite_affinityForType) {
		KexiDB_SQLite_affinityForType_deleter.setObject( KexiDB_SQLite_affinityForType, new QMap<int,int>() );
		KexiDB_SQLite_affinityForType->insert(Field::Byte, IntAffinity);
		KexiDB_SQLite_affinityForType->insert(Field::ShortInteger, IntAffinity);
		KexiDB_SQLite_affinityForType->insert(Field::Integer, IntAffinity);
		KexiDB_SQLite_affinityForType->insert(Field::BigInteger, IntAffinity);
		KexiDB_SQLite_affinityForType->insert(Field::Boolean, IntAffinity);
		KexiDB_SQLite_affinityForType->insert(Field::Date, TextAffinity);
		KexiDB_SQLite_affinityForType->insert(Field::DateTime, TextAffinity);
		KexiDB_SQLite_affinityForType->insert(Field::Time, TextAffinity);
		KexiDB_SQLite_affinityForType->insert(Field::Float, IntAffinity);
		KexiDB_SQLite_affinityForType->insert(Field::Double, IntAffinity);
		KexiDB_SQLite_affinityForType->insert(Field::Text, TextAffinity);
		KexiDB_SQLite_affinityForType->insert(Field::LongText, TextAffinity);
		KexiDB_SQLite_affinityForType->insert(Field::BLOB, BLOBAffinity);
	}
	return static_cast<SQLiteTypeAffinity>((*KexiDB_SQLite_affinityForType)[(int)type]);
}

tristate SQLiteConnection::drv_changeFieldProperty(TableSchema &table, Field& field, 
	const QString& propertyName, const QVariant& value)
{
/*	if (propertyName=="name") {
		
	}*/
	if (propertyName=="type") {
		bool ok;
		Field::Type type = KexiDB::intToFieldType( value.toUInt(&ok) );
		if (!ok || Field::InvalidType == type) {
			//! @todo msg
			return false;
		}
		return changeFieldType(table, field, type);
	}
	// not found
	return cancelled;
}

/*! 
 From http://sqlite.org/datatype3.html :
 Version 3 enhances provides the ability to store integer and real numbers in a more compact 
 format and the capability to store BLOB data.

 Each value stored in an SQLite database (or manipulated by the database engine) has one 
 of the following storage classes:
 * NULL. The value is a NULL value.
 * INTEGER. The value is a signed integer, stored in 1, 2, 3, 4, 6, or 8 bytes depending 
    on the magnitude of the value.
 * REAL. The value is a floating point value, stored as an 8-byte IEEE floating point number.
 * TEXT. The value is a text string, stored using the database encoding (UTF-8, UTF-16BE or UTF-16-LE).
 * BLOB. The value is a blob of data, stored exactly as it was input.

 Column Affinity
 In SQLite version 3, the type of a value is associated with the value itself, 
 not with the column or variable in which the value is stored.
.The type affinity of a column is the recommended type for data stored in that column.

 See alter_table_type_conversions.ods for details.
*/
tristate SQLiteConnection::changeFieldType(TableSchema &table, Field& field, 
	Field::Type type)
{
	Q_UNUSED(table);
	const Field::Type oldType = field.type();
	const SQLiteTypeAffinity oldAffinity = affinityForType(oldType);
	const SQLiteTypeAffinity newAffinity = affinityForType(type);
	if (oldAffinity!=newAffinity) {
		//type affinity will be changed
	}

	return cancelled;
}
