/***************************************************************************
 * pythonkexidbfieldlist.cpp
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#include "pythonkexidbfieldlist.h"
//#include "pythonkexidbfield.h"
//#include "pythonkexidb.h"
//#include "../pythonutils.h"

#include <kexidb/drivermanager.h>
//#include <kexidb/field.h>
#include <kexidb/fieldlist.h>

using namespace Kross;

namespace Kross
{
    class PythonKexiDBFieldListPrivate
    {
        public:
            KexiDB::FieldList* fieldlist;
    };
}

PythonKexiDBFieldList::PythonKexiDBFieldList()
{
    d = new PythonKexiDBFieldListPrivate();
    //d->fieldlist = ;
}

PythonKexiDBFieldList::~PythonKexiDBFieldList()
{
    delete d;
}

bool PythonKexiDBFieldList::accepts(PyObject* pyobj) const
{
    return pyobj && Py::PythonExtension<PythonKexiDBFieldList>::check(pyobj);
}

void PythonKexiDBFieldList::init_type(void)
{
    behaviors().name("KexiDBFieldList");
    behaviors().doc(
        "The KexiDBFieldList object provides access to the "
        "KexiDB::FieldList class.\n"
    );
}

Py::Object PythonKexiDBFieldList::fieldCount(const Py::Tuple&)
{
}

Py::Object PythonKexiDBFieldList::addField(const Py::Tuple&)
{
}

Py::Object PythonKexiDBFieldList::insertField(const Py::Tuple&)
{
}

Py::Object PythonKexiDBFieldList::removeField(const Py::Tuple&)
{
}

Py::Object PythonKexiDBFieldList::field(const Py::Tuple&)
{
}

Py::Object PythonKexiDBFieldList::hasField(const Py::Tuple&)
{
}

Py::Object PythonKexiDBFieldList::names(const Py::Tuple&)
{
}

