/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Joseph Wenninger <jowenn@kde.org>

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

#ifndef KEXIDBFIELD_H
#define KEXIDBFIELD_H

#include <qvariant.h>
#include <qstring.h>

class KEXIDB_EXPORT KexiDBField
{
	public:
		enum ColumnType
		{
			SQLInvalid = 0,
			SQLBigInt,
			SQLBinary,
			SQLBoolean,
			SQLDate,
			SQLDecimal,
			SQLDouble,
			SQLFloat,
			SQLInteger,
			SQLInterval,
			SQLLongVarBinary,
			SQLLongVarchar,
			SQLNumeric,
			SQLSmallInt,
			SQLTime,
			SQLTimeStamp,
			SQLTinyInt,
			SQLVarBinary,
			SQLVarchar,
			SQLLastType	// This line should be at the end of the enum!
		};

		enum ColumnConstraints
		{
			CCNone = 0,
			CCAutoInc = 1,
			CCUnique = 2,
			CCPrimaryKey = 4,
			CCForeignKey = 8,
			CCNotNull = 16
 		};

		KexiDBField(const QString& t);
		KexiDBField();

		KexiDBField(const QString& name, enum ColumnType ctype,
		int cconst=CCNone,int length=0, int precision=0,
		bool isUnsigned=false,bool isBinary=false, QVariant defaultValue=QVariant(""));


		virtual ~KexiDBField() {}

		static QVariant::Type sql2qt(ColumnType sqltype);
		static QString typeName(ColumnType sqltype);

		virtual QString		name() const;
		virtual QString		table() const;

		/*!
		 *	@returns true if the field is autoincrement (e.g. integer/numeric)
		 */
		bool		auto_increment() const { return constraints() & CCAutoInc; }
		/*!
		 *	@returns true if the field is a primary key
		 */
		bool		primary_key() const { return constraints() & CCPrimaryKey; }
		/*!
		 *	@returns true if the field is an unique key
		 */
		bool		unique_key() const { return constraints() & CCUnique; }
		/*!
		 *	@returns true if the field is a foreign key
		 */
		bool		foreign_key() const { return constraints() & CCForeignKey; }
		/*!
		 *	@returns true if the field is not allowed to be null
		 */
		bool		not_null() const { return constraints() & CCNotNull; }
		/*!
		 *	@returns the constraints on this column
		 */
		virtual int		constraints() const;

		/*!
		 *	@returns the table.column that this field references or QString::null if !foreign_key()
		 */
		virtual QString		references() const;

		virtual QVariant::Type	type() const;
		virtual ColumnType	sqlType() const;
		virtual QVariant defaultValue() const;
		virtual int length() const;
		virtual int precision() const; // for numeric and other fields that have both length and precision
		virtual bool unsignedType() const; // if the type has the unsigned attribute
		virtual bool binary() const;

		void setTable(const QString& t);
		void setName(const QString& n);
		void setColumnType(ColumnType t);
		void setConstraints(int c);
		void setLength(int l);
		void setPrecision(int p);
		void setUnsigned(bool u);
		void setBinary(bool b);
		void setDefaultValue(const QVariant& d);
		void setAutoIncrement(bool a);
		void setPrimaryKey(bool p);
		void setUniqueKey(bool u);
		void setForeignKey(bool f);
		void setNotNull(bool n);

	private:
		QString m_table;
		QString m_name;
		QString m_reference;
		ColumnType m_sqlType;
		int m_constraints;
		int m_length;
		int m_precision;
		bool m_unsigned;
		bool m_binary;
		QVariant m_defaultValue;
};

#endif
