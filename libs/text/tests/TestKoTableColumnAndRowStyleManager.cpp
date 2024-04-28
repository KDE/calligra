/*
 *  This file is part of Calligra tests
 *
 *  SPDX-FileCopyrightText: 2009-2010 C. Boemann <cbo@kogmbh.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "TestKoTableColumnAndRowStyleManager.h"

#include "../KoTableColumnAndRowStyleManager.h"
#include "styles/KoTableColumnStyle.h"
#include "styles/KoTableRowStyle.h"

#include <QTest>

void TestKoTableColumnAndRowStyleManager::testColumns()
{
    KoTableColumnAndRowStyleManager manager;

    KoTableColumnStyle *style1 = new KoTableColumnStyle();
    KoTableColumnStyle *style2 = new KoTableColumnStyle();
    KoTableColumnStyle *style3 = new KoTableColumnStyle();

    manager.setColumnStyle(0, *style1);
    manager.setColumnStyle(2, *style2);

    QVERIFY(manager.columnStyle(0) == *style1);
    // column 1 is a default inited style
    QVERIFY(manager.columnStyle(2) == *style2);

    manager.insertColumns(1, 2, *style3);

    QVERIFY(manager.columnStyle(0) == *style1);
    QVERIFY(manager.columnStyle(1) == *style3);
    QVERIFY(manager.columnStyle(2) == *style3);
    // column 3 is a default inited style
    QVERIFY(manager.columnStyle(4) == *style2);

    manager.removeColumns(2, 2);

    QVERIFY(manager.columnStyle(0) == *style1);
    QVERIFY(manager.columnStyle(1) == *style3);
    QVERIFY(manager.columnStyle(2) == *style2);
}

void TestKoTableColumnAndRowStyleManager::testRows()
{
    KoTableColumnAndRowStyleManager manager;

    KoTableRowStyle *style1 = new KoTableRowStyle();
    KoTableRowStyle *style2 = new KoTableRowStyle();
    KoTableRowStyle *style3 = new KoTableRowStyle();

    manager.setRowStyle(0, *style1);
    manager.setRowStyle(2, *style2);

    QVERIFY(manager.rowStyle(0) == *style1);
    QVERIFY(manager.rowStyle(2) == *style2);

    manager.insertRows(1, 2, *style3);

    QVERIFY(manager.rowStyle(0) == *style1);
    QVERIFY(manager.rowStyle(1) == *style3);
    QVERIFY(manager.rowStyle(2) == *style3);
    // row 3 is a default inited style
    QVERIFY(manager.rowStyle(4) == *style2);

    manager.removeRows(2, 2);

    QVERIFY(manager.rowStyle(0) == *style1);
    QVERIFY(manager.rowStyle(1) == *style3);
    QVERIFY(manager.rowStyle(2) == *style2);
}

QTEST_MAIN(TestKoTableColumnAndRowStyleManager)
