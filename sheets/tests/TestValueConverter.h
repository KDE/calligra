// This file is part of the KDE project
// SPDX-FileCopyrightText: 2015 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_VALUECONVERTER
#define CALLIGRA_SHEETS_TEST_VALUECONVERTER

#include <QObject>

namespace Calligra
{
namespace Sheets
{

class CalculationSettings;
class ValueParser;
class ValueConverter;

class TestValueConverter : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testAsBoolean_data();
    void testAsBoolean();
    void testAsInteger_data();
    void testAsInteger();
    void testAsFloat_data();
    void testAsFloat();
    void testAsComplex_data();
    void testAsComplex();
    void testAsNumeric_data();
    void testAsNumeric();
    void testAsString_data();
    void testAsString();
    void testAsDateTime_data();
    void testAsDateTime();
    void testAsDate_data();
    void testAsDate();
    void testAsTime_data();
    void testAsTime();

private:
    CalculationSettings *m_calcsettings;
    ValueParser *m_parser;
    ValueConverter *m_converter;
    QString m_translationsFile;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_VALUECONVERTER
