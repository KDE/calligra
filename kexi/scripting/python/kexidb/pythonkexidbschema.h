/***************************************************************************
 * pythonkexidbschema.h
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#ifndef KROSS_PYTHONKEXIDBSCHEMA_H
#define KROSS_PYTHONKEXIDBSCHEMA_H

#include <Python.h>
#include "../CXX/Objects.hxx"
#include "../CXX/Extensions.hxx"

#include <kexidb/drivermanager.h>
//#include <kexidb/cursor.h>
#include <kexidb/indexschema.h>
#include <kexidb/tableschema.h>

#include "pythonkexidbfieldlist.h"

namespace Kross
{

    class PythonKexiDBSchemaPrivate;

    class PythonKexiDBSchema : public Py::PythonExtension<PythonKexiDBSchema>
    {
        public:
            PythonKexiDBSchema(KexiDB::SchemaData* schema, KexiDB::FieldList* fieldlist);
            virtual ~PythonKexiDBSchema();

            virtual bool accepts(PyObject* pyobj) const;

            virtual Py::Object getattr(const char*);
            virtual int setattr(const char*, const Py::Object&);

            KexiDB::SchemaData* getSchema();

        private:
            PythonKexiDBSchemaPrivate* d;
    };

    class PythonKexiDBIndexSchema : public PythonKexiDBSchema
    {
        public:
            PythonKexiDBIndexSchema(KexiDB::IndexSchema* indexschema);
            virtual ~PythonKexiDBIndexSchema();

            virtual bool accepts(PyObject* pyobj) const;
            static void init_type(void);
    };

    class PythonKexiDBTableSchema : public PythonKexiDBSchema
    {
        public:
            PythonKexiDBTableSchema(KexiDB::TableSchema* tableschema);
            virtual ~PythonKexiDBTableSchema();

            virtual bool accepts(PyObject* pyobj) const;
            static void init_type(void);
    };

}

#endif

