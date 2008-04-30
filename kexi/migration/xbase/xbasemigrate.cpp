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

#include "xbasemigrate.h"

#include <qstring.h>
#include <qregexp.h>
#include <qfile.h>
#include <qvariant.h>
#include <q3valuelist.h>
#include <qdir.h>
#include <kdebug.h>

#include "migration/keximigratedata.h"
#include <kexidb/cursor.h>
#include <kexidb/field.h>
#include <kexidb/utils.h>
#include <kexidb/drivermanager.h>
#include <kexiutils/identifier.h>

using namespace KexiMigration;

/* This is the implementation for the xBase specific import routines. */

KEXIMIGRATE_DRIVER_INFO( xBaseMigrate, xbase )

//! Constructor (needed for trading interface)
xBaseMigrate::xBaseMigrate(QObject *parent, const QStringList &args) :
	KexiMigrate(parent, args)
{
	KexiDB::DriverManager manager;
	m_kexiDBDriver = manager.driver("xbase");
}

/* ************************************************************************** */
//! Destructor
xBaseMigrate::~xBaseMigrate()
{
}


/* ************************************************************************** */
/*! Connect to the db backend */
bool xBaseMigrate::drv_connect()
{
	// Get the xbase directory path
	Data* migrationData = data();
	KexiDB::ConnectionData* dataSource = migrationData->source;
	QString dbPath = dataSource->dbPath();

	QDir xBaseDirectory( dbPath );

	// set filters
	QStringList dbfFilters;
//! @todo use application/x-dbase mime type as soon as share mime db provides info on file extensions
	dbfFilters<<"*.dbf"<<"*.DBF";

	xBaseDirectory.setNameFilters( dbfFilters );
	QStringList dbfFiles = xBaseDirectory.entryList(); // set a readable files filter here ?

	foreach( QString fileName, dbfFiles ) {
		xbDbf* table = new xbDbf( this );
		int returnCode;
		// Calling OpenDatabase, will automatically add the pointer `table`
		// to the dbfList of xbXBase class ( if there is no error )
		QString absoluteFileName = xBaseDirectory.filePath( fileName );
		fileName.chop( 4 ); // remove the letters .dbf
		tableNamePathMap[fileName] = absoluteFileName;

		if (  ( returnCode = table->OpenDatabase( absoluteFileName.toUtf8().constData() ) ) != XB_NO_ERROR ) {
			switch( returnCode ) {
				case XB_OPEN_ERROR:
					kDebug()<<"Couldn't open "<<absoluteFileName<<".Skipping it.";
					break;
				case XB_NO_MEMORY:
					kDebug()<<"Memory allocation error in XBase library";
					return false;
				case XB_NOT_XBASE:
					kDebug()<<absoluteFileName<<" is not a DBF file.Skipping it.";
					break;
				default:
					break;
			}
			delete table;
		}
	}

	kDebug()<<"Successfully processed all the dbf files in the directory";
	//! TODO Open .NDX,.NTX, .DBT files too

	return true;
}


/*! Disconnect from the db backend */
bool xBaseMigrate::drv_disconnect()
{
	// delete all dbf pointers here ?
	xbDbList* tempDbfList = DbfList;

	while ( tempDbfList != NULL ) {
		xbDbf* currentDbf = tempDbfList->dbf;
		tempDbfList = tempDbfList->NextDbf;
		int returnCode;
		if ( ( returnCode = currentDbf->CloseDatabase() ) != XB_NO_ERROR ) {
			// File not open error
			kDebug()<<"File Not Open";
		}
		// delete currentDbf here ?
	}

	return true;
}


/* ************************************************************************** */
/*! Get the types and properties for each column. */
bool xBaseMigrate::drv_readTableSchema(
	const QString& originalName, KexiDB::TableSchema& tableSchema)
{
	// Steps
	// 1. Get the number of fields
	// 2. for i = 1 to no_of_fields
	// 3.	 Get the fieldName of the i th	field
	// 4.	 Generate a fieldId
	// 5.	 Create a KexiDB::Field object, using the fieldId and the fieldType ( you may need to write a type conversion function here )
	// 6.	 Examine enum fields of any
	// 7.	 Set the caption of the field
	// 8.	 Set other properties of the field ( pertaining to constraints like pkey, unique etc, and  AutoIncrement etc )
	// 9.	 Add the field to the TableSchema
	// 10. end for

	// Get table path
	QString tablePath = tableNamePathMap[originalName];
	// get dbf pointer for table
	xbDbf* tableDbf = GetDbfPtr( tablePath.toLatin1().constData() );

	xbLong numFlds = tableDbf->FieldCount();

	for( xbShort i = 0; i < numFlds; i = i + 1 ) {
		QString fldName = QString::fromLatin1( tableDbf->GetFieldName( i ) );
		QString fldID( KexiUtils::string2Identifier( fldName ) );

		KexiDB::Field *fld =
		    new KexiDB::Field( fldID, type( tableDbf->GetFieldType( i ) ) );

		//! TODO Open Index files and fill in these constraints
		// getConstraints()

		tableSchema.addField(fld);
	}
	return true;
}


/*! Get a list of tables and put into the supplied string list */
bool xBaseMigrate::drv_tableNames(QStringList& tableNames)
{
	// Get the names from the map directly
	tableNames<<tableNamePathMap.keys();

	kDebug()<<"Tables "<<tableNames;

	return true;
}

/*! Copy xBase table to KexiDB database */
bool xBaseMigrate::drv_copyTable(const QString& srcTable, KexiDB::Connection *destConn,
	KexiDB::TableSchema* dstTable)
{
	// Steps
	// 1. for all records in the table
	// 2.	num_fields = number of fields in the table
	// 3.	for each field in the table
	// 4.	    get the length of the field in the table
	// 5.	    Append the field to the variant list ( vals )
	// 6.	end for
	// 7.	Insert the record into the destinationConnection into the destinationTable
	// 8. end for

	// get dbf pointer for table
	QString tablePath = tableNamePathMap[srcTable];
	xbDbf* tableDbf = GetDbfPtr( tablePath.toLatin1().constData() );

	xbLong numRecords = tableDbf->NoOfRecords();

	const KexiDB::QueryColumnInfo::Vector fieldsExpanded( dstTable->query()->fieldsExpanded() );
	// records are indexed from 1
	for ( xbULong i = 1; i <= numRecords ; i = i + 1 ) {
		tableDbf->GetRecord( i );
		QList<QVariant> vals;

		xbLong numFlds = tableDbf->FieldCount();
		// fields are indexed from 0
		for( xbShort j = 0; j < numFlds; j = j + 1 ) {
			const char* data = tableDbf->GetField(j);
                        QVariant val;
			switch ( type( tableDbf->GetFieldType( j ) ) ) {
				case KexiDB::Field::Date:
					val = QDate::fromString( data, "yyyyMMdd" );
					break;
				case KexiDB::Field::Boolean:
					switch(data[0]) {
						case 'Y': case 'y': case 'T': case 't':
							val = true;
							break;
						case 'N': case 'n': case 'F' : case'f':
							val = false;
							break;
						default:
							val = false;
							break;
					}
					break;
				default:
					val = KexiDB::cstringToVariant(data, fieldsExpanded.at(j)->field, strlen( data ) ) ;
					break;
			}
			vals.append( val );
		}
		kDebug()<<vals;
		if (!destConn->insertRecord(*dstTable, vals)) {
			return false;
		}
	}

	return true;
}

KexiDB::Field::Type KexiMigration::xBaseMigrate::type(char xBaseColumnType)
{
	KexiDB::Field::Type kexiType = KexiDB::Field::InvalidType;

	switch( xBaseColumnType ) {
		case XB_CHAR_FLD:
			kexiType = KexiDB::Field::Text;
			break;
		case XB_LOGICAL_FLD:
			kexiType = KexiDB::Field::Boolean;
			break;
		case XB_NUMERIC_FLD:
			kexiType = KexiDB::Field::Float;
			break;
		case XB_DATE_FLD:
			kexiType = KexiDB::Field::Date;
			break;
		case XB_MEMO_FLD:
			kexiType = KexiDB::Field::BLOB;
			break;
		case XB_FLOAT_FLD:
			kexiType = KexiDB::Field::Double;
			break;
		default:
			kexiType = KexiDB::Field::InvalidType;
			break;
	}

	return kexiType;
}

#include "xbasemigrate.moc"
