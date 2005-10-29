/* This file is part of the KDE project
   Copyright (C) 2004 Adam Pigg <adam@piggz.co.uk>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>
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

using namespace KexiDB;
using namespace KexiMigration;

KexiMigrate::KexiMigrate()
{
}

KexiMigrate::KexiMigrate(QObject *parent, const char *name,
  const QStringList&) 
  : QObject( parent, name )
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
}

//=============================================================================
// Perform Import operation
bool KexiMigrate::performImport()
{
	QStringList tables;

	// Step 1 - connect
	kdDebug() << "KexiMigrate::performImport() CONNECTING..." << endl;
	if (!drv_connect()) {
		kdDebug() << "Couldnt connect to database server" << endl;
		return false;
	}

	// Step 2 - get table names
	kdDebug() << "KexiMigrate::performImport() GETTING TABLENAMES..." << endl;
	if (!tableNames(tables)) {
		kdDebug() << "Couldnt get list of tables" << endl;
		return false;
	}

	// Check if there are any tables
	if (tables.isEmpty()) {
		kdDebug() << "There were no tables to import" << endl;
		return false;
	}

	// Step 3 - Read table schemas
	foreach(QStringList::ConstIterator, it, tables) {
		if (m_migrateData->dest->driver()->isSystemObjectName( *it ))
			continue;

		const QString tableName( KexiUtils::string2Identifier(*it) );
		KexiDB::TableSchema *tableSchema;
		if (tableName.lower().startsWith("kexi__"))
			tableSchema = new KexiDB::InternalTableSchema(tableName);
		else
			tableSchema = new KexiDB::TableSchema(tableName);

		tableSchema->setCaption( *it ); //caption is equal to original name

		if (drv_readTableSchema(*tableSchema)) {
			//yeah, got a table
			//Add it to list of tables which we will create if all goes well
			m_tableSchemas.append(tableSchema);
		} else {
			delete tableSchema;
			return false;
		}
	}

	// Step 4 - Create new database as we have all required info
	if (!createDatabase(m_migrateData->destName)) {
		return false;
	}

	// Step 5 - Copy data if asked to
	bool ok = true;
	if (m_migrateData->keepData)
	{
		KexiDB::Transaction trans = m_migrateData->dest->beginTransaction();
		//copy data for these two as well
		m_tableSchemas.append(m_migrateData->dest->tableSchema("kexi__objects")); 
		m_tableSchemas.append(m_migrateData->dest->tableSchema("kexi__objectdata")); 
		for(QPtrListIterator<TableSchema> ts (m_tableSchemas); ts.current() != 0 ; ++ts)
		{
			const QString tname( ts.current()->name().lower() );
			if (m_migrateData->dest->driver()->isSystemObjectName( tname )
//! @todo what if these two tables are not compatible with tables created in detination db
//! because newer db format was used?
				&& tname!="kexi__objectdata" //copy this too
				&& tname!="kexi__objects" //copy this too
			)
			{
				kdDebug() << "Do not copy data for system table: " << tname << endl;
//! @todo copy kexi__db contents!
				continue;
			}
			if (tname=="kexi__objects") {
				// Special case: 
				// kexi__objects table has been filled while creating tables in destination db
				// remove these rows before copying
				ok = m_migrateData->dest->executeSQL("DELETE FROM kexi__objects");
				if (!ok)
					break;
			}
			kdDebug() << "Copying data for table: " << tname << endl;
			ok = drv_copyTable(tname, ts);
			if (!ok) {
				kdDebug() << "Failed to copy table " << tname << endl;
				break;
			}
		}
		if (ok) {
			ok = m_migrateData->dest->commitTransaction(trans);
		}
		if (!ok) {
			m_migrateData->dest->rollbackTransaction(trans);
			m_migrateData->dest->debugError();
			drv_disconnect();
			m_migrateData->dest->disconnect();
			m_migrateData->dest->dropDatabase(m_migrateData->destName);
			return false;
		}
	}
	
	return drv_disconnect() && m_migrateData->dest->disconnect();
}

//=============================================================================
// Copy a table
/*
bool KexiMigrate::copyData(const QString& table, 
                           KexiDB::TableSchema* dstTable) {
	kdDebug() << "Copying table " << table << endl;
	drv_copyTable(table, dstTable);
	return true;
}*/

//=============================================================================
// Create the final database
bool KexiMigrate::createDatabase(const QString& dbname)
{
	kdDebug() << "Creating database [" << dbname << "]" << endl;
	m_migrateData->dest->connect(); //(not always succeeds)
//	if(!m_migrateData->dest->connect()) {
//		kdDebug() << "Couldnt connect to destination database" << endl;
//		return false;
//	}

	if(m_migrateData->dest->databaseExists(dbname)) {
		//drop before recreating (user confirmed overwriting)
//! todo for file-based databases we can use tmp filename and rename later after success...
		if (!m_migrateData->dest->dropDatabase(dbname)) {
			return false;
		}
	}

	if(!m_migrateData->dest->createDatabase(dbname)) {
		kdDebug() << "Couldnt create database at destination" << endl;
		return false;
	}
	
	if (!m_migrateData->dest->useDatabase(dbname)) {
		kdDebug() << "Couldnt use newly created database" << endl;
		m_migrateData->dest->dropDatabase(dbname);
		m_migrateData->dest->disconnect();
		return false;
	}

	KexiDB::TransactionGuard tg(*m_migrateData->dest);
	if (tg.transaction().isNull()) {
		m_migrateData->dest->dropDatabase(dbname);
		m_migrateData->dest->disconnect();
		return false;
	}

	if(drv_progressSupported()) {
		progressInitialise();
	}

	//Right, were connected..create the tables
	KexiDB::TableSchema *ts;
	for(QPtrListIterator<TableSchema> it (m_tableSchemas); (ts = it.current()) != 0;++it) {
/*		if (m_migrateData->dest->driver()->isSystemObjectName( ts->name() )
//no longer true:
//			// kexi__parts table was created at kexidb level but in fact it's for kexi
//			|| ts->name().lower()=="kexi__parts"
			)
		{
			++it;
			continue;
		} */
		if(!m_migrateData->dest->createTable( ts )) {
			kdDebug() << "Failed to create a table " << ts->name() << endl;
			m_migrateData->dest->debugError();
			m_tableSchemas.remove(ts);
//			delete ts;
			m_migrateData->dest->dropDatabase(dbname);
			m_migrateData->dest->disconnect();
			return false;
		}
		updateProgress((Q_ULLONG)NUM_OF_ROWS_PER_CREATE_TABLE);
//		if (ts->name()=="kexi__objects") {
//			m_copyOfKexi__objects = new KexiDB::TableSchema(*ts);
//		}
	}
	tg.commit();
	return true;
}

//=============================================================================
// Functions for getting table data
bool KexiMigrate::tableNames(QStringList & tn)
{
	//! @todo Cache list of table names
	kdDebug() << "Reading list of tables..." << endl;
	return drv_tableNames(tn);
}

/*bool KexiMigrate::readTableSchema(const QString& table)
{
	kdDebug() << "Reading table schema for [" << table << "]" << endl;
	return drv_readTableSchema(table);
}*/


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
	int fakePercent = 0; //this task will take 5 percents of progress...
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
	progressTotal = sum;
	progressTotal += tables.count() * NUM_OF_ROWS_PER_CREATE_TABLE;
	progressTotal = progressTotal * 105 / 100; //add 5 percent for above task 1)
	progressNextReport = sum / 100;
	progressDone = progressTotal * 5 / 100; //5 perecent already done in task 1)
	return true;
}


void KexiMigrate::updateProgress(Q_ULLONG step) {
	progressDone += step;
	if (progressDone >= progressNextReport) {
		int percent = (progressDone+1) * 100 / progressTotal;
		progressNextReport = ((percent + 1) * progressTotal) / 100;
		kdDebug() << "KexiMigrate::updateProgress(): " << (ulong)progressDone << "/"
		          << (ulong)progressTotal << " (" << percent << "%) next report at " 
		          << (ulong)progressNextReport << endl;
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

#include "keximigrate.moc"
