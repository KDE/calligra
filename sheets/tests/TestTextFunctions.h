// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_TEXT_FUNCTIONS
#define CALLIGRA_SHEETS_TEST_TEXT_FUNCTIONS

#include <QObject>

#include <engine/Value.h>

namespace Calligra
{
namespace Sheets
{

class MapBase;

class TestTextFunctions : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testASC();
    void testCHAR();
    void testCLEAN();
    void testCODE();
    void testCONCATENATE();
    void testEXACT();
    void testFIND();
    void testFIXED();
    void testJIS();
    void testLEFT();
    void testLEN();
    void testLOWER();
    void testMID();
    void testNUMBERVALUE();
    void testPROPER();
    void testREPLACE();
    void testREPT();
    void testRIGHT();
    void testSEARCH();
    void testSUBSTITUTE();
    void testT();
    void testTRIM();
    void testUNICHAR();
    void testUNICODE();
    void testUPPER();
    void testROT13();
    void testBAHTTEXT();
    void testTEXT();

    void cleanupTestCase();

private:
    Value evaluate(const QString &, Value &ex);

    MapBase *m_map;
};

} // namespace Sheets
} // namespace Calligra

#endif
