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

#include "TestDependencies.h"

#include "qtest_kde.h"

#include "CellStorage.h"
#include "DependencyManager.h"
#include "DependencyManager_p.h"
#include "Formula.h"
#include "Map.h"
#include "Region.h"
#include "Sheet.h"
#include "Value.h"

using namespace KSpread;

void TestDependencies::initTestCase()
{
    m_map = new Map(0 /* no Doc */);
    m_sheet = m_map->addNewSheet();
    m_sheet->setSheetName("Sheet1");
    m_storage = m_sheet->cellStorage();
}

void TestDependencies::testCircleRemoval()
{
    Formula formula(m_sheet);
    formula.setExpression("=A1");
    m_storage->setFormula(1, 1, formula); // A1

    QApplication::processEvents(); // handle Damages

    QCOMPARE(m_storage->value(1, 1), Value::errorCIRCLE());
    DependencyManager* manager = m_map->dependencyManager();
    QVERIFY(manager->d->consumers.count() == 1);
    QVERIFY(manager->d->providers.count() == 1);
    QList<Cell> consumers = manager->d->consumers.value(m_sheet)->contains(QRect(1, 1, 1, 1));
    QCOMPARE(consumers.count(), 1);
    QCOMPARE(consumers.first(), Cell(m_sheet, 1, 1));
    QCOMPARE(manager->d->providers.value(Cell(m_sheet, 1, 1)), Region(QRect(1, 1, 1, 1), m_sheet));

    m_storage->setFormula(1, 1, Formula()); // A1

    QApplication::processEvents(); // handle Damages

    QCOMPARE(m_storage->value(1, 1), Value());
    QVERIFY(manager->d->consumers.value(m_sheet)->contains(QRect(1, 1, 1, 1)).count() == 0);
    QVERIFY(manager->d->providers.count() == 0);
}

void TestDependencies::testCircles()
{
    Formula formula(m_sheet);
    formula.setExpression("=A3");
    m_storage->setFormula(1, 1, formula); // A1
    formula.setExpression("=A1");
    m_storage->setFormula(1, 2, formula); // A2
    formula.setExpression("=A2");
    m_storage->setFormula(1, 3, formula); // A3

    QApplication::processEvents(); // handle Damages

    QCOMPARE(m_storage->value(1, 1), Value::errorCIRCLE());
    QCOMPARE(m_storage->value(1, 2), Value::errorCIRCLE());
    QCOMPARE(m_storage->value(1, 3), Value::errorCIRCLE());
}

void TestDependencies::cleanupTestCase()
{
    delete m_map;
}

QTEST_KDEMAIN(TestDependencies, GUI)

#include "TestDependencies.moc"
