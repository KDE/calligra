// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Brad Hards <bradh@frogmouth.net>
// SPDX-FileCopyrightText: 2007 Sascha Pfau <MrPeacock@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_INFORMATION_FUNCTIONS
#define CALLIGRA_SHEETS_TEST_INFORMATION_FUNCTIONS

#include <QObject>

#include <engine/CellBase.h>
#include <engine/Value.h>

namespace Calligra
{
namespace Sheets
{
class MapBase;
class CellBase;

class TestInformationFunctions : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();

    void testAREAS();
    void testCELL();
    void testCOLUMN();
    void testCOLUMNS();
    void testCOUNT();
    void testCOUNTA();
    void testCOUNTBLANK();
    void testCOUNTIF();
    void testERRORTYPE();
    void testFORMULA();
    void testINFO();
    void testISBLANK();
    void testISERR();
    void testISERROR();
    void testISEVEN();
    void testISFORMULA();
    void testISLOGICAL();
    void testISNONTEXT();
    void testISNA();
    void testISNUMBER();
    void testISODD();
    void testISTEXT();
    void testISREF();
    void testMATCH();
    void testN();
    void testNA();
    void testROW();
    void testROWS();
    void testSHEET();
    void testSHEETS();
    void testTYPE();
    void testVALUE();

    void cleanupTestCase();

private:
    Value evaluate(const QString &, Value &ex, const CellBase &cell = CellBase());

    MapBase *m_map;
};

} // namespace Sheets
} // namespace Calligra

#endif
