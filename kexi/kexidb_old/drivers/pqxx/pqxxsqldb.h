/* This file is part of the KDE project
  Copyright (C) 2003  Dawit Alemayehu <adawit@kde.org>


  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.
*/

/*
Modification of original source to use libpqxx
by Adam Pigg (adam@piggz.fsnet.co.uk)
*/

#ifndef PGSQLDB_H
#define PGSQLDB_H

#include <pqxx/pqxx>
#include <kexidb.h>
#include <kexidberror.h>

#include "pqxxsqlresult.h"
#include "pqxxsqlrecord.h"

using namespace PGSTD;
using namespace pqxx;
using namespace std;

class pqxxSqlDB : public KexiDB
{
  Q_OBJECT

public:
  pqxxSqlDB( QObject *parent=0, const char *name="pgsql",
           const QStringList &args=QStringList() );
  ~pqxxSqlDB();

public slots:
  //Returns the name of the driver
  QString driverName (); 

  //Returns a list of tables eithin the current database
  QStringList tableNames ();

  const KexiDBTable *const table(const QString&);

  //Returns a list of databases held on the server
  QStringList databases (); 

  //Properly quotes an SQL string
  QString escape (const QString &str) const;
  QString escape (const QByteArray& str) const;
  //Returns an escaped version of a named object, eg table...mysql uses `, postgres uses "
  QString escapeName(const QString &tn) const;
  //Escape a value given its type - mysql may allow everything to be quoted with " " but pg doesnt
  QString escapeValue(const QString &v, KexiDBField::ColumnType t) const;

  //Returns true if the specified database is consided a system db
  virtual bool isSystemDatabase(QString &dbName);

  //Returns a recordset object for the given query
  KexiDBRecordSet* queryRecord (const QString &query, bool buffer);

  //Connects to a server without specifiying the database to connect to
  bool connect (const QString& host, const QString& user, const QString& password, const QString& socket, const QString& port);

  //Connects to a server specifting the database to connect to
  bool connect (const QString& host, const QString& user, const QString& password, const QString& socket, const QString& port, const QString& db, bool create = false);

  //Executes a query, and if successful stores the result in _dbResult
  bool query (const QString& statement, bool trans);
  bool query (const QString& statement);



  //Alters a field
  bool alterField (const KexiDBField& changedField, unsigned int index, KexiDBTableStruct fields);

  //Creates a new field and optionally creates a table
  bool createField (const KexiDBField& newField, KexiDBTableStruct fields, bool createTable = false);

  //Returns the current result
  pqxxSqlResult	*getResult();

  //Stores the current result
  pqxxSqlResult	*storeResult();

  //uses the current result
  pqxxSqlResult	*useResult();

  //Returns the number of affected rows
  unsigned long	affectedRows();

  //Returns the last row inserted into a table using lastoid()
  result* lastRow(const string table);

  //Returns the kexi datatype given a postgres datatype
  static KexiDBField::ColumnType getInternalDataType(int t);

  //Returns the postgres datatype given a kexi datatype
  QString nativeDataType (const KexiDBField::ColumnType&) const;

  //Returns a table structure
  KexiDBTableStruct structure (const QString& table) const;

  //Returns the last error
  KexiDBError* latestError();

  //Helper functions that provide functionality alreay available in mysql c api
  //Return the name of a table given a table uid
  QString tableName(oid table_uid) const;

  //Return whether or not the field in the result is a primary key
  bool primaryKey(oid table_uid, int col) const;

   //Return whether or not the field in the result is a unique key
  bool uniqueKey(oid table_uid, int col) const;

  //Return whether or not the field in the result is not null
  bool notNull(oid table_uid, int col) const;

protected:
  int reconnect();
  void initCheckUpdate();

  connection* _dbConn;
  result* _dbResult;
  transaction_base* _dbTransaction;

  //Connection details
  bool m_connected;
  bool m_connectedDB;
  unsigned int m_port;
  QString m_socket;
  unsigned int m_client_flag;
  QString m_host;
  QString m_user;
  QString m_password;

private:
  QString createDefinition(const KexiDBField& field, int index, KexiDBTableStruct fields);
  bool changeKeys(const KexiDBField& field, int index, KexiDBTableStruct fields);
  KexiDBTable * createTableDef(const QString& name);

  //Table Definitions
  QDict<KexiDBTable> m_tableDefs;

  //Clears the result object
  void clearResultInfo ();

  //Clears the connection
  void clearConnectionInfo ();

  //Our error object
  KexiDBError m_error;


};

#endif
