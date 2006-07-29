/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>
                      Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003 Joseph Wenninger<jowenn@kde.org>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#include <qvariant.h>
#include <qfile.h>
#include <qdict.h>

#include <kgenericfactory.h>
#include <kdebug.h>

#include "mysqldriver.h"
#include "mysqlconnection.h"
#include "mysqlconnection_p.h"
#include "mysqlcursor.h"
#include "mysqlpreparedstatement.h"
#include <kexidb/error.h>


using namespace KexiDB;

//--------------------------------------------------------------------------

MySqlConnection::MySqlConnection( Driver *driver, ConnectionData &conn_data )
	:Connection(driver,conn_data)
	,d(new MySqlConnectionInternal(this))
{
}

MySqlConnection::~MySqlConnection() {
	destroy();
}

bool MySqlConnection::drv_connect() {
  return d->db_connect(*m_data);
}

bool MySqlConnection::drv_disconnect() {
  return d->db_disconnect();
}

Cursor* MySqlConnection::prepareQuery(const QString& statement, uint cursor_options) {
	return new MySqlCursor(this,statement,cursor_options);
}

Cursor* MySqlConnection::prepareQuery( QuerySchema& query, uint cursor_options ) {
	return new MySqlCursor( this, query, cursor_options );
}

bool MySqlConnection::drv_getDatabasesList( QStringList &list ) {
	KexiDBDrvDbg << "MySqlConnection::drv_getDatabasesList()" << endl;
	list.clear();
	MYSQL_RES *res;

	if((res=mysql_list_dbs(d->mysql,0)) != 0) {
		MYSQL_ROW  row;
		while ( (row = mysql_fetch_row(res))!=0) {
			list<<QString(row[0]);
		}
		mysql_free_result(res);
		return true;
	}

	d->storeResult();
//	setError(ERR_DB_SPECIFIC,mysql_error(d->mysql));
	return false;
}

bool MySqlConnection::drv_createDatabase( const QString &dbName) {
	KexiDBDrvDbg << "MySqlConnection::drv_createDatabase: " << dbName << endl;
	// mysql_create_db deprecated, use SQL here. 
	if (drv_executeSQL("CREATE DATABASE " + (dbName)))
		return true;
	d->storeResult();
	return false;
}

bool MySqlConnection::drv_useDatabase(const QString &dbName, bool *cancelled, MessageHandler* msgHandler)
{
	Q_UNUSED(cancelled);
	Q_UNUSED(msgHandler);
//TODO is here escaping needed?
	return d->useDatabase(dbName);
}

bool MySqlConnection::drv_closeDatabase() {
//TODO free resources 
//As far as I know, mysql doesn't support that
	return true;
}

bool MySqlConnection::drv_dropDatabase( const QString &dbName) {
//TODO is here escaping needed
	return drv_executeSQL("drop database "+dbName);
}
                
bool MySqlConnection::drv_executeSQL( const QString& statement ) {
  return d->executeSQL(statement);
}

Q_ULLONG MySqlConnection::drv_lastInsertRowID()
{
	//! @todo
	return (Q_ULLONG)mysql_insert_id(d->mysql);
}

int MySqlConnection::serverResult()
{
	return d->res;
}

QString MySqlConnection::serverResultName()
{
	return QString::null;
}

void MySqlConnection::drv_clearServerResult()
{
	if (!d)
		return;
	d->res = 0;
}

QString MySqlConnection::serverErrorMsg()
{
	return d->errmsg;
}

bool MySqlConnection::drv_containsTable( const QString &tableName )
{
	bool success;
	return resultExists(QString("show tables like %1")
		.arg(driver()->escapeString(tableName)), success) && success;
}

bool MySqlConnection::drv_getTablesList( QStringList &list )
{
	KexiDB::Cursor *cursor;
	m_sql = "show tables";
	if (!(cursor = executeQuery( m_sql ))) {
		KexiDBDbg << "Connection::drv_getTablesList(): !executeQuery()" << endl;
		return false;
	}
	list.clear();
	cursor->moveFirst();
	while (!cursor->eof() && !cursor->error()) {
		list += cursor->value(0).toString();
		cursor->moveNext();
	}
	if (cursor->error()) {
		deleteCursor(cursor);
		return false;
	}
	return deleteCursor(cursor);
}

PreparedStatement::Ptr MySqlConnection::prepareStatement(PreparedStatement::StatementType type, 
	FieldList& fields)
{
	return new MySqlPreparedStatement(type, *d, fields);
}

#include "mysqlconnection.moc"
