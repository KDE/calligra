// This file is part of the KDE project
// SPDX-FileCopyrightText: 2015 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#include "TestValueConverter.h"

#include <engine/ValueConverter.h>

#include <engine/CalculationSettings.h>
#include <engine/Localization.h>
#include <engine/ValueParser.h>
#include <engine/ValueStorage.h>

#include <QStandardPaths>

#include <QTest>

Q_DECLARE_METATYPE(complex<Number>)

using namespace Calligra::Sheets;

#define USE_LANGUAGE "nb"
#define USE_LOCALE "nb_NO"

namespace
{

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
    KLocalizedString::setApplicationDomain("calligrasheets");

    m_calcsettings = new CalculationSettings();
    m_parser = new ValueParser(m_calcsettings);
    m_converter = new ValueConverter(m_parser);

    // Custom reference date to make sure all date conversions use this date.
    m_calcsettings->setReferenceDate(QDate(2000, 1, 1));

    QStandardPaths::setTestModeEnabled(true);

#ifndef Q_OS_WIN
    // If run with 'C' locale translations will fail
    // Setting it to 'C.UTF-8' fixes this
    // HACK: (Since I don't really know why)
    char *l = setlocale(LC_MESSAGES, nullptr);
    if (l && strcmp(l, "C") == 0) {
        setlocale(LC_MESSAGES, "C.UTF-8");
        qDebug() << "Set locale:" << l << "->" << setlocale(LC_MESSAGES, nullptr);
    }
#endif
#ifdef Q_OS_WIN
    QEXPECT_FAIL("", "FIXME: Testing translations on windows does not work", Continue);
#endif
    // check that translation ok, else lot of tests will fail later
    QString s = ki18n("true").toString(QStringList() << QLatin1String(USE_LANGUAGE) << QLatin1String(USE_LOCALE));
    QVERIFY2(s == QStringLiteral("sann"), "Translation failed, check that calligrasheets is translated to the selected language");
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

    QTest::newRow("string en true") << "en_US" << Value("true") << true << true;
    QTest::newRow("string en false") << "en_US" << Value("false") << true << false;
    QTest::newRow("string en foobar") << "en_US" << Value("foobar") << false << false;
    QTest::newRow("string en TruE") << "en_US" << Value("TruE") << true << true;
    QTest::newRow("string en fAlSe") << "en_US" << Value("fAlSe") << true << false;
    QTest::newRow("string en sann") << "en_US" << Value("sann") << false << false;

#ifndef Q_OS_WIN
    QTest::newRow("string xx true") << USE_LOCALE << Value("true") << true << true;
    QTest::newRow("string xx false") << USE_LOCALE << Value("false") << true << false;
    QTest::newRow("string xx foobar") << USE_LOCALE << Value("foobar") << false << false;
    QTest::newRow("string xx TruE") << USE_LOCALE << Value("TruE") << true << true;
    QTest::newRow("string xx fAlSe") << USE_LOCALE << Value("fAlSe") << true << false;
    QTest::newRow("string xx sann") << USE_LOCALE << Value("sann") << true << true;
    QTest::newRow("string xx sAnn") << USE_LOCALE << Value("sAnn") << true << true;
    QTest::newRow("string xx usann") << USE_LOCALE << Value("usann") << true << false;
    QTest::newRow("string xx usAnn") << USE_LOCALE << Value("usAnn") << true << false;
#endif

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

    m_calcsettings->locale()->setLanguage(locale);

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
    QTest::newRow("string cz 1,4") << USE_LOCALE << Value("1,4") << true << 1;
    QTest::newRow("string cz 1,400") << USE_LOCALE << Value("1,400") << true << 1;

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

    m_calcsettings->locale()->setLanguage(locale);

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
    QTest::newRow("string cz 1,4") << USE_LOCALE << Value("1,4") << true << 1.4;
    QTest::newRow("string cz 1,400") << USE_LOCALE << Value("1,400") << true << 1.4;

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

    m_calcsettings->locale()->setLanguage(locale);

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
    QTest::newRow("float 123.9999") << "C" << Value(123.9999) << true << complex<Number>(123.9999, 0);
    QTest::newRow("float -45.65") << "C" << Value(-45.65) << true << complex<Number>(-45.65, 0);

    QTest::newRow("complex 0 0i") << "C" << Value(complex<Number>(0, 0)) << true << complex<Number>(0, 0);
    QTest::newRow("complex 1.6 -3i") << "C" << Value(complex<Number>(1.6, -3)) << true << complex<Number>(1.6, -3);
    QTest::newRow("complex 2 4.6i") << "C" << Value(complex<Number>(2, 4.6)) << true << complex<Number>(2, 4.6);
    QTest::newRow("complex -3.14 1i") << "C" << Value(complex<Number>(-3.14, 1)) << true << complex<Number>(-3.14, 1);

    QTest::newRow("string 123") << "C" << Value("123") << true << complex<Number>(123, 0);
    QTest::newRow("string -456") << "C" << Value("-456") << true << complex<Number>(-456, 0);
    QTest::newRow("string +5") << "C" << Value("+5") << true << complex<Number>(5, 0);
    QTest::newRow("string 1525%") << "C" << Value("1525%") << true << complex<Number>(15.25, 0);
    QTest::newRow("string 5+3i") << "C" << Value("5+3i") << true << complex<Number>(5, 3);
    QTest::newRow("string 2.4 + 3j") << "C" << Value("2.4 + 3j") << true << complex<Number>(2.4, 3);
    QTest::newRow("string 6 - 3i") << "C" << Value("6 - 3i") << true << complex<Number>(6, -3);
    QTest::newRow("string 2.4i") << "C" << Value("2.4i") << true << complex<Number>(0, 2.4);
    QTest::newRow("string 1,4") << "C" << Value("1,4") << false << complex<Number>(0, 0);
    QTest::newRow("string 1,400") << "C" << Value("1,400") << true << complex<Number>(1400, 0);
    QTest::newRow("string 3 5/2") << "C" << Value("3 5/2") << true << complex<Number>(5.5, 0);
    QTest::newRow("string 3e2") << "C" << Value("3e2") << true << complex<Number>(300.0, 0);
    QTest::newRow("string 1234E-2") << "C" << Value("1234E-2") << true << complex<Number>(12.34, 0);
    QTest::newRow("string 12.34e5 + 4.2e2i") << "C" << Value("12.34e5 + 4.2e2i") << true << complex<Number>(12.34e5, 4.2e2);
    QTest::newRow("string 12.34e5 + 4.2e+2i") << "C" << Value("12.34e5 + 4.2e+2i") << true << complex<Number>(12.34e5, 4.2e2);
    // QTest::newRow("string 12.34e+5 + 4.2e2i") << "C" << Value("12.34e+5 + 4.2e2i")
    //     << true << complex<Number>(12.34e5, 4.2e2);
    // QTest::newRow("string 12.34e+5 + 4.2e+2i") << "C" << Value("12.34e+5 + 4.2e+2i")
    //     << true << complex<Number>(12.34e5, 4.2e2);
    QTest::newRow("string cz 1,4") << USE_LOCALE << Value("1,4") << true << complex<Number>(1.4, 0);
    QTest::newRow("string cz 1,400") << USE_LOCALE << Value("1,400") << true << complex<Number>(1.4, 0);

    ValueStorage array;
    QTest::newRow("array empty") << "C" << Value(array, QSize(1, 1)) << true << complex<Number>(0, 0);
    array.insert(1, 1, Value(543.4));
    QTest::newRow("array 543.4") << "C" << Value(array, QSize(1, 1)) << true << complex<Number>(543.4, 0);

    QTest::newRow("errorCIRCLE") << "C" << Value::errorCIRCLE() << true << complex<Number>(0, 0);
    QTest::newRow("errorNA") << "C" << Value::errorNA() << true << complex<Number>(0, 0);
}

void TestValueConverter::testAsComplex()
{
    QFETCH(QString, locale);
    QFETCH(Value, value);
    QFETCH(bool, expectedOk);
    QFETCH(complex<Number>, expected);

    m_calcsettings->locale()->setLanguage(locale);

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

    QTest::newRow("complex 0 0i") << "C" << Value(complex<Number>(0, 0)) << true << Value(complex<Number>(0, 0));
    QTest::newRow("complex 1.6 -3i") << "C" << Value(complex<Number>(1.6, -3)) << true << Value(complex<Number>(1.6, -3));
    QTest::newRow("complex 2 4.6i") << "C" << Value(complex<Number>(2, 4.6)) << true << Value(complex<Number>(2, 4.6));
    QTest::newRow("complex -3.14 1i") << "C" << Value(complex<Number>(-3.14, 1)) << true << Value(complex<Number>(-3.14, 1));

    QTest::newRow("string 123") << "C" << Value("123") << true << Value(123);
    QTest::newRow("string -456") << "C" << Value("-456") << true << Value(-456);
    QTest::newRow("string +5") << "C" << Value("+5") << true << Value(5);
    QTest::newRow("string 1525%") << "C" << Value("1525%") << true << Value(15.25);
    QTest::newRow("string 5+3i") << "C" << Value("5+3i") << true << Value(complex<Number>(5, 3));
    QTest::newRow("string 2.4 + 3j") << "C" << Value("2.4 + 3j") << true << Value(complex<Number>(2.4, 3));
    QTest::newRow("string 6 - 3i") << "C" << Value("6 - 3i") << true << Value(complex<Number>(6, -3));
    QTest::newRow("string 2.4i") << "C" << Value("2.4i") << true << Value(complex<Number>(0, 2.4));
    QTest::newRow("string 1,4") << "C" << Value("1,4") << false << Value(0.0);
    QTest::newRow("string 1,400") << "C" << Value("1,400") << true << Value(1400);
    QTest::newRow("string 3 5/2") << "C" << Value("3 5/2") << true << Value(5.5);
    QTest::newRow("string 3e2") << "C" << Value("3e2") << true << Value(300.0);
    QTest::newRow("string 1234E-2") << "C" << Value("1234E-2") << true << Value(12.34);
    QTest::newRow("string cz 1,4") << USE_LOCALE << Value("1,4") << true << Value(1.4);
    QTest::newRow("string cz 1,400") << USE_LOCALE << Value("1,400") << true << Value(1.4);

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

    m_calcsettings->locale()->setLanguage(locale);

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

    QTest::newRow("bool True") << "C" << Value(true) << Value("true");
    QTest::newRow("bool False") << "C" << Value(false) << Value("false");

    QTest::newRow("integer plain") << "C" << Value(123) << Value("123");
    QTest::newRow("integer percent") << "C" << ValueWithFormat(3, Value::fmt_Percent) << Value("300 %");
    QTest::newRow("integer time") << "C" << ValueWithFormat(4, Value::fmt_Time) << Value("00:00:00");
    // TODO(mek): These next ones should almost certainly be using short date format.
    QTest::newRow("integer date 1") << "C" << ValueWithFormat(0, Value::fmt_Date) << Value("Saturday, 1 January 2000");
    QTest::newRow("integer date 2") << "C" << ValueWithFormat(2000, Value::fmt_Date) << Value("Thursday, 23 June 2005");
    QTest::newRow("integer date 3") << "C" << ValueWithFormat(-10, Value::fmt_Date) << Value("Wednesday, 22 December 1999");

    //    QTest::newRow("integer datetime 1") << "C" << ValueWithFormat(4, Value::fmt_DateTime) << Value("Wednesday, 5 January 2000 00:00:00");
    //    QTest::newRow("integer datetime 2") << "C" << ValueWithFormat(-10, Value::fmt_DateTime) << Value("Wednesday, 22 December 1999 00:00:00");

    QTest::newRow("float 123") << "C" << Value(123.0) << Value("123");
    QTest::newRow("float -3.14") << "C" << Value(-3.14) << Value("-3.14");
    QTest::newRow("float 1.5e99") << "C" << Value(1.5e99) << Value("1.5e+99");
    QTest::newRow("float 0.43e-12") << "C" << Value(0.43e-12) << Value("4.3e-13");
    // TODO(mek): Currently buggy/inconsistent in implementation.
    // QTest::newRow("float percent") << "C" << ValueWithFormat(3.45, Value::fmt_Percent) << Value("345 %");

    QTest::newRow("float time 0") << "C" << ValueWithFormat(4, Value::fmt_Time) << Value("00:00:00");
    QTest::newRow("float time 1") << "C" << ValueWithFormat(0.5, Value::fmt_Time) << Value("12:00:00");
    QTest::newRow("float time 2") << "C" << ValueWithFormat(3.675, Value::fmt_Time) << Value("16:12:00");
    QTest::newRow("float date 1") << "C" << ValueWithFormat(0.5, Value::fmt_Date) << Value("Saturday, 1 January 2000");
    QTest::newRow("float date 2") << "C" << ValueWithFormat(2000.324, Value::fmt_Date) << Value("Thursday, 23 June 2005");
    QTest::newRow("float date 3") << "C" << ValueWithFormat(-9.234, Value::fmt_Date) << Value("Wednesday, 22 December 1999");

    //    QTest::newRow("float datetime 0") << "C" << ValueWithFormat(4.0, Value::fmt_DateTime) << Value("Wednesday, 5 January 2000 00:00:00 UTC");
    //    QTest::newRow("float datetime 1") << "C" << ValueWithFormat(2000.5, Value::fmt_DateTime) << Value("Thursday, 23 June 2005 12:00:00 UTC");
    //    QTest::newRow("float datetime 2") << "C" << ValueWithFormat(-9.325, Value::fmt_DateTime) << Value("Wednesday, 22 December 1999 16:12:00 UTC");

    QTest::newRow("complex 0+0i") << "C" << Value(complex<Number>(0, 0)) << Value("0+0i");
    QTest::newRow("complex 3.14-2.7i") << "C" << Value(complex<Number>(3.14, -2.7)) << Value("3.14-2.7i");
    QTest::newRow("complex 2.2e99+3.3e88i") << "C" << Value(complex<Number>(2.2e99, 3.3e88)) << Value("2.2e+99+3.3e+88i");
    QTest::newRow("complex time 0") << "C" << ValueWithFormat(complex<Number>(4, 3), Value::fmt_Time) << Value("00:00:00");
    QTest::newRow("complex time 1") << "C" << ValueWithFormat(complex<Number>(0.5, -3), Value::fmt_Time) << Value("12:00:00");
    QTest::newRow("complex time 2") << "C" << ValueWithFormat(complex<Number>(3.675, 653), Value::fmt_Time) << Value("16:12:00");

    QTest::newRow("complex date 1") << "C" << ValueWithFormat(complex<Number>(0.5, 0), Value::fmt_Date) << Value("Saturday, 1 January 2000");
    QTest::newRow("complex date 2") << "C" << ValueWithFormat(complex<Number>(2000.324, 0), Value::fmt_Date) << Value("Thursday, 23 June 2005");
    QTest::newRow("complex date 3") << "C" << ValueWithFormat(complex<Number>(-9.234, 0), Value::fmt_Date) << Value("Wednesday, 22 December 1999");

    //    QTest::newRow("complex datetime 0") << "C"
    //        << ValueWithFormat(complex<Number>(4.0, 0), Value::fmt_DateTime)
    //        << Value("Wednesday, 5 January 2000 00:00:00 UTC");
    //    QTest::newRow("complex datetime 1") << "C"
    //        << ValueWithFormat(complex<Number>(2000.5, 0), Value::fmt_DateTime)
    //        << Value("Thursday, 23 June 2005 12:00:00 UTC");
    //    QTest::newRow("complex datetime 2") << "C"
    //        << ValueWithFormat(complex<Number>(-9.325, 0), Value::fmt_DateTime)
    //        << Value("Wednesday, 22 December 1999 16:12:00 UTC");
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

    QTest::newRow("integer time us") << "en_US" << ValueWithFormat(4, Value::fmt_Time) << Value("12:00:00 am");
    //    QTest::newRow("integer datetime 1 us") << "en_US" << ValueWithFormat(4, Value::fmt_DateTime) << Value("Wednesday, January 5, 2000 12:00:00 am");
    //    QTest::newRow("integer datetime 2 xx") << "en_US" << ValueWithFormat(-10, Value::fmt_DateTime) << Value("Wednesday, December 22, 1999 12:00:00 am");
    QTest::newRow("float time 0 us") << "en_US" << ValueWithFormat(4.0, Value::fmt_Time) << Value("12:00:00 am");
    QTest::newRow("float time 1 us") << "en_US" << ValueWithFormat(0.5, Value::fmt_Time) << Value("12:00:00 pm");
    QTest::newRow("float time 2 us") << "en_US" << ValueWithFormat(3.675, Value::fmt_Time) << Value("4:12:00 pm");

    //    QTest::newRow("float datetime 0 us") << "en_US" << ValueWithFormat(4.0, Value::fmt_DateTime)<< Value("Wednesday, January 5, 2000 12:00:00 AM UTC");
    //    QTest::newRow("float datetime 1 us") << "en_US" << ValueWithFormat(2000.5, Value::fmt_DateTime) << Value("Thursday, June 23, 2005 12:00:00 PM UTC");

    QTest::newRow("complex time 0 us") << "en_US" << ValueWithFormat(complex<Number>(4, 634), Value::fmt_Time) << Value("12:00:00 am");
    QTest::newRow("complex time 1 us") << "en_US" << ValueWithFormat(complex<Number>(0.5, 2.3), Value::fmt_Time) << Value("12:00:00 pm");
    QTest::newRow("complex time 2 us") << "en_US" << ValueWithFormat(complex<Number>(3.675, 2), Value::fmt_Time) << Value("4:12:00 pm");

    QTest::newRow("complex date 3 us") << "en_US" << ValueWithFormat(complex<Number>(-9.234, 0), Value::fmt_Date) << Value("Wednesday, December 22, 1999");
    //    QTest::newRow("float datetime 2 us") << "en_US" << ValueWithFormat(-9.325, Value::fmt_DateTime) << Value("Wednesday, December 22, 1999 4:12:00 PM
    //    UTC");

    //    QTest::newRow("complex datetime 0 us") << "en_US"
    //        << ValueWithFormat(complex<Number>(4.0, 0), Value::fmt_DateTime)
    //        << Value("Wednesday, January 5, 2000 12:00:00 AM UTC");
    //    QTest::newRow("complex datetime 1 us") << "en_US"
    //        << ValueWithFormat(complex<Number>(2000.5, 0), Value::fmt_DateTime)
    //        << Value("Thursday, June 23, 2005 12:00:00 PM UTC");
    //    QTest::newRow("complex datetime 2 us") << "en_US"
    //        << ValueWithFormat(complex<Number>(-9.325, 0), Value::fmt_DateTime)
    //        << Value("Wednesday, December 22, 1999 4:12:00 PM UTC");

#ifndef Q_OS_WIN
    QTest::newRow("bool True xx") << USE_LOCALE << Value(true) << Value("sann");
    QTest::newRow("bool False xx") << USE_LOCALE << Value(false) << Value("usann");
    QTest::newRow("integer date 1 xx") << USE_LOCALE << ValueWithFormat(0, Value::fmt_Date) << Value("lørdag 1. januar 2000");
    QTest::newRow("integer date 2 xx") << USE_LOCALE << ValueWithFormat(2000, Value::fmt_Date) << Value("torsdag 23. juni 2005");
    QTest::newRow("integer date 3 xx") << USE_LOCALE << ValueWithFormat(-10, Value::fmt_Date) << Value("onsdag 22. desember 1999");
    //    QTest::newRow("integer datetime 1 xx") << USE_LOCALE << ValueWithFormat(4, Value::fmt_DateTime) << Value("onsdag 5. januar 2000 00:00:00");
    //    QTest::newRow("integer datetime 2 xx") << USE_LOCALE << ValueWithFormat(-10, Value::fmt_DateTime) << Value("onsdag 22. desember 1999 00:00:00");
    QTest::newRow("float 123 xx") << USE_LOCALE << Value(123.0) << Value("123");
    QTest::newRow("float -3.14 xx") << USE_LOCALE << Value(-3.14) << Value("-3,14");
    QTest::newRow("float 1.5e99 xx") << USE_LOCALE << Value(1.5e99) << Value("1,5e+99");
    QTest::newRow("float 0.43e-12 xx") << USE_LOCALE << Value(0.43e-12) << Value("4,3e-13");
    QTest::newRow("float date 1 xx") << USE_LOCALE << ValueWithFormat(0.5, Value::fmt_Date) << Value("lørdag 1. januar 2000");
    QTest::newRow("float date 2 xx") << USE_LOCALE << ValueWithFormat(2000.324, Value::fmt_Date) << Value("torsdag 23. juni 2005");
    //    QTest::newRow("float datetime 0 xx") << USE_LOCALE << ValueWithFormat(4.0, Value::fmt_DateTime) << Value("onsdag 5. januar 2000 00:00:00 UTC");
    //    QTest::newRow("float datetime 1 xx") << USE_LOCALE << ValueWithFormat(2000.5, Value::fmt_DateTime) << Value("torsdag 23. juni 2005 12:00:00 UTC");
    //    QTest::newRow("float datetime 2 xx") << USE_LOCALE << ValueWithFormat(-9.325, Value::fmt_DateTime) << Value("onsdag 22. desember 1999 16:12:00 UTC");

    QTest::newRow("complex date 1 xx") << USE_LOCALE << ValueWithFormat(complex<Number>(0.5, 0), Value::fmt_Date) << Value("lørdag 1. januar 2000");
    QTest::newRow("complex date 2 xx") << USE_LOCALE << ValueWithFormat(complex<Number>(2000.324, 0), Value::fmt_Date) << Value("torsdag 23. juni 2005");
#endif
}

void TestValueConverter::testAsString()
{
    QFETCH(QString, locale);
    QFETCH(Value, value);
    QFETCH(Value, expected);
    QTest::addColumn<QString>("expfail");

    m_calcsettings->locale()->setLanguage(locale);

    Value result = m_converter->asString(value);
    qInfo() << locale << 'v' << value << 'r' << result << '=' << 'e' << expected;
    QCOMPARE(result, expected);
    QCOMPARE(Value(m_converter->toString(value)), expected);
}

void TestValueConverter::testAsDateTime_data()
{
    QTest::addColumn<QString>("locale");
    QTest::addColumn<Value>("value");
    QTest::addColumn<bool>("expectedOk");
    QTest::addColumn<Value>("expected");

    // ok = true and empty value is treated as current date/time
    QTest::newRow("empty") << "C" << Value() << true << Value();

    QTest::newRow("bool true") << "C" << Value(true) << true << Value();
    QTest::newRow("bool false") << "C" << Value(false) << true << Value();

    //    QTest::newRow("integer") << "C" << Value(123) << true
    //        << ValueWithFormat(123.0, Value::fmt_DateTime);
    //    QTest::newRow("float") << "C" << Value(10.3) << true
    //        << ValueWithFormat(10.3, Value::fmt_DateTime);
    //    QTest::newRow("complex") << "C" << Value(complex<Number>(10.3, 12.5)) << true
    //        << ValueWithFormat(10.3, Value::fmt_DateTime);

    //    QTest::newRow("string valid") << "C" << Value("1999-11-23") << true
    //        << ValueWithFormat(-39.0, Value::fmt_DateTime);
    //    // TODO(mek): This should probably not have a format.
    //    QTest::newRow("string invalid") << "C" << Value("invalid") << false
    //        << ValueWithFormat(Value::errorVALUE(), Value::fmt_DateTime);

    ValueStorage array;
    QTest::newRow("array empty") << "C" << Value(array, QSize(1, 1)) << true << Value();
    array.insert(1, 1, Value(543.4));
    //    QTest::newRow("array 543.4") << "C" << Value(array, QSize(1, 1)) << true
    //        << ValueWithFormat(543.4, Value::fmt_DateTime);
    //    // TODO(mek): Should this one return false for ok?
    //    array.insert(1, 1, Value("invalid"));
    //    // TODO(mek): This should probably not have a format.
    //    QTest::newRow("array invalid string") << "C" << Value(array, QSize(1, 1)) << true
    //        << ValueWithFormat(Value::errorVALUE(), Value::fmt_DateTime);

    // TODO(mek): Are these correct?
    // QTest::newRow("errorCIRCLE") << "C" << Value::errorCIRCLE() << true << Value();
    // QTest::newRow("errorNA") << "C" << Value::errorNA() << true << Value();
}

void TestValueConverter::testAsDateTime()
{
    QFETCH(QString, locale);
    QFETCH(Value, value);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);

    m_calcsettings->locale()->setLanguage(locale);

    bool ok;
    Value result = m_converter->asDateTime(value, &ok);
    QCOMPARE(ok, expectedOk);
    if (expected.isEmpty()) {
        QDateTime current = QDateTime::currentDateTime();
        expected = Value(current, m_calcsettings);
        QVERIFY(result.isFloat());
        QVERIFY(result.asFloat() <= expected.asFloat());
        QVERIFY(result.asFloat() >= expected.asFloat() - 1.0 / (24 * 60 * 60));
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

    QTest::newRow("empty") << "C" << Value() << true << Value(QDate::currentDate(), m_calcsettings);

    QTest::newRow("bool true") << "C" << Value(true) << true << Value(QDate::currentDate(), m_calcsettings);
    QTest::newRow("bool false") << "C" << Value(false) << true << Value(QDate::currentDate(), m_calcsettings);

    QTest::newRow("integer") << "C" << Value(123) << true << ValueWithFormat(123.0, Value::fmt_Date);
    QTest::newRow("float") << "C" << Value(10.3) << true << ValueWithFormat(10.3, Value::fmt_Date);
    QTest::newRow("complex") << "C" << Value(complex<Number>(10.3, 12.5)) << true << ValueWithFormat(10.3, Value::fmt_Date);

    QTest::newRow("string valid") << "C" << Value("2005-06-23") << true << Value(QDate(2005, 6, 23), m_calcsettings);
    QTest::newRow("string invalid") << "C" << Value("2005-26-23") << false << Value::errorVALUE();

    ValueStorage array;
    QTest::newRow("array empty") << "C" << Value(array, QSize(1, 1)) << true << Value(QDate::currentDate(), m_calcsettings);
    array.insert(1, 1, Value(543.4));
    QTest::newRow("array 543.4") << "C" << Value(array, QSize(1, 1)) << true << ValueWithFormat(543.4, Value::fmt_Date);
    // TODO(mek): Should this one return false for ok?
    array.insert(1, 1, Value("invalid"));
    QTest::newRow("array invalid string") << "C" << Value(array, QSize(1, 1)) << true << Value::errorVALUE();

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

    m_calcsettings->locale()->setLanguage(locale);

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

    QTest::newRow("bool true") << "C" << Value(true) << true << Value();
    QTest::newRow("bool false") << "C" << Value(false) << true << Value();

    QTest::newRow("integer") << "C" << Value(123) << true << ValueWithFormat(123.0, Value::fmt_Time);
    QTest::newRow("float") << "C" << Value(10.3) << true << ValueWithFormat(10.3, Value::fmt_Time);
    QTest::newRow("complex") << "C" << Value(complex<Number>(10.3, 12.5)) << true << ValueWithFormat(10.3, Value::fmt_Time);

    QTest::newRow("string valid 1") << "C" << Value("13:45") << true << Value(Time(13, 45));
    QTest::newRow("string valid 2") << "C" << Value("13:45:33") << true << Value(Time(13, 45, 33));
    QTest::newRow("string invalid") << "C" << Value("13:66:99") << false << Value::errorVALUE();

    ValueStorage array;
    QTest::newRow("array empty") << "C" << Value(array, QSize(1, 1)) << true << Value();
    array.insert(1, 1, Value(543.4));
    QTest::newRow("array 543.4") << "C" << Value(array, QSize(1, 1)) << true << ValueWithFormat(543.4, Value::fmt_Time);
    // TODO(mek): Should this one return false for ok?
    array.insert(1, 1, Value("invalid"));
    QTest::newRow("array invalid string") << "C" << Value(array, QSize(1, 1)) << true << Value::errorVALUE();

    // TODO(mek): Are these correct?
    // QTest::newRow("errorCIRCLE") << "C" << Value::errorCIRCLE() << true << Value();
    // QTest::newRow("errorNA") << "C" << Value::errorNA() << true << Value();
}

void TestValueConverter::testAsTime()
{
    QFETCH(QString, locale);
    QFETCH(Value, value);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);

    m_calcsettings->locale()->setLanguage(locale);

    bool ok;
    Value result = m_converter->asTime(value, &ok);
    QCOMPARE(ok, expectedOk);
    if (expected.isEmpty()) {
        auto currentTime = Time::currentTime();
        expected = Value(currentTime);
        QVERIFY(result.isFloat());
        QVERIFY(result.asFloat() <= expected.asFloat());
        QVERIFY(result.asFloat() >= expected.asFloat() - 1.0 / (24 * 60 * 60));
        QCOMPARE(result.format(), expected.format());
    } else {
        QCOMPARE(result, expected);
        QCOMPARE(result.format(), expected.format());
    }
}

QTEST_MAIN(TestValueConverter)
