/***************************************************************************
 * pythonkexidbdriver.cpp
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#include "pythonkexidbdriver.h"
//#include "pythonkexidb.h"
#include "../main/pythonutils.h"
#include "pythonkexidbconnection.h"
#include "pythonkexidbconnectiondata.h"

using namespace Kross;

namespace Kross
{
    class PythonKexiDBDriverPrivate
    {
        public:
            PythonKexiDB* kexidb;
            QGuardedPtr<KexiDB::Driver> driver;
    };
}

PythonKexiDBDriver::PythonKexiDBDriver(PythonKexiDB* kexidb, KexiDB::Driver* driver)
{
    d = new PythonKexiDBDriverPrivate();
    d->kexidb = kexidb;
    d->driver = driver;
}

PythonKexiDBDriver::~PythonKexiDBDriver()
{
    delete d;
}

void PythonKexiDBDriver::init_type(void)
{
    behaviors().name("KexiDBDriver");
    behaviors().doc(
        "The KexiDBDriver object provides access to the "
        "KexiDB::Driver class."
    );
    //behaviors().supportGetattr();
    //behaviors().supportSetattr();

    add_varargs_method(
        "createConnection",
        &PythonKexiDBDriver::createConnection,
        "KexiDBConnection KexiDBDriver.createConnection(connectiondata)\n"
        "Create a new KexiDBConnection object. "
    );
    add_varargs_method(
        "connectionList",
        &PythonKexiDBDriver::connectionList,
        "list KexiDBDriver.connectionList()\n"
        "Returns a list of connectionnames. Use getConnection(connectionname)"
        "to get a KexiDBConnection object to work with. "
    );
}

bool PythonKexiDBDriver::accepts(PyObject* pyobj) const
{
    return pyobj && Py::PythonExtension<PythonKexiDBDriver>::check(pyobj);
}

Py::Object PythonKexiDBDriver::createConnection(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    if(! PythonKexiDBConnectionData::check(args[0]))
        throw Py::TypeError("KexiDBDriver.createConnection(KexiDBConnectionData) expects a KexiDBConnectionData object as single parameter.");

    Py::ExtensionObject<PythonKexiDBConnectionData> obj(args[0]);
    PythonKexiDBConnectionData* connectiondata = obj.extensionObject();
    if(! connectiondata)
        throw Py::RuntimeError("KexiDBDriver.createConnection(KexiDBConnectionData) Failed to determinate the defined KexiDBConnectionData object.");

    QGuardedPtr<KexiDB::Connection> connection = d->driver->createConnection(*connectiondata->getConnectionData());
    if(! connection)
        throw Py::RuntimeError("KexiDBDriver.createConnection(connectiondict) Failed to create connection.");
    if(connection->error())
        throw Py::RuntimeError(QString("KexiDBDriver.createConnection(connectiondict) KexiDB::Connection error: " + connection->errorMsg()).latin1());

    return Py::asObject( new PythonKexiDBConnection(this, connectiondata, connection) );
}

Py::Object PythonKexiDBDriver::connectionList(const Py::Tuple& /*args*/)
{
    /*TODO
    if(args.size() != 0)
        throw Py::TypeError("KexiDBDriver.connectionList() expects 0 parameters.");
    if(! d->driver)
        throw Py::RuntimeError("KexiDBDriver.connectionList() KexiDB::Driver not initialized.");

    Py::List* list = new Py::List();
    QPtrList<KexiDB::Connection> connectionlist = d->driver->connectionsList();
    KexiDB::Connection* connection;
    for(connection = connectionlist.first(); connection; connection = connectionlist.next()) {
        //kdDebug() << "PythonKexiDBDriver::connectionList() ITEM currentDatabase()=" << connection->currentDatabase() << " tableNames()=" << connection->tableNames(true) << endl;
        list->append( Py::String(connection->name()) );
    }
    return *list;
    */
}

