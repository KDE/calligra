/***************************************************************************
 * pythonkexidbconnection.cpp
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#include "pythonkexidbconnection.h"
#include "pythonkexidbconnectiondata.h"
#include "pythonkexidb.h"
#include "../main/pythonutils.h"
#include "pythonkexidbdriver.h"
#include "pythonkexidbcursor.h"

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
        "list KexiDBConnection.tableNames()\n"
    );
    add_varargs_method("executeQuery", &PythonKexiDBConnection::executeQuery,
        "KexiDBCursor KexiDBConnection.executeQuery(querystatement)\n"
    );

    add_varargs_method("insertRecord", &PythonKexiDBConnection::insertRecord,
        "boolean KexiDBConnection.insertRecord(KexiDBFieldList)\n"
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
    PythonUtils::checkArgs(args, 0, 0);
    PythonKexiDB::checkObject(d->connection);
    return PythonUtils::toPyObject( d->connection->tableNames() );
}

Py::Object PythonKexiDBConnection::executeQuery(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    PythonKexiDB::checkObject(d->connection);
    QString query = args[0].as_string().c_str();

    //TODO: move this check to KexiDB::Connection?!
    if(d->connection->currentDatabase().isNull())
        throw Py::RuntimeError("KexiDBConnection.executeQuery(sqlstatement) No database selected, please use connection.useDatabase(databasename) before.");

    KexiDB::Cursor* cursor = d->connection->executeQuery(query);
    //KexiDB::Cursor* cursor = d->connection->executeQuery(query, KexiDB::Cursor::Buffered); //CRASHES!

    if(! cursor)
        throw Py::RuntimeError("KexiDBConnection.executeQuery(sqlstatement) executeQuery() returned with errors.");
    if(cursor->error())
        throw Py::TypeError(QString("KexiDBConnection.executeQuery(sqlstatement) executeQuery() error: " + cursor->errorMsg()).latin1());

    return Py::asObject( new PythonKexiDBCursor(this, cursor) );
}

Py::Object PythonKexiDBConnection::insertRecord(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 2, 2);

    //TODO
    //bool insertRecord(FieldList& fields, QValueList<QVariant>& values);

    return Py::Int(1); // boolean
}

