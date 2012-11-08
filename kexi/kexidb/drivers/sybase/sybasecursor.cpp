/* This file is part of the KDE project
   Copyright (C) 2007 Sharan Rao <sharanrao@gmail.com>

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


#include "sybasecursor.h"
#include "sybaseconnection.h"
#include "sybaseconnection_p.h"

#include <QtGlobal>

#include <db/error.h>
#include <db/utils.h>
#include <klocale.h>
#include <kdebug.h>
#include <limits.h>
#include <cstring>

#include <sqldb.h>

using namespace KexiDB;

SybaseCursor::SybaseCursor(KexiDB::Connection* conn, const QString& statement, uint cursor_options)
        : Cursor(conn, statement, cursor_options)
        , d(new SybaseCursorData(conn))
{

    //m_options |= Buffered;

    d->dbProcess = static_cast<SybaseConnection*>(conn)->d->dbProcess;
// KexiDBDrvDbg << "SybaseCursor: constructor for query statement";
}

SybaseCursor::SybaseCursor(Connection* conn, QuerySchema& query, uint options)
        : Cursor(conn, query, options)
        , d(new SybaseCursorData(conn))
{
    //  m_options |= Buffered;

    d->dbProcess = static_cast<SybaseConnection*>(conn)->d->dbProcess;
// KexiDBDrvDbg << "SybaseCursor: constructor for query statement";
}

SybaseCursor::~SybaseCursor()
{
    close();
}

bool SybaseCursor::drv_open()
{

    /* Pseudo Code
     *
     * Execute Query
     * If no error
     *   Store Result in buffer ( d-> ?? )
     *   Store fieldcount ( no. of columns ) in m_fieldCount
     *   Set m_fieldsToStoreInRow equal to m_fieldCount
     *   Store number of rows in d->numRows
     *   Set pointer at 0 ( m_at = 0 )
     *
     *   Set opened flag as true ( m_opened = true )
     *   Set numberoOfRecordsInbuffer as d->numRows ( m_records_in_buf = d->numRows )
     *   Set Buffering Complete flag = true
     *   Set After Last flag = false
     *
     */

    // clear all previous results ( if remaining )
    if (dbcancel(d->dbProcess) == FAIL)
        KexiDBDrvDbg << "drv_open" << "dead DBPROCESS ?";

    // insert into command buffer
    dbcmd(d->dbProcess, m_sql.toUtf8());
    // execute query
    dbsqlexec(d->dbProcess);

    if (dbresults(d->dbProcess) == SUCCEED) {
        // result set goes directly into dbProcess' buffer
        m_fieldCount = dbnumcols(d->dbProcess);
        m_fieldsToStoreInRow = m_fieldCount;

        // only relevant if buffering will ever work
        // <ignore>
        d->numRows = DBLASTROW(d->dbProcess);   // only true if buffering enabled
        m_records_in_buf = d->numRows;
        m_buffering_completed = true;
        // </ignore>

        m_afterLast = false;
        m_opened = true;
        m_at = 0;

        return true;
    }

    setError(ERR_DB_SPECIFIC, static_cast<SybaseConnection*>(connection())->d->errmsg);
    return false;
}


bool SybaseCursor::drv_close()
{

    m_opened = false;
    d->numRows = 0;
    return true;
}

/*bool SybaseCursor::drv_moveFirst() {
  return true; //TODO
}*/

void SybaseCursor::drv_getNextRecord()
{
// KexiDBDrvDbg << "SybaseCursor::drv_getNextRecord";

    // no buffering , and we don't know how many rows are there in result set

    if (dbnextrow(d->dbProcess) != NO_MORE_ROWS)
        m_result = FetchOK;
    else {
        m_result = FetchEnd;
    }

}


QVariant SybaseCursor::value(uint pos)
{
    if (!d->dbProcess || pos >= m_fieldCount)
        return QVariant();

    KexiDB::Field *f = (m_fieldsExpanded && pos < m_fieldsExpanded->count())
                       ? m_fieldsExpanded->at(pos)->field : 0;

    // db-library indexes its columns from 1
    pos = pos + 1;

    long int columnDataLength = dbdatlen(d->dbProcess, pos);

    // 512 is
    // 1. the length used internally in dblib for allocating data to each column in function dbprrow()
    // 2. it's greater than all the values returned in the dblib internal function _get_printable_size
    long int pointerLength = qMax(columnDataLength , (long int)512);

    BYTE* columnValue = new unsigned char[pointerLength + 1] ;

    // convert to string representation. All values are convertible to string
    dbconvert(d->dbProcess , dbcoltype(d->dbProcess , pos), dbdata(d->dbProcess , pos), columnDataLength , (SYBCHAR), columnValue, -2);

    QVariant returnValue = KexiDB::cstringToVariant((const char*)columnValue , f, strlen((const char*)columnValue));

    delete[] columnValue;

    return returnValue;
}


/* As with sqlite, the DB library returns all values (including numbers) as
   strings. So just put that string in a QVariant and let KexiDB deal with it.
 */
bool SybaseCursor::drv_storeCurrentRow(RecordData& data) const
{
// KexiDBDrvDbg << "SybaseCursor::storeCurrentRow: Position is " << (long)m_at;
// if (d->numRows<=0)
//  return false;

    const uint fieldsExpandedCount = m_fieldsExpanded ? m_fieldsExpanded->count() : UINT_MAX;
    const uint realCount = qMin(fieldsExpandedCount, m_fieldsToStoreInRow);
    for (uint i = 0; i < realCount; i++) {
        Field *f = m_fieldsExpanded ? m_fieldsExpanded->at(i)->field : 0;
        if (m_fieldsExpanded && !f)
            continue;

        long int columnDataLength = dbdatlen(d->dbProcess, i + 1);

        // 512 is
        // 1. the length used internally in dblib for allocating data to each column in function dbprrow()
        // 2. it's greater than all the values returned in the dblib internal function _get_printable_size
        long int pointerLength = qMax(columnDataLength , (long int)512);

        BYTE* columnValue = new unsigned char[pointerLength + 1] ;

        // convert to string representation. All values are convertible to string
        dbconvert(d->dbProcess , dbcoltype(d->dbProcess , i + 1), dbdata(d->dbProcess , i + 1), columnDataLength , (SYBCHAR), columnValue, -2);

        data[i] =  KexiDB::cstringToVariant((const char*)columnValue , f,  strlen((const char*)columnValue));

        delete[] columnValue;
    }
    return true;
}

void SybaseCursor::drv_appendCurrentRecordToBuffer()
{
}


void SybaseCursor::drv_bufferMovePointerNext()
{
    //dbgetrow( d->dbProcess, m_at + 1 );
}

void SybaseCursor::drv_bufferMovePointerPrev()
{
    //dbgetrow( d->dbProcess, m_at - 1 );
}


void SybaseCursor::drv_bufferMovePointerTo(qint64 /*to*/)
{
    //dbgetrow( d->dbProcess, to );
}

const char** SybaseCursor::rowData() const
{
    //! @todo
    return 0;
}

int SybaseCursor::serverResult()
{
    return d->res;
}

QString SybaseCursor::serverResultName()
{
    return QString();
}

void SybaseCursor::drv_clearServerResult()
{
    if (!d)
        return;
    d->res = 0;
}

QString SybaseCursor::serverErrorMsg()
{
    return d->errmsg;
}
