/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@mandrakesoft.com>

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
#include <gobjectfactory.h>

// these includes are needed to create the prototypes in the CTOR :)
#include <gline.h>


GObjectFactory::GObjectFactory() : m_registry(17, false) {

    // set up the dict with all the types we are aware of
    m_registry.setAutoDelete(true);
    registerPrototype("gline", new GLine("line"));
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
