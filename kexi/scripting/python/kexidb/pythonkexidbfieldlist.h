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

//#include <kdebug.h>
//#include <qguardedptr.h>

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
            PythonKexiDBFieldList();
            virtual ~PythonKexiDBFieldList();

            virtual bool accepts(PyObject* pyobj) const;
            static void init_type(void);

        private:
            PythonKexiDBFieldListPrivate* d;

            Py::Object fieldCount(const Py::Tuple&);
            Py::Object addField(const Py::Tuple&);
            Py::Object insertField(const Py::Tuple&);
            Py::Object removeField(const Py::Tuple&);
            Py::Object field(const Py::Tuple&);
            Py::Object hasField(const Py::Tuple&);
            Py::Object names(const Py::Tuple&);

            /*
            // \return number of fields in the list.
            inline unsigned int fieldCount() const { return m_fields.count(); }
            // Adds \a field at the and of field list.
            FieldList& addField(Field *field);
            // Inserts \a field into a specified position (\a index).
            // Note: You can reimplement this method but you should still call
            //this implementation in your subclass.
            virtual FieldList& insertField(uint index, Field *field);
            // Removes field from the field list. Use with care.
            // Note: You can reimplement this method but you should still call
            // this implementation in your subclass.
            virtual void removeField(KexiDB::Field *field);
            // \return field #id or NULL if there is no such a field.
            inline Field* field(unsigned int id) { return (id < m_fields.count()) ? m_fields.at(id) : 0; }
            // \return field with name \a name or NULL if there is no such a field.
            inline Field* field(const QString& name) const { return m_fields_by_name[name.lower()]; }
            // \return true if this list contains given \a field.
            inline bool hasField(Field* field) { return m_fields.findRef(field)!=-1; }
            // \return list of field names for this list.
            QStringList names() const;
            */
    };

}

#endif
