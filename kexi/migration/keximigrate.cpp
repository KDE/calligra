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

using namespace KexiDB;
using namespace KexiMigration;

KexiMigrate::KexiMigrate(QObject *parent, const char *name,
  const QStringList&) 
  : QObject( parent, name )
  , m_migrateData(0)
  , m_destPrj(0)
//  , m_copyOfKexi__objects(0)
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
	kdDebug() << "KexiMigrate::performImport() CONNECTING..." << endl;
	if (!drv_connect()) {
		kdDebug() << "Couldnt connect to database server" << endl;
		if (result)
			result->setStatus(i18n("Could not connect to data source \"%1\".")
				.arg(m_migrateData->source->serverInfoString()), "");
		return false;
	}

	// Step 2 - get table names
	kdDebug() << "KexiMigrate::performImport() GETTING TABLENAMES..." << endl;
	if (!tableNames(tables)) {
		kdDebug() << "Couldnt get list of tables" << endl;
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
		kdDebug() << "There were no tables to import" << endl;
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
	foreach(QStringList::ConstIterator, it, tables) {
		if (destDriver->isSystemObjectName( *it ) //"kexi__objects", etc.
			|| (*it).lower().startsWith("kexi__")) //tables at KexiProject level, e.g. "kexi__blobs"
			continue;

		const QString tableName( KexiUtils::string2Identifier(*it) );
		KexiDB::TableSchema *tableSchema;
//		if (tableName.lower().startsWith("kexi__"))
//			tableSchema = new KexiDB::InternalTableSchema(tableName);
//		else
		tableSchema = new KexiDB::TableSchema(tableName);

		tableSchema->setCaption( *it ); //caption is equal to the original name

		if (drv_readTableSchema(*it, *tableSchema)) {
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
		if (tables.find("kexi__objectdata")!=tables.end())
			m_tableSchemas.append(destConn->tableSchema("kexi__objectdata")); 
	}

	for(QPtrListIterator<TableSchema> ts(m_tableSchemas); ok && ts.current() != 0 ; ++ts)
	{
		const QString tname( ts.current()->name().lower() );
		if (destConn->driver()->isSystemObjectName( tname )
//! @todo what if these two tables are not compatible with tables created in detination db
//! because newer db format was used?
			&& tname!="kexi__objectdata" //copy this too
		)
		{
			kdDebug() << "Do not copy data for system table: " << tname << endl;
//! @todo copy kexi__db contents!
			continue;
		}
		kdDebug() << "Copying data for table: " << tname << endl;
		ok = drv_copyTable(
			ts.current()->caption().isEmpty() ? tname : ts.current()->caption(), //caption is equal to the original name
			destConn, 
			ts.current()
		);
		if (!ok) {
			kdDebug() << "Failed to copy table " << tname << endl;
			if (result)
				result->setStatus(destConn,
					i18n("Could not copy table \"%1\" to destination database.").arg(tname));
			break;
		}
	}//for

	// 5.1. Copy remaining "kexi__objects" contents (queries, forms, etc.) 
	//      if "kexi__objects" table is available in the source db
	if (ok && tables.find("kexi__objects")!=tables.end()) {
		// At 'source' side, we can only can use drv_copyTable, so let's create 
		// a temporary copy of "kexi__objects" and copy everything there
		KexiDB::TableSchema *kexi__objectsCopy = 
			new KexiDB::TableSchema( *destConn->tableSchema("kexi__objects") );
		kexi__objectsCopy->setName("kexi__objects__copy");
		ok = destConn->createTable( kexi__objectsCopy );
		if (!ok) {
			kdDebug() << "Failed to create a table " << kexi__objectsCopy->name() << endl;
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
	kdDebug() << "Creating database [" << m_migrateData->destination->databaseName() 
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
	for(QPtrListIterator<TableSchema> it (m_tableSchemas); (ts = it.current()) != 0;++it) {
		if(!prj->dbConnection()->createTable( ts )) {
			kdDebug() << "Failed to create a table " << ts->name() << endl;
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
		if(drv_progressSupported()) {
			updateProgress((Q_ULLONG)NUM_OF_ROWS_PER_CREATE_TABLE);
		}
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
	kdDebug() << "Reading list of tables..." << endl;
	return drv_tableNames(tn);
}

//=============================================================================
// Progress functions
bool KexiMigrate::progressInitialise() {
	Q_ULLONG sum = 0, size;
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
			kdDebug() << "KexiMigrate::progressInitialise() - table: " << *it 
			          << "size: " << (ulong)size << endl;
			sum += size;
			emit progressPercent(tableNumber * 5 /* 5% */ / tables.count());
		} else {
			return false;
		}
	}

	kdDebug() << "KexiMigrate::progressInitialise() - job size: " << (ulong)sum << endl;
	m_progressTotal = sum;
	m_progressTotal += tables.count() * NUM_OF_ROWS_PER_CREATE_TABLE;
	m_progressTotal = m_progressTotal * 105 / 100; //add 5 percent for above task 1)
	m_progressNextReport = sum / 100;
	m_progressDone = m_progressTotal * 5 / 100; //5 perecent already done in task 1)
	return true;
}


void KexiMigrate::updateProgress(Q_ULLONG step) {
	m_progressDone += step;
	if (m_progressDone >= m_progressNextReport) {
		int percent = (m_progressDone+1) * 100 / m_progressTotal;
		m_progressNextReport = ((percent + 1) * m_progressTotal) / 100;
		kdDebug() << "KexiMigrate::updateProgress(): " << (ulong)m_progressDone << "/"
		          << (ulong)m_progressTotal << " (" << percent << "%) next report at " 
		          << (ulong)m_progressNextReport << endl;
		emit progressPercent(percent);
	}
}

//=============================================================================
// Prompt the user to choose a field type
KexiDB::Field::Type KexiMigrate::userType(const QString& fname)
{
	KInputDialog *dlg;
	QStringList  types;
	QString res;

	types << "Byte";
	types << "Short Integer";
	types << "Integer";
	types << "Big Integer";
	types << "Boolean";
	types << "Date";
	types << "Date Time";
	types << "Time";
	types << "Float";
	types << "Double";
	types << "Text";
	types << "Long Text";
	types << "Binary Large Object";

	res = dlg->getItem( i18n("Field Type"),
	                    i18n("The data type for %1 could not be determined. "
				 "Please select one of the following data "
				 "types").arg(fname),
                      types, 0, false);

//! @todo use QMap<QCString, KexiDB::Field::Type> here!
	if (res == types[0])
		return KexiDB::Field::Byte;
	else if (res == types[1])
		return KexiDB::Field::ShortInteger;
	else if (res == types[2])
		return KexiDB::Field::Integer;
	else if (res == types[3])
		return KexiDB::Field::BigInteger;
	else if (res == types[4])
		return KexiDB::Field::Boolean;
	else if (res == types[5])
		return KexiDB::Field::Date;
	else if (res == types[6])
		return KexiDB::Field::DateTime;
	else if (res == types[7])
		return KexiDB::Field::Time;
	else if (res == types[8])
		return KexiDB::Field::Float;
	else if (res == types[9])
		return KexiDB::Field::Double;
	else if (res == types[10])
		return KexiDB::Field::Text;
	else if (res == types[11])
		return KexiDB::Field::LongText;
	else if (res == types[12])
		return KexiDB::Field::BLOB;
	else
		return KexiDB::Field::Text;
}

QVariant KexiMigrate::propertyValue( const QCString& propName )
{
	return m_properties[propName.lower()];
}

QString KexiMigrate::propertyCaption( const QCString& propName ) const
{
	return m_propertyCaptions[propName.lower()];
}

void KexiMigrate::setPropertyValue( const QCString& propName, const QVariant& value )
{
	m_properties[propName.lower()] = value;
}

QValueList<QCString> KexiMigrate::propertyNames() const
{
	QValueList<QCString> names = m_properties.keys();
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
		.arg(name())
		.arg(QString("%1.%2").arg(versionMajor()).arg(versionMinor()))
		.arg(QString("%1.%2").arg(KexiMigration::versionMajor()).arg(KexiMigration::versionMinor())));
		return false;
	}
	return true;
}

#include "keximigrate.moc"
