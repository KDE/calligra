/*
 *  This file is part of Calligra tests
 *
 *  SPDX-FileCopyrightText: 2011 Hanna Skott <hannaetscott@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "TestInputDevice.h"

#include "KoInputDevice.h"
#include <QTest>

// Tests so the KoInputDevice created is of tablet type
void TestInputDevice::testTabletConstructor()
{
    QInputDevice::DeviceType parameterTabletDevice = QInputDevice::DeviceType::Stylus;
    QPointingDevice::PointerType parameterPointerType = QPointingDevice::PointerType::Eraser;
    qint64 parameterUniqueTabletId = 0;
    KoInputDevice toTest(parameterTabletDevice, parameterPointerType, parameterUniqueTabletId);
    bool isMouse = toTest.isMouse();
    QVERIFY(!isMouse);
}
// Tests so the unique tablet ID of the default constructor is -1 like set.
void TestInputDevice::testNoParameterConstructor()
{
    KoInputDevice toTest;
    bool isMouse = toTest.isMouse();
    qint64 theUniqueTabletId = toTest.uniqueTabletId();
    qint64 toCompareWith = -1;
    QVERIFY(isMouse);
    QCOMPARE(theUniqueTabletId, toCompareWith);
}
// Tests so the single reference constructor can take both a tablet and a mouse as input device and set it correctly
void TestInputDevice::testConstructorWithSingleReference()
{
    QInputDevice::DeviceType parameterTabletDevice = QInputDevice::DeviceType::Stylus;
    QPointingDevice::PointerType parameterPointerType = QPointingDevice::PointerType::Eraser;
    qint64 parameterUniqueTabletId = 0;
    KoInputDevice parameterTabletDevice2(parameterTabletDevice, parameterPointerType, parameterUniqueTabletId);
    KoInputDevice toTest(parameterTabletDevice2);
    bool isMouse = toTest.isMouse();
    QVERIFY(!isMouse);
    KoInputDevice parameterMouseDevice;
    toTest = KoInputDevice(parameterMouseDevice);
    isMouse = toTest.isMouse();
    QVERIFY(isMouse);
}
// tests so the equality operator is working
void TestInputDevice::testEqualityCheckOperator()
{
    QInputDevice::DeviceType parameterTabletDevice = QInputDevice::DeviceType::Stylus;
    QPointingDevice::PointerType parameterPointerType = QPointingDevice::PointerType::Eraser;
    qint64 parameterUniqueTabletId = 0;
    KoInputDevice parameterTabletDevice2(parameterTabletDevice, parameterPointerType, parameterUniqueTabletId);
    KoInputDevice toTest(parameterTabletDevice2);
    KoInputDevice copy = toTest;
    KoInputDevice notSame;
    QVERIFY(toTest == copy);
    QVERIFY(toTest != notSame);
}
// tests that the device is set properly in the constructor
void TestInputDevice::testDevice()
{
    QInputDevice::DeviceType parameterTabletDevice = QInputDevice::DeviceType::Stylus;
    QPointingDevice::PointerType parameterPointerType = QPointingDevice::PointerType::Eraser;
    qint64 parameterUniqueTabletId = 0;
    KoInputDevice parameterTabletDevice2(parameterTabletDevice, parameterPointerType, parameterUniqueTabletId);
    KoInputDevice toTest(parameterTabletDevice2);
    auto aTabletDevice = toTest.device();

    QCOMPARE(aTabletDevice, QInputDevice::DeviceType::Stylus);
}
// tests that the PointerType is set correctly by the constructor
void TestInputDevice::testPointer()
{
    QInputDevice::DeviceType parameterTabletDevice = QInputDevice::DeviceType::Stylus;
    QPointingDevice::PointerType parameterPointerType = QPointingDevice::PointerType::Eraser;
    qint64 parameterUniqueTabletId = 0;
    KoInputDevice parameterTabletDevice2(parameterTabletDevice, parameterPointerType, parameterUniqueTabletId);
    KoInputDevice toTest(parameterTabletDevice2);
    auto aTabletDevice = toTest.pointer();

    QCOMPARE(aTabletDevice, QPointingDevice::PointerType::Eraser);
}
// verifies that the device returned by the mouse() function is indeed a mouse device
void TestInputDevice::testMouse()
{
    KoInputDevice toTest = KoInputDevice::mouse();
    bool isMouse = toTest.isMouse();
    QVERIFY(isMouse);
}
// verifies that the device returned by the stylus() function is indeed a tablet device
void TestInputDevice::testStylus()
{
    KoInputDevice toTest = KoInputDevice::stylus();
    bool isMouse = toTest.isMouse();
    QVERIFY(!isMouse);
}
// verifies that the device returned by the eraser() function is indeed a tablet device
void TestInputDevice::testEraser()
{
    KoInputDevice toTest = KoInputDevice::eraser();
    bool isMouse = toTest.isMouse();
    QVERIFY(!isMouse);
}

QTEST_MAIN(TestInputDevice)
