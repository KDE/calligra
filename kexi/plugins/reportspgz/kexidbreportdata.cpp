/*
* Kexi Report Plugin
* Copyright (C) 2007-2009 by Adam Pigg (adam@piggz.co.uk)
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "kexidbreportdata.h"
#include <kdebug.h>
#include <kexidb/queryschema.h>

KexiDBReportData::KexiDBReportData ( const QString &qstrSQL,
                                     KexiDB::Connection * pDb )
{
    m_cursor = 0;
    m_connection = pDb;
    m_schema = 0;
    m_qstrQuery = qstrSQL;
    getSchema();
}

void KexiDBReportData::setSorting ( KoReportData::SortList sl )
{
    if ( sl.count() > 0 )
    {
        KexiDB::OrderByColumnList order;
        
        for ( unsigned int i = 0; i < sl.count(); ++i )
        {
            order.appendField(*m_schema, sl[i].field, sl[i].order == KoReportData::Sort::Ascending);
        }
        m_schema->setOrderByColumnList(order);
    }
}

KexiDBReportData::~KexiDBReportData()
{
    close();
}

bool KexiDBReportData::open()
{
    if ( m_connection && m_cursor == 0 )
    {
        if ( m_qstrQuery.isEmpty() )
        {
            m_cursor = m_connection->executeQuery ( "SELECT '' AS expr1 FROM kexi__db WHERE kexi__db.db_property = 'kexidb_major_ver'" );
        }
        else if ( m_schema)
        {
            kDebug() << "Opening cursor..";
            m_cursor = m_connection->executeQuery ( *m_schema, 1 );
        }

        if ( m_cursor )
        {
            kDebug() << "Moving to first row..";
            return m_cursor->moveFirst();
        }
        else
            return false;
    }
    return false;
}

bool KexiDBReportData::close()
{
    if ( m_cursor )
    {
        m_cursor->close();
        delete m_cursor;
        m_cursor = 0;
    }

    return true;
}

bool KexiDBReportData::getSchema()
{
    if ( m_connection )
    {
        if ( m_connection->tableSchema ( m_qstrQuery ) )
        {
            kDebug() << m_qstrQuery <<  " is a table..";
            m_schema = new KexiDB::QuerySchema ( *(m_connection->tableSchema ( m_qstrQuery )) );
        }
        else if ( m_connection->querySchema ( m_qstrQuery ) )
        {
            kDebug() << m_qstrQuery <<  " is a query..";
            m_schema = m_connection->querySchema ( m_qstrQuery );
        }
        return true;
    }
    return false;
}

void* KexiDBReportData::connection() const
{
    return m_connection;
}

QString KexiDBReportData::source() const
{
    return m_qstrQuery;
}

uint KexiDBReportData::fieldNumber ( const QString &fld )
{
    KexiDB::QueryColumnInfo::Vector flds;
    
    uint x = -1;
    if ( m_cursor && m_cursor->query() )
    {
        flds = m_cursor->query()->fieldsExpanded();
    }

    for ( int i = 0; i < flds.size() ; ++i )
    {
        if ( fld.toLower() == flds[i]->aliasOrName().toLower() )
        {
            x = i;
        }
    }

    kDebug() << "Field number of" << fld << "is" << x;

    return x;
}

QStringList KexiDBReportData::fieldNames()
{
    QStringList names;

    if ( m_schema )
    {
        for(unsigned int i = 0; i < m_schema->fieldCount(); ++i)
        {
            names << m_schema->field(i)->name();
        }
    }

    return names;
}

void* KexiDBReportData::schema() const
{
    return m_schema;
}

QVariant KexiDBReportData::value ( unsigned int i )
{
    if ( m_cursor )
        return m_cursor->value ( i );

    return QVariant();
}

QVariant KexiDBReportData::value ( const QString &fld )
{
    int i = fieldNumber ( fld );

    if ( m_cursor )
        return m_cursor->value ( i );

    return QVariant();
}

bool KexiDBReportData::moveNext()
{
    if ( m_cursor )
        return m_cursor->moveNext();

    return false;
}

bool KexiDBReportData::movePrevious()
{
    if ( m_cursor ) return m_cursor->movePrev();

    return false;
}

bool KexiDBReportData::moveFirst()
{
    if ( m_cursor ) return m_cursor->moveFirst();

    return false;
}

bool KexiDBReportData::moveLast()
{
    if ( m_cursor )
        return m_cursor->moveLast();

    return false;
}

long KexiDBReportData::at() const
{
    if ( !m_cursor )
        return m_cursor->at();

    return 0;
}

long KexiDBReportData::recordCount() const
{
    if ( m_schema )
    {
        return KexiDB::rowCount ( *m_schema );
    }
    else
    {
        return 1;
    }
}
