/* This file is part of the KDE project
  Copyright (C) 2003  Dawit Alemayehu <adawit@kde.org>


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

#include <stdio.h>

#include <kdebug.h>
#include <kgenericfactory.h>

#include <kexidberror.h>
#include "pgsqldb.hpp"

K_EXPORT_COMPONENT_FACTORY(kexipgsqlinterface, KGenericFactory<PgSqlDB>( "pgsqlinterface" ));

PgSqlDB::PgSqlDB(QObject *parent, const char *name, const QStringList &)
        :KexiDB(parent, name), _dbConn(0), _dbResult(0)
{
}

PgSqlDB::~PgSqlDB()
{
  clearResultInfo ();
  clearConnectionInfo ();
}

void PgSqlDB::clearConnectionInfo ()
{
  if (_dbConn)
  {
    PQfinish (_dbConn);
    _dbConn = 0;
  }
}

void PgSqlDB::clearResultInfo ()
{
  if (_dbResult)
  {
    PQclear (_dbResult);
    _dbResult = 0;
  }
}

QString PgSqlDB::driverName() const
{
  return QString::fromLatin1 ("pgsql");
}

KexiDBRecordSet* PgSqlDB::queryRecord (const QString&, bool)
{
  return 0;
}


QStringList  PgSqlDB::databases ()
{
  QStringList list;

  if (query("SELECT datname FROM pg_database WHERE datallowconn = TRUE"))
  {
    int numTuples = PQntuples (_dbResult);
    for (int tuple = 0; tuple < numTuples; tuple++)
      list <<  QString::fromLatin1 (PQgetvalue(_dbResult, tuple, 0));
  }

  return list;
}

QStringList  PgSqlDB::tableNames ()
{
  QStringList list;

  if (query("SELECT relname FROM pg_class WHERE relkind = 'r' AND "
            "relname !~ '^pg_'"))
  {
    int numTuples = PQntuples (_dbResult);
    for (int tuple = 0; tuple < numTuples; tuple++)
      list <<  QString::fromLatin1 (PQgetvalue(_dbResult, tuple, 0));
  }

  return list;
}

const KexiDBTable *const PgSqlDB::table(const QString&)
{
  //FIXME: implement
  return 0;
}

bool PgSqlDB::query (const QString& statement)
{
  Q_ASSERT (_dbConn);

  // Clear the last result information...
  clearResultInfo ();

  // Prepare the query by properly escaping it...
  //statement = escape (statement);

  // Clear out the database error message...
  _dbError.clear ();

  // Send the query to the backend...
  kdDebug() << "*** query: " << statement << endl;

  _dbResult = PQexec(_dbConn, statement.latin1());

  // Get the status of the query...
  ExecStatusType status = PQresultStatus(_dbResult);

  if (status == PGRES_BAD_RESPONSE ||
      status == PGRES_NONFATAL_ERROR ||
      status == PGRES_FATAL_ERROR)
  {
    _dbError.setup (-1, PQresultErrorMessage(_dbResult));
    return false;
  }

  return true;
}

KexiDBError* PgSqlDB::latestError()
{
  return &_dbError;
}

QString PgSqlDB::escape (const QString& str)
{
  kdDebug() << "Escape: " << str << endl;

  const int size = (str.length() * 2) + 1;

  char to [size];
  char* from = const_cast<char*>(str.latin1());

  kdDebug() << "Wrote: " << PQescapeString (from, to, str.length()) << " chars" << endl;
  kdDebug() << "Escaped: " << to << endl;

  return to;
}

QString PgSqlDB::escape (const QByteArray& str)
{
  QCString to ((str.size()*2)+1);
  PQescapeString (str.data(), to.data(), str.size());
  return QString(to);
}

bool PgSqlDB::connect (const QString &host, const QString &user, const QString &password,
                     const QString &socket, const QString &port)
{
  return connect (host, user, password, socket, port, "");
}

bool PgSqlDB::connect (const QString &host, const QString &user, const QString &password,
                     const QString &socket, const QString &port, const QString &db, bool create)
{
  if (_dbConn)
  {
    if (PQstatus(_dbConn) == CONNECTION_OK &&
        db == QString::fromLatin1(PQdb(_dbConn)) &&
        host == QString::fromLatin1(PQhost(_dbConn)) &&
        user == QString::fromLatin1(PQuser(_dbConn)) &&
        password == QString::fromLatin1(PQpass(_dbConn)) &&
        port == QString::fromLatin1(PQport(_dbConn)))
      return true;

    // Close the current connection and establish a new one...
    clearConnectionInfo ();
  }

  QString conninfo;
  if (!host.isNull())
    conninfo = QString::fromLatin1("host='%1'").arg(host);

  bool ok;
  if (port.toInt(&ok) && ok)
    conninfo += QString::fromLatin1(" port='%1'").arg(port);

  if (db.isEmpty() || create)
    conninfo += QString::fromLatin1(" dbname='template1'");
  else
    conninfo += QString::fromLatin1(" dbname='%1'").arg(db);

  if (!user.isNull())
    conninfo += QString::fromLatin1(" user='%1'").arg(user);

  if (!password.isNull())
    conninfo += QString::fromLatin1(" password='%1'").arg(password);

  kdDebug () << "Connection info: " << conninfo << endl;
  kdDebug () << "Create: " << create << endl;

  _dbError.clear ();
  _dbConn = PQconnectdb( conninfo.latin1() );

  if (PQstatus (_dbConn) == CONNECTION_BAD)
  {
    _dbError.setup (-1, PQerrorMessage(_dbConn));
    return false;
  }

  if (create && !db.isEmpty())
  {
    // If we are able to create the requested database, close the
    // current connection and open a new connection the created db
    if (query (QString::fromLatin1("CREATE DATABASE %1").arg(db)))
      return connect (host, user, password, socket, port, db);
  }

  return true;
}

bool PgSqlDB::alterField (const KexiDBField& changedField, unsigned int index,
                 KexiDBTableStruct fields)
{
  return false;
}

bool PgSqlDB::createField (const KexiDBField& newField, KexiDBTableStruct fields,
                  bool createTable = false)
{
  return false;
}

bool PgSqlDB::createTable (const KexiDBTable& tableDef)
{
  return false;
}

KexiDBTableStruct PgSqlDB::structure (const QString& table) const
{
  return KexiDB::structure (table);
}

QString PgSqlDB::nativeDataType (const KexiDBField::ColumnType& type) const
{
  return QString::null;
}

#include "pgsqldb.moc"
