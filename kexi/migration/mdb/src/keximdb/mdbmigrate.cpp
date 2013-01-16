/* This file is part of the KDE project
   Copyright (C) 2005,2006 Martin Ellis <martin.ellis@kdemail.net>
   Copyright (C) 2005 Jarosław Staniek <staniek@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#include "mdbmigrate.h"

#include <QString>
#include <QRegExp>
#include <QFile>
#include <QVariant>
#include <QDateTime>
#include <QList>
#include <kdebug.h>

#include <kexiutils/identifier.h>
using namespace KexiMigration;

/* This is the implementation for the MDB file import routines. */
K_EXPORT_KEXIMIGRATE_DRIVER(MDBMigrate, "mdb")

static QByteArray isNonUnicodePropId("source_database_has_nonunicode_encoding");
static QByteArray nonUnicodePropId("source_database_nonunicode_encoding");

/* ************************************************************************** */

MDBMigrate::MDBMigrate(QObject *parent, const QVariantList &args)
        : KexiMigrate(parent, args)
{
    /*! @todo invert the sense of values, then remove "Non-" from these strings */
    setPropertyValue(isNonUnicodePropId, true);
    setPropertyCaption(isNonUnicodePropId, i18n("Source Database Has Non-Unicode Encoding"));
    setPropertyValue(nonUnicodePropId, QString());
    setPropertyCaption(nonUnicodePropId, i18n("Source Database Non-Unicode Encoding"));

    initBackend();
}

MDBMigrate::~MDBMigrate()
{
    releaseBackend();
}

void MDBMigrate::initBackend()
{
    mdb_init();

    // Date format associated with Qt::ISODate: YYYY-MM-DDTHH:MM:SS
    // (where T is a literal).  The following is equivalent to %FT%T, but
    // backards compatible with old/Windows C libraries.
    // See strftime documentation for more info.
    mdb_set_date_fmt("%Y-%m-%dT%H:%M:%S");
}

void MDBMigrate::releaseBackend()
{
    mdb_exit();
}

QVariant MDBMigrate::propertyValue(const QByteArray& propName)
{
    if (propName == isNonUnicodePropId) {
        setPropertyValue(isNonUnicodePropId, false);

        // Costly, but we need this to get this property from file...
        drv_connect();
        drv_disconnect();
    }

    return KexiMigrate::propertyValue(propName);
}

bool MDBMigrate::drv_connect()
{
    kDebug() << "mdb_open:";
    KexiDB::ConnectionData *data = this->data()->source;

    // mdb_open takes a char*, not const char*, hence this nonsense.
    char *filename = qstrdup(QFile::encodeName(data->fileName()));
    m_mdb = mdb_open(filename, MDB_NOFLAGS);
    delete [] filename;

    if (!m_mdb) {
        kDebug() << "mdb_open failed.";
        return false;
    }

    // Setting source encoding
    if (!propertyValue(nonUnicodePropId).toString().isEmpty()) {
        const QByteArray encoding(propertyValue(nonUnicodePropId).toByteArray());

        mdb_set_encoding(m_mdb, encoding.constData());
        kDebug() << "non-unicode encoding set to \""
        << encoding
        << '\"';
    }

    // Supports setting source encoding
    setPropertyValue(isNonUnicodePropId, bool(IS_JET3(m_mdb)));

    return true;
}

bool MDBMigrate::drv_disconnect()
{
    mdb_close(m_mdb);
    return true;
}

MdbTableDef* MDBMigrate::getTableDef(const QString& tableName)
{
    MdbTableDef *tableDef = 0;

    kDebug() << tableName;
    
    // Look through each entry in the catalog...
    for (unsigned int i = 0; i < m_mdb->num_catalog; i++) {
        MdbCatalogEntry* dbObject =
            (MdbCatalogEntry*)(g_ptr_array_index(m_mdb->catalog, i));

        // ... for a table with the given name
        if (dbObject->object_type == MDB_TABLE) {
            QString dbObjectName = QString::fromUtf8(dbObject->object_name);
            if (dbObjectName.toLower() == tableName.toLower()) {
                tableDef = mdb_read_table(dbObject);
                break;
            }
        }
    }

    return tableDef;
}

/* ************************************************************************** */
bool MDBMigrate::drv_readTableSchema(const QString& originalName,
                                     KexiDB::TableSchema& tableSchema)
{
    // Get the column meta-data
    MdbTableDef *tableDef = getTableDef(originalName);
    if (!tableDef) {
        kDebug() << "couldn't find table "
        << originalName;
        return false;
    }
    mdb_read_columns(tableDef);
    kDebug() << "#cols = " << tableDef->num_cols;

    /*! Convert column data to Kexi TableSchema
        Nice mix of terminology here, MDBTools has columns, Kexi has fields. */
    MdbColumn *col;
    for (unsigned int i = 0; i < tableDef->num_cols; i++) {
        col = (MdbColumn*) g_ptr_array_index(tableDef->columns, i);

        // Field name
        QString fldName = QString::fromUtf8(col->name);
        kDebug() << "got column "
        << fldName << '\"' << col->name;

        QString fldID(KexiUtils::string2Identifier(fldName));

        // Field type
        KexiDB::Field *fld =
            new KexiDB::Field(fldID, type(col->col_type));

        kDebug() << "size "
        << col->col_size << " type " << type(col->col_type);
        fld->setCaption(fldName);
        tableSchema.addField(fld);
    }

    getPrimaryKey(&tableSchema, tableDef);

    // Free the column meta-data - as soon as it doesn't seg fault.
    //mdb_free_tabledef(tableDef);

    return true;
}

bool MDBMigrate::drv_tableNames(QStringList& tableNames)
{
    // Try to read the catalog of database objects
    if (!mdb_read_catalog(m_mdb, MDB_ANY)) {
        kDebug() << "couldn't read catalog";
        return false;
    }

    // Add non-system tables to the list
    for (unsigned int i = 0; i < m_mdb->num_catalog; i++) {
        MdbCatalogEntry* dbObject =
            (MdbCatalogEntry*)(g_ptr_array_index(m_mdb->catalog, i));

        if (dbObject->object_type == MDB_TABLE) {
            QString dbObjectName = QString::fromUtf8(dbObject->object_name);

            if (!dbObjectName.startsWith("MSys")) {
                kDebug() << dbObjectName;
                tableNames << dbObjectName;
            }
        }
    }
    return true;
}

QVariant MDBMigrate::toQVariant(const char* data, unsigned int len, int type)
{
    if (len == 0)
        // Null ptr => null value
        return QVariant();

    switch (type) {
    case MDB_TEXT:
    case MDB_MEMO:
        return QVariant(QString::fromUtf8(data, len));
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

bool MDBMigrate::drv_copyTable(const QString& srcTable,
                               KexiDB::Connection *destConn, KexiDB::TableSchema* dstTable)
{
    MdbTableDef *tableDef = getTableDef(srcTable);
    if (!tableDef) {
        kDebug() << srcTable;
        return false;
    }

    char *columnData[256];
    int columnDataLength[256];

    //! Bind + allocate the DB columns to columnData and columnDataLength arrays
    mdb_read_columns(tableDef); // mdb_bind_column dies without this
    for (unsigned int i = 0; i < tableDef->num_cols; i++) {
        MdbColumn *col = (MdbColumn*) g_ptr_array_index(tableDef->columns, i);
        if (col->col_type == MDB_MEMO) {
//! @todo supporting 1GB (possible to insert programmatically) of MEMO field needs changes in mdbtools API;
//!       for now 65,535 is supported (maximum when entering data through the user interface)
            columnData[i] = (char*) g_malloc(0x10000);
        }
        else {
            columnData[i] = (char*) g_malloc(MDB_BIND_SIZE);
        }

        // Columns are numbered from 1
        // and why aren't these unsigned ints?
        mdb_bind_column(tableDef, i + 1, columnData[i], &columnDataLength[i]);
    }

    //! Copy each row into vals
    mdb_rewind_table(tableDef);
    kDebug() << "Fetching " << tableDef->num_rows << " records";

#ifdef KEXI_MIGRATION_MAX_ROWS_TO_IMPORT
    qulonglong rows = 0;
#endif

    bool ok = true;
    while (mdb_fetch_row(tableDef)) {
        QList<QVariant> vals;

//    kDebug() << kdLoc << "Copying " << tableDef->num_cols << " cols";
        for (unsigned int i = 0; i < tableDef->num_cols; i++) {
            MdbColumn *col = (MdbColumn*) g_ptr_array_index(tableDef->columns, i);

            if (col->col_type == MDB_OLE && col->cur_value_len) {
                mdb_ole_read(m_mdb, col, columnData[i], MDB_BIND_SIZE);
            }

            //! @todo: How to import binary data?
            QVariant var
            = toQVariant(columnData[i], columnDataLength[i], col->col_type);
            vals << var;
        }

        if (!destConn->insertRecord(*dstTable, vals)) {
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

    // Deallocate (unbind) the DB columns arrays and column meta-data
    for (unsigned int i = 0; i < tableDef->num_cols; i++) {
        g_free(columnData[i]);
    }

    // When memory leaks are better than seg. faults...
    //mdb_free_tabledef(tableDef);

    return ok;
}

KexiDB::Field::Type MDBMigrate::type(int type)
{
    KexiDB::Field::Type kexiType;

    switch (type) {
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

bool MDBMigrate::getPrimaryKey(KexiDB::TableSchema* table, MdbTableDef* tableDef)
{
    MdbIndex *idx;

    if (!tableDef) {
        return false;
    }
    mdb_read_columns(tableDef);
    mdb_read_indices(tableDef);

    // Find the PK index in the MDB file
    bool foundIdx = false;
    for (unsigned int i = 0; i < tableDef->num_idxs; i++) {
        idx = (MdbIndex*) g_ptr_array_index(tableDef->indices, i);
//  QString fldName = QString::fromUtf8(idx->name);

        if (!strcmp(idx->name, "PrimaryKey")) {
            idx = (MdbIndex*) g_ptr_array_index(tableDef->indices, i);
            foundIdx = true;
            break;
        }
    }

    if (!foundIdx) {
        mdb_free_indices(tableDef->indices);
        return false;
    }

    //! @todo: MDB index order (asc, desc)

    kDebug() << "num_keys " << idx->num_keys;

    //! Create the KexiDB IndexSchema ...
    QVector<int> key_col_num(idx->num_keys);

    // MDBTools counts columns from 1 - subtract 1 where necessary
    KexiDB::IndexSchema* p_idx = new KexiDB::IndexSchema(table);

    for (unsigned int i = 0; i < idx->num_keys; i++) {
        key_col_num[i] = idx->key_col_num[i];
        kDebug() << "key " << i + 1
        << " col " << key_col_num[i]
        << table->field(idx->key_col_num[i] - 1)->name()
;
        p_idx->addField(table->field(idx->key_col_num[i] - 1));
    }

    kDebug() << p_idx->debugString();

    // ... and add it to the table definition
    // but only if the PK has only one field, so far :o(

    KexiDB::Field *f;
    if (idx->num_keys == 1 && (f = table->field(idx->key_col_num[0] - 1))) {
        f->setPrimaryKey(true);
    } else {
        //! @todo: How to add a composite PK to a TableSchema?
        //m_table->setPrimaryKey(p_idx);
    }

    mdb_free_indices(tableDef->indices);
    return true;
}

bool MDBMigrate::drv_getTableSize(const QString& table, qulonglong& size)
{
    // Get the column meta-data, which contains the table size
    MdbTableDef *tableDef = getTableDef(table);
    if (!tableDef) {
        kDebug() << "couldn't find table "
        << table;
        return false;
    }
    size = (qulonglong)(tableDef->num_rows);
    mdb_free_tabledef(tableDef);
    return true;
}


#include "mdbmigrate.moc"
