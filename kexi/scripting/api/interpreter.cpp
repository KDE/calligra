/***************************************************************************
 * interpreter.cpp
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

#include "interpreter.h"

using namespace Kross::Api;

Interpreter::Interpreter()
{
}

Interpreter::~Interpreter()
{
    /*
    for(QMap<QString, Object*>::Iterator it = m_modules.begin(); it != m_modules.end(); ++it)
        delete it.data();
    */
}

bool Interpreter::hasModule(const QString& name)
{
    return m_modules.contains(name);
}

Object* Interpreter::getModule(const QString& name)
{
    return m_modules[name];
}

bool Interpreter::addModule(Object* module)
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

