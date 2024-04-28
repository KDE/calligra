// This file is part of the KDE project
// SPDX-FileCopyrightText: 2006 Ariya Hidayat <ariya@kde.org>
// SPDX-FileCopyrightText: 2007 Sascha Pfau <MrPeacock@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_FINANCIAL_FUNCTIONS
#define CALLIGRA_SHEETS_TEST_FINANCIAL_FUNCTIONS

#include <QObject>

#include <engine/Value.h>

namespace Calligra
{
namespace Sheets
{

class TestFinancialFunctions : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testACCRINT();
    void testACCRINTM();
    void testAMORDEGRC();
    void testAMORLINC();
    void testCOMPOUND(); // no ODF tests available
    void testCONTINUOUS(); // no ODF tests available
    void testCOUPDAYBS();
    void testCOUPDAYS();
    void testCOUPDAYSNC();
    void testCOUPNCD();
    void testCOUPNUM();
    void testCOUPPCD();
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
    void testRATE();
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

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_FINANCIAL_FUNCTIONS
