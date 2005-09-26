/***************************************************************************
 * interpreter.cpp
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

#include "interpreter.h"
#include "script.h"
#include "../main/manager.h"
#include "../main/scriptcontainer.h"

using namespace Kross::Api;

Interpreter::Interpreter(const QString& interpretername)
    : m_interpretername(interpretername)
{
}

Interpreter::~Interpreter()
{
    for(Option::Map::Iterator it = m_options.begin(); it != m_options.end(); ++it)
        delete it.data();
}

const QVariant& Interpreter::getOption(const QString name, const QVariant& defaultvalue)
{
    Option* opt = m_options[name];
    return opt ? opt->m_value : defaultvalue;
}

Interpreter::Option::Map Interpreter::getOptions()
{
    return m_options;
}

bool Interpreter::setOption(const QString name, const QVariant& value)
{
    Option* opt = m_options[name];
    if(! opt)
        return false;
    opt->m_value = value;
    return true;
}

const QString& Interpreter::getInterpretername()
{
    return m_interpretername;
}

