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

#include <kgobjectpool.h>

KGObjectPool *KGObjectPool::m_self=0L;

KGObjectPool *KGObjectPool::self() {
    if(m_self==0L)
        m_self=new KGObjectPool;
    return m_self;
}

KGObjectPool::KGObjectPool() : KGGenericPool<KGObject>() {
}

const bool KGObjectPool::remove(const unsigned int &index) {
    // if the object is member of a group, tell the
    // group to remove it (TODO)
    return pool.remove(index);
}

const bool KGObjectPool::remove(const KGObject *object) {
    // if the object is member of a group, tell the
    // group to remove it (TODO)
    return pool.removeRef(object);
}
