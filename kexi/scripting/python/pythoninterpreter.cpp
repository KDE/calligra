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
    // Options the python interpreter spends.
    m_options.replace(
        "restricted",
        new Option(QString("Restricted"), QString("Enable RestrictedPython module."), QVariant((bool)false))
    );

    //kdDebug() << "Py_GetVersion()=" << Py_GetVersion() << " Py_GetPath()=" << Py_GetPath() << endl;

    // Set name of the program.
    Py_SetProgramName(const_cast<char*>("Kross"));

    // Initialize python.
    Py_Initialize();

    // Set arguments.
    char* comm[0];
    comm[0] = const_cast<char*>("kross"); // name.
    PySys_SetArgv(1, comm);

    // Set the python path.
    //PySys_SetPath(Py_GetPath());

    // First we have to initialize threading if python supports it.
    PyEval_InitThreads();
    // The main thread. We don't use it later.
    m_globalthreadstate = PyThreadState_Swap(NULL);
    //m_globalthreadstate = PyEval_SaveThread();
    // We use an own sub-interpreter for each thread.
    m_threadstate = Py_NewInterpreter();
    // Note that this application has multiple threads.
    // It maintains a separate interp (sub-interpreter) for each thread.
    PyThreadState_Swap(m_threadstate);
    // Work done, release the lock.
    PyEval_ReleaseLock();

    // Initialize the main module.
    m_mainmodule = new PythonModule(this);

    // The main dictonary.
    Py::Dict moduledict = m_mainmodule->getDict();
    //TODO moduledict["KrossPythonVersion"] = Py::Int(KROSS_PYTHON_VERSION);

    // Prepare the interpreter.
    QString s = "import sys\n"
                //"sys.path.append(\"/home/snoopy/cvs/kde/branch_0_9/koffice/kexi/scripting/python/zope/\");\n"
                //"sys.stdout = self._bu\n"
                //"sys.stderr = self._bu\n"
                "import cStringIO\n"
                "sys.stdin = cStringIO.StringIO()\n"
                ;

    PyObject* pyrun = PyRun_String((char*)s.latin1(), Py_file_input, moduledict.ptr(), moduledict.ptr());
    if(! pyrun) {
        Py::Object errobj = Py::value(Py::Exception()); // get last error
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("Failed to prepare the __main__ module: %1").arg(errobj.as_string().c_str())) );
    }
    Py_XDECREF(pyrun); // free the reference.

    // Initialize the RestrictedPython module.
    m_security = new PythonSecurity(this);
}

PythonInterpreter::~PythonInterpreter()
{
    // Free the zope security module.
    delete m_security; m_security = 0;

    // Free the main module.
    delete m_mainmodule; m_mainmodule = 0;

    // Lock threads.
    PyEval_AcquireLock();
    // Free the used thread.
    PyEval_ReleaseThread(m_threadstate);
    // Set back to rememberd main thread.
    PyThreadState_Swap(m_globalthreadstate);
    // Work done, unlock.
    PyEval_ReleaseLock();
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

