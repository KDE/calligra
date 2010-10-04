/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "TestStyleStorage.h"

#include <StyleStorage.h>
#include <Map.h>

#include <qtest_kde.h>

using namespace KSpread;

class MyStyleStorage : public StyleStorage
{
public:
    MyStyleStorage(Map* map) : StyleStorage(map) {}
    using StyleStorage::garbageCollection;
};

void TestStyleStorage::testGarbageCollection()
{
    Map map;
    MyStyleStorage storage(&map);

    QRect rect(5, 5, 1, 1);
    QColor c1(Qt::red);
    QColor c2(Qt::blue);
    SharedSubStyle style1(new SubStyleOne<Style::BackgroundColor, QColor>(c1));
    SharedSubStyle style2(new SubStyleOne<Style::BackgroundColor, QColor>(c2));
    // we need to do this for multiple cells, so hopefully we'll end up with substyles that are for the same cell but not in the same leafnode in the rtree
    for (int i = 0; i < 100; i++)
        storage.insert(rect.adjusted(10*i, 0, 10*i, 0), style1);
    for (int i = 0; i < 100; i++)
        QCOMPARE(storage.contains(rect.adjusted(10*i, 0, 10*i, 0)).backgroundColor(), c1);
    for (int i = 0; i < 100; i++)
        storage.insert(rect.adjusted(10*i, 0, 10*i, 0), style2);
    for (int i = 0; i < 100; i++)
        QCOMPARE(storage.contains(rect.adjusted(10*i, 0, 10*i, 0)).backgroundColor(), c2);
    for (int i = 0; i < 100; i++)
        storage.insert(rect.adjusted(10*i, 0, 10*i, 0), style1);
    for (int i = 0; i < 100; i++)
        QCOMPARE(storage.contains(rect.adjusted(10*i, 0, 10*i, 0)).backgroundColor(), c1);
    for (int j = 0; j < 1000; j++) {
        storage.garbageCollection();
        for (int i = 0; i < 100; i++)
            QCOMPARE(storage.contains(rect.adjusted(10*i, 0, 10*i, 0)).backgroundColor(), c1);
    }
}

QTEST_KDEMAIN(TestStyleStorage, GUI)

#include "TestStyleStorage.moc"
