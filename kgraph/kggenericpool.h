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

#ifndef kggenericpool_h
#define kggenericpool_h

#include <qlist.h>


template<class type> class KGGenericPool {

public:
    type *first() { return pool.first(); }
    type *last() { return pool.last(); }
    type *next() { return pool.next(); }
    type *prev() { return pool.prev(); }
    type *current() { return pool.current(); }
    type *at(const unsigned int &index) { return pool.at(index); }
    const int find(const type *object) { return pool.findRef(object); }

    virtual const bool remove(const unsigned int &index) { return pool.remove(index); }
    virtual const bool remove(const type *object) { return pool.removeRef(object); }

    const unsigned int count() const { return pool.count(); }
    const bool isEmpty() const { return pool.isEmpty(); }

protected:
    KGGenericPool() { pool.setAutoDelete(true); }
    virtual ~KGGenericPool() { pool.clear(); }

    QList<type> pool;

private:
    KGGenericPool &operator=(const KGGenericPool &rhs);
};
#endif // kggenericpool_h
