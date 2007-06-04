/* This file is part of the KDE project
   Copyright (C) 2005,2006 Martin Ellis <martin.ellis@kdemail.net>
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#include "mdbmigrate.h"

#include <qstring.h>
#include <qcstring.h>
#include <qregexp.h>
#include <qfile.h>
#include <qvariant.h>
#include <qdatetime.h>
#include <qvaluelist.h>
#include <kdebug.h>

#include <kexiutils/identifier.h>
using namespace KexiMigration;

/* This is the implementation for the MDB file import routines. */
KEXIMIGRATE_DRIVER_INFO( MDBMigrate, mdb );

static QCString isNonUnicodePropId( "source_database_has_nonunicode_encoding" );
static QCString nonUnicodePropId( "source_database_nonunicode_encoding" );

/* ************************************************************************** */

MDBMigrate::MDBMigrate(QObject *parent, const char *name,
                       const QStringList &args) :
    KexiMigrate(parent, name, args)
{

  /*! @todo invert the sense of values, then remove "Non-" from these strings */
  m_properties[ isNonUnicodePropId ] = QVariant( true, 1 );
  m_propertyCaptions[ isNonUnicodePropId ] =
      i18n("Source Database Has Non-Unicode Encoding");
  m_properties[ nonUnicodePropId ] = QVariant("");
  m_propertyCaptions[ nonUnicodePropId ]
	  = i18n("Source Database Non-Unicode Encoding");

  initBackend();
}

/* ************************************************************************** */
//! Destructor
MDBMigrate::~MDBMigrate() {
  releaseBackend();
}

/* ************************************************************************** */
void MDBMigrate::initBackend() {
  mdb_init();

  // Date format associated with Qt::ISODate: YYYY-MM-DDTHH:MM:SS
  // (where T is a literal).  The following is equivalent to %FT%T, but
  // backards compatible with old/Windows C libraries.
  // See strftime documentation for more info.
  mdb_set_date_fmt("%Y-%m-%dT%H:%M%:%S");
}

void MDBMigrate::releaseBackend() {
  mdb_exit();
}

/* ************************************************************************** */
/*! Properties */
QVariant MDBMigrate::propertyValue( const QCString& propName )
{
  if ( propName == isNonUnicodePropId ) {
    m_properties[ isNonUnicodePropId ] = QVariant(false, 0);

    // Costly, but we need this to get this property from file...
    drv_connect();
    drv_disconnect();
  }

  return KexiMigrate::propertyValue( propName );
}

/* ************************************************************************** */
/*! Connect to the db backend */
bool MDBMigrate::drv_connect() {
  kdDebug() << "mdb_open:" << endl;
  KexiDB::ConnectionData *data = m_migrateData->source;

  // mdb_open takes a char*, not const char*, hence this nonsense.
  char *filename = qstrdup(QFile::encodeName(data->fileName()));
  m_mdb = mdb_open (filename, MDB_NOFLAGS);
  delete [] filename;

  if (!m_mdb) {
    kdDebug() << "mdb_open failed." << endl;
    return false;
  }

  // Setting source encoding
  if ( !m_properties[ nonUnicodePropId ].toCString().isEmpty() ) {
    QCString encoding = m_properties[ nonUnicodePropId ].toCString();

    mdb_set_encoding( m_mdb, (const char*) encoding );
    kdDebug() << "non-unicode encoding set to \""
              << encoding
              << "\"" << endl;
  }

  // Supports setting source encoding
  m_properties[ isNonUnicodePropId ] = QVariant( IS_JET3(m_mdb), 1 );

  return true;
}


/*! Disconnect from the db backend */
bool MDBMigrate::drv_disconnect()
{
  mdb_close( m_mdb );
  return true;
}

//! Get the table definition for a given table name
/*! Look up the table definition for the given table.  This only returns a ptr
    to the MdbTableDef - it doesn't load e.g. the column data.
    Remember to mdb_free_tabledef the table definition when it's finished
    with.
    \return the table definition, or null if no matching table was found
*/
MdbTableDef* MDBMigrate::getTableDef(const QString& tableName)
{
  MdbTableDef *tableDef = 0;

  // Look through each entry in the catalogue ...
  for (unsigned int i = 0; i < m_mdb->num_catalog; i++) {
    MdbCatalogEntry* dbObject =
        (MdbCatalogEntry*)(g_ptr_array_index (m_mdb->catalog, i));

    // ... for a table with the given name
    if (dbObject->object_type == MDB_TABLE) {
      QString dbObjectName = QString::fromUtf8(dbObject->object_name);
      if (dbObjectName.lower() == tableName.lower()) {
        tableDef = mdb_read_table(dbObject);
        break;
      }
    }
  }

  return tableDef;
}

/* ************************************************************************** */
/*! Get the types and properties for each column. */
bool MDBMigrate::drv_readTableSchema( const QString& originalName,
                                      KexiDB::TableSchema& tableSchema )
{
  //! Get the column meta-data
  MdbTableDef *tableDef = getTableDef(originalName);
  if(!tableDef) {
    kdDebug() << "MDBMigrate::drv_getTableDef: couldn't find table "
              << originalName << endl;
    return false;
  }
  mdb_read_columns(tableDef);
  kdDebug() << "MDBMigrate::drv_readTableSchema: #cols = "
            << tableDef->num_cols << endl;

  /*! Convert column data to Kexi TableSchema
      Nice mix of terminology here, MDBTools has columns, Kexi has fields. */
  MdbColumn *col;
  for (unsigned int i = 0; i < tableDef->num_cols; i++) {
    col = (MdbColumn*) g_ptr_array_index(tableDef->columns, i);

    // Field name
    QString fldName = QString::fromUtf8(col->name);
    kdDebug() << "MDBMigrate::drv_readTableSchema: got column "
        << fldName << "\"" << col->name << endl;

    QString fldID( KexiUtils::string2Identifier(fldName) );

    // Field type
    KexiDB::Field *fld =
        new KexiDB::Field(fldID, type(col->col_type));

    kdDebug() << "MDBMigrate::drv_readTableSchema: size "
        << col->col_size << " type " << type(col->col_type) <<endl;
    fld->setCaption(fldName);
    tableSchema.addField(fld);
  }

  getPrimaryKey(&tableSchema, tableDef);

  //! Free the column meta-data - as soon as it doesn't seg fault.
  //mdb_free_tabledef(tableDef);

  return true;
}


/*! Get a list of tables and put into the supplied string list */
bool MDBMigrate::drv_tableNames(QStringList& tableNames)
{
  // Try to read the catalogue of database objects
  if (!mdb_read_catalog (m_mdb, MDB_ANY)) {
    kdDebug() << "MDBMigrate::drv_tableNames: couldn't read catalogue" << endl;
    return false;
  }

  // Add non-system tables to the list
  for (unsigned int i = 0; i < m_mdb->num_catalog; i++) {
    MdbCatalogEntry* dbObject =
        (MdbCatalogEntry*)(g_ptr_array_index (m_mdb->catalog, i));

    if (dbObject->object_type == MDB_TABLE) {
      QString dbObjectName = QString::fromUtf8(dbObject->object_name);

      if (!dbObjectName.startsWith("MSys")) {
        kdDebug() << "MDBMigrate::drv_tableNames: " << dbObjectName << endl;
        tableNames << dbObjectName;
      }
    }
  }
  return true;
}

QVariant MDBMigrate::toQVariant(const char* data, unsigned int len, int type) {
  if(len == 0) {
    // Null ptr => null value
    return QVariant();
  } else {
    switch (type) {
    case MDB_TEXT:
    case MDB_MEMO:
      return QVariant( QString::fromUtf8(data, len) );
    case MDB_BOOL:    //! @todo use &bool!
    case MDB_BYTE:
      return QString::fromUtf8(data, len).toShort();
    case MDB_SDATETIME:
      return QDateTime::fromString(data, Qt::ISODate);
    case MDB_INT:
    case MDB_LONGINT:
      return QString::fromUtf8(data, len).toLongLong();
    case MDB_FLOAT:
      return QString::fromUtf8(data, len).toFloat();
    case MDB_DOUBLE:
    case MDB_MONEY:   //! @todo
    case MDB_NUMERIC: //! @todo
      return QString::fromUtf8(data, len).toDouble();
    case MDB_OLE:
    case MDB_REPID:
    default:
      return QVariant(QString::fromUtf8(data, len));
    }
  }
}

/*! Copy MDB table to KexiDB database */
bool MDBMigrate::drv_copyTable( const QString& srcTable,
                                KexiDB::Connection *destConn,
                                KexiDB::TableSchema* dstTable)
{
  QString kdLoc = "MDBMigrate::drv_copyTable: ";
  MdbTableDef *tableDef = getTableDef(srcTable);
  if(!tableDef) {
    kdDebug() << kdLoc << srcTable << endl;
    return false;
  }

  char *columnData[256];
  int columnDataLength[256];

  //! Bind + allocate the DB columns to columnData and columnDataLength arrays
  mdb_read_columns(tableDef); // mdb_bind_column dies without this
  for (unsigned int i = 0; i < tableDef->num_cols; i++) {
    columnData[i] = (char*) g_malloc(MDB_BIND_SIZE);

    // Columns are numbered from 1
    // and why aren't these unsigned ints?
    mdb_bind_column(tableDef, i + 1, columnData[i], &columnDataLength[i]);
  }

  //! Copy each row into vals
  mdb_rewind_table(tableDef);
  kdDebug() << kdLoc << "Fetching " << tableDef->num_rows << " rows" << endl;

#ifdef KEXI_MIGRATION_MAX_ROWS_TO_IMPORT
  Q_ULLONG rows=0;
#endif

  bool ok = true;
  while(mdb_fetch_row(tableDef)) {
    QValueList<QVariant> vals = QValueList<QVariant>();

//    kdDebug() << kdLoc << "Copying " << tableDef->num_cols << " cols" << endl;
    for (unsigned int i = 0; i < tableDef->num_cols; i++) {
      MdbColumn *col = (MdbColumn*) g_ptr_array_index(tableDef->columns, i);

      if (col->col_type == MDB_OLE && col->cur_value_len) {
        mdb_ole_read(m_mdb, col, columnData[i], MDB_BIND_SIZE);
      }

      //! @todo: How to import binary data?
      QVariant var = toQVariant(columnData[i], columnDataLength[i],
                              col->col_type);
      vals << var;
    }

    if ( !destConn->insertRecord( *dstTable, vals ) ) {
      ok = false;
      break;
    }
    updateProgress();

#ifdef KEXI_MIGRATION_MAX_ROWS_TO_IMPORT
//! @todo this is risky when there are references between tables
    if (++rows == KEXI_MIGRATION_MAX_ROWS_TO_IMPORT)
      break;
#endif
  }

  //! Deallocate (unbind) the DB columns arrays and column meta-data
  for (unsigned int i = 0; i < tableDef->num_cols; i++) {
    g_free(columnData[i]);
  }

  // When memory leaks are better than seg. faults...
  //mdb_free_tabledef(tableDef);

  return ok;
}


//! Convert an MDB type to a KexiDB type, prompting user if necessary.
KexiDB::Field::Type MDBMigrate::type(int type)
{
  // Field type
  KexiDB::Field::Type kexiType = KexiDB::Field::InvalidType;

  switch(type)
  {
    case MDB_BOOL:
      kexiType = KexiDB::Field::Boolean;
      break;
    case MDB_BYTE:
      kexiType = KexiDB::Field::Byte;
      break;
    case MDB_INT:
      kexiType = KexiDB::Field::Integer;
      break;
    case MDB_LONGINT:
      kexiType = KexiDB::Field::BigInteger;
      break;
    case MDB_MONEY:
//! @todo temporary simplification
      kexiType = KexiDB::Field::Double;
      break;
    case MDB_FLOAT:
      kexiType = KexiDB::Field::Float;
      break;
    case MDB_DOUBLE:
      kexiType = KexiDB::Field::Double;
      break;
    case MDB_SDATETIME:
      kexiType = KexiDB::Field::DateTime;
      break;
    case MDB_TEXT:
      kexiType = KexiDB::Field::LongText;
      break;
    case MDB_OLE:
      kexiType = KexiDB::Field::BLOB;
      break;
    case MDB_MEMO:
      kexiType = KexiDB::Field::LongText;
      break;
//! @todo temporary simplification
    case MDB_NUMERIC:
      kexiType = KexiDB::Field::Double;
      break;
    case MDB_REPID:
      // ?
    default:
      kexiType = KexiDB::Field::InvalidType;
  }

  // If we don't know what it is, hope it's text. :o)
  if (kexiType == KexiDB::Field::InvalidType) {
    return KexiDB::Field::LongText;
  }
  return kexiType;
}


bool MDBMigrate::getPrimaryKey( KexiDB::TableSchema* table,
                                MdbTableDef* tableDef ) {
  QString kdLoc = "MDBMigrate::getPrimaryKey: ";
  MdbIndex *idx;

  if (!tableDef) {
    return false;
  }
  mdb_read_columns(tableDef);
  mdb_read_indices(tableDef);

  //! Find the PK index in the MDB file
  bool foundIdx = false;
  for (unsigned int i = 0; i < tableDef->num_idxs; i++) {
    idx = (MdbIndex*) g_ptr_array_index (tableDef->indices, i);
    QString fldName = QString::fromUtf8(idx->name);

    if (!strcmp(idx->name, "PrimaryKey")) {
      idx = (MdbIndex*) g_ptr_array_index (tableDef->indices, i);
      foundIdx = true;
      break;
    }
  }

  if(!foundIdx) {
    mdb_free_indices(tableDef->indices);
    return false;
  }

  //! @todo: MDB index order (asc, desc)

  kdDebug() << kdLoc << "num_keys " << idx->num_keys << endl;

  //! Create the KexiDB IndexSchema ...
  QByteArray key_col_num(idx->num_keys);

  // MDBTools counts columns from 1 - subtract 1 where necessary
  KexiDB::IndexSchema* p_idx = new KexiDB::IndexSchema(table);

  for (unsigned int i = 0; i < idx->num_keys; i++) {
    key_col_num[i] = idx->key_col_num[i];
    kdDebug() << kdLoc << "key " << i + 1
              << " col " << key_col_num[i]
              << table->field(idx->key_col_num[i] - 1)->name()
              << endl;
    p_idx->addField(table->field(idx->key_col_num[i] - 1));
  }

  kdDebug() << kdLoc << p_idx->debugString() << endl;

  //! ... and add it to the table definition
  // but only if the PK has only one field, so far :o(

  KexiDB::Field *f;
  if(idx->num_keys == 1 && (f = table->field(idx->key_col_num[0] - 1))) {
    f->setPrimaryKey(true);
  } else {
    //! @todo: How to add a composite PK to a TableSchema?
    //m_table->setPrimaryKey(p_idx);
  }

  mdb_free_indices(tableDef->indices);
  return true;
}

bool MDBMigrate::drv_getTableSize(const QString& table, Q_ULLONG& size) {
  //! Get the column meta-data, which contains the table size
  MdbTableDef *tableDef = getTableDef(table);
  if(!tableDef) {
    kdDebug() << "MDBMigrate::drv_getTableDef: couldn't find table "
              << table << endl;
    return false;
  }
  size = (Q_ULLONG)(tableDef->num_rows);
  mdb_free_tabledef(tableDef);
  return true;
}


#include "mdbmigrate.moc"
