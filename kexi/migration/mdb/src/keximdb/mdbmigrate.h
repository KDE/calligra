/* This file is part of the KDE project
   Copyright (C) 2005,2006 Martin Ellis <martin.ellis@kdemail.net>
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef MDBMIGRATE_H
#define MDBMIGRATE_H

#include <mdbtools.h>

#include "kexidb/keximigrate.h"
#include "kexidb/field.h"
#include "kexidb/connection.h"

namespace KexiMigration
{

	class MDBMigrate : public KexiMigrate
	{
		Q_OBJECT
		KEXIMIGRATION_DRIVER
		
		public:
			MDBMigrate(QObject *parent, const char *name, const QStringList& args = QStringList());
			virtual ~MDBMigrate();
			
			KexiDB::Field::Type type(int type);
			MdbTableDef* getTableDef(const QString& tableName);
			QVariant toQVariant(const char* data, unsigned int len, int type);
			bool getPrimaryKey(KexiDB::TableSchema* table, MdbTableDef* tableDef);

			//! Reimplemented to add support for "sourceDatabaseHasNonUnicodeEncoding" property
			//! @todo this should be in Connection class but Migration framework has no such yet!
			virtual QVariant propertyValue( const QCString& propName );

		protected:
			//Driver specific function to return table names
			virtual bool drv_tableNames(QStringList& tablenames);
			
			//Driver specific implementation to read a table schema
			virtual bool drv_readTableSchema(
				const QString& originalName, KexiDB::TableSchema& tableSchema);
			
			//Driver specific connection implementation
			virtual bool drv_connect();
			virtual bool drv_disconnect();

			virtual bool drv_copyTable(const QString& srcTable, 
			                           KexiDB::Connection *destConn,
			                           KexiDB::TableSchema* dstTable);

			virtual bool drv_progressSupported() { return true; }
			virtual bool drv_getTableSize(const QString& table, Q_ULLONG& size);

		private:
			void initBackend();
			void releaseBackend();
			MdbHandle *m_mdb;
	};
}

#endif
