/***************************************************************************
 * pythonkexidbfield.h
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

#ifndef KROSS_PYTHONKEXIDBFIELD_H
#define KROSS_PYTHONKEXIDBFIELD_H

#include <Python.h>
#include "../CXX/Objects.hxx"
#include "../CXX/Extensions.hxx"

#include <kdebug.h>
//#include <qguardedptr.h>

#include <kexidb/drivermanager.h>
#include <kexidb/field.h>
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

            /**
             * Constructor.
             *
             * \param field The \a KexiDB::Field instance.
             */
            explicit PythonKexiDBField(KexiDB::Field* field);

            /**
             * Destructor.
             */
            virtual ~PythonKexiDBField();

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
             * Return the \a KexiDB::Field instance.
             *
             * \return The field object.
             */
            KexiDB::Field* getField() const;

        private:
            PythonKexiDBFieldPrivate* d;
    };

}

#endif
