/***************************************************************************
 * pythonkexidbdriver.h
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#ifndef KROSS_PYTHONKEXIDBDRIVER_H
#define KROSS_PYTHONKEXIDBDRIVER_H

#include <Python.h>

#include "../CXX/Objects.hxx"
#include "../CXX/Extensions.hxx"

#include <kdebug.h>
//#include <qguardedptr.h>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/object.h>

namespace Kross
{

    class PythonKexiDB;
    class PythonKexiDBDriverPrivate;

    /**
     * The PythonKexiDBDriver class is a from Py::Object inherited
     * object to represent the KexiDB::Driver class in python.
     */
    class PythonKexiDBDriver : public Py::PythonExtension<PythonKexiDBDriver>
    {
        public:

            /**
             * Constructor.
             */
            PythonKexiDBDriver(PythonKexiDB*, KexiDB::Driver*);

            /**
             * Destructor.
             */
            virtual ~PythonKexiDBDriver();

            /**
             * From Py::Object Overloaded method to validate if
             * the PyObject could be used within this context.
             *
             * \param pyobj The PyObject to check.
             * \return true if the PyObject is valid else false.
             */
            virtual bool accepts(PyObject* pyobj) const;

            /**
             * Called from PythonKexiDB::PythonKexiDB() to ensure
             * that this object initializes itself.
             */
            static void init_type(void);

        private:
            PythonKexiDBDriverPrivate* d;

            Py::Object createConnection(const Py::Tuple&);
            Py::Object connectionList(const Py::Tuple&);
    };

}

#endif
