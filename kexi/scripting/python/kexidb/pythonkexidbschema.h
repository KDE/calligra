/***************************************************************************
 * pythonkexidbschema.h
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

