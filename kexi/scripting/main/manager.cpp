/***************************************************************************
 * manager.cpp
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

#include "manager.h"

#include "../api/interpreter.h"
//#include "../api/qtobject.h"
#include "../api/eventslot.h"
#include "../api/eventsignal.h"
//#include "../api/script.h"

#include "krossconfig.h"
#include "scriptcontainer.h"

#include <qobject.h>
#include <kdebug.h>
#include <klibloader.h>

using namespace Kross::Api;

extern "C"
{
    typedef int (*def_interpreter_func)(Kross::Api::Manager*, const QString&);
}

namespace Kross { namespace Api {

    /// @internal
    class ManagerPrivate
    {
        public:
            /// List of script instances.
            //QMap<QString, ScriptContainer* > m_scriptcontainers;

            /// List of interpreter instances.
            QMap<QString, Interpreter*> m_interpreter;

            /// To dynamicly load libraries.
            KLibrary* m_library;
    };

}}

Manager::Manager()
    : MainModule("Kross") // the manager has the name "Kross"
    , d( new ManagerPrivate() )
{
    d->m_library = 0;
}

Manager::~Manager()
{
    for(QMap<QString, Interpreter*>::Iterator iit = d->m_interpreter.begin(); iit != d->m_interpreter.end(); ++iit)
        delete iit.data();

    delete d;
}

ScriptContainer::Ptr Manager::getScriptContainer(const QString& scriptname)
{
    if(hadException())
        return 0;

    //TODO at the moment we don't share ScriptContainer instances.

    //if(d->m_scriptcontainers.contains(scriptname))
    //    return d->m_scriptcontainers[scriptname];
    ScriptContainer* scriptcontainer = new ScriptContainer(scriptname);
    //ScriptContainer script(this, scriptname);
    //d->m_scriptcontainers.replace(scriptname, scriptcontainer);

    return scriptcontainer;
}

Interpreter* Manager::getInterpreter(const QString& interpretername)
{
    if(d->m_interpreter.contains(interpretername))
        return d->m_interpreter[interpretername];

    Interpreter* interpreter = 0;
    setException(0);

    if(interpretername == "python") {

#ifdef KROSS_PYTHON_LIBRARY
        // At least on linux it's needed to dlopen the libpython
        // with RTLD_GLOBAL else python-scripts that use e.g.
        // "from qt import *" would result in python-traceback's
        // with undefined symbol messages. This is a long time
        // known issue with imported libs that import other libs.
        // Till today QLibrary failed to spend the possibility to
        // load libs with RTLD_GLOBAL while KLibrary does the job
        // fine. So, at least on linux we have to stick with
        // KLibrary or go the hard and unportable dlopen() way.
        // And yes, I tried the at http://docs.python.org/ext/link-reqs.html
        // described ways as well, but they and -rdynamic doesn't
        // helped in that case.
        // See also http://mats.imk.fraunhofer.de/pipermail/pykde/2004-April/007645.html

        if(d->m_library) {
            kdWarning() << "The krosspython library is already loaded." << endl;
        }
        else {
            // Load the krosspython library.
            KLibLoader *libloader = KLibLoader::self();

            d->m_library = libloader->globalLibrary( KROSS_PYTHON_LIBRARY );
            if(! d->m_library) {
                setException( new Exception("Failed to load the krosspython library.") );
                return 0;
            }
        }

        // Get the extern "C" krosspython_instance function.
        def_interpreter_func interpreter_func;
        interpreter_func = (def_interpreter_func) d->m_library->symbol("krosspython_instance");
        if(! interpreter_func) {
            setException( new Exception("Failed to load symbol in krosspython library.") );
        }
        else {
            // and execute the extern krosspython_instance function.
            interpreter = (Kross::Api::Interpreter*) (interpreter_func)(this, "python");
            if(! interpreter) {
                setException( new Exception("Failed to load PythonInterpreter instance from krosspython library.") );
            }
            else {
                // Job done. The library is loaded and our Interpreter* points
                // to the external Kross::Python::PythonInterpreter* instance.
                kdDebug()<<"Successfully loaded PythonInterpreter instance from krosspython library."<<endl;
            }
        }

        // finally unload the library.
        d->m_library->unload();
        d->m_library = 0;
#endif
    }
/*
    else if(interpretername == "kjs") {
#ifdef KROSS_KJS_LIBRARY
        interpreter = new Kross::Kjs::KjsInterpreter(this, "kjs");
#endif
    }
*/
    else {
        setException( new Exception(QString("No such interpreter '%1'").arg(interpretername)) );
    }

    if(interpreter)
        d->m_interpreter.replace(interpretername, interpreter);

    return interpreter;
}

const QStringList Manager::getInterpreters()
{
    QStringList list;

#ifdef KROSS_PYTHON_LIBRARY
    list << "python";
#endif

/*
#ifdef KROSS_KJS_LIBRARY
    list << "kjs";
#endif
*/
list << "TestCase";

    return  list;
}
