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

#ifndef CQLRECORD_H
#define CQLRECORD_H

#include <qintdict.h>
#include <qmap.h>

#include <kexidbrecord.h>
#include <CqlSqlInclude.h>
#include <qvariant.h>
#include <qvaluevector.h>

#include "cqlfield.h"

typedef QIntDict<CqlField> FieldIndex;
typedef QIntDict<CqlString> RecordSet;
typedef QValueVector<CqlString> DataVector;

class CqlRecord : public KexiDBRecord
{
	public:
		CqlRecord(SqlHandle *handle, const QString statement);
		~CqlRecord();

		bool		readOnly();
		void		reset();

		bool		commit(unsigned int record, bool insertBuffer=false);
		
		QVariant	value(unsigned int field);
		QVariant	value(QString field);

		QVariant::Type	type(unsigned int field);
		QVariant::Type	type(QString field);

		KexiDBField::ColumnType sqlType(unsigned int field);
		KexiDBField::ColumnType sqlType(QString field);

		KexiDBField*	fieldInfo(unsigned int column);
		KexiDBField*	fieldInfo(QString column);

		bool		update(unsigned int record, unsigned int field, QVariant value);
		bool		update(unsigned int record, QString field, QVariant value);

		bool		deleteRecord(uint record);

		int		insert();

		void		gotoRecord(unsigned int record);

		unsigned int	fieldCount();
		
		QString		fieldName(unsigned int field);

		bool		next();

		unsigned long	last_id();


	protected:
		void		setupCursor();

		Cursor		*m_cursor;
		unsigned int	m_fieldCount;
		FieldIndex	m_fields;
		RecordSet	m_data;
		DataVector	m_datavector;
};

#endif
