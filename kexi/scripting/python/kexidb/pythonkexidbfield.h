/***************************************************************************
 * pythonkexidbfield.h
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#ifndef KROSS_PYTHONKEXIDBFIELD_H
#define KROSS_PYTHONKEXIDBFIELD_H

#include <Python.h>

#include "../CXX/Objects.hxx"
#include "../CXX/Extensions.hxx"

#include <kdebug.h>
//#include <qguardedptr.h>

//#include <kexidb/driver.h>
//#include <kexidb/connection.h>

namespace Kross
{

    class PythonKexiDBFieldPrivate;

    /**
     * The PythonKexiDBField class is a from Py::Object inherited
     * object to represent the KexiDB::Field class in python.
     */
    class PythonKexiDBField : public Py::PythonExtension<PythonKexiDBField>
    {
        public:
            PythonKexiDBField();
            virtual ~PythonKexiDBField();

            virtual bool accepts(PyObject* pyobj) const;
            static void init_type(void);

            virtual Py::Object getattr(const char*);
            virtual int setattr(const char*, const Py::Object&);

        private:
            PythonKexiDBFieldPrivate* d;
    };

}

#endif
