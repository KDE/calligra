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

#include <QByteArray>
#include <QStringList>
#include <QFile>

#include <kdebug.h>
#include <ktemporaryfile.h>

#include "xbaseconnection_p.h"
#include "xbaseexport.h"

#include <db/drivermanager.h>
#include <db/utils.h>
#include <db/connectiondata.h>
#include <migration/keximigrate.h>
#include <migration/migratemanager.h>

#include <core/kexiprojectdata.h>

#ifdef XBASEMIGRATE_H
#define NAMESPACE KexiMigration
#else
#define NAMESPACE KexiDB
#endif

using namespace NAMESPACE;

xBaseConnectionInternal::xBaseConnectionInternal(KexiDB::Connection* connection, KexiDB::Driver* internalDriver )
  : ConnectionInternal(connection),
  internalDriver(internalDriver)
{
}

xBaseConnectionInternal::~xBaseConnectionInternal()
{
// deletion of internalDriver and internalConn will be handled by Driver* class ( creator )
}

void xBaseConnectionInternal::storeResult()
{
  if (internalConn) {
    res = internalConn->serverResult();
    errmsg = internalConn->serverErrorMsg();
  }
}

//bool xBaseConnectionInternal::db_connect(QCString host, QCString user,
//  QCString password, unsigned short int port, QString socket)
bool xBaseConnectionInternal::db_connect(const KexiDB::ConnectionData& data)
{
  // we have to migrate the xbase source database into a .kexi file
  // xbase source database directory will be in connectiondata
  // we can choose a KTemporaryFile for the destination .kexi file

  KexiMigration::MigrateManager xBase2KexiMigrateManager;

  // create a temporary .kexi file
  KTemporaryFile temporaryKexiFile;
  temporaryKexiFile.setSuffix( ".kexi" );
  temporaryKexiFile.setAutoRemove( false );

  if ( !temporaryKexiFile.open() ) {
    kDebug()<<"Couldn't create .kexi file for exporting from xBase to .kexi";
    return false;
  }

        tempDatabase = temporaryKexiFile.fileName();

  KexiDB::ConnectionData* kexiConnectionData = 0;
  kexiConnectionData = new KexiDB::ConnectionData();

  // set destination file name here.
  kexiConnectionData->driverName = KexiDB::defaultFileBasedDriverName();
  kexiConnectionData->setFileName( tempDatabase );
  kDebug() << "Current file name: " << tempDatabase;


  QString sourceDriverName = "xbase";
  // get the source migration driver
  KexiMigration::KexiMigrate* sourceDriver = 0;
  sourceDriver = xBase2KexiMigrateManager.driver( sourceDriverName );
  if(!sourceDriver || xBase2KexiMigrateManager.error()) {
    kDebug() << "Import migrate driver error...";
    return false;
  }

  KexiMigration::Data* md = new KexiMigration::Data();
  md->keepData = true;
  md->destination = new KexiProjectData(*kexiConnectionData, tempDatabase);

  // Setup XBase connection data from input connection data passed
  //! TODO Check sanity of this
  md->source = new KexiDB::ConnectionData(data);
  md->sourceName = "";

  sourceDriver->setData(md);
  if ( !sourceDriver->performImport() ) {
    kDebug()<<"Import failed";
    return false;
  }

  // finished transferring xBase database into .kexi file

  // Get a driver to the destination database

  if ( internalDriver )
    internalConn = internalDriver->createConnection(*kexiConnectionData);
  else
    return false;

  if (!internalConn || internalDriver->error()) {
    internalDriver->debugError();
    return false;
  }
  if (!internalConn->connect()) {
    internalConn->debugError();
    storeResult();
    return false;
  }

        if (!internalConn->useDatabase(tempDatabase)) {
                internalConn->debugError();
                storeResult();
                return false;
        }

  // store mapping from xbase directory to .kexi file name for future use
  // Note: When a directory is specified ( as has to be done for xBase ), fileName()
  // will give directory name with an additional forward slash. dbPath() won't do so.
  // Need some more maintainable solution.

  dbMap[data.fileName()] = tempDatabase;

  return true;
}

/*! Disconnects from the database.
*/
bool xBaseConnectionInternal::db_disconnect(const KexiDB::ConnectionData& data)
{
  //! Export back to xBase
  xBaseExport export2xBase;
  KexiMigration::Data* migrateData = new KexiMigration::Data();
  migrateData->source = internalConn->data();
  migrateData->sourceName = tempDatabase;
  migrateData->destination = new KexiProjectData( data );
  migrateData->keepData = true;

  export2xBase.setData( migrateData );

  if (!export2xBase.performExport()) {
    return false;
  }

  return internalConn->disconnect();
}

/* ************************************************************************** */
/*! Selects dbName as the active database so it can be used.
*/
bool xBaseConnectionInternal::useDatabase(const QString &dbName)
{
  if ( !internalConn ) {
    return false;
  }
  return internalConn->useDatabase(dbMap[dbName]);
}

/*! Executes the given SQL statement
*/
bool xBaseConnectionInternal::executeSQL(const QString& statement)
{
//	KexiDBDrvDbg << "xBaseConnectionInternal::executeSQL: "
//	             << statement;
  if ( !internalConn ) {
    return false;
  }
  return internalConn->executeSQL(statement);
}
