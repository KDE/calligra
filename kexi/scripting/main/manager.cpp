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
//#include "../api/script.h"
#include "scriptcontainer.h"

#include <qobject.h>

using namespace Kross::Api;

Manager::Manager()
{
}

Manager::~Manager()
{
    for(QMap<QString, ScriptContainer*>::Iterator sit = m_scriptcontainers.begin(); sit != m_scriptcontainers.end(); ++sit)
        delete sit.data();
    for(QMap<QString, Interpreter*>::Iterator iit = m_interpreter.begin(); iit != m_interpreter.end(); ++iit)
        delete iit.data();
    for(QMap<QString, Object*>::Iterator mit = m_modules.begin(); mit != m_modules.end(); ++mit)
        delete mit.data();
}

bool Manager::hasModule(const QString& name)
{
    return m_modules.contains(name);
}

Object* Manager::getModule(const QString& name)
{
    return m_modules[name];
}

QMap<QString, Object*> Manager::getModules()
{
    return m_modules;
}

bool Manager::addModule(Object* module)
{
    if(! module) {
        kdWarning() << "Interpreter->addModule(Module*) failed cause Module is NULL" << endl;
        return false;
    }
    if(m_modules.contains(module->getName())) {
        kdWarning() << QString("Interpreter->addModule(Module*) failed cause there exists already a Module with name '%1'").arg(module->getName()) << endl;
        return false;
    }
    m_modules.replace(module->getName(), module);
    return true;
}

ScriptContainer* Manager::getScriptContainer(const QString& scriptname)
{
    if(m_scriptcontainers.contains(scriptname))
        return m_scriptcontainers[scriptname];
    ScriptContainer* script = new ScriptContainer(this, scriptname);
    m_scriptcontainers.replace(scriptname, script);
    return script;
}

Interpreter* Manager::getInterpreter(const QString& interpretername)
{
    if(m_interpreter.contains(interpretername))
        return m_interpreter[interpretername];
    Interpreter* interpreter = 0;

    if(interpretername == "kjs")
        interpreter = new Kross::Kjs::KjsInterpreter(this, "kjs");
    else if(interpretername == "python")
        interpreter = new Kross::Python::PythonInterpreter(this, "python");

    if(interpreter)
        m_interpreter.replace(interpretername, interpreter);
    return interpreter;
}

