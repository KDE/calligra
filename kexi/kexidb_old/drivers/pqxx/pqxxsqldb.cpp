/* This file is part of the KDE project
  Copyright (C) 2003  Dawit Alemayehu <adawit@kde.org>
 
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.
*/

#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <kdebug.h>
#include <kgenericfactory.h>

#include <kexidbfield.h>

#include "pqxxsqldb.h"
#include "pg_type.h"

K_EXPORT_COMPONENT_FACTORY(kexipqxxsqlinterface, KGenericFactory<pqxxSqlDB>( "pqxxsqlinterface" ));

//=========================================================================
//Default constructor
pqxxSqlDB::pqxxSqlDB(QObject *parent, const char *name, const QStringList &)
        :KexiDB(parent, name), _dbConn(0), _dbResult(0)
{
    kdDebug() << "pqxxSqlDB::Constructing pqxxsql plugin :)" << endl;
    _dbConn = 0;
    _dbTransaction = 0;
    _dbResult = 0;
}

//=========================================================================
//Destructor
pqxxSqlDB::~pqxxSqlDB()
{
    clearResultInfo ();
    clearConnectionInfo ();
    kdDebug() << "pqxxSqlDB::Deleting pqxx Plugin :(" << endl;
}

//=========================================================================
//Clears the current connection to the server
void pqxxSqlDB::clearConnectionInfo ()
{
    if (_dbConn)
    {
        _dbConn->disconnect();
        delete _dbConn;
    }
}

//=========================================================================
//Clears the current result
void pqxxSqlDB::clearResultInfo ()
{
    if (_dbResult)
    {
        delete _dbResult;
        _dbResult = 0;
    }
    if (_dbTransaction)
    {
        delete _dbTransaction;
        _dbTransaction = 0;
    }
}

//=========================================================================
//Return the name of the driver for use in the application
QString pqxxSqlDB::driverName()
{
    return QString::fromLatin1 ("PostgreSQL (libpqxx)");
}

//=========================================================================
//Returns data as a recordset
KexiDBRecordSet* pqxxSqlDB::queryRecord (const QString& querystatement, bool buffer)
{
    kdDebug() << "pqxxSqlDB::queryRecord" << endl;

    m_error.setup(0);

    if (query(querystatement))
    {
        //if(!buffer)
        //	res = mysql_use_result(m_mysql);
        //else
        //	res = mysql_store_result(m_mysql);

        if(_dbResult)
        {
            pqxxSqlRecord *rec = new pqxxSqlRecord(_dbResult, this, "record", false);
            return rec;
        }
    }
    else
    {
        kdDebug() << "pqxxSqlDB::queryRecord(): error:" + m_error.message() << endl;
    }

    return 0;

}
//=========================================================================
//Returns a list of databases on the current server
QStringList  pqxxSqlDB::databases ()
{
    QStringList list;

    if (query("SELECT datname FROM pg_database WHERE datallowconn = TRUE"))
    {
        for (result::const_iterator c = _dbResult->begin(); c != _dbResult->end(); ++c)
        {
            // Read value of column 0 into a string N
            string N;
            c[0].to(N);

            // Copy the result into the return list
            list << QString::fromLatin1 (N.c_str());
        }

    }

    return list;
}

//=========================================================================
//Returns true if the specified table is a system table
bool pqxxSqlDB::isSystemDatabase(QString& dbName)
{
    return false;
}

//=========================================================================
//Returns a list of tables in a the current database
QStringList pqxxSqlDB::tableNames ()
{
    QStringList list;

    //pg_ = standard postgresql tables, pga_ = tables added by pgaccess, sql_ = probably information schemas
    //TODO: There must be better ways of selecting user tables than this
    if (query("SELECT relname FROM pg_class WHERE ((relkind = 'r') AND ((relname !~ '^pg_') AND (relname !~ '^pga_') AND (relname !~ '^sql_')))", false))
    {
        for (result::const_iterator c = _dbResult->begin(); c != _dbResult->end(); ++c)
        {
            // Read value of column 0 into a string N
            string N;
            c[0].to(N);

            // Copy the result into the return list
            list << QString::fromLatin1 (N.c_str());
        }

        m_tableDefs.clear();
        for(QStringList::const_iterator it=list.begin();it!=list.end();++it)
        {
            m_tableDefs.insert((*it),createTableDef(*it));
        }
    }
    return list;
}

//=========================================================================
//
const KexiDBTable *const pqxxSqlDB::table(const QString& name)
{
    return m_tableDefs[name];
}

//=========================================================================
//Performs a query on a database
bool pqxxSqlDB::query(const QString& statement)
{
    return query(statement, true);
}

bool pqxxSqlDB::query (const QString& statement, bool trans)
{
    kdDebug() << "pqxxSqlDB::Query: " << statement.latin1() << endl;

    Q_ASSERT (_dbConn);

    // Clear the last result information...
    clearResultInfo ();

    // Clear out the database error message...
    m_error.clear ();

    try
    {
        //Create a transaction
        if (trans)
        {
            _dbTransaction = new transaction<>(*_dbConn);
        }
        else
        {
            _dbTransaction = new nontransaction(*_dbConn);
        }
        //Create a result opject through the transaction
        _dbResult = new result(_dbTransaction->exec(statement.latin1()));

        //Commit the transaction
        _dbTransaction->commit();


        //If all went well then return true, errors picked up by the catch block
        return true;
    }
    catch (const exception &e)
    {
        //If an error ocurred then put the error description into _dbError
        m_error.setup (-1, e.what());
        kdDebug() << "pqxxSqlDB::query:exception - " << e.what() << endl;
        return false;
    }
}

//=========================================================================
//Returns the last error that ocurred to the application
KexiDBError* pqxxSqlDB::latestError()
{
    return &m_error;
}

//=========================================================================
//Properly quotes an SQL statement given a string
QString pqxxSqlDB::escape (const QString& str) const
{
    string to;
    to = pqxx::Quote(str.latin1());

    kdDebug() << "pqxxSqlDB::escaped: " << to << endl;

    return to.c_str();
}

//=========================================================================
//Properly quotes an SQL statement given an array of bytes
QString pqxxSqlDB::escape (const QByteArray& str) const
{
    QString to;
    to = escape(str.data());

    return to;
}

//=========================================================================
//Connects to the default database of a server
bool pqxxSqlDB::connect (const QString& host, const QString& user, const QString& password, const QString& socket, const QString& port)
{
    return connect (host, user, password, socket, port, "");
}

//=========================================================================
//Connects to s specific database on a server
bool pqxxSqlDB::connect (const QString& host, const QString& user, const QString& password, const QString& socket, const QString& port, const QString& db, bool create)
{
    if (_dbConn)
    {
        if (_dbConn->is_open() &&
                db == QString::fromLatin1(_dbConn->dbname()) &&
                host == QString::fromLatin1(_dbConn->hostname()) &&
                user == QString::fromLatin1(_dbConn->username()) /*&&
                                                                                                                                    password == QString::fromLatin1(_dbConn)*/ &&
                port == QString::fromLatin1(_dbConn->port())) //Currently not checking passwd is equal as its not part of the connection class
            return true;

        // Close the current connection and establish a new one...
        clearConnectionInfo ();
    }

    QString conninfo;
    if (!host.isNull())
        conninfo = QString::fromLatin1("host='%1'").arg(host);

    bool ok;
    if (port.toInt(&ok) && ok)
        conninfo += QString::fromLatin1(" port='%1'").arg(port);

    if (db.isEmpty() || create)
        conninfo += QString::fromLatin1(" dbname='template1'");
    else
        conninfo += QString::fromLatin1(" dbname='%1'").arg(db);

    if (!user.isNull())
        conninfo += QString::fromLatin1(" user='%1'").arg(user);

    if (!password.isNull())
        conninfo += QString::fromLatin1(" password='%1'").arg(password);

    kdDebug () << "pqxxSqlDB::Connection info: " << conninfo << endl;
    kdDebug () << "pqxxSqlDB::Create: " << create << endl;

    m_error.clear ();
    try
    {
        _dbConn = new connection( conninfo.latin1() );
        m_connectedDB=true;
    }
    catch(const exception &e)
    {
        m_error.setup (-1, e.what());
        kdDebug() << "pqxxSqlDB::connect:exception - " << e.what() << endl;
        return false;
    }

    if (create && !db.isEmpty())
    {
        // If we are able to create the requested database, close the
        // current connection and open a new connection the created db
        if (query (QString::fromLatin1("CREATE DATABASE %1").arg(db), false))
            return connect (host, user, password, socket, port, db);
    }

    return true;
}

//=========================================================================
//
pqxxSqlResult* pqxxSqlDB::getResult()
{
    return useResult();
}

//=========================================================================
//
pqxxSqlResult* pqxxSqlDB::storeResult()
{
    kdDebug() << "pqxxSqlDB::storeResult()" << endl;
    /*	Result *res = mysql_store_result(m_mysql);
    	if(res)
    	{
    		kdDebug() << "MySqlDB::storeResult(): wow, got a result!!!" << endl;
    		return new pqxxSqlResult(res, this);
    	}
    	else
    	{
    		return 0;
    	}*/
    return 0;
}

//=========================================================================
//
pqxxSqlResult* pqxxSqlDB::useResult()
{
    kdDebug() << "pqxxSqlDB::useResult():" << endl;
    /*	MYSQL_RES *res = mysql_use_result(m_mysql);
    	if(res)
    	{
    		kdDebug() << "MySqlDB::useResult(): d2" << endl;
    		MySqlResult *result = new MySqlResult(res, this);
    		kdDebug() << "MySqlDB::useResulg(): d3" << endl;
    		return result;
    	}
    	else
    	{
    		kdDebug() << "MySqlDB::useResult(): not enough data" << endl;
    		return 0;
    	}*/
    return 0;
}

//=========================================================================
//
unsigned long pqxxSqlDB::affectedRows()
{
    kdDebug() << "pqxxSqlDB::affectedRows():" << endl;
    return _dbResult->AffectedRows();
}

//=========================================================================
//Return the id of the last inserted row
result* pqxxSqlDB::lastRow(const string m_table)
{
    stringstream statement;
    transaction_base* tran;
    result* tmpres;

    kdDebug() << "pqxxSqlDB::lastRow()" << endl;

    try
    {
        statement << "SELECT * FROM "<< escapeName(m_table) << " WHERE (oid = ";
        statement << _dbResult->InsertedOid() << ")";

        tran = new nontransaction(*_dbConn, "find_attnotnull");
        tmpres = new result(tran->exec(statement.str()));

        tran->commit();
        if (tmpres->size() != 1)
        {
            kdDebug() << "pqxxSqlDB::lastRow:Strange....we have " << tmpres->size() << " results." << endl;
            delete tmpres;
            tmpres = 0;
        }
    }
    catch(const exception &e)
    {
        kdDebug() << "pqxxSqlDB::lastRow:exception - " << e.what() << endl;
        kdDebug() << "pqxxSqlDB::lastRow:failed statement - " << statement.str() << endl;
        if (tmpres)
            delete tmpres;

        tmpres = 0;
    }

    if (tran)
        delete tran;

    return tmpres;
}

//=========================================================================
//Alters a field given a lname and a definition
bool pqxxSqlDB::alterField(const KexiDBField& changedField, unsigned int index, KexiDBTableStruct fields)
{
    kdDebug() << "pqxxSqlDB::alterField: Table: " << changedField.table() << " Field: " << fields.at(index)->name() << endl;
    kdDebug() << "pqxxSqlDB::alterField: DataType: " << nativeDataType(changedField.sqlType()) << "ColumnType: " << changedField.sqlType() << endl;
    QString qstr = "ALTER TABLE " + changedField.table() + " CHANGE " +
                   fields.at(index)->name() + " " + changedField.name();
    qstr += " " + createDefinition(changedField, index, fields);
    kdDebug() << "pqxxSqlDB::alterField: Query: " << qstr << endl;
    bool ok = query(qstr, true);

    if(ok)
    {
        ok = changeKeys(changedField, index, fields);
    }

    return ok;
}

//=========================================================================
//Creates a field given a name and definition
bool pqxxSqlDB::createField(const KexiDBField& newField, KexiDBTableStruct fields, bool createTable)
{
    kdDebug() << "pqxxSqlDB::createField: Table: " << newField.table() << " Field: " << newField.name() << endl;
    kdDebug() << "pqxxSqlDB::createField: DataType: " << nativeDataType(
        newField.sqlType()) << "ColumnType: " << newField.sqlType() << endl;
    QString qstr;

    if(!createTable)
    {
        qstr = "ALTER TABLE " + newField.table() + " ADD " + newField.name();
        qstr += " " + createDefinition(newField, -1, fields);
    }
    else
    {
        qstr = "CREATE TABLE " + newField.table() + " (" + newField.name();
        qstr += " " + createDefinition(newField, -1, fields);
        qstr += ")";
    }

    bool ok = query(qstr, true);

    if(ok)
    {
        ok = changeKeys(newField, -1, fields);
    }

    return ok;
}

//=========================================================================
//Creates a field definition given its structure
QString pqxxSqlDB::createDefinition(const KexiDBField& field, int index, KexiDBTableStruct fields)
{
    kdDebug() << "pqxxSqlDB::createDefinition()" << endl;
    QString qstr = nativeDataType(field.sqlType());
    bool allowUnsigned = false;

    switch(field.sqlType())
    {
    case KexiDBField::SQLInteger:
    case KexiDBField::SQLSmallInt:
    case KexiDBField::SQLTinyInt:
    case KexiDBField::SQLBigInt:
        allowUnsigned = true;
    case KexiDBField::SQLVarchar:
        qstr += "(" + QString::number((field.length()==0)?255:field.length()) + ")";
        break;
    case KexiDBField::SQLDecimal:
    case KexiDBField::SQLFloat:
    case KexiDBField::SQLDouble:
    case KexiDBField::SQLNumeric:
        allowUnsigned = true;
        qstr += "(" + QString::number(field.length()) + "," + QString::number(field.precision()) + ")";
        break;
    case KexiDBField::SQLInvalid:
    case KexiDBField::SQLBinary:
    case KexiDBField::SQLBoolean:
    case KexiDBField::SQLDate:
    case KexiDBField::SQLLongVarBinary:
    case KexiDBField::SQLTime:
    case KexiDBField::SQLTimeStamp:
    case KexiDBField::SQLVarBinary:
    case KexiDBField::SQLInterval:
    case KexiDBField::SQLLongVarchar:
    case KexiDBField::SQLLastType:
        break;
    }

    if((field.constraints() & KexiDBField::CCNotNull) || field.primary_key())
    {
        qstr += " NOT NULL";
    }
    else
    {
        qstr += " NULL";
    }

    if(field.binary() && (field.sqlType() == KexiDBField::SQLVarchar))
    {
        qstr += " BINARY";
    }

    if(field.unsignedType() && allowUnsigned)
    {
        qstr += " UNSIGNED";
    }

    if(!field.defaultValue().toString().isEmpty())
    {
        qstr += " DEFAULT " + field.defaultValue().toString();
    }

    if(field.constraints() & KexiDBField::CCAutoInc)
    {
        qstr += " SERIAL PRIMARY KEY";
    }

    return qstr;
}

//=========================================================================
//Returns the structure (field info) for a table
KexiDBTableStruct pqxxSqlDB::structure(const QString& table) const
{
    kdDebug() << "pqxxSqlDB::structure(" << table << ")" << endl;
    cout << "pqxxSqlDB::structure(" << table << ")" << endl;
    KexiDBTableStruct dbStruct;
    stringstream stmt;
    transaction_base* tran;
    result* tmpres;

    try
    {
        stmt << "select * from " << escapeName(table.local8Bit()) << " limit 0";
        tran = new nontransaction(*_dbConn, "structure");
        tmpres = new result(tran->exec(stmt.str()));
        tran->commit();
        if(tmpres)
        {
            for(int i = 0; i < tmpres->Columns(); i++)
            {
                oid toid = tmpres->column_table(i);
                kdDebug() << "Table UID: " << toid << endl;
                KexiDBField* f = new KexiDBField(tableName(toid));
                f->setName(tmpres->column_name(i));
                f->setColumnType(getInternalDataType(tmpres->column_type(i)));
                //f->setLength(tmpres->column_size(i));
                //f->setPrecision(m_field->decimals);
                //f->setUnsigned(m_field->flags & UNSIGNED_FLAG);
                //f->setBinary(m_field->flags & BINARY_FLAG);
                //f->setDefaultValue(m_field->def);
                //f->setAutoIncrement(m_field->flags & AUTO_INCREMENT_FLAG);
                f->setPrimaryKey(primaryKey(toid, i));
                f->setUniqueKey(uniqueKey(toid, i));
                dbStruct.append(f);
            }
        }

    }
    catch(const exception &e)
    {
        kdDebug() << "pqxxSqlDB::structure:exception - " << e.what() << endl;
    }
    delete tmpres;
    delete tran;
    return dbStruct;
}

//=========================================================================
//Returns the native datatype given a kexi data type
QString pqxxSqlDB::nativeDataType(const KexiDBField::ColumnType& t) const
{
    kdDebug() << "pqxxSqlDB::nativeDataType()" << endl;
    switch(t)
    {
    case KexiDBField::SQLLongVarchar:
        return "TEXT";
    case KexiDBField::SQLVarchar:
        return "VARCHAR";
    case KexiDBField::SQLInteger:
        return "INTEGER";
    case KexiDBField::SQLSmallInt:
        return "SMALLINT";
    case KexiDBField::SQLTinyInt:
        return "TINYINT";
    case KexiDBField::SQLNumeric:
        return "NUMERIC";
    case KexiDBField::SQLDouble:
        return "DOUBLE";
    case KexiDBField::SQLBigInt:
        return "BIGINT";
    case KexiDBField::SQLDecimal:
        return "DECIMAL";
    case KexiDBField::SQLFloat:
        return "FLOAT";
    case KexiDBField::SQLBinary:
        return "BLOB";
    case KexiDBField::SQLLongVarBinary:
        return "LONGBLOB";
    case KexiDBField::SQLVarBinary:
        return "BLOB";
    case KexiDBField::SQLDate:
        return "DATE";
    case KexiDBField::SQLTime:
        return "TIME";
    case KexiDBField::SQLTimeStamp:
        return "TIMESTAMP";
    case KexiDBField::SQLBoolean:
        return "BOOL";
    case KexiDBField::SQLInterval:
        return "ENUM";
    case KexiDBField::SQLInvalid:
    case KexiDBField::SQLLastType:
        return QString::null;
    }

    return QString::null;
}

//=========================================================================
//Returns the kexi data type given a postgres data type
KexiDBField::ColumnType pqxxSqlDB::getInternalDataType(int t)
{
    kdDebug() << "pqxxSqlDB::getInternalDataType(): " << t << endl;
    switch(t)
    {
    case UNKNOWNOID:
        return KexiDBField::SQLInvalid;
    case BOOLOID:
        return KexiDBField::SQLBoolean;
    case INT2OID:
        return KexiDBField::SQLSmallInt;
    case INT4OID:
        return KexiDBField::SQLInteger;
    case INT8OID:
        return KexiDBField::SQLBigInt;
    case FLOAT4OID:
        return KexiDBField::SQLFloat;
    case FLOAT8OID:
        return KexiDBField::SQLDouble;
    case NUMERICOID:
        return KexiDBField::SQLNumeric;
        //case :
        //	return KexiDBField::SQLDecimal;
    case DATEOID:
        return KexiDBField::SQLDate;
    case TIMEOID:
        return KexiDBField::SQLTime;
    case TIMESTAMPOID:
        return KexiDBField::SQLTimeStamp;
    case BYTEAOID:
        return KexiDBField::SQLVarBinary;
    case VARCHAROID:
        return KexiDBField::SQLVarchar;
    case TEXTOID:
        return KexiDBField::SQLLongVarchar;
        //TODO: Binary Types
    }
    return KexiDBField::SQLInvalid;
}

//=========================================================================
//Does something along the lines of change a primary key
bool pqxxSqlDB::changeKeys(const KexiDBField& field, int index, KexiDBTableStruct fields)
{
    kdDebug() << "pqxxSqlDB::changeKeys()" << endl;
    bool noPrimary = false;
    QString qstr = "ALTER TABLE " + field.table();

    if(index >= 0)
    {
        if(field.primary_key() == fields.at(index)->primary_key())
        {
            noPrimary = true;
        }
    }

    if(!noPrimary)
    {
        qstr += " DROP PRIMARY KEY";
        QString fstr;
        int i = 0, j = 0;

        for(KexiDBField* f = fields.first(); f; f = fields.next())
        {
            if((index != i) && (f->primary_key()))
            {
                if(j > 0)
                {
                    fstr += ",";
                }
                else
                {
                    j++;
                }

                fstr += f->name();
            }

            i++;
        }

        if(field.primary_key())
        {
            if(j > 0)
            {
                fstr += ",";
            }

            fstr += field.name();
        }


        if(!fstr.isEmpty())
        {
            qstr += ", ADD PRIMARY KEY(" + fstr + ")";
        }
    }

    if(!noPrimary)
    {
        kdDebug() << "pqxxSqlDB::changeKeys: Query: " << qstr << endl;
        return query(qstr, true);
    }

    return true;
}

//=========================================================================
//Return the name of a table given a result and column number
QString pqxxSqlDB::tableName(oid table_uid) const
{
    kdDebug() << "pqxxSqlDB::tableName()" << endl;
    stringstream statement;
    string tn;

    transaction_base* tran;
    result* tmpres;

    try
    {
        statement << "SELECT relname FROM pg_class WHERE (relfilenode=";
        statement << table_uid;
        statement << ")";

        kdDebug() << statement.str() << endl;

        tran = new nontransaction(*_dbConn, "get_table_name");
        tmpres = new result(tran->exec(statement.str()));
        tran->commit();

        if (tmpres->size() > 0)
        {
            tn = tmpres->at(0).at(0).c_str();
        }
        else
        {
            tn = "Unknown";
        }
    }
    catch(const exception &e)
    {
        kdDebug() << "pqxxSqlDB::tableName:exception - " << e.what() << endl;
        kdDebug() << "pqxxSqlDB::tableName:failed statement - " << statement.str() << endl;
        tn = "Unknown";
    }
    if (tmpres)
        delete tmpres;
    if (tran)
        delete tran;

    kdDebug() << "Table name: " << tn << endl;
    return tn;
}

//=========================================================================
//Return whether or not the curent field is a primary key
//FIXME:  Calling this function many times is probably quite ineficient
//Should really find a better way of doing this, eg caching table indexes
bool pqxxSqlDB::primaryKey(oid table_uid, int col) const
{
    kdDebug() << "pqxxSqlDB::primaryKey()" << endl;
    stringstream statement;
    bool pkey;
    int keyf;

    transaction_base* tran;
    result* tmpres;

    try
    {
        statement << "SELECT indkey FROM pg_index WHERE ((indisprimary = true) AND (indrelid = ";
        statement << table_uid;
        statement << "))";

        tran = new nontransaction(*_dbConn, "find_pkey");
        tmpres = new result(tran->exec(statement.str()));

        tran->commit();
        if (tmpres->size() > 0)
        {
            //We have a key field for this table, lets check if its this column
            tmpres->at(0).at(0).to(keyf);
            if (keyf-1 == col) //-1 because pg counts from 1 and we count from 0
            {
                pkey = true;
            }
            else
            {
                pkey = false;
            }
        }
        else
        {
            pkey = false;
        }
    }
    catch(const exception &e)
    {
        kdDebug() << "pqxxSqlDB::primaryKey:exception - " << e.what() << endl;
        kdDebug() << "pqxxSqlDB::primaryKey:failed statement - " << statement.str() << endl;
        pkey = false;
    }
    if (tmpres)
        delete tmpres;
    if (tran)
        delete tran;

    kdDebug() <<"Primary key: " << pkey << endl;
    return pkey;
}

//=========================================================================
//Return whether or not the curent field is a unique key
//FIXME:  Calling this function many times is probably quite ineficient
//Should really find a better way of doing this, eg caching table indexes
bool pqxxSqlDB::uniqueKey(oid table_uid, int col) const
{
    kdDebug() << "pqxxSqlDB::uniqueKey()" << endl;
    stringstream statement;
    bool ukey;
    int uf;

    transaction_base* tran;
    result* tmpres;


    try
    {
        statement << "SELECT indkey FROM pg_index WHERE ((indisunique = true) AND (indrelid = ";
        statement << table_uid;
        statement << "))";

        tran = new nontransaction(*_dbConn, "find_pkey");
        tmpres = new result(tran->exec(statement.str()));
        tran->commit();

        if (tmpres->size() > 0)
        {
            //We have a key field for this table, lets check if its this column
            tmpres->at(0).at(0).to(uf);
            if (uf-1 == col) //-1 because pg counts from 1 and we count from 0
            {
                kdDebug() << "Found unique key!" << endl;
                ukey = true;
            }
            else
            {
                ukey = false;
            }
        }
        else
        {
            ukey = false;
        }
    }
    catch(const exception &e)
    {
        kdDebug() << "pqxxSqlDB::uniqueKey:exception - " << e.what() << endl;
        kdDebug() << "pqxxSqlDB::uniqueKey:failed statement - " << statement.str() << endl;
        ukey = false;
    }
    if (tmpres)
        delete tmpres;
    if (tran)
        delete tran;

    kdDebug() << "Unique key: " << ukey << endl;
    return ukey;
}

//=========================================================================
//Return whether or not the curent field is notnull
//FIXME:  Calling this function many times is probably quite ineficient
//Should really find a better way of doing this, eg caching table attributes
bool pqxxSqlDB::notNull(oid table_uid, int col) const
{
    kdDebug() << "pqxxSqlDB::notNull()" << endl;
    stringstream statement;
    bool nn;

    transaction_base* tran;
    result* tmpres;

    try
    {
        statement << "SELECT attnotnull FROM pg_attribute WHERE ((attnotnull = true) AND (attrelid = ";
        statement << table_uid;
        statement << ") AND (attnum = " << col + 1 << "))";

        tran = new nontransaction(*_dbConn, "find_attnotnull");
        tmpres = new result(tran->exec(statement.str()));

        tran->commit();
        if (tmpres->size() > 0)
        {
            //The field cant be null
            kdDebug() << "Field is not null!" << endl;
            nn = true;
        }
        else
        {
            nn = false;
        }
    }
    catch(const exception &e)
    {
        kdDebug() << "pqxxSqlDB::notNull:exception - " << e.what() << endl;
        kdDebug() << "pqxxSqlDB::notNull:failed statement - " << statement.str() << endl;
        nn = false;
    }
    if (tmpres)
        delete tmpres;
    if (tran)
        delete tran;

    kdDebug() << "Not null: " << nn << endl;
    return nn;
}

QString pqxxSqlDB::escapeName(const QString &tn) const
{
    QString en;

    en = "\"" + tn + "\"";
    return en;
}

KexiDBTable* pqxxSqlDB::createTableDef(const QString &name)
{
    kdDebug() << "pqxxSqlDB::createTableDef()" << endl;
    if(!_dbConn->is_open())
        return 0;

    query("select * from " + escapeName(name) + " limit 0");

    if(!_dbResult)
        return 0;

    KexiDBTable *t=new KexiDBTable(name);

    //KexiDBField *f = 0;
    for (int i = 0; i < _dbResult->Columns(); i++)
    {
        //KexiDBField* f = new KexiDBField(tableName(_dbRresult, i));
        //Should maybe use 'name' for speed purposes, though above method is more correct
        KexiDBField* f = new KexiDBField(name);
        f->setName(_dbResult->column_name(i));
        t->addField(*f);
        //should we support other unique keys here too ?
        //if (f->primary_key()) t->addPrimaryKey(f->name());
    }

    return t;
}

QString pqxxSqlDB::escapeValue(const QString &v, KexiDBField::ColumnType t) const
{
    kdDebug() << "pqxxSqlDB::escapeValue() - type:" << t << endl;

    switch(t)
    {
    case KexiDBField::SQLLongVarchar:
        return escape(v);
    case KexiDBField::SQLVarchar:
        return escape(v);
    case KexiDBField::SQLInteger:
    case KexiDBField::SQLSmallInt:
    case KexiDBField::SQLTinyInt:
    case KexiDBField::SQLNumeric:
    case KexiDBField::SQLDouble:
    case KexiDBField::SQLBigInt:
    case KexiDBField::SQLDecimal:
    case KexiDBField::SQLFloat:
        return v;
    case KexiDBField::SQLBinary:
    case KexiDBField::SQLLongVarBinary:
    case KexiDBField::SQLVarBinary:
    case KexiDBField::SQLDate:
    case KexiDBField::SQLTime:
    case KexiDBField::SQLTimeStamp:
        return escape(v);
    case KexiDBField::SQLBoolean:
        {
            if (v == "0")
                return "false";
            else if (v == "1")
                return "true";
        }
    case KexiDBField::SQLInterval:
        return escape(v);
    case KexiDBField::SQLInvalid:
    case KexiDBField::SQLLastType:
        return QString::null;
    }
}
#include "pqxxsqldb.moc"

