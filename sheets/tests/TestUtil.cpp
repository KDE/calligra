/* This file is part of the KDE project
   Copyright 2004,2007 Ariya Hidayat <ariya@kde.org>

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
#include "TestUtil.h"

#include "TestKspreadCommon.h"
#include <Util.h>

using namespace Calligra::Sheets;

void TestUtil::testDecodeFormula_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");

    QTest::newRow("exponent") << "=(   (PI()/16)  *   ([.N102]*POWER([.J102];2))  *  (POWER(1.6E-019;2))   * (  1/ (1.6E-019*[.E102]*1000*9.1E-031) )     )"
                              << "=(   (PI()/16)  *   (N102*POWER(J102;2))  *  (POWER(1.6E-019;2))   * (  1/ (1.6E-019*E102*1000*9.1E-031) )     )";
    QTest::newRow("strange range") << "=['file:///Users/twacks/Library/Mail Downloads/Town Council/07-08 budgetFlat2%2525ColaSemiFinal-PropC.xls'#$'TH  3_7_2 proposed no p_s  _2_'.F21]"
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
