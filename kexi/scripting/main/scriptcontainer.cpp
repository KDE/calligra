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
#include "../api/signalhandler.h"

using namespace Kross::Api;

ScriptContainer::ScriptContainer(Manager* manager, const QString& name)
    : QObject()
    , m_manager(manager)
    , m_name(name)
    , m_script(0)
    , m_signalhandler(0)
{
}

ScriptContainer::~ScriptContainer()
{
    delete m_signalhandler;
    finalize();
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
    finalize();
    m_code = code;
}

const QString& ScriptContainer::getInterpreterName()
{
    return m_interpretername;
}

void ScriptContainer::setInterpreterName(const QString& name)
{
    finalize();
    m_interpretername = name;
}

const QStringList& ScriptContainer::getFunctionNames()
{
    return m_script ? m_script->getFunctionNames() : QStringList();
}

void ScriptContainer::initialize()
{
    finalize();
    Interpreter* interpreter = m_manager->getInterpreter(m_interpretername);
    if(! interpreter)
        throw Kross::Api::TypeException(i18n("Unknown interpreter on ScriptContainer::execute()."));
    m_script = interpreter->createScript(this);
}

void ScriptContainer::finalize()
{
    delete m_script; m_script = 0;
}

Kross::Api::Object* ScriptContainer::execute()
{
    if(! m_script) initialize();
    return m_script->execute();
}

Kross::Api::Object* ScriptContainer::callFunction(const QString& functionname, Kross::Api::List* arguments)
{
    if(functionname.isEmpty())
        throw Kross::Api::RuntimeException(i18n("No functionname defined for ScriptContainer::callFunction()."));

    if(! m_script) initialize();
    return m_script->callFunction(functionname, arguments);
}

bool ScriptContainer::connect(QObject *sender, const char *signal, const QString& functionname)
{
    if(! m_signalhandler) // create instance on demand
        m_signalhandler = new SignalHandler(this);
    return m_signalhandler->connect(sender, signal, functionname);
}

bool ScriptContainer::disconnect(QObject *sender, const char *signal, const QString& functionname)
{
    if(! m_signalhandler) return false;
    return m_signalhandler->disconnect(sender, signal, functionname);
}

