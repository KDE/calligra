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

#ifndef KEXIDBFIELD_H
#define KEXIDBFIELD_H

#include <qvariant.h>
#include <qstring.h>

class KexiDBField
{

	public:
		enum ColumnType
		{
			SQLInvalid = 0,
			SQLBigInt,
			SQLBinary,
			SQLBit,
			SQLBoolean,
			SQLDate,
			SQLDecimal,
			SQLDouble,
			SQLFloat,
			SQLInteger,
			SQLInterval,
			SQLLongVarBinary,
			SQLLongVarChar,
			SQLNumeric,
			SQLSmallInt,
			SQLTime,
			SQLTimeStamp,
			SQLTinyInt,
			SQLVarBinary,
			SQLVarchar
		};

		enum ColumnConstraints
		{
			None = 0,
			AutoInc = 1,            
			Unique = 2,
			PrimaryKey = 4,
			ForeignKey = 8,
			NotNull = 16
 		};

		KexiDBField(QString table, unsigned int field);
		virtual ~KexiDBField();

		static QVariant::Type sql2qt(ColumnType sqltype);
		static QString typeName(ColumnType sqltype);

		virtual QString		name() const;
		virtual QString		table() const;

		/*!
		 *	@returns true if the field is autoincrement (e.g. integer/numeric)
		 */
		bool		auto_increment() { return constraints() & AutoInc; }
		/*!
		 *	@returns true if the field is a primary key
		 */
		bool		primary_key() { return constraints() & PrimaryKey; }
		/*!
		 *	@returns true if the field is an unique key
		 */
		bool		unique_key() { return constraints() & Unique; }
		/*!
		 *	@returns true if the field is a foreign key
		 */
		bool		foreign_key() { return constraints() & ForeignKey; }
		/*!
		 *	@returns true if the field is not allowed to be null
		 */
		bool		not_null() { return constraints() & NotNull; }
		/*!
		 *	@returns the constraints on this column
		 */
		virtual ColumnConstraints		constraints();

		/*!
		 *	@returns the table.column that this field references or QString::null if !foreign_key()
		 */
		virtual QString		references();

		virtual QVariant::Type	type();
		virtual ColumnType	sqlType();
		virtual QVariant defaultValue();
		virtual int length();
		virtual int precision(); // for numeric and other fields that have both length and precision
		virtual bool unsignedType(); // if the type has the unsigned attribute
		virtual bool binary();

		static QString sql2string(KexiDBField::ColumnType sqltype);
};

#endif
