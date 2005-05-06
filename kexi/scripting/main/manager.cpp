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
#include "../python/pythoninterpreter.h"
#include "../kjs/kjsinterpreter.h"
#include "../api/object.h"
#include "../api/qtobject.h"
#include "../api/eventslot.h"
//#include "../api/script.h"
#include "scriptcontainer.h"

#include <qobject.h>
#include <kdebug.h>

using namespace Kross::Api;

namespace Kross { namespace Api {

    class ManagerPrivate
    {
        public:
            /// List of script instances.
            QMap<QString, KSharedPtr<ScriptContainer> > m_scriptcontainers;
            /// List of interpreter instances.
            QMap<QString, Interpreter*> m_interpreter;
            /// List of avaible modules.
            QMap<QString, Object*> m_modules;
            /// The buildin \a EventSlot for basic Qt slots.
            EventSlot* m_buildin_slot;
            /// List of additional \a EventSlot instances.
            QValueList<EventSlot*> m_slots;
    };

}}

Manager::Manager()
    : d( new ManagerPrivate() )
{
    d->m_buildin_slot = new EventSlot();
    addEventSlot(d->m_buildin_slot);
}

Manager::~Manager()
{
/*
    for(QMap<QString, ScriptContainer*>::Iterator sit = m_scriptcontainers.begin(); sit != m_scriptcontainers.end(); ++sit)
        delete sit.data();
*/
    for(QMap<QString, Interpreter*>::Iterator iit = d->m_interpreter.begin(); iit != d->m_interpreter.end(); ++iit)
        delete iit.data();
    for(QMap<QString, Object*>::Iterator mit = d->m_modules.begin(); mit != d->m_modules.end(); ++mit)
        delete mit.data();
    delete d->m_buildin_slot;

    delete d;
}

bool Manager::hasModule(const QString& name)
{
    return d->m_modules.contains(name);
}

Object* Manager::getModule(const QString& name)
{
    return d->m_modules[name];
}

QMap<QString, Object*> Manager::getModules()
{
    return d->m_modules;
}

bool Manager::addModule(Object* module)
{
    if(! module) {
        kdWarning() << "Interpreter->addModule(Module*) failed cause Module is NULL" << endl;
        return false;
    }
    if(d->m_modules.contains(module->getName())) {
        kdWarning() << QString("Interpreter->addModule(Module*) failed cause there exists already a Module with name '%1'").arg(module->getName()) << endl;
        return false;
    }
    d->m_modules.replace(module->getName(), module);
    return true;
}

QValueList<EventSlot*> Manager::getEventSlots()
{
    return d->m_slots;
}

void Manager::addEventSlot(EventSlot* eventslot)
{
    d->m_slots.append( eventslot );
}

KSharedPtr<Kross::Api::ScriptContainer> Manager::getScriptContainer(const QString& scriptname)
{
    //TODO at the moment we don't share ScriptContainer ...

    //if(d->m_scriptcontainers.contains(scriptname))
    //    return d->m_scriptcontainers[scriptname];

    ScriptContainer* script = new ScriptContainer(this, scriptname);
    //ScriptContainer script(this, scriptname);
    //d->m_scriptcontainers.replace(scriptname, script);

    return KSharedPtr<Kross::Api::ScriptContainer>(script);
    //return script;
}

Interpreter* Manager::getInterpreter(const QString& interpretername)
{
    if(d->m_interpreter.contains(interpretername))
        return d->m_interpreter[interpretername];
    Interpreter* interpreter = 0;

    if(interpretername == "kjs")
        interpreter = new Kross::Kjs::KjsInterpreter(this, "kjs");
    else if(interpretername == "python")
        interpreter = new Kross::Python::PythonInterpreter(this, "python");

    if(interpreter)
        d->m_interpreter.replace(interpretername, interpreter);
    return interpreter;
}

