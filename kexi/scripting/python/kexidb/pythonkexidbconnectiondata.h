/***************************************************************************
 * pythonkexidbconnectiondata.h
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

