/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@carinthia.com>

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

#include <kdebug.h>
#include <kggroup.h>

int KGGroup::ID=0;

KGGroup::KGGroup() : m_id(++ID), m_active(true), m_exclusive(true),
		     m_exclCache(false) {
}

KGGroup::KGGroup(const QDomElement &element) : m_exclusive(true),
					       m_exclCache(false) {
    bool ok;
    m_id=element.attribute("id").toInt(&ok);
    if(!ok)
	m_id=++ID;

    int tmp=element.attribute( "active" ).toInt(&ok);
    if(!ok || tmp!=0)
	m_active=true;
    else
	m_active=false;
}

KGGroup::~KGGroup() {

    for(QListIterator<KGObject> it(members); it.current(); ++it) {
	if(it.current()->group()==this)
	    it.current()->setGroup(0L);
	else if(it.current()->temporaryGroup()==this)
	    it.current()->setTemporaryGroup(0L);
	else
	    kdWarning(37001) << "KGGroup::~KGGroup(): Member had no ptr to this group!" << endl;
    }
    members.clear();
}

const bool KGGroup::isExclusive() {

    if(m_exclCache)
	return m_exclusive;

    m_exclusive=true;
    m_exclCache=true;
    QListIterator<KGObject> it(members);
    const char *firstName=it.current()->className();
    ++it;
    for( ; it.current() && m_exclusive; ++it) {
	if(strcmp(firstName, it.current()->className())!=0) {
	    m_exclusive=false;
	}
    }
    return m_exclusive;
}

const QDomElement KGGroup::save(QDomDocument &document) {

    QDomElement element=document.createElement("group");
    element.setAttribute("id", m_id);
    element.setAttribute("active", m_active);
    return element;
}

void KGGroup::addMember(KGObject *member) {

    if(!members.findRef(member)) {
	members.append(member);
	m_exclCache=false;
    }
}

void KGGroup::removeMember(KGObject *member) {
    members.removeRef(member);
}

const bool KGGroup::changeProperty(const char *property, const QVariant &value,
				   const KGObject *object) {
    if(!m_active)
	return false;

    bool ok=false;
    QListIterator<KGObject> it(members);

    // propagate it to all objects (if possible)
    if(object==0L) {
	for( ; it.current(); ++it) {
	    if(it.current()->setProperty(property, value))
		ok=true;  // ok, at least one successful change :)
	}
    }
    // propagate it only to one type of objects (e.g. KGPolygon)
    else {
	const char *name=object->className();
	for( ; it.current(); ++it) {
	    if(strcmp(name, it.current()->className())==0 && it.current()->setProperty(property, value))
		ok=true;  // ok, at least one successful change :)
	}
    }
    return ok;
}
