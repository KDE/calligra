/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qdom.h>
#include <kstaticdeleter.h>
#include <gobjectfactory.h>

// these includes are needed to create the prototypes in the CTOR :)
// primitives
#include <gline.h>

// compound objects
#include <ggroup.h>


GObjectFactory *GObjectFactory::m_self=0L;

// As this belongs to a part we can't ignore static objects, but we have
// to clean up on unloading properly.
namespace Graphite {
static KStaticDeleter<GObjectFactory> gobjfact;
};

GObjectFactory *GObjectFactory::self() {

    if(m_self==0)
        m_self=Graphite::gobjfact.setObject(new GObjectFactory());
    return m_self;
}

void GObjectFactory::registerPrototype(const QString &classname, const GObject *prototype) {
    m_registry.insert(classname, prototype);
}

void GObjectFactory::unregisterPrototype(const QString &classname) {
    m_registry.remove(classname);
}

GObject *GObjectFactory::create(const QString &classname) {

    const GObject *tmp=m_registry.find(classname);
    if(tmp)
        return tmp->clone();
    else
        return 0L;
}

GObject *GObjectFactory::create(const QDomElement &element) {

    const GObject *tmp=m_registry.find(element.tagName());
    if(tmp) {
        GObject *ret=tmp->instantiate(element);
        if(ret->isOk())
            return ret;
        else {
            delete ret;
            return 0L;
        }
    }
    else
        return 0L;
}

GObjectFactory::GObjectFactory() : m_registry(17, false) {

    // set up the dict with all the types we are aware of
    // TODO: Change the size of the Hash Table to fit the
    // number of objects
    m_registry.setAutoDelete(true);

    // primitives
    registerPrototype(QString::fromLatin1("gline"),
                      new GLine(QString::fromLatin1("line")));

    // compound objects
    registerPrototype(QString::fromLatin1("ggroup"),
                      new GGroup(QString::fromLatin1("group")));
}
