/* This file is part of the KDE project
   Copyright (C) 2004 Adam Pigg <adam@piggz.co.uk>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2005 Martin Ellis <kde@martinellis.co.uk>

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

#ifndef KEXI_MIGRATE_H
#define KEXI_MIGRATE_H

#include "kexidb/connection.h"
#include "kexidb/tableschema.h"
#include <qstringlist.h>
#include <vector>

/*!
 * \namespace KexiMigration
 * \brief Framework for importing databases into native KexiDB databases.
 */
namespace KexiMigration 
{
  //! Imports non-native databases into Kexi projects.
	/*! A generic API for importing data from an existing
	database into a new Kexi project.

	Basic idea is this:
	-# User selects an existing DB and new project (file or server based)
	-# User specifies whether to import structure and data or structure only.
	-# Import tool connects to db
	-# Checks if it is already a kexi project (not implemented yet)
	-# If not, then read structure and construct new project
	-# Ask user what to do if column type is not supported

	See kexi/doc/dev/kexi_import.txt for more info.
	*/
	class KEXIMIGR_EXPORT KexiMigrate : public QObject, public KexiDB::Object
	{
		Q_OBJECT

		public:
			//Destructor
			~KexiMigrate();

			//Constructor
			KexiMigrate();
			KexiMigrate(QObject *parent, const char *name, const QStringList &args = QStringList());

			//! Data Setup.  Requires two connection objects, a name and a bool
			void setData(KexiDB::ConnectionData* externalConnectionData,
			             QString dbFrom, KexiDB::Connection* kexiConnection,
			             QString newdbname, bool keep_data);

			//! Perform an import operation
			bool performImport();

			//! Perform an export operation
			bool performExport();

			inline bool progressSupported() { return drv_progressSupported(); }

			//! Returns true if the migration driver supports progress updates.
		signals:
			void progressPercent(int percent);

		public:
			virtual int versionMajor() const = 0;
			virtual int versionMinor() const = 0;
		protected:

			//! Connect to source database (driver specific)
			virtual bool drv_connect() = 0;
			//! Disconnect from source database (driver specific)
			virtual bool drv_disconnect() = 0;

			//! Get table names in source database (driver specific)
			virtual bool drv_tableNames(QStringList& tablenames) = 0;

			//! Read schema for a given table (driver specific)
			virtual bool drv_readTableSchema(const QString table) = 0;

			//! Copy a table from source DB to target DB (driver specific)
			virtual bool drv_copyTable(const QString& srcTable,
			                           KexiDB::TableSchema* dstTable) = 0;

			virtual bool drv_progressSupported() { return false; }

			//! Return the size of a table to be imported, or 0 if not supported
			/*! Finds the size of the named table, in order to provide feedback on
			    migration progress.

			    The units of the return type are deliberately unspecified.  Migration
			    drivers may return the number of records in the table, or the size in
			    bytes, etc.  Units should be chosen in order that the driver can 
			    return the size in the fastest way possible (e.g. migration from CSV
			    files should use file size to avoid counting the number of rows, and
			    migration from MDB files should return the number of rows as this is
			    stored within the file).

			    Obviously, the driver should use the same units when reporting
			    migration progress.

			    \return size of the specified table
			*/
			virtual bool drv_getTableSize(const QString&, Q_ULLONG&)
			{ return false; }

			void progressDoneRow();

			//! Prompt user to select a field type for unrecognised fields
			KexiDB::Field::Type userType();

			// Protected data members
			//! Connection data for external (non Kexi) database.
			KexiDB::ConnectionData* m_externalData;
			//! Destination KexiDB database.
			KexiDB::Connection* m_kexiDB;

			//! Name of the source DB (server based DBs only)
			QString m_dbName;
			//! Name of the target DB
			/*! Should be the same as m_kexiDB->filename() for file based projects. */
			QString m_todbname;

			// Temporary values used during import (set by driver specific methods)
			KexiDB::TableSchema* m_table;
			KexiDB::Field* m_f;

		private:
			//! Get the list of tables
			bool tableNames(QStringList& tablenames);

			//Perform general functionality and rely on drv_ReadTableSchema()
			//to do the real work
			//! Read a table schema object for a table (into m_table)
			bool readTableSchema(const QString& table);

			//! Copy data from original table to new table if required
			bool copyData(const QString& table, KexiDB::TableSchema* dstTable);

			//! Create the target database
			bool createDatabase(const QString& dbname);

			//Private data members
			//! Flag indicating whether data should be copied
			bool m_keepData;

			//! Table schemas from source DB
			std::vector<KexiDB::TableSchema*>v_tableSchemas;

			//! Estimate size of migration job
			/*! Calls drv_getTableSize for each table to be copied.
			    \return sum of the size of all tables to be copied.
			 */
			bool progressInitialise();

			//! Size of migration job
			Q_ULLONG progressTotal;
			//! Amount of migration job complete
			Q_ULLONG progressDone;
			//! Don't recalculate progress done until this value is reached.
			Q_ULLONG progressNextReport;


	};

} //namespace KexiMigration

#include <kgenericfactory.h>
#define KEXIMIGRATE_DRIVER_INFO( class_name, internal_name ) \
	int class_name::versionMajor() const { return 0; } \
	int class_name::versionMinor() const { return 0; } \
	K_EXPORT_COMPONENT_FACTORY(keximigrate_ ## internal_name, \
	  KGenericFactory<KexiMigration::class_name>( "keximigrate_" #internal_name ))
#endif

