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

#include "../../kexiDB/kexidbfield.h"

class MySqlField : public KexiDBField
{
	public:
		MySqlField(MYSQL_FIELD *field, unsigned int column);
		~MySqlField();

		QString				name() const;
		QString				table() const;

		bool				auto_increment();

		//key section
		bool				primary_key();
		bool				unique_key();
		bool				forign_key();

		bool				not_null();

		QVariant::Type			type();
		KexiDBField::ColumnType		sqlType();
		
		QVariant		defaultValue();
		int					length();
	
		static QString sql2string(KexiDBField::ColumnType sqltype);
	protected:
		MYSQL_FIELD *m_field;
};

#endif
