/***************************************************************************
 * pythonkexidbfield.cpp
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#include "pythonkexidbfield.h"
//#include "pythonkexidb.h"
#include "../pythonutils.h"

#include <kexidb/drivermanager.h>
#include <kexidb/field.h>

using namespace Kross;

namespace Kross
{
    class PythonKexiDBFieldPrivate
    {
        public:
            KexiDB::Field* field;
    };
}

PythonKexiDBField::PythonKexiDBField()
{
    d = new PythonKexiDBFieldPrivate();
}

PythonKexiDBField::~PythonKexiDBField()
{
    delete d;
}

bool PythonKexiDBField::accepts(PyObject* pyobj) const
{
    return pyobj && Py::PythonExtension<PythonKexiDBField>::check(pyobj);
}

void PythonKexiDBField::init_type(void)
{
    behaviors().name("KexiDBField");
    behaviors().doc(
        "The KexiDBField object provides access to the "
        "KexiDB::Field class.\n"
    );
    behaviors().supportGetattr();
    behaviors().supportSetattr();
}

Py::Object PythonKexiDBField::getattr(const char* n)
{
    std::string name(n);

    if(name == "__members__") {
        Py::List members;

        // Type
        for(int i = 0; i < KexiDB::Field::LastType; i++)
            members.append(PythonUtils::toPyObject( KexiDB::Field::typeString(i) ));

        // TypeGroup
        for(int i = 0; i < KexiDB::Field::LastTypeGroup; i++)
            members.append(PythonUtils::toPyObject( KexiDB::Field::typeGroupString(i) ));

        // Constraints
        members.append(Py::String("AutoInc"));
        members.append(Py::String("Unique"));
        members.append(Py::String("PrimaryKey"));
        members.append(Py::String("ForeignKey"));
        members.append(Py::String("NotNull"));
        members.append(Py::String("NotEmpty"));
        members.append(Py::String("Indexed"));

        // Some more members...
        members.append(Py::String("name"));
        members.append(Py::String("caption"));
        members.append(Py::String("description"));
        members.append(Py::String("length"));
        members.append(Py::String("precision"));

        return members;
    }

    // Type
    KexiDB::Field::Type type = KexiDB::Field::typeForString(name);
    if(type != KexiDB::Field::InvalidType && type < KexiDB::Field::LastType)
        return Py::Int(type == d->field->type());

    // TypeGroup
    KexiDB::Field::TypeGroup typegroup = KexiDB::Field::typeGroupForString(name);
    if(typegroup != KexiDB::Field::InvalidGroup && typegroup < KexiDB::Field::LastTypeGroup)
        return Py::Int(typegroup == d->field->typeGroup());

    // Constraints
    if(name == "AutoInc")
        return Py::Int(d->field->isAutoIncrement());
    if(name == "Unique")
        return Py::Int(d->field->isUniqueKey());
    if(name == "PrimaryKey")
        return Py::Int(d->field->isPrimaryKey());
    if(name == "ForeignKey")
        return Py::Int(d->field->isForeignKey());
    if(name == "NotNull")
        return Py::Int(d->field->isNotNull());
    if(name == "NotEmpty")
        return Py::Int(d->field->isNotEmpty());
    if(name == "Indexed")
        return Py::Int(d->field->isIndexed());

    // Some more members...
    if(name == "name")
        return PythonUtils::toPyObject(d->field->name());
    if(name == "caption")
        return PythonUtils::toPyObject(d->field->caption());
    if(name == "description")
        return PythonUtils::toPyObject(d->field->description());
    if(name == "length")
        return Py::Long((unsigned long)d->field->length());
    if(name == "precision")
        return Py::Long((unsigned long)d->field->precision());

    throw Py::AttributeError("Unknown attribute: " + name);
}

int PythonKexiDBField::setattr(const char* n, const Py::Object& value)
{
    std::string name(n);

    // Type
    KexiDB::Field::Type type = KexiDB::Field::typeForString(name);
    if(type != KexiDB::Field::InvalidType && type < KexiDB::Field::LastType)
        d->field->setType(type);

    // Constraints
    else if(name == "AutoInc")
        d->field->setAutoIncrement( Py::Int(value) );
    else if(name == "Unique")
        d->field->setUniqueKey( Py::Int(value) );
    else if(name == "PrimaryKey")
        d->field->setPrimaryKey( Py::Int(value) );
    else if(name == "ForeignKey")
        d->field->setForeignKey( Py::Int(value) );
    else if(name == "NotNull")
        d->field->setNotNull( Py::Int(value) );
    else if(name == "NotEmpty")
        d->field->setNotEmpty( Py::Int(value) );
    else if(name == "Indexed")
        d->field->setIndexed( Py::Int(value) );

    // Some more members...
    else if(name == "name")
        d->field->setName(value.as_string());
    else if(name == "caption")
        d->field->setCaption(value.as_string());
    else if(name == "description")
        d->field->setDescription(value.as_string());
    else if(name == "length")
        d->field->setLength( PythonUtils::toUInt(value) );
    else if(name == "precision")
        d->field->setPrecision( PythonUtils::toUInt(value) );
    else
        throw Py::AttributeError("Unknown attribute: " + name);

    return 0;
}

