// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-License-Identifier: LGPL-2.0-only

#include "TestRegion.h"

#include <KLocalizedString>
#include <QTest>

#include "engine/MapBase.h"
#include "engine/Region.h"
#include "engine/SheetBase.h"
#include "engine/calligra_sheets_limits.h"

using namespace Calligra::Sheets;

void TestRegion::initTestCase()
{
    KLocalizedString::setApplicationDomain("calligrasheets");
    m_map = new MapBase;
    SheetBase *sheet = m_map->addNewSheet();
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
    region1 = m_map->regionFromName("A1");
    region2 = m_map->regionFromName("A1");
    QVERIFY(region1 == region2);
    region1 = m_map->regionFromName("A1:A5");
    region2 = m_map->regionFromName("A1:A5");
    QVERIFY(region1 == region2);
    region1 = m_map->regionFromName("A1:A5;B4");
    region2 = m_map->regionFromName("A1:A5;B4");
    QVERIFY(region1 == region2);
    region2 = m_map->regionFromName("A1");
    QVERIFY(region1 != region2);
    region2 = m_map->regionFromName("A1:A5");
    QVERIFY(region1 != region2);
}

void TestRegion::testFixation()
{
    Region region;
    region = m_map->regionFromName("$A1", m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!$A1"));
    region = m_map->regionFromName("A$1", m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A$1"));
    region = m_map->regionFromName("$A$1", m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!$A$1"));
    region = m_map->regionFromName("$A1:B4", m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!$A1:B4"));
    region = m_map->regionFromName("A$1:B4", m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A$1:B4"));
    region = m_map->regionFromName("$A$1:B4", m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!$A$1:B4"));
    region = m_map->regionFromName("A1:$B4", m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A1:$B4"));
    region = m_map->regionFromName("A1:B$4", m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A1:B$4"));
    region = m_map->regionFromName("A1:$B$4", m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A1:$B$4"));
}

void TestRegion::testSheet()
{
    Region region;
    region = Region(QPoint(1, 1), m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A1"));
    QCOMPARE(region.firstSheet(), m_map->sheet(0));
    region = m_map->regionFromName("A1");
    QCOMPARE(region.name(), QString("A1"));
    QCOMPARE(region.firstSheet(), nullptr);
    region = m_map->regionFromName("A1", m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A1"));
    QCOMPARE(region.firstSheet(), m_map->sheet(0));
    region = m_map->regionFromName("Sheet1!A1", m_map->sheet(1));
    QCOMPARE(region.name(), QString("Sheet1!A1"));
    QCOMPARE(region.firstSheet(), m_map->sheet(0));
    region = m_map->regionFromName("Sheet2!A1");
    QCOMPARE(region.name(), QString("Sheet2!A1"));
    QCOMPARE(region.firstSheet(), m_map->sheet(1));
    region = m_map->regionFromName("Sheet2!A1", m_map->sheet(0));
    QCOMPARE(region.name(), QString("Sheet2!A1"));
    QCOMPARE(region.firstSheet(), m_map->sheet(1));
    region = m_map->regionFromName("Sheet 4!A1", m_map->sheet(0));
    QCOMPARE(region.name(), QString("'Sheet 4'!A1"));
    QCOMPARE(region.firstSheet(), m_map->sheet(3));
    region = m_map->regionFromName("'Sheet 4'!A1", m_map->sheet(0));
    QCOMPARE(region.name(), QString("'Sheet 4'!A1"));
    QCOMPARE(region.firstSheet(), m_map->sheet(3));
    // Multiple quotas should be compressed, use-case that
    // was visible in the xls from bug 284325.
    region = m_map->regionFromName("'''Sheet 4'''!A1", m_map->sheet(0));
    QCOMPARE(region.name(), QString("'Sheet 4'!A1"));
    // invalid calls:
    region = m_map->regionFromName("!A1", m_map->sheet(0));
    QVERIFY(region.isEmpty());
    region = m_map->regionFromName("Sheet99!A1", m_map->sheet(0));
    QVERIFY(region.isEmpty());
}

void TestRegion::testExtrem()
{
    Region region1 = Region(QPoint(-1, -1), m_map->sheet(0));
    QVERIFY(region1.isEmpty());
    QVERIFY(!region1.isValid());

    Region region2 = m_map->regionFromName("A1:A6553634523563453456356");
    QVERIFY(region2.isValid());

    Region region3 = Region(QRect(1, 1, KS_colMax, KS_rowMax), m_map->sheet(0));
    QVERIFY(region3.isValid());

    Region region4 = Region(QRect(1, 1, KS_colMax, KS_rowMax), m_map->sheet(0));
    QVERIFY(region4.isValid());
    Region region5 = Region(QRect(1, 1, KS_colMax + 12345, KS_rowMax + 12345), m_map->sheet(0));
    QVERIFY(region5.isValid());
    QCOMPARE(region4.name(), region5.name());

    Region region6 = Region(QPoint(KS_colMax, KS_rowMax), m_map->sheet(0));
    QVERIFY(region6.isValid());
    Region region7 = Region(QPoint(KS_colMax + 22, KS_rowMax + 22), m_map->sheet(0));
    QVERIFY(region7.isValid());
    QCOMPARE(region6.name(), region7.name());
}

void TestRegion::testOps()
{
    Region empty = Region();
    int rownum = 4;
    Region row = Region(QRect(QPoint(1, rownum), QPoint(KS_colMax, rownum)));
    Region a1 = row.intersected(empty);
    QVERIFY(a1.isEmpty());
    Region rect = Region(QRect(QPoint(3, 3), QPoint(5, 5)));
    Region a2 = row.intersected(rect);
    Region a3 = rect.intersected(row);
    QVERIFY(a2 == a3);
    Region smallrect = Region(QRect(QPoint(3, 4), QPoint(5, 4)));
    QVERIFY(a2 == smallrect);

    QVERIFY(row.isRowSelected(rownum));
    QVERIFY(row.isColumnOrRowSelected());
    QVERIFY(row.contains(QPoint(789, rownum)));
    QVERIFY(!row.contains(QPoint(789, rownum + 1)));

    QRect bound = empty.boundingRect();
    QVERIFY(bound == QRect());
}

void TestRegion::cleanupTestCase()
{
    delete m_map;
}

QTEST_MAIN(TestRegion)
