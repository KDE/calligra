/***************************************************************************
 * pythonkexidb.cpp
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#include "pythonkexidb.h"
#include "../pythonutils.h"
#include "pythonkexidbdriver.h"
#include "pythonkexidbconnectiondata.h"
#include "pythonkexidbconnection.h"
#include "pythonkexidbcursor.h"
#include "pythonkexidbfield.h"

using namespace Kross;

PythonKexiDB::PythonKexiDB()
    : Py::ExtensionModule<PythonKexiDB>("KexiDB")
{
    PythonKexiDBDriver::init_type();
    PythonKexiDBConnectionData::init_type();
    PythonKexiDBConnection::init_type();
    PythonKexiDBCursor::init_type();
    PythonKexiDBField::init_type();

    add_varargs_method("driverNames", &PythonKexiDB::driverNames,
        "list KexiDB.driverNames()\n"
        "Returns a list of all avaible drivernames. "
        "Iterate through the list and use driver(name) "
        "to handle the KexiDBDriver objects."
    );
    add_varargs_method("driver", &PythonKexiDB::driver,
        "KexiDBDriver KexiDB.driver(drivername)\n"
        "Returns the to name matching KexiDBDriver object."
    );
    add_varargs_method("lookupByMime", &PythonKexiDB::lookupByMime,
        "drivername KexiDB.lookupByMime(mimetype)\n"
        "Looks up a drivers list by MIME type of database file. "
        "Only file-based database drivers are checked. "
        "The lookup is case insensitive."
    );

    add_varargs_method("createConnectionData", &PythonKexiDB::createConnectionData,
        "KexiDBConnectionData KexiDB.createConnectionData()\n"
        "Returns a new KexiDBConnectionData object."
    );

    initialize(
        "The KexiDB python module provides a wrapper for the "
        "Kexi::KexiDB library and allows using the functionality "
        "from within python. "
    );

    //setAttr("value1", Py::String("This is value #1"));
}

PythonKexiDB::~PythonKexiDB()
{
}

void PythonKexiDB::checkObject(KexiDB::Object* obj)
{
    if(! obj)
        throw Py::RuntimeError("KexiDB::Object is NULL.");
    if(obj->error())
        throw Py::RuntimeError("KexiDB::Object error: " + obj->errorMsg());
}

KexiDB::DriverManager& PythonKexiDB::driverManager()
{
    if(m_drivermanager.error())
        throw Py::RuntimeError("KexiDB::DriverManager error: " + m_drivermanager.errorMsg());
    return m_drivermanager;
}

Py::Object PythonKexiDB::driverNames(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    return PythonUtils::toPyObject( driverManager().driverNames() );
}

Py::Object PythonKexiDB::driver(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    std::string drivername = args[0].as_string();
    QGuardedPtr<KexiDB::Driver> driver = driverManager().driver(drivername); // caching is done by the DriverManager
    if(! driver)
        throw Py::TypeError("KexiDB.driver(drivername) Unsupported driver '" + drivername + "'");
    if(driver->error())
        throw Py::RuntimeError("KexiDB::Driver error: " + driver->errorMsg());
    return Py::asObject( new PythonKexiDBDriver(this, driver) );
}

Py::Object PythonKexiDB::lookupByMime(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    std::string mimetype = args[0].as_string();
    return PythonUtils::toPyObject(driverManager().lookupByMime(mimetype));
}

Py::Object PythonKexiDB::createConnectionData(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    return Py::asObject( new PythonKexiDBConnectionData() );
    //FIXME: does the Python API garbage collect those objects or do we need it???
}

