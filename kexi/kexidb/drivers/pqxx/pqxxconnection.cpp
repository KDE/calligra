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
    m_is_connected = false;
}

//==================================================================================
//Do any tidying up before the object is deleted
pqxxSqlConnection::~pqxxSqlConnection()
{
    destroy();
}

//==================================================================================
//
Cursor* pqxxSqlConnection::prepareQuery( const QString& statement,  uint cursor_options)
{
return new pqxxSqlCursor(this, statement, cursor_options);
}

//==================================================================================
//
void pqxxSqlConnection::escapeString( const QString& str, char *target )
{}

//==================================================================================
//
QString pqxxSqlConnection::escapeString( const QString& str) const
    {}

//==================================================================================
//
QCString pqxxSqlConnection::escapeString( const QCString& str) const
    {}

//==================================================================================
//
QString pqxxSqlConnection::escapeName(const QString &name) const
{
    return QString("\"" + name + "\"");
}

//Made this a noop
//We tell kexi we are connected, but we wont actually connect until we use a database!
bool pqxxSqlConnection::drv_connect()
{
    kdDebug() << "pqxxSqlConnection::drv_connect" << endl;
    return true;
}

//==================================================================================
//Made this a noop
//We tell kexi wehave disconnected, but it is actually handled by closeDatabse
bool pqxxSqlConnection::drv_disconnect()
{
    kdDebug() << "pqxxSqlConnection::drv_disconnect: " << endl;
    return true;
}

//==================================================================================
//We need to check if we are connected...most likely we will have to open a connection to template1
//to get the list of databases
bool pqxxSqlConnection::drv_getDatabasesList( QStringList &list )
{
    kdDebug() << "pqxxSqlConnection::drv_getDatabaseList" << endl;

    if(!isConnected())
    {
        kdDebug() << "pqxxSqlConnection::drv_getDatabaseList: Not connected" << endl;
        if (!drv_useDatabase("template1"))
        {
            return false;
        }
    }

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

//==================================================================================
//
bool pqxxSqlConnection::drv_createDatabase( const QString &dbName )
{
    kdDebug() << "pqxxSqlConnection::drv_createDatabase: " << dbName << endl;

    if (drv_executeSQL("CREATE DATABASE " + escapeName(dbName)))
    	return true;

    return false;
}

//==================================================================================
//Use this as out connection instead of connect
bool pqxxSqlConnection::drv_useDatabase( const QString &dbName )
{
    kdDebug() << "pqxxSqlConnection::drv_useDatabase: " << dbName << endl;

    QString conninfo;
    QString socket;
    QStringList sockets;

    if (isConnected())
    {
	if(!drv_closeDatabase())
	{
        	setError(ERR_ALREADY_CONNECTED,i18n("Connection has already been opened and couldnt disconnect"));
        	return false;
	}
    }

    if (m_data.hostName.isEmpty() || m_data.hostName == "localhost")
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
        else
        {
            socket=m_data.fileName();
        }
    }
    else
    {
        conninfo = "host='" + m_data.hostName + "'";
    }

    //Build up the connection string
    if (m_data.port == 0)
        m_data.port = 5432;

    conninfo += QString::fromLatin1(" port='%1'").arg(m_data.port);

    conninfo += QString::fromLatin1(" dbname='%1'").arg(dbName);

    if (!m_data.userName.isNull())
        conninfo += QString::fromLatin1(" user='%1'").arg(m_data.userName);

    if (!m_data.password.isNull())
        conninfo += QString::fromLatin1(" password='%1'").arg(m_data.password);

    try
    {
        m_pqxxsql = new pqxx::connection( conninfo.latin1() );
        m_is_connected=true;
        return true;
    }
    catch(const std::exception &e)
    {
        kdDebug() << "pqxxSqlConnection::drv_useDatabase:exception - " << e.what() << endl;
        setError(ERR_DB_SPECIFIC,e.what());

    }
    return false;
}

//==================================================================================
//Here we close the database connection
bool pqxxSqlConnection::drv_closeDatabase()
{
    kdDebug() << "pqxxSqlConnection::drv_closeDatabase" << endl;
    if (isConnected())
    {
        delete m_pqxxsql;
        m_pqxxsql = 0;
        m_is_connected = false;
        return true;
    }
    else
    {
        setError(ERR_NO_CONNECTION, "Not connected to database backend");
    }
    return false;
}

//==================================================================================
//
bool pqxxSqlConnection::drv_dropDatabase( const QString &dbName )
{
    kdDebug() << "pqxxSqlConnection::drv_dropDatabase: " << dbName << endl;

    if (drv_executeSQL("DROP DATABASE " + escapeName(dbName)))
    	return true;

    return false;

}

//==================================================================================
//Execute an SQL statement
bool pqxxSqlConnection::drv_executeSQL( const QString& statement )
{
    kdDebug() << "pqxxSqlConnection::drv_executeSQL: " << statement << endl;

    // Clear the last result information...
    clearResultInfo ();

    //Exit if we are not connected
    if (!m_is_connected)
        return false;

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

//==================================================================================
//Check if a database exists
bool pqxxSqlConnection::drv_databaseExists( const QString &dbName )
{
    kdDebug() << "pqxxSqlConnection::drv_databaseExists: " << dbName << endl;
    QStringList databases;

    if (drv_getDatabasesList(databases))
    {
        for (QStringList::iterator it = databases.begin(); it != databases.end() ; ++it)
        {
            kdDebug() << *it << endl;
            if (*it == dbName)
                return true;
        }

    }
    else
    {
        kdDebug() << "pqxxSqlConnection::drv_databaseExists:Could not get list of databases" << endl;
    }
    return false;
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

//==================================================================================
//We cannot trust m_is_connected as it is set in conection.cpp
bool pqxxSqlConnection::isConnected()
{
    if((!m_pqxxsql) || (!m_pqxxsql->is_open()))
    {
        return false;
    }
    return true;
}
