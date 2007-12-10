/* This file is part of the KDE project
   Copyright (C) 2007 Sharan Rao <sharanrao@gmail.com>

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

#include <QByteArray>
#include <QStringList>
#include <QApplication>
#include <QFile>

#include <KDebug>
#include <KTemporaryFile>

#include "sybaseconnection_p.h"

#include <kexidb/connectiondata.h>

#ifdef SYBASEMIGRATE_H
#define NAMESPACE KexiMigration
#else
#define NAMESPACE KexiDB
#endif

using namespace NAMESPACE;

QMap<DBPROCESS*, SybaseConnectionInternal*> SybaseConnectionInternal::dbProcessConnectionMap;


int connectionMessageHandler(DBPROCESS* dbproc, DBINT msgno, int msgstate, int severity, char* msgtext
                                                        , char* srvname, char* procname, int line)
{
    if ( !dbproc ) {
        return 0;
    }

    SybaseConnectionInternal* conn = SybaseConnectionInternal::dbProcessConnectionMap[dbproc];
    if ( conn )
      conn->messageHandler( msgno, msgstate, severity, msgtext, srvname, procname, line );

    return ( 0 );
}

/* ************************************************************************** */
SybaseConnectionInternal::SybaseConnectionInternal(KexiDB::Connection* connection)
	: ConnectionInternal(connection)
	, dbProcess(0)
	, res(0)
{
}

SybaseConnectionInternal::~SybaseConnectionInternal()
{
       if ( sybase_owned && dbProcess ) {
           dbclose( dbProcess );
           dbProcess = 0;
       }
}

void SybaseConnectionInternal::storeResult()
{
    //KexiDBDrvDbg << "Store Result!!" << endl;
    // all message numbers and message texts were handled in the messageHandler
    // so don't do anything here
}

void SybaseConnectionInternal::messageHandler(DBINT msgno, int msgstate, int severity, char* msgtext, char* srvname, char* procname, int line)
{

     Q_UNUSED( msgstate );
     Q_UNUSED( severity );
     Q_UNUSED( srvname );
     Q_UNUSED( procname );
     Q_UNUSED( line );

     res = msgno;
     errmsg = QString::fromLatin1( msgtext );

     KexiDBDrvDbg<<"Message Handler"<<res<<errmsg;
}

/* ************************************************************************** */
/*! Connects to the Sybase server on host as the given user using the specified
    password.  If host is "localhost", then a socket on the local file system
    can be specified to connect to the server (several defaults will be tried if
    none is specified).  If the server is on a remote machine, then a port is
    the port that the remote server is listening on.
 */
//bool SybaseConnectionInternal::db_connect(QCString host, QCString user,
//  QCString password, unsigned short int port, QString socket)
bool SybaseConnectionInternal::db_connect(const KexiDB::ConnectionData& data)
{
	if ( dbinit() == FAIL )
	      return false;

        // set message handler
        dbmsghandle( connectionMessageHandler );

	KexiDBDrvDbg << "SybaseConnectionInternal::connect()" << endl;
	QByteArray localSocket;
	QString hostName = data.hostName;


        if ( data.serverName.isEmpty() ) {
            KexiDBDrvDbg<<"Can't connect without server name";
            return false;
        }


        // set error handlers
        // set message handlers

        LOGINREC* login;

        login = dblogin();
        if ( login == NULL ) {
            //dbexit();
            return false;
        }



        // umm, copied from pqxx driver.
        if (hostName.isEmpty() || hostName.toLower()=="localhost") {
 		if (data.useLocalSocketFile) {
 			if (data.localSocketFileName.isEmpty()) {
 				QStringList sockets;
 	#ifndef Q_WS_WIN
 				sockets.append("/tmp/s.sybase.2638");

 				foreach (const QString& socket, sockets) {
 					if (QFile(socket).exists()) {
 						localSocket = socket.toLatin1();
 						break;
 					}
 				}
 	#endif
 			}
 			else
 				localSocket = QFile::encodeName(data.localSocketFileName);
 		}
 		else {
 			//we're not using local socket
                 	hostName = "127.0.0.1";
 		}
 	}


        KTemporaryFile confFile;
        confFile.setSuffix(".conf");
        confFile.open();

        QTextStream out( &confFile );

        // write global portion
        out<< "[global]"<<"\n";
        out<< " text size = "<<64512<<"\n" ; // Copied from default freetds.conf. is there a more reasonable number?


        // write server portion
        out<<'['<<data.serverName<<']'<<"\n";
        out<<" host = "<<hostName<<"\n";

        if ( data.port == 0 )
            out<<" port = "<<2638<<"\n"; // default port to be used
        else
            out<<" port = "<<data.port<<"\n";

        out<<" tds version = "<<5.0<<"\n";

        // set the file to be read as confFile
        dbsetifile( confFile.fileName().toLatin1().data() );

        // set Login parameters
	QByteArray pwd( data.password.isNull() ? QByteArray() : data.password.toLatin1() );

        DBSETLUSER( login, data.userName.toLatin1() );
        DBSETLPWD( login, pwd );
        DBSETLAPP( login, qApp->applicationName().toLatin1() );

        // make the connection
        // Host name assumed to be same as servername
        // where are ports specified ? ( in the interfaces file ? )

        dbProcess = dbopen( login, data.serverName.toLatin1().data() );

        dbloginfree( login );

        // Set/ Unset quoted identifier ? ?

	if( dbProcess != NULL ) {
                // add to map
                SybaseConnectionInternal::dbProcessConnectionMap[dbProcess] = this;

                // set buffering to be true
                // what's a reasonable value of no. of rows to be kept in buffer ?
                // dbsetopt( dbProcess, DBBUFFER, "500", -1 );

                // set quoted identifier to be true
                dbsetopt( dbProcess, DBQUOTEDIDENT, "1", -1 );

		return true;
        }

        storeResult();

        //dbexit();
//	setError(ERR_DB_SPECIFIC,err);
	return false;
}

/*! Disconnects from the database.
 */
bool SybaseConnectionInternal::db_disconnect()
{

        dbclose( dbProcess );
	dbProcess = 0;
	KexiDBDrvDbg << "SybaseConnection::disconnect()" << endl;
	return true;
}

/* ************************************************************************** */
/*! Selects dbName as the active database so it can be used.
 */
bool SybaseConnectionInternal::useDatabase(const QString &dbName)
{
       if ( dbuse( dbProcess, dbName.toLatin1().data() ) == SUCCEED ) {
           return true;
       }
//       return false;
       return true; // for testing
}

/*! Executes the given SQL statement on the server.
 */
bool SybaseConnectionInternal::executeSQL(const QString& statement)
{

	QByteArray queryStr( statement.toUtf8() );
	const char *query = queryStr.constData();

        // remove queries in buffer if any. flush existing results if any
        dbcancel( dbProcess );
        // put query in command bufffer
        dbcmd( dbProcess, query );
        if ( dbsqlexec( dbProcess ) == SUCCEED ) {
            while (dbresults( dbProcess ) != NO_MORE_RESULTS ) {
                /* nop */
            }
            return true;
        }

        // error handling

	storeResult();
	return false;
}

QString SybaseConnectionInternal::escapeIdentifier(const QString& str) const
{
	return QString(str).replace("'", "''");
}



//--------------------------------------

SybaseCursorData::SybaseCursorData(KexiDB::Connection* connection)
: SybaseConnectionInternal(connection)
, numRows(0)
{
    sybase_owned = false;
}

SybaseCursorData::~SybaseCursorData()
{
}

