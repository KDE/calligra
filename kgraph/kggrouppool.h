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

// The class which handles the memory management for the
// groups.
// It simply stores all the groups in a list and
// deletes every group in the list if it gets destroyed.
// Every group has got a unique ID (ensured by the group's
// CTOR!).

#ifndef kggrouppool_h
#define kggrouppool_h

#include <qlist.h>

#include <kggenericpool.h>
#include <kggroup.h>


class KGGroupPool : public KGGenericPool<KGGroup> {

public:
    KGGroupPool();
    virtual ~KGGroupPool() {}    

    KGGroup *find(const int &id);  // find the group via its ID

    void add(const KGGroup *group);
    virtual const bool remove(const unsigned int &index);
    virtual const bool remove(const KGGroup *group);

    const KGGroup *createGroup(const QDomElement &element);    

private:
    KGGroupPool &operator=(const KGGroupPool &rhs);
};
#endif // kggrouppool_h
