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

#include <kglobal.h>
#include <kstandarddirs.h>

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
//TODO do we need the manager?
        //return new Kross::Python::PythonInterpreter(manager, interpretername);
        return new Kross::Python::PythonInterpreter(interpretername);
    }
};

using namespace Kross::Python;

namespace Kross { namespace Python {

    /// \internal
    class PythonInterpreterPrivate
    {
        public:

            /// The __main__ python module.
            PythonModule* mainmodule;

            /// The \a PythonSecurity python module to wrap the RestrictedPython functionality.
            PythonSecurity* security;

            /**
             * Python uses so called threads to separate
             * executions. The PyThreadState holds the
             * thread we use for this Python bridge.
             */
            PyThreadState* globalthreadstate;
            PyThreadState* threadstate;
    };

}}

PythonInterpreter::PythonInterpreter(const QString& interpretername)
    : Kross::Api::Interpreter(interpretername)
    , d(new PythonInterpreterPrivate())
{
    // Initialize the python interpreter.
    initialize();

    // Options the python interpreter provides.
    m_options.replace(
        "restricted",
        new Option(QString("Restricted"), QString("Enable RestrictedPython module."), QVariant((bool)false))
    );

    // Set name of the program.
    Py_SetProgramName(const_cast<char*>("Kross"));

    // Set arguments.
    char* comm[0];
    comm[0] = const_cast<char*>("kross"); // name.
    PySys_SetArgv(1, comm);

    // Set the python path.
    //PySys_SetPath(Py_GetPath());
    QString path = Py_GetPath();
    QStringList dirs = KGlobal::dirs()->findDirs("appdata", "scripts/kross");
    for(QStringList::Iterator it = dirs.begin(); it != dirs.end(); ++it) {
        path.append(
#if defined(Q_WS_WIN)
                ";" // windows delimiter for python sys-paths.
#else
                ":"
#endif
                + *it);
    }
    PySys_SetPath( const_cast<char*>( path.latin1() ) );

    kdDebug() << "Python ProgramName: " << Py_GetProgramName() << endl;
    kdDebug() << "Python ProgramFullPath: " << Py_GetProgramFullPath() << endl;
    kdDebug() << "Python Version: " << Py_GetVersion() << endl;
    kdDebug() << "Python Platform: " << Py_GetPlatform() << endl;
    kdDebug() << "Python Prefix: " << Py_GetPrefix() << endl;
    kdDebug() << "Python ExecPrefix: " << Py_GetExecPrefix() << endl;
    kdDebug() << "Python Path: " << Py_GetPath() << endl;

    // Initialize the main module.
    d->mainmodule = new PythonModule(this);

    // The main dictonary.
    Py::Dict moduledict = d->mainmodule->getDict();
    //TODO moduledict["KrossPythonVersion"] = Py::Int(KROSS_PYTHON_VERSION);

    // Prepare the interpreter.
    QString s =
        "import sys\n"

        // Dirty hack to get sys.argv defined. Needed for e.g. TKinter.
        "sys.argv = ['']\n"

        // On the try to read something from stdin always return an empty
        // string. That way such reads don't block our script.
        "import cStringIO\n"
        "sys.stdin = cStringIO.StringIO()\n"

        // Class to redirect something. We use this class e.g. to redirect
        // <stdout> and <stderr> to a c++ event.
        "class Redirect:\n"
        "  def __init__(self, target):\n"
        "    self.target = target\n"
        "  def write(self, s):\n"
        "    self.target.call(s)\n"

        // Wrap builtin __import__ method. All import requests are
        // first redirected to our PythonModule.import method and
        // if the call returns None, then we call the original
        // python import mechanism.
        "import __builtin__\n"
        "import __main__\n"
        "class Importer:\n"
        "    def __init__(self):\n"
        "        self.realImporter = __builtin__.__import__\n"
        "        __builtin__.__import__ = self._import\n"
        "    def _import(self, name, globals=None, locals=None, fromlist=[]):\n"
        "        mod = __main__._import(name, globals, locals, fromlist)\n"
        "        if mod != None: return mod\n"
        "        return self.realImporter(name, globals, locals, fromlist)\n"
        "Importer()\n"
        ;

    PyObject* pyrun = PyRun_String((char*)s.latin1(), Py_file_input, moduledict.ptr(), moduledict.ptr());
    if(! pyrun) {
        Py::Object errobj = Py::value(Py::Exception()); // get last error
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("Failed to prepare the __main__ module: %1").arg(errobj.as_string().c_str())) );
    }
    Py_XDECREF(pyrun); // free the reference.

    // Initialize the RestrictedPython module.
    d->security = new PythonSecurity(this);
}

PythonInterpreter::~PythonInterpreter()
{
    // Free the zope security module.
    delete d->security; d->security = 0;
    // Free the main module.
    delete d->mainmodule; d->mainmodule = 0;
    // Finalize the python interpreter.
    finalize();
    // Delete the private d-pointer.
    delete d;
}

void PythonInterpreter::initialize()
{
    // Initialize python.
    Py_Initialize();
    // First we have to initialize threading if python supports it.
    PyEval_InitThreads();
    // The main thread. We don't use it later.
    d->globalthreadstate = PyThreadState_Swap(NULL);
    //d->globalthreadstate = PyEval_SaveThread();
    // We use an own sub-interpreter for each thread.
    d->threadstate = Py_NewInterpreter();
    // Note that this application has multiple threads.
    // It maintains a separate interp (sub-interpreter) for each thread.
    PyThreadState_Swap(d->threadstate);
    // Work done, release the lock.
    PyEval_ReleaseLock();
}

void PythonInterpreter::finalize()
{
    // Lock threads.
    PyEval_AcquireLock();
    // Free the used thread.
    PyEval_ReleaseThread(d->threadstate);
    // Set back to rememberd main thread.
    PyThreadState_Swap(d->globalthreadstate);
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

PythonModule* PythonInterpreter::mainModule()
{
    return d->mainmodule;
}
