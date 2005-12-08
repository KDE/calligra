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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "scriptcontainer.h"
#include "../api/object.h"
#include "../api/list.h"
#include "../api/interpreter.h"
#include "../api/script.h"
#include "../main/manager.h"
#include "mainmodule.h"

using namespace Kross::Api;

namespace Kross { namespace Api {

    /// @internal
    class ScriptContainerPrivate
    {
        public:

            /**
            * The \a Script instance the \a ScriptContainer uses
            * if initialized. It will be NULL as long as we
            * didn't initialized it what will be done on
            * demand.
            */
            Script* m_script;

            /**
            * The unique name the \a ScriptContainer is
            * reachable as.
            */
            QString m_name;

            /**
            * The scripting code.
            */
            QString m_code;

            /**
            * The name of the interpreter. This could be
            * something like "python" for the python
            * binding.
            */
            QString m_interpretername;

            /**
            * Map of options that overwritte the \a InterpreterInfo::Option::Map
            * standard options.
            */
            QMap<QString, QVariant> m_options;

    };

}}

ScriptContainer::ScriptContainer(const QString& name)
    : MainModule(name)
    , d( new ScriptContainerPrivate() ) // initialize d-pointer class
{
    kdDebug() << QString("ScriptContainer::ScriptContainer() Ctor name='%1'").arg(name) << endl;

    d->m_script = 0;
    d->m_name = name;
}

ScriptContainer::~ScriptContainer()
{
    kdDebug() << QString("ScriptContainer::~ScriptContainer() Dtor name='%1'").arg(d->m_name) << endl;

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

const QVariant& ScriptContainer::getOption(const QString name, QVariant defaultvalue, bool /*recursive*/)
{
kdDebug()<<"############################# getOption name="<<name<<" value="<<d->m_options[name]<<endl;
    if(d->m_options.contains(name))
        return d->m_options[name];
    Kross::Api::InterpreterInfo* info = Kross::Api::Manager::scriptManager()->getInterpreterInfo( d->m_interpretername );
    return info ? info->getOptionValue(name, defaultvalue) : defaultvalue;
}

void ScriptContainer::setOption(const QString name, const QVariant& value)
{
kdDebug()<<"############################# setOption name="<<name<<" value="<<value<<endl;
    d->m_options.replace(name, value);
}

Object::Ptr ScriptContainer::execute()
{
    if(! d->m_script)
        if(! initialize())
            return 0;

    if(hadException())
        return 0;

    Object::Ptr r = d->m_script->execute();
    if(d->m_script->hadException()) {
        setException( d->m_script->getException() );
        finalize();
        return 0;
    }
    return r;
}

const QStringList ScriptContainer::getFunctionNames()
{
    return d->m_script ? d->m_script->getFunctionNames() : QStringList(); //FIXME init before if needed?
}

Object::Ptr ScriptContainer::callFunction(const QString& functionname, List::Ptr arguments)
{
    if(! d->m_script)
        if(! initialize())
            return 0;

    if(hadException())
        return 0;

    if(functionname.isEmpty()) {
        setException( new Exception(QString("No functionname defined for ScriptContainer::callFunction().")) );
        finalize();
        return 0;
    }

    Object::Ptr r = d->m_script->callFunction(functionname, arguments);
    if(d->m_script->hadException()) {
        setException( d->m_script->getException() );
        finalize();
        return 0;
    }
    return r;
}

const QStringList ScriptContainer::getClassNames()
{
    return d->m_script ? d->m_script->getClassNames() : QStringList(); //FIXME init before if needed?
}

Object::Ptr ScriptContainer::classInstance(const QString& name)
{
    if(! d->m_script)
        if(! initialize())
            return 0;

    if(hadException())
        return 0;

    Object::Ptr r = d->m_script->classInstance(name);
    if(d->m_script->hadException()) {
        setException( d->m_script->getException() );
        finalize();
        return 0;
    }
    return r;
}

bool ScriptContainer::initialize()
{
    finalize();
    Interpreter* interpreter = Manager::scriptManager()->getInterpreter(d->m_interpretername);
    if(! interpreter) {
        setException( new Exception(QString("Unknown interpreter '%1'").arg(d->m_interpretername)) );
        return false;
    }
    d->m_script = interpreter->createScript(this);
    if(! d->m_script) {
        setException( new Exception(QString("Failed to create script for interpreter '%1'").arg(d->m_interpretername)) );
        return false;
    }
    if(d->m_script->hadException()) {
        setException( d->m_script->getException() );
        finalize();
        return false;
    }
    setException( 0 ); // clear old exception
    return true;
}

void ScriptContainer::finalize()
{
    delete d->m_script;
    d->m_script = 0;
}


