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

#ifndef QTSQLRESULT_H
#define QTSQLRESULT_H

//#include <mysql/mysql.h>

#include <qmap.h>
#include <qintdict.h>
#include <qobject.h>

#include "../../kexidbfield.h"
#include "qtsqldb.h"

typedef QMap<QString, int> FieldNames;
typedef QIntDict<KexiDBField> FieldInfo;

class QtSqlResult
{
	public:
		QtSqlResult(void *result, QtSqlDB *parent);
		~QtSqlResult();

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
		/*QTSQL_RES*/void	*m_result;
		/*QTSQL_ROW*/int	m_row;
		unsigned long* m_lengths;
		/*QTSQL_FIELD*/void	*m_field;
		unsigned int	m_numFields;

		FieldNames	m_fieldNames;
		FieldInfo	m_fields;

		unsigned long	m_currentRecord;

		QtSqlDB		*m_parent;
};

#endif
