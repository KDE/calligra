// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_LOGIC_FUNCTIONS
#define CALLIGRA_SHEETS_TEST_LOGIC_FUNCTIONS

#include <QObject>

#include <engine/Value.h>

namespace Calligra
{
namespace Sheets
{
class MapBase;
class SheetBase;

class TestLogicFunctions : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();
    void cleanup();

    void initTestCase();
    void testAND();
    void testFALSE();
    void testIF();
    void testNOT();
    void testOR();
    void testTRUE();
    void testXOR();

private:
    MapBase *m_map;
    SheetBase *m_sheet;
    Value evaluate(const QString &, Value &ex);
};

} // namespace Sheets
} // namespace Calligra

#endif
