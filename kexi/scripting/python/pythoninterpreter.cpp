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

using namespace Kross::Python;

PythonInterpreter::PythonInterpreter()
    : Kross::Api::Interpreter()
{
    kdDebug() << "Python Version " << Py_GetVersion() << endl;

    // Set name of the program.
    Py_SetProgramName(const_cast<char*>("Kross"));

    // Initialize python.
    if(! Py_IsInitialized())
        Py_Initialize();

    // Set arguments.
    //PySys_SetArgv(argc, argv);

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

bool PythonInterpreter::execute(const QString& execstring)
{
    if(! m_gtstate) {
        kdWarning() << "PyEval_SaveThread() failed. Execution aborted." << endl;
        return false;
    }

    // Execution will be done in it's own locked interpreter
    // instance.
    PyEval_AcquireLock();
    PyThreadState *tstate = Py_NewInterpreter();
    if(! tstate) {
        kdWarning() << "Py_NewInterpreter() failed. Execution aborted." << endl;
        return false;
    }

    // Create modules
    for(QMap<QString, Kross::Api::Object*>::Iterator it = m_modules.begin(); it != m_modules.end(); ++it) {
        PythonModule* pythonmodule = new PythonModule(it.key().latin1(), it.data());
        m_pythonmodules.replace(it.key(), pythonmodule);
    }

    // Execute the string.
    PyRun_SimpleString((char*)execstring.latin1());

    // Explicit free the modules.
    for(QMap<QString, PythonModule*>::Iterator it = m_pythonmodules.begin(); it != m_pythonmodules.end(); ++it) {
        kdDebug() << "Deleting PythonModule name=" << it.key() << endl;
        delete it.data();
    }

    // Execution is done. Free the interpreter and release the lock.
    Py_EndInterpreter(tstate);
    PyEval_ReleaseLock();

    return true;
}

