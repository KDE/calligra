//
// C++ Interface: pqxxconnection
//
// Description: 
//
//
// Author: Adam Pigg <piggz@defiant.piggz.co.uk>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PQXXCONNECTION_H
#define PQXXCONNECTION_H

#include <qstringlist.h>

#include <kexidb/connection.h>
#include "pqxxcursor.h"
#include <pqxx/all.h>


namespace KexiDB
{

/**
@author Adam Pigg
*/
class pqxxSqlConnection : public Connection
{
	Q_OBJECT

        public:
                ~pqxxSqlConnection();

                virtual Cursor* prepareQuery( const QString& statement = QString::null, uint cursor_options = 0 );
		virtual Cursor* prepareQuery( QuerySchema& query, uint cursor_options = 0 );

	protected:

                pqxxSqlConnection( Driver *driver, ConnectionData &conn_data );

		virtual bool drv_isDatabaseUsed() const;
                virtual bool drv_connect();
                virtual bool drv_disconnect();
                virtual bool drv_getDatabasesList( QStringList &list );
                virtual bool drv_createDatabase( const QString &dbName = QString::null );
                virtual bool drv_useDatabase( const QString &dbName = QString::null );
                virtual bool drv_closeDatabase();
                virtual bool drv_dropDatabase( const QString &dbName = QString::null );
                virtual bool drv_executeSQL( const QString& statement );
		virtual Q_ULLONG drv_lastInsertRowID();

//TODO: move this somewhere to low level class (MIGRATION?)
		virtual bool drv_getTablesList( QStringList &list );
//TODO: move this somewhere to low level class (MIGRATION?)
		virtual bool drv_containsTable( const QString &tableName );

		pqxx::connection* m_pqxxsql;
	private:
		void clearResultInfo();
		QString escapeName(const QString &tn) const;

		pqxx::result* m_res;
  		pqxx::transaction_base* m_trans;

	friend class pqxxSqlDriver;
	friend class pqxxSqlCursor;
};
}
#endif
