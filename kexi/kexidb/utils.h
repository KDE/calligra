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

	/*! \return list of type i18n'd names for a given \a typeGroup. */
	KEXI_DB_EXPORT QStringList typeNamesForGroup(KexiDB::Field::TypeGroup typeGroup);
}

#endif

