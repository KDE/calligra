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
	: Cursor(conn,statement,cursor_options), m_res(0),m_row(0),
	m_lengths(0),m_numRows(0)
{
}

MySqlCursor::~MySqlCursor() {
	close();
}

bool MySqlCursor::drv_open() {
	
	MySqlConnection *conn=(MySqlConnection*)m_conn;
	if ( (!conn)  || (!conn->m_mysql)) {
		//should never happen, but who knows
		setError(ERR_NO_CONNECTION,i18n("No connection for cursor open operation specified"));
		return false;
	}
        if(mysql_real_query(conn->m_mysql, m_statement.utf8(), strlen(m_statement.utf8())) == 0)
        {
                if(mysql_errno(conn->m_mysql) == 0) {
			m_res= mysql_store_result(conn->m_mysql);
			m_fieldCount=mysql_num_fields(m_res);
			m_readAhead=(m_numRows=mysql_num_rows(m_res));
			m_at=0;
/*
                        MYSQL_RES *res;
                        res = mysql_use_result(m_mysql);
                        MYSQL_ROW  row;
                        while ( (row = mysql_fetch_row(res))!=0) {
                                //(void) mysql_fetch_lengths(m_res);
                                list<<QString(row[0]);
                        }*
                        mysql_free_result(res);*/
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
       
bool MySqlCursor::drv_getNextRecord() {
	m_row=mysql_fetch_row(m_res);
	if (m_row) {
		m_at++;
		m_beforeFirst=false;
		m_validRecord=true;
		m_afterLast=false;
		m_lengths=mysql_fetch_lengths(m_res);
		return true;
	} else {
		m_at=m_numRows+1;
		m_validRecord = false;
		m_afterLast = true;
		return false;	
	}
	return true;
}

bool MySqlCursor::drv_getPrevRecord() {
	MYSQL_ROW_OFFSET ro=mysql_row_tell(m_res);
	if (m_at>1) {
		m_at--;
		mysql_data_seek(m_res,m_at-1);
		m_row=mysql_fetch_row(m_res);
		m_beforeFirst=false;
		m_validRecord=true;
		m_afterLast=false;
		m_lengths=mysql_fetch_lengths(m_res);
		return true;
	}
	m_at=0;
	m_validRecord=false;
	m_afterLast=false;
	m_beforeFirst=true;
	return false;
}



QVariant MySqlCursor::value(int pos) {
	if (!m_row) return QVariant();
	if (pos>=m_fieldCount) return QVariant();
	if (m_row[pos]==0) return QVariant();
	return QVariant(QString::fromUtf8((const char*)m_row[pos]));
}
