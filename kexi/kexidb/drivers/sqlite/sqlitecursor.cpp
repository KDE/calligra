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
#include <kexidb/driver.h>

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <kdebug.h>
#include <klocale.h>

#include <qptrvector.h>

using namespace KexiDB;

class KexiDB::SQLiteCursorData : public SQLiteConnectionInternal
{
	public:
		SQLiteCursorData()
			:
			SQLiteConnectionInternal()
//			: curr_cols(0)
//			errmsg_p(0)
//			, res(SQLITE_OK)
			, curr_coldata(0)
			, curr_colname(0)
			, cols_pointers_mem_size(0)
//			, rec_stored(false)
/* MOVED TO Cursor:
			, cols_pointers_mem_size(0)
			, records_in_buf(0)
			, buffering_completed(false)
			, at_buffer(false)*/
		{
		}

		QCString st;
		//for sqlite:
		sqlite *data; //! taken from SQLiteConnection
		sqlite_vm *vm;
		char *utail;
		
//		QString errmsg; //<! server-specific message of last operation
//		char *errmsg_p; //<! temporary: server-specific message of last operation
//		int res; //<! result code of last operation on server

//		int curr_cols;
		const char **curr_coldata;
		const char **curr_colname;

		int next_cols;
//		const char **next_coldata;
//		const char **next_colname;
//		bool rec_stored : 1; //! true, current record is stored in next_coldata

/* MOVED TO Cursor:
		uint cols_pointers_mem_size; //! size of record's array of pointers to values
		int records_in_buf; //! number of records currently stored in the buffer
		bool buffering_completed; //! true if we have already all records stored in the buffer
		QPtrVector<const char*> records; //buffer data
		bool at_buffer; //! true if we already point to the buffer with curr_coldata
*/

/*		int prev_cols;
		const char **prev_coldata;
		const char **prev_colname;*/
		
		uint cols_pointers_mem_size; //! size of record's array of pointers to values
		QPtrVector<const char*> records;//! buffer data
};



SQLiteCursor::SQLiteCursor(Connection* conn, const QString& statement, uint options)
	: Cursor( conn, statement, options )
	, d( new SQLiteCursorData() )
{
	d->data = ((SQLiteConnection*)conn)->d->data;
}

SQLiteCursor::SQLiteCursor(Connection* conn, QuerySchema& query, uint options )
	: Cursor( conn, query, options )
	, d( new SQLiteCursorData() )
{
	d->data = ((SQLiteConnection*)conn)->d->data;
}

SQLiteCursor::~SQLiteCursor()
{
	close();
	delete d;
}

bool SQLiteCursor::drv_open(const QString& statement)
{
//	d->st.resize(statement.length()*2);
	//TODO: decode
	d->st = statement.local8Bit();
//	d->st = m_conn->driver()->escapeString( statement.local8Bit() );

	d->res = sqlite_compile(
		d->data,
		d->st.data(),
		(const char**)&d->utail,
		&d->vm,
		&d->errmsg_p );
	d->storeResult();
	if (d->res!=SQLITE_OK) {
		return false;
	}
//cursor is automatically @ first record
//	m_beforeFirst = true;

	if (isBuffered()) {
		d->records.resize(128); //TODO: manage size dynamically
	}

	return true;
}

/*bool SQLiteCursor::drv_getFirstRecord()
{
	bool ok = drv_getNextRecord();*/
/*	if ((m_options & Buffered) && ok) { //1st record is there:
		//compute parameters for cursor's buffer:
		//-size of record's array of pointer to values
		d->cols_pointers_mem_size = d->curr_cols * sizeof(char*);
		d->records_in_buf = 1;
	}*/
	/*return ok;
}*/

bool SQLiteCursor::drv_close()
{
	d->res = sqlite_finalize( d->vm, &d->errmsg_p );
	d->storeResult();
	if (d->res!=SQLITE_OK) {
		return false;
	}
	return true;
}

void SQLiteCursor::drv_getNextRecord()
{
	static int _fieldCount;
	if ((d->res = sqlite_step(
	 d->vm,
	 &_fieldCount,
	 &d->curr_coldata,
	 &d->curr_colname))==SQLITE_ROW) {
		m_result = FetchOK;
		m_fieldCount = (uint)_fieldCount;
	} else if (d->res==SQLITE_DONE)
		m_result = FetchEnd;
	else
		m_result = FetchError;
	
	//debug
/*
	if (m_result == FetchOK && d->curr_coldata) {
		for (uint i=0;i<m_fieldCount;i++) {
			KexiDBDrvDbg<<"col."<< i<<": "<< d->curr_colname[i]<<" "<< d->curr_colname[m_fieldCount+i]
			<< " = " << (d->curr_coldata[i] ? QString::fromLocal8Bit(d->curr_coldata[i]) : "(NULL)") <<endl;
		}
//		KexiDBDrvDbg << "SQLiteCursor::drv_getNextRecord(): "<<m_fieldCount<<" col(s) fetched"<<endl;
	}*/
}

void SQLiteCursor::drv_appendCurrentRecordToBuffer()
{
//	KexiDBDrvDbg << "SQLiteCursor::drv_appendCurrentRecordToBuffer():" <<endl;
	if (!d->cols_pointers_mem_size)
		d->cols_pointers_mem_size = m_fieldCount * sizeof(char*);
	const char **record = (const char**)malloc(d->cols_pointers_mem_size);
	const char **src_col = d->curr_coldata;
	const char **dest_col = record;
	for (uint i=0; i<m_fieldCount; i++,src_col++,dest_col++) {
//		KexiDBDrvDbg << i <<": '" << *src_col << "'" <<endl;
//		KexiDBDrvDbg << "src_col: " << src_col << endl;
		*dest_col = *src_col ? strdup(*src_col) : 0;
	}
	d->records.insert(m_records_in_buf,record);
//	KexiDBDrvDbg << "SQLiteCursor::drv_appendCurrentRecordToBuffer() ok." <<endl;
}

void SQLiteCursor::drv_bufferMovePointerNext()
{
	d->curr_coldata++; //move to next record in the buffer
}

void SQLiteCursor::drv_bufferMovePointerPrev()
{
	d->curr_coldata--; //move to prev record in the buffer
}

//compute a place in the buffer that contain next record's data
//and move internal buffer pointer to that place
void SQLiteCursor::drv_bufferMovePointerTo(Q_LLONG at)
{
	d->curr_coldata = d->records.at(at);
}


#if 0 //old impl.
bool SQLiteCursor::drv_getNextRecord()
{
	m_data->res = -1;

	if ((m_options & Buffered) && (m_at < (m_records_in_buf-1)) ) {
		//this cursor is buffered:
		//-we have next record already buffered:
		if (m_at_buffer) {//we already have got a pointer to buffer
			m_data->curr_coldata++; //just move to next record in the buffer
		} else {//we have no pointer
			//compute a place in the buffer that contain next record's data
			m_data->curr_coldata = m_records.at(m_at+1);
			m_at_buffer = true; //now current record is stored in the buffer
		}
	}
	else {//we are after last retrieved record: we need to physically fetch a record:
		if (!m_readAhead) {//we have no record that was read ahead
			if (!m_buffering_completed) {
				//for buffered cursor: only retrieve record 
				//if we are not at after last buffer's item when buffer is fully filled
				KexiDBDrvDbg<<"==== sqlite_step ===="<<endl;
				m_data->res = sqlite_step(
					m_data->vm,
//					&m_data->curr_cols,
					&m_fieldCount,
					&m_data->curr_coldata,
					&m_data->curr_colname);
			}
			if (m_data->res!=SQLITE_ROW) {//there is no record
				if (m_options & Buffered) {
					m_buffering_completed = true; //no more records to buffer
				}
				KexiDBDrvDbg<<"res!=SQLITE_ROW ********"<<endl;
				m_validRecord = false;
				m_afterLast = true;
				m_at = -1;
				if (m_data->res==SQLITE_DONE) {
					return false;
				}
				//SQLITE_ERROR:
				setError(ERR_CURSOR_RECORD_FETCHING, I18N_NOOP("Cannot fetch a record with a cursor"));
				return false;
			}
			
			//we have a record
			if (m_data->curr_coldata) {
				for (int i=0;i<m_fieldCount;i++) {
					KexiDBDrvDbg<<"col."<< i<<": "<< m_data->curr_colname[i]<<" "<< m_data->curr_colname[m_fieldCount+i]
					<< " = " << (m_data->curr_coldata[i] ? QString::fromLocal8Bit(m_data->curr_coldata[i]) : "(NULL)") <<endl;
				}
			}
			if (m_options & Buffered) {
				//store this record's values in the buffer
				if (!m_cols_pointers_mem_size)
					m_cols_pointers_mem_size = m_fieldCount * sizeof(char*);
//						m_data->cols_pointers_mem_size = m_data->curr_cols * sizeof(char*);
				const char **record = (const char**)malloc(m_cols_pointers_mem_size);
				const char **src_col = m_data->curr_coldata;
				const char **dest_col = record;
//					for (int i=0; i<m_data->curr_cols; i++,src_col++,dest_col++) {
				for (int i=0; i<m_fieldCount; i++,src_col++,dest_col++) {
					*dest_col = strdup(*src_col);
				}
				m_records.insert(m_records_in_buf++,record);
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
		if ((m_at <= 0) || (m_records_in_buf <= 0)) {
			m_at=-1;
			m_beforeFirst = true;
			return false;
		}

		m_at--;
		if (m_at_buffer) {//we already have got a pointer to buffer
			m_data->curr_coldata--; //just move to prev record in the buffer
		} else {//we have no pointer
			//compute a place in the buffer that contain next record's data
			m_data->curr_coldata = m_records.at(m_at);
			m_at_buffer = true; //now current record is stored in the buffer
		}
		return true;
	}
//		setError( NOT_SUPPORTED,  )
	return false;
}
#endif

void SQLiteCursor::drv_clearBuffer()
{
	if (d->cols_pointers_mem_size>0) {
		const uint records_in_buf = m_records_in_buf;
		const char ***r_ptr = d->records.data();
		for (uint i=0; i<records_in_buf; i++, r_ptr++) {
	//		const char **record = m_records.at(i);
			const char **field_data = *r_ptr;
	//		for (int col=0; col<d->curr_cols; col++, field_data++) {
			for (uint col=0; col<m_fieldCount; col++, field_data++) {
				free((void*)*field_data); //free field memory
			}
			free(*r_ptr); //free pointers to fields array
		}
	}
//	d->curr_cols=0;
//	m_fieldCount=0;
	m_records_in_buf=0;
	d->cols_pointers_mem_size=0;
//	m_at_buffer=false;
	d->records.clear();
}

/*
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
*/

/*TODO
const char *** SQLiteCursor::bufferData()
{
	if (!isBuffered())
		return 0;
	return m_records.data();
}*/

const char ** SQLiteCursor::rowData() const
{
	return d->curr_coldata;
}

void SQLiteCursor::storeCurrentRow(RowData &data) const
{
	const char **col = d->curr_coldata;
	data.reserve(m_fieldCount);
	if (!m_fieldsExpanded) {//simply version: without types
		for( uint i=0; i<m_fieldCount; i++, col++ ) {
			data[i] = QVariant( *col );
		}
		return;
	}

	const uint fieldsExpandedCount = m_fieldsExpanded->count();
	for( uint i=0, j=0; i<m_fieldCount; i++, col++, j++ ) {
//		while (j < m_detailedVisibility.count() && !m_detailedVisibility[j]) //!m_query->isColumnVisible(j))
			//j++;
		while (j < fieldsExpandedCount && !m_fieldsExpanded->at(j)->visible)
			j++;
		if (j >= fieldsExpandedCount) {
			//ERR!
			break;
		}
//		KexiDB::Field *f = m_fieldsExpanded->at(j);
		KexiDB::Field *f = m_fieldsExpanded->at(j)->field;
		KexiDBDrvDbg << "SQLiteCursor::storeCurrentRow(): col=" << (col ? *col : 0) << endl;

		if (!*col)
			data[i] = QVariant();
		else if (f->isTextType())
#ifdef SQLITE_UTF8
			data[i] = QString::fromUtf8( *col );
#else
			data[i] = QVariant( *col ); //only latin1
#endif
		else if (f->isIntegerType())
			data[i] = QVariant( QCString(*col).toInt() );
		else if (f->isFPNumericType())
			data[i] = QVariant( QCString(*col).toDouble() );
		else if (f->type()==KexiDB::Field::Boolean)
			data[i] = QVariant( QCString(*col).toInt()==0, 0 );
		else
			data[i] = QVariant( *col ); //default
	}
}

QVariant SQLiteCursor::value(uint i)
{
	if (i > (m_fieldCount-1)) //range checking
		return QVariant();
//TODO: allow disable range checking! - performance reasons
//	const KexiDB::Field *f = m_query ? m_query->field(i) : 0;
	KexiDB::Field *f = m_fieldsExpanded ? m_fieldsExpanded->at(i)->field : 0;
	//from most to least frequently used types:
	if (!f || f->isTextType())
		return QVariant( d->curr_coldata[i] );
	else if (f->isIntegerType())
		return QVariant( QCString(d->curr_coldata[i]).toInt() );
	else if (f->isFPNumericType())
		return QVariant( QCString(d->curr_coldata[i]).toDouble() );

	return QVariant( d->curr_coldata[i] ); //default
}

/*! Stores string value taken from field number \a i to \a str.
 \return false when range checking failed. 
bool SQLiteCursor::storeStringValue(uint i, QString &str)
{
	if (i > (m_fieldCount-1)) //range checking
		return false;
	str = d->curr_coldata[i];
	return true;
}*/

int SQLiteCursor::serverResult() const
{
	return d->res;
}

QString SQLiteCursor::serverResultName() const
{
	return QString::fromLatin1( sqlite_error_string(d->res) );
}

QString SQLiteCursor::serverErrorMsg() const
{
	return d->errmsg;
}

void SQLiteCursor::drv_clearServerResult()
{
	d->res = SQLITE_OK;
	d->errmsg_p = 0;
}

