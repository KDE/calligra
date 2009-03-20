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
#include <QSqlDatabase>

#include <kexidb/cursor.h>
#include <kexidb/utils.h>
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

public:
    orQuery();
    orQuery(const QString &, const QString &, bool doexec, KexiDB::Connection *conn = 0);

    virtual ~orQuery();

    inline bool queryExecuted() const {
        return (m_cursor != 0);
    }
    bool execute();

    inline KexiDB::Cursor *getQuery() {
        return m_cursor;
    }
    inline const QString &getSql() const {
        return m_qstrQuery;
    }
    inline const QString &getName() const {
        return m_qstrName;
    }

    uint fieldNumber(const QString &fld);
    KexiDB::TableOrQuerySchema &schema();
};


// Data class
class orData
{
private:
    orQuery *m_query;
    QString m_field;
    QString m_value;
    bool    m_valid;
    QByteArray m_rawValue;

public:
    orData();

    void  setQuery(orQuery *qryPassed);
    void  setField(const QString &qstrPPassed);

    inline bool  isValid() const {
        return m_valid;
    }

    const QString &getValue();
    const QByteArray &getRawValue();
};

#endif // __ORUTILS_H__

