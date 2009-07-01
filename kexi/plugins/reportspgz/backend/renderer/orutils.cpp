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

orQuery::orQuery(const QString &qstrSQL,
                 KexiDB::Connection * pDb)
{
    QString qstrParsedSQL(qstrSQL);
    QString qstrParam;
    int     intParamNum;
    int     intStartIndex = 0;

    m_externalData = false;
    m_cursor = 0;
    m_connection = pDb;
    m_schema = 0;
    //  Initialize some privates
    m_qstrQuery = qstrSQL;

    kDebug() << m_qstrName << ":" << m_qstrQuery;

    m_valid = executeInternal();
}

//!Connect to an external data source
//!connStr is in the form driver|connection_string|table
orQuery::orQuery(const QString & connStr)
{
  m_externalData = true;
  m_cursor = 0;
  
  QStringList extConn = connStr.split("|");
  
  if (extConn.size() == 3)
  {
    KexiMigration::MigrateManager mm;
    
    m_KexiMigrate = mm.driver(extConn[0]);
    KexiDB::ConnectionData cd;
    KexiMigration::Data dat;
    cd.setFileName(extConn[1]);
    dat.source = &cd;
    m_KexiMigrate->setData(&dat);
    m_valid = m_KexiMigrate->connectSource();
    QStringList names;
    
    if (m_valid) {  
      m_valid = m_KexiMigrate->readTableSchema(extConn[2], m_TableSchema);
    }
    if (m_valid) {
      m_schema = new KexiDB::TableOrQuerySchema(m_TableSchema);
    }
    m_valid = m_KexiMigrate->tableNames(names);
    if (m_valid && names.contains(extConn[2])){ 
       m_valid = m_KexiMigrate->readFromTable(extConn[2]);
    }
  }
}

orQuery::~orQuery()
{
  if (m_externalData)
  {
    delete m_KexiMigrate;
  }
  else
  {
    if (m_cursor) {
      m_cursor->close();
      delete m_cursor;
      m_cursor = 0;
    }
  }
}

bool orQuery::executeInternal()
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
  KexiDB::QueryColumnInfo::Vector flds;
  
    uint x = -1;
    if (m_externalData) {
      flds = schema().columns();
    }
    else {
      if (m_cursor->query()) {
	  flds = m_cursor->query()->fieldsExpanded();
	  
      }
    }
    for (int i = 0; i < flds.size() ; ++i) {
	  if (fld.toLower() == flds[i]->aliasOrName().toLower()) {
	    x = i;
	  }
    
    }
	  
    return x;
}

KexiDB::TableOrQuerySchema &orQuery::schema() const
{
    if (m_schema)
        return *m_schema;
    else {
        Q_ASSERT(m_connection);
        KexiDB::TableOrQuerySchema *tq = new KexiDB::TableOrQuerySchema(m_connection, "");
        return *tq;
    }
}

QVariant orQuery::value(unsigned int i)
{
  if(!m_valid)
    return QVariant();
  
  if (m_externalData) {
    return m_KexiMigrate->value(i); 
  } else {
    return m_cursor->value(i);
  }
}

QVariant orQuery::value(const QString &fld)
{
  if(!m_valid)
    return QVariant();
  
  int i = fieldNumber(fld);
  
  if (m_externalData) {
    return m_KexiMigrate->value(i); 
  } else {
    return m_cursor->value(i);
  }
}

bool orQuery::moveNext()
{
  if (!m_valid)
    return false;
  
  if (m_externalData) {
    return m_KexiMigrate->moveNext(); 
  } else {
    return m_cursor->moveNext();
  }
}

bool orQuery::movePrevious()
{
  if(!m_valid)
    return false;
  
  if (m_externalData) {
    return m_KexiMigrate->movePrevious(); 
  } else {
    if (m_cursor) return m_cursor->movePrev();
  }
}

bool orQuery::moveFirst()
{
  if(!m_valid)
    return false;
  
  if (m_externalData) {
    return m_KexiMigrate->moveFirst(); 
  } else {
    if (m_cursor) return m_cursor->moveFirst();
  }
}

bool orQuery::moveLast()
{
  if(!m_valid)
    return false;
  
  if (m_externalData) {
    return m_KexiMigrate->moveLast(); 
  } else {
    if (m_cursor) return m_cursor->moveLast();
  }
}

long orQuery::at() const
{
  if(!m_valid)
    return 0;
  
  if (m_externalData) {
    return 0; 
  } else {
    return m_cursor->at();
  }
}

long orQuery::recordCount()
{
  if (m_externalData) {
    return 1;
  }
  else {
    if (schema().table() || schema().query()) {
      return KexiDB::rowCount(schema());
    } else {
      return 1;
    }
  }
}
