/* This file is part of the KDE project
   Copyright (C) 2004 Adam Pigg <adam@piggz.co.uk>
   Copyright (C) 2004-2006 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2005 Martin Ellis <martin.ellis@kdemail.net>

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

#ifndef KEXI_MIGRATE_H
#define KEXI_MIGRATE_H


#include "kexidb/tableschema.h"
#include "kexidb/connection.h"
#include "keximigratedata.h"

#include <kgenericfactory.h>
#include <qstringlist.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3CString>
#include <Q3PtrList>
#include <QPointer>

class KexiProject;
namespace Kexi
{
	class ObjectStatus;
}

/*! KexiMigration implementation version. 
 It is altered after every change: 
 - major number is increased after every major Kexi release, 
 - minor is increased after adding binary-incompatible change.
 In external code: do not use this to get library version information:
 use KexiMigration::versionMajor() and KexiMigration::versionMinor() instead to get real version.
*/
#define KEXI_MIGRATION_VERSION_MAJOR 1
#define KEXI_MIGRATION_VERSION_MINOR 1

/*!
 * \namespace KexiMigration
 * \brief Framework for importing databases into native KexiDB databases.
 */
namespace KexiMigration 
{

//! \return KexiMigration version info (most significant part)
KEXIMIGR_EXPORT int versionMajor();

//! \return KexiMigration version info (least significant part)
KEXIMIGR_EXPORT int versionMinor();


//! @short Imports non-native databases into Kexi projects.
/*! A generic API for importing schema and data from an existing
database into a new Kexi project. Can be also used for importing native Kexi databases.

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
		virtual ~KexiMigrate();

//! @todo Remove this! KexiMigrate should be usable for multiple concurrent migrations!
		KexiMigration::Data* data() const { return m_migrateData; }

//! @todo Remove this! KexiMigrate should be usable for multiple concurrent migrations!
		//! Data Setup.  Requires two connection objects, a name and a bool
		void setData(KexiMigration::Data* migrateData);

		/*! Checks whether the destination database exists.
		 For file-based dest. projects, we've already asked about overwriting 
		 existing project but for server-based projects it's better to ask user.
		 This method should be called before performImport() or performExport().

		 \return true if no connection-related errors occurred. 
		 \a acceptingNeeded is set to true if destination database exists. 
		 In this case you should ask about accepting database overwriting. 
		 Used in ImportWizard::import(). */
		bool checkIfDestinationDatabaseOverwritingNeedsAccepting(Kexi::ObjectStatus* result,
			bool& acceptingNeeded);

		/*! Checks if the source- and the destination databases are identical.
		\return true if they are identical else false. */
		bool isSourceAndDestinationDataSourceTheSame() const;

		//! Perform an import operation
		bool performImport(Kexi::ObjectStatus* result = 0);

		//! Perform an export operation
		bool performExport(Kexi::ObjectStatus* result = 0);

		//! Returns true if the migration driver supports progress updates.
		inline bool progressSupported() { return drv_progressSupported(); }

		virtual int versionMajor() const = 0;
		virtual int versionMinor() const = 0;

//! @todo This is copied from KexiDB::Driver. One day it will be merged with KexiDB.
		//! \return property value for \a propeName available for this driver. 
		//! If there's no such property defined for driver, Null QVariant value is returned.
		virtual QVariant propertyValue( const Q3CString& propName );

//! @todo This is copied from KexiDB::Driver. One day it will be merged with KexiDB.
		void setPropertyValue( const Q3CString& propName, const QVariant& value );

//! @todo This is copied from KexiDB::Driver. One day it will be merged with KexiDB.
		//! \return translated property caption for \a propeName. 
		//! If there's no such property defined for driver, empty string value is returned.
		QString propertyCaption( const Q3CString& propName ) const;

//! @todo This is copied from KexiDB::Driver. One day it will be merged with KexiDB.
		//! \return a list of property names available for this driver.
		Q3ValueList<Q3CString> propertyNames() const;

		/*! \return true is driver is valid. Checks if KexiMigrate::versionMajor() 
		 and KexiMigrate::versionMinor() are matching. 
		 You can reimplement this but always call KexiMigrate::isValid() implementation. */
		virtual bool isValid();

	signals:
		void progressPercent(int percent);

	protected:
		//! Used by MigrateManager.
		KexiMigrate(QObject *parent, const char *name, const QStringList &args = QStringList());

		//! Connect to source database (driver specific)
		virtual bool drv_connect() = 0;
		//! Disconnect from source database (driver specific)
		virtual bool drv_disconnect() = 0;

		//! Get table names in source database (driver specific)
		virtual bool drv_tableNames(QStringList& tablenames) = 0;

		//! Read schema for a given table (driver specific)
		virtual bool drv_readTableSchema(
			const QString& originalName, KexiDB::TableSchema& tableSchema) = 0;

		/*! Fetches maximum number from table \a tableName, column \a columnName 
		 into \a result. On success true is returned. If there is no records in the table, 
		 \a result is set to 0 and true is returned. 
		 - Note 1: implement only if the database can already contain kexidb__* tables
		   (so e.g. keximdb driver doea not need this).
		 - Note 2: default implementation uses drv_querySingleStringFromSQL() 
		   with "SELECT MAX(columName) FROM tableName" statement, assuming SQL-compliant
		   backend. 
		*/
		virtual bool drv_queryMaxNumber(const QString& tableName, 
			const QString& columnName, int& result);

		/*! Fetches single string at column \a columnNumber for each record from result obtained 
		 by running \a sqlStatement. \a numRecords can be specified to limit number of records read.
		 If \a numRecords is -1, all records are loaded.
		 On success the result is stored in \a stringList and true is returned. 
		 \return cancelled if there are no records available. 
		 - Note: implement only if the database can already contain kexidb__* tables
		  (so e.g. keximdb driver does not need this). */
//! @todo SQL-dependent!
		virtual tristate drv_queryStringListFromSQL(
			const QString& sqlStatement, uint columnNumber, QStringList& stringList, int numRecords = -1)
		 { return cancelled; }

		/*! Fetches single string at column \a columnNumber from result obtained 
		 by running \a sqlStatement.
		 On success the result is stored in \a string and true is returned. 
		 \return cancelled if there are no records available. 
		 This implementation uses drv_queryStringListFromSQL() with numRecords == 1. */
//! @todo SQL-dependent!
		virtual tristate drv_querySingleStringFromSQL(const QString& sqlStatement, 
			uint columnNumber, QString& string);

		/*! Fetches single record from result obtained 
		 by running \a sqlStatement.
		 \a firstRecord should be first initialized to true, so the method can run
		 the query at first call and then set it will set \a firstRecord to false,
		 so subsequent calls will only fetch records.
		 On success the result is stored in \a data and true is returned,
		 \a data is resized to appropriate size. cancelled is returned on EOF. */
//! @todo SQL-dependent!
		virtual tristate drv_fetchRecordFromSQL(const QString& sqlStatement, 
			KexiDB::RowData& data, bool &firstRecord)
		 { return cancelled; }

		//! Copy a table from source DB to target DB (driver specific)
		//! - create copies of KexiDB tables
		//! - create copies of non-KexiDB tables
		virtual bool drv_copyTable(const QString& srcTable, KexiDB::Connection *destConn, 
			KexiDB::TableSchema* dstTable) = 0;

		virtual bool drv_progressSupported() { return false; }

		/*! \return the size of a table to be imported, or 0 if not supported
			Finds the size of the named table, in order to provide feedback on
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
		virtual bool drv_getTableSize(const QString&, quint64&)
		{ return false; }

		void updateProgress(quint64 step = 1ULL);

//! @todo user should be asked ONCE using a convenient wizard's page, not a popup dialog
		//! Prompt user to select a field type for unrecognized fields
		KexiDB::Field::Type userType(const QString& fname);

		virtual QString drv_escapeIdentifier( const QString& str ) const {
			return m_kexiDBDriver ? m_kexiDBDriver->escapeIdentifier(str) : str; }

//! @todo Remove this! KexiMigrate should be usable for multiple concurrent migrations!
		//! Migrate Options
		KexiMigration::Data* m_migrateData;

//			// Temporary values used during import (set by driver specific methods)
//			KexiDB::Field* m_f;

		/*! Driver properties dictionary (indexed by name), 
		 useful for presenting properties to the user. 
		 Set available properties here in driver implementation. */
		QMap<Q3CString,QVariant> m_properties;

		/*! i18n'd captions for properties. You do not need 
		 to set predefined properties' caption in driver implementation 
		 -it's done automatically. */
		QMap<Q3CString,QString> m_propertyCaptions;

		//! KexiDB driver. For instance, it is used for escaping identifiers
		QPointer<KexiDB::Driver> m_kexiDBDriver;

	private:
		//! Get the list of tables
		bool tableNames(QStringList& tablenames);

		//! Create the target database project
		KexiProject* createProject(Kexi::ObjectStatus* result);

//		//Private data members
//		//! Flag indicating whether data should be copied
//		bool m_keepData;

		//! Table schemas from source DB
		Q3PtrList<KexiDB::TableSchema> m_tableSchemas;

		QPtrList<KexiDB::TableSchema> m_kexiDBCompatibleTableSchemasToRemoveFromMemoryAfterImport;

		/*! Estimate size of migration job
		 Calls drv_getTableSize for each table to be copied.
		 \return sum of the size of all tables to be copied.
		*/
		bool progressInitialise();

		KexiProject *m_destPrj;

		//! Size of migration job
		quint64 m_progressTotal;

		//! Amount of migration job complete
		quint64 m_progressDone;

		//! Don't recalculate progress done until this value is reached.
		quint64 m_progressNextReport;

		friend class MigrateManager;
};

} //namespace KexiMigration

//! Driver's static version information (implementation), 
//! with KLibFactory symbol declaration.
#define KEXIMIGRATE_DRIVER_INFO( class_name, internal_name ) \
	int class_name::versionMajor() const { return KEXI_MIGRATION_VERSION_MAJOR; } \
	int class_name::versionMinor() const { return KEXI_MIGRATION_VERSION_MINOR; } \
	K_EXPORT_COMPONENT_FACTORY(keximigrate_ ## internal_name, \
	  KGenericFactory<KexiMigration::class_name>( "keximigrate_" #internal_name ))

/*! Driver's static version information, automatically implemented for KexiDB drivers.
 Put this into migration driver class declaration just like Q_OBJECT macro. */
#define KEXIMIGRATION_DRIVER \
	public: \
	virtual int versionMajor() const; \
	virtual int versionMinor() const;

#endif

