/***************************************************************************
 * list.cpp
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

#include "list.h"
#include "exception.h"

#include <kdebug.h>

using namespace Kross::Api;

List::List(QValueList<Object::Ptr> value, const QString& name)
    : Value< List, QValueList<Object::Ptr> >(value, name)
{
}

List::~List()
{
}

const QString List::getClassName() const
{
    return "Kross::Api::List";
}

const QString List::toString()
{
    QString s = "[";
    QValueList<Object::Ptr> list = getValue();
    for(QValueList<Object::Ptr>::Iterator it = list.begin(); it != list.end(); ++it)
        s += "'" + (*it)->toString() + "', ";
    return (s.endsWith(", ") ? s.left(s.length() - 2) : s) + "]";
}

Object::Ptr List::item(uint idx)
{
    QValueList<Object::Ptr> list = getValue();
    if(idx >= list.count()) {
        kdDebug() << "List::item index=" << idx << " is out of bounds. Raising TypeException." << endl;
        throw Exception::Ptr( new Exception(QString("List-index %1 out of bounds.").arg(idx)) );
    }
    Object::Ptr obj = list[idx];
    return obj;
}

uint List::count()
{
    QValueList<Object::Ptr> list = getValue();
    return list.count();
}

void List::append(Object::Ptr object)
{
    QValueList<Object::Ptr> list = getValue();
    list.append(object);
}

