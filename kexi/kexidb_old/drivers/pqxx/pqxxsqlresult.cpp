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

#include <qvariant.h>
#include <qdatetime.h>

#include <kdebug.h>

#include "pqxxsqlresult.h"
#include "pqxxsqldb.h"
#include <stdio.h>
#include <iostream>
pqxxSqlResult::pqxxSqlResult(result *result, QObject *parent)
{
    oid tabloid;

    kdDebug() << "pqxxSqlResult::pqxxSqlResult()" << endl;
    //various initialisations...
    m_parent = parent;
    m_result = result;
    m_row = new result::tuple(m_result, 0);
    m_field = 0;
    m_currentRecord = 0;
    m_numFields = m_row->size();
    //creating field-index
    for (unsigned int i = 0; i < m_numFields; i++)
    {
        if (m_field)
        {
            delete m_field;
        }
        m_field = new result::field(*m_row, i);
        m_fieldNames.insert(QString::fromLatin1(m_field->name()), i);
        tabloid = m_result->column_table(i);
        kdDebug() << "Table OID: " << tabloid << endl;
        KexiDBField* f = new KexiDBField(static_cast<pqxxSqlDB*>(m_parent)->tableName(tabloid));
        f->setName(m_field->name());
        f->setColumnType(pqxxSqlDB::getInternalDataType(m_field->type()));
        f->setLength(m_field->size());
        //FIXME All This
        //f->setPrecision(m_field->decimals);
        //f->setUnsigned(m_field->flags & UNSIGNED_FLAG);
        //f->setBinary(m_field->flags & BINARY_FLAG);
        //f->setDefaultValue(m_field->def);
        //f->setAutoIncrement(m_field->flags & AUTO_INCREMENT_FLAG);
        f->setPrimaryKey(static_cast<pqxxSqlDB*>(m_parent)->primaryKey(m_field->table(), i));
        f->setUniqueKey(static_cast<pqxxSqlDB*>(m_parent)->uniqueKey(m_field->table(), i));
        f->setNotNull(static_cast<pqxxSqlDB*>(m_parent)->notNull(m_field->table(), i));
        m_fields.insert(i, f);
    }
}

unsigned int
pqxxSqlResult::numFields()
{
    return m_numFields;
}

unsigned int
pqxxSqlResult::numRows()
{
    return m_result->size();
}

bool
pqxxSqlResult::next()
{
    kdDebug() << "pqxxSqlResult::next()" << endl;
    if (m_currentRecord < m_result->size())
    {
        m_currentRecord++;
        delete m_row;
        m_row = new result::tuple(m_result, m_currentRecord - 1);
        return true;
    }
    else
    {
        return false;
    }
}

bool pqxxSqlResult::prev()
{
    kdDebug() << "pqxxSqlResult::prev()" << endl;
    if ((m_currentRecord > 1) && m_currentRecord < m_result->size())
    {
        m_currentRecord--;
        delete m_row;
        m_row = new result::tuple(m_result, m_currentRecord - 1);
        return true;
    }
    else
    {
        return false;
    }
}

bool pqxxSqlResult::gotoRecord(unsigned long r)
{
    kdDebug() << "pqxxSqlResult::gotoRecord():" << endl;

    delete m_row;
    m_row = new result::tuple(m_result, r);
    if(!m_row)
    {
        kdDebug() << "pqxxSqlResult::gotoRecord(): jump faild!" << endl;
        return false;
    }

    m_currentRecord = r;
    return true;
}

QVariant pqxxSqlResult::value(unsigned int field)
{
    kdDebug() << "pqxxSqlResult::value(int):" << field << endl;
    if(!m_row)
    {
        return 0;
    }

    switch(fieldInfo(field)->type())
    {
    case QVariant::Date:
        return QVariant(QDate::fromString(m_row->at(field).c_str(), Qt::ISODate));
    case QVariant::Int:
        return QVariant(QString(m_row->at(field).c_str()).toInt());
    case QVariant::ByteArray:
    {
            QByteArray ba;
            ba.duplicate(m_row->at(field).c_str(), m_row->at(field).size());
            return QVariant(ba);
    }
    case QVariant::Bool:
    {
	QString b = m_row->at(field).c_str();

	if (b == "t")
		return QVariant(1);
	else if (b == "f")
		return QVariant(0);
    }
    default:
        return QVariant(m_row->at(field).c_str());
    }

    return 0;
}

QVariant pqxxSqlResult::value(QString field)
{
    if(!m_fieldNames.contains(field))
        return QVariant();

    FieldNames::Iterator it;
    it = m_fieldNames.find(field);
    return value(it.data());
}

KexiDBField* pqxxSqlResult::fieldInfo(unsigned int field)
{
    return m_fields[field];
}

KexiDBField* pqxxSqlResult::fieldInfo(QString field)
{
    FieldNames::Iterator it;
    it = m_fieldNames.find(field);
    return fieldInfo(it.data());
}

unsigned int pqxxSqlResult::currentRecord()
{
    return m_currentRecord;
}

pqxxSqlResult::~pqxxSqlResult()
{
    kdDebug()<<"pqxxSqlResult::~pqxxSqlResult()"<<endl;
    //delete m_field;
    //delete m_row;
    //delete m_result;
}

