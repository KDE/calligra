/* This file is part of the KDE project
   Copyright (C) 2004 Martin Ellis <m.a.ellis@ncl.ac.uk>

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

#ifndef MYSQLMIGRATE_H
#define MYSQLMIGRATE_H

#include "migration/keximigrate.h"
#include "kexidb/field.h"
#include "kexidb/connection.h"
#include "kexidb/drivers/mySQL/mysqlconnection_p.h"

namespace KexiMigration
{

	class MySQLMigrate : public KexiMigrate
	{
		Q_OBJECT
		private:
			MySqlConnectionInternal *d;

		protected:
			//Driver specific function to return table names
			virtual bool drv_tableNames(QStringList& tablenames);
			
			//Driver specific implementation to read a table schema
			virtual bool drv_readTableSchema(const QString table);
			
			//Driver specific connection implementation
			virtual bool drv_connect();
			virtual bool drv_disconnect();

			virtual bool drv_copyTable(const QString& srcTable,
			                           KexiDB::TableSchema* dstTable);

			virtual bool drv_progressSupported() { return true; }
			virtual bool drv_getTableSize(const QString& table, Q_ULLONG& size);

//TODO: move this somewhere to low level class (MIGRATION?)
//			virtual bool drv_getTablesList( QStringList &list );
//TODO: move this somewhere to low level class (MIGRATION?)
//			virtual bool drv_containsTable( const QString &tableName );

		public:
			~MySQLMigrate();
			//Constructor
			MySQLMigrate();
			MySQLMigrate::MySQLMigrate(QObject *parent, const char *name, const QStringList& args = QStringList());
			
			KexiDB::Field::Type type(const QString& table, const MYSQL_FIELD* t);
			
			KexiDB::Field::Type examineBlobField(const QString& table, 
			                                     const MYSQL_FIELD* fld);
			
			QStringList examineEnumField(const QString& table,
			                             const MYSQL_FIELD* fld);
			void MySQLMigrate::getConstraints(int mysqlConstraints, KexiDB::Field* fld);
			void MySQLMigrate::getOptions(int flags, KexiDB::Field* fld);

		/*! driver's static version information, it is automatically implemented
		 in implementation using KEXIDB_DRIVER macro (see driver_p.h) */
		virtual int versionMajor() const;
		virtual int versionMinor() const;
	};
}

#endif
