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
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef _MYSQLCURSOR_H_
#define _MYSQLCURSOR_H_

#include <kexidb/cursor.h>
#include <kexidb/connection.h>

namespace KexiDB {

class MySqlCursorData;

class MySqlCursor: public Cursor {
public:
	MySqlCursor(Connection* conn, const QString& statement = QString(), 
		uint cursor_options = NoOptions );
	MySqlCursor(Connection* conn, QuerySchema& query, uint options = NoOptions );
	virtual ~MySqlCursor();
	virtual bool drv_open();
	virtual bool drv_close();
//        virtual bool drv_moveFirst();
	virtual void drv_getNextRecord();
        //virtual bool drv_getPrevRecord();
	virtual QVariant value(uint);

	virtual void drv_clearServerResult();
	virtual void drv_appendCurrentRecordToBuffer();
	virtual void drv_bufferMovePointerNext();
	virtual void drv_bufferMovePointerPrev();
	virtual void drv_bufferMovePointerTo(qint64 to);
	virtual const char** rowData() const;
	virtual bool drv_storeCurrentRow(RecordData &data) const;
//        virtual bool save(RecordData& data, RowEditBuffer& buf);

	virtual int serverResult();
	virtual QString serverResultName();
	virtual QString serverErrorMsg();

protected:
	QVariant pValue(uint pos) const;
//	MYSQL_RES *m_res;	
//	MYSQL_ROW m_row;
//	MYSQL *my_conn;
//	unsigned long *m_lengths;
//js: int m_numFields;
//	unsigned long m_numRows;
	MySqlCursorData *d;
};

}

#endif
