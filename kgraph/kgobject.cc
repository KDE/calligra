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

#include <kgobject.h>
#include <kggroup.h>
#include <kgobjectpool.h>


KGObject::~KGObject() {

    if(m_group!=0L)
	m_group->removeMember(this);
    if(tmpGroup!=0L)
	tmpGroup->removeMember(this);
}

QDomElement KGObject::save(QDomDocument &doc) const {

    QDomElement e=doc.createElement("kgobject");
    e.setAttribute("name", m_name);
    e.setAttribute("originX", m_origin.x());
    e.setAttribute("originY", m_origin.y());
    // TODO
    return e;
}

const bool KGObject::setTemporaryGroup(KGGroup *group) {

    if(group==m_group)
	return false;
    group->addMember(this);
    tmpGroup=group;
    return true;
}

const bool KGObject::setGroup(KGGroup *group) {

    if(group==tmpGroup)
	return false;
    group->addMember(this);
    m_group=group;
    return true;
}

KGObject::KGObject(const KGObjectPool * const pool, const QString &name) :
    QObject(0L, name.local8Bit()), m_pool(pool), m_name(name) {
}

KGObject::KGObject(const KGObject &rhs) : QObject(0L, rhs.name().local8Bit()),
					  m_pool(rhs.pool()), m_name(rhs.name()) {
    m_origin=rhs.origin();
    // TODO
}

KGObject::KGObject(const KGObjectPool * const pool, const QDomElement &/*element*/) :
    QObject(), m_pool(pool) {
    // TODO (don't forget so set the QObject name!)
}
#include <kgobject.moc>
