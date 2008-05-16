/* This file is part of the KDE project
   Copyright (C) 2008 Sharan Rao <sharanrao@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef XBASEMIGRATE_H
#define XBASEMIGRATE_H

#include <QHash>

#include "migration/keximigrate.h"
#include "xbase.h"

namespace KexiMigration
{

class xBaseMigrate : public KexiMigrate, protected xbXBase
{
	Q_OBJECT
	KEXIMIGRATION_DRIVER

	public:
		xBaseMigrate(QObject *parent, const QStringList& args = QStringList());
		virtual ~xBaseMigrate();
		
	protected:
		//! Driver specific function to return table names
		virtual bool drv_tableNames(QStringList& tablenames);
		
		//! Driver specific implementation to read a table schema
		virtual bool drv_readTableSchema(
			const QString& originalName, KexiDB::TableSchema& tableSchema);
		
		//! Driver specific connection implementation
		virtual bool drv_connect();
		
		virtual bool drv_disconnect();

		virtual bool drv_copyTable(const QString& srcTable, 
			KexiDB::Connection *destConn, KexiDB::TableSchema* dstTable);

//TODO: move this somewhere to low level class (MIGRATION?)
//			virtual bool drv_getTablesList( QStringList &list );
//TODO: move this somewhere to low level class (MIGRATION?)
//			virtual bool drv_containsTable( const QString &tableName );


	private:
		KexiDB::Field::Type type(char xBaseColumnType);

		//! Sets and existing constraints on the field
		void getConstraints(const QString& tableName, KexiDB::Field* fld);

		//! Returns a list of index files corresponding to the specific fieldName
		QStringList getIndexFileNames(const QString& tableName, const QString& fieldName);

		//! Mapping tableNames to actual absoolute file name paths
		//  XBase only deals with absolute names ( with the .dbf extension ) which is pretty cumbersome
		QHash<QString,QString> m_tableNamePathMap;
		
};
}

#endif
