/***************************************************************************
 * pythonkexidbfieldlist.h
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#ifndef KROSS_PYTHONKEXIDBFIELDLIST_H
#define KROSS_PYTHONKEXIDBFIELDLIST_H

#include <Python.h>
#include "../CXX/Objects.hxx"
#include "../CXX/Extensions.hxx"

#include <kexidb/drivermanager.h>
//#include <kexidb/field.h>
#include <kexidb/fieldlist.h>
//#include <kexidb/driver.h>
//#include <kexidb/connection.h>

namespace Kross
{

    class PythonKexiDBFieldListPrivate;

    /**
     * The PythonKexiDBField class is a from Py::Object inherited
     * object to represent the KexiDB::Field class in python.
     */
    class PythonKexiDBFieldList : public Py::PythonExtension<PythonKexiDBFieldList>
    {
        public:
            explicit PythonKexiDBFieldList(KexiDB::FieldList*);
            virtual ~PythonKexiDBFieldList();

            virtual bool accepts(PyObject* pyobj) const;
            static void init_type(void);

            KexiDB::FieldList* getFieldList();

        private:
            PythonKexiDBFieldListPrivate* d;

            Py::Object fieldCount(const Py::Tuple&);
            Py::Object addField(const Py::Tuple&);
            Py::Object insertField(const Py::Tuple&);
            Py::Object removeField(const Py::Tuple&);
            Py::Object field(const Py::Tuple&);
            Py::Object hasField(const Py::Tuple&);
            Py::Object names(const Py::Tuple&);
    };

}

#endif
