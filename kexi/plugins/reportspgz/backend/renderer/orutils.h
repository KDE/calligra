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
#ifndef __ORUTILS_H__
#define __ORUTILS_H__

#include <QString>
#include <QStringList>

#include <kexidb/cursor.h>
#include <kexidb/utils.h>

#include <migration/migratemanager.h>
#include <migration/keximigrate.h>

//
// These classes are used by the original orRender class and the new
// ORPreRenderer class as internal structures for processing. There is
// no need to have or use these classes otherwise.
//


//
// Private class definitions
// These classes are convienience classes just used here
// so there is no need to expose them to the outside
//
//  Query Class
class orQuery
{
private:
    QString         m_qstrName;

    QString         m_qstrQuery;
    KexiDB::Cursor   *m_cursor;

    KexiDB::Connection   *m_connection;
    KexiDB::TableOrQuerySchema *m_schema;
    
    KexiMigration::KexiMigrate *m_KexiMigrate;
    
    bool executeInternal();
    
    bool m_externalData;

    bool m_valid;
    KexiDB::TableSchema m_TableSchema;
    
public:
    orQuery(const QString &, KexiDB::Connection *conn); //Internal Data
    orQuery(const QString &); //External data
    
    virtual ~orQuery();
    
    inline KexiDB::Connection *connection() const {
      if (!m_externalData)
	return m_connection;
	
      return 0;
    }

    inline const QString &getSql() const {
        return m_qstrQuery;
    }

    uint fieldNumber(const QString &fld);
    KexiDB::TableOrQuerySchema &schema() const;
    
    QVariant value(unsigned int);
    QVariant value(const QString &);
    
    bool moveNext();
    bool movePrevious();
    bool moveFirst();
    bool moveLast();
    
    long at() const;
    
    long recordCount();
};

#endif // __ORUTILS_H__

