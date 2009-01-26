/* This file is part of the KDE project
   Copyright (C) 2009 Sharan Rao <sharanrao@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "odbccursor.h"
#include "odbcconnection.h"
#include "odbcconnection_p.h"
#include "odbcqueryunit.h"
#include "odbcsqlqueryunit.h"

#include <kexidb/error.h>
#include <kexidb/utils.h>
#include <klocale.h>
#include <kdebug.h>
#include <limits.h>

using namespace KexiDB;

ODBCCursor::ODBCCursor(KexiDB::Connection* conn, ODBCSQLQueryUnit* queryUnit, const QString& statement, uint cursor_options)
        : Cursor(conn, statement, cursor_options), d( 0 ), m_queryUnit( queryUnit )
{
}

ODBCCursor::ODBCCursor(Connection* conn, ODBCSQLQueryUnit* queryUnit, QuerySchema& query, uint options)
        : Cursor(conn, query, options), d( 0 ), m_queryUnit( queryUnit )
{
}

ODBCCursor::ODBCCursor(Connection* conn, ODBCQueryUnit* queryUnit, uint options)
        : Cursor( conn, "-", options ), d( 0 ), m_queryUnit( queryUnit )
{
    // this is a non-sql query cursor. so we don't need any query. such hacks :/
}

ODBCCursor::~ODBCCursor()
{
        close();
}

bool ODBCCursor::drv_open()
{
// KexiDBDrvDbg << "ODBCCursor::drv_open:" << m_sql;
    ODBCConnection* odbcConnection = dynamic_cast<ODBCConnection*>( connection() );
    if ( !odbcConnection )
        return false;

    d = new ODBCCursorData( m_conn );
    d->sql = m_sql; // store the sql in cursordata

    SQLRETURN returnStatus;
    // allocate a statement handle
    returnStatus = SQLAllocHandle( SQL_HANDLE_STMT, odbcConnection->d->connectionHandle, &d->statementHandle );
    if ( !SQL_SUCCEEDED( returnStatus ) ) {
        ODBCConnection::extractError( odbcConnection, d->connectionHandle, SQL_HANDLE_DBC );
        return false;
    }

    m_queryUnit->setCursorData( d );

    returnStatus = m_queryUnit->execute();
    if ( !SQL_SUCCEEDED( returnStatus ) ) {
        ODBCConnection::extractError( odbcConnection, d->statementHandle, SQL_HANDLE_STMT );
        return false;
    }

    // get the number of columns
    SQLSMALLINT columnCount;
    SQLNumResultCols( d->statementHandle, &columnCount );
    m_fieldCount = columnCount;
    m_fieldsToStoreInRow = m_fieldCount;

    // get the number of rows
    SQLLEN rowCount;
    SQLRowCount( d->statementHandle, &rowCount );
    m_records_in_buf = rowCount;      //  ignore.
    m_buffering_completed = true;     //  we don't do buffering yet as there are no cursor libraries

    // sanitise pointers
    m_at = 0;     // set pointer at 0
    m_afterLast = false; // set we are not after last
    m_opened = true; // set opened

    return true;
}

bool ODBCCursor::drv_close()
{
    m_opened = false;
    SQLFreeHandle( SQL_HANDLE_STMT, d->statementHandle );

    delete d;
    return true;
}

/*bool ODBCCursor::drv_moveFirst() {
  return true; //TODO
}*/

void ODBCCursor::drv_getNextRecord()
{
// KexiDBDrvDbg << "ODBCCursor::drv_getNextRecord";

    if ( SQL_SUCCEEDED( SQLFetch( d->statementHandle ) ) ) {
        m_result = FetchOK;
    } else {
        m_result = FetchEnd;
    }

}

QVariant ODBCCursor::value(uint pos)
{
    KexiDB::Field *f = (m_fieldsExpanded && pos < (uint)m_fieldsExpanded->count())
                       ? m_fieldsExpanded->at(pos)->field : 0;

    SQLRETURN returnStatus;
    SQLINTEGER indicator;

    // we index from 1 in ODBC
    pos = pos + 1;

    // first do a fake query to get the length
    returnStatus = SQLGetData(d->statementHandle , pos, SQL_C_CHAR, NULL, 0 , &indicator);
    if ( !SQL_SUCCEEDED(returnStatus ) ) {
        ODBCConnection* odbcConnection = dynamic_cast<ODBCConnection*>( connection() );
        if ( !odbcConnection )
            return false;
        ODBCConnection::extractError( odbcConnection, d->statementHandle, SQL_HANDLE_STMT );
        return false;
    }

    char* buf = 0;
    if ( indicator != SQL_NULL_DATA ) {
        // allocate space to read the stuff
        buf = new char[indicator+1];
        returnStatus = SQLGetData(d->statementHandle , pos, SQL_C_CHAR, buf, indicator + 1, &indicator);
        if ( !SQL_SUCCEEDED(returnStatus ) ) {
            ODBCConnection* odbcConnection = dynamic_cast<ODBCConnection*>( connection() );
            if ( !odbcConnection )
                return false;
            ODBCConnection::extractError( odbcConnection, d->statementHandle, SQL_HANDLE_STMT );
            return false;
        }
    }

    QVariant returnValue = QVariant() ; // an invalid value;
    returnValue =  KexiDB::cstringToVariant( ( const char* )buf, f, buf!=0 ? strlen( ( const char* )buf ) : 0);

    delete[] buf;
    return returnValue;
}


/* As with sqlite, the DB library returns all values (including numbers) as
   strings. So just put that string in a QVariant and let KexiDB deal with it.
 */
bool ODBCCursor::drv_storeCurrentRow(RecordData& data) const
{
// KexiDBDrvDbg << "ODBCCursor::storeCurrentRow: Position is " << (long)m_at;

    const uint fieldsExpandedCount = m_fieldsExpanded ? m_fieldsExpanded->count() : UINT_MAX;
    const uint realCount = qMin(fieldsExpandedCount, m_fieldsToStoreInRow);
    for (uint i = 0; i < realCount; i++) {
        Field *f = m_fieldsExpanded ? m_fieldsExpanded->at(i)->field : 0;
        if (m_fieldsExpanded && !f)
            continue;

        SQLRETURN returnStatus;
        SQLINTEGER indicator;

        // first do a fake query to get the length
        returnStatus = SQLGetData(d->statementHandle , i + 1, SQL_C_CHAR, NULL, 0 , &indicator);
        if ( !SQL_SUCCEEDED(returnStatus)) {
            ODBCConnection* odbcConnection = dynamic_cast<ODBCConnection*>( connection() );
            if ( !odbcConnection )
                return false;
            ODBCConnection::extractError( odbcConnection, d->statementHandle, SQL_HANDLE_STMT );
            return false;
        }

        char* buf = 0;
        if ( indicator != SQL_NULL_DATA ) {
            // allocate space to read the stuff
            buf = new char[indicator+1];
            returnStatus = SQLGetData(d->statementHandle , i + 1, SQL_C_CHAR, buf, indicator+1 , &indicator);
            if ( !SQL_SUCCEEDED(returnStatus)) {
                ODBCConnection* odbcConnection = dynamic_cast<ODBCConnection*>( connection() );
                if ( !odbcConnection )
                    return false;
                ODBCConnection::extractError( odbcConnection , d->statementHandle, SQL_HANDLE_STMT );
                delete[] buf;
                return false;
            }
        }

        data[i] = KexiDB::cstringToVariant( ( const char* )buf , f, buf != 0 ? strlen((const char*)buf) : 0 );
        delete[] buf;
    }

    return true;
}

void ODBCCursor::drv_appendCurrentRecordToBuffer()
{
}


void ODBCCursor::drv_bufferMovePointerNext()
{
}

void ODBCCursor::drv_bufferMovePointerPrev()
{
}

void ODBCCursor::drv_bufferMovePointerTo(qint64 /* to */ )
{
}

const char** ODBCCursor::rowData() const
{
    //! @todo
    return 0;
}

int ODBCCursor::serverResult()
{
    // TODO: return a server result code.
    return 0;
}

QString ODBCCursor::serverResultName()
{
    // TODO: return server result name.
    return QString();
}

void ODBCCursor::drv_clearServerResult()
{
    if (!d)
        return;
}

QString ODBCCursor::serverErrorMsg()
{
    return QString();
}
