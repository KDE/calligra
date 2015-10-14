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
#include "TestValueConverter.h"

#include <ValueConverter.h>

#include <CalculationSettings.h>
#include <ValueParser.h>
#include <ValueStorage.h>

#include <klocale.h>

#include <QTest>

//Q_DECLARE_METATYPE(Calligra::Sheets::Format::Type)
//Q_DECLARE_METATYPE(Calligra::Sheets::Style::FloatFormat)

using namespace Calligra::Sheets;

namespace {

template<typename T>
Value ValueWithFormat(T i, Value::Format fmt)
{
    Value result(i);
    result.setFormat(fmt);
    return result;
}

} // namespace

void TestValueConverter::initTestCase()
{
    m_calcsettings = new CalculationSettings();
    m_parser = new ValueParser(m_calcsettings);
    m_converter = new ValueConverter(m_parser);

    // Custom reference date to make sure all date conversions use this date.
    m_calcsettings->setReferenceDate(QDate(2000, 1, 1));

    // Some tests need translations of certain words. Install some xx translations into test path
    // for some arbitrary language (nl).
    QStandardPaths::setTestModeEnabled(true);
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    QString localePath = dataPath + "/locale/nl/LC_MESSAGES";
    QVERIFY(QDir(localePath).mkpath("."));
    m_translationsFile = localePath + "/sheets.mo";
    QFile::copy(KDESRCDIR "/data/sheets.mo", m_translationsFile);
}

void TestValueConverter::cleanupTestCase()
{
    QStandardPaths::setTestModeEnabled(false);
    QFile::remove(m_translationsFile);

    delete m_converter;
    delete m_parser;
    delete m_calcsettings;
}

void TestValueConverter::testAsBoolean_data()
{
    QTest::addColumn<QString>("locale");
    QTest::addColumn<Value>("value");
    QTest::addColumn<bool>("expectedOk");
    QTest::addColumn<Value>("expected");

    QTest::newRow("empty") << "C" << Value() << true << Value(false);

    QTest::newRow("bool true") << "C" << Value(true) << true << Value(true);
    QTest::newRow("bool false") << "C" << Value(false) << true << Value(false);

    QTest::newRow("integer <0") << "C" << Value(-5) << true << Value(true);
    QTest::newRow("integer =0") << "C" << Value(0) << true << Value(false);
    QTest::newRow("integer >0") << "C" << Value(1) << true << Value(true);

    QTest::newRow("float <0") << "C" << Value(-0.00001) << true << Value(true);
    QTest::newRow("float =0") << "C" << Value(0.0) << true << Value(false);
    QTest::newRow("float >0") << "C" << Value(1e-99) << true << Value(true);

    QTest::newRow("complex -1 0i") << "C" << Value(complex<Number>(-1, 0)) << true << Value(true);
    QTest::newRow("complex  0 0i") << "C" << Value(complex<Number>(0, 0)) << true << Value(false);
    QTest::newRow("complex +1 0i") << "C" << Value(complex<Number>(1, 0)) << true << Value(true);
    QTest::newRow("complex -1 1i") << "C" << Value(complex<Number>(-1, 1)) << true << Value(true);
    QTest::newRow("complex  0 1i") << "C" << Value(complex<Number>(0, 1)) << true << Value(false);
    QTest::newRow("complex +1 1i") << "C" << Value(complex<Number>(1, 1)) << true << Value(true);

    QTest::newRow("string en true") << "C" << Value("true") << true << Value(true);
    QTest::newRow("string en false") << "C" << Value("false") << true << Value(false);
    QTest::newRow("string en foobar") << "C" << Value("foobar") << false << Value(false);
    QTest::newRow("string en TruE") << "C" << Value("TruE") << true << Value(true);
    QTest::newRow("string en fAlSe") << "C" << Value("fAlSe") << true << Value(false);
    QTest::newRow("string en xxtruexx") << "C" << Value("xxtruexx") << false << Value(false);
    QTest::newRow("string xx true") << "nl" << Value("true") << true << Value(true);
    QTest::newRow("string xx false") << "nl" << Value("false") << true << Value(false);
    QTest::newRow("string xx foobar") << "nl" << Value("foobar") << false << Value(false);
    QTest::newRow("string xx TruE") << "nl" << Value("TruE") << true << Value(true);
    QTest::newRow("string xx fAlSe") << "nl" << Value("fAlSe") << true << Value(false);
    QTest::newRow("string xx xxtruexx") << "nl" << Value("xxtruexx") << true << Value(true);
    QTest::newRow("string xx xxtRuexx") << "nl" << Value("xxtRuexx") << true << Value(true);
    QTest::newRow("string xx xxfalSexx") << "nl" << Value("xxfalSexx") << true << Value(false);

    ValueStorage array;
    QTest::newRow("array empty") << "C" << Value(array, QSize(1, 1)) << true << Value(false);
    array.insert(1, 1, Value(true));
    QTest::newRow("array true") << "C" << Value(array, QSize(1, 1)) << true << Value(true);

    QTest::newRow("errorCIRCLE") << "C" << Value::errorCIRCLE() << true << Value(false);
    QTest::newRow("errorNA") << "C" << Value::errorNA() << true << Value(false);
}

void TestValueConverter::testAsBoolean()
{
    QFETCH(QString, locale);
    QFETCH(Value, value);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);

    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);

    bool ok;
    Value result = m_converter->asBoolean(value, &ok);
    QCOMPARE(ok, expectedOk);
    QCOMPARE(result, expected);
}

void TestValueConverter::testAsInteger_data()
{
    QTest::addColumn<QString>("locale");
    QTest::addColumn<Value>("value");
    QTest::addColumn<bool>("expectedOk");
    QTest::addColumn<Value>("expected");

    QTest::newRow("empty") << "C" << Value() << true << Value(0);

    QTest::newRow("bool true") << "C" << Value(true) << true << Value(1);
    QTest::newRow("bool false") << "C" << Value(false) << true << Value(0);

    QTest::newRow("integer") << "C" << Value(94610) << true << Value(94610);

    QTest::newRow("float 0") << "C" << Value(0.0) << true << Value(0);
    QTest::newRow("float 3.3") << "C" << Value(3.3) << true << Value(3);
    QTest::newRow("float 123.9999") << "C" << Value(123.9999) << true << Value(123);
    QTest::newRow("float -45.65") << "C" << Value(-45.65) << true << Value(-46);

    QTest::newRow("complex 0 0i") << "C" << Value(complex<Number>(0, 0)) << true << Value(0);
    QTest::newRow("complex 1.6 -3i") << "C" << Value(complex<Number>(1.6, -3)) << true << Value(1);
    QTest::newRow("complex 2 4.6i") << "C" << Value(complex<Number>(2, 4.6)) << true << Value(2);
    QTest::newRow("complex -3.14 1i") << "C" << Value(complex<Number>(-3.14, 1)) << true << Value(-4);

    QTest::newRow("string 123") << "C" << Value("123") << true << Value(123);
    QTest::newRow("string -456") << "C" << Value("-456") << true << Value(-456);
    QTest::newRow("string +5") << "C" << Value("+5") << true << Value(5);
    QTest::newRow("string 1501%") << "C" << Value("1501%") << true << Value(15);
    QTest::newRow("string 5+3i") << "C" << Value("5+3i") << true << Value(5);
    QTest::newRow("string 2.4 + 3j") << "C" << Value("2.4 + 3j") << true << Value(2);
    QTest::newRow("string 6 - 3i") << "C" << Value("6 - 3i") << true << Value(6);
    QTest::newRow("string 2.4i") << "C" << Value("2.4i") << true << Value(0);
    QTest::newRow("string 1,4") << "C" << Value("1,4") << false << Value(0);
    QTest::newRow("string 1,400") << "C" << Value("1,400") << true << Value(1400);
    QTest::newRow("string 3 5/2") << "C" << Value("3 5/2") << true << Value(5);
    QTest::newRow("string 3e2") << "C" << Value("3e2") << true << Value(300);
    QTest::newRow("string 1234E-2") << "C" << Value("1234E-2") << true << Value(12);
    QTest::newRow("string nl 1,4") << "nl" << Value("1,4") << true << Value(1);
    QTest::newRow("string nl 1,400") << "nl" << Value("1,400") << true << Value(1);

    ValueStorage array;
    QTest::newRow("array empty") << "C" << Value(array, QSize(1, 1)) << true << Value(0);
    array.insert(1, 1, Value(543));
    QTest::newRow("array 543") << "C" << Value(array, QSize(1, 1)) << true << Value(543);

    QTest::newRow("errorCIRCLE") << "C" << Value::errorCIRCLE() << true << Value(0);
    QTest::newRow("errorNA") << "C" << Value::errorNA() << true << Value(0);
}

void TestValueConverter::testAsInteger()
{
    QFETCH(QString, locale);
    QFETCH(Value, value);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);

    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);

    bool ok;
    Value result = m_converter->asInteger(value, &ok);
    QCOMPARE(ok, expectedOk);
    QCOMPARE(result, expected);
}

void TestValueConverter::testAsFloat_data()
{
    QTest::addColumn<QString>("locale");
    QTest::addColumn<Value>("value");
    QTest::addColumn<bool>("expectedOk");
    QTest::addColumn<Value>("expected");

    QTest::newRow("empty") << "C" << Value() << true << Value(0.0);

    QTest::newRow("bool true") << "C" << Value(true) << true << Value(1.0);
    QTest::newRow("bool false") << "C" << Value(false) << true << Value(0.0);

    QTest::newRow("integer") << "C" << Value(94610) << true << Value(94610.0);

    QTest::newRow("float 0") << "C" << Value(0.0) << true << Value(0.0);
    QTest::newRow("float 3.3") << "C" << Value(3.3) << true << Value(3.3);
    QTest::newRow("float 123.9999") << "C" << Value(123.9999) << true << Value(123.9999);
    QTest::newRow("float -45.65") << "C" << Value(-45.65) << true << Value(-45.65);

    QTest::newRow("complex 0 0i") << "C" << Value(complex<Number>(0, 0)) << true << Value(0.0);
    QTest::newRow("complex 1.6 -3i") << "C" << Value(complex<Number>(1.6, -3)) << true << Value(1.6);
    QTest::newRow("complex 2 4.6i") << "C" << Value(complex<Number>(2, 4.6)) << true << Value(2.0);
    QTest::newRow("complex -3.14 1i") << "C" << Value(complex<Number>(-3.14, 1)) << true << Value(-3.14);

    QTest::newRow("string 123") << "C" << Value("123") << true << Value(123.0);
    QTest::newRow("string -456") << "C" << Value("-456") << true << Value(-456.0);
    QTest::newRow("string +5") << "C" << Value("+5") << true << Value(5.0);
    QTest::newRow("string 1525%") << "C" << Value("1525%") << true << Value(15.25);
    QTest::newRow("string 5+3i") << "C" << Value("5+3i") << true << Value(5.0);
    QTest::newRow("string 2.4 + 3j") << "C" << Value("2.4 + 3j") << true << Value(2.4);
    QTest::newRow("string 6 - 3i") << "C" << Value("6 - 3i") << true << Value(6.0);
    QTest::newRow("string 2.4i") << "C" << Value("2.4i") << true << Value(0.0);
    QTest::newRow("string 1,4") << "C" << Value("1,4") << false << Value(0.0);
    QTest::newRow("string 1,400") << "C" << Value("1,400") << true << Value(1400.0);
    QTest::newRow("string 3 5/2") << "C" << Value("3 5/2") << true << Value(5.5);
    QTest::newRow("string 3e2") << "C" << Value("3e2") << true << Value(300.0);
    QTest::newRow("string 1234E-2") << "C" << Value("1234E-2") << true << Value(12.34);
    QTest::newRow("string nl 1,4") << "nl" << Value("1,4") << true << Value(1.4);
    QTest::newRow("string nl 1,400") << "nl" << Value("1,400") << true << Value(1.4);

    ValueStorage array;
    QTest::newRow("array empty") << "C" << Value(array, QSize(1, 1)) << true << Value(0.0);
    array.insert(1, 1, Value(543.4));
    QTest::newRow("array 543.4") << "C" << Value(array, QSize(1, 1)) << true << Value(543.4);

    QTest::newRow("errorCIRCLE") << "C" << Value::errorCIRCLE() << true << Value(0.0);
    QTest::newRow("errorNA") << "C" << Value::errorNA() << true << Value(0.0);
}

void TestValueConverter::testAsFloat()
{
    QFETCH(QString, locale);
    QFETCH(Value, value);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);

    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);

    bool ok;
    Value result = m_converter->asFloat(value, &ok);
    QCOMPARE(ok, expectedOk);
    QCOMPARE(result, expected);
}

void TestValueConverter::testAsComplex_data()
{
    QTest::addColumn<QString>("locale");
    QTest::addColumn<Value>("value");
    QTest::addColumn<bool>("expectedOk");
    QTest::addColumn<Value>("expected");

    QTest::newRow("empty") << "C" << Value() << true << Value(complex<Number>(0, 0));

    QTest::newRow("bool true") << "C" << Value(true) << true << Value(complex<Number>(1, 0));
    QTest::newRow("bool false") << "C" << Value(false) << true << Value(complex<Number>(0, 0));

    QTest::newRow("integer") << "C" << Value(94610) << true << Value(complex<Number>(94610, 0));

    QTest::newRow("float 0") << "C" << Value(0.0)
        << true << Value(complex<Number>(0, 0));
    QTest::newRow("float 3.3") << "C" << Value(3.3)
        << true << Value(complex<Number>(3.3, 0));
    QTest::newRow("float 123.9999") << "C" << Value(123.9999)
        << true << Value(complex<Number>(123.9999, 0));
    QTest::newRow("float -45.65") << "C" << Value(-45.65)
        << true << Value(complex<Number>(-45.65, 0));

    QTest::newRow("complex 0 0i") << "C" << Value(complex<Number>(0, 0))
        << true << Value(complex<Number>(0, 0));
    QTest::newRow("complex 1.6 -3i") << "C" << Value(complex<Number>(1.6, -3))
        << true << Value(complex<Number>(1.6, -3));
    QTest::newRow("complex 2 4.6i") << "C" << Value(complex<Number>(2, 4.6))
        << true << Value(complex<Number>(2, 4.6));
    QTest::newRow("complex -3.14 1i") << "C" << Value(complex<Number>(-3.14, 1))
        << true << Value(complex<Number>(-3.14, 1));

    QTest::newRow("string 123") << "C" << Value("123")
        << true << Value(complex<Number>(123, 0));
    QTest::newRow("string -456") << "C" << Value("-456")
        << true << Value(complex<Number>(-456, 0));
    QTest::newRow("string +5") << "C" << Value("+5")
        << true << Value(complex<Number>(5, 0));
    QTest::newRow("string 1525%") << "C" << Value("1525%")
        << true << Value(complex<Number>(15.25, 0));
    QTest::newRow("string 5+3i") << "C" << Value("5+3i")
        << true << Value(complex<Number>(5, 3));
    QTest::newRow("string 2.4 + 3j") << "C" << Value("2.4 + 3j")
        << true << Value(complex<Number>(2.4, 3));
    QTest::newRow("string 6 - 3i") << "C" << Value("6 - 3i")
        << true << Value(complex<Number>(6, -3));
    QTest::newRow("string 2.4i") << "C" << Value("2.4i")
        << true << Value(complex<Number>(0, 2.4));
    QTest::newRow("string 1,4") << "C" << Value("1,4")
        << false << Value(complex<Number>(0, 0));
    QTest::newRow("string 1,400") << "C" << Value("1,400")
        << true << Value(complex<Number>(1400, 0));
    QTest::newRow("string 3 5/2") << "C" << Value("3 5/2")
        << true << Value(complex<Number>(5.5, 0));
    QTest::newRow("string 3e2") << "C" << Value("3e2")
        << true << Value(complex<Number>(300.0, 0));
    QTest::newRow("string 1234E-2") << "C" << Value("1234E-2")
        << true << Value(complex<Number>(12.34, 0));
    QTest::newRow("string 12.34e5 + 4.2e2i") << "C" << Value("12.34e5 + 4.2e2i")
        << true << Value(complex<Number>(12.34e5, 4.2e2));
    QTest::newRow("string 12.34e5 + 4.2e+2i") << "C" << Value("12.34e5 + 4.2e+2i")
        << true << Value(complex<Number>(12.34e5, 4.2e2));
    //QTest::newRow("string 12.34e+5 + 4.2e2i") << "C" << Value("12.34e+5 + 4.2e2i")
    //    << true << Value(complex<Number>(12.34e5, 4.2e2));
    //QTest::newRow("string 12.34e+5 + 4.2e+2i") << "C" << Value("12.34e+5 + 4.2e+2i")
    //    << true << Value(complex<Number>(12.34e5, 4.2e2));
    QTest::newRow("string nl 1,4") << "nl" << Value("1,4")
        << true << Value(complex<Number>(1.4, 0));
    QTest::newRow("string nl 1,400") << "nl" << Value("1,400")
        << true << Value(complex<Number>(1.4, 0));

    ValueStorage array;
    QTest::newRow("array empty") << "C" << Value(array, QSize(1, 1))
        << true << Value(complex<Number>(0, 0));
    array.insert(1, 1, Value(543.4));
    QTest::newRow("array 543.4") << "C" << Value(array, QSize(1, 1))
        << true << Value(complex<Number>(543.4, 0));

    QTest::newRow("errorCIRCLE") << "C" << Value::errorCIRCLE()
        << true << Value(complex<Number>(0, 0));
    QTest::newRow("errorNA") << "C" << Value::errorNA()
        << true << Value(complex<Number>(0, 0));
}

void TestValueConverter::testAsComplex()
{
    QFETCH(QString, locale);
    QFETCH(Value, value);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);

    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);

    bool ok;
    Value result = m_converter->asComplex(value, &ok);
    QCOMPARE(ok, expectedOk);
    QCOMPARE(result, expected);
}

void TestValueConverter::testAsNumeric_data()
{
    QTest::addColumn<QString>("locale");
    QTest::addColumn<Value>("value");
    QTest::addColumn<bool>("expectedOk");
    QTest::addColumn<Value>("expected");

    QTest::newRow("empty") << "C" << Value() << true << Value(0.0);

    QTest::newRow("bool true") << "C" << Value(true) << true << Value(1.0);
    QTest::newRow("bool false") << "C" << Value(false) << true << Value(0.0);

    QTest::newRow("integer") << "C" << Value(94610) << true << Value(94610);

    QTest::newRow("float 0") << "C" << Value(0.0) << true << Value(0.0);
    QTest::newRow("float 3.3") << "C" << Value(3.3) << true << Value(3.3);
    QTest::newRow("float 123.9999") << "C" << Value(123.9999) << true << Value(123.9999);
    QTest::newRow("float -45.65") << "C" << Value(-45.65) << true << Value(-45.65);

    QTest::newRow("complex 0 0i") << "C" << Value(complex<Number>(0, 0))
        << true << Value(complex<Number>(0, 0));
    QTest::newRow("complex 1.6 -3i") << "C" << Value(complex<Number>(1.6, -3))
        << true << Value(complex<Number>(1.6, -3));
    QTest::newRow("complex 2 4.6i") << "C" << Value(complex<Number>(2, 4.6))
        << true << Value(complex<Number>(2, 4.6));
    QTest::newRow("complex -3.14 1i") << "C" << Value(complex<Number>(-3.14, 1))
        << true << Value(complex<Number>(-3.14, 1));

    QTest::newRow("string 123") << "C" << Value("123") << true << Value(123);
    QTest::newRow("string -456") << "C" << Value("-456") << true << Value(-456);
    QTest::newRow("string +5") << "C" << Value("+5") << true << Value(5);
    QTest::newRow("string 1525%") << "C" << Value("1525%") << true << Value(15.25);
    QTest::newRow("string 5+3i") << "C" << Value("5+3i") << true << Value(complex<Number>(5, 3));
    QTest::newRow("string 2.4 + 3j") << "C" << Value("2.4 + 3j")
        << true << Value(complex<Number>(2.4, 3));
    QTest::newRow("string 6 - 3i") << "C" << Value("6 - 3i")
        << true << Value(complex<Number>(6, -3));
    QTest::newRow("string 2.4i") << "C" << Value("2.4i") << true << Value(complex<Number>(0, 2.4));
    QTest::newRow("string 1,4") << "C" << Value("1,4") << false << Value(0.0);
    QTest::newRow("string 1,400") << "C" << Value("1,400") << true << Value(1400);
    QTest::newRow("string 3 5/2") << "C" << Value("3 5/2") << true << Value(5.5);
    QTest::newRow("string 3e2") << "C" << Value("3e2") << true << Value(300.0);
    QTest::newRow("string 1234E-2") << "C" << Value("1234E-2") << true << Value(12.34);
    QTest::newRow("string nl 1,4") << "nl" << Value("1,4") << true << Value(1.4);
    QTest::newRow("string nl 1,400") << "nl" << Value("1,400") << true << Value(1.4);

    ValueStorage array;
    QTest::newRow("array empty") << "C" << Value(array, QSize(1, 1)) << true << Value(0.0);
    array.insert(1, 1, Value(543.4));
    QTest::newRow("array 543.4") << "C" << Value(array, QSize(1, 1)) << true << Value(543.4);

    QTest::newRow("errorCIRCLE") << "C" << Value::errorCIRCLE() << true << Value(0.0);
    QTest::newRow("errorNA") << "C" << Value::errorNA() << true << Value(0.0);
}

void TestValueConverter::testAsNumeric()
{
    QFETCH(QString, locale);
    QFETCH(Value, value);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);

    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);

    bool ok;
    Value result = m_converter->asNumeric(value, &ok);
    QCOMPARE(ok, expectedOk);
    QCOMPARE(result, expected);
}

void TestValueConverter::testAsString_data()
{
    QTest::addColumn<QString>("locale");
    QTest::addColumn<Value>("value");
    QTest::addColumn<Value>("expected");

    QTest::newRow("empty") << "C" << Value() << Value("");

    QTest::newRow("bool true") << "C" << Value(true) << Value("True");
    QTest::newRow("bool false") << "C" << Value(false) << Value("False");
    QTest::newRow("bool true xx") << "nl" << Value(true) << Value("xxTruexx");
    QTest::newRow("bool false xx") << "nl" << Value(false) << Value("xxFalsexx");

    QTest::newRow("integer plain") << "C" << Value(123) << Value("123");
    QTest::newRow("integer percent") << "C" << ValueWithFormat(3, Value::fmt_Percent)
        << Value("300 %");
    QTest::newRow("integer time") << "C" << ValueWithFormat(4, Value::fmt_Time) << Value("00:00");
    QTest::newRow("integer time us") << "us" << ValueWithFormat(4, Value::fmt_Time)
        << Value("12:00 AM");
    // TODO(mek): These next ones should almost certainly be using short date format.
    QTest::newRow("integer date 1") << "C" << ValueWithFormat(0, Value::fmt_Date)
        << Value("Saturday 01 January 2000");
    QTest::newRow("integer date 2") << "C" << ValueWithFormat(2000, Value::fmt_Date)
        << Value("Thursday 23 June 2005");
    QTest::newRow("integer date 3") << "C" << ValueWithFormat(-10, Value::fmt_Date)
        << Value("Wednesday 22 December 1999");
    // TODO(mek): KLocale doesn't take its language into account when formatting dates...
    //QTest::newRow("integer date 1 nl") << "nl" << ValueWithFormat(0, Value::fmt_Date)
    //    << Value("xxSaturdayxx 01 xxJanuaryxx 2000");
    //QTest::newRow("integer date 2 nl") << "nl" << ValueWithFormat(2000, Value::fmt_Date)
    //    << Value("xxThursdayxx 23 xxJunexx 2005");
    //QTest::newRow("integer date 3 nl") << "nl" << ValueWithFormat(-10, Value::fmt_Date)
    //    << Value("xxWednesdayxx 22 xxDecemberxx 1999");
    QTest::newRow("integer datetime 1") << "C" << ValueWithFormat(4, Value::fmt_DateTime)
        << Value("2000-01-05 00:00");
    QTest::newRow("integer datetime 1 us") << "us" << ValueWithFormat(4, Value::fmt_DateTime)
        << Value("01/05/00 12:00 AM");
    QTest::newRow("integer datetime 1 nl") << "nl" << ValueWithFormat(4, Value::fmt_DateTime)
        << Value("05/01/00 00:00");
    QTest::newRow("integer datetime 2") << "C" << ValueWithFormat(-10, Value::fmt_DateTime)
        << Value("1999-12-22 00:00");
    QTest::newRow("integer datetime 2 us") << "us" << ValueWithFormat(-10, Value::fmt_DateTime)
        << Value("12/22/99 12:00 AM");
    QTest::newRow("integer datetime 2 nl") << "nl" << ValueWithFormat(-10, Value::fmt_DateTime)
        << Value("22/12/99 00:00");

    QTest::newRow("float 123") << "C" << Value(123.0) << Value("123");
    QTest::newRow("float -3.14") << "C" << Value(-3.14) << Value("-3.14");
    QTest::newRow("float 1.5e99") << "C" << Value(1.5e99) << Value("1.5e+99");
    QTest::newRow("float 0.43e-12") << "C" << Value(0.43e-12) << Value("4.3e-13");
    QTest::newRow("float 123 nl") << "nl" << Value(123.0) << Value("123");
    QTest::newRow("float -3.14 nl") << "nl" << Value(-3.14) << Value("-3,14");
    QTest::newRow("float 1.5e99 nl") << "nl" << Value(1.5e99) << Value("1,5e+99");
    QTest::newRow("float 0.43e-12 nl") << "nl" << Value(0.43e-12) << Value("4,3e-13");
    // TODO(mek): Currently buggy/inconsistent in implementation.
    //QTest::newRow("float percent") << "C" << ValueWithFormat(3.45, Value::fmt_Percent)
    //    << Value("345 %");
    QTest::newRow("float time 0") << "C" << ValueWithFormat(4, Value::fmt_Time) << Value("00:00");
    QTest::newRow("float time 1") << "C" << ValueWithFormat(0.5, Value::fmt_Time) << Value("12:00");
    QTest::newRow("float time 2") << "C" << ValueWithFormat(3.675, Value::fmt_Time)
        << Value("16:12");
    QTest::newRow("float time 0 us") << "us" << ValueWithFormat(4.0, Value::fmt_Time)
        << Value("12:00 AM");
    QTest::newRow("float time 1 us") << "us" << ValueWithFormat(0.5, Value::fmt_Time)
        << Value("12:00 PM");
    QTest::newRow("float time 2 us") << "us" << ValueWithFormat(3.675, Value::fmt_Time)
        << Value("04:12 PM");
    QTest::newRow("float date 1") << "C" << ValueWithFormat(0.5, Value::fmt_Date)
        << Value("2000-01-01");
    QTest::newRow("float date 2") << "C" << ValueWithFormat(2000.324, Value::fmt_Date)
        << Value("2005-06-23");
    QTest::newRow("float date 3") << "C" << ValueWithFormat(-9.234, Value::fmt_Date)
        << Value("1999-12-22");
    QTest::newRow("float date 1 nl") << "nl" << ValueWithFormat(0.5, Value::fmt_Date)
        << Value("01/01/00");
    QTest::newRow("float date 2 nl") << "nl" << ValueWithFormat(2000.324, Value::fmt_Date)
        << Value("23/06/05");
    QTest::newRow("float date 3 us") << "us" << ValueWithFormat(-9.234, Value::fmt_Date)
        << Value("12/22/99");
    QTest::newRow("float datetime 0") << "C" << ValueWithFormat(4.0, Value::fmt_DateTime)
        << Value("2000-01-05 00:00");
    QTest::newRow("float datetime 1") << "C" << ValueWithFormat(2000.5, Value::fmt_DateTime)
        << Value("2005-06-23 12:00");
    QTest::newRow("float datetime 2") << "C" << ValueWithFormat(-9.325, Value::fmt_DateTime)
        << Value("1999-12-22 16:12");
    QTest::newRow("float datetime 0 us") << "us" << ValueWithFormat(4.0, Value::fmt_DateTime)
        << Value("01/05/00 12:00 AM");
    QTest::newRow("float datetime 1 us") << "us" << ValueWithFormat(2000.5, Value::fmt_DateTime)
        << Value("06/23/05 12:00 PM");
    QTest::newRow("float datetime 2 us") << "us" << ValueWithFormat(-9.325, Value::fmt_DateTime)
        << Value("12/22/99 04:12 PM");
    QTest::newRow("float datetime 0 nl") << "nl" << ValueWithFormat(4.0, Value::fmt_DateTime)
        << Value("05/01/00 00:00");
    QTest::newRow("float datetime 1 nl") << "nl" << ValueWithFormat(2000.5, Value::fmt_DateTime)
        << Value("23/06/05 12:00");
    QTest::newRow("float datetime 2 nl") << "nl" << ValueWithFormat(-9.325, Value::fmt_DateTime)
        << Value("22/12/99 16:12");

    QTest::newRow("complex 0+0i") << "C" << Value(complex<Number>(0, 0)) << Value("0+0i");
    QTest::newRow("complex 3.14-2.7i") << "C" << Value(complex<Number>(3.14, -2.7))
        << Value("3.14-2.7i");
    QTest::newRow("complex 2.2e99+3.3e88i") << "C" << Value(complex<Number>(2.2e99, 3.3e88))
        << Value("2.2e+99+3.3e+88i");
    QTest::newRow("complex time 0") << "C"
        << ValueWithFormat(complex<Number>(4, 3), Value::fmt_Time)
        << Value("00:00");
    QTest::newRow("complex time 1") << "C"
        << ValueWithFormat(complex<Number>(0.5, -3), Value::fmt_Time)
        << Value("12:00");
    QTest::newRow("complex time 2") << "C"
        << ValueWithFormat(complex<Number>(3.675, 653), Value::fmt_Time)
        << Value("16:12");
    QTest::newRow("complex time 0 us") << "us"
        << ValueWithFormat(complex<Number>(4, 634), Value::fmt_Time)
        << Value("12:00 AM");
    QTest::newRow("complex time 1 us") << "us"
        << ValueWithFormat(complex<Number>(0.5, 2.3), Value::fmt_Time)
        << Value("12:00 PM");
    QTest::newRow("complex time 2 us") << "us"
        << ValueWithFormat(complex<Number>(3.675, 2), Value::fmt_Time)
        << Value("04:12 PM");
    QTest::newRow("complex date 1") << "C"
        << ValueWithFormat(complex<Number>(0.5, 0), Value::fmt_Date)
        << Value("2000-01-01");
    QTest::newRow("complex date 2") << "C"
        << ValueWithFormat(complex<Number>(2000.324, 0), Value::fmt_Date)
        << Value("2005-06-23");
    QTest::newRow("complex date 3") << "C"
        << ValueWithFormat(complex<Number>(-9.234, 0), Value::fmt_Date)
        << Value("1999-12-22");
    QTest::newRow("complex date 1 nl") << "nl"
        << ValueWithFormat(complex<Number>(0.5, 0), Value::fmt_Date)
        << Value("01/01/00");
    QTest::newRow("complex date 2 nl") << "nl"
        << ValueWithFormat(complex<Number>(2000.324, 0), Value::fmt_Date)
        << Value("23/06/05");
    QTest::newRow("complex date 3 us") << "us"
        << ValueWithFormat(complex<Number>(-9.234, 0), Value::fmt_Date)
        << Value("12/22/99");
    QTest::newRow("complex datetime 0") << "C"
        << ValueWithFormat(complex<Number>(4.0, 0), Value::fmt_DateTime)
        << Value("2000-01-05 00:00");
    QTest::newRow("complex datetime 1") << "C"
        << ValueWithFormat(complex<Number>(2000.5, 0), Value::fmt_DateTime)
        << Value("2005-06-23 12:00");
    QTest::newRow("complex datetime 2") << "C"
        << ValueWithFormat(complex<Number>(-9.325, 0), Value::fmt_DateTime)
        << Value("1999-12-22 16:12");
    QTest::newRow("complex datetime 0 us") << "us"
        << ValueWithFormat(complex<Number>(4.0, 0), Value::fmt_DateTime)
        << Value("01/05/00 12:00 AM");
    QTest::newRow("complex datetime 1 us") << "us"
        << ValueWithFormat(complex<Number>(2000.5, 0), Value::fmt_DateTime)
        << Value("06/23/05 12:00 PM");
    QTest::newRow("complex datetime 2 us") << "us"
        << ValueWithFormat(complex<Number>(-9.325, 0), Value::fmt_DateTime)
        << Value("12/22/99 04:12 PM");

    QTest::newRow("string") << "C" << Value("foobar") << Value("foobar");

    ValueStorage array;
    QTest::newRow("array empty") << "C" << Value(array, QSize(1, 1)) << Value("");
    array.insert(1, 1, Value(123));
    QTest::newRow("array 123") << "C" << Value(array, QSize(1, 1)) << Value("123");

    QTest::newRow("errorCIRCLE") << "C" << Value::errorCIRCLE() << Value("#CIRCLE!");
    QTest::newRow("errorDEPEND") << "C" << Value::errorDEPEND() << Value("#DEPEND!");
    QTest::newRow("errorDIV0") << "C" << Value::errorDIV0() << Value("#DIV/0!");
    QTest::newRow("errorNA") << "C" << Value::errorNA() << Value("#N/A");
    QTest::newRow("errorNAME") << "C" << Value::errorNAME() << Value("#NAME?");
    QTest::newRow("errorNUM") << "C" << Value::errorNUM() << Value("#NUM!");
    QTest::newRow("errorNULL") << "C" << Value::errorNULL() << Value("#NULL!");
    QTest::newRow("errorPARSE") << "C" << Value::errorPARSE() << Value("#PARSE!");
    QTest::newRow("errorREF") << "C" << Value::errorREF() << Value("#REF!");
    QTest::newRow("errorVALUE") << "C" << Value::errorVALUE() << Value("#VALUE!");
}

void TestValueConverter::testAsString()
{
    QFETCH(QString, locale);
    QFETCH(Value, value);
    QFETCH(Value, expected);

    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);

    Value result = m_converter->asString(value);
    if (result != expected) kDebug() << result << " != " << expected;
    QCOMPARE(result, expected);
}

QTEST_MAIN(TestValueConverter)
