/***************************************************************************
 * pythonkexidbfieldlist.cpp
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

#include "pythonkexidbfieldlist.h"
//#include "pythonkexidb.h"
#include "pythonkexidbfield.h"
#include "../main/pythonutils.h"

using namespace Kross;

namespace Kross
{
    class PythonKexiDBFieldListPrivate
    {
        public:
            KexiDB::FieldList* fieldlist;
    };
}

PythonKexiDBFieldList::PythonKexiDBFieldList(KexiDB::FieldList* fieldlist)
{
    d = new PythonKexiDBFieldListPrivate();
    d->fieldlist = fieldlist;
}

PythonKexiDBFieldList::~PythonKexiDBFieldList()
{
    //delete d->fieldlist;
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

    add_varargs_method("fieldCount", &PythonKexiDBFieldList::fieldCount,
        "uint KexiDBFileList.fieldCount()\n"
    );
    add_varargs_method("addField", &PythonKexiDBFieldList::addField,
        "KexiDBFileList KexiDBFileList.addField(KexiDBField)\n"
    );
    add_varargs_method("insertField", &PythonKexiDBFieldList::insertField,
        "KexiDBFileList KexiDBFileList.insertField(uint,KexiDBField)\n"
    );
    add_varargs_method("removeField", &PythonKexiDBFieldList::removeField,
        "None KexiDBFileList.removeField(KexiDBField)\n"
    );
    add_varargs_method("field", &PythonKexiDBFieldList::field,
        "KexiDBField KexiDBFileList.field()\n"
    );
    add_varargs_method("hasField", &PythonKexiDBFieldList::hasField,
        "boolean KexiDBFileList.hasField(KexiDBField)\n"
    );
    add_varargs_method("names", &PythonKexiDBFieldList::names,
        "list KexiDBFileList.names()\n"
    );
}

KexiDB::FieldList* PythonKexiDBFieldList::getFieldList() const
{
    return d->fieldlist;
}

Py::Object PythonKexiDBFieldList::fieldCount(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    return Py::Long((unsigned long)d->fieldlist->fieldCount());
}

Py::Object PythonKexiDBFieldList::addField(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    Py::ExtensionObject<PythonKexiDBField> obj(args[0]);
    PythonKexiDBField* field = obj.extensionObject();
    if(! field)
        throw Py::RuntimeError("KexiDBFieldList.addField(KexiDBField) Failed to determinate the defined KexiDBField object.");
    d->fieldlist->addField(field->getField());
    return Py::asObject(this);
}

Py::Object PythonKexiDBFieldList::insertField(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 2, 2);
    uint index = PythonUtils::toUInt(args[0]);
    Py::ExtensionObject<PythonKexiDBField> obj(args[1]);
    PythonKexiDBField* field = obj.extensionObject();
    if(! field)
        throw Py::RuntimeError("KexiDBFieldList.insertField(KexiDBField) Failed to determinate the defined KexiDBField object.");
    d->fieldlist->insertField(index, field->getField());
    return Py::asObject(this);
}

Py::Object PythonKexiDBFieldList::removeField(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    Py::ExtensionObject<PythonKexiDBField> obj(args[0]);
    PythonKexiDBField* field = obj.extensionObject();
    if(! field)
        throw Py::RuntimeError("KexiDBFieldList.removeField(KexiDBField) Failed to determinate the defined KexiDBField object.");
    d->fieldlist->removeField(field->getField());
    return Py::None();
}

Py::Object PythonKexiDBFieldList::field(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    KexiDB::Field* field = 0;
    if(args.isNumeric())
        field = d->fieldlist->field( PythonUtils::toUInt(args) );
    else {
        QString fieldname = QString(args.as_string().c_str());
        if(fieldname.isEmpty()) return Py::None();
        field = d->fieldlist->field(fieldname);
    }
    if(! field)
        throw Py::RuntimeError("KexiDBFieldList.field(fieldname) Failed to determinate KexiDB::Field.");
    return Py::asObject(new PythonKexiDBField(field));
}

Py::Object PythonKexiDBFieldList::hasField(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    Py::ExtensionObject<PythonKexiDBField> obj(args[0]);
    PythonKexiDBField* field = obj.extensionObject();
    if(! field)
        throw Py::RuntimeError("KexiDBFieldList.hasField(KexiDBField) Failed to determinate the defined KexiDBField object.");
    return Py::Int(d->fieldlist->hasField(field->getField()));
}

Py::Object PythonKexiDBFieldList::names(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    return PythonUtils::toPyObject(d->fieldlist->names());
}

