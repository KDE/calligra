/***************************************************************************
 * pythonkexidbfieldlist.h
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

            /**
             * Constructor.
             *
             * \param fieldlist The \a KexiDB::FieldList instance.
             */
            explicit PythonKexiDBFieldList(KexiDB::FieldList* fieldlist);

            /**
             * Destructor.
             */
            virtual ~PythonKexiDBFieldList();

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
             * Return the \a KexiDB::FieldList instance.
             *
             * \return The fieldlist object.
             */
            KexiDB::FieldList* getFieldList() const;

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
