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

#include <locale.h>

Q_DECLARE_METATYPE(complex<Number>)

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

    QStandardPaths::setTestModeEnabled(true);

    // If run with 'C' locale translations will fail
    // Setting it to 'C.UTF-8' fixes this
    char *l = setlocale(LC_MESSAGES, 0);
    if (l && strcmp(l, "C") == 0) {
        setlocale(LC_MESSAGES, "C.UTF-8");
        qDebug()<<"Set locale:"<<l<<"->"<<setlocale(LC_MESSAGES, 0);
    }

    // Some tests need translations of certain words.
    // These are available in .mo file in the data directory.
    // Install these xx translations into test path for some arbitrary language.
    // We use the 'nl'.
    QString locale = "nl";
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    QString localePath = dataPath + "/locale/" + locale + "/LC_MESSAGES";
    QVERIFY(QDir(localePath).mkpath("."));
    m_translationsFile = localePath + "/calligrasheets.mo";
    if (QFile::exists(m_translationsFile)) {
        QFile::remove(m_translationsFile);
    }
    // NOTE: sheets.mo -> calligrasheets.mo. Maybe rename sheets.mo
    QVERIFY(QFile::copy(QFINDTESTDATA("data/sheets.mo"), m_translationsFile));

    // check that translation ok, else lot of tests will fail later
    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);
    QString s = ki18n("true").toString(QStringList()<<locale);
    QVERIFY2(s == QString("xxtruexx"), "Translation failed, check that you have the correct .mo file in the data directory and that it installs correctly");
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
    QTest::addColumn<bool>("expected");

    QTest::newRow("empty") << "C" << Value() << true << false;

    QTest::newRow("bool true") << "C" << Value(true) << true << true;
    QTest::newRow("bool false") << "C" << Value(false) << true << false;

    QTest::newRow("integer <0") << "C" << Value(-5) << true << true;
    QTest::newRow("integer =0") << "C" << Value(0) << true << false;
    QTest::newRow("integer >0") << "C" << Value(1) << true << true;

    QTest::newRow("float <0") << "C" << Value(-0.00001) << true << true;
    QTest::newRow("float =0") << "C" << Value(0.0) << true << false;
    QTest::newRow("float >0") << "C" << Value(1e-99) << true << true;

    QTest::newRow("complex -1 0i") << "C" << Value(complex<Number>(-1, 0)) << true << true;
    QTest::newRow("complex  0 0i") << "C" << Value(complex<Number>(0, 0)) << true << false;
    QTest::newRow("complex +1 0i") << "C" << Value(complex<Number>(1, 0)) << true << true;
    QTest::newRow("complex -1 1i") << "C" << Value(complex<Number>(-1, 1)) << true << true;
    QTest::newRow("complex  0 1i") << "C" << Value(complex<Number>(0, 1)) << true << false;
    QTest::newRow("complex +1 1i") << "C" << Value(complex<Number>(1, 1)) << true << true;

    QTest::newRow("string en true") << "C" << Value("true") << true << true;
    QTest::newRow("string en false") << "C" << Value("false") << true << false;
    QTest::newRow("string en foobar") << "C" << Value("foobar") << false << false;
    QTest::newRow("string en TruE") << "C" << Value("TruE") << true << true;
    QTest::newRow("string en fAlSe") << "C" << Value("fAlSe") << true << false;
    QTest::newRow("string en xxtruexx") << "C" << Value("xxtruexx") << false << false;
    QTest::newRow("string xx true") << "nl" << Value("true") << true << true;
    QTest::newRow("string xx false") << "nl" << Value("false") << true << false;
    QTest::newRow("string xx foobar") << "nl" << Value("foobar") << false << false;
    QTest::newRow("string xx TruE") << "nl" << Value("TruE") << true << true;
    QTest::newRow("string xx fAlSe") << "nl" << Value("fAlSe") << true << false;
    QTest::newRow("string xx xxtruexx") << "nl" << Value("xxtruexx") << true << true;
    QTest::newRow("string xx xxtRuexx") << "nl" << Value("xxtRuexx") << true << true;
    QTest::newRow("string xx xxfalSexx") << "nl" << Value("xxfalSexx") << true << false;

    ValueStorage array;
    QTest::newRow("array empty") << "C" << Value(array, QSize(1, 1)) << true << false;
    array.insert(1, 1, Value(true));
    QTest::newRow("array true") << "C" << Value(array, QSize(1, 1)) << true << true;

    QTest::newRow("errorCIRCLE") << "C" << Value::errorCIRCLE() << true << false;
    QTest::newRow("errorNA") << "C" << Value::errorNA() << true << false;
}

void TestValueConverter::testAsBoolean()
{
    QFETCH(QString, locale);
    QFETCH(Value, value);
    QFETCH(bool, expectedOk);
    QFETCH(bool, expected);

    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);

    bool ok;
    Value result = m_converter->asBoolean(value, &ok);
    QCOMPARE(ok, expectedOk);
    QCOMPARE(result, Value(expected));
    QCOMPARE(m_converter->toBoolean(value), expected);
}

void TestValueConverter::testAsInteger_data()
{
    QTest::addColumn<QString>("locale");
    QTest::addColumn<Value>("value");
    QTest::addColumn<bool>("expectedOk");
    QTest::addColumn<int>("expected");

    QTest::newRow("empty") << "C" << Value() << true << 0;

    QTest::newRow("bool true") << "C" << Value(true) << true << 1;
    QTest::newRow("bool false") << "C" << Value(false) << true << 0;

    QTest::newRow("integer") << "C" << Value(94610) << true << 94610;

    QTest::newRow("float 0") << "C" << Value(0.0) << true << 0;
    QTest::newRow("float 3.3") << "C" << Value(3.3) << true << 3;
    QTest::newRow("float 123.9999") << "C" << Value(123.9999) << true << 123;
    QTest::newRow("float -45.65") << "C" << Value(-45.65) << true << -46;

    QTest::newRow("complex 0 0i") << "C" << Value(complex<Number>(0, 0)) << true << 0;
    QTest::newRow("complex 1.6 -3i") << "C" << Value(complex<Number>(1.6, -3)) << true << 1;
    QTest::newRow("complex 2 4.6i") << "C" << Value(complex<Number>(2, 4.6)) << true << 2;
    QTest::newRow("complex -3.14 1i") << "C" << Value(complex<Number>(-3.14, 1)) << true << -4;

    QTest::newRow("string 123") << "C" << Value("123") << true << 123;
    QTest::newRow("string -456") << "C" << Value("-456") << true << -456;
    QTest::newRow("string +5") << "C" << Value("+5") << true << 5;
    QTest::newRow("string 1501%") << "C" << Value("1501%") << true << 15;
    QTest::newRow("string 5+3i") << "C" << Value("5+3i") << true << 5;
    QTest::newRow("string 2.4 + 3j") << "C" << Value("2.4 + 3j") << true << 2;
    QTest::newRow("string 6 - 3i") << "C" << Value("6 - 3i") << true << 6;
    QTest::newRow("string 2.4i") << "C" << Value("2.4i") << true << 0;
    QTest::newRow("string 1,4") << "C" << Value("1,4") << false << 0;
    QTest::newRow("string 1,400") << "C" << Value("1,400") << true << 1400;
    QTest::newRow("string 3 5/2") << "C" << Value("3 5/2") << true << 5;
    QTest::newRow("string 3e2") << "C" << Value("3e2") << true << 300;
    QTest::newRow("string 1234E-2") << "C" << Value("1234E-2") << true << 12;
    QTest::newRow("string nl 1,4") << "nl" << Value("1,4") << true << 1;
    QTest::newRow("string nl 1,400") << "nl" << Value("1,400") << true << 1;

    ValueStorage array;
    QTest::newRow("array empty") << "C" << Value(array, QSize(1, 1)) << true << 0;
    array.insert(1, 1, Value(543));
    QTest::newRow("array 543") << "C" << Value(array, QSize(1, 1)) << true << 543;

    QTest::newRow("errorCIRCLE") << "C" << Value::errorCIRCLE() << true << 0;
    QTest::newRow("errorNA") << "C" << Value::errorNA() << true << 0;
}

void TestValueConverter::testAsInteger()
{
    QFETCH(QString, locale);
    QFETCH(Value, value);
    QFETCH(bool, expectedOk);
    QFETCH(int, expected);

    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);

    bool ok;
    Value result = m_converter->asInteger(value, &ok);
    QCOMPARE(ok, expectedOk);
    QCOMPARE(result, Value(expected));
    QCOMPARE(m_converter->toInteger(value), expected);
}

void TestValueConverter::testAsFloat_data()
{
    QTest::addColumn<QString>("locale");
    QTest::addColumn<Value>("value");
    QTest::addColumn<bool>("expectedOk");
    QTest::addColumn<double>("expected");

    QTest::newRow("empty") << "C" << Value() << true << 0.0;

    QTest::newRow("bool true") << "C" << Value(true) << true << 1.0;
    QTest::newRow("bool false") << "C" << Value(false) << true << 0.0;

    QTest::newRow("integer") << "C" << Value(94610) << true << 94610.0;

    QTest::newRow("float 0") << "C" << Value(0.0) << true << 0.0;
    QTest::newRow("float 3.3") << "C" << Value(3.3) << true << 3.3;
    QTest::newRow("float 123.9999") << "C" << Value(123.9999) << true << 123.9999;
    QTest::newRow("float -45.65") << "C" << Value(-45.65) << true << -45.65;

    QTest::newRow("complex 0 0i") << "C" << Value(complex<Number>(0, 0)) << true << 0.0;
    QTest::newRow("complex 1.6 -3i") << "C" << Value(complex<Number>(1.6, -3)) << true << 1.6;
    QTest::newRow("complex 2 4.6i") << "C" << Value(complex<Number>(2, 4.6)) << true << 2.0;
    QTest::newRow("complex -3.14 1i") << "C" << Value(complex<Number>(-3.14, 1)) << true << -3.14;

    QTest::newRow("string 123") << "C" << Value("123") << true << 123.0;
    QTest::newRow("string -456") << "C" << Value("-456") << true << -456.0;
    QTest::newRow("string +5") << "C" << Value("+5") << true << 5.0;
    QTest::newRow("string 1525%") << "C" << Value("1525%") << true << 15.25;
    QTest::newRow("string 5+3i") << "C" << Value("5+3i") << true << 5.0;
    QTest::newRow("string 2.4 + 3j") << "C" << Value("2.4 + 3j") << true << 2.4;
    QTest::newRow("string 6 - 3i") << "C" << Value("6 - 3i") << true << 6.0;
    QTest::newRow("string 2.4i") << "C" << Value("2.4i") << true << 0.0;
    QTest::newRow("string 1,4") << "C" << Value("1,4") << false << 0.0;
    QTest::newRow("string 1,400") << "C" << Value("1,400") << true << 1400.0;
    QTest::newRow("string 3 5/2") << "C" << Value("3 5/2") << true << 5.5;
    QTest::newRow("string 3e2") << "C" << Value("3e2") << true << 300.0;
    QTest::newRow("string 1234E-2") << "C" << Value("1234E-2") << true << 12.34;
    QTest::newRow("string nl 1,4") << "nl" << Value("1,4") << true << 1.4;
    QTest::newRow("string nl 1,400") << "nl" << Value("1,400") << true << 1.4;

    ValueStorage array;
    QTest::newRow("array empty") << "C" << Value(array, QSize(1, 1)) << true << 0.0;
    array.insert(1, 1, Value(543.4));
    QTest::newRow("array 543.4") << "C" << Value(array, QSize(1, 1)) << true << 543.4;

    QTest::newRow("errorCIRCLE") << "C" << Value::errorCIRCLE() << true << 0.0;
    QTest::newRow("errorNA") << "C" << Value::errorNA() << true << 0.0;
}

void TestValueConverter::testAsFloat()
{
    QFETCH(QString, locale);
    QFETCH(Value, value);
    QFETCH(bool, expectedOk);
    QFETCH(double, expected);

    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);

    bool ok;
    Value result = m_converter->asFloat(value, &ok);
    QCOMPARE(ok, expectedOk);
    QCOMPARE(result, Value(expected));
    QCOMPARE(m_converter->toFloat(value), Number(expected));
}

void TestValueConverter::testAsComplex_data()
{
    QTest::addColumn<QString>("locale");
    QTest::addColumn<Value>("value");
    QTest::addColumn<bool>("expectedOk");
    QTest::addColumn<complex<Number>>("expected");

    QTest::newRow("empty") << "C" << Value() << true << complex<Number>(0, 0);

    QTest::newRow("bool true") << "C" << Value(true) << true << complex<Number>(1, 0);
    QTest::newRow("bool false") << "C" << Value(false) << true << complex<Number>(0, 0);

    QTest::newRow("integer") << "C" << Value(94610) << true << complex<Number>(94610, 0);

    QTest::newRow("float 0") << "C" << Value(0.0) << true << complex<Number>(0, 0);
    QTest::newRow("float 3.3") << "C" << Value(3.3) << true << complex<Number>(3.3, 0);
    QTest::newRow("float 123.9999") << "C" << Value(123.9999)
        << true << complex<Number>(123.9999, 0);
    QTest::newRow("float -45.65") << "C" << Value(-45.65)
        << true << complex<Number>(-45.65, 0);

    QTest::newRow("complex 0 0i") << "C" << Value(complex<Number>(0, 0))
        << true << complex<Number>(0, 0);
    QTest::newRow("complex 1.6 -3i") << "C" << Value(complex<Number>(1.6, -3))
        << true << complex<Number>(1.6, -3);
    QTest::newRow("complex 2 4.6i") << "C" << Value(complex<Number>(2, 4.6))
        << true << complex<Number>(2, 4.6);
    QTest::newRow("complex -3.14 1i") << "C" << Value(complex<Number>(-3.14, 1))
        << true << complex<Number>(-3.14, 1);

    QTest::newRow("string 123") << "C" << Value("123")
        << true << complex<Number>(123, 0);
    QTest::newRow("string -456") << "C" << Value("-456")
        << true << complex<Number>(-456, 0);
    QTest::newRow("string +5") << "C" << Value("+5")
        << true << complex<Number>(5, 0);
    QTest::newRow("string 1525%") << "C" << Value("1525%")
        << true << complex<Number>(15.25, 0);
    QTest::newRow("string 5+3i") << "C" << Value("5+3i")
        << true << complex<Number>(5, 3);
    QTest::newRow("string 2.4 + 3j") << "C" << Value("2.4 + 3j")
        << true << complex<Number>(2.4, 3);
    QTest::newRow("string 6 - 3i") << "C" << Value("6 - 3i")
        << true << complex<Number>(6, -3);
    QTest::newRow("string 2.4i") << "C" << Value("2.4i")
        << true << complex<Number>(0, 2.4);
    QTest::newRow("string 1,4") << "C" << Value("1,4")
        << false << complex<Number>(0, 0);
    QTest::newRow("string 1,400") << "C" << Value("1,400")
        << true << complex<Number>(1400, 0);
    QTest::newRow("string 3 5/2") << "C" << Value("3 5/2")
        << true << complex<Number>(5.5, 0);
    QTest::newRow("string 3e2") << "C" << Value("3e2")
        << true << complex<Number>(300.0, 0);
    QTest::newRow("string 1234E-2") << "C" << Value("1234E-2")
        << true << complex<Number>(12.34, 0);
    QTest::newRow("string 12.34e5 + 4.2e2i") << "C" << Value("12.34e5 + 4.2e2i")
        << true << complex<Number>(12.34e5, 4.2e2);
    QTest::newRow("string 12.34e5 + 4.2e+2i") << "C" << Value("12.34e5 + 4.2e+2i")
        << true << complex<Number>(12.34e5, 4.2e2);
    //QTest::newRow("string 12.34e+5 + 4.2e2i") << "C" << Value("12.34e+5 + 4.2e2i")
    //    << true << complex<Number>(12.34e5, 4.2e2);
    //QTest::newRow("string 12.34e+5 + 4.2e+2i") << "C" << Value("12.34e+5 + 4.2e+2i")
    //    << true << complex<Number>(12.34e5, 4.2e2);
    QTest::newRow("string nl 1,4") << "nl" << Value("1,4")
        << true << complex<Number>(1.4, 0);
    QTest::newRow("string nl 1,400") << "nl" << Value("1,400")
        << true << complex<Number>(1.4, 0);

    ValueStorage array;
    QTest::newRow("array empty") << "C" << Value(array, QSize(1, 1))
        << true << complex<Number>(0, 0);
    array.insert(1, 1, Value(543.4));
    QTest::newRow("array 543.4") << "C" << Value(array, QSize(1, 1))
        << true << complex<Number>(543.4, 0);

    QTest::newRow("errorCIRCLE") << "C" << Value::errorCIRCLE()
        << true << complex<Number>(0, 0);
    QTest::newRow("errorNA") << "C" << Value::errorNA()
        << true << complex<Number>(0, 0);
}

void TestValueConverter::testAsComplex()
{
    QFETCH(QString, locale);
    QFETCH(Value, value);
    QFETCH(bool, expectedOk);
    QFETCH(complex<Number>, expected);

    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);

    bool ok;
    Value result = m_converter->asComplex(value, &ok);
    QCOMPARE(ok, expectedOk);
    QCOMPARE(result, Value(expected));
    QCOMPARE(m_converter->toComplex(value), expected);
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
    QTest::addColumn<QString>("expected");

    QTest::newRow("empty") << "C" << Value() << "";

    QTest::newRow("bool True") << "C" << Value(true) << "True";
    QTest::newRow("bool False") << "C" << Value(false) << "False";
    QTest::newRow("bool True xx") << "nl" << Value(true) << "xxTruexx";
    QTest::newRow("bool False xx") << "nl" << Value(false) << "xxFalsexx";

    QTest::newRow("integer plain") << "C" << Value(123) << "123";
    QTest::newRow("integer percent") << "C" << ValueWithFormat(3, Value::fmt_Percent)
        << "300 %";
    QTest::newRow("integer time") << "C" << ValueWithFormat(4, Value::fmt_Time) << "00:00";
    QTest::newRow("integer time us") << "us" << ValueWithFormat(4, Value::fmt_Time)
        << "12:00 AM";
    // TODO(mek): These next ones should almost certainly be using short date format.
    QTest::newRow("integer date 1") << "C" << ValueWithFormat(0, Value::fmt_Date)
        << "Saturday 01 January 2000";
    QTest::newRow("integer date 2") << "C" << ValueWithFormat(2000, Value::fmt_Date)
        << "Thursday 23 June 2005";
    QTest::newRow("integer date 3") << "C" << ValueWithFormat(-10, Value::fmt_Date)
        << "Wednesday 22 December 1999";
    // TODO(mek): KLocale doesn't take its language into account when formatting dates...
    //QTest::newRow("integer date 1 nl") << "nl" << ValueWithFormat(0, Value::fmt_Date)
    //    << "xxSaturdayxx 01 xxJanuaryxx 2000";
    //QTest::newRow("integer date 2 nl") << "nl" << ValueWithFormat(2000, Value::fmt_Date)
    //    << "xxThursdayxx 23 xxJunexx 2005";
    //QTest::newRow("integer date 3 nl") << "nl" << ValueWithFormat(-10, Value::fmt_Date)
    //    << "xxWednesdayxx 22 xxDecemberxx 1999";
    QTest::newRow("integer datetime 1") << "C" << ValueWithFormat(4, Value::fmt_DateTime)
        << "2000-01-05 00:00";
    QTest::newRow("integer datetime 1 us") << "us" << ValueWithFormat(4, Value::fmt_DateTime)
        << "01/05/00 12:00 AM";
    QTest::newRow("integer datetime 1 nl") << "nl" << ValueWithFormat(4, Value::fmt_DateTime)
        << "05/01/00 00:00";
    QTest::newRow("integer datetime 2") << "C" << ValueWithFormat(-10, Value::fmt_DateTime)
        << "1999-12-22 00:00";
    QTest::newRow("integer datetime 2 us") << "us" << ValueWithFormat(-10, Value::fmt_DateTime)
        << "12/22/99 12:00 AM";
    QTest::newRow("integer datetime 2 nl") << "nl" << ValueWithFormat(-10, Value::fmt_DateTime)
        << "22/12/99 00:00";

    QTest::newRow("float 123") << "C" << Value(123.0) << "123";
    QTest::newRow("float -3.14") << "C" << Value(-3.14) << "-3.14";
    QTest::newRow("float 1.5e99") << "C" << Value(1.5e99) << "1.5e+99";
    QTest::newRow("float 0.43e-12") << "C" << Value(0.43e-12) << "4.3e-13";
    QTest::newRow("float 123 nl") << "nl" << Value(123.0) << "123";
    QTest::newRow("float -3.14 nl") << "nl" << Value(-3.14) << "-3,14";
    QTest::newRow("float 1.5e99 nl") << "nl" << Value(1.5e99) << "1,5e+99";
    QTest::newRow("float 0.43e-12 nl") << "nl" << Value(0.43e-12) << "4,3e-13";
    // TODO(mek): Currently buggy/inconsistent in implementation.
    //QTest::newRow("float percent") << "C" << ValueWithFormat(3.45, Value::fmt_Percent)
    //    << "345 %";
    QTest::newRow("float time 0") << "C" << ValueWithFormat(4, Value::fmt_Time) << "00:00";
    QTest::newRow("float time 1") << "C" << ValueWithFormat(0.5, Value::fmt_Time) << "12:00";
    QTest::newRow("float time 2") << "C" << ValueWithFormat(3.675, Value::fmt_Time) << "16:12";
    QTest::newRow("float time 0 us") << "us" << ValueWithFormat(4.0, Value::fmt_Time) << "12:00 AM";
    QTest::newRow("float time 1 us") << "us" << ValueWithFormat(0.5, Value::fmt_Time) << "12:00 PM";
    QTest::newRow("float time 2 us") << "us" << ValueWithFormat(3.675, Value::fmt_Time)
        << "04:12 PM";
    QTest::newRow("float date 1") << "C" << ValueWithFormat(0.5, Value::fmt_Date) << "2000-01-01";
    QTest::newRow("float date 2") << "C" << ValueWithFormat(2000.324, Value::fmt_Date)
        << "2005-06-23";
    QTest::newRow("float date 3") << "C" << ValueWithFormat(-9.234, Value::fmt_Date)
        << "1999-12-22";
    QTest::newRow("float date 1 nl") << "nl" << ValueWithFormat(0.5, Value::fmt_Date)
        << "01/01/00";
    QTest::newRow("float date 2 nl") << "nl" << ValueWithFormat(2000.324, Value::fmt_Date)
        << "23/06/05";
    QTest::newRow("float date 3 us") << "us" << ValueWithFormat(-9.234, Value::fmt_Date)
        << "12/22/99";
    QTest::newRow("float datetime 0") << "C" << ValueWithFormat(4.0, Value::fmt_DateTime)
        << "2000-01-05 00:00";
    QTest::newRow("float datetime 1") << "C" << ValueWithFormat(2000.5, Value::fmt_DateTime)
        << "2005-06-23 12:00";
    QTest::newRow("float datetime 2") << "C" << ValueWithFormat(-9.325, Value::fmt_DateTime)
        << "1999-12-22 16:12";
    QTest::newRow("float datetime 0 us") << "us" << ValueWithFormat(4.0, Value::fmt_DateTime)
        << "01/05/00 12:00 AM";
    QTest::newRow("float datetime 1 us") << "us" << ValueWithFormat(2000.5, Value::fmt_DateTime)
        << "06/23/05 12:00 PM";
    QTest::newRow("float datetime 2 us") << "us" << ValueWithFormat(-9.325, Value::fmt_DateTime)
        << "12/22/99 04:12 PM";
    QTest::newRow("float datetime 0 nl") << "nl" << ValueWithFormat(4.0, Value::fmt_DateTime)
        << "05/01/00 00:00";
    QTest::newRow("float datetime 1 nl") << "nl" << ValueWithFormat(2000.5, Value::fmt_DateTime)
        << "23/06/05 12:00";
    QTest::newRow("float datetime 2 nl") << "nl" << ValueWithFormat(-9.325, Value::fmt_DateTime)
        << "22/12/99 16:12";

    QTest::newRow("complex 0+0i") << "C" << Value(complex<Number>(0, 0)) << "0+0i";
    QTest::newRow("complex 3.14-2.7i") << "C" << Value(complex<Number>(3.14, -2.7)) << "3.14-2.7i";
    QTest::newRow("complex 2.2e99+3.3e88i") << "C" << Value(complex<Number>(2.2e99, 3.3e88))
        << "2.2e+99+3.3e+88i";
    QTest::newRow("complex time 0") << "C"
        << ValueWithFormat(complex<Number>(4, 3), Value::fmt_Time)
        << "00:00";
    QTest::newRow("complex time 1") << "C"
        << ValueWithFormat(complex<Number>(0.5, -3), Value::fmt_Time)
        << "12:00";
    QTest::newRow("complex time 2") << "C"
        << ValueWithFormat(complex<Number>(3.675, 653), Value::fmt_Time)
        << "16:12";
    QTest::newRow("complex time 0 us") << "us"
        << ValueWithFormat(complex<Number>(4, 634), Value::fmt_Time)
        << "12:00 AM";
    QTest::newRow("complex time 1 us") << "us"
        << ValueWithFormat(complex<Number>(0.5, 2.3), Value::fmt_Time)
        << "12:00 PM";
    QTest::newRow("complex time 2 us") << "us"
        << ValueWithFormat(complex<Number>(3.675, 2), Value::fmt_Time)
        << "04:12 PM";
    QTest::newRow("complex date 1") << "C"
        << ValueWithFormat(complex<Number>(0.5, 0), Value::fmt_Date)
        << "2000-01-01";
    QTest::newRow("complex date 2") << "C"
        << ValueWithFormat(complex<Number>(2000.324, 0), Value::fmt_Date)
        << "2005-06-23";
    QTest::newRow("complex date 3") << "C"
        << ValueWithFormat(complex<Number>(-9.234, 0), Value::fmt_Date)
        << "1999-12-22";
    QTest::newRow("complex date 1 nl") << "nl"
        << ValueWithFormat(complex<Number>(0.5, 0), Value::fmt_Date)
        << "01/01/00";
    QTest::newRow("complex date 2 nl") << "nl"
        << ValueWithFormat(complex<Number>(2000.324, 0), Value::fmt_Date)
        << "23/06/05";
    QTest::newRow("complex date 3 us") << "us"
        << ValueWithFormat(complex<Number>(-9.234, 0), Value::fmt_Date)
        << "12/22/99";
    QTest::newRow("complex datetime 0") << "C"
        << ValueWithFormat(complex<Number>(4.0, 0), Value::fmt_DateTime)
        << "2000-01-05 00:00";
    QTest::newRow("complex datetime 1") << "C"
        << ValueWithFormat(complex<Number>(2000.5, 0), Value::fmt_DateTime)
        << "2005-06-23 12:00";
    QTest::newRow("complex datetime 2") << "C"
        << ValueWithFormat(complex<Number>(-9.325, 0), Value::fmt_DateTime)
        << "1999-12-22 16:12";
    QTest::newRow("complex datetime 0 us") << "us"
        << ValueWithFormat(complex<Number>(4.0, 0), Value::fmt_DateTime)
        << "01/05/00 12:00 AM";
    QTest::newRow("complex datetime 1 us") << "us"
        << ValueWithFormat(complex<Number>(2000.5, 0), Value::fmt_DateTime)
        << "06/23/05 12:00 PM";
    QTest::newRow("complex datetime 2 us") << "us"
        << ValueWithFormat(complex<Number>(-9.325, 0), Value::fmt_DateTime)
        << "12/22/99 04:12 PM";

    QTest::newRow("string") << "C" << Value("foobar") << "foobar";

    ValueStorage array;
    QTest::newRow("array empty") << "C" << Value(array, QSize(1, 1)) << "";
    array.insert(1, 1, Value(123));
    QTest::newRow("array 123") << "C" << Value(array, QSize(1, 1)) << "123";

    QTest::newRow("errorCIRCLE") << "C" << Value::errorCIRCLE() << "#CIRCLE!";
    QTest::newRow("errorDEPEND") << "C" << Value::errorDEPEND() << "#DEPEND!";
    QTest::newRow("errorDIV0") << "C" << Value::errorDIV0() << "#DIV/0!";
    QTest::newRow("errorNA") << "C" << Value::errorNA() << "#N/A";
    QTest::newRow("errorNAME") << "C" << Value::errorNAME() << "#NAME?";
    QTest::newRow("errorNUM") << "C" << Value::errorNUM() << "#NUM!";
    QTest::newRow("errorNULL") << "C" << Value::errorNULL() << "#NULL!";
    QTest::newRow("errorPARSE") << "C" << Value::errorPARSE() << "#PARSE!";
    QTest::newRow("errorREF") << "C" << Value::errorREF() << "#REF!";
    QTest::newRow("errorVALUE") << "C" << Value::errorVALUE() << "#VALUE!";
}

void TestValueConverter::testAsString()
{
    QFETCH(QString, locale);
    QFETCH(Value, value);
    QFETCH(QString, expected);

    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);

    Value result = m_converter->asString(value);
    QCOMPARE(result, Value(expected));
    QCOMPARE(m_converter->toString(value), expected);
}

void TestValueConverter::testAsDateTime_data()
{
    QTest::addColumn<QString>("locale");
    QTest::addColumn<Value>("value");
    QTest::addColumn<bool>("expectedOk");
    QTest::addColumn<Value>("expected");

    // ok = true and empty value is treated as current date/time
    QTest::newRow("empty") << "C" << Value() << true << Value();

    QTest::newRow("bool true") << "C" << Value(true) << true
        << Value();
    QTest::newRow("bool false") << "C" << Value(false) << true
        << Value();

    QTest::newRow("integer") << "C" << Value(123) << true
        << ValueWithFormat(123.0, Value::fmt_DateTime);
    QTest::newRow("float") << "C" << Value(10.3) << true
        << ValueWithFormat(10.3, Value::fmt_DateTime);
    QTest::newRow("complex") << "C" << Value(complex<Number>(10.3, 12.5)) << true
        << ValueWithFormat(10.3, Value::fmt_DateTime);

    QTest::newRow("string valid") << "C" << Value("1999-11-23") << true
        << ValueWithFormat(-39, Value::fmt_DateTime);
    // TODO(mek): This should probably not have a format.
    QTest::newRow("string invalid") << "C" << Value("invalid") << false
        << ValueWithFormat(Value::errorVALUE(), Value::fmt_DateTime);

    ValueStorage array;
    QTest::newRow("array empty") << "C" << Value(array, QSize(1, 1)) << true << Value();
    array.insert(1, 1, Value(543.4));
    QTest::newRow("array 543.4") << "C" << Value(array, QSize(1, 1)) << true
        << ValueWithFormat(543.4, Value::fmt_DateTime);
    // TODO(mek): Should this one return false for ok?
    array.insert(1, 1, Value("invalid"));
    // TODO(mek): This should probably not have a format.
    QTest::newRow("array invalid string") << "C" << Value(array, QSize(1, 1)) << true
        << ValueWithFormat(Value::errorVALUE(), Value::fmt_DateTime);

    // TODO(mek): Are these correct?
    //QTest::newRow("errorCIRCLE") << "C" << Value::errorCIRCLE() << true << Value();
    //QTest::newRow("errorNA") << "C" << Value::errorNA() << true << Value();
}

void TestValueConverter::testAsDateTime()
{
    QFETCH(QString, locale);
    QFETCH(Value, value);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);

    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);

    bool ok;
    Value result = m_converter->asDateTime(value, &ok);
    QCOMPARE(ok, expectedOk);
    if (expected.isEmpty()) {
        QDateTime current = QDateTime::currentDateTime();
        expected = Value(current, m_calcsettings);
        QVERIFY(result.isFloat());
        QVERIFY(result.asFloat() <= expected.asFloat());
        QVERIFY(result.asFloat() >= expected.asFloat() - 1.0/(24*60*60));
        QCOMPARE(result.format(), expected.format());
    } else {
      qDebug() << result << " != " << expected;
        QCOMPARE(result, expected);
        QCOMPARE(result.format(), expected.format());
    }
}

void TestValueConverter::testAsDate_data()
{
    QTest::addColumn<QString>("locale");
    QTest::addColumn<Value>("value");
    QTest::addColumn<bool>("expectedOk");
    QTest::addColumn<Value>("expected");

    QTest::newRow("empty") << "C" << Value() << true
        << Value(QDate::currentDate(), m_calcsettings);

    QTest::newRow("bool true") << "C" << Value(true) << true
        << Value(QDate::currentDate(), m_calcsettings);
    QTest::newRow("bool false") << "C" << Value(false) << true
        << Value(QDate::currentDate(), m_calcsettings);

    QTest::newRow("integer") << "C" << Value(123) << true
        << ValueWithFormat(123.0, Value::fmt_Date);
    QTest::newRow("float") << "C" << Value(10.3) << true
        << ValueWithFormat(10.3, Value::fmt_Date);
    QTest::newRow("complex") << "C" << Value(complex<Number>(10.3, 12.5)) << true
        << ValueWithFormat(10.3, Value::fmt_Date);

    QTest::newRow("string valid") << "C" << Value("2005-06-23") << true
        << Value(QDate(2005, 6, 23), m_calcsettings);
    QTest::newRow("string invalid") << "C" << Value("2005-26-23") << false
        << Value::errorVALUE();

    ValueStorage array;
    QTest::newRow("array empty") << "C" << Value(array, QSize(1, 1)) << true
        << Value(QDate::currentDate(), m_calcsettings);
    array.insert(1, 1, Value(543.4));
    QTest::newRow("array 543.4") << "C" << Value(array, QSize(1, 1)) << true
        << ValueWithFormat(543.4, Value::fmt_Date);
    // TODO(mek): Should this one return false for ok?
    array.insert(1, 1, Value("invalid"));
    QTest::newRow("array invalid string") << "C" << Value(array, QSize(1, 1)) << true
        << Value::errorVALUE();

    // TODO(mek): Are these correct?
    QTest::newRow("errorCIRCLE") << "C" << Value::errorCIRCLE() << true << Value();
    QTest::newRow("errorNA") << "C" << Value::errorNA() << true << Value();
}

void TestValueConverter::testAsDate()
{
    QFETCH(QString, locale);
    QFETCH(Value, value);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);

    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);

    bool ok;
    Value result = m_converter->asDate(value, &ok);
    QCOMPARE(ok, expectedOk);
    QCOMPARE(result, expected);
    QCOMPARE(result.format(), expected.format());
}

void TestValueConverter::testAsTime_data()
{
    QTest::addColumn<QString>("locale");
    QTest::addColumn<Value>("value");
    QTest::addColumn<bool>("expectedOk");
    QTest::addColumn<Value>("expected");

    // ok = true and empty value is treated as current time
    QTest::newRow("empty") << "C" << Value() << true << Value();

    QTest::newRow("bool true") << "C" << Value(true) << true
        << Value();
    QTest::newRow("bool false") << "C" << Value(false) << true
        << Value();

    QTest::newRow("integer") << "C" << Value(123) << true
        << ValueWithFormat(123.0, Value::fmt_Time);
    QTest::newRow("float") << "C" << Value(10.3) << true
        << ValueWithFormat(10.3, Value::fmt_Time);
    QTest::newRow("complex") << "C" << Value(complex<Number>(10.3, 12.5)) << true
        << ValueWithFormat(10.3, Value::fmt_Time);

    QTest::newRow("string valid 1") << "C" << Value("13:45") << true
        << Value(QTime(13, 45));
    QTest::newRow("string valid 2") << "C" << Value("13:45:33") << true
        << Value(QTime(13, 45, 33));
    QTest::newRow("string invalid") << "C" << Value("13:66:99") << false
        << Value::errorVALUE();

    ValueStorage array;
    QTest::newRow("array empty") << "C" << Value(array, QSize(1, 1)) << true << Value();
    array.insert(1, 1, Value(543.4));
    QTest::newRow("array 543.4") << "C" << Value(array, QSize(1, 1)) << true
        << ValueWithFormat(543.4, Value::fmt_Time);
    // TODO(mek): Should this one return false for ok?
    array.insert(1, 1, Value("invalid"));
    QTest::newRow("array invalid string") << "C" << Value(array, QSize(1, 1)) << true
        << Value::errorVALUE();

    // TODO(mek): Are these correct?
    //QTest::newRow("errorCIRCLE") << "C" << Value::errorCIRCLE() << true << Value();
    //QTest::newRow("errorNA") << "C" << Value::errorNA() << true << Value();
}

void TestValueConverter::testAsTime()
{
    QFETCH(QString, locale);
    QFETCH(Value, value);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);

    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);

    bool ok;
    Value result = m_converter->asTime(value, &ok);
    QCOMPARE(ok, expectedOk);
    if (expected.isEmpty()) {
        QTime currentTime = QTime::currentTime();
        expected = Value(currentTime);
        QVERIFY(result.isFloat());
        QVERIFY(result.asFloat() <= expected.asFloat());
        QVERIFY(result.asFloat() >= expected.asFloat() - 1.0/(24*60*60));
        QCOMPARE(result.format(), expected.format());
    } else {
        QCOMPARE(result, expected);
        QCOMPARE(result.format(), expected.format());
    }
}

QTEST_MAIN(TestValueConverter)
