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

using namespace KexiDB;

MySqlCursor::MySqlCursor(KexiDB::Connection* conn, const QString& statement, uint cursor_options)
	: Cursor(conn,statement,cursor_options)
	, m_res(0)
	, m_row(0)
	, m_lengths(0)
	, m_numRows(0)
{
	m_options |= Buffered;
}

MySqlCursor::MySqlCursor(Connection* conn, QuerySchema& query, uint options )
	: Cursor( conn, query, options )
	, m_res(0)
	, m_row(0)
	, m_lengths(0)
	, m_numRows(0)
{
	m_options |= Buffered;
}

MySqlCursor::~MySqlCursor() {
	close();
}

bool MySqlCursor::drv_open(const QString& statement) {
	
	MySqlConnection *conn=(MySqlConnection*)m_conn;
	if ( (!conn)  || (!conn->m_mysql)) {
		//should never happen, but who knows
		setError(ERR_NO_CONNECTION,i18n("No connection for cursor open operation specified"));
		return false;
	}
        if(mysql_real_query(conn->m_mysql, statement.utf8(), strlen(statement.utf8())) == 0)
        {
                if(mysql_errno(conn->m_mysql) == 0) {
			m_res= mysql_store_result(conn->m_mysql);
			m_fieldCount=mysql_num_fields(m_res);
			m_numRows=mysql_num_rows(m_res);
			m_at=0;

			m_opened=true;
			m_afterLast=false;
                        return true;
                }
        }
	setError(ERR_DB_SPECIFIC,QString::fromUtf8(mysql_error(conn->m_mysql)));
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
        
bool MySqlCursor::drv_moveFirst() {
	return false; //TODO
}
       
void MySqlCursor::drv_getNextRecord() {
	m_row=mysql_fetch_row(m_res);
	if (m_row) {
		m_lengths=mysql_fetch_lengths(m_res);
		m_result=FetchOK;
		/*return true;*/
	} else {
		m_result=FetchEnd;
		/*return false;	*/
	}
	/*return true;*/
}



QVariant MySqlCursor::value(int pos) const {
	if (!m_row) return QVariant();
	if (pos>=m_fieldCount) return QVariant();
	if (m_row[pos]==0) return QVariant();
	return QVariant(QString::fromUtf8((const char*)m_row[pos]));
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
	mysql_data_seek(m_res,m_at);
	m_row=mysql_fetch_row(m_res);
	m_lengths=mysql_fetch_lengths(m_res);
}


void MySqlCursor::drv_bufferMovePointerTo(Q_LLONG to) {
	MYSQL_ROW_OFFSET ro=mysql_row_tell(m_res);
	mysql_data_seek(m_res,to);
	m_row=mysql_fetch_row(m_res);
	m_lengths=mysql_fetch_lengths(m_res);
}


const char** MySqlCursor::recordData() const {
	//! @todo
	return 0;
}

void MySqlCursor::storeCurrentRecord(RecordData &data) const {
}

