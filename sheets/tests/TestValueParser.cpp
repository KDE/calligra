// This file is part of the KDE project
// SPDX-FileCopyrightText: 2015 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#include "TestValueParser.h"

#include "TestKspreadCommon.h"

#include <engine/CS_Time.h>
#include <engine/CalculationSettings.h>
#include <engine/Localization.h>
#include <engine/Value.h>
#include <engine/ValueParser.h>

#include <KLocalizedString>
#include <QStandardPaths>
#include <QTest>

using namespace Calligra::Sheets;

#define USE_LOCALE "da_DK"
#define USE_LANGAGE "nb"

void TestValueParser::initTestCase()
{
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

    KLocalizedString::setApplicationDomain("calligrasheets");
    KLocalizedString::setLanguages(QStringList() << USE_LOCALE);
    QString s = ki18n("true").toString(QStringList() << USE_LANGAGE << USE_LOCALE);
    QVERIFY2(s == QString("sann"), "Translation failed, check that you have the correct .mo file in the data directory and that it installs correctly");

    m_calcsettings = new CalculationSettings();
    m_parser = new ValueParser(m_calcsettings);
}

void TestValueParser::cleanupTestCase()
{
    QStandardPaths::setTestModeEnabled(false);
    QFile::remove(m_translationsFile);

    delete m_parser;
    delete m_calcsettings;
}

void TestValueParser::testTryParseBool_data(bool addCol)
{
    if (addCol) {
        QTest::addColumn<QString>("locale");
        QTest::addColumn<QString>("str");
        QTest::addColumn<bool>("expectedOk");
        QTest::addColumn<Value>("expected");
        QTest::addColumn<QString>("expfail");
        QTest::addColumn<QString>("parsefail");
    }

    QString locale_str = "nb_NO";
    QTest::newRow("en true") << "C"
                             << "true" << true << Value(true) << ""
                             << "";
    QTest::newRow("en false") << "C"
                              << "false" << true << Value(false) << ""
                              << "";
    QTest::newRow("en foobar") << "C"
                               << "foobar" << false << Value() << ""
                               << "";
    QTest::newRow("en TruE") << "C"
                             << "TruE" << true << Value(true) << ""
                             << "";
    QTest::newRow("en fAlSe") << "C"
                              << "fAlSe" << true << Value(false) << ""
                              << "";
    QTest::newRow("en xxtruexx") << "C"
                                 << "sann" << false << Value() << ""
                                 << "";
    QTest::newRow("xx true") << locale_str << "true" << true << Value(true) << ""
                             << "";
    QTest::newRow("xx false") << locale_str << "false" << true << Value(false) << ""
                              << "";
    QTest::newRow("xx foobar") << locale_str << "foobar" << false << Value() << ""
                               << "";
    QTest::newRow("xx TruE") << locale_str << "TruE" << true << Value(true) << ""
                             << "";
    QTest::newRow("xx fAlSe") << locale_str << "fAlSe" << true << Value(false) << ""
                              << "";
    QTest::newRow("xx sann") << locale_str << "sann" << true << Value(true) << ""
                             << "";
    QTest::newRow("xx sAnn") << locale_str << "sAnn" << true << Value(true) << ""
                             << "";
    QTest::newRow("xx usann") << locale_str << "usann" << true << Value(false) << ""
                              << "";
}

void TestValueParser::testTryParseBool()
{
    QFETCH(QString, locale);
    QFETCH(QString, str);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);

    m_calcsettings->locale()->setLanguage(locale);
    QCOMPARE(m_calcsettings->locale()->name(), locale);

    bool ok;
    Value result = m_parser->tryParseBool(str, &ok);
    QCOMPARE(ok, expectedOk);
    QCOMPARE(result, expected);
}

void TestValueParser::testTryParseNumber_data(bool addCol)
{
    if (addCol) {
        QTest::addColumn<QString>("locale");
        QTest::addColumn<QString>("str");
        QTest::addColumn<bool>("expectedOk");
        QTest::addColumn<Value>("expected");
        QTest::addColumn<QString>("expfail");
        QTest::addColumn<QString>("parsefail");
    }

    QTest::newRow("123") << "C"
                         << "123" << true << Value(123) << ""
                         << "";
    QTest::newRow("-456") << "C"
                          << "-456" << true << Value(-456) << ""
                          << "";
    QTest::newRow("+5") << "C"
                        << "+5" << true << Value(5) << ""
                        << "";
    QTest::newRow("1525%") << "C"
                           << "1525%" << true << Value(15.25) << ""
                           << "";
    QTest::newRow("10000000000000000") << "C"
                                       << "10000000000000000" << true << Value(10000000000000000) << ""
                                       << "";
    QTest::newRow("100000000000000000000") << "C"
                                           << "100000000000000000000" << true << Value(1e20) << ""
                                           << ""; // more than int64 can handle
    QTest::newRow("5+3i") << "C"
                          << "5+3i" << true << Value(complex<Number>(5, 3)) << ""
                          << "";
    QTest::newRow("2.4 + 3j") << "C"
                              << "2.4 + 3j" << true << Value(complex<Number>(2.4, 3)) << ""
                              << "";
    QTest::newRow("6 - 3i") << "C"
                            << "6 - 3i" << true << Value(complex<Number>(6, -3)) << ""
                            << "";
    QTest::newRow("2.4i") << "C"
                          << "2.4i" << true << Value(complex<Number>(0, 2.4)) << ""
                          << "";
    QTest::newRow("1,4") << "C"
                         << "1,4" << false << Value() << ""
                         << "";
    QTest::newRow("1,400") << "C"
                           << "1,400" << true << Value(1400) << ""
                           << "";
    QTest::newRow("3 5/2") << "C"
                           << "3 5/2" << true << Value(5.5) << ""
                           << "";
    QTest::newRow("3e2") << "C"
                         << "3e2" << true << Value(300.0) << ""
                         << "";
    QTest::newRow("1234E-2") << "C"
                             << "1234E-2" << true << Value(12.34) << ""
                             << "";
    // QTest::newRow("string 12.34e+5 + 4.2e2i") << "C" << "12.34e+5 + 4.2e2i"
    //     << true << complex<Number>(12.34e5, 4.2e2)<<""<<"";
    // QTest::newRow("string 12.34e+5 + 4.2e+2i") << "C" << "12.34e+5 + 4.2e+2i"
    //     << true << complex<Number>(12.34e5, 4.2e2)<<""<<"";
    QTest::newRow("1,4 nl") << "da_DK"
                            << "1,4" << true << Value(1.4) << ""
                            << "";
    QTest::newRow("1,400 nl") << "da_DK"
                              << "1,400" << true << Value(1.4) << ""
                              << "";
}

void TestValueParser::testTryParseNumber()
{
    QFETCH(QString, locale);
    QFETCH(QString, str);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);
    QFETCH(QString, expfail);

    m_calcsettings->locale()->setLanguage(locale);
    QCOMPARE(m_calcsettings->locale()->name(), locale);

    bool ok;
    Value result = m_parser->tryParseNumber(str, &ok);
    QCOMPARE(ok, expectedOk);
    QCOMPARE(result, expected);
}

void TestValueParser::testTryParseDate_data(bool addCol)
{
    if (addCol) {
        QTest::addColumn<QString>("locale");
        QTest::addColumn<QString>("str");
        QTest::addColumn<bool>("expectedOk");
        QTest::addColumn<Value>("expected");
        QTest::addColumn<QString>("expfail");
        QTest::addColumn<QString>("parsefail");
    }

    // TODO(mek): Return value for invalid dates seems wrong.
    QTest::newRow("ShortDate") << "C"
                               << "2005-06-11" << true << Value(QDate(2005, 6, 11), m_calcsettings) << ""
                               << "";
    QTest::newRow("ShortDate yy") << "C"
                                  << "05-06-11" << true << Value(QDate(2005, 6, 11), m_calcsettings) << ""
                                  << "";
    QTest::newRow("ShortDate m < 1") << "C"
                                     << "2005-00-12" << false << Value(QDate(), m_calcsettings) << ""
                                     << "";
    QTest::newRow("ShortDate m > 12") << "C"
                                      << "2005-13-12" << false << Value(QDate(), m_calcsettings) << ""
                                      << "";
    QTest::newRow("ShortDate d < 1") << "C"
                                     << "2005-02-00" << false << Value(QDate(), m_calcsettings) << ""
                                     << "";
    QTest::newRow("ShortDate d > 31") << "C"
                                      << "2005-02-29" << false << Value(QDate(), m_calcsettings) << ""
                                      << "";
    QTest::newRow("LongDate") << "C"
                              << "Saturday, 01 January 2000" << true << Value(QDate(2000, 1, 1), m_calcsettings) << ""
                              << "";
    QTest::newRow("LongDate wrong weekday") << "C"
                                            << "Friday, 01 January 2000" << true << Value(QDate(2000, 1, 1), m_calcsettings)
                                            << "Qt does not handle wrong weekday"
                                            << "";
    QTest::newRow("LongDate lowercase") << "C"
                                        << "saturday, 01 january 2000" << true << Value(QDate(2000, 1, 1), m_calcsettings) << ""
                                        << "";
    QTest::newRow("LongDate bad day 1") << "C"
                                        << "Saturday, 0 January 2000" << false << Value(QDate(), m_calcsettings) << ""
                                        << "";
    QTest::newRow("LongDate bad day 2") << "C"
                                        << "Saturday, 32 January 2000" << false << Value(QDate(), m_calcsettings) << ""
                                        << "";
    QTest::newRow("LongDate short month") << "C"
                                          << "Tuesday, 1 feb 2000" << true << Value(QDate(2000, 2, 1), m_calcsettings) << ""
                                          << "";
    QTest::newRow("LongDate bad month") << "C"
                                        << "Saturday 1, feburary 2000" << true << Value(QDate(), m_calcsettings) << "Qt returns QDate(\"2000-01-01\")"
                                        << "";

    QTest::newRow("IsoWeekFormat1") << "C"
                                    << "2004-W53-7" << true << Value(QDate(2005, 1, 2), m_calcsettings) << "Weeknumbers not implemented"
                                    << "";
    QTest::newRow("IsoWeekFormat2") << "C"
                                    << "2004-w53-7" << true << Value(QDate(2005, 1, 2), m_calcsettings) << "Weeknumbers not implemented"
                                    << "";
    QTest::newRow("IsoWeekFormat3") << "C"
                                    << "2005-W3-4" << true << Value(QDate(2005, 1, 20), m_calcsettings) << "Weeknumbers not implemented"
                                    << "";
    QTest::newRow("IsoWeekFormat4") << "C"
                                    << "2009-W01-1" << true << Value(QDate(2008, 12, 29), m_calcsettings) << "Weeknumbers not implemented"
                                    << "";
    QTest::newRow("IsoWeekFormat error 1") << "C"
                                           << "2004-W54-7" << false << Value(QDate(), m_calcsettings) << ""
                                           << "";
    QTest::newRow("IsoWeekFormat error 2") << "C"
                                           << "2004-W50-0" << false << Value(QDate(), m_calcsettings) << ""
                                           << "";
    QTest::newRow("IsoWeekFormat error 3") << "C"
                                           << "2004-W50-8" << false << Value(QDate(), m_calcsettings) << ""
                                           << "";
    QTest::newRow("IsoWeekFormat error 4") << "C"
                                           << "2004-W0-1" << false << Value(QDate(), m_calcsettings) << ""
                                           << "";
    QTest::newRow("IsoWeekFormat error 5") << "C"
                                           << "2008-W53-1" << false << Value(QDate(), m_calcsettings) << ""
                                           << "";

    QTest::newRow("IsoDate") << "C"
                             << "2005-06-11" << true << Value(QDate(2005, 6, 11), m_calcsettings) << ""
                             << "";
    QTest::newRow("IsoDate yy") << "C"
                                << "05-06-11" << true << Value(QDate(2005, 6, 11), m_calcsettings) << ""
                                << "";
    QTest::newRow("IsoDate bad day 1") << "C"
                                       << "2005-06-0" << false << Value(QDate(), m_calcsettings) << ""
                                       << "";
    QTest::newRow("IsoDate bad day 2") << "C"
                                       << "2005-06-32" << false << Value(QDate(), m_calcsettings) << ""
                                       << "";
    QTest::newRow("IsoDate bad month 1") << "C"
                                         << "2005-0-06" << false << Value(QDate(), m_calcsettings) << ""
                                         << "";
    QTest::newRow("IsoDate bad month 2") << "C"
                                         << "2005-13-06" << false << Value(QDate(), m_calcsettings) << ""
                                         << "";

    QTest::newRow("IsoOrdinalFormat1") << "C"
                                       << "2004-1" << true << Value(QDate(2004, 1, 1), m_calcsettings) << "Ordinal format not implemented"
                                       << "";
    QTest::newRow("IsoOrdinalFormat2") << "C"
                                       << "2005-51" << true << Value(QDate(2005, 2, 20), m_calcsettings) << "Ordinal format not implemented"
                                       << "";
    QTest::newRow("IsoOrdinalFormat3") << "C"
                                       << "2006-151" << true << Value(QDate(2006, 5, 31), m_calcsettings) << "Ordinal format not implemented"
                                       << "";
    QTest::newRow("IsoOrdinalFormat4") << "C"
                                       << "2000-366" << true << Value(QDate(2000, 12, 31), m_calcsettings) << "Ordinal format not implemented"
                                       << "";
    QTest::newRow("IsoOrdinal error 1") << "C"
                                        << "2006-0" << false << Value(QDate(), m_calcsettings) << ""
                                        << "";
    QTest::newRow("IsoOrdinal error 2") << "C"
                                        << "2006-366" << false << Value(QDate(), m_calcsettings) << ""
                                        << "";
    QTest::newRow("IsoOrdinal error 3") << "C"
                                        << "2000-367" << false << Value(QDate(), m_calcsettings) << ""
                                        << "";
    QTest::newRow("string invalid") << "C"
                                    << "not a date" << false << Value(QDate(), m_calcsettings) << ""
                                    << "";

    QTest::newRow("ShortDate us") << "en_US"
                                  << "06/11/2005" << true << Value(QDate(2005, 6, 11), m_calcsettings) << ""
                                  << "";
    QTest::newRow("ShortDate yy us") << "en_US"
                                     << "6/11/05" << true << Value(QDate(2005, 6, 11), m_calcsettings) << ""
                                     << "";
    QTest::newRow("ShortDate us m < 1") << "en_US"
                                        << "0/11/05" << false << Value(QDate(), m_calcsettings) << ""
                                        << "";
    QTest::newRow("ShortDate us m > 12") << "en_US"
                                         << "13/11/05" << false << Value(QDate(), m_calcsettings) << ""
                                         << "";
    QTest::newRow("ShortDate us d < 1") << "en_US"
                                        << "2/0/05" << false << Value(QDate(), m_calcsettings) << ""
                                        << "";
    QTest::newRow("ShortDate us d > 31") << "en_US"
                                         << "2/29/05" << false << Value(QDate(), m_calcsettings) << ""
                                         << "";

    QTest::newRow("ExcelCompat") << "en_US"
                                 << "3/4/45" << true << Value(QDate(1945, 3, 4), m_calcsettings) << ""
                                 << "";

    QTest::newRow("IsoDate us") << "en_US"
                                << "2005-06-11" << true << Value(QDate(2005, 6, 11), m_calcsettings) << ""
                                << "";
    QTest::newRow("IsoDate yy us") << "en_US"
                                   << "05-06-11" << true << Value(QDate(2005, 6, 11), m_calcsettings) << ""
                                   << "";

    QTest::newRow("ShortDate-year us") << "en_US"
                                       << "12/30" << true << Value(QDate(QDate::currentDate().year(), 12, 30), m_calcsettings) << ""
                                       << "";
    QTest::newRow("ShortDate-year us bad month 1") << "en_US"
                                                   << "0/06" << false << Value(QDate(), m_calcsettings) << ""
                                                   << "";
    QTest::newRow("ShortDate-year us bad month 2") << "en_US"
                                                   << "13/06" << false << Value(QDate(), m_calcsettings) << ""
                                                   << "";
    QTest::newRow("ShortDate-year us bad day 1") << "en_US"
                                                 << "11/0" << false << Value(QDate(), m_calcsettings) << ""
                                                 << "";
    QTest::newRow("ShortDate-year us bad day 2") << "en_US"
                                                 << "11/33" << false << Value(QDate(), m_calcsettings) << ""
                                                 << "";

    QTest::newRow("ShortDate-year nl") << USE_LOCALE << "30.12" << true << Value(QDate(QDate::currentDate().year(), 12, 30), m_calcsettings) << ""
                                       << "";
    QTest::newRow("ShortDate-year nl bad month 1") << USE_LOCALE << "06.0" << false << Value(QDate(), m_calcsettings) << ""
                                                   << "";
    QTest::newRow("ShortDate-year nl bad month 2") << USE_LOCALE << "06.13" << false << Value(QDate(), m_calcsettings) << ""
                                                   << "";
    QTest::newRow("ShortDate-year nl bad day 1") << USE_LOCALE << "11.0" << false << Value(QDate(), m_calcsettings) << ""
                                                 << "";
    QTest::newRow("ShortDate-year nl bad day 2") << USE_LOCALE << "11.33" << false << Value(QDate(), m_calcsettings) << ""
                                                 << "";

    QTest::newRow("ShortDate nl") << USE_LOCALE << "13.06.2005" << true << Value(QDate(2005, 6, 13), m_calcsettings) << ""
                                  << "";
    QTest::newRow("ShortDate yy nl") << USE_LOCALE << "13.06.05" << true << Value(QDate(2005, 6, 13), m_calcsettings) << ""
                                     << "";
    QTest::newRow("ShortDate nl m < 1") << USE_LOCALE << "13.0.05" << false << Value(QDate(), m_calcsettings) << ""
                                        << "";
    QTest::newRow("ShortDate nl m > 12") << USE_LOCALE << "13.13.05" << false << Value(QDate(), m_calcsettings) << ""
                                         << "";
    QTest::newRow("ShortDate nl d < 1") << USE_LOCALE << "0.2.05" << false << Value(QDate(), m_calcsettings) << ""
                                        << "";
    QTest::newRow("ShortDate nl d > 31") << USE_LOCALE << "29.2.05" << false << Value(QDate(), m_calcsettings) << ""
                                         << "";

    QTest::newRow("IsoDate nl") << USE_LOCALE << "2005-06-11" << true << Value(QDate(2005, 6, 11), m_calcsettings) << ""
                                << "";
    QTest::newRow("IsoDate yy nl") << USE_LOCALE << "05-06-11" << true << Value(QDate(2005, 6, 11), m_calcsettings) << ""
                                   << "";
}

void TestValueParser::testTryParseDate()
{
    QFETCH(QString, locale);
    QFETCH(QString, str);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);
    QFETCH(QString, expfail);

    m_calcsettings->locale()->setLanguage(locale);
    QCOMPARE(m_calcsettings->locale()->name(), locale);

    bool ok;
    Value result = m_parser->tryParseDate(str, &ok);
    if (ok != expectedOk && !expfail.isEmpty()) {
        QEXPECT_FAIL("", expfail.toLatin1(), Abort);
    }
    QCOMPARE(ok, expectedOk);
    if (!expfail.isEmpty()) {
        QEXPECT_FAIL("", expfail.toLatin1(), Abort);
    }
    QCOMPARE(result, expected);
}

void TestValueParser::testTryParseTime_data(bool addCol)
{
    if (addCol) {
        QTest::addColumn<QString>("locale");
        QTest::addColumn<QString>("str");
        QTest::addColumn<bool>("expectedOk");
        QTest::addColumn<Value>("expected");
        QTest::addColumn<QString>("expfail");
        QTest::addColumn<QString>("parsefail");
    }

    QTest::newRow("24hr with seconds") << "C"
                                       << "13:14:15" << true << Value(Time(13, 14, 15)) << ""
                                       << "";
    QTest::newRow("0hr") << "C"
                         << "0:14:15" << true << Value(Time(0, 14, 15)) << ""
                         << "";
    QTest::newRow("0hr am") << "C"
                            << "00:14:15 am" << true << Value(Time(0, 14, 15)) << ""
                            << "";
    QTest::newRow("0hr pm") << "C"
                            << "0:14:15 pm" << true << Value(Time(12, 14, 15)) << ""
                            << "";
    QTest::newRow("0hr pM") << "C"
                            << "0:14:15 pM" << true << Value(Time(12, 14, 15)) << "Cannot handle pM (only PM/pm)"
                            << "";
    QTest::newRow("24hr with neg seconds") << "C"
                                           << "13:14:-1" << true << Value(Time(12, 13, 59)) << "TODO: Negative values"
                                           << "";
    QTest::newRow("24hr with large seconds") << "C"
                                             << "13:14:60" << true << Value(Time(13, 14, 60)) << "TODO: Too large values"
                                             << "";
    QTest::newRow("24hr w/o seconds") << "C"
                                      << "13:14" << true << Value(Time(13, 14)) << ""
                                      << "";
    QTest::newRow("25hr duration") << "C"
                                   << "25:01:04" << true << Value(Time(25, 1, 4)) << ""
                                   << "";
    QTest::newRow("12hr") << "C"
                          << "10:14:15" << true << Value(Time(10, 14, 15)) << ""
                          << "";
    QTest::newRow("am with seconds") << "C"
                                     << "10:11:12 am" << true << Value(Time(10, 11, 12)) << ""
                                     << "";
    QTest::newRow("am w/o seconds") << "C"
                                    << "10:11 am" << true << Value(Time(10, 11)) << ""
                                    << "";
    QTest::newRow("pm with seconds") << "C"
                                     << "10:11:12 pm" << true << Value(Time(22, 11, 12)) << ""
                                     << "";
    QTest::newRow("pm w/o seconds") << "C"
                                    << "10:11 pm" << true << Value(Time(22, 11)) << ""
                                    << "";
    QTest::newRow("negative time") << "C"
                                   << "-1:30:10" << true << Value(Time(22, 29, 50)) << "TODO: Negative values"
                                   << "";
    QTest::newRow("time with ms") << "C"
                                  << "2:3:4.5" << true << Value(Time(2, 3, 4, 500)) << ""
                                  << "";
    QTest::newRow("time with ms") << "C"
                                  << "02:03:04.5" << true << Value(Time(2, 3, 4, 500)) << ""
                                  << "";
    QTest::newRow("time with ms 2") << "C"
                                    << "2:3:4.999" << true << Value(Time(2, 3, 4, 999)) << ""
                                    << "";
    QTest::newRow("too low ms") << "C"
                                << "2:3:4.-1" << true << Value(Time(2, 3, 4, -1)) << "TODO: Negative values"
                                << "";
    QTest::newRow("too high ms") << "C"
                                 << "2:3:4:1000" << true << Value(Time(2, 3, 4, 1000)) << "TODO: Too large values"
                                 << "";
    QTest::newRow("24hr with neg minutes") << "C"
                                           << "13:-1:4" << true << Value(Time(13, -1, 4)) << "TODO: Negative values"
                                           << "";
    QTest::newRow("24hr with large minutes") << "C"
                                             << "13:60:10" << true << Value(Time(13, 60, 10)) << "TODO: Too large values"
                                             << "";

    QTest::newRow("24hr with seconds us") << "en_US"
                                          << "13:14:15" << false << Value() << "Needs ap/pm"
                                          << "";
    QTest::newRow("24hr w/o seconds us") << "en_US"
                                         << "13:14" << false << Value() << "Needs am/pm"
                                         << "";

    QTest::newRow("24hr with seconds nl") << USE_LOCALE << "13:14:15" << true << Value(Time(13, 14, 15)) << ""
                                          << "";
    QTest::newRow("24hr with bad seconds 1 nl") << USE_LOCALE << "13:14:-1" << false << Value() << ""
                                                << "";
    QTest::newRow("24hr with bad seconds 2 nl") << USE_LOCALE << "13:14:60" << false << Value() << ""
                                                << "";
    QTest::newRow("24hr w/o seconds nl") << USE_LOCALE << "13:14" << true << Value(Time(13, 14)) << ""
                                         << "";
    QTest::newRow("24hr with bad minutes 1 nl") << USE_LOCALE << "13:-1:4" << false << Value() << ""
                                                << "";
    QTest::newRow("24hr with bad minutes 2 nl") << USE_LOCALE << "13:60:10" << false << Value() << ""
                                                << "";

    QTest::newRow("0hr us") << "en_US"
                            << "0:14:15" << true << Value(Time(0, 14, 15)) << ""
                            << "";
    QTest::newRow("0hr pm us") << "en_US"
                               << "0:14:15 PM" << true << Value(Time(12, 14, 15)) << ""
                               << "";
    QTest::newRow("0hr am us") << "en_US"
                               << "0:14:15 AM" << true << Value(Time(0, 14, 15)) << ""
                               << "";
    QTest::newRow("12hr us") << "en_US"
                             << "10:14:15" << true << Value(Time(10, 14, 15)) << ""
                             << "";
    QTest::newRow("am with seconds us") << "en_US"
                                        << "10:11:12 am" << true << Value(Time(10, 11, 12)) << ""
                                        << "";
    QTest::newRow("am w/o seconds us") << "en_US"
                                       << "10:11 am" << true << Value(Time(10, 11)) << ""
                                       << "";
    QTest::newRow("pm with seconds us") << "en_US"
                                        << "10:11:12 pm" << true << Value(Time(22, 11, 12)) << ""
                                        << "";
    QTest::newRow("pm w/o seconds us") << "en_US"
                                       << "10:11 pm" << true << Value(Time(22, 11)) << ""
                                       << "";
    QTest::newRow("time with ms us") << "en_US"
                                     << "2:3:4.5 am" << true << Value(Time(2, 3, 4, 500)) << ""
                                     << "";
    QTest::newRow("noon us") << "en_US"
                             << "12:00 pm" << true << Value(Time(12, 0)) << ""
                             << "";
    QTest::newRow("midnight us") << "en_US"
                                 << "12:00 am" << true << Value(Time(0, 0)) << ""
                                 << "";
    QTest::newRow("noon us") << "en_US"
                             << "12:00pm" << true << Value(Time(12, 0)) << "TODO: Missing space before pm"
                             << "";
    QTest::newRow("midnight us") << "en_US"
                                 << "12:00am" << true << Value(Time(0, 0)) << "TODO: Missing space before am"
                                 << "";

    QTest::newRow("0hr nl") << USE_LOCALE << "0.14.15" << true << Value(Time(0, 14, 15)) << ""
                            << "";
    QTest::newRow("25hr nl") << USE_LOCALE << "25.01.04" << true << Value(Time(25, 1, 4)) << ""
                             << "";
    QTest::newRow("0hr am nl") << USE_LOCALE << "0.14.15 am" << true << Value(Time(0, 14, 15)) << ""
                               << "";
    QTest::newRow("0hr pm nl") << USE_LOCALE << "0.14.15 pm" << true << Value(Time(12, 14, 15)) << ""
                               << "";
    QTest::newRow("12hr nl") << USE_LOCALE << "10.14.15" << true << Value(Time(10, 14, 15)) << ""
                             << "";
    QTest::newRow("am with seconds nl") << USE_LOCALE << "10.11.12" << true << Value(Time(10, 11, 12)) << ""
                                        << "";
    QTest::newRow("am w/o seconds nl") << USE_LOCALE << "10.11" << true << Value(Time(10, 11)) << ""
                                       << "";
    QTest::newRow("pm with seconds nl") << USE_LOCALE << "22.11.12" << true << Value(Time(22, 11, 12)) << ""
                                        << "";
    QTest::newRow("pm w/o seconds nl") << USE_LOCALE << "22.11" << true << Value(Time(22, 11)) << ""
                                       << "";
    QTest::newRow("time with ms nl") << USE_LOCALE << "2.3.4,5" << true << Value(Time(2, 3, 4, 500)) << ""
                                     << "";
}

void TestValueParser::testTryParseTime()
{
    QFETCH(QString, locale);
    QFETCH(QString, str);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);
    QFETCH(QString, expfail);

    m_calcsettings->locale()->setLanguage(locale);
    QCOMPARE(m_calcsettings->locale()->name(), locale);

    bool ok;
    Value result = m_parser->tryParseTime(str, &ok);
    if (ok != expectedOk && !expfail.isEmpty()) {
        QEXPECT_FAIL("", expfail.toLatin1(), Abort);
    }
    QCOMPARE(ok, expectedOk);
    if (expectedOk) {
        // Negative time values now returns a valid 0 time
        if (result.format() == Value::fmt_Time && expected.format() == Value::fmt_Time) {
            QCOMPARE(result, expected); // FIXME
        } else {
            if (!expfail.isEmpty()) {
                QEXPECT_FAIL("", expfail.toLatin1(), Abort);
            }
            QCOMPARE(result, expected);
        }
    }
}

void TestValueParser::testTryParseDateTime_data()
{
    QTest::addColumn<QString>("locale");
    QTest::addColumn<QString>("str");
    QTest::addColumn<bool>("expectedOk");
    QTest::addColumn<QDate>("date");
    QTest::addColumn<QTime>("time");
    QTest::addColumn<QString>("expfail");

    QTest::newRow("DT Iso short") << "C"
                                  << "2023-01-13T13:14:15" << true << QDate(2023, 1, 13) << QTime(13, 14, 15) << "";
    QTest::newRow("DT Iso long") << "C"
                                 << "2023-01-13T13:14:15.999" << true << QDate(2023, 1, 13) << QTime(13, 14, 15, 999) << "";

    QTest::newRow("DT short") << "C"
                              << "13 Apr 2023 13:14:15" << true << QDate(2023, 4, 13) << QTime(13, 14, 15) << "";
    QTest::newRow("DT long") << "C"
                             << "Thursday, 13 April 2023 13:14:15" << true << QDate(2023, 4, 13) << QTime(13, 14, 15) << "";

    QTest::newRow("DT short us") << "en_US"
                                 << "4/13/23 1:14 pm" << true << QDate(2023, 4, 13) << QTime(13, 14) << "";
    QTest::newRow("DT long us") << "en_US"
                                << "Thursday, April 13, 2023 1:14:15 pm" << true << QDate(2023, 4, 13) << QTime(13, 14, 15) << "";
}

void TestValueParser::testTryParseDateTime()
{
    QFETCH(QString, locale);
    QFETCH(QString, str);
    QFETCH(bool, expectedOk);
    QFETCH(QDate, date);
    QFETCH(QTime, time);
    QFETCH(QString, expfail);

    m_calcsettings->locale()->setLanguage(locale);
    QCOMPARE(m_calcsettings->locale()->name(), locale);

    bool ok;
    QDateTime exp(date, time, Qt::UTC);
    Value expected(exp, m_calcsettings);
    Value result = m_parser->tryParseDateTime(str, &ok);
    if (ok != expectedOk && !expfail.isEmpty()) {
        QEXPECT_FAIL("", expfail.toLatin1(), Abort);
    }
    QCOMPARE(ok, expectedOk);
    if (!expfail.isEmpty()) {
        QEXPECT_FAIL("", expfail.toLatin1(), Abort);
    }
    QCOMPARE(result.asDateTime(m_calcsettings), expected.asDateTime(m_calcsettings));
    QCOMPARE(result, expected);
}

void TestValueParser::testParse_data()
{
    QTest::addColumn<QString>("locale");
    QTest::addColumn<QString>("str");
    QTest::addColumn<bool>("expectedOk");
    QTest::addColumn<Value>("expected");
    QTest::addColumn<QString>("expfail");
    QTest::addColumn<QString>("parsefail");

    testTryParseBool_data(false);
    testTryParseNumber_data(false);
    testTryParseDate_data(false);
    testTryParseTime_data(false);
    // TODO: Datetime cannot be tested this way, add separate test instead
    // testTryParseDateTime_data(false);

    QTest::newRow("empty") << "C"
                           << "" << true << Value("") << ""
                           << "";
    QTest::newRow("bool with quote") << "C"
                                     << "'true" << true << Value("'true") << ""
                                     << "";
    QTest::newRow("int with quote") << "C"
                                    << "'123" << true << Value("'123") << ""
                                    << "";
}

void TestValueParser::testParse()
{
    QFETCH(QString, locale);
    QFETCH(QString, str);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);
    QFETCH(QString, expfail);
    QFETCH(QString, parsefail);

    if (!parsefail.isEmpty()) {
        QEXPECT_FAIL("", parsefail.toLatin1(), Continue);
        QVERIFY(false);
    }
    m_calcsettings->locale()->setLanguage(locale);
    QCOMPARE(m_calcsettings->locale()->name(), locale);

    if (!expectedOk) {
        expected = Value(str);
    }

    Value result = m_parser->parse(str);
    if (result.format() == Value::fmt_Time && expected.format() == Value::fmt_String
        || result.format() == Value::fmt_Date && expected.format() == Value::fmt_Time) {
        // NOTE: Some tests may not work because e.g. an invalid date might be a valid time.
        // E.g: In Danish 13.13.05 is a valid time but an invalid date.
        qDebug() << "Skip test";
    } else {
        if (!expfail.isEmpty()) {
            QEXPECT_FAIL("", expfail.toLatin1(), Abort);
        }
        if (result.format() == Value::fmt_Time && expected.format() == Value::fmt_Time) {
            // Negative times are a bit annoying. For now just compare the Time representations.
            QCOMPARE(result.asTime().toQTime(), expected.asTime().toQTime()); // FIXME
        } else {
            QCOMPARE(result, expected);
        }
    }
}

QTEST_MAIN(TestValueParser)
