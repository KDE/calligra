// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Ariya Hidayat <ariya@kde.org>
// SPDX-FileCopyrightText: 2007 Sascha Pfau <MrPeacock@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_MATH_FUNCTIONS
#define CALLIGRA_SHEETS_TEST_MATH_FUNCTIONS

#include <QObject>

#include <engine/Value.h>

namespace Calligra
{
namespace Sheets
{
class MapBase;

class TestMathFunctions : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();

    void testABS();
    void testACOS();
    void testACOSH();
    void testACOT();
    void testACOTH();
    void testASIN();
    void testASINH();
    void testATAN();
    void testATAN2();
    void testATANH();
    void testBESSELI();
    void testBESSELJ();
    void testBESSELK();
    void testBESSELY();
    void testCOMBIN();
    void testCOMBINA(); // to be implemented
    void testCONVERT();
    void testCEIL();
    void testCEILING();
    void testCOT();
    void testCOTH();
    void testDEGREES();
    void testDELTA();
    //   void testERF();  -> TestEngineering
    //   void testERFC(); -> TestEngineering
    void testEVEN();
    void testEXP();
    void testFACT();
    void testFACTDOUBLE();
    void testFIB();
    void testFLOOR();
    void testGAMMA();
    void testGAMMALN();
    void testGCD();
    void testGESTEP();
    void testINT();
    void testLCM();
    void testLN();
    void testLOG();
    void testLOG10();
    void testMDETERM();
    void testMINVERSE();
    void testMMULT();
    void testMOD();
    void testMROUND();
    void testMULTINOMIAL();
    void testMUNIT();
    void testODD();
    //   void testPI(); -> TestEngineering
    void testPOWER();
    void testPRODUCT();
    void testQUOTIENT();
    void testRADIANS();
    void testRAND();
    void testRANDBETWEEN();
    void testROUND();
    void testROUNDDOWN();
    void testROUNDUP();
    void testSERIESSUM();
    void testSIGN();
    void testSQRT();
    void testSQRTPI();
    void testSUBTOTAL();
    void testSUMA();
    void testSUMIF();
    void testSUMIF_STRING();
    void testSUMIF_WILDCARDS();
    void testSUMIF_REGULAREXPRESSIONS();
    void testSUMSQ();
    void testTRUNC();

private:
    Value TestDouble(const QString &formula, const Value &v2, int accuracy);
    Value evaluate(const QString &);

    MapBase *m_map;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_MATH_FUNCTIONS
