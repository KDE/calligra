/***************************************************************************
 * pythonkexidbschema.cpp
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

template<TEMPLATE_TYPENAME T>
PythonKexiDBSchema<T>::PythonKexiDBSchema<T>(KexiDB::SchemaData* schema, KexiDB::FieldList* fieldlist)
{
    d = new PythonKexiDBSchemaPrivate();
    d->schema = schema;
    d->fieldlist = new PythonKexiDBFieldList(fieldlist);
}

template<TEMPLATE_TYPENAME T>
PythonKexiDBSchema<T>::~PythonKexiDBSchema<T>()
{
    //delete d->fieldlist; // got auto destroyed.
    delete d;
}

template<TEMPLATE_TYPENAME T>
bool PythonKexiDBSchema<T>::accepts(PyObject* pyobj) const
{
    return pyobj && Py::PythonExtension<PythonKexiDBSchema>::check(pyobj);
}

template<TEMPLATE_TYPENAME T>
void PythonKexiDBSchema<T>::init_type(void)
{
    behaviors().supportGetattr();
    behaviors().supportSetattr();
}

template<TEMPLATE_TYPENAME T>
Py::Object PythonKexiDBSchema<T>::getattr(const char* n)
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

    return getattr_methods(n); // needed to eval methods as well.
}

template<TEMPLATE_TYPENAME T>
int PythonKexiDBSchema<T>::setattr(const char* n, const Py::Object& value)
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

template<TEMPLATE_TYPENAME T>
KexiDB::SchemaData* PythonKexiDBSchema<T>::getSchema()
{
    return d->schema;
}

/*********************************************************************
 * PythonKexiDBIndexSchema
 */

PythonKexiDBIndexSchema::PythonKexiDBIndexSchema(KexiDB::IndexSchema* indexschema)
    : PythonKexiDBSchema<PythonKexiDBIndexSchema>(indexschema, indexschema)
{
}

PythonKexiDBIndexSchema::~PythonKexiDBIndexSchema()
{
}

bool PythonKexiDBIndexSchema::accepts(PyObject* pyobj) const
{
    return pyobj && Py::PythonExtension<PythonKexiDBIndexSchema>::check(pyobj);
}

void PythonKexiDBIndexSchema::init_type(void)
{
    behaviors().name("KexiDBIndexSchema");
    behaviors().doc(
        "The KexiDBIndexSchema object provides access to the "
        "KexiDB::IndexSchema class. "
    );

    PythonKexiDBSchema<PythonKexiDBIndexSchema>::init_type();
}

/*********************************************************************
 * PythonKexiDBTableSchema
 */

PythonKexiDBTableSchema::PythonKexiDBTableSchema(KexiDB::TableSchema* tableschema)
    : PythonKexiDBSchema<PythonKexiDBTableSchema>(tableschema, tableschema)
{
}

PythonKexiDBTableSchema::~PythonKexiDBTableSchema()
{
}

bool PythonKexiDBTableSchema::accepts(PyObject* pyobj) const
{
    return pyobj && Py::PythonExtension<PythonKexiDBTableSchema>::check(pyobj);
}

void PythonKexiDBTableSchema::init_type(void)
{
    behaviors().name("KexiDBTableSchema");
    behaviors().doc(
        "The KexiDBTableSchema object provides access to the "
        "KexiDB::TableSchema class. "
    );

    PythonKexiDBSchema<PythonKexiDBTableSchema>::init_type();
}

/*********************************************************************
 * PythonKexiDBQuerySchema
 */

PythonKexiDBQuerySchema::PythonKexiDBQuerySchema(KexiDB::QuerySchema* queryschema)
    : PythonKexiDBSchema<PythonKexiDBQuerySchema>(queryschema, queryschema)
{
}

PythonKexiDBQuerySchema::~PythonKexiDBQuerySchema()
{
}

bool PythonKexiDBQuerySchema::accepts(PyObject* pyobj) const
{
    return pyobj && Py::PythonExtension<PythonKexiDBQuerySchema>::check(pyobj);
}

void PythonKexiDBQuerySchema::init_type(void)
{
    behaviors().name("KexiDBQuerySchema");
    behaviors().doc(
        "The KexiDBQuerySchema object provides access to the "
        "KexiDB::QuerySchema class. "
    );

    PythonKexiDBSchema<PythonKexiDBQuerySchema>::init_type();

    add_varargs_method("statement", &PythonKexiDBQuerySchema::statement,
        "string KexiDBConnection.statement()\n"
    );
    add_varargs_method("setStatement", &PythonKexiDBQuerySchema::setStatement,
        "KexiDBConnection.setStatement(string)\n"
    );
}

Py::Object PythonKexiDBQuerySchema::statement(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 0, 0);
    return PythonUtils::toPyObject( ((KexiDB::QuerySchema*)getSchema())->statement() );
}

Py::Object PythonKexiDBQuerySchema::setStatement(const Py::Tuple& args)
{
    PythonUtils::checkArgs(args, 1, 1);
    if(! args[0].isString())
        throw Py::TypeError("boolean KexiDBQuerySchema.setStatement(string) Invalid argument. String expected.");
    ((KexiDB::QuerySchema*)getSchema())->setStatement( args[0].as_string().c_str() );
    return Py::None();
}

