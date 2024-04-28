// This file is part of the KDE project
// SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_VALUE
#define CALLIGRA_SHEETS_TEST_VALUE

#include <QObject>

namespace Calligra
{
namespace Sheets
{

class TestValue : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testEmpty();
    void testBoolean();
    void testInteger();
    void testFloat();
    void testString();
    void testDate();
    void testTime();
    void testError();
    void testArray();
    void testCopy();
    void testAssignment();
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_VALUE
