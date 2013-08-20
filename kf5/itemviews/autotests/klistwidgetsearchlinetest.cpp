/***************************************************************************
 *   Copyright (C) 2011 by Frank Reininghaus (frank78ac@googlemail.com)    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#include <QtTest/QtTest>

#include <klistwidgetsearchline.h>

#include <QListWidget>

class KListWidgetSearchLineTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testAddItems();

};

/**
 * If items are added to the list view or modified, KListWidgetSearchLine
 * should hide them if they do not match the search string, see
 *
 * https://bugs.kde.org/show_bug.cgi?id=265709
 */

void KListWidgetSearchLineTest::testAddItems()
{
    QListWidget listWidget;
    listWidget.addItem("Matching test item");
    listWidget.addItem("Another test item");

    KListWidgetSearchLine searchLine(0, &listWidget);
    searchLine.setText("match");

    // The initial filtering is delayed; we have to wait
    while(!listWidget.item(1)->isHidden()) {
        QTest::qWait(50);
    }

    QVERIFY(!listWidget.item(0)->isHidden());
    QVERIFY(listWidget.item(1)->isHidden());

    // Add two items
    listWidget.addItem("Another item that matches the search pattern");
    listWidget.addItem("This item should be hidden");

    QVERIFY(!listWidget.item(0)->isHidden());
    QVERIFY(listWidget.item(1)->isHidden());
    QVERIFY(!listWidget.item(2)->isHidden());
    QVERIFY(listWidget.item(3)->isHidden());

    // Modify an item
    listWidget.item(3)->setText("Now this item matches");

    QVERIFY(!listWidget.item(0)->isHidden());
    QVERIFY(listWidget.item(1)->isHidden());
    QVERIFY(!listWidget.item(2)->isHidden());
    QVERIFY(!listWidget.item(3)->isHidden());
}

QTEST_MAIN(KListWidgetSearchLineTest)

#include "klistwidgetsearchlinetest.moc"
