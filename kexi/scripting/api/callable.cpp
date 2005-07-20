/***************************************************************************
 * callable.cpp
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

#include "callable.h"

#include <klocale.h>
//#include <kdebug.h>

using namespace Kross::Api;

Callable::Callable(const QString& name, Object::Ptr parent, ArgumentList arglist, const QString& documentation)
    : Object(name, parent)
    , m_arglist(arglist)
    , m_documentation(documentation)
{
}

Callable::~Callable()
{
}

const QString Callable::getClassName() const
{
    return "Kross::Api::Callable";
}

const QString Callable::getDescription() const
{
    return m_documentation;
}

void Callable::checkArguments(KSharedPtr<List> arguments)
{
    QValueList<Object::Ptr>& arglist = arguments->getValue();
    uint fmax = m_arglist.getMaxParams();
    uint fmin = m_arglist.getMinParams();

    // check the number of parameters passed.
    if(arglist.size() < fmin)
        throw AttributeException(i18n("Too few parameters for callable object '%1'.").arg(getName()));
    if(arglist.size() > fmax)
        throw AttributeException(i18n("Too many parameters for callable object '%1'.").arg(getName()));

    // check type of passed parameters.
    QValueList<Argument> farglist = m_arglist.getArguments();
    for(uint i = 0; i < fmax; i++) {
        if(i >= arglist.count()) { // handle default arguments
            arglist.append( farglist[i].getObject() );
            continue;
        }
        Object::Ptr o = arguments->item(i);
        QString fcn = farglist[i].getClassName();
        QString ocn = o->getClassName();

        //FIXME
        //e.g. on 'Kross::Api::Variant::String' vs. 'Kross::Api::Variant'
        //We should add those ::String part even to the arguments in Kross::KexiDB::*

        if(fcn.find(ocn) != 0)
            throw AttributeException(i18n("Callable object '%1' expected parameter of type '%1', but got '%2'.").arg(getName()).arg(fcn).arg(ocn));
    }
}
