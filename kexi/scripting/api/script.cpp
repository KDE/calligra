/***************************************************************************
 * script.cpp
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

#include "script.h"
#include "../api/object.h"
#include "../api/list.h"
#include "../main/manager.h"

#include "../api/interpreter.h"

using namespace Kross::Api;

Script::Script(Manager* manager)
    : QObject()
    , m_manager(manager)
{
}

Script::~Script()
{
}

const QString& Script::getCode()
{
    return m_code;
}

void Script::setCode(const QString& code)
{
    m_code = code;
}

const QString& Script::getInterpreter()
{
    return m_interpreter;
}

void Script::setInterpreter(const QString& interpreter)
{
    m_interpreter = interpreter;
}

bool Script::execute()
{
    Interpreter* interpreter = m_manager->getInterpreter(m_interpreter);
    if(! interpreter) return false;
    return interpreter->execute(m_code);
}

const QVariant& Script::execute(const QString& name, const QVariant& args)
{
    //TODO
}

Kross::Api::Object* Script::execute(const QString& name, Kross::Api::List* args)
{
    Interpreter* interpreter = m_manager->getInterpreter(m_interpreter);
    if(! interpreter) return false;
    return interpreter->execute(m_code, name, args);
}

