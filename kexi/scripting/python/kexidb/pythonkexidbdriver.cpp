/***************************************************************************
 * pythonkexidbdriver.cpp
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

#include "pythonkexidbdriver.h"
//#include "pythonkexidb.h"
#include "../main/pythonutils.h"
#include "pythonkexidbconnection.h"
#include "pythonkexidbconnectiondata.h"
#include "pythonkexidbfield.h"

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

    //setAttr("isFileDriver", Py::Int( driver->isFileDriver() ));
    //setAttr("versionMajor", Py::Int( driver->versionMajor() ));
    //setAttr("versionMinor", Py::Int( driver->versionMinor() ));
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
        "getConnection",
        &PythonKexiDBDriver::getConnection,
        "KexiDBConnection KexiDBDriver.getConnection(connectiondata)\n"
        "Create a new KexiDBConnection object. "
    );
    add_varargs_method(
        "connectionList",
        &PythonKexiDBDriver::connectionList,
        "list KexiDBDriver.connectionList()\n"
        "Returns a list of connectionnames. Use getConnection(connectionname)"
        "to get a KexiDBConnection object to work with. "
    );
    add_varargs_method(
        "escapeString",
        &PythonKexiDBDriver::escapeString,
        "string KexiDBDriver.escapeString(string)\n"
        "Returns a Driver-specific escaped SQL string. "
    );
    add_varargs_method(
        "valueToSQL",
        &PythonKexiDBDriver::valueToSQL,
        "string KexiDBDriver.valueToSQL(KexiDBField, value)\n"
        "Escapes and converts a value to the string representation "
        "required by SQL commands. "
    );
}

bool PythonKexiDBDriver::accepts(PyObject* pyobj) const
{
    return pyobj && Py::PythonExtension<PythonKexiDBDriver>::check(pyobj);
}

Py::Object PythonKexiDBDriver::getConnection(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    if(! PythonKexiDBConnectionData::check(args[0]))
        throw Py::TypeError("KexiDBDriver.getConnection(KexiDBConnectionData) expects a KexiDBConnectionData object as single parameter.");

    Py::ExtensionObject<PythonKexiDBConnectionData> obj(args[0]);
    PythonKexiDBConnectionData* connectiondata = obj.extensionObject();
    if(! connectiondata)
        throw Py::RuntimeError("KexiDBDriver.getConnection(KexiDBConnectionData) Failed to determinate the defined KexiDBConnectionData object.");

    QGuardedPtr<KexiDB::Connection> connection = d->driver->createConnection(*connectiondata->getConnectionData());
    if(! connection)
        throw Py::RuntimeError("KexiDBDriver.getConnection(connectiondict) Failed to create connection.");
    if(connection->error())
        throw Py::RuntimeError(QString("KexiDBDriver.getConnection(connectiondict) KexiDB::Connection error: " + connection->errorMsg()).latin1());

    return Py::asObject( new PythonKexiDBConnection(this, connectiondata, connection) );
}

Py::Object PythonKexiDBDriver::connectionList(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    Py::List* list = new Py::List();
    QPtrList<KexiDB::Connection> connectionlist = d->driver->connectionsList();
    KexiDB::Connection* connection;
    for(connection = connectionlist.first(); connection; connection = connectionlist.next())
        list->append( Py::String(connection->name()) );
    return *list;
}

Py::Object PythonKexiDBDriver::escapeString(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    if(! args[0].isString())
        throw Py::TypeError("string KexiDBDriver.escapeString(string) Invalid argument. String expected.");
    return PythonUtils::toPyObject( d->driver->escapeString(QString(args[0].as_string().c_str())) );
}

Py::Object PythonKexiDBDriver::valueToSQL(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 2, 2);
    Py::ExtensionObject<PythonKexiDBField> obj(args[0]);
    PythonKexiDBField* field = obj.extensionObject();
    if(! field)
        throw Py::TypeError("string KexiDBDriver.valueToSQL(KexiDBField, value) Invalid argument.");
    return PythonUtils::toPyObject( d->driver->valueToSQL(field->getField(), PythonUtils::toVariant(args[1])) );
}

