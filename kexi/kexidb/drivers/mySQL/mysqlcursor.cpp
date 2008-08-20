/* This file is part of the KDE project
   Copyright (C) 2003 Joseph Wenninger<jowenn@kde.org>
   Copyright (C) 2005 Jarosław Staniek <staniek@kde.org>

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

#include "mysqlcursor.h"
#include "mysqlconnection.h"
#include "mysqlconnection_p.h"
#include <kexidb/error.h>
#include <kexidb/utils.h>
#include <klocale.h>
#include <kdebug.h>
#include <limits.h>

#define BOOL bool

using namespace KexiDB;

MySqlCursor::MySqlCursor(KexiDB::Connection* conn, const QString& statement, uint cursor_options)
        : Cursor(conn, statement, cursor_options)
        , d(new MySqlCursorData(conn))
{
    m_options |= Buffered;
    d->mysql = static_cast<MySqlConnection*>(conn)->d->mysql;
// KexiDBDrvDbg << "MySqlCursor: constructor for query statement" << endl;
}

MySqlCursor::MySqlCursor(Connection* conn, QuerySchema& query, uint options)
        : Cursor(conn, query, options)
        , d(new MySqlCursorData(conn))
{
    m_options |= Buffered;
    d->mysql = static_cast<MySqlConnection*>(conn)->d->mysql;
// KexiDBDrvDbg << "MySqlCursor: constructor for query statement" << endl;
}

MySqlCursor::~MySqlCursor()
{
    close();
}

bool MySqlCursor::drv_open()
{
// KexiDBDrvDbg << "MySqlCursor::drv_open:" << m_sql << endl;
    // This can't be right?  mysql_real_query takes a length in order that
    // queries can have binary data - but strlen does not allow binary data.
    if (mysql_real_query(d->mysql, m_sql.toUtf8(), strlen(m_sql.toUtf8())) == 0) {
        if (mysql_errno(d->mysql) == 0) {
            d->mysqlres = mysql_store_result(d->mysql);
            m_fieldCount = mysql_num_fields(d->mysqlres);
            m_fieldsToStoreInRow = m_fieldCount;
            d->numRows = mysql_num_rows(d->mysqlres);
            m_at = 0;

            m_opened = true;
            m_records_in_buf = d->numRows;
            m_buffering_completed = true;
            m_afterLast = false;
            return true;
        }
    }

    setError(ERR_DB_SPECIFIC, QString::fromUtf8(mysql_error(d->mysql)));
    return false;
}

bool MySqlCursor::drv_close()
{
    mysql_free_result(d->mysqlres);
    d->mysqlres = 0;
    d->mysqlrow = 0;
//js: done in superclass: m_numFields=0;
    d->lengths = 0;
    m_opened = false;
    d->numRows = 0;
    return true;
}

/*bool MySqlCursor::drv_moveFirst() {
  return true; //TODO
}*/

void MySqlCursor::drv_getNextRecord()
{
// KexiDBDrvDbg << "MySqlCursor::drv_getNextRecord" << endl;
    if (at() < d->numRows && at() >= 0) {
        d->lengths = mysql_fetch_lengths(d->mysqlres);
        m_result = FetchOK;
    } else if (at() >= d->numRows) {
        m_result = FetchEnd;
    } else {
        // control will reach here only when at() < 0 ( which is usually -1 )
        // -1 is same as "1 beyond the End"
        m_result = FetchEnd;
    }
}

// This isn't going to work right now as it uses d->mysqlrow
QVariant MySqlCursor::value(uint pos)
{
    if (!d->mysqlrow || pos >= m_fieldCount || d->mysqlrow[pos] == 0)
        return QVariant();

    KexiDB::Field *f = (m_fieldsExpanded && pos < m_fieldsExpanded->count())
                       ? m_fieldsExpanded->at(pos)->field : 0;

//! @todo js: use MYSQL_FIELD::type here!

    return KexiDB::cstringToVariant(d->mysqlrow[pos], f, d->lengths[pos]);
    /* moved to cstringToVariant()
      //from most to least frequently used types:
      if (!f || f->isTextType())
        return QVariant( QString::fromUtf8((const char*)d->mysqlrow[pos], d->lengths[pos]) );
      else if (f->isIntegerType())
    //! @todo support BigInteger
        return QVariant( Q3CString((const char*)d->mysqlrow[pos], d->lengths[pos]).toInt() );
      else if (f->isFPNumericType())
        return QVariant( Q3CString((const char*)d->mysqlrow[pos], d->lengths[pos]).toDouble() );

      //default
      return QVariant(QString::fromUtf8((const char*)d->mysqlrow[pos], d->lengths[pos]));*/
}


/* As with sqlite, the DB library returns all values (including numbers) as
   strings. So just put that string in a QVariant and let KexiDB deal with it.
 */
bool MySqlCursor::drv_storeCurrentRow(RecordData& data) const
{
// KexiDBDrvDbg << "MySqlCursor::storeCurrentRow: Position is " << (long)m_at<< endl;
    if (d->numRows <= 0)
        return false;

//! @todo js: use MYSQL_FIELD::type here!
//!           see SQLiteCursor::storeCurrentRow()

    const uint fieldsExpandedCount = m_fieldsExpanded ? m_fieldsExpanded->count() : UINT_MAX;
    const uint realCount = qMin(fieldsExpandedCount, m_fieldsToStoreInRow);
    for (uint i = 0; i < realCount; i++) {
        Field *f = m_fieldsExpanded ? m_fieldsExpanded->at(i)->field : 0;
        if (m_fieldsExpanded && !f)
            continue;
        data[i] = KexiDB::cstringToVariant(d->mysqlrow[i], f, d->lengths[i]);
        /* moved to cstringToVariant()
            if (f && f->type()==Field::BLOB) {
              data[i] = QByteArray(d->mysqlrow[i], d->mysqlres->lengths[i]);
              KexiDBDbg << data[i].toByteArray().size() << endl;
            }
        //! @todo more types!
        //! @todo look at what type mysql declares!
            else {
              data[i] = QVariant(QString::fromUtf8((const char*)d->mysqlrow[i], d->lengths[i]));
            }*/
    }
    return true;
}

void MySqlCursor::drv_appendCurrentRecordToBuffer()
{
}


void MySqlCursor::drv_bufferMovePointerNext()
{
    d->mysqlrow = mysql_fetch_row(d->mysqlres);
    d->lengths = mysql_fetch_lengths(d->mysqlres);
}

void MySqlCursor::drv_bufferMovePointerPrev()
{
    //MYSQL_ROW_OFFSET ro=mysql_row_tell(d->mysqlres);
    mysql_data_seek(d->mysqlres, m_at - 1);
    d->mysqlrow = mysql_fetch_row(d->mysqlres);
    d->lengths = mysql_fetch_lengths(d->mysqlres);
}


void MySqlCursor::drv_bufferMovePointerTo(qint64 to)
{
    //MYSQL_ROW_OFFSET ro=mysql_row_tell(d->mysqlres);
    mysql_data_seek(d->mysqlres, to);
    d->mysqlrow = mysql_fetch_row(d->mysqlres);
    d->lengths = mysql_fetch_lengths(d->mysqlres);
}

const char** MySqlCursor::rowData() const
{
    //! @todo
    return 0;
}

int MySqlCursor::serverResult()
{
    return d->res;
}

QString MySqlCursor::serverResultName()
{
    return QString();
}

void MySqlCursor::drv_clearServerResult()
{
    if (!d)
        return;
    d->res = 0;
}

QString MySqlCursor::serverErrorMsg()
{
    return d->errmsg;
}
