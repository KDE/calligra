/***************************************************************************
 * pythonkexidbconnectiondata.cpp
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#include "pythonkexidbconnectiondata.h"
//#include "pythonkexidb.h"
#include "../main/pythonutils.h"

using namespace Kross;

PythonKexiDBConnectionData::PythonKexiDBConnectionData()
{
    m_connectiondata = new KexiDB::ConnectionData();
}

PythonKexiDBConnectionData::~PythonKexiDBConnectionData()
{
    delete m_connectiondata;
    m_connectiondata = 0;
}

bool PythonKexiDBConnectionData::accepts(PyObject* pyobj) const
{
    return pyobj && Py::PythonExtension<PythonKexiDBConnectionData>::check(pyobj);
}

void PythonKexiDBConnectionData::init_type(void)
{
    behaviors().name("KexiDBConnectionData");
    behaviors().doc(
        "Database specific connection data, e.g. host, port."
    );
    behaviors().supportGetattr();
    behaviors().supportSetattr();
}

Py::Object PythonKexiDBConnectionData::getattr(const char* n)
{
    std::string name(n);

    if(name == "__members__") {
        Py::List members;
        members.append(Py::String("connName"));
        members.append(Py::String("driverName"));
        members.append(Py::String("hostName"));
        members.append(Py::String("port"));
        members.append(Py::String("password"));
        members.append(Py::String("userName"));
        members.append(Py::String("fileName"));
        return members;
    }

    //PythonKexiDB::check_obj(m_connectiondata);
    if(name == "connName")
        return PythonUtils::toPyObject(m_connectiondata->connName);
    if(name == "driverName")
        return PythonUtils::toPyObject(m_connectiondata->driverName);
    if(name == "hostName")
        return PythonUtils::toPyObject(m_connectiondata->hostName);
    if(name == "port")
        return Py::Long((unsigned long)m_connectiondata->port);
    if(name == "password")
        return PythonUtils::toPyObject(m_connectiondata->password);
    if(name == "userName")
        return PythonUtils::toPyObject(m_connectiondata->userName);
    if(name == "fileName")
        return PythonUtils::toPyObject(m_connectiondata->fileName());

    throw Py::AttributeError("Unknown attribute: " + name);
}

int PythonKexiDBConnectionData::setattr(const char* n, const Py::Object& value)
{
    //PythonKexiDB::check_obj(m_connectiondata);
    std::string name(n);

    if(name == "connName")
        m_connectiondata->connName = value.as_string().c_str();
    //else if(name == "driverName") // guess it doesn't make sense to be able to alter the driverName...
    //    m_connectiondata->driverName = value.as_string();
    else if(name == "hostName")
        m_connectiondata->hostName = value.as_string().c_str();
    else if(name == "port") {
        if(! value.isNumeric())
            throw Py::AttributeError("Attribute 'port' needs to be numeric.");
        long port = Py::Long(value); //LONG_MAX=2^31-1 while UINT_MAX=2^32-1
        if(port < 0)
            throw Py::AttributeError("Attribute 'port' out of range.");
        m_connectiondata->port = (uint)port;
    }
    else if(name == "password")
        m_connectiondata->password = value.as_string().c_str();
    else if(name == "userName")
        m_connectiondata->userName = value.as_string().c_str();
    else if(name == "fileName")
        m_connectiondata->setFileName(value.as_string().c_str());
    else
        throw Py::AttributeError("Unknown attribute: " + name);

    return 0;
}

KexiDB::ConnectionData* PythonKexiDBConnectionData::getConnectionData()
{
    return m_connectiondata;
}
