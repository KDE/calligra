/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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


#ifndef KEXIDB_UTILS_H
#define KEXIDB_UTILS_H

#include <qvaluelist.h>
#include <qvariant.h>

#include <kexidb/connection.h>
#include <kexidb/driver.h>

namespace KexiDB
{
	//! for convenience
	inline KEXI_DB_EXPORT bool deleteRow(Connection &conn, TableSchema *table, 
		const QString &keyname, const QString &keyval)
	{
		return table!=0 && conn.executeSQL("DELETE FROM " + table->name() + " WHERE " 
			+ keyname + "=" + conn.driver()->valueToSQL( Field::Text, QVariant(keyval) ));
	}

	inline KEXI_DB_EXPORT bool deleteRow(Connection &conn, const QString &tableName, 
		const QString &keyname, const QString &keyval)
	{
		return conn.executeSQL("DELETE FROM " + tableName + " WHERE " 
			+ keyname + "=" + conn.driver()->valueToSQL( Field::Text, QVariant(keyval) ));
	}

	inline KEXI_DB_EXPORT bool deleteRow(Connection &conn, TableSchema *table, 
		const QString &keyname, int keyval)
	{
		return table!=0 && conn.executeSQL("DELETE FROM " + table->name() + " WHERE " 
			+ keyname + "=" + conn.driver()->valueToSQL( Field::Integer, QVariant(keyval) ));
	}

	inline KEXI_DB_EXPORT bool deleteRow(Connection &conn, const QString &tableName, 
		const QString &keyname, int keyval)
	{
		return conn.executeSQL("DELETE FROM " + tableName + " WHERE " 
			+ keyname + "=" + conn.driver()->valueToSQL( Field::Integer, QVariant(keyval) ));
	}

	/*! Delete row with two generic criterias. */
	inline KEXI_DB_EXPORT bool deleteRow(Connection &conn, const QString &tableName, 
		const QString &keyname1, Field::Type keytype1, const QVariant& keyval1, 
		const QString &keyname2, Field::Type keytype2, const QVariant& keyval2)
	{
		return conn.executeSQL("DELETE FROM " + tableName + " WHERE " 
			+ keyname1 + "=" + conn.driver()->valueToSQL( keytype1, keyval1 )
			+ " AND " + keyname2 + "=" + conn.driver()->valueToSQL( keytype2, keyval2 ));
	}

	inline KEXI_DB_EXPORT bool replaceRow(Connection &conn, TableSchema *table, 
		const QString &keyname, const QString &keyval, const QString &valname, QVariant val, int ftype)
	{
		if (!table || !KexiDB::deleteRow(conn, table, keyname, keyval))
			return false;
		return conn.executeSQL("INSERT INTO " + table->name() + " (" + keyname + "," + valname + ") VALUES (" + conn.driver()->valueToSQL( Field::Text, QVariant(keyval) ) + "," + conn.driver()->valueToSQL( ftype, val) + ")");
	}

	typedef QValueList<uint> TypeGroupList;

	/*! \return list of types for type group \a typeGroup. */
	KEXI_DB_EXPORT const TypeGroupList typesForGroup(KexiDB::Field::TypeGroup typeGroup);

	/*! \return list of i18n'd type names for type group \a typeGroup. */
	KEXI_DB_EXPORT QStringList typeNamesForGroup(KexiDB::Field::TypeGroup typeGroup);

	/*! \return list of (not-i18n'd) type names for type group \a typeGroup. */
	KEXI_DB_EXPORT QStringList typeStringsForGroup(KexiDB::Field::TypeGroup typeGroup);

	/*! \return default field type for type group \a typeGroup,
	 for example, Field::Integer for Field::IntegerGroup.
	 It is used e.g. in KexiAlterTableDialog, to properly fill 
	 'type' property when user selects type group for a field. */
	KEXI_DB_EXPORT KexiDB::Field::Type defaultTypeForGroup(KexiDB::Field::TypeGroup typeGroup);

	/*! \return true if \a v represents an empty (but not null) value.
	 Values of some types (as for strings) can be both empty and not null. */
	inline bool isEmptyValue(KexiDB::Field *f, const QVariant &v) {
		if (f->hasEmptyProperty() && v.toString().isEmpty() && !v.toString().isNull())
			return true;
		return v.isNull();
	}

	/*! Sets \a msg to an error message retrieved from object \a obj, and \a details
	 to details of this error (server message and result number).
	 Does nothing if \a obj is null or no error occured.
	 \a msg and \a details strings are not overwritten.
	 If \a msg is not empty, \a obj's error message is appended to \a details.
	 */
	KEXI_DB_EXPORT void getHTMLErrorMesage(Object* obj, QString& msg, QString &details);

	/*! This methods works like above, but appends both a message and a description
	 to \a msg. */
	KEXI_DB_EXPORT void getHTMLErrorMesage(Object* obj, QString& msg);

	/*! This methods works like above, but works on \a result's  members instead. */
	KEXI_DB_EXPORT void getHTMLErrorMesage(Object* obj, ResultInfo *result);

	/*! Function useful for building WHERE parts of sql statements.
	Constructs an sql string like "fielname = value" for specific \a drv driver,
	 field type \a t, \a fieldName and \a value. If \a value is null, "fieldname is NULL" 
	 string is returned. */
	inline KEXI_DB_EXPORT QString sqlWhere(KexiDB::Driver *drv, KexiDB::Field::Type t, 
		const QString fieldName, const QVariant value)
	{
		if (value.isNull())
			return fieldName + " is NULL";
		return fieldName + "=" + drv->valueToSQL( t, value );
	}

	/*! \return identifier for object \a objName of type \a objType
	 or 0 if such object does not exist. */
	KEXI_DB_EXPORT int idForObjectName( Connection &conn, const QString& objName, int objType );
}

#endif

