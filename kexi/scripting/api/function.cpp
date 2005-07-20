/***************************************************************************
 * function.cpp
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

#include "function.h"

using namespace Kross::Api;

Function::Function(const QString& name, ArgumentList arglist, const QString& documentation)
    : Object(name)
    , m_arglist(arglist)
    , m_documentation(documentation)
{
}

Function::~Function()
{
}

const QString Function::getClassName() const
{
    return "Kross::Api::Function";
}

const QString Function::getDescription() const
{
    return m_documentation;
}

