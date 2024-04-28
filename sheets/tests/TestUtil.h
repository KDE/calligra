// This file is part of the KDE project
// SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_UTIL
#define CALLIGRA_SHEETS_TEST_UTIL

#include <QObject>

namespace Calligra
{
namespace Sheets
{

class TestUtil : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testDecodeFormula_data();
    void testDecodeFormula();
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_UTIL
