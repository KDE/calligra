#ifndef KEXIDB_DRIVER_PQXX_H
#define KEXIDB_DRIVER_PQXX_H

#include <qstringlist.h>

#include <kexidb/driver.h>

namespace KexiDB
{

class Connection;
class DriverManager;

class KEXI_DB_EXPORT pqxxSqlDriver : public Driver
{
	Q_OBJECT

	public:
		pqxxSqlDriver( QObject *parent, const char *name, const QStringList &args = QStringList() );
		~pqxxSqlDriver();

		virtual bool isSystemObjectName( const QString& n )const;
		virtual bool isSystemFieldName( const QString& n )const;

	protected:
		virtual Connection *drv_createConnection( ConnectionData &conn_data );

	private:

};

};

#endif
