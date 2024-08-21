/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "TestResourceManager.h"

#include "KoCanvasResourceManager.h"
#include "KoPathShape.h"
#include "KoUnit.h"
#include <QSignalSpy>
#include <QTest>

void TestResourceManager::koShapeResource()
{
    KoPathShape *shape = new KoPathShape();
    int key = 9001;

    KoCanvasResourceManager rp(nullptr);
    rp.setResource(key, shape);
    QVERIFY(shape == rp.koShapeResource(key));
}

void TestResourceManager::testUnitChanged()
{
    KoCanvasResourceManager rm(nullptr);
    QSignalSpy spy(&rm, &KoCanvasResourceManager::canvasResourceChanged);

    KoUnit a;
    rm.setResource(KoCanvasResourceManager::Unit, a);
    QCOMPARE(spy.count(), 1);

    KoUnit b(KoUnit::Millimeter);
    rm.setResource(KoCanvasResourceManager::Unit, b);
    QCOMPARE(spy.count(), 2);
}

QTEST_MAIN(TestResourceManager)
