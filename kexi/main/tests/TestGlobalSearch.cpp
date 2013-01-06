/* This file is part of the KDE project
   Copyright (C) 2012 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "TestGlobalSearch.h"

#include <core/kexi.h>
#include <core/kexipartitem.h>
#include <core/kexiaboutdata.h>
#include <main/KexiMainWindow.h>
#include <kexiutils/KexiTester.h>
#include <widget/navigator/KexiProjectNavigator.h>

#include <KDebug>
#include <KGlobal>
#include <KApplication>
#include <KLineEdit>

#include <QtTest>
#include <QtTest/qtestkeyboard.h>
#include <QtTest/qtestmouse.h>
#include <QFile>
#include <QTreeView>

const int GUI_DELAY = 10;

#define FILES_DATA_DIR KDESRCDIR "data"

void TestGlobalSearch::initTestCase()
{
}

//! Copies 0-th arg and adds second empty
class NewArgs
{
public:
    NewArgs(char *argv[]) {
        count = 2;
        vals = new char*[count];
        vals[0] = qstrdup(argv[0]);
        vals[count - 1] = 0;
    }
    ~NewArgs() {
        for (int i = 0; i < count; i++) {
            delete vals[i];
        }
        delete [] vals;
    }

    int count;
    char **vals;
};

TestGlobalSearch::TestGlobalSearch(int argc, char **argv, bool goToEventLoop)
 : m_argc(argc), m_argv(argv), m_goToEventLoop(goToEventLoop)
{
}
void TestGlobalSearch::testGlobalSearch()
{
    QString filename(QFile::decodeName(FILES_DATA_DIR "/TestGlobalSearch.kexi"));
    kDebug() << filename;
    NewArgs args(m_argv);
    args.vals[args.count - 1] = qstrdup(QFile::encodeName(filename).constData());

    KexiAboutData aboutData;
    aboutData.setProgramName(ki18n(metaObject()->className()));
    int result = KexiMainWindow::create(args.count, args.vals, aboutData);
    QVERIFY(kapp);
    QCOMPARE(result, 0);

    KLineEdit *lineEdit = Kexi::tester().widget<KLineEdit*>("globalSearch.lineEdit");
    QVERIFY(lineEdit);
    QTreeView *treeView = Kexi::tester().widget<QTreeView*>("globalSearch.treeView");
    QVERIFY(treeView);

    lineEdit->setFocus();
    // enter "cars", expect 4 completion items
    QTest::keyClicks(lineEdit, "cars");
    QVERIFY(treeView->isVisible());
    int globalSearchCompletionListRows = treeView->model()->rowCount();
    QCOMPARE(globalSearchCompletionListRows, 4);

    // add "x", expect no completion items and hidden list
    QTest::keyClicks(lineEdit, "x");
    QVERIFY(!treeView->isVisible());
    globalSearchCompletionListRows = treeView->model()->rowCount();
    QCOMPARE(globalSearchCompletionListRows, 0);

    // Escape should clear
    QTest::keyClick(lineEdit, Qt::Key_Escape,  Qt::NoModifier, GUI_DELAY);
    QVERIFY(lineEdit->text().isEmpty());

    QTest::keyClicks(lineEdit, "cars");
    QVERIFY(treeView->isVisible());
    treeView->setFocus();
    QTest::keyPress(treeView, Qt::Key_Down, Qt::NoModifier, GUI_DELAY);
    QTest::keyPress(treeView, Qt::Key_Down, Qt::NoModifier, GUI_DELAY);
    QTest::keyPress(treeView, Qt::Key_Down, Qt::NoModifier, GUI_DELAY);

    // 3rd row should be "cars" form
    QModelIndexList selectedIndices = treeView->selectionModel()->selectedRows();
    QCOMPARE(selectedIndices.count(), 1);
    QCOMPARE(treeView->model()->data(selectedIndices.first(), Qt::DisplayRole).toString(), QLatin1String("cars"));

    // check if proper entry of Project Navigator is selected
    QTest::keyPress(treeView, Qt::Key_Enter, Qt::NoModifier, GUI_DELAY);

    KexiProjectNavigator *projectNavigator = Kexi::tester().widget<KexiProjectNavigator*>("KexiProjectNavigator");
    QVERIFY(projectNavigator);
    KexiPart::Item* selectedPartItem = projectNavigator->selectedPartItem();
    QVERIFY(selectedPartItem);
    QCOMPARE(selectedPartItem->name(), QLatin1String("cars"));
    QCOMPARE(selectedPartItem->partClass(), QLatin1String("org.kexi-project.form"));

    if (m_goToEventLoop) {
        result = kapp->exec();
        QCOMPARE(result, 0);
    }
}

void TestGlobalSearch::cleanupTestCase()
{
}

int main(int argc, char *argv[])
{
    // Pull off custom options
    bool goToEventLoop = false;
    int realCount = 0;
    char **realVals = new char*[argc];
    for (int i = 0; i < argc; ++i) {
        realVals[i] = 0;
    }
    for (int i = 0; i < argc; ++i) {
        if (0 == qstrcmp(argv[i], "-loop")) {
            goToEventLoop = true;
            continue;
        }
        else {
            if (0 == qstrcmp(argv[i], "-help") || 0 == qstrcmp(argv[i], "--help")) {
                printf(" Options coming from the Kexi test suite:\n -loop : Go to event loop after successful test\n\n");
            }
            realVals[realCount] = qstrdup(argv[i]);
            ++realCount;
        }
    }

    // Actual test
    TestGlobalSearch tc(realCount, realVals, goToEventLoop);
    int result = QTest::qExec(&tc, realCount, realVals);

    // Clean up
    for (int i = 0; i < argc; i++) {
        delete realVals[i];
    }
    delete [] realVals;
    return result;
}
