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

#include "Doc.h"
#include "Map.h"
#include "Region.h"
#include "Sheet.h"

using namespace KSpread;

void TestRegion::initTestCase()
{
    m_doc = new Doc();
    Sheet* sheet = m_doc->map()->addNewSheet();
    sheet->setSheetName("Sheet1");
    sheet = m_doc->map()->addNewSheet();
    sheet->setSheetName("Sheet2");
    sheet = m_doc->map()->addNewSheet();
    sheet->setSheetName("Sheet3");
    sheet = m_doc->map()->addNewSheet();
    sheet->setSheetName("Sheet 4");
}

void TestRegion::testFixation()
{
    Region region;
    region = Region(m_doc->map(), "$A1", m_doc->map()->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!$A1"));
    region = Region(m_doc->map(), "A$1", m_doc->map()->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A$1"));
    region = Region(m_doc->map(), "$A$1", m_doc->map()->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!$A$1"));
    region = Region(m_doc->map(), "$A1:B4", m_doc->map()->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!$A1:B4"));
    region = Region(m_doc->map(), "A$1:B4", m_doc->map()->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A$1:B4"));
    region = Region(m_doc->map(), "$A$1:B4", m_doc->map()->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!$A$1:B4"));
    region = Region(m_doc->map(), "A1:$B4", m_doc->map()->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A1:$B4"));
    region = Region(m_doc->map(), "A1:B$4", m_doc->map()->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A1:B$4"));
    region = Region(m_doc->map(), "A1:$B$4", m_doc->map()->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A1:$B$4"));
}

void TestRegion::testSheet()
{
    Region region;
    region = Region(QPoint(1, 1), m_doc->map()->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A1"));
    QCOMPARE(region.firstSheet(), m_doc->map()->sheet(0));
    region = Region(m_doc->map(), "A1", m_doc->map()->sheet(0));
    QCOMPARE(region.name(), QString("Sheet1!A1"));
    QCOMPARE(region.firstSheet(), m_doc->map()->sheet(0));
    region = Region(m_doc->map(), "Sheet1!A1", m_doc->map()->sheet(1));
    QCOMPARE(region.name(), QString("Sheet1!A1"));
    QCOMPARE(region.firstSheet(), m_doc->map()->sheet(0));
    region = Region(m_doc->map(), "Sheet2!A1");
    QCOMPARE(region.name(), QString("Sheet2!A1"));
    QCOMPARE(region.firstSheet(), m_doc->map()->sheet(1));
    region = Region(m_doc->map(), "Sheet2!A1", m_doc->map()->sheet(0));
    QCOMPARE(region.name(), QString("Sheet2!A1"));
    QCOMPARE(region.firstSheet(), m_doc->map()->sheet(1));
    region = Region(m_doc->map(), "Sheet 4!A1", m_doc->map()->sheet(0));
    QCOMPARE(region.name(), QString("'Sheet 4'!A1"));
    QCOMPARE(region.firstSheet(), m_doc->map()->sheet(3));
    region = Region(m_doc->map(), "'Sheet 4'!A1", m_doc->map()->sheet(0));
    QCOMPARE(region.name(), QString("'Sheet 4'!A1"));
    QCOMPARE(region.firstSheet(), m_doc->map()->sheet(3));
    // invalid calls:
    region = Region(m_doc->map(), "A1");
    QVERIFY(region.isEmpty());
    region = Region(m_doc->map(), "!A1", m_doc->map()->sheet(0));
    QVERIFY(region.isEmpty());
    region = Region(m_doc->map(), "Sheet99!A1", m_doc->map()->sheet(0));
    QVERIFY(region.isEmpty());
}

void TestRegion::cleanupTestCase()
{
    delete m_doc;
}

QTEST_KDEMAIN(TestRegion, GUI)

#include "TestRegion.moc"
