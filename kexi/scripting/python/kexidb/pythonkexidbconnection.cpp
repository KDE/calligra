/***************************************************************************
 * pythonkexidbconnection.cpp
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#include "pythonkexidbconnection.h"
#include "../main/pythonutils.h"
#include "pythonkexidbconnectiondata.h"
#include "pythonkexidb.h"
#include "pythonkexidbdriver.h"
#include "pythonkexidbcursor.h"
#include "pythonkexidbfieldlist.h"
#include "pythonkexidbschema.h"
#include "pythonkexidbtransaction.h"

using namespace Kross;

namespace Kross
{
    class PythonKexiDBConnectionPrivate
    {
        public:
            PythonKexiDBDriver* driver;
            PythonKexiDBConnectionData* connectiondata;
            QGuardedPtr<KexiDB::Connection> connection;
    };
}

PythonKexiDBConnection::PythonKexiDBConnection(PythonKexiDBDriver* driver, PythonKexiDBConnectionData* connectiondata, KexiDB::Connection* connection)
{
    d = new PythonKexiDBConnectionPrivate();
    d->driver = driver;
    d->connectiondata = connectiondata;
    d->connection = connection;
}

PythonKexiDBConnection::~PythonKexiDBConnection()
{
    delete d;
}

void PythonKexiDBConnection::init_type(void)
{
    behaviors().name("KexiDBConnection");
    behaviors().doc(
        "The KexiDBConnection object provides access to the "
        "KexiDB::Connection class. "
    );

    add_varargs_method("data", &PythonKexiDBConnection::data,
        "PythonKexiDBConnectionData KexiDBConnection.data()\n"
    );
    add_varargs_method("driver", &PythonKexiDBConnection::driver,
        "KexiDBDriver KexiDBConnection.driver()\n"
    );

    add_varargs_method("connect", &PythonKexiDBConnection::connect,
        "bool KexiDBConnection.connect()\n"
    );
    add_varargs_method("isConnected", &PythonKexiDBConnection::isConnected,
        "bool KexiDBConnection.isConnected()\n"
    );
    add_varargs_method("disconnect", &PythonKexiDBConnection::disconnect,
        "bool KexiDBConnection.disconnect()\n"
    );

    add_varargs_method("isDatabase", &PythonKexiDBConnection::isDatabase,
        "bool KexiDBConnection.isDatabase(databasename).\n"
    );
    add_varargs_method("currentDatabase", &PythonKexiDBConnection::currentDatabase,
        "string KexiDBConnection.currentDatabase()\n"
    );
    add_varargs_method("databaseNames", &PythonKexiDBConnection::databaseNames,
        "list KexiDBConnection.databaseNames().\n"
    );
    add_varargs_method("isDatabaseUsed", &PythonKexiDBConnection::isDatabaseUsed,
        "bool KexiDBConnection.isDatabaseUsed()\n"
    );
    add_varargs_method("useDatabase", &PythonKexiDBConnection::useDatabase,
        "bool KexiDBConnection.useDatabase(databasename)\n"
    );
    add_varargs_method("closeDatabase", &PythonKexiDBConnection::closeDatabase,
        "bool KexiDBConnection.closeDatabase()\n"
    );
    add_varargs_method("tableNames", &PythonKexiDBConnection::tableNames,
        "list KexiDBConnection.tableNames(also_systables)\n"
        "@param also_systables Optional boolean value if internal kexi "
        "systemtables should be returned too. Default is False.\n"
        "@return list List of tablename-strings.\n"
    );
    add_varargs_method("executeQuery", &PythonKexiDBConnection::executeQuery,
        "KexiDBCursor KexiDBConnection.executeQuery(querystatement or KexiDBTableSchema or KexiDBQuerySchema)\n"
    );
    add_varargs_method("querySingleString", &PythonKexiDBConnection::querySingleString,
        "value KexiDBConnection.querySingleString(sqlstatement, columnnumber)\n"
    );
    add_varargs_method("queryStringList", &PythonKexiDBConnection::queryStringList,
        "valuelist KexiDBConnection.queryStringList(sqlstatement, columnnumber)\n"
    );
    add_varargs_method("querySingleRecord", &PythonKexiDBConnection::querySingleRecord,
        "valuelist KexiDBConnection.querySingleRecord(sqlstatement)\n"
    );
    Py::Object querySingleRecord(const Py::Tuple&);

    add_varargs_method("insertRecord", &PythonKexiDBConnection::insertRecord,
        "boolean KexiDBConnection.insertRecord(KexiDBFieldList)\n"
    );
    add_varargs_method("createDatabase", &PythonKexiDBConnection::createDatabase,
        "boolean KexiDBConnection.createDatabase(databasename)\n"
    );
    add_varargs_method("dropDatabase", &PythonKexiDBConnection::dropDatabase,
        "boolean KexiDBConnection.dropDatabase(databasename)\n"
    );
    add_varargs_method("executeSQL", &PythonKexiDBConnection::executeSQL,
        "boolean KexiDBConnection.executeSQL(sqlstatement)\n"
    );

    add_varargs_method("createTable", &PythonKexiDBConnection::createTable,
        "boolean KexiDBConnection.createTable(KexiDBTableSchema)\n"
    );
    add_varargs_method("dropTable", &PythonKexiDBConnection::dropTable,
        "boolean KexiDBConnection.dropTable(KexiDBTableSchema or tablename)\n"
    );
    add_varargs_method("alterTable", &PythonKexiDBConnection::alterTable,
        "boolean KexiDBConnection.alterTable(KexiDBTableSchema, KexiDBTableSchema)\n"
    );
    add_varargs_method("alterTableName", &PythonKexiDBConnection::alterTableName,
        "boolean KexiDBConnection.alterTableName(KexiDBTableSchema, tablename)\n"
    );

    add_varargs_method("tableSchema", &PythonKexiDBConnection::tableSchema,
        "KexiDBTableSchema KexiDBConnection.tableSchema(tablename)\n"
    );
    add_varargs_method("isEmptyTable", &PythonKexiDBConnection::isEmptyTable,
        "boolean KexiDBConnection.isEmptyTable(KexiDBTableSchema)\n"
    );

    add_varargs_method("autoCommit", &PythonKexiDBConnection::autoCommit,
        "boolean KexiDBConnection.autoCommit()\n"
    );
    add_varargs_method("setAutoCommit", &PythonKexiDBConnection::setAutoCommit,
        "boolean KexiDBConnection.setAutoCommit(boolean)\n"
    );
    add_varargs_method("beginTransaction", &PythonKexiDBConnection::beginTransaction,
        "KexiDBTransaction KexiDBConnection.beginTransaction()\n"
    );
    add_varargs_method("commitTransaction", &PythonKexiDBConnection::commitTransaction,
        "boolean KexiDBConnection.commitTransaction(KexiDBTransaction)\n"
    );
    add_varargs_method("rollbackTransaction", &PythonKexiDBConnection::rollbackTransaction,
        "boolean KexiDBConnection.rollbackTransaction(KexiDBTransaction)\n"
    );
    add_varargs_method("defaultTransaction", &PythonKexiDBConnection::defaultTransaction,
        "KexiDBTransaction KexiDBConnection.defaultTransaction()\n"
    );
    add_varargs_method("setDefaultTransaction", &PythonKexiDBConnection::setDefaultTransaction,
        "None KexiDBConnection.setDefaultTransaction(KexiDBTransaction)\n"
    );
    add_varargs_method("transactions", &PythonKexiDBConnection::transactions,
        "list<KexiDBTraction> KexiDBConnection.transactions()\n"
    );
}

bool PythonKexiDBConnection::accepts(PyObject* pyobj) const
{
    return pyobj && Py::PythonExtension<PythonKexiDBConnection>::check(pyobj);
}

Py::Object PythonKexiDBConnection::data(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    return d->connectiondata ? Py::asObject(d->connectiondata) : Py::None();
}

Py::Object PythonKexiDBConnection::driver(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    return d->driver ? Py::asObject(d->driver) : Py::None();
}

Py::Object PythonKexiDBConnection::connect(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    PythonKexiDB::checkObject(d->connection);
    return Py::Int( d->connection->connect() );
}

Py::Object PythonKexiDBConnection::isConnected(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    PythonKexiDB::checkObject(d->connection);
    return Py::Int( d->connection->isConnected() );
}

Py::Object PythonKexiDBConnection::disconnect(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    PythonKexiDB::checkObject(d->connection);
    return Py::Int( d->connection->disconnect() );
}

Py::Object PythonKexiDBConnection::isDatabase(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    PythonKexiDB::checkObject(d->connection);
    Py::String dbname = args[0];
    return Py::Int( d->connection->databaseExists(dbname.as_string().c_str()) );
}

Py::Object PythonKexiDBConnection::currentDatabase(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    PythonKexiDB::checkObject(d->connection);
    return PythonUtils::toPyObject(d->connection->currentDatabase());
}

Py::Object PythonKexiDBConnection::databaseNames(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    PythonKexiDB::checkObject(d->connection);
    return PythonUtils::toPyObject( d->connection->databaseNames() );
}

Py::Object PythonKexiDBConnection::isDatabaseUsed(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    PythonKexiDB::checkObject(d->connection);
    return Py::Int( d->connection->isDatabaseUsed() );
}

Py::Object PythonKexiDBConnection::useDatabase(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    PythonKexiDB::checkObject(d->connection);
    QString dbname = args[0].as_string().c_str();
    if(! d->connection->databaseExists(dbname))
        throw Py::TypeError(QString("KexiDBConnection.useDatabase(databasename) The database '" + dbname + "' doesn't exists.").latin1());
    return Py::Int( d->connection->useDatabase(dbname) );
}

Py::Object PythonKexiDBConnection::closeDatabase(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    PythonKexiDB::checkObject(d->connection);
    return Py::Int( d->connection->closeDatabase() );
}

Py::Object PythonKexiDBConnection::tableNames(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 1);
    PythonKexiDB::checkObject(d->connection);
    bool alsosystables = (args.size() == 1 && args[0].isTrue());
    return PythonUtils::toPyObject( d->connection->tableNames(alsosystables) );
}

Py::Object PythonKexiDBConnection::executeQuery(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    PythonKexiDB::checkObject(d->connection);

    //TODO: move this check to KexiDB::Connection?!
    if(d->connection->currentDatabase().isNull())
        throw Py::RuntimeError("KexiDBConnection.executeQuery(sqlstatement or KexiDBTableSchema or KexiDBQuerySchema) No database selected, please use connection.useDatabase(databasename) before.");

    KexiDB::Cursor* cursor = 0;
    if(args[0].isString()) {
        cursor = d->connection->executeQuery( args[0].as_string().c_str() );
    }
    else if(PythonKexiDBTableSchema::check(args[0])) {
        Py::ExtensionObject<PythonKexiDBTableSchema> obj(args[0]);
        PythonKexiDBTableSchema* table = obj.extensionObject();
        if(! table)
            throw Py::TypeError("boolean KexiDBConnection.executeQuery(sqlstatement or KexiDBTableSchema or KexiDBQuerySchema) Invalid KexiDBTableSchema argument.");
        cursor = d->connection->executeQuery( *(KexiDB::TableSchema*)table->getSchema() );
    }
    else if(PythonKexiDBQuerySchema::check(args[0])) {
        Py::ExtensionObject<PythonKexiDBQuerySchema> obj(args[0]);
        PythonKexiDBQuerySchema* query = obj.extensionObject();
        if(! query)
            throw Py::TypeError("boolean KexiDBConnection.executeQuery(sqlstatement or KexiDBTableSchema or KexiDBQuerySchema) Invalid KexiDBQuerySchema argument.");
        cursor = d->connection->executeQuery( *(KexiDB::QuerySchema*)query->getSchema() );
    }
    else
        throw Py::TypeError("boolean KexiDBConnection.executeQuery(sqlstatement or KexiDBTableSchema or KexiDBQuerySchema) Invalid argument.");

    if(! cursor)
        throw Py::RuntimeError("KexiDBConnection.executeQuery(sqlstatement) executeQuery() returned with errors.");
    if(cursor->error())
        throw Py::TypeError(QString("KexiDBConnection.executeQuery(sqlstatement) executeQuery() error: " + cursor->errorMsg()).latin1());

    return Py::asObject( new PythonKexiDBCursor(this, cursor) );
}

Py::Object PythonKexiDBConnection::querySingleString(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 2);
    PythonKexiDB::checkObject(d->connection);
    QString sql = args[0].as_string().c_str();
    QString value;
    uint column = 0;
    if(args.size() >= 2) {
        if(! args[1].isNumeric())
            throw Py::TypeError("KexiDBConnection.querySingleString(sqlstatement, columnnumber) columnnumber needs to be numeric.");
        column = (unsigned long)Py::Long(args[1]);
    }
    if(! d->connection->querySingleString(sql, value, column))
        return Py::None();
    return PythonUtils::toPyObject(value);
}

Py::Object PythonKexiDBConnection::queryStringList(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 2);
    PythonKexiDB::checkObject(d->connection);
    QString sql = args[0].as_string().c_str();
    QStringList valuelist;
    uint column = 0;
    if(args.size() >= 2) {
        if(! args[1].isNumeric())
            throw Py::TypeError("KexiDBConnection.querySingleString(sqlstatement, columnnumber) columnnumber needs to be numeric.");
        column = (unsigned long)Py::Long(args[1]);
    }
    if(! d->connection->queryStringList(sql, valuelist, column))
        return Py::None();
    return PythonUtils::toPyObject(valuelist);
}

Py::Object PythonKexiDBConnection::querySingleRecord(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    PythonKexiDB::checkObject(d->connection);
    QString sql = args[0].as_string().c_str();
    QValueVector<QVariant> valuelist;
    if(! d->connection->querySingleRecord(sql, valuelist))
        return Py::None();
    return PythonUtils::toPyObject(valuelist);
}

Py::Object PythonKexiDBConnection::insertRecord(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 2, 2);
    PythonKexiDB::checkObject(d->connection);

    Py::ExtensionObject<PythonKexiDBFieldList> obj(args[0]);
    PythonKexiDBFieldList* fieldlist = obj.extensionObject();
    if(! fieldlist)
        throw Py::TypeError("KexiDBConnection.insertRecord(KexiDBFieldList,valuelist) Failed to determinate the defined KexiDBFieldList object.");

    if(! args[1].isList())
        throw Py::TypeError("KexiDBConnection.insertRecord(KexiDBFieldList,valuelist) Failed to determinate the defined List of values.");
    Py::List valuelist = args[1];

    QValueList<QVariant> vlist = PythonUtils::toVariant(valuelist).toList();
    return Py::Int( d->connection->insertRecord(*fieldlist->getFieldList(), vlist) );
}

Py::Object PythonKexiDBConnection::createDatabase(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    PythonKexiDB::checkObject(d->connection);
    QString dbname = args[0].as_string().c_str();
    if(dbname.isEmpty() || ! args[0].isString())
        throw Py::TypeError("KexiDBConnection.createDatabase(databasename) Invalid databasename string.");
    return Py::Int( d->connection->createDatabase(dbname) );
}

Py::Object PythonKexiDBConnection::dropDatabase(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 1);
    PythonKexiDB::checkObject(d->connection);
    QString dbname = QString::null;
    if(args.size() > 0) {
        if(! args[0].isString())
            throw Py::TypeError("KexiDBConnection.dropDatabase(databasename) Invalid databasename string.");
        dbname = args[0].as_string().c_str();
    }
    return Py::Int( d->connection->dropDatabase(dbname) );
}

Py::Object PythonKexiDBConnection::executeSQL(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    PythonKexiDB::checkObject(d->connection);
    if(! args[0].isString())
        throw Py::TypeError("boolean KexiDBConnection.executeSQL(sqlstatement) Invalid argument. String expected.");
    return Py::Int( d->connection->executeSQL(args[0].as_string().c_str()) );
}

Py::Object PythonKexiDBConnection::createTable(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    PythonKexiDB::checkObject(d->connection);
    Py::ExtensionObject<PythonKexiDBTableSchema> obj(args[0]);
    PythonKexiDBTableSchema* tableschema = obj.extensionObject();
    if(! tableschema)
        throw Py::TypeError("boolean KexiDBConnection.createTable(KexiDBTableSchema) Invalid argument.");
    return Py::Int( d->connection->createTable((KexiDB::TableSchema*)tableschema->getSchema(), true) );
}

Py::Object PythonKexiDBConnection::dropTable(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    PythonKexiDB::checkObject(d->connection);
    if(args[0].isString())
        return Py::Int( d->connection->dropTable(args[0].as_string().c_str()) );
    Py::ExtensionObject<PythonKexiDBTableSchema> obj(args[0]);
    PythonKexiDBTableSchema* tableschema = obj.extensionObject();
    if(! tableschema)
        throw Py::TypeError("boolean KexiDBConnection.dropTable(KexiDBTableSchema or string) Invalid argument.");
    return Py::Int( d->connection->dropTable((KexiDB::TableSchema*)tableschema->getSchema()) );
}

Py::Object PythonKexiDBConnection::alterTable(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 2, 2);
    PythonKexiDB::checkObject(d->connection);

    Py::ExtensionObject<PythonKexiDBTableSchema> obj(args[0]);
    PythonKexiDBTableSchema* tableschema = obj.extensionObject();

    Py::ExtensionObject<PythonKexiDBTableSchema> newobj(args[1]);
    PythonKexiDBTableSchema* newtableschema = newobj.extensionObject();

    if(! tableschema || ! newtableschema)
        throw Py::TypeError("boolean KexiDBConnection.alterTable(KexiDBTableSchema, KexiDBTableSchema) Invalid argument.");

    return Py::Int( d->connection->alterTable(*(KexiDB::TableSchema*)tableschema->getSchema(), *(KexiDB::TableSchema*)newtableschema->getSchema()) );
}

Py::Object PythonKexiDBConnection::alterTableName(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 2, 2);
    PythonKexiDB::checkObject(d->connection);
    Py::ExtensionObject<PythonKexiDBTableSchema> obj(args[0]);
    PythonKexiDBTableSchema* tableschema = obj.extensionObject();
    QString name = args[1].as_string().c_str();
    if(! tableschema)
        throw Py::TypeError("boolean KexiDBConnection.alterTableName(KexiDBTableSchema, tablename) Invalid argument.");
    return Py::Int( d->connection->alterTableName(*(KexiDB::TableSchema*)tableschema->getSchema(), name, true) );
}

Py::Object PythonKexiDBConnection::tableSchema(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    PythonKexiDB::checkObject(d->connection);
    QString name = args[1].as_string().c_str();
    KexiDB::TableSchema* tableschema = d->connection->tableSchema(name);
    if(! tableschema) {
        //throw Py::TypeError("KexiDBTableSchema KexiDBConnection.tableSchema(tablename) Invalid table.");
        return Py::None();
    }
    return Py::asObject(new PythonKexiDBTableSchema(tableschema));
}

Py::Object PythonKexiDBConnection::isEmptyTable(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    PythonKexiDB::checkObject(d->connection);
    Py::ExtensionObject<PythonKexiDBTableSchema> obj(args[0]);
    PythonKexiDBTableSchema* tableschema = obj.extensionObject();
    if(! tableschema)
        throw Py::TypeError("boolean KexiDBConnection.isEmptyTable(KexiDBTableSchema) Invalid argument.");
    bool success;
    bool notempty = d->connection->isEmpty(*(KexiDB::TableSchema*)tableschema->getSchema(), success);
    return Py::Int(! (success && notempty));
}

Py::Object PythonKexiDBConnection::autoCommit(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    PythonKexiDB::checkObject(d->connection);
    return Py::Int( d->connection->autoCommit() );
}

Py::Object PythonKexiDBConnection::setAutoCommit(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    PythonKexiDB::checkObject(d->connection);
    return Py::Int( d->connection->setAutoCommit(args[0].isTrue()) );
}

Py::Object PythonKexiDBConnection::beginTransaction(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    PythonKexiDB::checkObject(d->connection);
    KexiDB::Transaction transaction = d->connection->beginTransaction();
    PythonKexiDBTransaction* t = new PythonKexiDBTransaction(transaction);
    return Py::asObject(t);
}

Py::Object PythonKexiDBConnection::commitTransaction(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 1);
    PythonKexiDB::checkObject(d->connection);
    PythonKexiDBTransaction* transaction = 0;
    if(args.size() > 0) {
        Py::ExtensionObject<PythonKexiDBTransaction> obj(args[0]);
        transaction = obj.extensionObject();
        if(! transaction)
            throw Py::TypeError("boolean KexiDBConnection.commitTransaction(KexiDBTransaction) Invalid argument.");
    }
    return Py::Int( d->connection->commitTransaction(transaction->getTransaction()) );
}

Py::Object PythonKexiDBConnection::rollbackTransaction(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 1);
    PythonKexiDB::checkObject(d->connection);
    PythonKexiDBTransaction* transaction = 0;
    if(args.size() > 0) {
        Py::ExtensionObject<PythonKexiDBTransaction> obj(args[0]);
        transaction = obj.extensionObject();
        if(! transaction)
            throw Py::TypeError("boolean KexiDBConnection.rollbackTransaction(KexiDBTransaction) Invalid argument.");
    }
    return Py::Int( d->connection->rollbackTransaction(transaction->getTransaction()) );
}

Py::Object PythonKexiDBConnection::defaultTransaction(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    PythonKexiDB::checkObject(d->connection);
    return Py::asObject(new PythonKexiDBTransaction(d->connection->defaultTransaction()));
}

Py::Object PythonKexiDBConnection::setDefaultTransaction(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    PythonKexiDB::checkObject(d->connection);
    Py::ExtensionObject<PythonKexiDBTransaction> obj(args[0]);
    PythonKexiDBTransaction* transaction = obj.extensionObject();
    if(! transaction)
        throw Py::TypeError("None KexiDBConnection.setDefaultTransaction(KexiDBTransaction) Invalid argument.");
    d->connection->setDefaultTransaction(transaction->getTransaction());
    return Py::None();
}

Py::Object PythonKexiDBConnection::transactions(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    PythonKexiDB::checkObject(d->connection);
    Py::List transactionlist;
    QValueList<KexiDB::Transaction> list = d->connection->transactions();
    for(QValueList<KexiDB::Transaction>::Iterator it = list.begin(); it != list.end(); ++it)
        transactionlist.append( Py::asObject(new PythonKexiDBTransaction(*it)) );
    return transactionlist;
}

