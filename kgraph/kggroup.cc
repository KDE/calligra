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

#include <kggroup.h>
#include <qdom.h>

int KGGroup::ID=0;

KGGroup::KGGroup() : m_id(++ID), m_active(true) {
}

KGGroup::KGGroup(const QDomElement &element) {

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
    members.clear();
}

const QDomElement KGGroup::save(QDomDocument &document) {

    QDomElement element=document.createElement("group");
    element.setAttribute("id", m_id);
    element.setAttribute("active", m_active);
    return element;
}

void KGGroup::addMember(KGObject *member) {
    if(!members.findRef(member))
	members.append(member);
}

void KGGroup::removeMember(KGObject *member) {
    members.removeRef(member);
}
