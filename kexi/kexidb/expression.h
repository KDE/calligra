/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIDB_EXPRESSION_H
#define KEXIDB_EXPRESSION_H

#include <kexidb/field.h>

namespace KexiDB {

class Connection;
class ConnectionData;
class ConnectionInternal;
class DriverManager;
class DriverBehaviour;
class DriverPrivate;
class ExpressionPrivate;

/*! This class cantains information about any expression that can be used
 in sql queries.
*/
class KEXI_DB_EXPORT Expression
{
	public:
		Expression();
		virtual ~Expression();

		/*! \return type of this expression (selected from Field::Type).
		 Type information is just reused from field's type.
		 If it is not valid expression (no field is assigned), Field::InvalidType is returned. */
		int type();

		//! owner of this expression: the filed that uses expression
		KexiDB::Field* field();
	protected:
		ExpressionPrivate *d; //unused

		Field *m_field; //owner

		friend class Field;
};

} //namespace KexiDB

#endif

