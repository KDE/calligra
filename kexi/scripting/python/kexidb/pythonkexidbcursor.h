/***************************************************************************
 * pythonkexidbcursor.h
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#ifndef KROSS_PYTHONKEXIDBCURSOR_H
#define KROSS_PYTHONKEXIDBCURSOR_H

#include <Python.h>

#include "../CXX/Objects.hxx"
#include "../CXX/Extensions.hxx"

#include <kdebug.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>

namespace Kross
{

    class PythonKexiDBConnection;
    class PythonKexiDBCursorPrivate;

    /**
     * The PythonKexiDBCursor class is a from Py::Object inherited
     * object to represent the KexiDB::Cursor class in python.
     */
    class PythonKexiDBCursor : public Py::PythonExtension<PythonKexiDBCursor>
    {
        public:

            /**
             * Constructor.
             */
            PythonKexiDBCursor(PythonKexiDBConnection*, KexiDB::Cursor*);

            /**
             * Destructor.
             */
            virtual ~PythonKexiDBCursor();

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
            PythonKexiDBCursorPrivate* d;

            Py::Object moveFirst(const Py::Tuple&);
            Py::Object moveLast(const Py::Tuple&);
            Py::Object moveNext(const Py::Tuple&);
            Py::Object movePrev(const Py::Tuple&);
            Py::Object eof(const Py::Tuple&);
            Py::Object bof(const Py::Tuple&);

            Py::Object value(const Py::Tuple&);
            Py::Object at(const Py::Tuple&);
            Py::Object fieldCount(const Py::Tuple&);

            //bool updateRow(RowData& data, RowEditBuffer& buf);
            //bool insertRow(RowData& data, RowEditBuffer& buf);
            //bool deleteRow(RowData& data);
            //bool deleteAllRows();
            //virtual const char ** rowData() const = 0;
    };

}

#endif
