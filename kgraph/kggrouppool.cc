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
#include <kggrouppool.h>


KGGroupPool::KGGroupPool() : KGGenericPool<KGGroup>() {
}

QDomElement KGGroupPool::save(QDomDocument &doc) {

    QDomElement e=doc.createElement("groups");
    // TODO: Save all groups
    return e;
}

KGGroup *KGGroupPool::find(const int &id) const {

    for(QListIterator<KGGroup> it(pool); it.current(); ++it) {
	if(it.current()->id()==id)
	    return it.current();
    }
    return 0L;
}

const bool KGGroupPool::remove(const unsigned int &index) {
    return pool.remove(index);
}

const bool KGGroupPool::remove(const KGGroup *group) {
    return pool.removeRef(group);
}

void KGGroupPool::add(const KGGroup *group) {
    pool.append(group);
}

const KGGroup *KGGroupPool::createGroup(const QDomElement &element) {

    KGGroup *tmp=new KGGroup(element);
    pool.append(tmp);
    return tmp;
}
