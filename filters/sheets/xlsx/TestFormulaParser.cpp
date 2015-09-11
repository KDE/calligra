/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2010 Sebastian Sauer <sebsauer@kdab.com>
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */
#include "TestFormulaParser.h"

#include <QTest>

#include "FormulaParser.h"
#include "XlsxXmlWorksheetReader_p.h"
#include <sheets/Util.h>

void TestFormulaParser::testConvertFormula_data()
{
    QTest::addColumn<QString>("xlsx");
    QTest::addColumn<QString>("odf");

    QTest::newRow("simple")
        << "A1"
        << "=A1";
    QTest::newRow("argument delimiter")
        << "IF(A1=A2,1,2)"
        << "=IF(A1=A2;1;2)";
    QTest::newRow("string")
        << "LEFT(\" Some   ~text \",3)"
        << "=LEFT(\" Some   ~text \";3)";
    QTest::newRow("condition")
        << "IF(A15<$B$6*$B$4,A15+1,\"\")"
        << "=IF(A15<$B$6*$B$4;A15+1;\"\")";
    QTest::newRow("union operator")
        << "AREAS((A1:A3,B3:C5))"
        << "=AREAS((A1:A3~B3:C5))";
    QTest::newRow("nested function calls")
        << "IF(OR(C12=\"\",D12=\"\"),\"\",IF(C12=D12,\"Pass\",\"Fail\"))"
        << "=IF(OR(C12=\"\";D12=\"\");\"\";IF(C12=D12;\"Pass\";\"Fail\"))";
    QTest::newRow("intersection operator")
        << "AREAS((A1:C5 B2:B3))"
        << "=AREAS((A1:C5!B2:B3))";
    QTest::newRow("whitespace in normal arguments")
        << "IF(A1=A2, 2, \" IF(1,2) \")"
        << "=IF(A1=A2; 2; \" IF(1,2) \")";
    QTest::newRow("multiple whitespace in normal arguments")
        << "IF(A1=A2 ,   2  , \" IF(1,2) \")"
        << "=IF(A1=A2 ;   2  ; \" IF(1,2) \")";
    QTest::newRow("mixing union and intersection")
        << "AREAS((A1:C5 B2:B3,C2:C3))"
        << "=AREAS((A1:C5!B2:B3~C2:C3))";
    QTest::newRow("absolute positions")
        << "AREAS(($A$1:$A$3,$B3:C$5))"
        << "=AREAS(($A$1:$A$3~$B3:C$5))";
    QTest::newRow("whole column")
        << "IF(A=B,A:B,2)"
        << "=IF(A=B;A$1:B$65536;2)";
    QTest::newRow("Sheetname")
        << "=IF('Sheet 1'!A1,''Sheet '1''!A2,'''Sheet 1'''!A3"
        << "=IF('Sheet 1'!A1;'Sheet ''1'!A2;'Sheet 1'!A3";
    QTest::newRow("intersection operator without extra parenthesis")
        << "AREAS(B2:D4 B2)"
        << "=AREAS(B2:D4!B2)";
    QTest::newRow("intersection operator without extra parenthesis, extra whitespace")
        << "AREAS(B2:D4    B2)"
        << "=AREAS(B2:D4!   B2)";
}

void TestFormulaParser::testConvertFormula()
{
    QFETCH(QString, xlsx);
    QFETCH(QString, odf);

    QCOMPARE(Calligra::Sheets::MSOOXML::convertFormula(xlsx), odf);
}

void TestFormulaParser::testSharedFormulaReferences()
{
    Sheet s1("Sheet1");
    Cell* c1 = s1.cell(2, 5, true);
    c1->formula = new FormulaImpl("=D6-E7");
    Cell* c2 = s1.cell(12, 43, true);
    QCOMPARE(MSOOXML::convertFormulaReference(c1, c2), QString("=N44-O45"));

    static_cast<FormulaImpl*>(c1->formula)->m_formula = "=SUM(D6-E7)";
    QCOMPARE(MSOOXML::convertFormulaReference(c1, c2), QString("=SUM(N44-O45)"));

    static_cast<FormulaImpl*>(c1->formula)->m_formula = "=D6";
    QCOMPARE(MSOOXML::convertFormulaReference(c1, c2), QString("=N44"));

    static_cast<FormulaImpl*>(c1->formula)->m_formula = "=SUM(D6)";
    QCOMPARE(MSOOXML::convertFormulaReference(c1, c2), QString("=SUM(N44)"));

    static_cast<FormulaImpl*>(c1->formula)->m_formula = "=F8(H12)";
    QCOMPARE(MSOOXML::convertFormulaReference(c1, c2), QString("=F8(R50)"));
}

QTEST_GUILESS_MAIN(TestFormulaParser)
