// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#include "TestValueFormatter.h"

#include <core/ValueFormatter.h>

#include <engine/CalculationSettings.h>
#include <engine/Localization.h>
#include <engine/ValueConverter.h>
#include <engine/ValueParser.h>

#include <QTest>

Q_DECLARE_METATYPE(Calligra::Sheets::Format::Type)
Q_DECLARE_METATYPE(Calligra::Sheets::Style::FloatFormat)

using namespace Calligra::Sheets;

class PublicValueFormatter : public ValueFormatter
{
public:
    explicit PublicValueFormatter(const ValueConverter *converter)
        : ValueFormatter(converter)
    {
    }

    using ValueFormatter::createNumberFormat;
    using ValueFormatter::fractionFormat;
};

void TestValueFormatter::initTestCase()
{
    KLocalizedString::setApplicationDomain("calligrasheets");
    qRegisterMetaType<Format::Type>();

    m_calcsettings = new CalculationSettings();
    m_parser = new ValueParser(m_calcsettings);
    m_converter = new ValueConverter(m_parser);
}

void TestValueFormatter::cleanupTestCase()
{
    delete m_converter;
    delete m_parser;
    delete m_calcsettings;
}

void TestValueFormatter::testFractionFormat_data()
{
    QTest::addColumn<double>("value");
    QTest::addColumn<Format::Type>("formatType");
    QTest::addColumn<QString>("result");

    // fraction_half
    QTest::newRow("half 0.0") << 0.0 << Format::fraction_half << "0";
    QTest::newRow("half 1.0") << 1.0 << Format::fraction_half << "1";
    QTest::newRow("half 1.5") << 1.5 << Format::fraction_half << "1 1/2";
    QTest::newRow("half 1.4") << 1.4 << Format::fraction_half << "1 1/2";
    QTest::newRow("half 1.6") << 1.6 << Format::fraction_half << "1 1/2";
    QTest::newRow("half 0.9") << 0.9 << Format::fraction_half << "1";
    QTest::newRow("half 1.1") << 1.1 << Format::fraction_half << "1";
    QTest::newRow("half -0.2") << -0.2 << Format::fraction_half << "-0";
    QTest::newRow("half -0.4") << -0.4 << Format::fraction_half << "-1/2";
    QTest::newRow("half -0.6") << -0.6 << Format::fraction_half << "-1/2";
    QTest::newRow("half -0.9") << -0.9 << Format::fraction_half << "-1";

    // fraction_quarter
    QTest::newRow("quarter 0.0") << 0.0 << Format::fraction_quarter << "0";
    QTest::newRow("quarter 1.0") << 1.0 << Format::fraction_quarter << "1";
    QTest::newRow("quarter 0.1") << 0.1 << Format::fraction_quarter << "0";
    QTest::newRow("quarter 0.2") << 0.2 << Format::fraction_quarter << "1/4";
    QTest::newRow("quarter 0.3") << 0.3 << Format::fraction_quarter << "1/4";
    QTest::newRow("quarter 0.5") << 0.5 << Format::fraction_quarter << "2/4";
    QTest::newRow("quarter 0.8") << 0.8 << Format::fraction_quarter << "3/4";
    QTest::newRow("quarter 0.9") << 0.9 << Format::fraction_quarter << "1";
    QTest::newRow("quarter 1.1") << 1.1 << Format::fraction_quarter << "1";
    QTest::newRow("quarter 1.2") << 1.2 << Format::fraction_quarter << "1 1/4";
    QTest::newRow("quarter -0.1") << -0.1 << Format::fraction_quarter << "-0";
    QTest::newRow("quarter -0.2") << -0.2 << Format::fraction_quarter << "-1/4";
    QTest::newRow("quarter -0.3") << -0.3 << Format::fraction_quarter << "-1/4";
    QTest::newRow("quarter -0.5") << -0.5 << Format::fraction_quarter << "-2/4";
    QTest::newRow("quarter -0.8") << -0.8 << Format::fraction_quarter << "-3/4";
    QTest::newRow("quarter -0.9") << -0.9 << Format::fraction_quarter << "-1";

    // fraction_eighth
    QTest::newRow("eighth 0.0") << 0.0 << Format::fraction_eighth << "0";
    QTest::newRow("eighth 1.0") << 1.0 << Format::fraction_eighth << "1";
    QTest::newRow("eighth 0.05") << 0.05 << Format::fraction_eighth << "0";
    QTest::newRow("eighth 0.10") << 0.10 << Format::fraction_eighth << "1/8";
    QTest::newRow("eighth 0.15") << 0.15 << Format::fraction_eighth << "1/8";
    QTest::newRow("eighth 0.25") << 0.25 << Format::fraction_eighth << "2/8";
    QTest::newRow("eighth 0.50") << 0.50 << Format::fraction_eighth << "4/8";
    QTest::newRow("eighth 0.90") << 0.90 << Format::fraction_eighth << "7/8";
    QTest::newRow("eighth 0.95") << 0.95 << Format::fraction_eighth << "1";
    QTest::newRow("eighth 1.05") << 1.05 << Format::fraction_eighth << "1";
    QTest::newRow("eighth 1.10") << 1.10 << Format::fraction_eighth << "1 1/8";
    QTest::newRow("eighth -0.05") << -0.05 << Format::fraction_eighth << "-0";
    QTest::newRow("eighth -0.10") << -0.10 << Format::fraction_eighth << "-1/8";
    QTest::newRow("eighth -0.15") << -0.15 << Format::fraction_eighth << "-1/8";
    QTest::newRow("eighth -0.10") << -0.25 << Format::fraction_eighth << "-2/8";
    QTest::newRow("eighth -0.90") << -0.90 << Format::fraction_eighth << "-7/8";
    QTest::newRow("eighth -0.95") << -0.95 << Format::fraction_eighth << "-1";

    // fraction_sixteenth

    // fraction_tenth
    QTest::newRow("tenth 0.0") << 0.0 << Format::fraction_tenth << "0";
    QTest::newRow("tenth 1.0") << 1.0 << Format::fraction_tenth << "1";
    QTest::newRow("tenth 0.04") << 0.04 << Format::fraction_tenth << "0";
    QTest::newRow("tenth 0.06") << 0.06 << Format::fraction_tenth << "1/10";
    QTest::newRow("tenth 0.14") << 0.14 << Format::fraction_tenth << "1/10";
    QTest::newRow("tenth 0.53") << 0.53 << Format::fraction_tenth << "5/10";
    QTest::newRow("tenth 0.94") << 0.94 << Format::fraction_tenth << "9/10";
    QTest::newRow("tenth 0.97") << 0.97 << Format::fraction_tenth << "1";
    QTest::newRow("tenth 1.02") << 1.02 << Format::fraction_tenth << "1";
    QTest::newRow("tenth 1.47") << 1.47 << Format::fraction_tenth << "1 5/10";
    QTest::newRow("tenth -0.04") << -0.04 << Format::fraction_tenth << "-0";
    QTest::newRow("tenth -0.06") << -0.06 << Format::fraction_tenth << "-1/10";
    QTest::newRow("tenth -0.14") << -0.14 << Format::fraction_tenth << "-1/10";
    QTest::newRow("tenth -0.53") << -0.53 << Format::fraction_tenth << "-5/10";
    QTest::newRow("tenth -0.94") << -0.94 << Format::fraction_tenth << "-9/10";
    QTest::newRow("tenth -0.97") << -0.97 << Format::fraction_tenth << "-1";

    // fraction_hundredth

    // fraction_one_digit
    QTest::newRow("one_digit 0.0") << 0.0 << Format::fraction_one_digit << "0";
    QTest::newRow("one_digit 0.05") << 0.05 << Format::fraction_one_digit << "0";
    QTest::newRow("one_digit 0.1") << 0.1 << Format::fraction_one_digit << "1/9";
    QTest::newRow("one_digit 0.2") << 0.2 << Format::fraction_one_digit << "1/5";
    QTest::newRow("one_digit 0.3") << 0.3 << Format::fraction_one_digit << "2/7";
    QTest::newRow("one_digit 0.4") << 0.4 << Format::fraction_one_digit << "2/5";
    QTest::newRow("one_digit 0.5") << 0.5 << Format::fraction_one_digit << "1/2";
    QTest::newRow("one_digit 0.6") << 0.6 << Format::fraction_one_digit << "3/5";
    QTest::newRow("one_digit 0.7") << 0.7 << Format::fraction_one_digit << "5/7";
    QTest::newRow("one_digit 0.8") << 0.8 << Format::fraction_one_digit << "4/5";
    QTest::newRow("one_digit 0.9") << 0.9 << Format::fraction_one_digit << "8/9";
    QTest::newRow("one_digit 0.95") << 0.95 << Format::fraction_one_digit << "1";
    QTest::newRow("one_digit 1.0") << 1.0 << Format::fraction_one_digit << "1";
    QTest::newRow("one_digit 1.1") << 1.1 << Format::fraction_one_digit << "1 1/9";
    QTest::newRow("one_digit 1.2") << 1.2 << Format::fraction_one_digit << "1 1/5";
    QTest::newRow("one_digit 1.3") << 1.3 << Format::fraction_one_digit << "1 2/7";
    QTest::newRow("one_digit -0.05") << -0.05 << Format::fraction_one_digit << "-0";
    QTest::newRow("one_digit -0.1") << -0.1 << Format::fraction_one_digit << "-1/9";
    QTest::newRow("one_digit -0.2") << -0.2 << Format::fraction_one_digit << "-1/5";
    QTest::newRow("one_digit -0.3") << -0.3 << Format::fraction_one_digit << "-2/7";

    // fraction_two_digits
    QTest::newRow("two_digits 0.00") << 0.00 << Format::fraction_two_digits << "0";
    QTest::newRow("two_digits 0.005") << 0.005 << Format::fraction_two_digits << "0";
    QTest::newRow("two_digits 0.01") << 0.01 << Format::fraction_two_digits << "1/99";
    QTest::newRow("two_digits 0.02") << 0.02 << Format::fraction_two_digits << "1/50";
    QTest::newRow("two_digits 0.03") << 0.03 << Format::fraction_two_digits << "2/67";
    QTest::newRow("two_digits 0.07") << 0.07 << Format::fraction_two_digits << "4/57";
    QTest::newRow("two_digits 0.09") << 0.09 << Format::fraction_two_digits << "8/89";
    QTest::newRow("two_digits 0.11") << 0.11 << Format::fraction_two_digits << "10/91";
    QTest::newRow("two_digits 0.995") << 0.995 << Format::fraction_two_digits << "1";
    QTest::newRow("two_digits 1.00") << 1.00 << Format::fraction_two_digits << "1";
    QTest::newRow("two_digits 1.01") << 1.01 << Format::fraction_two_digits << "1 1/99";
    QTest::newRow("two_digits 1.02") << 1.02 << Format::fraction_two_digits << "1 1/50";
    QTest::newRow("two_digits 1.03") << 1.03 << Format::fraction_two_digits << "1 2/67";
    QTest::newRow("two_digits -0.005") << -0.005 << Format::fraction_two_digits << "-0";
    QTest::newRow("two_digits -0.01") << -0.01 << Format::fraction_two_digits << "-1/99";
    QTest::newRow("two_digits -0.02") << -0.02 << Format::fraction_two_digits << "-1/50";
    QTest::newRow("two_digits -0.03") << -0.03 << Format::fraction_two_digits << "-2/67";
    QTest::newRow("two_digits -0.07") << -0.07 << Format::fraction_two_digits << "-4/57";

    // fraction_three_digits
}

void TestValueFormatter::testFractionFormat()
{
    QFETCH(double, value);
    QFETCH(Format::Type, formatType);
    QFETCH(QString, result);

    Number num(value);
    PublicValueFormatter fmt(m_converter);
    QCOMPARE(fmt.fractionFormat(num, formatType), result);
}

void TestValueFormatter::testCreateNumberFormat_data()
{
    QTest::addColumn<double>("value");
    QTest::addColumn<int>("precision");
    QTest::addColumn<Format::Type>("formatType");
    QTest::addColumn<Style::FloatFormat>("floatFormat");
    QTest::addColumn<QString>("currencySymbol");
    QTest::addColumn<QString>("formatString");
    QTest::addColumn<bool>("thousandsSep");
    QTest::addColumn<QString>("expected");

    Style::FloatFormat def = Style::DefaultFloatFormat;

    QTest::newRow("negative sign in format string") << -5.0 << 0 << Format::Number << Style::DefaultFloatFormat << ""
                                                    << "(-.)" << false << "(-5)";

    QTest::newRow("unspecified precision 1") << 1.0 << -1 << Format::Number << Style::DefaultFloatFormat << ""
                                             << "0" << false << "1";
    QTest::newRow("unspecified precision 0.5") << 0.5 << -1 << Format::Number << Style::DefaultFloatFormat << ""
                                               << "0" << false << "0.5";

    QTest::newRow("no thousands separators") << 3000.0 << 0 << Format::Number << Style::DefaultFloatFormat << ""
                                             << "" << false << "3000";
    QTest::newRow("with thousands separator") << 3000.0 << 0 << Format::Number << Style::DefaultFloatFormat << ""
                                              << "" << true << "3,000";
    QTest::newRow("bigger number 0") << 300000.456 << 0 << Format::Number << Style::DefaultFloatFormat << ""
                                     << "" << true << "300,000";
    QTest::newRow("bigger number 1") << 300000.456 << 1 << Format::Number << Style::DefaultFloatFormat << ""
                                     << "" << true << "300,000.5";
    QTest::newRow("bigger number 2") << 300000.456 << 2 << Format::Number << Style::DefaultFloatFormat << ""
                                     << "" << true << "300,000.46";
    QTest::newRow("bigger number 3") << 300000.456 << 3 << Format::Number << Style::DefaultFloatFormat << ""
                                     << "" << true << "300,000.456";

    // scientific
    Format::Type scient = Format::Scientific;
    QTest::newRow("scientific: precision=0") << 123.0 << 0 << scient << def << ""
                                             << "" << false << "1E+02";
    QTest::newRow("scientific: precision=1") << 123.0 << 1 << scient << def << ""
                                             << "" << false << "1.2E+02";
    QTest::newRow("scientific: precision=2") << 123.0 << 2 << scient << def << ""
                                             << "" << false << "1.23E+02";
    QTest::newRow("scientific: precision=3") << 123.0 << 3 << scient << def << ""
                                             << "" << false << "1.230E+02";
    QTest::newRow("scientific: precision=-1") << 123.0 << -1 << scient << def << ""
                                              << "" << false << "1.23E+02";

    QTest::newRow("scientific: format=0.E+0") << 123.0 << 0 << scient << def << ""
                                              << "0.E+0" << false << "1E+2";
    QTest::newRow("scientific: format=0.0E+0") << 123.0 << 0 << scient << def << ""
                                               << "0.0E+0" << false << "1.2E+2";
    QTest::newRow("scientific: format=0.00E+0") << 123.0 << 0 << scient << def << ""
                                                << "0.00E+0" << false << "1.23E+2";
    QTest::newRow("scientific: format=0.000E+0") << 123.0 << 0 << scient << def << ""
                                                 << "0.000E+0" << false << "1.230E+2";
    // pre/post
    QTest::newRow("scientific: format=aaa 0.E+0 bbb") << 123.0 << 0 << scient << def << ""
                                                      << "aaa 0.E+0 bbb" << false << "aaa 1E+2 bbb";

    // min number of exponent digits
    QTest::newRow("scientific: exponent=1") << 123.0 << 0 << scient << def << ""
                                            << "0.E+0" << false << "1E+2";
    QTest::newRow("scientific: exponent=2") << 123.0 << 0 << scient << def << ""
                                            << "0.E+00" << false << "1E+02";
    QTest::newRow("scientific: exponent=3") << 123.0 << 0 << scient << def << ""
                                            << "0.E+000" << false << "1E+002";

    // big numbers
    QTest::newRow("scientific: big, exponent=1") << 1234567890123456789012345678901.0 << 0 << scient << def << ""
                                                 << "0.E+0" << false << "1E+30";
    QTest::newRow("scientific: big, exponent=2") << 1234567890123456789012345678901.0 << 0 << scient << def << ""
                                                 << "0.E+00" << false << "1E+30";
    QTest::newRow("scientific: big, exponent=3") << 1234567890123456789012345678901.0 << 0 << scient << def << ""
                                                 << "0.E+000" << false << "1E+030";
    QTest::newRow("scientific: big, round up") << 1234567890123456789012345678901.0 << 0 << scient << def << ""
                                               << "0.000000E+000" << false << "1.234568E+030";

    QTest::newRow("scientific: 10, exponent=1") << 12345678901.0 << 0 << scient << def << ""
                                                << "0.0E+0" << false << "1.2E+10";
    QTest::newRow("scientific: 10, exponent=2") << 12345678901.0 << 0 << scient << def << ""
                                                << "0.0E+00" << false << "1.2E+10";
    QTest::newRow("scientific: 10, exponent=3") << 12345678901.0 << 0 << scient << def << ""
                                                << "0.0E+000" << false << "1.2E+010";

    QTest::newRow("scientific: dec pnt + exp end in 0") << 12345678901.0 << 0 << scient << def << ""
                                                        << "0.0E+000" << false << "1.2E+010";
    QTest::newRow("scientific: dec pnt + value end in 0") << 12300000000.0 << 0 << scient << def << ""
                                                          << "0.00000E+000" << false << "1.23000E+010";

    QTest::newRow("with thousands separators and decimal point") << 3000123.456 << 3 << Format::Number << Style::DefaultFloatFormat << ""
                                                                 << "" << true << "3,000,123.456";
}

void TestValueFormatter::testCreateNumberFormat()
{
    QFETCH(double, value);
    QFETCH(int, precision);
    QFETCH(Format::Type, formatType);
    QFETCH(Style::FloatFormat, floatFormat);
    QFETCH(QString, currencySymbol);
    QFETCH(QString, formatString);
    QFETCH(bool, thousandsSep);
    QFETCH(QString, expected);

    m_calcsettings->locale()->setLanguage("en_US");

    Number num(value);
    PublicValueFormatter fmt(m_converter);

    auto actual = fmt.createNumberFormat(num, precision, formatType, floatFormat, currencySymbol, formatString, thousandsSep);
    QCOMPARE(actual, expected);
}

QTEST_MAIN(TestValueFormatter)
