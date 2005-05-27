/***************************************************************************
 * pythoninterpreter.cpp
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

#include "pythoninterpreter.h"
#include "pythonscript.h"
#include "pythonmodule.h"
#include "pythonsecurity.h"
//#include "pythonextension.h"
#include "../api/variant.h"

extern "C"
{
    /**
     * Exported and loadable function as entry point to use
     * the \a PythonInterpreter.
     * The krosspython library the \a PythonInterpreter is part
     * will be loaded dynamicly at runtime from e.g.
     * \a Kross::Api::Manager::getInterpreter and this exported
     * function will be used to return an instance of the
     * \a PythonInterpreter implementation.
     */
    void* krosspython_instance(Kross::Api::Manager* manager, const QString& interpretername)
    {
        return new Kross::Python::PythonInterpreter(manager, interpretername);
    }
};

using namespace Kross::Python;

PythonInterpreter::PythonInterpreter(Kross::Api::Manager* manager, const QString& interpretername)
    : Kross::Api::Interpreter(manager, interpretername)
    , m_globalthreadstate(0)
{
    m_options.replace(
        "restricted",
        new Option(i18n("Restricted"), i18n("Enable RestrictedPython module."), QVariant((bool)false))
    );

    //kdDebug() << "Py_GetVersion()=" << Py_GetVersion() << " Py_GetPath()=" << Py_GetPath() << endl;

    // Set name of the program.
    Py_SetProgramName(const_cast<char*>("Kross"));

    // Initialize python.
    //if(! Py_IsInitialized()) Py_Initialize();
    Py_Initialize();

    // Set arguments.
    //PySys_SetArgv(argc, argv);
    //PySys_SetPath(Py_GetPath());

    // First we have to initialize threading if python supports it.
    PyEval_InitThreads();
    m_globalthreadstate = PyEval_SaveThread();

    // We use an own interpreter.
    PyEval_AcquireLock();
    m_threadstate = Py_NewInterpreter();

    // Initialize the main module.
    m_module = new PythonModule(this);

    // Prepare the global scope accessible by all PythonScript
    // instances. We import the global accessible Kross module.
    Py::Dict moduledict = m_module->getDict();
    QString s = "globalvar = 0\n"
                //"import sys\n"
                //"sys.path.append(\"/home/snoopy/cvs/kde/branch_0_9/koffice/kexi/scripting/python/zope/\");\n"
                "def maintestfunc():\n"
                "    print \"this is maintestfunc!\"\n"
                "    return \"this is the maintestfunc return value!\"\n";
    PyObject* pyrun = PyRun_String((char*)s.latin1(), Py_file_input, moduledict.ptr(), moduledict.ptr());
    if(! pyrun) {
        Py::Object errobj = Py::value(Py::Exception()); // get last error
        throw Kross::Api::RuntimeException(i18n("Failed to prepare the __main__ module: %1").arg(errobj.as_string().c_str()));
    }
    Py::Object run(pyrun, true);

    // Initialize the RestrictedPython module.
    m_security = new PythonSecurity(this);
}

PythonInterpreter::~PythonInterpreter()
{
    // Free the zope security module.
    delete m_security; m_security = 0;

    // Free the main module.
    delete m_module; m_module = 0;

    // Free the used interpreter.
    Py_EndInterpreter(m_threadstate);
    PyEval_ReleaseLock();

    // Free the used thread.
    PyEval_AcquireThread(m_globalthreadstate);

    // Finalize python.
    Py_Finalize();
}

const QStringList PythonInterpreter::mimeTypes()
{
    return QStringList() << "text/x-python" << "application/x-python";
}

Kross::Api::Script* PythonInterpreter::createScript(Kross::Api::ScriptContainer* scriptcontainer)
{
    return new PythonScript(this, scriptcontainer);
}

