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

// The class which handles the memory management
// It simply stores all the objects in a list and
// deletes every element in the list if it gets destroyed.
// The order in this list also represents the Z-sorting for
// the chart's elements (a kind of "Painter's Algorithm").

#ifndef kgobjectpool_h
#define kgobjectpool_h

#include <qlist.h>
#include <kgobject.h>


class KGObjectPool {

public:
    static KGObjectPool *self();   // allow only one object pool!
    
    KGObject *first() { return objects.first(); }
    KGObject *last() { return objects.last(); }
    KGObject *next() { return objects.next(); }
    KGObject *prev() { return objects.prev(); }
    KGObject *current() { return objects.current(); }
    KGObject *at(const unsigned int &index) { return objects.at(index); }

    const bool remove(const unsigned int &index);
    const bool remove(const KGObject *object);

    const unsigned int count() const { return objects.count(); }
    const bool isEmpty() const { return objects.isEmpty(); }
    
    //KGLine *createLine(); // default line (from (0|0) to (1|1))
    //KGLine *createLine(const QDomElement &e);
    //KGLine *createLine(const QPoint &a, const QPoint &b);


protected:
    KGObjectPool();
    ~KGObjectPool();

private:
    QList<KGObject> objects;
    static KGObjectPool *m_self;
};
#endif // kgobjectpool_h
