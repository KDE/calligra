/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
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
 * Please contact info@openmfg.com with any questions on this license.
 */

#include "orutils.h"
#include <kdebug.h>

//
// Class orQuery implementations
//
orQuery::orQuery()
{
    m_cursor = 0;
    m_connection = 0;
}

orQuery::orQuery(const QString &qstrPName, const QString &qstrSQL,
                 bool doexec, KexiDB::Connection * pDb)
{
    QString qstrParsedSQL(qstrSQL);
    QString qstrParam;
    int     intParamNum;
    int     intStartIndex = 0;

    m_cursor = 0;
    m_connection = pDb;
    m_schema = 0;
    //  Initialize some privates
    m_qstrName  = qstrPName;
    m_qstrQuery = qstrSQL;

    kDebug() << m_qstrName << ":" << m_qstrQuery;

    //For now, lets assume we only support simple tables or queries
    if (doexec) {
        execute();
    }

}

orQuery::~orQuery()
{
    m_cursor->close();
    delete m_cursor;
    m_cursor = 0;
}

bool orQuery::execute()
{
    if (m_connection && m_cursor == 0) {
        //NOTE we can use the variation of executeQuery to pass in parameters
        if (m_qstrQuery.isEmpty()) {
            m_cursor = m_connection->executeQuery("SELECT '' AS expr1 FROM kexi__db WHERE kexi__db.db_property = 'kexidb_major_ver'");
        } else if (m_connection->tableSchema(m_qstrQuery)) {
            kDebug() << m_qstrQuery <<  " is a table..";
            m_cursor = m_connection->executeQuery(* (m_connection->tableSchema(m_qstrQuery)), 1);
            m_schema = new KexiDB::TableOrQuerySchema(m_connection->tableSchema(m_qstrQuery));
        } else if (m_connection->querySchema(m_qstrQuery)) {
            kDebug() << m_qstrQuery <<  " is a query..";
            m_cursor = m_connection->executeQuery(* (m_connection->querySchema(m_qstrQuery)), 1);
            kDebug() << "...got test result";
            m_schema = new KexiDB::TableOrQuerySchema(m_connection->querySchema(m_qstrQuery));
            kDebug() << "...got schema";

        }

        if (m_cursor) {
            kDebug() << "Moving to first row..";
            return m_cursor->moveFirst();
        } else
            return false;
    }
    return false;
}

uint orQuery::fieldNumber(const QString &fld)
{
    uint x = -1;
    if (m_cursor->query()) {
        KexiDB::QueryColumnInfo::Vector flds = m_cursor->query()->fieldsExpanded();
        for (int i = 0; i < flds.size() ; ++i) {
            if (fld.toLower() == flds[i]->aliasOrName()) {
                x = i;
            }
        }
    }
    return x;
}

KexiDB::TableOrQuerySchema &orQuery::schema()
{
    if (m_schema)
        return *m_schema;
    else {
        Q_ASSERT(m_connection);
        KexiDB::TableOrQuerySchema *tq = new KexiDB::TableOrQuerySchema(m_connection, "");
        return *tq;
    }
}

//
// Class orData
//
orData::orData()
{
    m_valid = false;
    m_query = 0;
}

void orData::setQuery(orQuery *qryPassed)
{
    m_query = qryPassed;
    m_valid = (m_query != 0 && m_field.length());
}

void orData::setField(const QString &qstrPPassed)
{
    m_field = qstrPPassed;
    m_valid = (m_query != 0 && m_field.length());
}

const QString &orData::getValue()
{
    if (m_valid && m_query->getQuery()) {
        m_value = m_query->getQuery()->value(m_query->fieldNumber(m_field)).toString();
    } else {
        kDebug() << "Not Valid";
    }
    return m_value;
}

const QByteArray &orData::getRawValue()
{
    if (m_valid && m_query->getQuery()) {
        m_rawValue = m_query->getQuery()->value(m_query->fieldNumber(m_field)).toByteArray();
    }

    return m_rawValue;
}

