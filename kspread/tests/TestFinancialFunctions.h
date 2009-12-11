/* This file is part of the KDE project
   Copyright 2006 Ariya Hidayat <ariya@kde.org>
   Copyright 2007 Sascha Pfau <MrPeacock@gmail.com>

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

#ifndef KSPREAD_TEST_FINANCIAL_FUNCTIONS
#define KSPREAD_TEST_FINANCIAL_FUNCTIONS

#include <QtGui>
#include <qtest_kde.h>

#include <Value.h>

namespace KSpread
{

class TestFinancialFunctions: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testACCRINT();
    void testACCRINTM();
    void testAMORDEGRC();
    void testAMORLINC();
    void testCOMPOUND();   // no ODF tests available
    void testCONTINUOUS(); // no ODF tests available
//   void testCOUPDAYBS();  // to be implemented
//   void testCOUPDAYS();   // to be implemented
//   void testCOUPDAYSNC(); // to be implemented
//   void testCOUPNCD();    // to be implemented
    void testCOUPNUM();
//   void testCOUPPCD();    // to be implemented
    void testCUMIPMT();
    void testCUMPRINC();
    void testDB();
    void testDDB();
    void testDISC();
    void testDOLLARDE();
    void testDOLLARFR();
    void testDURATION();
    void testDURATION_ADD(); // excel
    void testEFFECT();
    void testEURO();
    void testEUROCONVERT();
    void testFV();
//   void testFV_ANNUITY(); // k
    void testFVSCHEDULE();
    void testINTRATE();
    void testIPMT();
//   void testIRR();        // to be implemented
    void testISPMT();
    void testLEVELCOUPON();
    void testMDURATION();
    void testMIRR();
    void testNOMINAL();
    void testNPER();
    void testNPV();
//   void testODDFPRICE();  // to be implemented
//   void testODDFYIELD();  // to be implemented
    void testODDLPRICE();
    void testODDLYIELD();
    void testPDURATION();
    void testPMT();
    void testPPMT();
//   void testPRICE();      // to be implemented
//   void testPRICEDISC();  // to be implemented
    void testPRICEMAT();
    void testPV();
    void testPV_ANNUITY(); // no ODF test available
//   void testRATE();       // to be implemented
    void testRECEIVED();
    void testRRI();
    void testSLN();
    void testSYD();
    void testTBILLEQ();
    void testTBILLPRICE();
    void testTBILLYIELD();
    void testVDB();
    void testXIRR();
    void testXNPV();
//   void testYIELD();      // to be implemented
    void testYIELDDISC();
    void testYIELDMAT();
    void testZEROCOUPON();

// private:
//   Value evaluate(const QString&);
//   Value evaluateShort(const QString&);
};

} // namespace KSpread

#endif // KSPREAD_TEST_FINANCIAL_FUNCTIONS
