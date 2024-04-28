// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Sascha Pfau <MrPeacock@web.de>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_DATETIME_FUNCTIONS
#define CALLIGRA_SHEETS_TEST_DATETIME_FUNCTIONS

#include <QObject>

#include <engine/Value.h>

namespace Calligra
{
namespace Sheets
{

class MapBase;
class SheetBase;

class TestDatetimeFunctions : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void cleanupTestCase();
    void initTestCase();
    void testYEARFRAC();
    void testDATEDIF();
    void testISLEAPYEAR();
    void testWEEKNUM();
    void testWEEKSINYEAR();
    void testWORKDAY();
    void testNETWORKDAY();
    void testUNIX2DATE();
    void testDATE2UNIX();
    void testDATE();
    void testDATEVALUE();
    void testDAY();
    void testDAYS();
    void testDAYSINMONTH();
    void testDAYSINYEAR();
    void testDAYS360();
    void testEDATE();
    void testEOMONTH();
    void testHOUR_data();
    void testHOUR();
    void testISOWEEKNUM();
    void testMINUTE();
    void testMONTH();
    void testMONTHS();
    void testNOW();
    void testSECOND();
    void testTIME();
    void testTIMEVALUE();
    void testTODAY();
    void testWEEKDAY();
    void testYEAR();
    void testYEARS();
    void testWEEKS();

private:
    MapBase *m_map;
    SheetBase *m_sheet;
    Value evaluate(const QString &, Value &ex);
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_DATETIME_FUNCTIONS
