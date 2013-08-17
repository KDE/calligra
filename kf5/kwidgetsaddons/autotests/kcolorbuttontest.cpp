/*
    Copyright 2013 Albert Astals Cid <aacid@kde.org>

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

#include "kcolorbuttontest.h"

#include <QtTest/QtTest>
#include <kcolorbutton.h>
#include <QColorDialog>
#include <QComboBox>
#include <QDialogButtonBox>

QTEST_MAIN(KColorButtonTest)

static void workaround()
{
    // Workaround for Qt-5.1 bug, which assumes GTK if there's no running desktop.
    // (and apparently QTest::qWaitForWindowExposed doesn't work for GTK native dialogs)
    qputenv("KDE_FULL_SESSION", "1");

    // TODO: it means this test will always fail with native dialogs, though.
    // But we can't set QColorDialog::DontUseNativeDialog from here...
}
Q_CONSTRUCTOR_FUNCTION(workaround)

void KColorButtonTest::initTestCase()
{
    black40Colors.setHsv(-1, 0, 0);
}

void KColorButtonTest::testChangeAndCancel()
{
    KColorButton colorButton(Qt::red);
    colorButton.show();
    QVERIFY(QTest::qWaitForWindowExposed(&colorButton));
    QTest::mouseClick(&colorButton, Qt::LeftButton);
    QColorDialog *dialog = colorButton.findChild<QColorDialog*>();
    QVERIFY(dialog != NULL);
    QVERIFY(QTest::qWaitForWindowExposed(dialog));
#pragma message("port to QColorDialog")
#if 0
    KColorCells *cells = dialog->findChild<KColorCells*>();
    QVERIFY(cells != NULL);
    QTest::mouseClick(cells->viewport(), Qt::LeftButton, 0, QPoint(1, 1));
    QCOMPARE(dialog->color(), black40Colors);
    dialog->reject();
    QCOMPARE(colorButton.color(), QColor(Qt::red));
#endif
}

void KColorButtonTest::testDoubleClickChange()
{
    KColorButton colorButton(Qt::red);
    colorButton.show();
    QVERIFY(QTest::qWaitForWindowExposed(&colorButton));
    QTest::mouseClick(&colorButton, Qt::LeftButton);
    QColorDialog *dialog = colorButton.findChild<QColorDialog*>();
    QVERIFY(dialog != NULL);
    QVERIFY(QTest::qWaitForWindowExposed(dialog));
#pragma message("port to QColorDialog")
#if 0
    KColorCells *cells = dialog->findChild<KColorCells*>();
    QVERIFY(cells != NULL);
    QTest::mouseDClick(cells->viewport(), Qt::LeftButton, 0, QPoint(1, 1));
    QCOMPARE(colorButton.color(), black40Colors);
#endif
}

void KColorButtonTest::testOkChange()
{
    KColorButton colorButton(Qt::red);
    colorButton.show();
    QVERIFY(QTest::qWaitForWindowExposed(&colorButton));
    QTest::mouseClick(&colorButton, Qt::LeftButton);
    QColorDialog *dialog = colorButton.findChild<QColorDialog*>();
    QVERIFY(dialog != NULL);
    QVERIFY(QTest::qWaitForWindowExposed(dialog));
#pragma message("port to QColorDialog")
#if 0
    KColorCells *cells = dialog->findChild<KColorCells*>();
    QVERIFY(cells != NULL);
    QTest::mouseClick(cells->viewport(), Qt::LeftButton, 0, QPoint(1, 1));
    QCOMPARE(dialog->color(), black40Colors);

    QSignalSpy okClickedSpy(dialog, SIGNAL(okClicked()));
    const QDialogButtonBox *buttonBox = dialog->findChild<QDialogButtonBox*>();
    const QList<QAbstractButton *> buttons = buttonBox->buttons();
    foreach(QAbstractButton *button, buttons) {
        if (buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole) {
            QTest::mouseClick(button, Qt::LeftButton);
            break;
        }
    }
    QCOMPARE(okClickedSpy.count(), 1);

    QCOMPARE(colorButton.color(), black40Colors);
#endif
}

void KColorButtonTest::testRecentColorsPick()
{
    KColorButton colorButton(Qt::red);
    colorButton.show();
    QVERIFY(QTest::qWaitForWindowExposed(&colorButton));
    QTest::mouseClick(&colorButton, Qt::LeftButton);
    QColorDialog *dialog = colorButton.findChild<QColorDialog*>();
    QVERIFY(dialog != NULL);
    QVERIFY(QTest::qWaitForWindowExposed(dialog));

#pragma message("port to QColorDialog")
#if 0
    QComboBox *combo = dialog->findChild<QComboBox*>();
    combo->setFocus();
    QTest::keyPress(combo, Qt::Key_Up);
    QTest::keyPress(combo, Qt::Key_Up);

    KColorCells *cells = dialog->findChild<KColorCells*>();
    QVERIFY(cells != NULL);
    QTest::mouseMove(cells->viewport(), QPoint(1, 1));
    QTest::mouseClick(cells->viewport(), Qt::LeftButton, 0, QPoint(30, 1));
    const QColor color = dialog->color();
    dialog->accept();
    QCOMPARE(colorButton.color(), color);
#endif
}
