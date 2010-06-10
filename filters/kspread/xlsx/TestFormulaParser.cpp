/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2010 Sebastian Sauer <sebsauer@kdab.com>
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
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

#include "qtest_kde.h"

#include "FormulaParser.h"

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
    QTest::newRow("union operator")
        << "AREAS((A1:A3,B3:C5))"
        << "=AREAS((A1:A3~B3:C5))";
    QTest::newRow("nested function calls")
        << "IF(OR(C12=\"\",D12=\"\"),\"\",IF(C12=D12,\"Pass\",\"Fail\"))"
        << "=IF(OR(C12=\'\';D12=\"\");\"\";IF(C12=D12;\"Pass\";\"Fail\"))";
    QTest::newRow("intersection operator")
        << "AREAS((A1:C5 B2:B3))"
        << "=AREAS((A1:C5!B2:B3))";
    QTest::newRow("whitespace in normal arguments")
        << "IF(A1=A2, 2, \" IF(1,2) \")"
        << "=IF(A1=A2; 2; \" IF(1,2) \")";
    QTest::newRow("mixing union and intersection")
        << "AREAS((A1:C5 B2:B3,C2:C3))"
        << "=AREAS((A1:C5!B2:B2~C2:C3))";
}

void TestFormulaParser::testConvertFormula()
{
    QFETCH(QString, xlsx);
    QFETCH(QString, odf);

    QCOMPARE(MSOOXML::convertFormula(xlsx), odf);
}

QTEST_KDEMAIN(TestFormulaParser, NoGUI)

#include "TestFormulaParser.moc"
