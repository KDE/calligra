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
#include "../CXX/Config.hxx"
#include "../CXX/Objects.hxx"
#include "../CXX/Extensions.hxx"

#include <kexidb/drivermanager.h>
#include <kexidb/indexschema.h>
#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>

#include "pythonkexidbfieldlist.h"

namespace Kross
{

    class PythonKexiDBSchemaPrivate;

    /**
     * The base schema class to represent the \a KexiDB::SchemaData
     * class in python.
     * We define it as template class cause the underlaying
     * Py::PythonExtension does need it to accept python-methods
     * of inherited classes.
     */
    template<TEMPLATE_TYPENAME T>
    class PythonKexiDBSchema : public Py::PythonExtension<T>
    {
        protected:

            /**
             * Constructor.
             *
             * The constructor is protected cause the class shouldn't
             * be directly instanciated. Use the from this class
             * inherited classes instat.
             * We define the both params to handle the multiple
             * inheritance of from KexiDB::SchemaData and
             * KexiDB::FieldList inherited classes like
             * KexiDB::TableSchema.
             *
             * \param schema The \a KexiDB::SchemaData instance.
             * \param fieldlist The \a KexiDB::FieldList instance.
             */
            PythonKexiDBSchema(KexiDB::SchemaData* schema, KexiDB::FieldList* fieldlist);

        public:

            /**
             * Destructor.
             */
            virtual ~PythonKexiDBSchema();

            /**
             * From Py::Object Overloaded method to validate if
             * the PyObject could be used within this context.
             *
             * \param pyobj The PyObject to check.
             * \return true if the PyObject is valid else false.
             */
            virtual bool accepts(PyObject* pyobj) const;

            /**
             * Called from inherited classes (not from
             * PythonKexiDB::PythonKexiDB() !) to ensure
             * that this object initializes itself.
             */
            static void init_type(void);

            /**
             * Attribute getter handler.
             *
             * \param name The attribute name.
             * \return The Py::Object attribute value on
             *         success else throws a
             *         Py::AttributeError exception.
             */
            virtual Py::Object getattr(const char* name);

            /**
             * Attribute setter handler.
             *
             * \param name The attribute name.
             * \param obj The attribute value.
             * \return 0 on success else throws a
             *         Py::AttributeError exception.
             */
            virtual int setattr(const char* name, const Py::Object& obj);

            /**
             * Return the from \a KexiDB::SchemaData inherited
             * instance (e.g. \a PythonKexiDBTableSchema).
             *
             * \return The SchemaData object.
             */
            virtual KexiDB::SchemaData* getSchema();

        private:
            PythonKexiDBSchemaPrivate* d;
    };

    /**
     * From \a PythonKexiDBSchema inherited class to represent the
     * \a KexiDB::IndexSchema class in python.
     */
    class PythonKexiDBIndexSchema : public PythonKexiDBSchema<PythonKexiDBIndexSchema>
    {
        public:

            /**
             * Constructor.
             *
             * \param indexschema The \a KexiDB::IndexSchema instance.
             */
            explicit PythonKexiDBIndexSchema(KexiDB::IndexSchema* indexschema);

            /**
             * Destructor.
             */
            virtual ~PythonKexiDBIndexSchema();

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
    };

    /**
     * From \a PythonKexiDBSchema inherited class to represent the
     * \a KexiDB::TableSchema class in python.
     */
    class PythonKexiDBTableSchema : public PythonKexiDBSchema<PythonKexiDBTableSchema>
    {
        public:

            /**
             * Constructor.
             *
             * \param tableschema The \a KexiDB::TableSchema instance.
             */
            explicit PythonKexiDBTableSchema(KexiDB::TableSchema* tableschema);

            /**
             * Destructor.
             */
            virtual ~PythonKexiDBTableSchema();

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
    };

    /**
     * From \a PythonKexiDBSchema inherited class to represent the
     * \a KexiDB::QuerySchema class in python.
     */
    class PythonKexiDBQuerySchema : public PythonKexiDBSchema<PythonKexiDBQuerySchema>
    {
        public:

            /**
             * Constructor.
             *
             * \param queryschema The \a KexiDB::QuerySchema instance.
             */
            explicit PythonKexiDBQuerySchema(KexiDB::QuerySchema* queryschema);

            /**
             * Destructor.
             */
            virtual ~PythonKexiDBQuerySchema();

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
            Py::Object statement(const Py::Tuple&);
            Py::Object setStatement(const Py::Tuple&);
    };

}

#endif

