/***************************************************************************
 * pythoninterpreter.cpp
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

#include "pythoninterpreter.h"
#include "pythonmodule.h"
//#include "pythonextension.h"

#include "../api/variant.h"

#include "CXX/Objects.hxx"
//#include "CXX/Extensions.hxx"

using namespace Kross::Python;

PythonInterpreter::PythonInterpreter()
    : Kross::Api::Interpreter()
    , tstate(0)
{
    //kdDebug() << "Py_GetVersion()=" << Py_GetVersion() << " Py_GetPath()=" << Py_GetPath() << endl;

    // Set name of the program.
    Py_SetProgramName(const_cast<char*>("Kross"));

    // Initialize python.
    if(! Py_IsInitialized())
        Py_Initialize();

    // Set arguments.
    //PySys_SetArgv(argc, argv);
    //PySys_SetPath(Py_GetPath());

    // We will use an own thread for execution.
    PyEval_InitThreads();
    m_gtstate = PyEval_SaveThread();
}

PythonInterpreter::~PythonInterpreter()
{
    if(m_gtstate) {
        // Free the used thread.
        PyEval_AcquireThread(m_gtstate);

        // Finalize python.
        Py_Finalize();
    }
}

const QStringList PythonInterpreter::mimeTypes()
{
    return QStringList() << "text/x-python" << "application/x-python";
}

bool PythonInterpreter::parseString(QString&)
{
    /*TODO
    For the moment we don't use that function. It would be an idear
    to use it to extract what functions are avaible.
    We should spend a separated parse() function anyway to have
    something similar to execute() to verify the code.
    */
    return true;
}

bool PythonInterpreter::init()
{
    if(! m_gtstate) {
        kdWarning() << "PythonInterpreter::init(): PyEval_SaveThread() failed." << endl;
        return false;
    }
    if(tstate) {
        kdWarning() << "PythonInterpreter::init(): Interpreter wasn't released." << endl;
        return false;
    }

    // Execution will be done in it's own locked interpreter instance.
    PyEval_AcquireLock();
    tstate = Py_NewInterpreter();
    if(! tstate) {
        kdWarning() << "Py_NewInterpreter() failed. Execution aborted." << endl;
        return false;
    }

    // Create modules
    for(QMap<QString, Kross::Api::Object*>::Iterator it = m_modules.begin(); it != m_modules.end(); ++it) {
        PythonModule* pythonmodule = new PythonModule(it.key().latin1(), it.data());
        m_pythonmodules.replace(it.key(), pythonmodule);
    }

    return true;
}

bool PythonInterpreter::finesh()
{
    // Explicit free the modules.
    for(QMap<QString, PythonModule*>::Iterator it = m_pythonmodules.begin(); it != m_pythonmodules.end(); ++it) {
        //kdDebug() << "Deleting PythonModule name=" << it.key() << endl;
        delete it.data();
    }

    // Execution is done. Free the interpreter.
    Py_EndInterpreter(tstate);
    tstate = 0;

    // Release the lock.
    PyEval_ReleaseLock();

    return true;
}

bool PythonInterpreter::execute()
{
    //TESTCASE
    /*
    try {
        QValueList<Kross::Api::Object*> list;
        list.append( Kross::Api::Variant::create("This is the argument passed from within c/c++") );
        Kross::Api::Object* o = execute("myfunc", Kross::Api::List::create(list));
        kdDebug() << "RETURNVALUE => " << Kross::Api::Variant::toString(o) << endl;
        return true;
    }
    catch(Kross::Api::Exception& e) {
        kdDebug() << QString("myException => %1: %2").arg(e.type()).arg(e.description()) << endl;
        return false;
    }
    catch(Py::Exception&) {
        kdDebug() << "UNKNOWN EXCEPTION !!!" << endl;
        return false;
    }
    */

    if(! init()) return false;
    int r = PyRun_SimpleString((char*)getScript().latin1());
    return finesh() && r == 0;
}

Kross::Api::Object* PythonInterpreter::execute(const QString& name, Kross::Api::List* args)
{
    QString script = getScript();
    if(script.isEmpty())
        throw Kross::Api::AttributeException(i18n("No script to execute."));

    // Initialize
    if(! init())
        throw Kross::Api::RuntimeException(i18n("Failed to initialize interpreter."));

    // We need to use the dictonary of the main module. After PyRun_String
    // we are able to access that way the parsed script and determinate the
    // function to execute.
    Py::Module module("__main__");
    Py::Dict moduledict = module.getDict();

    // Parse the script.
    PyObject* run = PyRun_String((char*)script.latin1(), Py_file_input, moduledict.ptr(), moduledict.ptr());
    if(! run) {
        Py::Object errobj = Py::value(Py::Exception()); // get last error
        finesh();
        throw Kross::Api::RuntimeException(i18n("Python Exception: %1").arg(errobj.as_string().c_str()));
    }
    Py_XDECREF(run); // not any longer needed

    // Try to determinate the function we like to execute.
    PyObject* func = PyDict_GetItemString(moduledict.ptr(), name.latin1());
    if(! func) {
        finesh();
        throw Kross::Api::AttributeException(i18n("No such function."));
    }
    Py::Callable funcobject(func, true); // the funcobject takes care of freeing our func pyobject.

    // Check if the object is really a function and therefore callable.
    if(! funcobject.isCallable()) {
        finesh();
        throw Kross::Api::AttributeException(i18n("Function is not callable."));
    }

    Py::Object result = Py::None();
    try {
        // Call the function.
        result = funcobject.apply(PythonExtension::toPyTuple(args));
    }
    catch(Kross::Api::Exception& e) {
        finesh();
        throw e;
    }
    catch(Py::Exception& e) {
        Py::Object errobj = Py::value(e);
        finesh();
        throw Kross::Api::RuntimeException(i18n("Python Exception: %1").arg(errobj.as_string().c_str()));
    }

    if(! finesh())
        throw Kross::Api::RuntimeException(i18n("Failed to finalize interpreter."));

    return PythonExtension::toObject(result);
}

