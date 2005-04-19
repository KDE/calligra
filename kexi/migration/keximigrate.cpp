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

#include "keximigrate.h"
#include <kdebug.h>
#include <kinputdialog.h>
#include <kapplication.h>

using namespace KexiDB;
using namespace KexiMigration;

KexiMigrate::KexiMigrate()
{
}

KexiMigrate::KexiMigrate(QObject *parent, const char *name,
  const QStringList&) : QObject( parent, name )
{
}

//=============================================================================
// Migration parameters
void KexiMigrate::setData(KexiDB::ConnectionData* externalConnectionData, QString dbname, KexiDB::Connection* kexiConnection, QString newdbname, bool keep_data)
{
	m_externalData = externalConnectionData;
	m_kexiDB = kexiConnection;
	m_keepData = keep_data;
	m_dbName = dbname;
	m_todbname = newdbname;
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
		if(readTableSchema(*it)) {
			//yeah, got a table
			//Add it to list of tables which we will create if all goes well
			m_tableSchemas.append(m_table);
		} else {
			return false;
		}
	}

  // Step 4 - Create new database as we have all required info
	if (!createDatabase(m_todbname)) {
		return false;
	}

	if(drv_progressSupported()) {
		progressInitialise();
	}


	for(QPtrListIterator<TableSchema> ts (m_tableSchemas); ts.current() != 0 ; ++ts) {
	kdDebug() << "Copying ... " << ts << endl;
		if(!copyData(ts.current()->name(), ts)) {
			kdDebug() << "Failed to copy table " << ts << endl;
			m_kexiDB->debugError();
			drv_disconnect();
			return false;
		}
	}

	drv_disconnect();
	return true;
}

//=============================================================================
// Copy a table
bool KexiMigrate::copyData(const QString& table, 
                           KexiDB::TableSchema* dstTable) {
	kdDebug() << "Copying table " << table << endl;
	drv_copyTable(table, dstTable);
	return true;
}

//=============================================================================
// Create the final database
bool KexiMigrate::createDatabase(const QString& dbname)
{
	bool failure = false;
	
	kdDebug() << "Creating database [" << dbname << "]" << endl;
	if(!m_kexiDB->connect()) {
		kdDebug() << "Couldnt connect to destination database" << endl;
		return false;
	}

	if(!m_kexiDB->createDatabase(dbname)) {
		kdDebug() << "Couldnt create database at destination" << endl;
		return false;
	}
	
	if (!m_kexiDB->useDatabase(dbname)) {
		kdDebug() << "Couldnt use newly created database" << endl;
		return false;
	}

	//Right, were connected..create the tables
	for(QPtrListIterator<TableSchema> ts (m_tableSchemas); ts.current() != 0 ; ++ts) {
		/*! @todo check this earlier: on creating table list! */
		if (m_kexiDB->driver()->isSystemObjectName( ts.current()->name() ))
			continue;
		if(!m_kexiDB->createTable( ts.current() )) {
			kdDebug() << "Failed to create a table" << ts.current() << endl;
			m_kexiDB->debugError();
			failure = true;
		}
	}
	return !failure;
}

//=============================================================================
// Functions for getting table data
bool KexiMigrate::tableNames(QStringList & tn)
{
	//! @todo Cache list of table names
	kdDebug() << "Reading list of tables..." << endl;
	return drv_tableNames(tn);
}

bool KexiMigrate::readTableSchema(const QString& table)
{
	kdDebug() << "Reading table schema for [" << table << "]" << endl;
	return drv_readTableSchema(table);
}


//=============================================================================
// Progress functions
bool KexiMigrate::progressInitialise() {
	Q_ULLONG sum = 0, size;
	bool success = true;

  //! @todo Don't copy table names here
	QStringList tables;
	if(!tableNames(tables))
		return false;

	// Get the number of rows/bytes to import
	for(QStringList::Iterator it = tables.begin();
	    it != tables.end() && success; ++it) {

		if(drv_getTableSize(*it, size)) {
			kdDebug() << "KexiMigrate::progressInitialise() - table: " << *it 
			          << "size: " << (ulong)size << endl;
			sum += size;
		} else {
			success = false;
		}

	}

	kdDebug() << "KexiMigrate::progressInitialise() - job size: " << (ulong)sum << endl;
	emit progressPercent(0);
	progressDone = 0;
	progressTotal = sum;
	progressNextReport = sum / 100;
	return success;
}


void KexiMigrate::progressDoneRow() {
	progressDone++;
	if (progressDone >= progressNextReport) {
		int percent = (progressDone+1) * 100 / progressTotal;
		progressNextReport = ((percent + 1) * progressTotal) / 100;
		kdDebug() << "KexiMigrate::progressDoneRow(): " << (ulong)progressDone << "/"
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
	                    i18n("I could not determine the data type for ") + fname +
                      i18n(".  Please  select on of the following data types"),
                      types, 0, false);

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
