/***************************************************************************
 * pythonmodule.cpp
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
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

#include "pythonmodule.h"

#include <kdebug.h>

using namespace Kross::Python;

PythonModule::PythonModule(const char *name, Kross::Api::Object* object)
    : Py::ExtensionModule<PythonModule>(name)
    , m_object(object)
{
#ifdef KROSS_PYTHON_MODULE_DEBUG
    kdDebug() << QString("Kross::Python::PythonModule::Constructor name='%1'").arg(name) << endl;
#endif

    add_varargs_method("get", &PythonModule::get, "FIXME: Documentation");

    Kross::Api::ObjectMap children = object->getChildren();
    for(Kross::Api::ObjectMap::Iterator it = children.begin(); it != children.end(); ++it) {
#ifdef KROSS_PYTHON_MODULE_DEBUG
        kdDebug() << QString("Kross::Python::PythonModule adding extension '%1'.").arg(it.key()) << endl;
#endif
        PythonExtension* extension = new PythonExtension(it.data());
        m_extensions.replace(it.key(), extension);
    }

    initialize( object->getDescription().latin1() );
}

PythonModule::~PythonModule()
{
#ifdef KROSS_PYTHON_MODULE_DEBUG
    kdDebug() << QString("Kross::Python::PythonModule::Destructor name='%1'").arg(name().c_str()) << endl;
#endif
}

Py::Object PythonModule::get(const Py::Tuple& args)
{
    if(args.size() < 1)
        throw Py::TypeError("Too few arguments.");
    if(args.size() > 1)
        throw Py::TypeError("Too many arguments.");
    if(! args[0].isString())
        throw Py::TypeError("String argument expected.");
    QString name = args[0].as_string().c_str();
    if(! m_extensions.contains(name))
        throw Py::TypeError(QString("Unknown argument '%1'.").arg(name).latin1());
    PythonExtension* extension = m_extensions[name];
    Kross::Api::Object* obj = extension ? extension->getObject() : 0;
    if(! obj)
        throw Py::RuntimeError(QString("There exists no such object '%1'.").arg(name).latin1());
    //kdDebug() << "PythonModule::getObject name=" << name << " obj->getName()=" << (obj->getName().isNull() ? QString("<NULL>") : obj->getName()) << endl;
    return Py::asObject(extension);
}

