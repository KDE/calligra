/***************************************************************************
 * pythonkexidbconnectiondata.h
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#ifndef KROSS_PYTHONKEXIDBCONNECTIONDATA_H
#define KROSS_PYTHONKEXIDBCONNECTIONDATA_H

#include <Python.h>

#include <iostream>

#include "../CXX/Objects.hxx"
#include "../CXX/Extensions.hxx"

#include <kexidb/drivermanager.h>
#include <kexidb/connectiondata.h>

namespace Kross
{

    /**
     * The PythonKexiDBConnectionData class is a from Py::Object
     * inherited object to represent the KexiDB::ConnectionData
     * class in python.
     */
    class PythonKexiDBConnectionData : public Py::PythonExtension<PythonKexiDBConnectionData>
    {
        public:
            PythonKexiDBConnectionData();
            virtual ~PythonKexiDBConnectionData();

            virtual bool accepts(PyObject*) const;
            static void init_type(void);

            virtual Py::Object getattr(const char*);
            virtual int setattr(const char*, const Py::Object&);

            KexiDB::ConnectionData* getConnectionData();

        private:
            KexiDB::ConnectionData* m_connectiondata;
    };
}

#endif

