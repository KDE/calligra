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
#include "../main/manager.h"
#include "../api/qtobject.h"
#include "../api/interpreter.h"
#include "../api/script.h"

using namespace Kross::Api;

ScriptContainer::ScriptContainer(Manager* manager, const QString& name)
    : QObject()
    , m_manager(manager)
    , m_name(name)
    , m_script(0)
    , m_functionargs(0)
{
}

ScriptContainer::~ScriptContainer()
{
    setScript();
}

const QString& ScriptContainer::getName()
{
    return m_name;
}

const QString& ScriptContainer::getCode()
{
    return m_code;
}

void ScriptContainer::setCode(const QString& code)
{
    setScript();
    m_code = code;
}

const QString& ScriptContainer::getInterpreterName()
{
    return m_interpretername;
}

void ScriptContainer::setInterpreterName(const QString& name)
{
    setScript();
    m_interpretername = name;
}

const QString& ScriptContainer::getFunctionName()
{
    return m_functionname;
}

void ScriptContainer::setFunctionName(const QString& name)
{
    m_functionname = name;
}

Kross::Api::List* ScriptContainer::getFunctionArguments()
{
    return m_functionargs;
}

void ScriptContainer::setFunctionArguments(Kross::Api::List* args)
{
    m_functionargs = args;
}

Script* ScriptContainer::getScript()
{
    return m_script;
}

void ScriptContainer::setScript(Script* script)
{
    if(script != m_script) {
        delete m_script;
        m_script = script;
    }
}

Kross::Api::Object* ScriptContainer::execute()
{
    if(! m_script) {
        Interpreter* interpreter = m_manager->getInterpreter(m_interpretername);
        if(! interpreter)
            throw Kross::Api::TypeException(i18n("Unknown interpreter on ScriptContainer::execute()."));
        setScript( interpreter->createScript(this) );
    }
    return m_script->execute();
}

Kross::Api::Object* ScriptContainer::callFunction()
{
    if(m_functionname.isEmpty())
        throw Kross::Api::RuntimeException(i18n("No functionname defined for ScriptContainer::callFunction()."));

    if(! m_script) // we need to initialize the script before.
        execute();

    return m_script->callFunction(m_functionname, m_functionargs);
}
