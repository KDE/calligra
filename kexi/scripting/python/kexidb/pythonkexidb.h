/***************************************************************************
 * pythonkexidb.h
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#ifndef KROSS_PYTHONKEXIDB_H
#define KROSS_PYTHONKEXIDB_H

#include <Python.h>

#include "../CXX/Objects.hxx"
#include "../CXX/Extensions.hxx"

#include <qstring.h>
#include <qguardedptr.h>
//#include <qmap.h>
//#include <kdebug.h>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>

namespace Kross
{

    /**
     * The PythonKexiDB class represents the KexiDB python
     * module and manages all objects to access the
     * KexiDB library.
     */
    class PythonKexiDB : public Py::ExtensionModule<PythonKexiDB>
    {
        public:

            /**
             * Constructor.
             */
            PythonKexiDB();

            /**
             * Destructor.
             */
            virtual ~PythonKexiDB();

            /**
             * Check the KexiDB::Object and throw a Py::Exception
             * if NULL or the error() condition is true.
             *
             * \param obj The KexiDB::Object to check.
             */
            static void checkObject(KexiDB::Object* obj);

        private:
            KexiDB::DriverManager m_drivermanager;

            Py::Object driverNames(const Py::Tuple&);
            Py::Object driver(const Py::Tuple&);
            Py::Object lookupByMime(const Py::Tuple&);
            Py::Object createConnectionData(const Py::Tuple&);

            /**
             * Return the shared KexiDB::DriverManager instance
             * or throws Py::Exception if something went wrong.
             *
             * \return The KexiDB::DriverManager.
             */
            KexiDB::DriverManager& driverManager();
    };

}

#endif
