/* This file is part of the KDE project
Copyright (C) 2003 Joseph Wenninger<jowenn@kde.org>

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
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
*/

#include "mysqlcursor.h"
#include "mysqlconnection.h"
#include <kexidb/error.h>
#include <klocale.h>
#include <kdebug.h>

using namespace KexiDB;

MySqlCursor::MySqlCursor(KexiDB::Connection* conn, const QString& statement, uint cursor_options)
	: Cursor(conn,statement,cursor_options)
	, m_res(0)
	, m_row(0)
	, m_lengths(0)
	, m_numRows(0)
{
	m_options |= Buffered;
	my_conn = static_cast<MySqlConnection*>(conn)->m_mysql;
	KexiDBDrvDbg << "MySqlCursor: constructor for query statement" << endl;
}

MySqlCursor::MySqlCursor(Connection* conn, QuerySchema& query, uint options )
	: Cursor( conn, query, options )
	, m_res(0)
	, m_row(0)
	, m_lengths(0)
	, m_numRows(0)
{
	m_options |= Buffered;
	my_conn = static_cast<MySqlConnection*>(conn)->m_mysql;	
	KexiDBDrvDbg << "MySqlCursor: constructor for query statement" << endl;
}

MySqlCursor::~MySqlCursor() {
	close();
}

bool MySqlCursor::drv_open(const QString& statement) {
	KexiDBDrvDbg << "MySqlCursor::drv_open:" << statement << endl;
	if (!my_conn) {
		//should never happen, but who knows
		setError(ERR_NO_CONNECTION,i18n("No connection for cursor open operation specified"));
		return false;
	}
	// This can't be right?  mysql_real_query takes a length in order that
	// queries can have binary data - but strlen does not allow binary data.
	if(mysql_real_query(my_conn, statement.utf8(), strlen(statement.utf8())) == 0) {
		if(mysql_errno(my_conn) == 0) {
			m_res= mysql_store_result(my_conn);
			m_fieldCount=mysql_num_fields(m_res);
			m_numRows=mysql_num_rows(m_res);
			m_at=0;
			
			m_opened=true;
			m_records_in_buf = m_numRows;
			m_buffering_completed = true;
			m_afterLast=false;
			return true;
			}
	}
	
	setError(ERR_DB_SPECIFIC,QString::fromUtf8(mysql_error(my_conn)));
	return false;
}

bool MySqlCursor::drv_close() {
	mysql_free_result(m_res);
	m_res=0;
	m_row=0;
//js: done in superclass:	m_numFields=0;
	m_lengths=0;
	m_opened=false;
	m_numRows=0;
	return true;
}

/*bool MySqlCursor::drv_moveFirst() {
	return true; //TODO
}*/

void MySqlCursor::drv_getNextRecord() {
	KexiDBDrvDbg << "MySqlCursor::drv_getNextRecord" << endl;
	if (at() < m_numRows && at() >=0) {
		m_lengths=mysql_fetch_lengths(m_res);
		m_result=FetchOK;
	}
	else if (at() >= m_numRows) {
		m_result = FetchEnd;
	}
	else {
		m_result = FetchError;
	}
}

// This isn't going to work right now as it uses m_row
QVariant MySqlCursor::value(uint pos) {
	if (!m_row) return QVariant();
	if (pos>=m_fieldCount) return QVariant();
	if (m_row[pos]==0) return QVariant();
	//js TODO: encode for type using m_fieldsExpanded like in SQLiteCursor::value()
	return QVariant(QString::fromUtf8((const char*)m_row[pos]));
}


/* As with sqlite, the DB library returns all values (including numbers) as
   strings. So just put that string in a QVariant and let KexiDB deal with it.
 */
void MySqlCursor::storeCurrentRow(RowData &data) const {
	KexiDBDrvDbg << "MySqlCursor::storeCurrentRow: Position is " << (long)m_at<< endl;
	if (m_numRows<=0)
		return;

	data.reserve(m_fieldCount);
	for( uint i=0; i<m_fieldCount; i++) {
		data[i] = QVariant(m_row[i]);
	}
}

void MySqlCursor::drv_clearServerResult() {
}

void MySqlCursor::drv_appendCurrentRecordToBuffer() {
}


void MySqlCursor::drv_bufferMovePointerNext() {
	m_row=mysql_fetch_row(m_res);
	m_lengths=mysql_fetch_lengths(m_res);
}

void MySqlCursor::drv_bufferMovePointerPrev() {
	MYSQL_ROW_OFFSET ro=mysql_row_tell(m_res);
	mysql_data_seek(m_res,m_at-1);
	m_row=mysql_fetch_row(m_res);
	m_lengths=mysql_fetch_lengths(m_res);
}


void MySqlCursor::drv_bufferMovePointerTo(Q_LLONG to) {
	MYSQL_ROW_OFFSET ro=mysql_row_tell(m_res);
	mysql_data_seek(m_res,to);
	m_row=mysql_fetch_row(m_res);
	m_lengths=mysql_fetch_lengths(m_res);
}


const char** MySqlCursor::rowData() const {
	//! @todo
	return 0;
}



/*bool MySqlCursor::save(RowData& data, RowEditBuffer& buf)
{
	KexiDBDrvDbg << "MySqlCursor::save.." << endl;
	//! @todo
	return true;
}
*/
