/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

#ifndef MYSQLFIELD_H
#define MYSQLFIELD_H

#include <mysql/mysql.h>

#include <kexidbfield.h>

class MySqlField : public KexiDBField
{
	public:
		MySqlField(MYSQL_FIELD *field, unsigned int column);
		~MySqlField();

		QString				name() const;
		QString				table() const;
		/*!
		 *	@returns the constraints on this column or QString::null if none
		 */
		virtual ColumnConstraints		constraints();

		/*!
		 *	@returns the table.column that this field references or QString::null if !foreign_key()
		 */
		virtual QString		references();

		QVariant::Type			type();
		KexiDBField::ColumnType		sqlType();
		
		QVariant		defaultValue();
		int					length();
		int precision(); // for numeric and other fields that have both length and precision
		virtual bool unsignedType(); // if the type has the unsigned attribute
		virtual bool binary();
	
		static QString sql2string(KexiDBField::ColumnType sqltype);
	protected:
		MYSQL_FIELD *m_field;
};

#endif
