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

#ifndef MYSQLRESULT_H
#define MYSQLRESULT_H

#include <mysql/mysql.h>

#include <qmap.h>
#include <qintdict.h>

#include "../../kexiDB/kexidbresult.h"
#include "../../kexiDB/kexidbfield.h"

#include "mysqlfield.h"
//#include "mysqlrecord.h"

typedef QMap<QString, int> FieldNames;
typedef QIntDict<KexiDBField> FieldInfo;

class MySqlResult : public KexiDBResult
{
	Q_OBJECT
	
	public:
		MySqlResult(MYSQL_RES *result, QObject *parent=0);
		~MySqlResult();

		QVariant	value(unsigned int field);
		QVariant	value(QString field);
		
		bool		next();

		unsigned int	numFields();
		unsigned int	numRows();
		
		KexiDBField	*fieldInfo(unsigned int field);
		KexiDBField	*fieldInfo(QString field);

	protected:
		// mysql specific
		MYSQL_RES	*m_result;
	        MYSQL_ROW	m_row;
		MYSQL_FIELD	*m_field;
		unsigned int	m_numFields;

		FieldNames	m_fieldNames;
		FieldInfo	m_fields;

		unsigned int	m_currentRecord;
};

#endif
