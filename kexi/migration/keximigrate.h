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

#ifndef KEXI_IMPORT_H
#define KEXI_IMPORT_H

#include "kexidb/connection.h"
#include "kexidb/tableschema.h"
#include <qstringlist.h>
#include <vector>

namespace KexiMigration 
{
	/*! This class provides a generic api for importing data from an existing
	database into a kexi project.
	
	Basic idea is this:
	1. User specifies existing db on server and new project (file or server based)
	2. user specifies whether or not to read data aswell
	3. Import tool connects to db
	4. Checks if it is already a kexi project
	5. If not then read tables/columns and construct new project
	6. Ask user what to do if column type is not supported
	7. See kexi/doc/dec/kexi_import.txt for more info
	*/
	class KexiMigrate : public QObject, public KexiDB::Object
	{
		Q_OBJECT
	
		public:
			//Destructor
			~KexiMigrate();
			
			//Constructor
			KexiMigrate();
			
			//Data Setup.  Requires two connection objects, a name and a bool
			void setData(KexiDB::ConnectionData* externalConnectionData, QString dbFrom, KexiDB::Connection* kexiConnection, QString newdbname, bool keep_data);
			
			//Performs an import operation
			bool performImport();
			
			//Perform an export operation
			bool performExport();
			
		protected:
			
					
			//Driver specific function to return table names
			virtual bool drv_tableNames(QStringList& tablenames) = 0;
			
			//Driver specific implementation to read a table schema
			virtual bool drv_readTableSchema(const QString table) = 0;
			
			//Driver specific connection function
			virtual bool drv_connect() = 0;
			virtual bool drv_disconnect() = 0;
			
			//Generic helper functions
			KexiDB::Field::Type userType();
			
			//Protected data members
			//Connextiondata for external (non kexi) db
			KexiDB::ConnectionData* m_externalData;
			
			QString m_dbName;
                        QString m_todbname;
			KexiDB::TableSchema* m_table;
			KexiDB::Field* m_f;
		private:		
			//Get the list of tables
			bool tableNames(QStringList& tablenames);
			
			//Get a table schema object for a table
			//Perform general functionality and rely on drv_ReadTableSchema()
			//to do the real work
			bool readTableSchema(const QString& tabl, int i);
			
			//Copies data from original table to new table if required
			bool copyData(const QString table);
			
			//Create the final database
			bool createDatabase(const QString& dbname);
			 
			//Private data members
			KexiDB::Connection* m_kexiDB;
			bool m_keepData;
			
			std::vector<KexiDB::TableSchema*>v_tableSchemas;
	};

} //namespace KexiMigration

#endif

