/***************************************************************************
 * pythonscript.cpp
 * This file is part of the KDE project
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

#include "pythonscript.h"
#include "pythonmodule.h"
#include "pythoninterpreter.h"
//#include "../api/object.h"
//#include "../api/list.h"
//#include "../main/manager.h"
#include "../main/scriptcontainer.h"
//#include "../api/qtobject.h"
//#include "../api/interpreter.h"

#include <kdebug.h>

using namespace Kross::Python;

PythonScript::PythonScript(Kross::Api::Interpreter* interpreter, Kross::Api::ScriptContainer* scriptcontainer)
    : Kross::Api::Script(interpreter, scriptcontainer)
    , m_module(0)
{
    PyObject* pymod = PyModule_New((char*)m_scriptcontainer->getName().latin1());
    m_module = new Py::Module(pymod, true);
}

PythonScript::~PythonScript()
{
    delete m_module; m_module = 0;
}

Kross::Api::Object* PythonScript::execute()
{
    try {
        Py::Dict mainmoduledict = ((PythonInterpreter*)m_interpreter)->m_mainmodule->getDict();

        PyObject* pyrun = PyRun_String(
            (char*)m_scriptcontainer->getCode().latin1(),
            Py_file_input,
            mainmoduledict.ptr(),
            m_module->getDict().ptr()
        );
        if(! pyrun) {
            Py::Object errobj = Py::value(Py::Exception()); // get last error
            throw Kross::Api::RuntimeException(i18n("Python Exception: %1").arg(errobj.as_string().c_str()));
        }
        Py::Object run(pyrun, true);

        //kdDebug() << QString("PythonScript::execute() PyRun_String='%1'").arg(run.str().as_string().c_str()) << endl;
        return PythonExtension::toObject(run);
    }
    catch(Py::Exception& e) {
        Py::Object errobj = Py::value(e);
        throw Kross::Api::RuntimeException(i18n("Python Exception: %1").arg(errobj.as_string().c_str()));
    }
}

Kross::Api::Object* PythonScript::callFunction(const QString& name, Kross::Api::List* args)
{
    try {
        Py::Dict moduledict = m_module->getDict();

        // Try to determinate the function we like to execute.
        PyObject* func = PyDict_GetItemString(moduledict.ptr(), name.latin1());
        if(! func)
            throw Kross::Api::AttributeException(i18n("No such function '%1'.").arg(name));
        Py::Callable funcobject(func, true); // the funcobject takes care of freeing our func pyobject.

        // Check if the object is really a function and therefore callable.
        if(! funcobject.isCallable())
            throw Kross::Api::AttributeException(i18n("Function is not callable."));

        // Call the function.
        Py::Object result = funcobject.apply(PythonExtension::toPyTuple(args));
        return PythonExtension::toObject(result);
    }
    catch(Py::Exception& e) {
        Py::Object errobj = Py::value(e);
        throw Kross::Api::RuntimeException(i18n("Python Exception: %1").arg(errobj.as_string().c_str()));
    }
}

