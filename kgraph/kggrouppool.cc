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

#include <kggrouppool.h>


KGGroupPool::KGGroupPool() : KGGenericPool<KGGroup>() {
}

KGGroup *KGGroupPool::find(const int &id) {

    for(KGGroup *tmp=pool.first(); tmp!=0L; tmp=pool.next()) {
	if(tmp->id()==id)
	    return tmp;
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
