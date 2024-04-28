// This file is part of the KDE project
// SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#include "TestStyleStorage.h"

#include <core/Map.h>
#include <core/StyleStorage.h>

#include <QTest>

using namespace Calligra::Sheets;

class MyStyleStorage : public StyleStorage
{
public:
    MyStyleStorage(Map *map)
        : StyleStorage(map)
    {
    }
    using StyleStorage::garbageCollection;
};

void TestStyleStorage::testGarbageCollection()
{
    Map map;
    MyStyleStorage storage(&map);

    QRect rect(5, 5, 1, 1);
    QColor c1(Qt::red);
    QColor c2(Qt::blue);
    SharedSubStyle style1(new SubStyleOne<QColor>(Style::BackgroundColor, c1));
    SharedSubStyle style2(new SubStyleOne<QColor>(Style::BackgroundColor, c2));
    // we need to do this for multiple cells, so hopefully we'll end up with substyles that are for the same cell but not in the same leafnode in the rtree
    for (int i = 0; i < 100; i++)
        storage.insert(rect.adjusted(10 * i, 0, 10 * i, 0), style1);
    for (int i = 0; i < 100; i++)
        QCOMPARE(storage.contains(rect.adjusted(10 * i, 0, 10 * i, 0)).backgroundColor(), c1);
    for (int i = 0; i < 100; i++)
        storage.insert(rect.adjusted(10 * i, 0, 10 * i, 0), style2);
    for (int i = 0; i < 100; i++)
        QCOMPARE(storage.contains(rect.adjusted(10 * i, 0, 10 * i, 0)).backgroundColor(), c2);
    for (int i = 0; i < 100; i++)
        storage.insert(rect.adjusted(10 * i, 0, 10 * i, 0), style1);
    for (int i = 0; i < 100; i++)
        QCOMPARE(storage.contains(rect.adjusted(10 * i, 0, 10 * i, 0)).backgroundColor(), c1);
    for (int j = 0; j < 1000; j++) {
        storage.garbageCollection();
        for (int i = 0; i < 100; i++)
            QCOMPARE(storage.contains(rect.adjusted(10 * i, 0, 10 * i, 0)).backgroundColor(), c1);
    }
}

QTEST_MAIN(TestStyleStorage)
