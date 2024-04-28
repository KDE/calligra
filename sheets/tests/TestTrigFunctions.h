// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Ariya Hidayat <ariya@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_TRIG_FUNCTIONS
#define CALLIGRA_SHEETS_TEST_TRIG_FUNCTIONS

#include <QObject>

#include <engine/Value.h>

namespace Calligra
{
namespace Sheets
{

class TestTrigFunctions : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testCOS();
    void testCOSH();
    void testPI();
    void testSIN();
    void testSINH();
    void testTAN();
    void testCSC();
    void testCSCH();
    void testSEC();
    void testSECH();

private:
    Value evaluate(const QString &);
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_TRIG_FUNCTIONS
