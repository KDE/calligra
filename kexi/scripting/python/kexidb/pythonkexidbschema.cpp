/***************************************************************************
 * pythonkexidbschema.cpp
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#include "pythonkexidbschema.h"
#include "../main/pythonutils.h"

using namespace Kross;

namespace Kross
{
    class PythonKexiDBSchemaPrivate
    {
        public:
            KexiDB::SchemaData* schema;
            PythonKexiDBFieldList* fieldlist;
    };
}

/*********************************************************************
 * PythonKexiDBSchema
 */

PythonKexiDBSchema::PythonKexiDBSchema(KexiDB::SchemaData* schema, KexiDB::FieldList* fieldlist)
{
    d = new PythonKexiDBSchemaPrivate();
    d->schema = schema;
    d->fieldlist = new PythonKexiDBFieldList(fieldlist);
}

PythonKexiDBSchema::~PythonKexiDBSchema()
{
    //delete d->fieldlist; // got auto destroyed.
    delete d;
}

bool PythonKexiDBSchema::accepts(PyObject* pyobj) const
{
    return pyobj && Py::PythonExtension<PythonKexiDBSchema>::check(pyobj);
}

Py::Object PythonKexiDBSchema::getattr(const char* n)
{
    std::string name(n);

    if(name == "__members__") {
        Py::List members;
        members.append(Py::String("name"));
        members.append(Py::String("caption"));
        members.append(Py::String("description"));
        members.append(Py::String("fieldlist"));
        return members;
    }

    if(name == "name")
        return PythonUtils::toPyObject(d->schema->name());
    if(name == "caption")
        return PythonUtils::toPyObject(d->schema->caption());
    if(name == "description")
        return PythonUtils::toPyObject(d->schema->description());
    if(name == "fieldlist")
        return Py::asObject(d->fieldlist);

    throw Py::AttributeError("Unknown attribute: " + name);
}

int PythonKexiDBSchema::setattr(const char* n, const Py::Object& value)
{
    std::string name(n);

    if(name == "name")
        d->schema->setName(value.as_string().c_str());
    else if(name == "caption")
        d->schema->setCaption(value.as_string().c_str());
    else if(name == "description")
        d->schema->setDescription(value.as_string().c_str());
    else
        throw Py::AttributeError("Unknown attribute: " + name);

    return 0;
}

KexiDB::SchemaData* PythonKexiDBSchema::getSchema()
{
    return d->schema;
}

/*********************************************************************
 * PythonKexiDBIndexSchema
 */

PythonKexiDBIndexSchema::PythonKexiDBIndexSchema(KexiDB::IndexSchema* indexschema)
    : PythonKexiDBSchema(indexschema, indexschema)
{
}

PythonKexiDBIndexSchema::~PythonKexiDBIndexSchema()
{
}

bool PythonKexiDBIndexSchema::accepts(PyObject* pyobj) const
{
    //FIXME: is that correct?
    return pyobj && Py::PythonExtension<PythonKexiDBIndexSchema>::check(pyobj);
}

void PythonKexiDBIndexSchema::init_type(void)
{
    behaviors().name("KexiDBIndexSchema");
    behaviors().doc(
        "The KexiDBIndexSchema object provides access to the "
        "KexiDB::IndexSchema class. "
    );
}

/*********************************************************************
 * PythonKexiDBTableSchema
 */

PythonKexiDBTableSchema::PythonKexiDBTableSchema(KexiDB::TableSchema* tableschema)
    : PythonKexiDBSchema(tableschema, tableschema)
{
}

PythonKexiDBTableSchema::~PythonKexiDBTableSchema()
{
}

bool PythonKexiDBTableSchema::accepts(PyObject* pyobj) const
{
    //FIXME: is that correct?
    return pyobj && Py::PythonExtension<PythonKexiDBTableSchema>::check(pyobj);
}

void PythonKexiDBTableSchema::init_type(void)
{
    behaviors().name("KexiDBTableSchema");
    behaviors().doc(
        "The KexiDBTableSchema object provides access to the "
        "KexiDB::TableSchema class. "
    );
}

