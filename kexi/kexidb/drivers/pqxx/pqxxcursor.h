//
// C++ Interface: pqxxcursor
//
// Description: 
//
//
// Author: Adam Pigg <piggz@defiant.piggz.co.uk>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KEXIDB_CURSOR_PQXX_H
#define KEXIDB_CURSOR_PQXX_H

#include <kexidb/cursor.h>
#include <kexidb/connection.h>

#include <pqxx/all.h>

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
        virtual bool drv_open(const QString& statement);
        virtual bool drv_close();
        virtual void drv_getNextRecord();
        virtual void drv_getPrevRecord();
			
private:
	pqxx::result* m_res;
	pqxx::nontransaction* m_tran;
	pqxx::connection* my_conn;
	QVariant pValue(uint pos) const;
	
	friend class pqxxSqlConnection;
};

}

#endif
