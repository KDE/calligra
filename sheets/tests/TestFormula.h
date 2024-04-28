// This file is part of the KDE project
// SPDX-FileCopyrightText: 2004, 2007 Ariya Hidayat <ariya@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_FORMULA
#define CALLIGRA_SHEETS_TEST_FORMULA

#include <QObject>
#include <engine/Value.h>

namespace Calligra
{
namespace Sheets
{
class SheetBase;

class TestFormula : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testTokenizer();
    void testConstant();
    void testWhitespace();
    void testInvalid();
    void testUnary();
    void testBinary();
    void testOperators();
    void testComparison();
    void testString();
    void testReferences();
    void testFunction();
    void testInlineArrays();

private:
    Value evaluate(const QString &, Value &);
    SheetBase *m_sheet;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_FORMULA
