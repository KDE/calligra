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

            /**
             * Converts a QString to a Py::String. If
             * the QString isNull() then Py::None() will
             * be returned.
             *
             * \param s The QString to convert.
             * \return The to a Py::String converted QString.
             */
            static Py::Object toString(const QString& s);

            /**
             * Converts a QStringList to a Py::List.
             *
             * \param list The QStringList to convert.
             * \return The to a Py::List converted QStringList.
             */
            static Py::List toObject(QStringList list);

            /**
             * Converts a QMap to a Py::Dict.
             *
             * \param map The QMap to convert.
             * \return The to a Py::Dict converted QMap.
             */
            static Py::Dict toObject(QMap<QString, QVariant> map);

            /**
             * Converts a QValueList to a Py::List.
             *
             * \param list The QValueList to convert.
             * \return The to a Py::List converted QValueList.
             */
            static Py::List toObject(QValueList<QVariant> list);

            /**
             * Converts a QVariant to a Py::Object.
             *
             * \param variant The QVariant to convert.
             * \return The to a Py::Object converted QVariant.
             */
            static Py::Object toObject(QVariant variant);

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
