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
	pqxxSqlCursor(Connection* conn, const QString& statement = QString::null, uint options = NoOptions );
	~pqxxSqlCursor();
        virtual bool drv_open(const QString& statement);
        virtual bool drv_close();
        virtual void drv_getNextRecord();
        virtual void drv_getPrevRecord();
	virtual QVariant value(int) const;
	virtual const char** recordData() const;
	virtual void storeCurrentRecord(RecordData &data) const;
	virtual void drv_clearServerResult();
	virtual void drv_appendCurrentRecordToBuffer();
	virtual void drv_bufferMovePointerNext();
	virtual void drv_bufferMovePointerPrev();
	virtual void drv_bufferMovePointerTo(Q_LLONG to);
	
private:
	pqxx::result* m_res;
	pqxx::nontransaction* m_tran;
	pqxx::connection* my_conn;
};

}

#endif
