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
        virtual bool drv_open();
        virtual bool drv_close();
        virtual bool drv_moveFirst();
        virtual bool drv_getNextRecord();
        virtual bool drv_getPrevRecord();
	virtual QVariant value(int) const;
	virtual const char** recordData() const;
	virtual void storeCurrentRecord(RecordData &data) const;

private:
	pqxx::result m_res;
	pqxx::transaction<pqxx::serializable>* m_tran;
	pqxx::Cursor* m_cur;
	unsigned int m_numFields;
};

}

#endif
