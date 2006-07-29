/* This file is part of the KDE project
   Copyright (C) 2003 Adam Pigg <adam@piggz.co.uk>

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

#ifndef KEXIDB_CURSOR_PQXX_H
#define KEXIDB_CURSOR_PQXX_H

#include <kexidb/cursor.h>
#include <kexidb/connection.h>

#if 0
#include <pqxx/all.h>
#else
#include <pqxx/pqxx>
#endif

#include <pqxx/binarystring>

namespace KexiDB {

class pqxxSqlCursor: public Cursor {
public:
	virtual ~pqxxSqlCursor();

	virtual QVariant value(uint i);
	virtual const char** rowData() const;
	virtual void storeCurrentRow(RowData &data) const;

//TODO		virtual const char *** bufferData()
		
//TODO	virtual int serverResult() const;
		
//TODO	virtual QString serverResultName() const;

//TODO	virtual QString serverErrorMsg() const;
		
protected:
	pqxxSqlCursor(Connection* conn, const QString& statement = QString::null, uint options = NoOptions );
	pqxxSqlCursor(Connection* conn, QuerySchema& query, uint options = NoOptions );
	virtual void drv_clearServerResult();
	virtual void drv_appendCurrentRecordToBuffer();
	virtual void drv_bufferMovePointerNext();
	virtual void drv_bufferMovePointerPrev();
	virtual void drv_bufferMovePointerTo(Q_LLONG to);
	virtual bool drv_open();
	virtual bool drv_close();
	virtual void drv_getNextRecord();
	virtual void drv_getPrevRecord();

private:
	pqxx::result* m_res;
//	pqxx::nontransaction* m_tran;
	pqxx::connection* my_conn;
	QVariant pValue(uint pos)const;
	bool m_implicityStarted : 1;
	//QByteArray processBinaryData(pqxx::binarystring*) const;
	friend class pqxxSqlConnection;
};

}

#endif
