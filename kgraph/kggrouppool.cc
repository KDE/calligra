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

KGGroupPool *KGGroupPool::m_self=0L;

KGGroupPool *KGGroupPool::self() {
    if(m_self==0L)
        m_self=new KGGroupPool;
    return m_self;
}

KGGroupPool::KGGroupPool() {
    groups.setAutoDelete(true);
}

const bool KGGroupPool::remove(const unsigned int &index) {
    // tell the objects that this group will be deleted (TODO)
    return groups.remove(index);
}

const bool KGGroupPool::remove(const KGGroup *group) {
    // tell the objects that this group will be deleted (TODO)
    return groups.remove(group);
}
