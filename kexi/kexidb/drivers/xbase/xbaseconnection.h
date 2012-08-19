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
* Boston, MA 02110-1301, USA.
*/

#ifndef XBASECONNECTION_H
#define XBASECONNECTION_H

#include <QStringList>

#include <db/connection.h>
#include "xbasecursor.h"

namespace KexiDB {

class xBaseConnectionInternal;

/*!
* Should override kexiDB/kexiDB
* all other members are done by the
* base class.
*/
class xBaseConnection : public Connection
{
  Q_OBJECT

  public:
    virtual ~xBaseConnection();

    virtual Cursor* prepareQuery( const QString& statement = QString(), uint cursor_options = 0 );
    virtual Cursor* prepareQuery( QuerySchema& query, uint cursor_options = 0 );

    virtual PreparedStatement::Ptr prepareStatement(PreparedStatement::StatementType type, 
      FieldList& fields);

  protected:

    /*! Used by driver */
    xBaseConnection( Driver *driver, Driver* internalDriver, ConnectionData &conn_data );

    virtual bool drv_connect(KexiDB::ServerVersionInfo& version);
    virtual bool drv_disconnect();
    virtual bool drv_getDatabasesList( QStringList &list );
    virtual bool drv_createDatabase( const QString &dbName = QString() );
    virtual bool drv_useDatabase( const QString &dbName = QString(), bool *cancelled = 0, 
      MessageHandler* msgHandler = 0 );
    virtual bool drv_closeDatabase();
    virtual bool drv_dropDatabase( const QString &dbName = QString() );
    virtual bool drv_executeSQL( const QString& statement );
    virtual quint64 drv_lastInsertRowID();

    virtual int serverResult();
    virtual QString serverResultName();
    virtual QString serverErrorMsg();
    virtual void drv_clearServerResult();

//TODO: move this somewhere to low level class (MIGRATION?)
    virtual bool drv_getTablesList( QStringList &list );
//TODO: move this somewhere to low level class (MIGRATION?)
    virtual bool drv_containsTable( const QString &tableName );

    xBaseConnectionInternal* d;

    friend class xBaseDriver;
    friend class xBaseCursor;
};

}

#endif
