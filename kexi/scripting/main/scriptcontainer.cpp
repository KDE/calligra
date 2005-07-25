/***************************************************************************
 * scriptcontainer.cpp
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

#include "scriptcontainer.h"
#include "../api/object.h"
#include "../api/list.h"
#include "../api/interpreter.h"
#include "../api/script.h"
//#include "../api/eventmanager.h"
#include "../main/manager.h"

using namespace Kross::Api;

namespace Kross { namespace Api {

    /// @internal
    class ScriptContainerPrivate
    {
        public:
            Script* m_script;
            QString m_name;
            QString m_code;
            QString m_interpretername;
    };

}}

ScriptContainer::ScriptContainer(const QString& name)
    : KShared() // initialize reference-counter
    , d( new ScriptContainerPrivate() )
{
    d->m_script = 0;
    //d->m_eventmanager = 0;
    d->m_name = name;
}

ScriptContainer::~ScriptContainer()
{
    finalize();
    delete d;
}

const QString& ScriptContainer::getName()
{
    return d->m_name;
}

const QString& ScriptContainer::getCode()
{
    return d->m_code;
}

void ScriptContainer::setCode(const QString& code)
{
    finalize();
    d->m_code = code;
}

const QString& ScriptContainer::getInterpreterName()
{
    return d->m_interpretername;
}

void ScriptContainer::setInterpreterName(const QString& name)
{
    finalize();
    d->m_interpretername = name;
}

void ScriptContainer::initialize()
{
    finalize();
    Interpreter* interpreter = Manager::scriptManager()->getInterpreter(d->m_interpretername);
    if(! interpreter)
        throw TypeException(i18n("Unknown interpreter on ScriptContainer::execute()."));
    d->m_script = interpreter->createScript(this);
}

void ScriptContainer::finalize()
{
    delete d->m_script;
    d->m_script = 0;
}

Object* ScriptContainer::execute()
{
    if(! d->m_script)
        initialize();
    return d->m_script->execute();
}

const QStringList& ScriptContainer::getFunctionNames()
{
    return d->m_script ? d->m_script->getFunctionNames() : QStringList();
}

KSharedPtr<Object> ScriptContainer::callFunction(const QString& functionname, List* arguments)
{
    if(functionname.isEmpty())
        throw RuntimeException(i18n("No functionname defined for ScriptContainer::callFunction()."));

    if(! d->m_script)
        initialize();
    return d->m_script->callFunction(functionname, arguments);
}

const QStringList& ScriptContainer::getClassNames()
{
    return d->m_script ? d->m_script->getClassNames() : QStringList();
}

KSharedPtr<Object> ScriptContainer::classInstance(const QString& name)
{
    if(! d->m_script)
        initialize();
    return d->m_script->classInstance(name);
}

bool ScriptContainer::connect(QObject *sender, const QCString& signal, const QString& functionname)
{
/*TODO
    if(! d->m_eventmanager) // create instance on demand
        d->m_eventmanager = new EventManager(this);
    return d->m_eventmanager->connect(sender, signal, functionname);
*/
    return false;
}

bool ScriptContainer::disconnect(QObject *sender, const QCString& signal, const QString& functionname)
{
/*TODO
    if(! d->m_eventmanager)
        return false;
    return d->m_eventmanager->disconnect(sender, signal, functionname);
*/
    return false;
}

bool ScriptContainer::connect(const QCString& signal, QObject *receiver, const QCString& slot)
{
/*TODO
    if(! d->m_eventmanager) // create instance on demand
        d->m_eventmanager = new EventManager(this);
    return d->m_eventmanager->connect(signal, receiver, slot);
*/
    return false;
}

bool ScriptContainer::disconnect(const QCString& signal, QObject *receiver, const QCString& slot)
{
/*TODO
    if(! d->m_eventmanager)
        return false;
    return d->m_eventmanager->disconnect(signal, receiver, slot);
*/
    return false;
}

