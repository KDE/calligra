/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#include "sqlitecursor.h"

#include "sqliteconnection.h"
#include "sqliteconnection_p.h"

#include <assert.h>
#include <string.h>
#include <kdebug.h>

using namespace KexiDB;

class KexiDB::SQLiteCursorData
{
	public:
		SQLiteCursorData()
			: curr_cols(0)
			, curr_coldata(0)
			, curr_colname(0)
			, rec_stored(false)
		{}

		QCString st;
		sqlite *data;
		sqlite_vm *vm;
		char *utail;
		char *errmsg;

		int curr_cols;
		const char **curr_coldata;
		const char **curr_colname;

		int next_cols;
		const char **next_coldata;
//		const char **next_colname;

		bool rec_stored; //! true, current record is stored in next_coldata
/*		int prev_cols;
		const char **prev_coldata;
		const char **prev_colname;*/
};

SQLiteCursor::SQLiteCursor(Connection* conn, const QString& statement)
	: Cursor( conn, statement )
	, m_data( new SQLiteCursorData() )
{
	m_data->data = ((SQLiteConnection*)conn)->d->data;
}

SQLiteCursor::~SQLiteCursor()
{
	close();
}

bool SQLiteCursor::drv_open()
{
	m_data->st.resize(m_statement.length()*2);
	m_data->st = m_conn->escapeString( m_statement.local8Bit() );

	int res = sqlite_compile(
		m_data->data,
		m_data->st.data(),
		(const char**)&m_data->utail,
		&m_data->vm,
		&m_data->errmsg );
	if (res!=SQLITE_OK) {
		return false;
	}
//cursor is automatically @ first record
//	m_beforeFirst = true;

	return true;
}

bool SQLiteCursor::drv_close()
{
	int res = sqlite_finalize( m_data->vm, &m_data->errmsg );
	if (res!=SQLITE_OK) {
		return false;
	}
	return true;
}

////! nothing to do
//bool SQLiteCursor::drv_moveFirst()
//{
//	return true;
//}

bool SQLiteCursor::drv_getNextRecord()
{
//  const char *dd = m_data->curr_coldata ? m_data->curr_coldata[0] : 0;

	int res;
	if (!m_readAhead) {//we have no record that was read ahead
		res = sqlite_step(
			m_data->vm,
			&m_data->curr_cols,
			&m_data->curr_coldata,
			&m_data->curr_colname);
///	debug(">>>> %s", dd);

		for (int i=0;i<m_data->curr_cols;i++) {
			kdDebug()<<"column "<< i<<","<< m_data->curr_colname[i]<<","<< m_data->curr_colname[m_data->curr_cols+i]<<endl;
		}

		if (res!=SQLITE_ROW) {//there is no record
			kdDebug()<<"res!=SQLITE_ROW ********"<<endl;
			m_validRecord = false;
			m_afterLast = true;
			m_at = -1;
			return false;
		}
	}
	else //we have a record that was read ahead: eat this
		m_readAhead = false;

	m_at++;
	
	if (m_data->curr_colname && m_data->curr_coldata)
		for (int i=0;i<m_data->curr_cols;i++) {
//			kdDebug()<<i<<": "<< m_data->curr_colname[i]<<" == "<< m_data->curr_coldata[i]<<endl;
		}
	kdDebug()<<"m_at == "<<(long)m_at<<endl;
	m_validRecord = true;
	return true;
}

bool SQLiteCursor::drv_getPrevRecord()
{
#ifndef Q_WS_WIN
#warning todo
#endif
	return false;//rm this
}


void SQLiteCursor::drv_storeCurrentRecord()
{
#if 0
	assert(!m_data->rec_stored);
	m_data->rec_stored = true;
	m_data->next_cols = m_data->curr_cols;
	for (int i=0;i<m_data->curr_cols;i++) {
		kdDebug()<<"[COPY] "<<i<<": "<< m_data->curr_coldata[i]<<endl;
		if (m_data->curr_coldata[i])
			m_data->next_coldata[i] = strdup( m_data->curr_coldata[i] );
		else
			m_data->next_coldata[i] = 0;
	}
#endif
}

QVariant SQLiteCursor::value(int i)
{
	if (i > (m_data->curr_cols-1)) //range checking
		return QVariant();
//TODO: allow disable range checking! - performance reasons
	return QVariant( m_data->curr_coldata[i] );
}

//bool SQLiteCursor::moveLast()
//{
	//TODO
//	return true;
//}

/*bool SQLiteCursor::moveNext()
{
	//TODO
	return true;
}*/

/*
bool SQLiteCursor::eof()
{
	return m_afterLast;
}

int SQLiteCursor::at()
{
	if (m_readAhead)
		return 0;
	return m_at;
}

*/
