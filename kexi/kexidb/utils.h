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

namespace KexiDB
{
	//! for convenience
	KEXI_DB_EXPORT bool deleteRow(Connection &conn, TableSchema *table, const QString &keyname, const QString &keyval);
	KEXI_DB_EXPORT bool deleteRow(Connection &conn, TableSchema *table, const QString &keyname, int keyval);
	KEXI_DB_EXPORT bool replaceRow(Connection &conn, TableSchema *table, const QString &keyname, const QString &keyval, const QString &valname, QVariant val, int ftype);

	typedef QValueList<uint> TypeGroupList;

	/*! \return list of types for a given \a typeGroup. */
	KEXI_DB_EXPORT const TypeGroupList typesForGroup(KexiDB::Field::TypeGroup typeGroup);

	/*! \return list of i18n'd type names for a given \a typeGroup. */
	KEXI_DB_EXPORT QStringList typeNamesForGroup(KexiDB::Field::TypeGroup typeGroup);

	/*! \return list of (not-i18n'd) type names for a given \a typeGroup. */
	KEXI_DB_EXPORT QStringList typeStringsForGroup(KexiDB::Field::TypeGroup typeGroup);

	/*! \return true if \a v represents an empty (but not null) value.
	 Values of some types (as for strings) can be both empty and not null. */
	inline bool isEmptyValue(KexiDB::Field *f, const QVariant &v) {
		if (f->isTextType() && v.toString().isEmpty() && !v.toString().isNull())
			return true;
		return v.isNull();
	}

	/*! Sets \a msg to an error message retrieved from object \a obj, and \a details
	 to details of this error (server message and result number).
	 Does nothing if \a obj is null or no error occured.
	 \a msg and \a details strings are not overwritten but strings are appended.
	 */
	KEXI_DB_EXPORT void getHTMLErrorMesage(Object* obj, QString& msg, QString &details);

	/*! This methods works like above, but appends both a message and a description
	 to \a msg. */
	KEXI_DB_EXPORT void getHTMLErrorMesage(Object* obj, QString& msg);
}

#endif

