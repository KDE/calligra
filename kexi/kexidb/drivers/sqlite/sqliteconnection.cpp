/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "sqliteconnection.h"
#include "sqliteconnection_p.h"
#include "sqlitecursor.h"

#include "sqlite.h"

#include <kexidb/driver.h>
#include <kexidb/cursor.h>
#include <kexidb/error.h>

#include <qfile.h>
#include <qdir.h>

#include <kgenericfactory.h>
#include <kdebug.h>

//remove debug
#undef KexiDBDrvDbg
#define KexiDBDrvDbg if (0) kdDebug()

using namespace KexiDB;

SQLiteConnectionInternal::SQLiteConnectionInternal()
	: data(0),errmsg_p(0),res(SQLITE_OK)
	, temp_st(0x10000) //
{
}

SQLiteConnectionInternal::~SQLiteConnectionInternal() 
{
	if (data) {
		free( data ); 
		data = 0;
	}
//sqlite_freemem does this	if (errmsg) {
//		free( errmsg );
//		errmsg = 0;
//	}
}

void SQLiteConnectionInternal::storeResult()
{
	if (errmsg_p) {
		errmsg = errmsg_p;
		sqlite_freemem(errmsg_p);
		errmsg_p = 0;
	}
}

/*! Used by driver */
SQLiteConnection::SQLiteConnection( Driver *driver, ConnectionData &conn_data )
	: Connection( driver, conn_data )
	,d(new SQLiteConnectionInternal())
{
}

SQLiteConnection::~SQLiteConnection()
{
	KexiDBDrvDbg << "SQLiteConnection::~SQLiteConnection()" << endl;
	//disconnect if was connected
//	disconnect();
	destroy();
	delete d;
	KexiDBDrvDbg << "SQLiteConnection::~SQLiteConnection() ok" << endl;
}

bool SQLiteConnection::drv_connect()
{
	KexiDBDrvDbg << "SQLiteConnection::connect()" << endl;
	return true;
}

bool SQLiteConnection::drv_disconnect()
{
	KexiDBDrvDbg << "SQLiteConnection::disconnect()" << endl;
	return true;
}

bool SQLiteConnection::drv_getDatabasesList( QStringList &list )
{
	//this is one-db-per-file database
	list.append( m_data->dbFileName() );
	return true;
}

bool SQLiteConnection::drv_createDatabase( const QString &/*dbName*/ )
{
	d->data = sqlite_open( QFile::encodeName( m_data->fileName() ), 0/*mode: unused*/, 
		&d->errmsg_p );
	d->storeResult();
	return d->data != 0;
}

bool SQLiteConnection::drv_useDatabase( const QString &/*dbName*/ )
{
	d->data = sqlite_open( QFile::encodeName( m_data->fileName() ), 0/*mode: unused*/, 
		&d->errmsg_p );
	d->storeResult();
	return d->data != 0;
}

bool SQLiteConnection::drv_closeDatabase()
{
	if (!d->data)
		return false;
	sqlite_close(d->data);
	d->data = 0;
	return true;
}

bool SQLiteConnection::drv_dropDatabase( const QString &dbName )
{
	if (QFile(m_data->fileName()).exists() && !QDir().remove(m_data->fileName())) {
		setError(ERR_ACCESS_RIGHTS, i18n("Could not remove file \"%1\".").arg(dbName) );
		return false;
	}
	return true;
}

//CursorData* SQLiteConnection::drv_createCursor( const QString& statement )
Cursor* SQLiteConnection::prepareQuery( const QString& statement, uint cursor_options )
{
	return new SQLiteCursor( this, statement, cursor_options );
}

Cursor* SQLiteConnection::prepareQuery( QuerySchema& query, uint cursor_options )
{
	return new SQLiteCursor( this, query, cursor_options );
}

bool SQLiteConnection::drv_executeSQL( const QString& statement )
{
	KexiDBDrvDbg << "SQLiteConnection::drv_executeSQL(" << statement << ")" <<endl;
//	QCString st(statement.length()*2);
//	st = escapeString( statement.local8Bit() ); //?
#ifdef SQLITE_UTF8
	d->temp_st = statement.utf8();
#else
	d->temp_st = statement.local8Bit(); //latin1 only
#endif

	d->res = sqlite_exec( 
		d->data, 
		(const char*)d->temp_st, 
		0/*callback*/, 
		0,
		&d->errmsg_p );
	d->storeResult();
	return d->res==SQLITE_OK;
}

Q_ULLONG SQLiteConnection::drv_lastInsertRowID()
{
	return (Q_ULLONG)sqlite_last_insert_rowid(d->data);
}

int SQLiteConnection::serverResult()
{
	return d->res;
}

QString SQLiteConnection::serverResultName()
{
	return QString::fromLatin1( sqlite_error_string(d->res) );
}

void SQLiteConnection::drv_clearServerResult()
{
	if (!d)
		return;
	d->res = SQLITE_OK;
	d->errmsg_p = 0;
}

QString SQLiteConnection::serverErrorMsg()
{
	return d->errmsg;
}

#include "sqliteconnection.moc"
