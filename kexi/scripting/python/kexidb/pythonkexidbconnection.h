/***************************************************************************
 * pythonkexidbconnection.h
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

#ifndef KROSS_PYTHONKEXIDBCONNECTION_H
#define KROSS_PYTHONKEXIDBCONNECTION_H

#include <Python.h>
#include "../CXX/Objects.hxx"
#include "../CXX/Extensions.hxx"

//#include <iostream>
#include <kexidb/drivermanager.h>
#include <kexidb/cursor.h>

namespace Kross
{

    class PythonKexiDBDriver;
    class PythonKexiDBConnectionData;
    class PythonKexiDBConnectionPrivate;

    /**
     * The PythonKexiDBConnection class is a from Py::Object inherited
     * object to represent the KexiDB::Connection class in python.
     */
    class PythonKexiDBConnection : public Py::PythonExtension<PythonKexiDBConnection>
    {
        public:
            PythonKexiDBConnection(PythonKexiDBDriver*, PythonKexiDBConnectionData*, KexiDB::Connection*);
            virtual ~PythonKexiDBConnection();

            virtual bool accepts(PyObject* pyobj) const;
            static void init_type(void);

        private:
            PythonKexiDBConnectionPrivate* d;

            Py::Object data(const Py::Tuple&);
            Py::Object driver(const Py::Tuple&);

            Py::Object connect(const Py::Tuple&);
            Py::Object isConnected(const Py::Tuple&);
            Py::Object disconnect(const Py::Tuple&);

            Py::Object isDatabase(const Py::Tuple&);
            Py::Object currentDatabase(const Py::Tuple&);
            Py::Object databaseNames(const Py::Tuple&);
            Py::Object isDatabaseUsed(const Py::Tuple&);
            Py::Object useDatabase(const Py::Tuple&);
            Py::Object closeDatabase(const Py::Tuple&);

            Py::Object tableNames(const Py::Tuple&);

            Py::Object executeQuery(const Py::Tuple&);
            Py::Object querySingleString(const Py::Tuple&);
            Py::Object queryStringList(const Py::Tuple&);
            Py::Object querySingleRecord(const Py::Tuple&);

            Py::Object executeSQL(const Py::Tuple&);

            Py::Object insertRecord(const Py::Tuple&);
            Py::Object createDatabase(const Py::Tuple&);
            Py::Object dropDatabase(const Py::Tuple&);

            Py::Object createTable(const Py::Tuple&);
            Py::Object dropTable(const Py::Tuple&);
            Py::Object alterTable(const Py::Tuple&);
            Py::Object alterTableName(const Py::Tuple&);

            Py::Object tableSchema(const Py::Tuple&);
            Py::Object isEmptyTable(const Py::Tuple&);
    };

}

#endif

