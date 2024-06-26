// This file is part of the KDE project
// SPDX-FileCopyrightText: 2004, 2007 Ariya Hidayat <ariya@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#include "TestUtil.h"

#include "TestKspreadCommon.h"
#include "core/odf/SheetsOdf.h"

using namespace Calligra::Sheets;

void TestUtil::testDecodeFormula_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");

    QTest::newRow("exponent") << "=(   (PI()/16)  *   ([.N102]*POWER([.J102];2))  *  (POWER(1.6E-019;2))   * (  1/ (1.6E-019*[.E102]*1000*9.1E-031) )     )"
                              << "=(   (PI()/16)  *   (N102*POWER(J102;2))  *  (POWER(1.6E-019;2))   * (  1/ (1.6E-019*E102*1000*9.1E-031) )     )";
    QTest::newRow("strange range")
        << "=['file:///Users/twacks/Library/Mail Downloads/Town Council/07-08 budgetFlat2%2525ColaSemiFinal-PropC.xls'#$'TH  3_7_2 proposed no p_s  _2_'.F21]"
        << "='file:///Users/twacks/Library/Mail Downloads/Town Council/07-08 budgetFlat2%2525ColaSemiFinal-PropC.xls'#$'TH  3_7_2 proposed no p_s  _2_'!F21";
    QTest::newRow("handle !") << "=#ref!#ref!"
                              << "=#ref!#ref!";
}

void TestUtil::testDecodeFormula()
{
    QFETCH(QString, string);
    QFETCH(QString, result);

    QCOMPARE(Odf::decodeFormula(string), result);
}

QTEST_MAIN(TestUtil)
