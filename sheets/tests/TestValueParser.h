// This file is part of the KDE project
// SPDX-FileCopyrightText: 2015 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_VALUEPARSER
#define CALLIGRA_SHEETS_TEST_VALUEPARSER

#include <QObject>

namespace Calligra
{
namespace Sheets
{

class CalculationSettings;
class ValueParser;

class TestValueParser : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testTryParseBool_data(bool addCol = true);
    void testTryParseBool();
    void testTryParseNumber_data(bool addCol = true);
    void testTryParseNumber();
    void testTryParseDate_data(bool addCol = true);
    void testTryParseDate();
    void testTryParseTime_data(bool addCol = true);
    void testTryParseTime();
    void testTryParseDateTime_data();
    void testTryParseDateTime();
    void testParse_data();
    void testParse();

private:
    CalculationSettings *m_calcsettings;
    ValueParser *m_parser;
    QString m_translationsFile;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_VALUEPARSER
