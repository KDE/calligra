/* This file is part of the KDE project
   Copyright (C) 2001 Werner Trobin <trobin@kde.org>

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

#include <priorityqueue.h>
#include <kdebug.h>
#include <qptrlist.h>
#include <stdlib.h>
#include <time.h>

struct Node {
    Node(unsigned int key) : m_key(key), m_index(0) {}

    unsigned int key() const { return m_key; }
    void setKey(unsigned int key) { m_key=key; }

    int index() const { return m_index; }
    void setIndex(int i) { m_index=i; }
private:
    unsigned int m_key;
    int m_index;
};

int main(int /*argc*/, char **/*argv*/) {

    QPtrList<Node> list;
    list.setAutoDelete(true);
    KOffice::PriorityQueue<Node> queue;

    srand(time(0));
    for(int i=0; i<12; ++i) {
        Node *n=new Node(rand()%20);
        list.append(n);
        queue.insert(n);
    }
    queue.dump();

    Node *n=list.at(6);
    kdDebug() << "Decreasing node: " << n->key() << " at " << n->index() << endl;
    n->setKey(2);
    queue.keyDecreased(n);
    queue.dump();

    n=list.at(2);
    kdDebug() << "Decreasing node: " << n->key() << " at " << n->index() << endl;
    n->setKey(0);
    queue.keyDecreased(n);
    queue.dump();

    n=queue.extractMinimum();
    while(n) {
        queue.dump();
        n=queue.extractMinimum();
    }
    return 0;
}
