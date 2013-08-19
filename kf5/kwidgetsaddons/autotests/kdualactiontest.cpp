/* This file is part of the KDE libraries
 *
 * Copyright (c) 2010 Aurélien Gâteau <agateau@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */
#include <kdualaction.h>

#include <QtTest/QtTest>

#include "kguiitem.h"


Q_DECLARE_METATYPE(QAction*)

static const QString INACTIVE_TEXT = QStringLiteral("Show Foo");
static const QString ACTIVE_TEXT = QStringLiteral("Hide Foo");

class KDualActionTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        qRegisterMetaType<QAction*>("QAction*");
    }

    void testSetGuiItem()
    {
        KDualAction action(0);
        action.setInactiveGuiItem(KGuiItem(INACTIVE_TEXT));
        action.setActiveGuiItem(KGuiItem(ACTIVE_TEXT));
        QCOMPARE(action.inactiveText(), INACTIVE_TEXT);
        QCOMPARE(action.activeText(), ACTIVE_TEXT);
        QCOMPARE(action.text(), INACTIVE_TEXT);
    }

    void testSetIconForStates()
    {
        QIcon icon = QIcon::fromTheme(QStringLiteral("kde"));
        KDualAction action(0);
        QVERIFY(action.inactiveIcon().isNull());
        QVERIFY(action.activeIcon().isNull());
        action.setIconForStates(icon);
        QCOMPARE(action.inactiveIcon(), icon);
        QCOMPARE(action.activeIcon(), icon);
    }

    void testSetActive()
    {
        KDualAction action(INACTIVE_TEXT, ACTIVE_TEXT, 0);
        QVERIFY(!action.isActive());
        QCOMPARE(action.text(), INACTIVE_TEXT);
        QSignalSpy activeChangedSpy(&action, SIGNAL(activeChanged(bool)));
        QSignalSpy activeChangedByUserSpy(&action, SIGNAL(activeChangedByUser(bool)));

        action.setActive(true);
        QVERIFY(action.isActive());
        QCOMPARE(action.text(), ACTIVE_TEXT);
        QCOMPARE(activeChangedSpy.count(), 1);
        QCOMPARE(activeChangedSpy.takeFirst().at(0).toBool(), true);
        QCOMPARE(activeChangedByUserSpy.count(), 0);

        action.setActive(false);
        QVERIFY(!action.isActive());
        QCOMPARE(action.text(), INACTIVE_TEXT);
        QCOMPARE(activeChangedSpy.count(), 1);
        QCOMPARE(activeChangedSpy.takeFirst().at(0).toBool(), false);
        QCOMPARE(activeChangedByUserSpy.count(), 0);
    }

    void testTrigger()
    {
        KDualAction action(INACTIVE_TEXT, ACTIVE_TEXT, 0);
        QVERIFY(!action.isActive());
        QCOMPARE(action.text(), INACTIVE_TEXT);
        QSignalSpy activeChangedSpy(&action, SIGNAL(activeChanged(bool)));
        QSignalSpy activeChangedByUserSpy(&action, SIGNAL(activeChangedByUser(bool)));

        action.trigger();
        QVERIFY(action.isActive());
        QCOMPARE(action.text(), ACTIVE_TEXT);
        QCOMPARE(activeChangedSpy.count(), 1);
        QCOMPARE(activeChangedSpy.takeFirst().at(0).toBool(), true);
        QCOMPARE(activeChangedByUserSpy.count(), 1);
        QCOMPARE(activeChangedByUserSpy.takeFirst().at(0).toBool(), true);

        action.trigger();
        QVERIFY(!action.isActive());
        QCOMPARE(action.text(), INACTIVE_TEXT);
        QCOMPARE(activeChangedSpy.count(), 1);
        QCOMPARE(activeChangedSpy.takeFirst().at(0).toBool(), false);
        QCOMPARE(activeChangedByUserSpy.count(), 1);
        QCOMPARE(activeChangedByUserSpy.takeFirst().at(0).toBool(), false);

        // Turn off autoToggle, nothing should happen
        action.setAutoToggle(false);
        action.trigger();
        QVERIFY(!action.isActive());
        QCOMPARE(action.text(), INACTIVE_TEXT);
        QCOMPARE(activeChangedSpy.count(), 0);
        QCOMPARE(activeChangedByUserSpy.count(), 0);

        // Turn on autoToggle, action should change
        action.setAutoToggle(true);
        action.trigger();
        QCOMPARE(action.text(), ACTIVE_TEXT);
        QCOMPARE(activeChangedSpy.count(), 1);
        QCOMPARE(activeChangedSpy.takeFirst().at(0).toBool(), true);
        QCOMPARE(activeChangedByUserSpy.count(), 1);
        QCOMPARE(activeChangedByUserSpy.takeFirst().at(0).toBool(), true);
    }
};

QTEST_MAIN(KDualActionTest)

#include "kdualactiontest.moc"
