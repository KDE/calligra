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

class TestValueConverter: public QObject
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
    CalculationSettings* m_calcsettings;
    ValueParser* m_parser;
    ValueConverter* m_converter;
    QString m_translationsFile;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_VALUECONVERTER
