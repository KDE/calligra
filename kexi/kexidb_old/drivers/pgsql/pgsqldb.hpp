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

#ifndef PGSQLDB_H
#define PGSQLDB_H

#include <libpq-fe.h>

#include <kexidb.h>
#include <kexidberror.h>

class PgSqlDB : public KexiDB
{
  Q_OBJECT

public:
  PgSqlDB( QObject *parent=0, const char *name="pgsql",
           const QStringList &args=QStringList() );
  ~PgSqlDB();

  QStringList tableNames ();
  QStringList databases ();
  QString driverName () const;
  KexiDBTableStruct structure (const QString&) const;
  QString nativeDataType (const KexiDBField::ColumnType&) const;
  const KexiDBTable *const table(const QString&)=0;

public slots:

  KexiDBRecordSet* queryRecord (const QString &query, bool buffer = false);

  bool connect (const QString &host, const QString &user, const QString &password,
                const QString &socket, const QString &port);

  bool connect (const QString &host, const QString &user, const QString &password,
                const QString &socket, const QString &port, const QString &db, bool create = false);

  bool query (const QString &statement);

  QString escape (const QString &str);

  QString escape (const QByteArray &str);

  bool alterField (const KexiDBField& changedField, unsigned int index,
                   KexiDBTableStruct fields);

  bool createField (const KexiDBField& newField, KexiDBTableStruct fields,
                    bool createTable = false);

  bool createTable (const KexiDBTable& tableDef);

  KexiDBError* latestError();

private:
  void clearResultInfo ();
  void clearConnectionInfo ();

private:
  PGconn* _dbConn;
  PGresult* _dbResult;
  KexiDBError _dbError;
};

#endif
