/* This file is part of the KDE project
   Copyright 2015 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef CALLIGRA_SHEETS_TEST_VALUEPARSER
#define CALLIGRA_SHEETS_TEST_VALUEPARSER

#include <QObject>

namespace Calligra
{
namespace Sheets
{

class CalculationSettings;
class ValueParser;

class TestValueParser: public QObject
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
    void testParse_data();
    void testParse();

private:
    CalculationSettings* m_calcsettings;
    ValueParser* m_parser;
    QString m_translationsFile;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_VALUEPARSER
