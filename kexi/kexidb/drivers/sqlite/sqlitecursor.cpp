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


#include <kexidb/error.h>

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <kdebug.h>
#include <klocale.h>

#include <qptrvector.h>

using namespace KexiDB;

class KexiDB::SQLiteCursorData
{
	public:
		SQLiteCursorData()
			:
//			: curr_cols(0)
			curr_coldata(0)
			, curr_colname(0)
			, rec_stored(false)
			, cols_pointers_mem_size(0)
			, records_in_buf(0)
			, buffering_completed(false)
			, at_buffer(false)
		{
		}

		QCString st;
		sqlite *data;
		sqlite_vm *vm;
		char *utail;
		char *errmsg;

//		int curr_cols;
		const char **curr_coldata;
		const char **curr_colname;

		int next_cols;
		const char **next_coldata;
//		const char **next_colname;

		bool rec_stored; //! true, current record is stored in next_coldata

		uint cols_pointers_mem_size;
		int records_in_buf; //! number of records currently stored in the buffer
		bool buffering_completed; //! true if we have already all records stored in the buffer
		QPtrVector<const char*> records; //buffer data
		bool at_buffer; //! true if we already point to the buffer with curr_coldata
/*		int prev_cols;
		const char **prev_coldata;
		const char **prev_colname;*/
};

SQLiteCursor::SQLiteCursor(Connection* conn, const QString& statement, uint options)
	: Cursor( conn, statement, options )
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

	if (isBuffered()) {
		m_data->records.resize(128); //TODO: manage size dynamically
	}

	return true;
}

bool SQLiteCursor::drv_getFirstRecord()
{
	bool ok = drv_getNextRecord();
/*	if ((m_options & Buffered) && ok) { //1st record is there:
		//compute parameters for cursor's buffer:
		//-size of record's array of pointer to values
		m_data->cols_pointers_mem_size = m_data->curr_cols * sizeof(char*);
		m_data->records_in_buf = 1;
	}*/
	return ok;
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
	int res = 0;

	if ((m_options & Buffered) && (m_at < (m_data->records_in_buf-1)) ) {
		//this cursor is buffered:
		//-we have next record already buffered:
		if (m_data->at_buffer) {//we already have got a pointer to buffer
			m_data->curr_coldata++; //just move to next record in the buffer
		} else {//we have no pointer
			//compute a place in the buffer that contain next record's data
			m_data->curr_coldata = m_data->records.at(m_at+1);
			m_data->at_buffer = true; //now current record is stored in the buffer
		}
	}
	else {//we need to physically fetch a record:
		if (!m_readAhead) {//we have no record that was read ahead
			res = sqlite_step(
				m_data->vm,
//				&m_data->curr_cols,
				&m_fieldCount,
				&m_data->curr_coldata,
				&m_data->curr_colname);

			if (m_data->curr_coldata) {
				for (int i=0;i<m_fieldCount;i++) {
					KexiDBDrvDbg<<"col."<< i<<": "<< m_data->curr_colname[i]<<" "<< m_data->curr_colname[m_fieldCount+i]
					 << " = " << (m_data->curr_coldata[i] ? QString::fromLocal8Bit(m_data->curr_coldata[i]) : "(NULL)") <<endl;
				}
			}
			if (res==SQLITE_ROW) {//we have the record
				if (m_options & Buffered) {
					//store this record's values in the buffer
					if (!m_data->cols_pointers_mem_size)
						m_data->cols_pointers_mem_size = m_fieldCount * sizeof(char*);
//						m_data->cols_pointers_mem_size = m_data->curr_cols * sizeof(char*);
					const char **record = (const char**)malloc(m_data->cols_pointers_mem_size);
					const char **src_col = m_data->curr_coldata;
					const char **dest_col = record;
//					for (int i=0; i<m_data->curr_cols; i++,src_col++,dest_col++) {
					for (int i=0; i<m_fieldCount; i++,src_col++,dest_col++) {
						*dest_col = strdup(*src_col);
					}
					m_data->records.insert(m_data->records_in_buf++,record);
				}
			} else {//there is no record
				if (m_options & Buffered) {
					m_data->buffering_completed = true; //no more records to buffer
				}
				KexiDBDrvDbg<<"res!=SQLITE_ROW ********"<<endl;
				m_validRecord = false;
				m_afterLast = true;
				m_at = -1;
				if (res==SQLITE_DONE)
					return false;
				//SQLITE_ERROR:
				setError(ERR_CURSOR_RECORD_FETCHING, I18N_NOOP("Cannot fetch a record with a cursor"));
				return false;
			}
		}
		else //we have a record that was read ahead: eat this
			m_readAhead = false;
	}

	m_at++;

	
//	if (m_data->curr_colname && m_data->curr_coldata)
//		for (int i=0;i<m_data->curr_cols;i++) {
//			KexiDBDrvDbg<<i<<": "<< m_data->curr_colname[i]<<" == "<< m_data->curr_coldata[i]<<endl;
//		}
	KexiDBDrvDbg<<"m_at == "<<(long)m_at<<endl;

	
	m_validRecord = true;
	return true;
}

bool SQLiteCursor::drv_getPrevRecord()
{
#ifndef Q_WS_WIN
#warning todo
#endif

		
	if (m_options & Buffered) {
		if ((m_at <= 0) || (m_data->records_in_buf <= 0)) {
			m_at=-1;
			m_beforeFirst = true;
			return false;
		}

		m_at--;
		if (m_data->at_buffer) {//we already have got a pointer to buffer
			m_data->curr_coldata--; //just move to prev record in the buffer
		} else {//we have no pointer
			//compute a place in the buffer that contain next record's data
			m_data->curr_coldata = m_data->records.at(m_at);
			m_data->at_buffer = true; //now current record is stored in the buffer
		}
		return true;
	}
//		setError( NOT_SUPPORTED,  )
	return false;
}

void SQLiteCursor::drv_clearBuffer()
{
//	if (!m_data->cols_pointers_mem_size || !m_data->curr_cols )
	if (!m_data->cols_pointers_mem_size || !m_fieldCount )
		return;
	const uint records_in_buf = m_data->records_in_buf;
	const char ***r_ptr = m_data->records.data();
	for (uint i=0; i<records_in_buf; i++, r_ptr++) {
//		const char **record = m_data->records.at(i);
		const char **field_data = *r_ptr;
//		for (int col=0; col<m_data->curr_cols; col++, field_data++) {
		for (int col=0; col<m_fieldCount; col++, field_data++) {
			free((void*)*field_data); //free field memory
		}
		free(*r_ptr); //free pointers to fields array
	}
//	m_data->curr_cols=0;
//	m_fieldCount=0;
	m_data->records_in_buf=0;
	m_data->cols_pointers_mem_size=0;
	m_data->at_buffer=false;
	m_data->records.clear();
}

void SQLiteCursor::drv_storeCurrentRecord()
{
#if 0
	assert(!m_data->rec_stored);
	m_data->rec_stored = true;
	m_data->next_cols = m_data->curr_cols;
	for (int i=0;i<m_data->curr_cols;i++) {
		KexiDBDrvDbg<<"[COPY] "<<i<<": "<< m_data->curr_coldata[i]<<endl;
		if (m_data->curr_coldata[i])
			m_data->next_coldata[i] = strdup( m_data->curr_coldata[i] );
		else
			m_data->next_coldata[i] = 0;
	}
#endif
}

/*TODO
const char *** SQLiteCursor::bufferData()
{
	if (!isBuffered())
		return 0;
	return m_data->records.data();
}*/

const char ** SQLiteCursor::recordData()
{
	return m_data->curr_coldata;
}


QVariant SQLiteCursor::value(int i)
{
//	if (i > (m_data->curr_cols-1)) //range checking
	if (i > (m_fieldCount-1)) //range checking
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
