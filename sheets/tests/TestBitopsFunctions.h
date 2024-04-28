// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_BITOPS_FUNCTIONS
#define CALLIGRA_SHEETS_TEST_BITOPS_FUNCTIONS

#include <QObject>

#include <engine/Value.h>

namespace Calligra
{
namespace Sheets
{

class TestBitopsFunctions : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testBITAND();
    void testBITOR();
    void testBITXOR();
    void testBITLSHIFT();
    void testBITRSHIFT();

private:
    Value evaluate(const QString &, Value &ex);
};

} // namespace Sheets
} // namespace Calligra

#endif
