//
// C++ Implementation: pqxxconnection
//
// Description:
//
//
// Author: Adam Pigg <piggz@defiant.piggz.co.uk>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "pqxxconnection.h"
#include <qvariant.h>
#include <qfile.h>
#include <kdebug.h>
#include <kexidb/error.h>
#include <klocale.h>
#include <string>

using namespace KexiDB;

pqxxSqlConnection::pqxxSqlConnection(Driver *driver, const ConnectionData &conn_data)
        :Connection(driver,conn_data)
        ,m_pqxxsql(0), m_res(0), m_trans(0)
{
}

pqxxSqlConnection::~pqxxSqlConnection()
{
}

Cursor* pqxxSqlConnection::prepareQuery( const QString& statement,  uint cursor_options)
{
}

void pqxxSqlConnection::escapeString( const QString& str, char *target )
{}

QString pqxxSqlConnection::escapeString( const QString& str) const
{
}

QCString pqxxSqlConnection::escapeString( const QCString& str) const
{
}

bool pqxxSqlConnection::drv_connect()
{
    QString conninfo;
    QString socket;
    QStringList sockets;

    if (m_is_connected)
    {
        setError(ERR_ALREADY_CONNECTED,i18n("Connection has already been opened"));
        return false;
    }

    kdDebug() << "pqxxSqlConnection::connect()" << endl;

    if (m_data.hostName.isEmpty() || (m_data.hostName=="localhost"))
    {
        if (m_data.fileName().isEmpty())
        {
            sockets.append("/tmp/.s.PGSQL.5432");

            for(QStringList::Iterator it = sockets.begin(); it != sockets.end(); it++)
            {
                if(QFile(*it).exists())
                {
                    socket = (*it);
                    break;
                }
            }
        }
        else    if (m_data.port != 0)
	{
            socket=m_data.fileName();
        }
	//conninfo = "host='" + socket + "'";
    }
    else
    {
        conninfo = "host='" + m_data.hostName + "'";
    }

    //Build up the connection string
    if (m_data.port == 0)
    	m_data.port = 5432;

    conninfo += QString::fromLatin1(" port='%1'").arg(m_data.port);

    conninfo += QString::fromLatin1(" dbname='template1'");

    if (!m_data.userName.isNull())
        conninfo += QString::fromLatin1(" user='%1'").arg(m_data.userName);

    if (!m_data.password.isNull())
        conninfo += QString::fromLatin1(" password='%1'").arg(m_data.password);

    kdDebug() << conninfo << endl;

    try
    {
        m_pqxxsql = new pqxx::connection( conninfo.latin1() );
        m_is_connected=true;
	return true;
    }
    catch(const std::exception &e)
    {
        kdDebug() << "pqxxSqlDB::connect:exception - " << e.what() << endl;
	setError(ERR_DB_SPECIFIC,e.what());
        return false;
    }
}

bool pqxxSqlConnection::drv_disconnect()
{}

bool pqxxSqlConnection::drv_getDatabasesList( QStringList &list )
{
    if (drv_executeSQL("SELECT datname FROM pg_database WHERE datallowconn = TRUE"))
    {
        std::string N;
        for (pqxx::result::const_iterator c = m_res->begin(); c != m_res->end(); ++c)
        {
            // Read value of column 0 into a string N
            c[0].to(N);

            // Copy the result into the return list
            list << QString::fromLatin1 (N.c_str());
        }
    return true;
    }

    return false;
}

bool pqxxSqlConnection::drv_createDatabase( const QString &dbName )
{
kdDebug() << "pqxxSqlConnection::drv_createDatabase: " << dbName << endl;
}

bool pqxxSqlConnection::drv_useDatabase( const QString &dbName )
{
kdDebug() << "pqxxSqlConnection::drv_useDatabase: " << dbName << endl;
}

bool pqxxSqlConnection::drv_closeDatabase()
{
kdDebug() << "pqxxSqlConnection::drv_closeDatabasetabase: " << endl;
}

bool pqxxSqlConnection::drv_dropDatabase( const QString &dbName )
{
kdDebug() << "pqxxSqlConnection::drv_dropDatabase: " << dbName << endl;
}

bool pqxxSqlConnection::drv_executeSQL( const QString& statement )
{
kdDebug() << "pqxxSqlConnection::drv_executeSQL: " << statement << endl;

    // Clear the last result information...
    clearResultInfo ();

    try
    {
        //Create a transaction
        m_trans = new pqxx::nontransaction(*m_pqxxsql);

        //Create a result opject through the transaction
        m_res = new pqxx::result(m_trans->exec(statement.utf8()));

        //Commit the transaction
        m_trans->commit();

        //If all went well then return true, errors picked up by the catch block
        return true;
    }
    catch (const std::exception &e)
    {
        //If an error ocurred then put the error description into _dbError
        setError(ERR_DB_SPECIFIC,e.what());
        kdDebug() << "pqxxSqlConnection::drv_executeSQL:exception - " << e.what() << endl;
        return false;
    }
}

bool pqxxSqlConnection::drv_databaseExists( const QString &dbName )
{
kdDebug() << "pqxxSqlConnection::drv_databaseExists: " << dbName << endl;
}

//Private Functions I Need
//=========================================================================
//Clears the current result
void pqxxSqlConnection::clearResultInfo ()
{
    if (m_res)
    {
        delete m_res;
        m_res = 0;
    }
    if (m_trans)
    {
        delete m_trans;
        m_trans = 0;
    }
}