/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

#ifndef MYSQLRESULT_H
#define MYSQLRESULT_H

#ifdef Q_WS_WIN
# include <mysql/config-win.h>
#endif
#include <mysql/mysql.h>
#ifndef _mysql_h
typedef struct st_mysql_res MYSQL_RES;
typedef char **MYSQL_ROW;		/* return data as array of strings */
typedef struct st_mysql_field MYSQL_FIELD;
#endif

#include <qmap.h>
#include <qintdict.h>
#include <qobject.h>

#include "../../kexidbfield.h"
#include "mysqldb.h"

//#define BOOL bool

typedef QMap<QString, int> FieldNames;
typedef QIntDict<KexiDBField> FieldInfo;

class KEXI_MYSQL_IFACE_EXPORT MySqlResult
{
	public:
		MySqlResult(MYSQL_RES *result, MySqlDB *parent);
		~MySqlResult();

		QVariant	value(unsigned int field);
		QVariant	value(QString field);

		bool		next();
		bool		prev();
		bool		gotoRecord(unsigned long r);

		unsigned int	numFields();
		unsigned int	numRows();

		unsigned int	currentRecord();

		KexiDBField	*fieldInfo(unsigned int field);
		KexiDBField	*fieldInfo(QString field);

	protected:
		// mysql specific
		MYSQL_RES	*m_result;
		MYSQL_ROW	m_row;
		unsigned long* m_lengths;
		MYSQL_FIELD	*m_field;
		unsigned int	m_numFields;

		FieldNames	m_fieldNames;
		FieldInfo	m_fields;

		unsigned long	m_currentRecord;

		MySqlDB		*m_parent;
};

#endif
