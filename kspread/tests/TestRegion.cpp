/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "TestRegion.h"

#include "qtest_kde.h"

#include "Limits.h"
#include "Map.h"
#include "Region.h"
#include "Sheet.h"

using namespace KSpread;

void TestRegion::initTestCase()
{
    m_map = new Map(0 /* no Doc*/);
    Sheet* sheet = m_map->addNewSheet();
    sheet->setSheetName("Sheet1");
    sheet = m_map->addNewSheet();
    sheet->setSheetName("Sheet2");
    sheet = m_map->addNewSheet();
    sheet->setSheetName("Sheet3");
    sheet = m_map->addNewSheet();
    sheet->setSheetName("Sheet 4");
}

void TestRegion::testComparison()
{
    Region region1;
    Region region2;
    region1 = Region("A1");
    region2 = Region("A1");
    QVERIFY(region1 == region2);
    region1 = Region("A1:A5");
    region2 = Region("A1:A5");
    QVERIFY(region1 == region2);
    region1 = Region("A1:A5;B4");
    region2 = Region("A1:A5;B4");
    QVERIFY(region1 == region2);
    region2 = Region("A1");
    QVERIFY(region1 != region2);
    region2 = Region("A1:A5");
    QVERIFY(region1 != region2);
}

void TestRegion::testFixation()
{
    Region region;
    region = Region("$A1", m_map, m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!$A1"));
    region = Region("A$1", m_map, m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A$1"));
    region = Region("$A$1", m_map, m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!$A$1"));
    region = Region("$A1:B4", m_map, m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!$A1:B4"));
    region = Region("A$1:B4", m_map, m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A$1:B4"));
    region = Region("$A$1:B4", m_map, m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!$A$1:B4"));
    region = Region("A1:$B4", m_map, m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A1:$B4"));
    region = Region("A1:B$4", m_map, m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A1:B$4"));
    region = Region("A1:$B$4", m_map, m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A1:$B$4"));
}

void TestRegion::testSheet()
{
    Region region;
    region = Region(QPoint(1, 1), m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A1"));
    QCOMPARE(region.firstSheet(), m_map->sheet(0));
    region = Region("A1");
    QCOMPARE(region.name(), QString("A1"));
    QCOMPARE(region.firstSheet(), (Sheet*)0);
    region = Region("A1", m_map, m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A1"));
    QCOMPARE(region.firstSheet(), m_map->sheet(0));
    region = Region("Sheet1!A1", m_map, m_map->sheet(1));
    QCOMPARE(region.name(), QString("Sheet1!A1"));
    QCOMPARE(region.firstSheet(), m_map->sheet(0));
    region = Region("Sheet2!A1", m_map);
    QCOMPARE(region.name(), QString("Sheet2!A1"));
    QCOMPARE(region.firstSheet(), m_map->sheet(1));
    region = Region("Sheet2!A1", m_map, m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet2!A1"));
    QCOMPARE(region.firstSheet(), m_map->sheet(1));
    region = Region("Sheet 4!A1", m_map, m_map->sheet(0));
    QCOMPARE(region.name(), QString("'Sheet 4'!A1"));
    QCOMPARE(region.firstSheet(), m_map->sheet(3));
    region = Region("'Sheet 4'!A1", m_map, m_map->sheet(0));
    QCOMPARE(region.name(), QString("'Sheet 4'!A1"));
    QCOMPARE(region.firstSheet(), m_map->sheet(3));
    // invalid calls:
    region = Region("!A1", m_map, m_map->sheet(0));
    QVERIFY(region.isEmpty());
    region = Region("Sheet99!A1", m_map, m_map->sheet(0));
    QVERIFY(region.isEmpty());
}

void TestRegion::testExtrem()
{
    Region region1 = Region(QPoint(-1, -1), m_map->sheet(0));
    QVERIFY(region1.isEmpty());
    QVERIFY(!region1.isValid());

    Region region2 = Region("A1:A6553634523563453456356");
    QVERIFY(region2.isValid());

    Region region3 = Region(QRect(1,1,KS_colMax,KS_rowMax), m_map->sheet(0));
    QVERIFY(region3.isValid());

    Region region4 = Region(QRect(1,1,KS_colMax,KS_rowMax), m_map->sheet(0));
    QVERIFY(region4.isValid());
    Region region5 = Region(QRect(1,1,KS_colMax+12345,KS_rowMax+12345), m_map->sheet(0));
    QVERIFY(region5.isValid());
    QCOMPARE(region4.name(), region5.name());
    
    Region region6 = Region(QPoint(KS_colMax, KS_rowMax), m_map->sheet(0));
    QVERIFY(region6.isValid());
    Region region7 = Region(QPoint(KS_colMax+22, KS_rowMax+22), m_map->sheet(0));
    QVERIFY(region7.isValid());
    QCOMPARE(region6.name(), region7.name());
}

void TestRegion::cleanupTestCase()
{
    delete m_map;
}

QTEST_KDEMAIN(TestRegion, GUI)

#include "TestRegion.moc"
