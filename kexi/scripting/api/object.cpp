/***************************************************************************
 * object.cpp
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

#include "object.h"
#include "list.h"

#include <klocale.h>
#include <kdebug.h>

using namespace Kross::Api;

Object::Object(const QString& name, Object* parent)
    : m_name(name)
    , m_parent(parent)
{
#ifdef KROSS_API_OBJECT_DEBUG
    kdDebug() << QString("Kross::Api::Object::Constructor() name='%1'").arg(m_name) << endl;
#endif
}

Object::~Object()
{
#ifdef KROSS_API_OBJECT_DEBUG
    kdDebug() << QString("Kross::Api::Object::Destructor() name='%1'").arg(m_name) << endl;
#endif

    //removeAllChildren(); // not needed cause we use KShared to handle ref-couting and freeing.
}

const QString& Object::getName() const
{
    return m_name;
}

Object* Object::getParent() const
{
    return m_parent;
}

bool Object::hasChild(const QString& name) const
{
    return m_children.contains(name);
}

Object* Object::getChild(const QString& name) const
{
    return m_children[name];
}

ObjectMap Object::getChildren() const
{
    return m_children;
}

bool Object::addChild(const QString& name, Object* object, bool replace)
{
#ifdef KROSS_API_OBJECT_DEBUG
    kdDebug() << QString("Kross::Api::Object::addChild() name='%1' object.name='%2' object.classname='%3' replace='%4'")
        .arg(name).arg(object->getName()).arg(object->getClassName()).arg(replace) << endl;
#endif

    //if(! method) return false;
    if(! replace && m_children.contains(name)) return false;
    object->m_parent = this;
    m_children.replace(name, object);
    return true;
}

void Object::removeChild(const QString& name)
{
#ifdef KROSS_API_OBJECT_DEBUG
    kdDebug() << QString("Kross::Api::Object::removeChild() name='%1'").arg(name) << endl;
#endif
    m_children.remove(name);
}

void Object::removeAllChildren()
{
#ifdef KROSS_API_OBJECT_DEBUG
    kdDebug() << "Kross::Api::Object::removeAllChildren()" << endl;
#endif
    m_children.clear();
}

Object* Object::call(const QString&, List*)
{
#ifdef KROSS_API_OBJECT_DEBUG
    kdDebug() << "Kross::Api::Object::call() on not callable object." << endl;
#endif
    return 0;
}

