/* This file is part of the KDE project
   Copyright (C) 2008 Sharan Rao <sharanrao@gmail.com>

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
*  Boston, MA 02110-1301, USA.
*/

#include <QVariant>
#include <QFile>
#include <QRegExp>

#include <kgenericfactory.h>
#include <kdebug.h>

#include "xbasedriver.h"
#include "xbasecursor.h"
#include "xbaseconnection.h"
#include "xbaseconnection_p.h"
#include <kexidb/error.h>


using namespace KexiDB;


xBaseConnection::xBaseConnection( Driver *driver, Driver* internalDriver, ConnectionData &conn_data )
  :Connection(driver,conn_data)
  ,d(new xBaseConnectionInternal(this, internalDriver))
{
}

xBaseConnection::~xBaseConnection() {
  destroy();
}

bool xBaseConnection::drv_connect(KexiDB::ServerVersionInfo& version)
{
  const bool ok = d->db_connect(*data());
  if (!ok)
    return false;

  //! TODO xBase version here
  //version.string = mysql_get_host_info(d->mysql);

  return true;
}

bool xBaseConnection::drv_disconnect() {
  return d->db_disconnect(*data());
}

Cursor* xBaseConnection::prepareQuery(const QString& statement, uint cursor_options) {
  if ( !d->internalConn ) {
    return 0;
  }
  Cursor* internalCursor = d->internalConn->prepareQuery(statement,cursor_options);
  return new xBaseCursor( this, internalCursor, statement, cursor_options );
}

Cursor* xBaseConnection::prepareQuery( QuerySchema& query, uint cursor_options ) {
  if ( !d->internalConn ) {
    return 0;
  }
  Cursor* internalCursor = d->internalConn->prepareQuery(query, cursor_options);
  return new xBaseCursor( this, internalCursor, query, cursor_options );
}

bool xBaseConnection::drv_getDatabasesList( QStringList &list ) {
  KexiDBDrvDbg << "xBaseConnection::drv_getDatabasesList()";

  //! TODO Check whether this is the right thing to do
  list<<QStringList( d->dbMap.keys() );
//        list<<d->internalConn->databaseNames();
  return true;
}

bool xBaseConnection::drv_createDatabase( const QString &dbName) {
  //! TODO Check whether this function has any use.
  KexiDBDrvDbg << "xBaseConnection::drv_createDatabase: " << dbName;
//	return d->internalConn->createDatabase(d->dbMap[dbName]);
  return true;
}

bool xBaseConnection::drv_useDatabase(const QString &dbName, bool *cancelled, MessageHandler* msgHandler)
{
  Q_UNUSED(cancelled);
  Q_UNUSED(msgHandler);
//TODO is here escaping needed?
  return d->useDatabase(dbName);
}

bool xBaseConnection::drv_closeDatabase() {
  if (!d->internalConn || !d->internalConn->closeDatabase() ) {
    return false;
  }
  return true;
}

bool xBaseConnection::drv_dropDatabase( const QString &dbName) {
//TODO is here escaping needed
  // Delete the directory ?
  return true;
}

bool xBaseConnection::drv_executeSQL( const QString& statement ) {
  return d->executeSQL(statement);
}

quint64 xBaseConnection::drv_lastInsertRowID()
{
  //! TODO
  quint64 rowID = -1;
  if (d->internalConn)
    d->internalConn->lastInsertedAutoIncValue(QString(), QString(), &rowID );

  return rowID;
}

int xBaseConnection::serverResult()
{
  return d->res;
}

QString xBaseConnection::serverResultName()
{
  if (!d->internalConn) {
    return QString();
  }
  return d->internalConn->serverResultName();
}

void xBaseConnection::drv_clearServerResult()
{
  if (!d || !d->internalConn)
    return;
  d->internalConn->clearError();
  d->res = 0;
}

QString xBaseConnection::serverErrorMsg()
{
  return d->errmsg;
}

bool xBaseConnection::drv_containsTable( const QString &tableName )
{
  bool success;
  // this will be called on the SQLite database
  return resultExists(QString("show tables like %1")
    .arg(driver()->escapeString(tableName)), success) && success;
}

bool xBaseConnection::drv_getTablesList( QStringList &list )
{
  if ( !d->internalConn ) {
    return false;
  }
  list<<d->internalConn->tableNames();
  return true;
}

PreparedStatement::Ptr xBaseConnection::prepareStatement(PreparedStatement::StatementType type,
  FieldList& fields)
{
  if ( !d->internalConn ) {
    // return a null pointer
    return KSharedPtr<PreparedStatement>();
  }
  return d->internalConn->prepareStatement( type, fields );
}

#include "xbaseconnection.moc"
