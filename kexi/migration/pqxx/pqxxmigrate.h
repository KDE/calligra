/* This file is part of the KDE project
   Copyright (C) 2004 Adam Pigg <adam@piggz.co.uk>

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

#ifndef PQXXIMPORT_H
#define PQXXIMPORT_H

#include <migration/keximigrate.h>

//Kexi Includes
#include <kexidb/field.h>
#include <kexidb/connection.h>

#include <pqxx/all.h>

namespace KexiMigration
{
	class pqxxMigrate : public KexiMigrate
	{
		Q_OBJECT
		private:
			//lowlevel functions/objects
			//database connection
			pqxx::connection* m_conn;
			//transaction
			pqxx::nontransaction* m_trans;
			//lowlevel result
			pqxx::result* m_res;
			//perform a query on the database
			bool query(const QString& statement);
			//Clear the result info
			void clearResultInfo ();
			
			pqxx::oid tableOid(const QString& tablename);
			
			//Convert the pqxx type to a kexi type
			KexiDB::Field::Type type(int t, const QString& fname);
			
			//Find out the field constraints
			//Return whether or not the field is a pkey
			bool primaryKey(pqxx::oid table, int col) const;
			
			//Return whether or not the field is unique
			bool uniqueKey(pqxx::oid table, int col) const;
			
			//Return whether or not the field is a foreign key
			bool foreignKey(pqxx::oid table, int col) const;
			
			//Return whether or not the field is not null
			bool notNull(pqxx::oid table, int col) const;
			
			//Return whether or not the field is not empty
			bool notEmpty(pqxx::oid table, int col) const;
			
			//Return whether or not the field is auto incrementing
			bool autoInc(pqxx::oid table, int col) const;
			
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
//TODO: move this somewhere to low level class (MIGRATION?)
			//virtual bool drv_getTablesList( QStringList &list );
//TODO: move this somewhere to low level class (MIGRATION?)
			//virtual bool drv_containsTable( const QString &tableName );
		public:
			~pqxxMigrate();
			//Constructor
			pqxxMigrate();
			pqxxMigrate(QObject *parent, const char *name, const QStringList &args = QStringList());
			virtual int versionMajor() const;
			virtual int versionMinor() const;
			
	};
}

#endif
