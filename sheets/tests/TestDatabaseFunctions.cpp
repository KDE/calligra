// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-License-Identifier: LGPL-2.0-only

#include "TestDatabaseFunctions.h"

#include "engine/CellBaseStorage.h"
#include "engine/MapBase.h"
#include "engine/NamedAreaManager.h"
#include "engine/SheetBase.h"

#include "TestKspreadCommon.h"

#define CHECK_EVAL(x, y)                                                                                                                                       \
    {                                                                                                                                                          \
        Value z(RoundNumber(y));                                                                                                                               \
        QCOMPARE(evaluate(x, z), (z));                                                                                                                         \
    }
#define ROUND(x) (roundf(1e10 * x) / 1e10)

// round to get at most 10-digits number
static Value RoundNumber(const Value &v)
{
    if (v.isNumber()) {
        double d = numToDouble(v.asFloat());
        if (fabs(d) < DBL_EPSILON)
            d = 0.0;
        return Value(ROUND(d));
    } else
        return v;
}

Value TestDatabaseFunctions::evaluate(const QString &formula, Value &ex)
{
    Formula f(m_map->sheet(0));
    QString expr = formula;
    if (expr[0] != '=')
        expr.prepend('=');
    f.setExpression(expr);
    Value result = f.eval();

    if (result.isFloat() && ex.isInteger())
        ex = Value(ex.asFloat());
    if (result.isInteger() && ex.isFloat())
        result = Value(result.asFloat());

    return RoundNumber(result);
}

void TestDatabaseFunctions::initTestCase()
{
    KLocalizedString::setApplicationDomain("calligrasheets");
    FunctionModuleRegistry::instance()->loadFunctionModules();
    m_map = new MapBase;
    m_map->addNewSheet();
    SheetBase *sheet = m_map->sheet(0);
    CellBaseStorage *storage = sheet->cellStorage();

    // TESTDB = A18:I31
    m_map->namedAreaManager()->insert(Region(QRect(QPoint(1, 18), QPoint(9, 31)), sheet), "TESTDB");
    // A18:A31
    storage->setValue(1, 18, Value("TestID"));
    for (int row = 19; row <= 31; ++row)
        storage->setValue(1, row, Value((double)::pow(2.0, row - 19)));
    // B18:B31
    storage->setValue(2, 18, Value("Constellation"));
    QList<QString> constellations = QList<QString>() << "Cancer"
                                                     << "Canis Major"
                                                     << "Canis Minor"
                                                     << "Carina"
                                                     << "Draco"
                                                     << "Eridanus"
                                                     << "Gemini"
                                                     << "Hercules"
                                                     << "Orion"
                                                     << "Phoenix"
                                                     << "Scorpio"
                                                     << "Ursa Major"
                                                     << "Ursa Minor";
    for (int i = 0; i < constellations.count(); ++i)
        storage->setValue(2, 19 + i, Value(constellations[i]));
    // C18:C31
    storage->setValue(3, 18, Value("Bright Stars"));
    QList<int> stars = QList<int>() << 0 << 5 << 2 << 5 << 3 << 4 << 4 << 0 << 8 << 1 << 9 << 6 << 2;
    for (int i = 0; i < stars.count(); ++i)
        storage->setValue(3, 19 + i, Value(stars[i]));
    // B36:B37
    storage->setValue(2, 36, Value("Bright Stars"));
    storage->setValue(2, 37, Value(4));
    // D36:D37
    storage->setValue(4, 36, Value("Constellation"));
    storage->setValue(4, 37, Value("Ursa Major"));
}

void TestDatabaseFunctions::testDAVERAGE()
{
    CHECK_EVAL("=DAVERAGE(TESTDB; \"TestID\"; B36:B37)", Value(48));
}

void TestDatabaseFunctions::testDCOUNT()
{
    CHECK_EVAL("=DCOUNT(TESTDB; \"Bright Stars\"; B36:B37)", Value(2));
}

void TestDatabaseFunctions::testDCOUNTA()
{
    CHECK_EVAL("=DCOUNTA(TESTDB; \"Bright Stars\"; B36:B37)", Value(2));
}

void TestDatabaseFunctions::testDGET()
{
    CHECK_EVAL("=DGET(TESTDB; \"TestID\"; D36:D37)", Value(2048));
    CHECK_EVAL("=DGET(TESTDB; \"TestID\"; B36:B37)", Value::errorVALUE());
}

void TestDatabaseFunctions::testDMAX()
{
    CHECK_EVAL("=DMAX(TESTDB; \"TestID\"; B36:B37)", Value(64));
}

void TestDatabaseFunctions::testDMIN()
{
    CHECK_EVAL("=DMIN(TESTDB; \"TestID\"; B36:B37)", Value(32));
}

void TestDatabaseFunctions::testDPRODUCT()
{
    CHECK_EVAL("=DPRODUCT(TESTDB; \"TestID\"; B36:B37)", Value(2048));
}

void TestDatabaseFunctions::testDSTDEV()
{
    CHECK_EVAL("=DSTDEV(TESTDB; \"TestID\"; B36:B37)", Value(22.6274169979695));
}

void TestDatabaseFunctions::testDSTDEVP()
{
    CHECK_EVAL("=DSTDEVP(TESTDB; \"TestID\"; B36:B37)", Value(16));
}

void TestDatabaseFunctions::testDSUM()
{
    CHECK_EVAL("=DSUM(TESTDB; \"TestID\"; B36:B37)", Value(96));
}

void TestDatabaseFunctions::testDVAR()
{
    CHECK_EVAL("=DVAR(TESTDB; \"TestID\"; B36:B37)", Value(512));
}

void TestDatabaseFunctions::testDVARP()
{
    CHECK_EVAL("=DVARP(TESTDB; \"TestID\"; B36:B37)", Value(256));
}

void TestDatabaseFunctions::cleanupTestCase()
{
    delete m_map;
}

QTEST_MAIN(TestDatabaseFunctions)
