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

/**
@author Adam Pigg
*/
namespace KexiDB
{
class pqxxSqlConnection : public Connection
{
	Q_OBJECT

        public:
                ~pqxxSqlConnection();

                virtual Cursor* prepareQuery( const QString& statement = QString::null, uint cursor_options = 0 );
                virtual void escapeString( const QString& str, char *target );
                virtual QString escapeString( const QString& str) const;
                virtual QCString escapeString( const QCString& str) const;

	protected:

                pqxxSqlConnection( Driver *driver, const ConnectionData &conn_data );

                virtual bool drv_connect();
                virtual bool drv_disconnect();
                virtual bool drv_getDatabasesList( QStringList &list );
                virtual bool drv_createDatabase( const QString &dbName = QString::null );
                virtual bool drv_useDatabase( const QString &dbName = QString::null );
                virtual bool drv_closeDatabase();
                virtual bool drv_dropDatabase( const QString &dbName = QString::null );
                virtual bool drv_executeSQL( const QString& statement );
		virtual bool drv_databaseExists( const QString &dbName );
		pqxx::connection* m_pqxxsql;

	private:
		void clearResultInfo();
		bool isConnected();
		QString escapeName(const QString &tn) const;

		pqxx::result* m_res;
  		pqxx::transaction_base* m_trans;

	friend class pqxxSqlDriver;
	friend class pqxxSqlCursor;
};
}
#endif
