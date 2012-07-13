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

#include "sybasemigrate.h"

#include <QString>
#include <QRegExp>
#include <QFile>
#include <QVariant>
#include <QList>
#include <kdebug.h>

#include <migration/keximigratedata.h>
#include <db/cursor.h>
#include <db/field.h>
#include <db/utils.h>
#include <db/drivermanager.h>
#include <kexidb/drivers/sybase/sybaseconnection_p.cpp>
#include <kexiutils/identifier.h>

using namespace KexiMigration;

/* This is the implementation for the Sybase specific import routines. */

K_EXPORT_KEXIMIGRATE_DRIVER(SybaseMigrate, "sybase")

/* ************************************************************************** */
//! Constructor
/*SybaseMigrate::SybaseMigrate() :
  d(new MySqlConnectionInternal())
{
}*/

//! Constructor (needed for trading interface)
SybaseMigrate::SybaseMigrate(QObject *parent, const QVariantList&args) :
        KexiMigrate(parent, args)
        , d(new SybaseConnectionInternal(0))
        //,m_mysqlres(0)
{
    KexiDB::DriverManager manager;
    m_kexiDBDriver = manager.driver("sybase");
}

/* ************************************************************************** */
//! Destructor
SybaseMigrate::~SybaseMigrate()
{
}


/* ************************************************************************** */
/*! Connect to the db backend */
bool SybaseMigrate::drv_connect()
{
    if (!d->db_connect(*m_migrateData->source))
        return false;
    return d->useDatabase(m_migrateData->sourceName);
}


/*! Disconnect from the db backend */
bool SybaseMigrate::drv_disconnect()
{
    return d->db_disconnect();
}


/* ************************************************************************** */
/*! Get the types and properties for each column. */
bool SybaseMigrate::drv_readTableSchema(
    const QString& originalName, KexiDB::TableSchema& tableSchema)
{
// m_table = new KexiDB::TableSchema(table);

// //TODO IDEA: ask for user input for captions
// tableSchema.setCaption(table + " table");

    //Perform a query on the table to get some data
    QString sqlStatement = QString("SELECT TOP 0 * FROM ") + drv_escapeIdentifier(originalName);

    if (!query(sqlStatement))
        return false;

    unsigned int numFlds = dbnumcols(d->dbProcess);

    QVector<KexiDB::Field*> fieldVector;

    for (unsigned int i = 1; i <= numFlds; i++) {
        //  dblib indexes start from 1
        DBCOL* colInfo = new DBCOL;
        if (dbcolinfo(d->dbProcess, CI_REGULAR, i , 0, colInfo) != SUCCEED) {
            return false;
        }

        QString fldName(dbcolname(d->dbProcess, i));
        QString fldID(KexiUtils::string2Identifier(fldName));

        KexiDB::Field *fld =
            new KexiDB::Field(fldID, type(originalName, dbcoltype(d->dbProcess, i)));
        fld->setCaption(fldName);
        fld->setAutoIncrement(colInfo->Identity == true ? true : false);
        fld->setNotNull(colInfo->Null == false ? true : false);

        // collect the fields for post-processing
        fieldVector.append(fld);

        tableSchema.addField(fld);

        kDebug() << fld->caption() << "No.of fields in tableSchema" << tableSchema.fieldCount();

        delete colInfo;
    }

//    foreach( KexiDB::Field* fld, fieldVector ) {
//     fld->setPrimaryKey( primaryKey( originalName, fld->caption() ) );
//     fld->setUniqueKey( uniqueKey( originalName, fld->caption() ) );
//    }

    // read all the indexes on this table
    QList<KexiDB::IndexSchema*> indexList = readIndexes(originalName, tableSchema);
    foreach(KexiDB::IndexSchema* index, indexList) {
        // We are only looking for indexes on single fields
        // **kexi limitation**
        // generalise this when we get full support for indexes
        if (index->fieldCount() != 1) {
            continue;
        }
        KexiDB::Field* fld = index->field(0);

        if (!fld)
            return false;

        if (index->isPrimaryKey()) {
            fld->setPrimaryKey(true);
            tableSchema.setPrimaryKey(index);
        } else if (index->isUnique()) {
            fld->setUniqueKey(true);
        } else {
            fld->setIndexed(true);
        }
    }

    return true;
}


/*! Get a list of tables and put into the supplied string list */
bool SybaseMigrate::drv_tableNames(QStringList& tableNames)
{
    if (!query("Select name from sysobjects where type='U'"))
        return false;
    while (dbnextrow(d->dbProcess) != NO_MORE_ROWS) {
        kDebug() << value(0);
        tableNames << value(0);
    }
    return true;
}

/*! Fetches single string at column \a columnNumber for each record from result obtained
 by running \a sqlStatement.
 On success the result is stored in \a stringList and true is returned.
 \return cancelled if there are no records available. */
tristate SybaseMigrate::drv_queryStringListFromSQL(
    const QString& sqlStatement, uint columnNumber, QStringList& stringList, int numRecords)
{
    if (!query(sqlStatement))
        return false;

    int counter = 0;

    while (dbnextrow(d->dbProcess) != NO_MORE_ROWS && (numRecords == -1 || counter < numRecords)) {

    }

    for (int i = 0; numRecords == -1 || i < numRecords; i++) {
        RETCODE returnCode = dbnextrow(d->dbProcess);
        if (returnCode != SUCCEED) {
            tristate r;
            if (returnCode == FAIL)
                r = false;
            else if (returnCode == NO_MORE_RESULTS)
                r = (numRecords == -1) ? true : cancelled;
            return r;
        }

        uint numFields = dbnumcols(d->dbProcess);
        if (columnNumber > (numFields - 1)) {
            kWarning() << "SybaseMigrate::drv_querySingleStringFromSQL(" << sqlStatement
            << "): columnNumber too large ("
            << columnNumber << "), expected 0.." << numFields;
        }
        stringList.append(value(i));
    }
    return true;
}

/*! Fetches single record from result obtained
 by running \a sqlStatement. */
tristate SybaseMigrate::drv_fetchRecordFromSQL(const QString& sqlStatement,
        KexiDB::RecordData& data, bool &firstRecord)
{
    if (firstRecord) {
        if (!query(sqlStatement))
            return false;
        firstRecord = false;
    }

    RETCODE returnCode = dbnextrow(d->dbProcess);
    if (returnCode != SUCCEED) {
        tristate r = cancelled;
        if (returnCode == FAIL)
            r = false;
        return r;
    }
    const int numFields = dbnumcols(d->dbProcess);
    data.resize(numFields);
    for (int i = 0; i < numFields; i++)
        data[i] = value(i) ;   //ok? utf8?
    return true;
}

/*! Copy Sybase table to KexiDB database */
bool SybaseMigrate::drv_copyTable(const QString& srcTable, KexiDB::Connection *destConn,
                                  KexiDB::TableSchema* dstTable)
{
    if (!query("Select * from " + drv_escapeIdentifier(srcTable)))  {
        return false;
    }

    const KexiDB::QueryColumnInfo::Vector fieldsExpanded(dstTable->query()->fieldsExpanded());
    RETCODE returnCode;
    while ((returnCode = dbnextrow(d->dbProcess)) != NO_MORE_ROWS) {
        const int numFields = qMin((int)fieldsExpanded.count(), (int)dbnumcols(d->dbProcess));
        QList<QVariant> vals;

        for (int i = 0; i < numFields; i++) {
            QString fieldValue = value(i);
            vals.append(KexiDB::cstringToVariant(fieldValue.toUtf8(), fieldsExpanded.at(i)->field, fieldValue.length()));
        }
        if (!destConn->insertRecord(*dstTable, vals)) {
            return false;
        }
        updateProgress();
    }

    if (returnCode == FAIL) {
        return false;
    }

    return true;
}

bool SybaseMigrate::drv_getTableSize(const QString& table, quint64& size)
{
    if (!query("SELECT COUNT(*) FROM " + drv_escapeIdentifier(table)))
        return false;
    while (dbnextrow(d->dbProcess) != NO_MORE_ROWS) {
        //! @todo check result valid
        size = value(0).toULongLong();
    }
    return true;
}

//! Convert a Sybase type to a KexiDB type, prompting user if necessary.
KexiDB::Field::Type SybaseMigrate::type(const QString& table,
                                        int columnType)
{
    // Field type
    KexiDB::Field::Type kexiType = KexiDB::Field::InvalidType;
    switch (columnType) {
    case SYBINT1:       // 1 byte integer
        kexiType = KexiDB::Field::Byte;
        break;

    case SYBINT2:   // 2 byte integer
        kexiType = KexiDB::Field::ShortInteger;
        break;

    case SYBINT4: // 4 byte integer
        kexiType = KexiDB::Field::Integer;
        break;

    case SYBINT8: // 8 byte integer
        kexiType = KexiDB::Field::BigInteger;
        break;

    case SYBFLT8: // 8 byte floating point
        kexiType = KexiDB::Field::Double;
        break;

    case SYBREAL: // 4 byte float type
        kexiType = KexiDB::Field::Float;
        break;

    case SYBDATETIME: // date time type
    case SYBDATETIME4:
        kexiType = KexiDB::Field::DateTime;
        break;

    case SYBMONEY: // money data types
    case SYBMONEY4:
        break;

    case SYBBIT: // bit data type
        kexiType = KexiDB::Field::Boolean;
        break;

    case SYBBINARY: // binary
        kexiType = KexiDB::Field::BLOB;
        break;

    case SYBCHAR: //
        kexiType = KexiDB::Field::Text;
        break;

    case SYBTEXT: // text type
        kexiType = KexiDB::Field::LongText;
        break;

    default:
        kexiType = KexiDB::Field::InvalidType;
        break;
    }

    if (kexiType == KexiDB::Field::InvalidType) {
        return userType(table);
    }

    return kexiType;
}

bool SybaseMigrate::primaryKey(const QString& tableName, const QString& fieldName) const
{
    // table fields information
    // http://infocenter.sybase.com/help/index.jsp?topic=/com.sybase.help.ase_15.0.sqlug/html/sqlug/sqlug538.htm
    // keycnt -> The number of fields involved in this index ( for clustered indexes )
    // status -> will help determine if this index is on a primary key
    //       ( the other case is on Unique Key )
    // indid  -> index Id.
    // id   -> The id of the table on which this index exists

    QString sqlStatement = QString("Select indid,keycnt,status from sysindexes where id = object_id('%1') and ( status & 2048 !=0 ) ").arg(drv_escapeIdentifier(tableName))  ;

    if (!query(sqlStatement)) {
        return false;
    }

    int indexId = -1 ,  keyCount = -1;
    // we are expecting only one row because there can be only *one* primary key
    while (dbnextrow(d->dbProcess) != NO_MORE_ROWS) {
        // get indexid :  indid
        indexId = value(0).toInt();
        // get key count : keycnt
        keyCount = value(1).toInt();
    }

    if (indexId != 1) {
        // if index is non clustered ( !=1 ), keycnt is 1 greater than the actual number of keys
        keyCount = keyCount - 1;
    }

    for (int i = 1; i <= keyCount; ++i) {
        sqlStatement = QString("Select 1 where index_col('%1',%2, %3 ) = '%4' ").arg(drv_escapeIdentifier(tableName)).arg(indexId).arg(i).arg(fieldName);

        if (!query(sqlStatement)) {
            return false;
        }

        while (dbnextrow(d->dbProcess) != NO_MORE_ROWS) {
            // we've had a hit!!
            return true;
        }
    }

    return false;

}

bool SybaseMigrate::uniqueKey(const QString& tableName, const QString& fieldName) const
{
    // table fields information
    // http://infocenter.sybase.com/help/index.jsp?topic=/com.sybase.help.ase_15.0.sqlug/html/sqlug/sqlug538.htm
    // keycnt -> The number of fields involved in this index ( for clustered indexes )
    // status -> will help determine if this index is on a primary key
    //       ( the other case is on Unique Key )
    // indid  -> index Id.
    // id   -> The id of the table on which this index exists

    QString sqlStatement = QString("Select indid,keycnt,status from sysindexes where id = object_id('%1') and ( status & 2 !=0 ) ").arg(drv_escapeIdentifier(tableName)) ;

    if (!query(sqlStatement)) {
        return false;
    }

    QMap<int, int> indexIdKeyCountMap;

    // we can expect multiple rows as there can be multiple unique indexes
    while (dbnextrow(d->dbProcess) != NO_MORE_ROWS) {
        // get indexid :  indid
        int indexId = value(0).toInt();
        // get key count : keycnt
        int keyCount = value(1).toInt();

        indexIdKeyCountMap[indexId] = keyCount;

    }

    // we need to check whether the field is involved in any one of the indexes
    foreach(int indexId, indexIdKeyCountMap.keys()) {

        int keyCount = indexIdKeyCountMap[indexId];

        if (indexId != 1) {
            // if index is non clustered ( !=1 ), keycnt is 1 greater than the actual number of keys
            keyCount = keyCount - 1;
        }

        for (int i = 1; i <= keyCount; i++) {
            sqlStatement = QString("Select 1 where index_col('%1',%2, %3 ) = '%4' ").arg(drv_escapeIdentifier(tableName)).arg(indexId).arg(i).arg(fieldName);

            if (!query(sqlStatement)) {
                return false;
            }

            while (dbnextrow(d->dbProcess) != NO_MORE_ROWS) {
                // we've had a hit!!
                return true;
            }
        }
    }

    return false;
}

QString SybaseMigrate::value(int pos) const
{
    // db-library indexes its columns from 1
    pos = pos + 1;

    long int columnDataLength = dbdatlen(d->dbProcess, pos);

    // 512 is
    // 1. the length used internally in dblib for allocating data to each column in function dbprrow()
    // 2. it's greater than all the values returned in the dblib internal function _get_printable_size
    long int pointerLength = qMax(columnDataLength , (long int)512);

    BYTE* columnValue = new unsigned char[pointerLength + 1] ;

    // convert to string representation. All values are convertible to string
    dbconvert(d->dbProcess , dbcoltype(d->dbProcess , pos), dbdata(d->dbProcess , pos), columnDataLength , (SYBCHAR), columnValue, -2);

    return QString::fromUtf8((const char*)columnValue, strlen((const char*)columnValue));
}

bool SybaseMigrate::query(const QString& sqlStatement) const
{

    //kDebug()<<sqlStatement;
    // discard any previous results, if remaining
    dbcancel(d->dbProcess);

    // insert command into buffer
    dbcmd(d->dbProcess, sqlStatement.toUtf8());

    // execute
    dbsqlexec(d->dbProcess);

    if (dbresults(d->dbProcess) != SUCCEED) {
        return false;
    }

    return true;

}

QList<KexiDB::IndexSchema*> KexiMigration::SybaseMigrate::readIndexes(const QString& tableName, KexiDB::TableSchema& tableSchema)
{
    // table fields information
    // http://infocenter.sybase.com/help/index.jsp?topic=/com.sybase.help.ase_15.0.sqlug/html/sqlug/sqlug538.htm
    // keycnt -> The number of fields involved in this index ( for clustered indexes )
    // status -> will help determine if this index is on a primary key
    //       ( the other case is on Unique Key )
    // indid  -> index Id.
    // id   -> The id of the table on which this index exists

    QList<KexiDB::IndexSchema*> indexList;

    QString sqlStatement = QString("Select indid,keycnt,status from sysindexes where id = object_id('%1')").arg(drv_escapeIdentifier(tableName)) ;

    if (!query(sqlStatement)) {
        return QList<KexiDB::IndexSchema*>();
    }

    // QMap< indexId, QPair<keycount,status> >
    QMap< int, QPair<int, int> > indexIdKeyCountStatusMap;

    // we can expect multiple rows as there can be multiple unique indexes
    while (dbnextrow(d->dbProcess) != NO_MORE_ROWS) {
        // get indexid :  indid
        int indexId = value(0).toInt();
        // get key count : keycnt
        int keyCount = value(1).toInt();

        // get status of the index
        int status = value(2).toInt();

        indexIdKeyCountStatusMap[indexId] = qMakePair(keyCount, status);

    }

    // create a FieldName -> FieldPointer Hash for faster lookup by name
    QHash<QString, KexiDB::Field*> fieldHash;
    const KexiDB::Field::List* fieldList = tableSchema.fields();

    foreach(KexiDB::Field* field, *fieldList) {
        kDebug() << field->caption();
        fieldHash[field->caption()] = field;
    }


    // we need to check whether the field is involved in any one of the indexes
    foreach(int indexId, indexIdKeyCountStatusMap.keys()) {

        QPair<int, int> keyCountStatusPair = indexIdKeyCountStatusMap[indexId];

        int keyCount = keyCountStatusPair.first;
        int status = keyCountStatusPair.second;

        if (indexId != 1) {
            // if index is non clustered ( !=1 ), keycnt is 1 greater than the actual number of keys
            // see link for more information
            keyCount = keyCount - 1;
        }

        KexiDB::IndexSchema* indexSchema = new KexiDB::IndexSchema(&tableSchema);

        for (int i = 1; i <= keyCount; i++) {
            sqlStatement = QString("Select index_col('%1',%2, %3 )").arg(drv_escapeIdentifier(tableName)).arg(indexId).arg(i);

            if (!query(sqlStatement)) {
                return QList<KexiDB::IndexSchema*>();
            }

            while (dbnextrow(d->dbProcess) != NO_MORE_ROWS) {
                // only one row is expected
                QString fieldName = value(0);
                kDebug() << fieldName;
                indexSchema->addField(fieldHash[fieldName]);
            }
        }

        // now check if this index is either a primary key/ unique key etc
        // see link for more information about status values
        if ((status & 2048) != 0) {      // primary key check
            indexSchema->setPrimaryKey(true);
        } else if ((status & 2) != 0) {      // unique check
            indexSchema->setUnique(true);
        }

        indexList.append(indexSchema);
    }

    return indexList;
}

#include "sybasemigrate.moc"
