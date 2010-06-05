/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2006 Ariya Hidayat <ariya@kde.org>

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

#ifndef KSPREAD_TEST_STATISTICAL_FUNCTIONS
#define KSPREAD_TEST_STATISTICAL_FUNCTIONS

#include <QtGui>
#include <QtTest/QtTest>

#include <Value.h>

namespace KSpread
{
class Map;

class TestStatisticalFunctions : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

//     void testARRANG(); // alias PERMUT
    void testAVEDEV();
    void testAVERAGE();
    void testAVERAGEA();
    void testBETADIST();
    void testBETAINV();
//     void testBINO(); // kspread version of BINOMDIST with 3 Parameters
    void testBINOMDIST();
    void testCHIDIST();
//     void testCOMBIN();  // in -> TestMathFunctions
//     void testCOMBINA(); // in -> TestMathFunctions
    void testCONFIDENCE();
    void testCORREL();
    void testCOVAR();
    void testDEVSQ();
//     void testDEVSQA(); // no ODF-test available
    void testEXPONDIST();
    void testFDIST();
    void testFINV();
    void testFISHER();
    void testFISHERINV();
    void testFREQUENCY();
    void testFTEST();
    void testGAMMADIST();
    void testGAMMAINV();
//     void testGAMMALN(); in -> TestMathFunctions
    void testGAUSS();
    void testGROWTH(); // to be implemented
    void testGEOMEAN();
    void testHARMEAN();
    void testHYPGEOMDIST();
    void testINTERCEPT();
//     void testINVBINO();
    void testKURT();
//     void testKURTP(); // ???
    void testLARGE();


    void testLEGACYCHIDIST();
    void testLEGACYCHIINV();
    void testLEGACYFDIST();
    void testLEGACYFINV();
//     void testLEGACYNORMSDIST(); // same as NORMDIST required for OpenFormula compliance


//     void testLINEST(); // ???
//     void testLOGEST(); // ???
    void testLOGINV();
    void testLOGNORMDIST();
    void testMAX();
    void testMAXA();
    void testMEDIAN();
    void testMIN();
    void testMINA();
    void testMODE();
    void testNEGBINOMDIST();
    void testNORMDIST();
    void testNORMINV();
//     void testNORMSDIST();
//     void testNORMSINV();

    void testPEARSON();
    void testPERCENTILE();
    void testPERMUT();
    void testPHI();
    void testPOISSON();
//     void testPROB();     // ???
    void testQUARTILE();
    void testRANK();
    void testRSQ();
    void testSKEW();
    void testSKEWP();
    void testSLOPE();
    void testSMALL();
    void testSTANDARDIZE();
    void testSTDEV();
    void testSTDEVA();
    void testSTDEVP();
    void testSTDEVPA();
    void testSTEYX();
//     void testSUMXMY2(); // deprecated
//     void testSUMPRODUCT();
//     void testSUMX2PY2();
//     void testSUMX2MY2();
    void testTDIST();
    void testTINV();
    void testTREND();
    void testTRIMMEAN();
    void testTTEST();
    void testVAR();
    void testVARA();
    void testVARIANCE();
    void testVARP();
    void testVARPA();
    void testWEIBULL();
    void testZTEST();

    void cleanupTestCase();

private:
    bool  TestArray(const QString& formula, const QString& Array, int accuracy, bool checkSize);
    Value evaluate(const QString&);
    Value TestDouble(const QString& formula, const Value& v2, int accuracy);

    Map* m_map;
};

} // namespace KSpread

#endif // KSPREAD_TEST_STATISTICAL_FUNCTIONS
