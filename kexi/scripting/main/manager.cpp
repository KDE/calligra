/***************************************************************************
 * manager.cpp
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

//#include "api/object.h"
#include "../api/interpreter.h"
//#include "python/pythonextension.h"
//#include "python/pythonmodule.h"
#include "../python/pythoninterpreter.h"
#include "../kjs/kjsinterpreter.h"

using namespace Kross::Api;

Manager::Manager()
{
}

Manager::~Manager()
{
    /*
    for(QMap<QString, Interpreter*>::Iterator it = m_interpreter.begin(); it != m_interpreter.end(); ++it)
        delete it.data();
    */
}

Interpreter* Manager::getInterpreter(const QString& interpretername)
{
    if(m_interpreter.contains(interpretername))
        return m_interpreter[interpretername];
    Interpreter* interpreter = 0;

    if(interpretername == "kjs")
        interpreter = new Kross::Kjs::KjsInterpreter();
    else if(interpretername == "python")
        interpreter = new Kross::Python::PythonInterpreter();

    if(interpreter)
        m_interpreter.replace(interpretername, interpreter);
    return interpreter;
}

