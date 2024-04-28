// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_VALUEFORMATTER
#define CALLIGRA_SHEETS_TEST_VALUEFORMATTER

#include <QObject>

namespace Calligra
{
namespace Sheets
{

class CalculationSettings;
class ValueParser;
class ValueConverter;

class TestValueFormatter : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testFractionFormat_data();
    void testFractionFormat();
    void testCreateNumberFormat_data();
    void testCreateNumberFormat();

private:
    CalculationSettings *m_calcsettings;
    ValueParser *m_parser;
    ValueConverter *m_converter;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_VALUEFORMATTER
