/* This file is part of the KDE project
   Copyright (C) 2004 Adam Pigg <adam@piggz.co.uk>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>
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

#include "keximigrate.h"

#include <kdebug.h>
#include <kinputdialog.h>
#include <kapplication.h>

#include <kexiutils/identifier.h>
#include <core/kexi.h>
#include <core/kexiproject.h>
#include <kexidb/drivermanager.h>
#include <kexidb/utils.h>
#include <q3tl.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3CString>

using namespace KexiDB;
using namespace KexiMigration;

KexiMigrate::KexiMigrate(QObject *parent, const char *name,
  const QStringList&) 
  : QObject( parent, name )
  , m_migrateData(0)
  , m_destPrj(0)
{
}

//! Used for computing progress: 
//! let's assume that each table creation costs the same as inserting 20 rows
#define NUM_OF_ROWS_PER_CREATE_TABLE 20


//=============================================================================
// Migration parameters
void KexiMigrate::setData(KexiMigration::Data* migrateData)
{
	m_migrateData = migrateData;
}

//=============================================================================
// Destructor
KexiMigrate::~KexiMigrate()
{
	delete m_destPrj;
}

bool KexiMigrate::checkIfDestinationDatabaseOverwritingNeedsAccepting(Kexi::ObjectStatus* result, 
	bool& acceptingNeeded)
{
	acceptingNeeded = false;
	if (result)
		result->clearStatus();

	KexiDB::DriverManager drvManager;
	KexiDB::Driver *destDriver = drvManager.driver(
		m_migrateData->destination->connectionData()->driverName);
	if (!destDriver) {
		result->setStatus(&drvManager,
			i18n("Could not create database \"%1\".")
			.arg(m_migrateData->destination->databaseName()));
		return false;
	}

	// For file-based dest. projects, we've already asked about overwriting 
	// existing project but for server-based projects we need to ask now.
	if (destDriver->isFileDriver())
		return true; //nothing to check
	KexiDB::Connection *tmpConn 
		= destDriver->createConnection( *m_migrateData->destination->connectionData() );
	if (!tmpConn || destDriver->error() || !tmpConn->connect()) {
		delete tmpConn;
		return true;
	}
	if (tmpConn->databaseExists( m_migrateData->destination->databaseName() )) {
		acceptingNeeded = true;
	}
	tmpConn->disconnect();
	delete tmpConn;
	return true;
}

bool KexiMigrate::isSourceAndDestinationDataSourceTheSame() const
{
	KexiDB::ConnectionData* sourcedata = m_migrateData->source;
	KexiDB::ConnectionData* destinationdata = m_migrateData->destination->connectionData();
	return (
		sourcedata && destinationdata &&
		m_migrateData->sourceName == m_migrateData->destination->databaseName() && // same database name
		sourcedata->driverName == destinationdata->driverName && // same driver
		sourcedata->hostName == destinationdata->hostName && // same host
		sourcedata->fileName() == destinationdata->fileName() && // same filename
		sourcedata->dbPath() == destinationdata->dbPath() && // same database path
		sourcedata->dbFileName() == destinationdata->dbFileName() // same database filename
	);
}

//=============================================================================
// Perform Import operation
bool KexiMigrate::performImport(Kexi::ObjectStatus* result)
{
	if (result)
		result->clearStatus();

	KexiDB::DriverManager drvManager;
	KexiDB::Driver *destDriver = drvManager.driver(
		m_migrateData->destination->connectionData()->driverName);
	if (!destDriver) {
		result->setStatus(&drvManager,
			i18n("Could not create database \"%1\".")
			.arg(m_migrateData->destination->databaseName()));
		return false;
	}

	QStringList tables;

	// Step 1 - connect
	kDebug() << "KexiMigrate::performImport() CONNECTING..." << endl;
	if (!drv_connect()) {
		kDebug() << "Couldnt connect to database server" << endl;
		if (result)
			result->setStatus(i18n("Could not connect to data source \"%1\".")
				.arg(m_migrateData->source->serverInfoString()), "");
		return false;
	}

	// Step 2 - get table names
	kDebug() << "KexiMigrate::performImport() GETTING TABLENAMES..." << endl;
	if (!tableNames(tables)) {
		kDebug() << "Couldnt get list of tables" << endl;
		if (result)
			result->setStatus(
				i18n("Could not get a list of table names for data source \"%1\".")
					.arg(m_migrateData->source->serverInfoString()), "");
		return false;
	}

	//tmp to force error!!!
	//tables.prepend("$$$$$");

	// Check if there are any tables
	if (tables.isEmpty()) {
		kDebug() << "There were no tables to import" << endl;
		if (result)
			result->setStatus(
				i18n("No tables to import found in data source \"%1\".")
					.arg(m_migrateData->source->serverInfoString()), "");
		return false;
	}

	// Step 3 - Read table schemas
	m_tableSchemas.clear();
	if (!destDriver) {
		result->setStatus(&drvManager);
		return false;
	}
	foreach(const QString tableName, tables) {
		if (destDriver->isSystemObjectName( tableName ) //"kexi__objects", etc.
			|| tableName.toLower().startsWith("kexi__")) //tables at KexiProject level, e.g. "kexi__blobs"
			continue;

		const QString tableNameId( KexiUtils::string2Identifier(tableName) );
		KexiDB::TableSchema *tableSchema;
//		if (tableName.toLower().startsWith("kexi__"))
//			tableSchema = new KexiDB::InternalTableSchema(tableName);
//		else
		tableSchema = new KexiDB::TableSchema(tableNameId);

		tableSchema->setCaption( tableName ); //caption is equal to the original name

		if (drv_readTableSchema(tableName, *tableSchema)) {
			//yeah, got a table
			//Add it to list of tables which we will create if all goes well
			m_tableSchemas.append(tableSchema);
		} else {
			delete tableSchema;
			if (result)
				result->setStatus(
					i18n("Could not import project from data source \"%1\". Error reading table \"%2\".")
					.arg(m_migrateData->source->serverInfoString()).arg(tableName), "");
			return false;
		}
	}

	// Step 4 - Create new database as we have all required info
	delete m_destPrj;
	m_destPrj = createProject(result);
	if (!m_destPrj || m_destPrj->error()) {
		if (result)
			result->setStatus(m_destPrj,
				i18n("Could not import project from data source \"%1\".")
				.arg(m_migrateData->source->serverInfoString()));
		return false;
	}

	// Step 5 - Copy data if asked to
	bool ok = true;
	KexiDB::Transaction trans;
	if (!m_migrateData->keepData)
		m_tableSchemas.clear();

	KexiDB::Connection *destConn = m_destPrj->dbConnection();
	ok = destConn;
	if (ok) {
		trans = destConn->beginTransaction();
		ok = !trans.isNull();
	}
	if (ok) {
		// Copy data for "kexi__objectdata" as well, if available in the source db
		if (tables.contains("kexi__objectdata"))
			m_tableSchemas.append(destConn->tableSchema("kexi__objectdata")); 
	}

	for(Q3PtrListIterator<TableSchema> ts(m_tableSchemas); ok && ts.current() != 0 ; ++ts)
	{
		const QString tname( ts.current()->name().toLower() );
		if (destConn->driver()->isSystemObjectName( tname )
//! @todo what if these two tables are not compatible with tables created in detination db
//! because newer db format was used?
			&& tname!="kexi__objectdata" //copy this too
		)
		{
			kDebug() << "Do not copy data for system table: " << tname << endl;
//! @todo copy kexi__db contents!
			continue;
		}
		kDebug() << "Copying data for table: " << tname << endl;
		ok = drv_copyTable(
			ts.current()->caption().isEmpty() ? tname : ts.current()->caption(), //caption is equal to the original name
			destConn, 
			ts.current()
		);
		if (!ok) {
			kDebug() << "Failed to copy table " << tname << endl;
			if (result)
				result->setStatus(destConn,
					i18n("Could not copy table \"%1\" to destination database.").arg(tname));
			break;
		}
	}//for

	// 5.1. Copy remaining "kexi__objects" contents (queries, forms, etc.) 
	//      if "kexi__objects" table is available in the source db
	if (ok && tables.contains("kexi__objects")) {
		// At 'source' side, we can only can use drv_copyTable, so let's create 
		// a temporary copy of "kexi__objects" and copy everything there
		KexiDB::TableSchema *kexi__objectsCopy = 
			new KexiDB::TableSchema( *destConn->tableSchema("kexi__objects") );
		kexi__objectsCopy->setName("kexi__objects__copy");
		ok = destConn->createTable( kexi__objectsCopy );
		if (!ok) {
			kDebug() << "Failed to create a table " << kexi__objectsCopy->name() << endl;
			delete kexi__objectsCopy;
			kexi__objectsCopy = 0;
			destConn->debugError();
			if (result)
				result->setStatus(destConn,
					i18n("Could not create database \"%1\".")
					.arg(m_migrateData->destination->databaseName()));
		}
		if (ok) {
			ok = drv_copyTable("kexi__objects", destConn, kexi__objectsCopy);
		}
		//! Copy every row not being of a type 'table'
//! @todo this is not portable to non-SQL target engines!
		if (ok) {
			ok = destConn->executeSQL(
				QString::fromLatin1("INSERT INTO kexi__objects SELECT * FROM kexi__objects__copy "
					"WHERE o_type<>%1").arg((int)KexiDB::TableObjectType));
		}
		if (kexi__objectsCopy && !destConn->dropTable( kexi__objectsCopy )) {
			ok = false;
		}
	}

	// Done.
	if (ok) {
		ok = destConn->commitTransaction(trans);
	}

	if (ok)
		ok = drv_disconnect(); // && m_migrateData->dest->disconnect();

	if (!ok) {
		if (result && result->error())
			result->setStatus(destConn,
				i18n("Could not import data from data source \"%1\".")
					.arg(m_migrateData->source->serverInfoString()));
		if (destConn) {
			destConn->debugError();
			destConn->rollbackTransaction(trans);
		}
		drv_disconnect();
		if (destConn) {
			destConn->disconnect();
			destConn->dropDatabase(m_migrateData->destination->databaseName());
		}
		//later		delete prj;
		return false;
	}
	if (destConn)
		ok = destConn->disconnect();
	//later	delete prj;
	return ok;
}
//=============================================================================

bool KexiMigrate::performExport(Kexi::ObjectStatus* result)
{
	if (result)
		result->clearStatus();

	//! @todo performExport

	return false;
}

//=============================================================================
// Create the final database project
KexiProject *KexiMigrate::createProject(Kexi::ObjectStatus* result)
{
	kDebug() << "Creating database [" << m_migrateData->destination->databaseName() 
		<< "]" << endl;

	KexiProject *prj = new KexiProject(m_migrateData->destination,
		(KexiDB::MessageHandler*)*result);
	tristate r = prj->create(true /*forceOverwrite*/);
	if (r!=true) {
		//don't delete prj, otherwise eror message will be deleted		delete prj;
		return prj;
	}

	KexiDB::TransactionGuard tg(*prj->dbConnection());
	if (tg.transaction().isNull()) {
		if (result)
			result->setStatus(prj->dbConnection(),
				i18n("Could not create database \"%1\".")
				.arg(m_migrateData->destination->databaseName()));
		prj->dbConnection()->dropDatabase(m_migrateData->destination->databaseName());
		//don't delete prj, otherwise eror message will be deleted		delete prj;
		return prj;
	}

	if(drv_progressSupported()) {
		progressInitialise();
	}

	//Right, were connected..create the tables
	KexiDB::TableSchema *ts;
	for(Q3PtrListIterator<TableSchema> it (m_tableSchemas); (ts = it.current()) != 0;++it) {
		if(!prj->dbConnection()->createTable( ts )) {
			kDebug() << "Failed to create a table " << ts->name() << endl;
			prj->dbConnection()->debugError();
			if (result)
				result->setStatus(prj->dbConnection(),
					i18n("Could not create database \"%1\".")
					.arg(m_migrateData->destination->databaseName()));
			m_tableSchemas.remove(ts);
			prj->dbConnection()->dropDatabase(m_migrateData->destination->databaseName());
			//don't delete prj, otherwise eror message will be deleted			delete prj;
			return prj;
		}
		updateProgress((quint64)NUM_OF_ROWS_PER_CREATE_TABLE);
	}
	if (!tg.commit()) {
		prj->dbConnection()->dropDatabase(m_migrateData->destination->databaseName());
		//don't delete prj, otherwise eror message will be deleted		delete prj;
		return prj;
	}
	return prj;
}

//=============================================================================
// Functions for getting table data
bool KexiMigrate::tableNames(QStringList & tn)
{
	//! @todo Cache list of table names
	kDebug() << "Reading list of tables..." << endl;
	return drv_tableNames(tn);
}

//=============================================================================
// Progress functions
bool KexiMigrate::progressInitialise() {
	quint64 sum = 0, size;
	emit progressPercent(0);

  //! @todo Don't copy table names here
	QStringList tables;
	if(!tableNames(tables))
		return false;

	// 1) Get the number of rows/bytes to import
	int tableNumber = 1;
	for(QStringList::Iterator it = tables.begin();
	    it != tables.end(); ++it, tableNumber++)
	{
		if(drv_getTableSize(*it, size)) {
			kDebug() << "KexiMigrate::progressInitialise() - table: " << *it 
			          << "size: " << (ulong)size << endl;
			sum += size;
			emit progressPercent(tableNumber * 5 /* 5% */ / tables.count());
		} else {
			return false;
		}
	}

	kDebug() << "KexiMigrate::progressInitialise() - job size: " << (ulong)sum << endl;
	m_progressTotal = sum;
	m_progressTotal += tables.count() * NUM_OF_ROWS_PER_CREATE_TABLE;
	m_progressTotal = m_progressTotal * 105 / 100; //add 5 percent for above task 1)
	m_progressNextReport = sum / 100;
	m_progressDone = m_progressTotal * 5 / 100; //5 perecent already done in task 1)
	return true;
}


void KexiMigrate::updateProgress(quint64 step) {
	m_progressDone += step;
	if (m_progressDone >= m_progressNextReport) {
		int percent = (m_progressDone+1) * 100 / m_progressTotal;
		m_progressNextReport = ((percent + 1) * m_progressTotal) / 100;
		kDebug() << "KexiMigrate::updateProgress(): " << (ulong)m_progressDone << "/"
		          << (ulong)m_progressTotal << " (" << percent << "%) next report at " 
		          << (ulong)m_progressNextReport << endl;
		emit progressPercent(percent);
	}
}

//=============================================================================
// Prompt the user to choose a field type
KexiDB::Field::Type KexiMigrate::userType(const QString& fname)
{
	QStringList types;
	for (uint i = KexiDB::Field::InvalidType+1; i <= KexiDB::Field::LastType; i++)
		types.append( KexiDB::Field::typeName((KexiDB::Field::Type)i) );

	bool ok;
	const QString res = KInputDialog::getItem( i18n("Select Field Type"),
		i18n("The data type for %1 could not be determined. "
		"Please select one of the following data "
		"types").arg(fname), types, 0, false, &ok);

//! @todo use KexiDB::Field::typeForString()
	const int index = types.indexOf( res );
	if (index==-1 || !ok)
		return KexiDB::Field::InvalidType;
	const KexiDB::Field::Type t = KexiDB::intToFieldType( 
		(int)KexiDB::Field::InvalidType + 1 + index );
	return t==KexiDB::Field::InvalidType ? KexiDB::Field::Text : t;
}

QVariant KexiMigrate::propertyValue( const Q3CString& propName )
{
	return m_properties[propName.toLower()];
}

QString KexiMigrate::propertyCaption( const Q3CString& propName ) const
{
	return m_propertyCaptions[propName.toLower()];
}

void KexiMigrate::setPropertyValue( const Q3CString& propName, const QVariant& value )
{
	m_properties[propName.toLower()] = value;
}

Q3ValueList<Q3CString> KexiMigrate::propertyNames() const
{
	Q3ValueList<Q3CString> names = m_properties.keys();
	qHeapSort(names);
	return names;
}

bool KexiMigrate::isValid()
{
	if (KexiMigration::versionMajor() != versionMajor()
		|| KexiMigration::versionMinor() != versionMinor())
	{
		setError(ERR_INCOMPAT_DRIVER_VERSION,
		i18n("Incompatible migration driver's \"%1\" version: found version %2, expected version %3.")
		.arg(objectName())
		.arg(QString("%1.%2").arg(versionMajor()).arg(versionMinor()))
		.arg(QString("%1.%2").arg(KexiMigration::versionMajor()).arg(KexiMigration::versionMinor())));
		return false;
	}
	return true;
}

#include "keximigrate.moc"
