/* This file is part of the KDE project
   Copyright (C) 2009 Sharan Rao <sharanrao@gmail.com>

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

#include "odbcdriver.h"
#include "odbccursor.h"
#include "odbcconnection.h"
#include "odbcconnection_p.h"
#include "odbctypeinfoqueryunit.h"
#include "odbcspecialcolumnsqueryunit.h"

#include <kexidb/field.h>
#include <kexidb/driver_p.h>
#include <kexidb/utils.h>

#include <QVariant>
#include <QFile>

#include <sql.h>
#include <sqltypes.h>

#include <kgenericfactory.h>
#include <kdebug.h>

using namespace KexiDB;

class ODBCDriver::ODBCDatabaseProperties {

public:
    /**
     * Returns the single instance of this class
     */
    static ODBCDatabaseProperties* getInstance();

    /**
     * Returns the behaviour structure for the given dbname, else returns null
     * \param dbName The name of the DBMS for which the bevaiour needs to be retrieved
     * \return The value of the driver behaviour for the given dbName, propName
     */
    DriverBehaviour* driverBehaviour(const QByteArray& dbName);

    ~ODBCDatabaseProperties();

protected:
    ODBCDatabaseProperties();

    /**
     * Stores behaviour for each database
     */
    QHash<QByteArray, DriverBehaviour* > m_databaseBehaviourHash;

    static ODBCDatabaseProperties* s_instance;
};

ODBCDriver::ODBCDatabaseProperties* ODBCDriver::ODBCDatabaseProperties::s_instance = 0;

ODBCDriver::ODBCDatabaseProperties* ODBCDriver::ODBCDatabaseProperties::getInstance() {
    if ( !s_instance ) {
        s_instance = new ODBCDatabaseProperties;
    }
    return s_instance;
}

ODBCDriver::ODBCDatabaseProperties::~ODBCDatabaseProperties() {
    QHashIterator<QByteArray, DriverBehaviour*> i(m_databaseBehaviourHash);
    while (i.hasNext()) {
        i.next();
        delete i.value();
    }
}

DriverBehaviour* ODBCDriver::ODBCDatabaseProperties::driverBehaviour(const QByteArray& dbName) {
    QHash<QByteArray, DriverBehaviour*>::iterator it = m_databaseBehaviourHash.find( dbName.toLower() );
    if ( it == m_databaseBehaviourHash.end() ) {
        return 0;
    }
    return it.value();
}

ODBCDriver::ODBCDatabaseProperties::ODBCDatabaseProperties() {
    // fill the database properties map with whatever we know

    // for MySQL
    DriverBehaviour* mysqlBehaviour = new DriverBehaviour;
    mysqlBehaviour->ROW_ID_FIELD_NAME = "LAST_INSERT_ID()";
    mysqlBehaviour->ROW_ID_FIELD_RETURNS_LAST_AUTOINCREMENTED_VALUE = true;
    m_databaseBehaviourHash.insert( "mysql", mysqlBehaviour ); // insert mysql entry into hash

    // for pg
    DriverBehaviour* pgBehaviour = new DriverBehaviour;
    pgBehaviour->UNSIGNED_TYPE_KEYWORD = "";
    pgBehaviour->ROW_ID_FIELD_NAME = "oid";
    pgBehaviour->SPECIAL_AUTO_INCREMENT_DEF = false;
    pgBehaviour->AUTO_INCREMENT_TYPE = "SERIAL";
    pgBehaviour->AUTO_INCREMENT_FIELD_OPTION = "";
    pgBehaviour->AUTO_INCREMENT_PK_FIELD_OPTION = "PRIMARY KEY";
    m_databaseBehaviourHash.insert( "postgresql", pgBehaviour ); // insert pg entry into hash

    // for sybase
    DriverBehaviour* sybaseBehaviour = new DriverBehaviour;
    sybaseBehaviour->ROW_ID_FIELD_NAME = "@@IDENTITY";
    sybaseBehaviour->ROW_ID_FIELD_RETURNS_LAST_AUTOINCREMENTED_VALUE = true ;
    // for Sybase ASA this field is "DEFAULT AUTOINCREMENT"
    // for MSSQL and Sybase ASE it's IDENTITY
    sybaseBehaviour->AUTO_INCREMENT_FIELD_OPTION = "IDENTITY";
    sybaseBehaviour->AUTO_INCREMENT_PK_FIELD_OPTION = "IDENTITY PRIMARY KEY ";
    m_databaseBehaviourHash.insert( "sybase", sybaseBehaviour ); // insert sybase behaviour into hash

    // for sqlite
    DriverBehaviour* sqliteBehaviour = new DriverBehaviour;
    sqliteBehaviour->SPECIAL_AUTO_INCREMENT_DEF = true;
    sqliteBehaviour->AUTO_INCREMENT_FIELD_OPTION = ""; //not available
    sqliteBehaviour->AUTO_INCREMENT_TYPE = "INTEGER";
    sqliteBehaviour->AUTO_INCREMENT_PK_FIELD_OPTION = "PRIMARY KEY";
    sqliteBehaviour->AUTO_INCREMENT_REQUIRES_PK = true;
    sqliteBehaviour->ROW_ID_FIELD_NAME = "OID";
    m_databaseBehaviourHash.insert( "sqlite", sqliteBehaviour ); // insert sqlite behaviour into hash

    // for oracle
    DriverBehaviour* oracleBehaviour = new DriverBehaviour;
    oracleBehaviour->ROW_ID_FIELD_NAME="ROW_ID";
    // oracleBehaviour->ROW_ID_FIELD_RETURNS_LAST_AUTOINCREMENTED_VALUE=true; I don't think so
    oracleBehaviour->UNSIGNED_TYPE_KEYWORD="";
    //Autoincrement
    oracleBehaviour->SPECIAL_AUTO_INCREMENT_DEF = false;
    oracleBehaviour->AUTO_INCREMENT_FIELD_OPTION="";
    oracleBehaviour->AUTO_INCREMENT_PK_FIELD_OPTION="PRIMARY KEY";
    oracleBehaviour->AUTO_INCREMENT_TYPE = "";
    m_databaseBehaviourHash.insert( "oracle", oracleBehaviour ); // insert oracle behaviour into hash

    // default
    DriverBehaviour* defaultBehaviour = new DriverBehaviour;
    m_databaseBehaviourHash.insert( "default", defaultBehaviour ); // insert default behaviour into hash
}

/*!
 * Constructor sets database features and
 * maps the types in KexiDB::Field::Type to the ODBC types.
 *
 */
ODBCDriver::ODBCDriver(QObject *parent, const QStringList &args) :
        Driver(parent, args),
        m_driverInfoUpdated( false ),
        o_d( ODBCDatabaseProperties::getInstance() )
{
// KexiDBDrvDbg << "ODBCDriver::ODBCDriver()";

    d->isFileDriver = false;
    d->features = IgnoreTransactions | CursorForward; // TODO

    beh->_1ST_ROW_READ_AHEAD_REQUIRED_TO_KNOW_IF_THE_RESULT_IS_EMPTY = false;
    beh->USING_DATABASE_REQUIRED_TO_CONNECT = false;
}

ODBCDriver::~ODBCDriver()
{
}

KexiDB::Connection*
ODBCDriver::drv_createConnection(ConnectionData &conn_data)
{
    KexiDB::Connection* connection = new ODBCConnection(this, conn_data);
    return connection;
}

bool ODBCDriver::isSystemDatabaseName(const QString&  name) const
{
    // TODO. somehow query the data source to find out.
    return Driver::isSystemObjectName(name);
}

bool ODBCDriver::drv_isSystemFieldName(const QString&) const
{
    return false;
}

QString ODBCDriver::escapeString(const QString& str) const
{
    //! TODO: Find if we get any info about escaping strings
    return QString::fromLatin1("'") +
           QString(str).replace("\'", "\\''") +
           QString::fromLatin1("'");
}

QString ODBCDriver::escapeBLOB(const QByteArray& array) const
{
    //! TODO: Find if we get any info about escaping blobs ( wasn't there something in SQLGetInfo ? )
    return KexiDB::escapeBLOB(array, KexiDB::BLOBEscape0xHex);
}

QByteArray ODBCDriver::escapeString(const QByteArray& str) const
{
    //! TODO: Find if we get any info about escaping strings
    return QByteArray("'") + QByteArray(str)
           .replace("\'", "\\''")
           + QByteArray("'");
}

/*! Add back-ticks to an identifier, and replace any back-ticks within
 * the name with single quotes.
 */
QString ODBCDriver::drv_escapeIdentifier(const QString& str) const
{
    return str;
}

QByteArray ODBCDriver::drv_escapeIdentifier(const QByteArray& str) const
{
    return str;
}

bool ODBCDriver::updateDriverInfo(ODBCConnection* connection )
{
    if ( m_driverInfoUpdated ) {
        return true;
    }

    // Before I forget/for future use, let me note down what's required to be done here
    // 1. Get the information about all the types - use the odbctypeinfoqueryunit   - done
    // 2. Get the quotation marks for identifier - sqlgetinfo ? - done
    // 3. initialize the driver specific keywords - hmm - done
    // 4. check if select 1 subquery is supported - done
    // 5. Get the rowid column name - suspended
    // 6. Get the rowid/auto increment behavioural parameters

    // executes step 1
    if ( !populateTypeInfo(connection) )
        return false;

    // executes steps 2,3,4
    if ( !populateGeneralInfo(connection ) ) {
        return false;
    }

    // execute step 5.
//     if ( !populateROWID( connection ) ) {
//         //return false;
//     }

    if ( !populateBehaviourInfo( m_dbmsName ) ) {
        KexiDBDrvWarn <<"Loaded default behaviour for database "<<m_dbmsName
                     <<".Nothing guaranteed now :)";
    }

    m_driverInfoUpdated = true;
    return true;
}

bool ODBCDriver::populateTypeInfo(ODBCConnection* connection )
{
    ODBCTypeInfoQueryUnit* queryUnit = new ODBCTypeInfoQueryUnit( connection );
    Cursor* cursor = new ODBCCursor( connection, queryUnit );

    if ( !cursor->open() || !cursor->moveFirst() ) {
        connection->deleteCursor( cursor );
        return false;
    }

    while ( !cursor->eof() ) {
        // get all the values
        QString typeName = cursor->value( 0 ).toString();
        switch( cursor->value(1 ).toInt() ) {
        case SQL_TINYINT:
            setTypeName(Field::Byte, typeName);
            break;
        case SQL_SMALLINT:
            setTypeName(Field::ShortInteger, typeName);
            break;
        case SQL_INTEGER:
            setTypeName(Field::Integer, typeName);
            break;
        case SQL_BIGINT:
            setTypeName(Field::BigInteger, typeName);
            break;
        case SQL_BIT:
            setTypeName(Field::Boolean, typeName);
            break;
        case SQL_TYPE_DATE:
            setTypeName(Field::Date, typeName);
            break;
        case SQL_TYPE_TIMESTAMP:
            setTypeName(Field::DateTime, typeName);
            break;
        case SQL_TYPE_TIME:
            setTypeName(Field::Time, typeName);
            break;
        case SQL_FLOAT:
            setTypeName(Field::Float, typeName);
            break;
        case SQL_DOUBLE:
            setTypeName(Field::Double, typeName);
            break;
        case SQL_VARCHAR:
            setTypeName(Field::Text, typeName);
            break;
        case SQL_LONGVARCHAR:
            setTypeName(Field::LongText, typeName);
            break;
        case SQL_VARBINARY:
            setTypeName(Field::BLOB, typeName);
            break;
        default:
            break;
        }
        if (!cursor->moveNext() && cursor->error()) {
            connection->deleteCursor(cursor);
            return false;
        }
    }

    return connection->deleteCursor( cursor );
}

bool ODBCDriver::populateROWID(ODBCConnection* connection ) {
    // pass true, as we want *some* table. If there are no tables which already exist
    // lets atleast get the kexidb system tables
    QStringList tableNames = connection->tableNames(true);
    if ( tableNames.size() == 0 ) {
        return false;
    }

    QString tableName = tableNames.first(); // get any existing table name

    // the ODBC function which retrieves special columns needs some table to work on
    // as we are inerested only in pseudo columns like ROWID, any table would do
    ODBCSpecialColumnsQueryUnit* queryUnit = new ODBCSpecialColumnsQueryUnit( connection , tableName );
    Cursor* cursor = new ODBCCursor( connection, queryUnit );

    if ( !cursor->open() || !cursor->moveFirst() ) {
        connection->deleteCursor( cursor );
        return false;
    }

    // we need exactly one pseudo-column. So if the pseudo column count is greater than that, quit :(
    unsigned int pseudoColumnCount = 0;
    QString columnName;
    while ( !cursor->eof() ) {
        // eigth column is the pseudo column indicator ( cursor indexes by 0, hence we use 7 )
        // see http://msdn.microsoft.com/en-us/library/ms714602(VS.85).aspx
        if ( cursor->value( 7 ).toInt() != ( int )SQL_PC_PSEUDO )
            continue;

        // else we have a pseudo column !
        columnName = cursor->value( 0 ).toString();
        pseudoColumnCount++;
    }

    if ( pseudoColumnCount == 1 ) {
        beh->ROW_ID_FIELD_NAME = columnName;
    }

    return true;
}

void ODBCDriver::setTypeName( Field::Type type, const QString& typeName)
{
    // explaining this is a bit tricky
    // this is mainly done for mysql. now, mysql is, as we all know, harassing.
    // it returns 4 datatypes for SQL_INTEGER and some other data types
    // int, int unsigned, int auto_increment, int unsigned auto_increment
    // ( yes, all with SQL_INTEGER, decorated with other values for unsigned and autoincrement )
    // if I hadn't done this the typename would have gone as int unsigned auto_increment

    if ( d->typeNames[type].isEmpty() ) { // the typenames vector had been allocated using resize which calls default ctor
        d->typeNames[type] = typeName;    // for QString, which is QString()
    }
}

bool ODBCDriver::populateGeneralInfo(ODBCConnection* connection)
{
    SQLCHAR identifierQuote[5];
    SQLSMALLINT stringLen;
    SQLRETURN returnStatus;
    SQLCHAR stringVal[1024];

    // get the database name
    returnStatus = SQLGetInfo( connection->d->connectionHandle,  SQL_DBMS_NAME, stringVal, sizeof(stringVal), &stringLen );
    if ( !SQL_SUCCEEDED(returnStatus) ) {
        ODBCConnection::extractError( connection, connection->d->connectionHandle , SQL_HANDLE_DBC );
        return false;
    }
    m_dbmsName = QString::fromLatin1( ( const char* )stringVal );

    // Get the quotation marks for identifier
    returnStatus = SQLGetInfo( connection->d->connectionHandle, SQL_IDENTIFIER_QUOTE_CHAR, identifierQuote, sizeof( identifierQuote ), &stringLen );
    if ( !SQL_SUCCEEDED(returnStatus ) ) {
        ODBCConnection::extractError( connection, connection->d->connectionHandle, SQL_HANDLE_DBC );
        return false;
    }
    beh->QUOTATION_MARKS_FOR_IDENTIFIER = QString::fromLatin1( ( const char* )identifierQuote )[0];

    SQLCHAR sqlKeywords[1000];
    returnStatus = SQLGetInfo( connection->d->connectionHandle, SQL_KEYWORDS, sqlKeywords, sizeof( sqlKeywords ), &stringLen );
    if ( !SQL_SUCCEEDED(returnStatus ) ) {
        ODBCConnection::extractError( connection, connection->d->connectionHandle, SQL_HANDLE_DBC );
        return false;
    }
    QStringList keywordList = QString::fromLatin1( ( const char* )sqlKeywords ).split( "," );

    // Get the Keywords
    char** keywords;
    keywords = new char*[keywordList.size() + 1];
    // TODO TODO convert the keywordList to a character array
    int i = 0;
    foreach( const QString& keyword, keywordList ) {
        QByteArray ba = keyword.toAscii();
        // we want ownership as the bytearray is going to be destroyed.
        // we don't want ownership as the KexiUtils::StaticSetOfStrings, is not going to delete thiss
        // TODO: find a solution :)
        keywords[i] = new char[keyword.length() + 1];
        strcpy( keywords[i], ba.constData() );
        i++;
    }

    keywords[i] = 0;
    initDriverSpecificKeywords( ( const char** )keywords);

    // get whether subqueries are supported. Only checking for EXISTS
    SQLUINTEGER subqueries;
    returnStatus = SQLGetInfo( connection->d->connectionHandle, SQL_SUBQUERIES, ( SQLPOINTER )&subqueries , 0 , &stringLen );
    if ( !SQL_SUCCEEDED(returnStatus ) ) {
        ODBCConnection::extractError( connection, connection->d->connectionHandle, SQL_HANDLE_DBC );
        return false;
    }
    beh->SELECT_1_SUBQUERY_SUPPORTED = ( bool )( subqueries & SQL_SQ_EXISTS );

    return true;
}

bool ODBCDriver::populateBehaviourInfo(const QString& dbName)
{
    bool defaultBehaviourLoaded = false;

    KexiDBDrvDbg <<"Loading behaviour for :"<< dbName;

    DriverBehaviour* db = o_d->driverBehaviour(dbName.toAscii());
    if ( !db ) {
        db = o_d->driverBehaviour( "default" );
        defaultBehaviourLoaded = true;
    }

    // unsigned keyword;
    beh->UNSIGNED_TYPE_KEYWORD = db->UNSIGNED_TYPE_KEYWORD;

    // rowid related
    beh->ROW_ID_FIELD_NAME = db->ROW_ID_FIELD_NAME;
    beh->ROW_ID_FIELD_RETURNS_LAST_AUTOINCREMENTED_VALUE = db->ROW_ID_FIELD_RETURNS_LAST_AUTOINCREMENTED_VALUE;

    // auto increment related
    beh->SPECIAL_AUTO_INCREMENT_DEF = db->SPECIAL_AUTO_INCREMENT_DEF;
    beh->AUTO_INCREMENT_TYPE = db->AUTO_INCREMENT_TYPE;
    beh->AUTO_INCREMENT_FIELD_OPTION = db->AUTO_INCREMENT_FIELD_OPTION;
    beh->AUTO_INCREMENT_PK_FIELD_OPTION = db->AUTO_INCREMENT_PK_FIELD_OPTION;
    beh->AUTO_INCREMENT_REQUIRES_PK = db->AUTO_INCREMENT_REQUIRES_PK;

    return !defaultBehaviourLoaded;
}

QString ODBCDriver::getQueryForOID()
{
    // will only work for sybase/mysql
    if ( beh->ROW_ID_FIELD_RETURNS_LAST_AUTOINCREMENTED_VALUE )
        return "Select " + beh->ROW_ID_FIELD_NAME;
    return QString();
}


// to make the compiler shut up :).
DatabaseVersionInfo ODBCDriver::version() const { return KEXIDB_VERSION; }


#include "odbcdriver.moc"


