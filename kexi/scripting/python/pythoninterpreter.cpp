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
//#include "pythonextension.h"
#include "../api/variant.h"

using namespace Kross::Python;

PythonInterpreter::PythonInterpreter(Kross::Api::Manager* manager, const QString& interpretername)
    : Kross::Api::Interpreter(manager, interpretername)
    , m_globalthreadstate(0)
{
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
    m_mainmodule = new Py::Module("__main__");

    // Initialize the module manager.
    m_modulemanager = new PythonModuleManager(this);

    // Prepare the global scope accessible by all PythonScript
    // instances. We import the global accessible Kross module.
    Py::Dict moduledict = m_mainmodule->getDict();
    QString s = "import Kross\n"
                "globalvar = 0\n"
                "def maintestfunc():\n"
                "    print \"this is maintestfunc!\"\n"
                "    return \"this is the maintestfunc return value!\"\n";
    PyObject* run = PyRun_String((char*)s.latin1(), Py_file_input, moduledict.ptr(), moduledict.ptr());
    if(! run)
        throw Kross::Api::RuntimeException(i18n("Failed to prepare the __main__ module."));
}

PythonInterpreter::~PythonInterpreter()
{
    // Free the main module.
    delete m_mainmodule; m_mainmodule = 0;

    // Free the module manager.
    delete m_modulemanager; m_modulemanager = 0;

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

