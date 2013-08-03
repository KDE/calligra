/* This file is part of the KDE libraries
    Copyright (C) 2006 Olivier Goffart  <ogoffart at kde.org>

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
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "kconfigskeletontest.h"

#include <kconfig.h>
#include <QFont>
#include <QtTest/QtTestGui>


QTEST_MAIN( KConfigSkeletonTest)


#define DEFAULT_SETTING1 false
#define DEFAULT_SETTING2 QColor(1,2,3)
#define DEFAULT_SETTING3 QFont("helvetica",12)
#define DEFAULT_SETTING4 QString("Hello World")

#define WRITE_SETTING1 true
#define WRITE_SETTING2 QColor(3,2,1)
#define WRITE_SETTING3 QFont("helvetica",14)
#define WRITE_SETTING4 QString("KDE")

void KConfigSkeletonTest::initTestCase()
{
    setCurrentGroup("MyGroup");
    addItemBool("MySetting1", mMyBool, DEFAULT_SETTING1);
    addItemColor("MySetting2", mMyColor, DEFAULT_SETTING2);

    setCurrentGroup("MyOtherGroup");
    addItemFont("MySetting3", mMyFont, DEFAULT_SETTING3);
    addItemString("MySetting4", mMyString, DEFAULT_SETTING4);

    QCOMPARE(mMyBool, DEFAULT_SETTING1);
    QCOMPARE(mMyColor, DEFAULT_SETTING2);
    QCOMPARE(mMyFont, DEFAULT_SETTING3);
    QCOMPARE(mMyString, DEFAULT_SETTING4);
}

void KConfigSkeletonTest::testSimple()
{
    mMyBool = WRITE_SETTING1;
    mMyColor = WRITE_SETTING2;
    mMyFont = WRITE_SETTING3;
    mMyString = WRITE_SETTING4;

    writeConfig ();

    mMyBool = false;
    mMyColor = QColor();
    mMyString.clear();
    mMyFont = QFont();

    readConfig ();

    QCOMPARE(mMyBool, WRITE_SETTING1);
    QCOMPARE(mMyColor, WRITE_SETTING2);
    QCOMPARE(mMyFont, WRITE_SETTING3);
    QCOMPARE(mMyString, WRITE_SETTING4);

}

void KConfigSkeletonTest::testRemoveItem()
{
    QVERIFY(findItem("MySetting1"));
    removeItem("MySetting1");
    QVERIFY(!findItem("MySetting1"));
}

void KConfigSkeletonTest::testClear()
{
    QVERIFY(findItem("MySetting2"));
    QVERIFY(findItem("MySetting3"));
    QVERIFY(findItem("MySetting4"));

    clearItems();

    QVERIFY(!findItem("MySetting2"));
    QVERIFY(!findItem("MySetting3"));
    QVERIFY(!findItem("MySetting4"));
}

void KConfigSkeletonTest::testDefaults()
{
    setDefaults ();

    QCOMPARE(mMyBool, DEFAULT_SETTING1);
    QCOMPARE(mMyColor, DEFAULT_SETTING2);
    QCOMPARE(mMyFont, DEFAULT_SETTING3);
    QCOMPARE(mMyString, DEFAULT_SETTING4);

    writeConfig();
}


