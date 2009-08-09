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
#include <qlist.h>
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

  foreach( const QString& fileName, dbfFiles ) {
    xbDbf* table = new xbDbf( this );
    // Calling OpenDatabase, will automatically add the pointer `table`
    // to the dbfList of xbXBase class ( if there is no error )
    QString absoluteFileName = xBaseDirectory.filePath( fileName );

    // remove the letters '.dbf'. Hence the -4
    QString choppedFileName = fileName.left( fileName.length() - 4 ).toLower();
    m_tableNamePathMap[choppedFileName] = absoluteFileName;
    kDebug()<<choppedFileName<<" Path:"<<absoluteFileName;

    int returnCode;
    QByteArray ba = absoluteFileName.toUtf8();
    if (  ( returnCode = table->OpenDatabase( ba.constData() ) ) != XB_NO_ERROR ) {
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
          kDebug()<<"Error code "<<returnCode;
          return false;
      }
    }
  }

  kDebug()<<"Successfully processed all the dbf files in the directory";
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
  QString tablePath = m_tableNamePathMap.value( originalName );
  // get dbf pointer for table
  xbDbf* tableDbf = GetDbfPtr( tablePath.toLatin1().constData() );

  xbLong numFlds = tableDbf->FieldCount();

  for( xbShort i = 0; i < numFlds; i++ ) {
    QString fldName = QString::fromLatin1( tableDbf->GetFieldName( i ) );
    QString fldID( KexiUtils::string2Identifier( fldName.toLower() ) );

    KexiDB::Field *fld =
        new KexiDB::Field( fldID, type( tableDbf->GetFieldType( i ) ) );

    if ( fld->type() == KexiDB::Field::Text ) {
      fld->setLength( tableDbf->GetFieldLen(i) );
    }

    if ( fld->isFPNumericType() ) {
      fld->setScale( tableDbf->GetFieldDecimal(i) );
    }

    getConstraints(originalName, fld);

    tableSchema.addField(fld);
  }
  return true;
}


/*! Get a list of tables and put into the supplied string list */
bool xBaseMigrate::drv_tableNames(QStringList& tableNames)
{
  // Get the names from the map directly
  tableNames<<m_tableNamePathMap.keys();

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
  QString tablePath = m_tableNamePathMap.value( srcTable );
  xbDbf* tableDbf = GetDbfPtr( tablePath.toLatin1().constData() );

  xbLong numRecords = tableDbf->NoOfRecords();

  const KexiDB::QueryColumnInfo::Vector fieldsExpanded( dstTable->query()->fieldsExpanded() );
  // records are indexed from 1
  for ( xbULong i = 1; i <= (xbULong)numRecords ; i++ ) {
    tableDbf->GetRecord( i );
    QList<QVariant> vals;

    xbLong numFlds = tableDbf->FieldCount();
    // fields are indexed from 0
    for( xbShort j = 0; j < numFlds; j++ ) {
      char data[1024];
      tableDbf->GetField(j, data);
      QVariant val;

      #ifdef XB_MEMO_FIELDS
        int blobFieldLength;
        char* memoBuffer = 0;
        int returnCode;
      #endif
  
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
        case KexiDB::Field::BLOB:
        #ifdef XB_MEMO_FIELDS
          blobFieldLength = tableDbf->GetMemoFieldLen(j);
          memoBuffer = new char[blobFieldLength];

          #ifdef XB_LOCKING_ON
            tableDbf->LockMemoFile( F_SETLK, F_RDLCK );
          #endif

          if ( ( returnCode = tableDbf->GetMemoField( j , blobFieldLength, memoBuffer, F_SETLKW ) ) != XB_NO_ERROR ) {
            kDebug()<<"Error reading blob field. Error code: "<<returnCode; // make error message more verbose
          } else {
            val = KexiDB::cstringToVariant( memoBuffer, fieldsExpanded.at(j)->field, blobFieldLength );
          }
          #ifdef XB_LOCKING_ON
            tableDbf->LockMemoFile( F_SETLK, F_UNLCK );
          #endif

          break;
        #else
          kDebug()<<"XB_MEMO_FIELDS support disabled during compilation of XBase libraries";
        #endif

        default:
          val = KexiDB::cstringToVariant(data, fieldsExpanded.at(j)->field, strlen( data ) ) ;
          break;
      }
      vals.append( val );
    }
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

void KexiMigration::xBaseMigrate::getConstraints(const QString& tableName, KexiDB::Field* fld)
{
  // 1. Get the names of the index files
  // 2. Create appropriate xbIndex type object ( xbNdx or xbNtx ) depending on extension
  // 3. Open the index file
  // 4. Check the expression of the index to crosscheck whether this is indeed the index file on the required field.
  // 5. Determine the index type ( unique or not )
  // 6. Set appropriate properties to the field
  
  // Create a base class pointer to an xbIndex
  xbIndex* index = 0;
  
  QStringList indexFileNames = getIndexFileNames(tableName, fld->name());
  
  if ( indexFileNames.isEmpty() ) {
    // no index files exist for this field
    return;
  }
  
  foreach( const QString& indexFileName, indexFileNames ) {
  
    // get dbf pointer for table
    QString tablePath = m_tableNamePathMap.value( tableName );
    xbDbf* tableDbf = GetDbfPtr( tablePath.toLatin1().constData() );

    // determine type of indexFile
    // currently done by checking extension.
    //! @TODO Check mimetype instead
    QString fileExtension = indexFileName.right( 3 );
  
    if ( fileExtension.toLower() == "ndx" ) {
      index = new xbNdx( tableDbf );
    } else if ( fileExtension.toLower() == "ntx" ) {
      index = new xbNtx( tableDbf );
    } else {
      // couldn't recognize extension
      kDebug()<<"Couldn't recognize extension";
      return;
    }
  
    if ( index->OpenIndex( indexFileName.toLatin1().constData() ) != XB_NO_ERROR ) {
      kDebug()<<"Couldn't open index file"<<indexFileName;
      return;
    }
  
    // verfiy if this index is on the required field
    char buf[256];
    index->GetExpression( buf, 256 );
    QString expressionName = QString::fromLatin1( buf );
  
    if ( expressionName.toLower() != fld->name() ) {
      kDebug()<<"Expression mismatch in "<<indexFileName;
      continue;
    }
  
    // all is well, set the index
    if ( index->UniqueIndex() == XB_UNIQUE ) {
      fld->setUniqueKey( true );
      kDebug()<<"Unique Index on "<<fld->name();
    } else {  // index->UniqueIndex() == XB_NOT_UNIQUE
      fld->setIndexed( true );
      kDebug()<<"Normal Index on "<<fld->name();
    }

    // ok, moving through the loop is fairly useless as we can only set a single index on a field anyway
    // does any one use multiple indexes on the same field ?
    // well anyway, when Kexi supports it, we'll use IndexSchemas till then ...
  }
}

QStringList KexiMigration::xBaseMigrate::getIndexFileNames(const QString& tableName, const QString& fieldName)
{
  // this function needs to return a lits of index files corresponding to the given tablename and field.
  // The current policy uses the xbsql ( http://www.quaking.demon.co.uk/xbsql/ ) semantics for determining
  // the filenames of the index files
  // index files are assumed to be of the type <tablename>_<fieldname>.ndx or .ntx
  
  // Though the current semantics allows only one index on a field ( actually two, considering we can
  // have both .ndx and .ntx index, there can be multiple indices, hence a list of filenames is returned
  // (Note: Kexi fields support only a single index. But we have a separate IndexSchema class ...)
  
  QString dbPath = data()->source->dbPath();
  QDir xBaseDirectory( dbPath );
  
  QString fileName = tableName + '_' + fieldName;
  
  QStringList indexFilters;
  indexFilters<<fileName+'*'; // filter all files of the form <tableName>_<fieldName>
  
  xBaseDirectory.setNameFilters( indexFilters );
  QStringList fileNameList = xBaseDirectory.entryList();
  
  QStringList absolutePathNames;
  foreach( const QString& fileName, fileNameList ) {
    absolutePathNames<<xBaseDirectory.filePath( fileName );
  }
  
  return absolutePathNames;
}

#include "xbasemigrate.moc"
