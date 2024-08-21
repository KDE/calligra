/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "priorityqueue_test.h"
#include <PriorityQueue_p.h>
#include <QList>
#include <QTest>
#include <ctime>

struct Node {
    Node(unsigned int key)
        : m_key(key)
        , m_index(0)
    {
    }

    unsigned int key() const
    {
        return m_key;
    }
    void setKey(unsigned int key)
    {
        m_key = key;
    }

    int index() const
    {
        return m_index;
    }
    void setIndex(int i)
    {
        m_index = i;
    }

private:
    unsigned int m_key;
    int m_index;
};

static const char *const keys[] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "eleven", "twelve", nullptr};

void PriorityQueue_test::testQueue()
{
    QList<Node *> list;
    QHash<QByteArray, Node *> dict;

    CalligraFilter::PriorityQueue<Node> queue;

    srand(time(nullptr));
    for (int i = 0; i < 12; ++i) {
        Node *n = new Node(rand() % 20);
        list.append(n);
        queue.insert(n);
        Node *n2 = new Node(*n);
        dict.insert(keys[i], n2);
    }

    qDebug() << "##### Queue 1:";
    queue.dump();
    QCOMPARE((int)queue.count(), list.count());
    QCOMPARE(queue.isEmpty(), false);
    QCOMPARE(queue.extractMinimum()->index(), 0);

    qDebug() << "##### Queue 2:";
    CalligraFilter::PriorityQueue<Node> queue2(dict);
    // queue2.dump();

    Node *n = list.at(6);
    qDebug() << "##### Decreasing node:" << n->key() << " at" << n->index();
    n->setKey(2);
    queue.keyDecreased(n);
    queue.dump();

    n = list.at(2);
    qDebug() << "##### Decreasing node:" << n->key() << " at" << n->index();
    n->setKey(0);
    queue.keyDecreased(n);
    queue.dump();

    n = queue.extractMinimum();
    while (n) {
        queue.dump();
        n = queue.extractMinimum();
    }
}

QTEST_MAIN(PriorityQueue_test)
