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

#ifndef MYSQLRECORD_H
#define MYSQLRECORD_H

#include <qvariant.h>
#include <qptrlist.h>
#include <qmap.h>

#include "mysqlresult.h"

#include "../../kexiDB/kexidbrecord.h"
#include "../../kexiDB/kexidbfield.h"

class MySqlRecord;
class MySqlResult;

typedef QPtrList<MySqlRecord> InsertList;
typedef QMap<unsigned int, QVariant> UpdateBuffer;
typedef QMap<QString, unsigned int> FieldList;

class MySqlRecord : KexiDBRecord
{
	public:
		MySqlRecord(MySqlResult *result, unsigned int record, MySqlRecord *parent=0);
		~MySqlRecord();

		//KexiDBRecord members
		void reset();
		bool commit(bool insertBuffer);

		QVariant value(unsigned int field);
		QVariant value(QString field);

		QVariant::Type type(unsigned int field);
		QVariant::Type type(QString field);

		KexiDBField::ColumnType sqlType(unsigned int field);
		KexiDBField::ColumnType sqlType(QString field);

		bool update(unsigned int field, QVariant value);
		bool update(QString field, QVariant value);

		bool deleteRecord();

		MySqlRecord *insert();

		MySqlRecord *operator++();
		MySqlRecord *operator--();

		MySqlRecord *gotoRecord(unsigned int record);

		//needed members
		void takeInsertBuffer(MySqlRecord *buffer);

	protected:
		MySqlRecord	*m_parent;

		InsertList	m_insertList;
		UpdateBuffer	m_updateBuffer;
};

#endif
