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

#include <iostream.h>

#include <klocale.h>
#include <kdebug.h>

#include <kexidberror.h>

#include "cqldb.h"
#include "cqlrecord.h"

CqlRecord::CqlRecord(SqlHandle *handle, const QString statement)
 : KexiDBRecord()
{
	try
	{
		m_cursor = handle->declareCursor(statement.latin1());

		try
		{
			m_cursor->open();
			setupCursor();
		}
		catch(CqlException &err)
		{
			cerr << err << endl;
		}
	}
	catch(CqlException &err)
	{
		cerr << err << endl;
		throw KexiDBError(0, CqlDB::errorText(err));
	}
	
//	m_cursor->GetResultInfo();
}

bool
CqlRecord::readOnly()
{
	return true;
}

void
CqlRecord::reset()
{
	return false;
}

bool
CqlRecord::commit(unsigned int record, bool insertBuffer=false)
{
	return false;
}

QVariant
CqlRecord::value(unsigned int field)
{
	kdDebug() << "CqlRecord::value" << endl;
//	return QVariant(CqlDB::cqlString(*m_datavector.at(field)));
	return QVariant(CqlDB::cqlString(*m_data[field]));
//	return QVariant("");
//	return QVariant(m_
}

QVariant
CqlRecord::value(QString field)
{
	kdDebug() << "CqlRecord::value" << endl;
}

QVariant::Type
CqlRecord::type(unsigned int field)
{
	return m_fields[field]->type();
	kdDebug() << "CqlRecord::type" << endl;
}

QVariant::Type
CqlRecord::type(QString field)
{
	kdDebug() << "CqlRecord::type" << endl;
}

KexiDBField::ColumnType
CqlRecord::sqlType(unsigned int field)
{
	kdDebug() << "CqlRecord::sqlType" << endl;
}

KexiDBField::ColumnType
CqlRecord::sqlType(QString field)
{
	kdDebug() << "CqlRecord::sqlType" << endl;
}

KexiDBField*
CqlRecord::fieldInfo(unsigned int column)
{
	kdDebug() << "CqlRecord::fieldInfo" << endl;
	return m_fields[column];
/*
 l,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,.
*/
}

KexiDBField*
CqlRecord::fieldInfo(QString column)
{
	kdDebug() << "CqlRecord::filedInfo" << endl;
}

bool
CqlRecord::update(unsigned int record, unsigned int field, QVariant value)
{
}

bool
CqlRecord::update(unsigned int record, QString field, QVariant value)
{
}

bool
CqlRecord::deleteRecord(uint record)
{
}

int
CqlRecord::insert()
{
}

void
CqlRecord::gotoRecord(unsigned int record)
{
}

unsigned int
CqlRecord::fieldCount()
{
	return m_fieldCount;
}

QString
CqlRecord::fieldName(unsigned int field)
{
	kdDebug() << "CqlRecord::fieldName" << endl;
	return m_fields[field]->name();
}

bool
CqlRecord::next()
{
	kdDebug() << "CqlRecord::next" << endl;
	if(!m_cursor)
		return false;

//	return false;
	try
	{
		return m_cursor->fetch();
	}
	catch(CqlException &err)
	{
		cerr << err << endl;
	}
	
//	return false;
//	kdDebug() << "CqlRecord::next (dbg2)" << endl;
	
}

unsigned long
CqlRecord::last_id()
{
}

void
CqlRecord::setupCursor()
{
	unsigned int fields = 0;
	ColumnMetadataList *metalist = m_cursor->describe();
	for(ColumnMetadata *meta = metalist->first(); meta; meta = metalist->next())
	{
		CqlField *cfield = new CqlField(meta);
		m_fields.insert(fields, cfield);

//		m_datavector.push_back(CqlString());
		bool null;
		m_nullvector.push_back(null);
		CqlString *s = new CqlString();
//		CqlString data;
//		bool flag;
//		m_cursor->bindColumn(fields, m_datavector.last(), m_nullvector.last(), true);
		m_cursor->bindColumn(fields, *s, m_nullvector.last(), true);
		m_data.insert(fields, s);

		kdDebug() << "CqlRecord::setupCursor: bind cursor " << fields << endl;
		fields++;
	}

	m_fieldCount = fields;
}

CqlRecord::~CqlRecord()
{
}

//#include "cqlrecord.moc"
