/***************************************************************************
 * pythonkexidb.cpp
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

#include "pythonkexidb.h"
#include "../main/pythonutils.h"
#include "pythonkexidbdriver.h"
#include "pythonkexidbconnectiondata.h"
#include "pythonkexidbconnection.h"
#include "pythonkexidbcursor.h"
#include "pythonkexidbfield.h"
#include "pythonkexidbfieldlist.h"
#include "pythonkexidbschema.h"

#include <string>

using namespace Kross;

PythonKexiDB::PythonKexiDB()
    : Py::ExtensionModule<PythonKexiDB>("KexiDB")
{
    PythonKexiDBDriver::init_type();
    PythonKexiDBConnectionData::init_type();
    PythonKexiDBConnection::init_type();
    PythonKexiDBCursor::init_type();
    PythonKexiDBField::init_type();
    PythonKexiDBFieldList::init_type();
    PythonKexiDBIndexSchema::init_type();
    PythonKexiDBTableSchema::init_type();

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

    add_varargs_method("getConnectionData", &PythonKexiDB::getConnectionData,
        "KexiDBConnectionData KexiDB.getConnectionData()\n"
        "Returns a new KexiDBConnectionData object."
    );
    add_varargs_method("getField", &PythonKexiDB::getField,
        "KexiDBField KexiDB.getField()\n"
        "Returns a new KexiDBField object."
    );
    add_varargs_method("getTableSchema", &PythonKexiDB::getTableSchema,
        "KexiDBTableSchema KexiDB.getTableSchema()\n"
        "Returns a new KexiDBTableSchema object."
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
        throw Py::RuntimeError( QString("KexiDB::Object error: " + obj->errorMsg()).latin1() );
}

KexiDB::DriverManager& PythonKexiDB::driverManager()
{
    if(m_drivermanager.error())
        throw Py::RuntimeError( QString("KexiDB::DriverManager error: " + m_drivermanager.errorMsg()).latin1() );
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
    QString drivername = args[0].as_string().c_str();
    QGuardedPtr<KexiDB::Driver> driver = driverManager().driver(drivername); // caching is done by the DriverManager
    if(! driver)
        throw Py::TypeError(QString("KexiDB.driver(drivername) Unsupported driver '" + drivername + "'").latin1());
    if(driver->error())
        throw Py::RuntimeError(QString("KexiDB::Driver error: " + driver->errorMsg()).latin1());
    return Py::asObject( new PythonKexiDBDriver(this, driver) );
}

Py::Object PythonKexiDB::lookupByMime(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    QString mimetype = args[0].as_string().c_str();
    return PythonUtils::toPyObject(driverManager().lookupByMime(mimetype));
}

Py::Object PythonKexiDB::getConnectionData(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    return Py::asObject( new PythonKexiDBConnectionData() );
    //FIXME: does the Python API garbage collect those objects or do we need it???
}

Py::Object PythonKexiDB::getField(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    QString name = args[0].as_string().c_str();
    KexiDB::Field* field = new KexiDB::Field(name, KexiDB::Field::Text);
    return Py::asObject( new PythonKexiDBField(field) );
    /*TODO
    Field(TableSchema *tableSchema);
    Field(QuerySchema *querySchema, BaseExpr* expr = 0);
    Field();
    */
}

Py::Object PythonKexiDB::getTableSchema(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    QString name = args[0].as_string().c_str();
    KexiDB::TableSchema* tableschema = new KexiDB::TableSchema(name);
    return Py::asObject( new PythonKexiDBTableSchema(tableschema) );
}


