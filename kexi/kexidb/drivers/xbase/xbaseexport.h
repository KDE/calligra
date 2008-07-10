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

#ifndef XBASE_EXPORT_H
#define XBASE_EXPORT_H

#include <kexidb/tableschema.h>

namespace Kexi {
	class ObjectStatus;
}

namespace KexiMigration {
	class Data;
}

namespace KexiDB {

class xBaseConnectionInternal;
class xBaseExportPrivate;

class xBaseExport : public KexiDB::Object 
{
	public:
		
		void setData(KexiMigration::Data* migrateData);

		//! Exports data 
		bool performExport(Kexi::ObjectStatus* result = 0);

	protected:
		
		xBaseExport();

		//! Connect to destination database
		bool dest_connect();

		//! Disconnect from destination database
		bool dest_disconnect();

		//! Create a table in the destination database
		bool dest_createTable(const QString& originalName, KexiDB::TableSchema* tableSchema);

		//! Copy table data from source to destination
		bool dest_copyTable(const QString& srcTableName, KexiDB::Connection *srcConn, 
			KexiDB::TableSchema* srcTable);

		//! Checks whether objectName is a system object name
		bool dest_isSystemObjectName(const QString& objectName);

		KexiMigration::Data* m_migrateData;

	private:
		xBaseExportPrivate* d;

	friend class KexiDB::xBaseConnectionInternal;
};

}

#endif // 
