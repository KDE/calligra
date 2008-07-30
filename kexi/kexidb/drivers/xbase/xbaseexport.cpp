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

#include "xbaseexport.h"

#include <QHash>
#include <QDir>

#include <kdebug.h>

#include <kexidb/field.h>
#include <kexidb/RecordData.h>
#include <kexidb/cursor.h>
#include <kexidb/drivermanager.h>
#include <core/kexi.h>
#include <migration/keximigratedata.h>

#include <cstring>

#include "xbase.h"

using namespace KexiDB;

class KexiDB::xBaseExportPrivate {
  public:
    xBaseExportPrivate() {
    }

    //! Converts kexidb field types to xbase types
    char type(KexiDB::Field::Type fieldType);

    //! Appends record to xbase table
    bool appendRecord(const QString& sourceTableName , KexiDB::RecordData* recordData);

    //! Returns max fieldlengths for xBase table
    int fieldLength(KexiDB::Field* f );

    //! converts QVariant data to a format understood by xBase
    QByteArray fieldData(QVariant data, char type);

    //! Creates xBase indexes for the table
    bool createIndexes(const QString& sourceTableName, KexiDB::TableSchema* tableSchema);

    xbXBase xbase;
    QHash<QString, QString> tableNamePathMap;
};

char xBaseExportPrivate::type(KexiDB::Field::Type fieldType)
{
  char xBaseType = '\0';

  switch( fieldType ) {
    case KexiDB::Field::Text:
    case KexiDB::Field::LongText:
      xBaseType = XB_CHAR_FLD;
      break;

    case KexiDB::Field::Boolean:
      xBaseType = XB_LOGICAL_FLD;
      break;

    case KexiDB::Field::Float:
    case KexiDB::Field::Double:
      xBaseType = XB_FLOAT_FLD;

    case KexiDB::Field::ShortInteger:
    case KexiDB::Field::Integer:
    case KexiDB::Field::BigInteger:
      xBaseType = XB_NUMERIC_FLD;
      break;

    case KexiDB::Field::DateTime:
    case KexiDB::Field::Date:
    case KexiDB::Field::Time:
      xBaseType = XB_DATE_FLD;
      break;

    case KexiDB::Field::BLOB:
      xBaseType = XB_MEMO_FLD;
      break;

    default:
      xBaseType = '\0';
  }

  return xBaseType;
}

bool xBaseExportPrivate::appendRecord( const QString& sourceTableName , KexiDB::RecordData* recordData ) {

// 	kDebug()<<recordData->debugString();
  QString pathName = tableNamePathMap.value( sourceTableName );
  QByteArray pathNameBa = pathName.toAscii();
  xbDbf* table = xbase.GetDbfPtr( pathNameBa.constData() );

  int returnCode;
  table->BlankRecord();
  for (int i=0;i < recordData->size();++i) {
  char fieldType = table->GetFieldType(i);
  QByteArray stringData = fieldData(recordData->value(i), fieldType);

  if (fieldType == XB_MEMO_FLD) {
  #ifdef XB_MEMO_FIELDS
    // we use size()+1 as size to accomodate `\0`
    table->UpdateMemoData(i, stringData.size()+1, stringData.constData(), F_SETLKW );
  #else
    kDebug()<<"XB_MEMO_FIELDS support disabled during compilation of XBase libraries";
  #endif
  } else {
    if ((returnCode = table->PutField( i, stringData.constData())) != XB_NO_ERROR ) {
      switch(returnCode) {
      case XB_INVALID_FIELDNO:
        kDebug()<<"Invalid field number "<<i;
        return false;
      case XB_INVALID_DATA:
        kDebug()<<"Invalid data "<<stringData;
        return false;
      default:
        kDebug()<<"Error number "<<returnCode<<" has occured";
        return false;
    }
    }
  }
  }

  if((returnCode = table->AppendRecord()) != XB_NO_ERROR) {
    kDebug() << "\nxBase Error " << returnCode << " appending data record.";
    return false;
  }

// 	// for debugging purposes only
// 	for ( uint i=0; i< (uint)recordData->size(); ++i ) {
// 		kDebug()<<table->GetField(i);
// 	}

  return true;
}

int xBaseExportPrivate::fieldLength(KexiDB::Field* f ) {
  if ( f->type() == KexiDB::Field::Text ) {
    return f->length();
  }
  // return the max possible (string)length of the types
  // see http://linux.techass.com/projects/xdb/xbasedocs/xbase_c3.html
  switch(type( f->type())) {
    case XB_CHAR_FLD:
      return 254;
    case XB_LOGICAL_FLD:
      return 1;
    case XB_FLOAT_FLD:
    case XB_NUMERIC_FLD:
      return 17;
    case XB_DATE_FLD:
      return 8;
    case XB_MEMO_FLD:
      return 10;
    default:
      return 0;
  }
}

QByteArray xBaseExportPrivate::fieldData(QVariant data, char type) {

  switch(type) {
    case XB_CHAR_FLD:
    case XB_FLOAT_FLD:
    case XB_NUMERIC_FLD:
      return data.toString().toUtf8();

    case XB_LOGICAL_FLD:
      if (data.toBool()) {
        return QString( "t" ).toAscii();
      } else
        return QString( "f" ).toAscii();

    case XB_DATE_FLD:
      return data.toDate().toString("yyyyMMdd").toAscii();

    case XB_MEMO_FLD:
      return data.toByteArray();
    default:
      return QByteArray();
  }
}

bool xBaseExportPrivate::createIndexes(const QString& sourceTableName, KexiDB::TableSchema* tableSchema) {

  QString pathName = tableNamePathMap.value( sourceTableName );
  QByteArray pathNameBa = pathName.toAscii();
  xbDbf* table = xbase.GetDbfPtr( pathNameBa.constData() );
  uint fieldCount = tableSchema->fieldCount();

  QString dirName = QFileInfo( pathName ).path();

  for (uint i=0; i< (uint)fieldCount ; ++i) {
    KexiDB::Field* f = tableSchema->field(i);

    int returnCode;
    QString fieldName = f->name();
    QString indexName = dirName + QDir::separator() + sourceTableName + '_' + fieldName + ".ndx";
    QByteArray indexNameBa = indexName.toAscii();
    QByteArray fieldNameBa = fieldName.toLatin1();

    xbNdx index(table);
    if (f->isUniqueKey() || f->isPrimaryKey()) {

      if ((returnCode = index.CreateIndex(indexNameBa.constData(), fieldNameBa.constData(), XB_UNIQUE, XB_OVERLAY)) != XB_NO_ERROR ) {
        kDebug()<<"Couldn't create unique index for fieldName "<<fieldName<<" on table "<<sourceTableName<<" Error Code "<<returnCode;
        return false;
      }
      index.CloseIndex();

    } else if ( f->isIndexed() ) {

      if ((returnCode = index.CreateIndex(indexNameBa.constData(), fieldNameBa.constData(), XB_NOT_UNIQUE, XB_OVERLAY)) != XB_NO_ERROR ) {
        kDebug()<<"Couldn't create index for fieldName "<<fieldName<<" on table "<<sourceTableName<<" Error Code "<<returnCode;
        return false;
      }
      index.CloseIndex();

    }
  }
  return true;
}


xBaseExport::xBaseExport()
: m_migrateData( 0 ),
d(new xBaseExportPrivate)
{
}

void xBaseExport::setData(KexiMigration::Data* migrateData) {
  m_migrateData = migrateData;
}

bool xBaseExport::performExport(Kexi::ObjectStatus* result) {

  if (result)
    result->clearStatus();


  KexiDB::DriverManager drvManager;

  if (!m_migrateData) {
    kDebug()<<"Migration Data not set yet !!";
    result->setStatus(&drvManager, i18n("Data not set for migration"));
    return false;
  }

  KexiDB::Driver *sourceDriver = drvManager.driver(
    m_migrateData->source->driverName);
  if (!sourceDriver) {
    result->setStatus(&drvManager,
      i18n("Could not export back to destination database"));
    return false;
  }

  // connect to destination database
  if (!dest_connect()) {
    kDebug()<<"Couldn't connect to destination database";
    if (result)
      result->setStatus(i18n("Could not connect to data source \"%1\".",
        m_migrateData->destination->connectionData()->serverInfoString()), "");
    return false;
  }

  KexiDB::Connection* sourceConn = sourceDriver->createConnection(*(m_migrateData->source));

  if (!sourceConn || sourceDriver->error()) {
    kDebug()<<"Export failed";
    return false;
  }
  if (!sourceConn->connect()) {
    kDebug()<<"Export failed.Could not connect";
    return false;
  }

  if (!sourceConn->useDatabase(m_migrateData->sourceName)) {
    kDebug()<<"Couldn't use database "<<m_migrateData->sourceName;
    return false;
  }

  QStringList tables = sourceConn->tableNames();

  // Check if there are any tables
  if (tables.isEmpty()) {
    kDebug() << "There were no tables to export" << endl;
    if (result)
      result->setStatus(
        i18n("No tables to export found in data source \"%1\".",
          m_migrateData->source->serverInfoString()), "");
    return false;
  }

  tables.sort();

  // -- read table schemas and create them in memory (only for non-KexiDB-compat tables)
  foreach (const QString& tableCaption, tables) {
    if (dest_isSystemObjectName( tableCaption )) {
      return false;
    }

    KexiDB::TableSchema *tableSchema = sourceConn->tableSchema( tableCaption );

    if (!dest_createTable(tableCaption, tableSchema)) {
      if (result)
        result->setStatus(i18n("Could not create table in destination \"%1\". Error reading table \"%2\".",	m_migrateData->destination->connectionData()->serverInfoString(), tableCaption), "");
      return false;
    }

    if (m_migrateData->keepData) {
      if (!dest_copyTable(tableCaption, sourceConn, tableSchema)) {
        kDebug() << "Failed to copy table " << tableCaption << endl;
        if (result)
          result->setStatus(sourceConn,
              i18n("Could not copy table \"%1\" to destination database.", tableCaption));
      }
    }

  }

  if (dest_disconnect()) {
    bool ok = false;
    if (sourceConn)
      ok = sourceConn->disconnect();
    return ok;
  }

  // Finally: error handling
  if (result && result->error())
    result->setStatus(sourceConn,
      i18n("Could not export data to \"%1\".",
        m_migrateData->source->serverInfoString()));
  dest_disconnect();
  if (sourceConn) {
    sourceConn->disconnect();
  }
  return false;
}

bool xBaseExport::dest_connect() {
  return true;
}

bool xBaseExport::dest_disconnect() {
  QList<QString> pathNameList = d->tableNamePathMap.values();
  foreach(const QString& pathName, pathNameList) {
    QByteArray ba = pathName.toAscii();
    xbDbf* tablePtr = d->xbase.GetDbfPtr(ba.constData());
    tablePtr->CloseDatabase();
    // delete tablePtr ?
  }
  return true;
}

bool xBaseExport::dest_createTable(const QString& originalName, KexiDB::TableSchema* tableSchema) {
  // Algorithm
  // 1. For each fields in the table schema.
  // 2.   Create a xbSchema entry and add it to xbSchema array.
  // 3. End for
  // 4. Create table in overlay mode ( overwrite )

  uint fieldCount = tableSchema->fieldCount();
  const int arrayLength = fieldCount + 1; // and extra space for the `null`
  xbSchema xBaseTableSchema[arrayLength];// = new xbSchema[fieldCount+1][4];

  uint i = 0;
  for (i = 0; i < fieldCount ; ++i) {
    KexiDB::Field* f = tableSchema->field(i);

    QByteArray ba = f->name().toLatin1();
    //! TODO Fieldname can only be 11 characters
    strcpy(xBaseTableSchema[i].FieldName, ba.data());
    xBaseTableSchema[i].Type = d->type(f->type());
    xBaseTableSchema[i].FieldLen = d->fieldLength( f ); //! TODO Check semantics
    xBaseTableSchema[i].NoOfDecs = ( xBaseTableSchema[i].Type != XB_CHAR_FLD )? f->scale() : 0 ;

  }

  // last member should be all 0
  strcpy( xBaseTableSchema[i].FieldName , "" );
  xBaseTableSchema[i].Type = 0;
  xBaseTableSchema[i].FieldLen = 0;
  xBaseTableSchema[i].NoOfDecs = 0;

  const KexiDB::ConnectionData* connData = m_migrateData->destination->connectionData();
  QString dirName = connData->fileName(); // this includes the forward slash after the dir name

  QString pathName = dirName + originalName + ".dbf";
  d->tableNamePathMap[originalName] = pathName;

  QByteArray pathNameBa = pathName.toAscii();

  xbDbf* xBaseTable = new xbDbf( &d->xbase );
  xBaseTable->SetVersion( 4 ); // create dbase IV style files
  xbShort returnCode;
  if (( returnCode = xBaseTable->CreateDatabase( pathNameBa.constData() , xBaseTableSchema, XB_OVERLAY ))  != XB_NO_ERROR ) {
    kDebug()<<"Error creating table "<<originalName<<" Error Code "<<returnCode;
    return false;
  }

  if (!d->createIndexes(originalName, tableSchema)) {
    return false;
  }

  return true;
}

bool xBaseExport::dest_copyTable(const QString& srcTableName, KexiDB::Connection *srcConn,
        KexiDB::TableSchema* /*srcTable*/) {
  // Algorithm
  // 1. pick each row
  // 2. Insert it into the xBase table

  // using the tableSchema as argument automatically appends rowid
  // info to the recordData which we don't want. Hence we use SQL query
  KexiDB::Cursor* cursor = srcConn->executeQuery(QString( "Select * from %1" ).arg(srcTableName));

  if (!cursor)
    return false;

  if (!cursor->moveFirst() && cursor->error())
    return false;

  while (!cursor->eof()) {
    KexiDB::RecordData *record = cursor->storeCurrentRow();
    if (!record) {
      return false;
    }
    if (!d->appendRecord(srcTableName, record)) {
      kDebug()<<"Couldn't append record";
      return false;
    }

    if (!cursor->moveNext() && cursor->error()) {
      return false;
    }
  }
  return true;
}

bool xBaseExport::dest_isSystemObjectName( const QString& /* objectName */ ) {
  return false;
}
