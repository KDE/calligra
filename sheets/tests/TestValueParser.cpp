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
#include "TestValueParser.h"

#include <ValueParser.h>

#include <CalculationSettings.h>
#include <Value.h>

#include <klocale.h>

#include <QTest>

#include <locale.h>

using namespace Calligra::Sheets;

void TestValueParser::initTestCase()
{
    m_calcsettings = new CalculationSettings();
    m_parser = new ValueParser(m_calcsettings);

    QStandardPaths::setTestModeEnabled(true);

    // If run with 'C' locale translations will fail
    // Setting it to 'C.UTF-8' fixes this
    // HACK: (Since I don't really know why)
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
    QString s = ki18n("true").toString(QStringList()<<"nl");
    QVERIFY2(s == QString("xxtruexx"), "Translation failed, check that you have the correct .mo file in the data directory and that it installs correctly");
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
    }

    QTest::newRow("en true") << "C" << "true" << true << Value(true);
    QTest::newRow("en false") << "C" << "false" << true << Value(false);
    QTest::newRow("en foobar") << "C" << "foobar" << false << Value();
    QTest::newRow("en TruE") << "C" << "TruE" << true << Value(true);
    QTest::newRow("en fAlSe") << "C" << "fAlSe" << true << Value(false);
    QTest::newRow("en xxtruexx") << "C" << "xxtruexx" << false << Value();
    QTest::newRow("xx true") << "nl" << "true" << true << Value(true);
    QTest::newRow("xx false") << "nl" << "false" << true << Value(false);
    QTest::newRow("xx foobar") << "nl" << "foobar" << false << Value();
    QTest::newRow("xx TruE") << "nl" << "TruE" << true << Value(true);
    QTest::newRow("xx fAlSe") << "nl" << "fAlSe" << true << Value(false);
    QTest::newRow("xx xxtruexx") << "nl" << "xxtruexx" << true << Value(true);
    QTest::newRow("xx xxtRuexx") << "nl" << "xxtRuexx" << true << Value(true);
    QTest::newRow("xx xxfalSexx") << "nl" << "xxfalSexx" << true << Value(false);
}

void TestValueParser::testTryParseBool()
{
    QFETCH(QString, locale);
    QFETCH(QString, str);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);

    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);

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
    }

    QTest::newRow("123") << "C" << "123" << true << Value(123);
    QTest::newRow("-456") << "C" << "-456" << true << Value(-456);
    QTest::newRow("+5") << "C" << "+5" << true << Value(5);
    QTest::newRow("1525%") << "C" << "1525%" << true << Value(15.25);
    QTest::newRow("5+3i") << "C" << "5+3i" << true << Value(complex<Number>(5, 3));
    QTest::newRow("2.4 + 3j") << "C" << "2.4 + 3j"
        << true << Value(complex<Number>(2.4, 3));
    QTest::newRow("6 - 3i") << "C" << "6 - 3i"
        << true << Value(complex<Number>(6, -3));
    QTest::newRow("2.4i") << "C" << "2.4i" << true << Value(complex<Number>(0, 2.4));
    QTest::newRow("1,4") << "C" << "1,4" << false << Value();
    QTest::newRow("1,400") << "C" << "1,400" << true << Value(1400);
    QTest::newRow("3 5/2") << "C" << "3 5/2" << true << Value(5.5);
    QTest::newRow("3e2") << "C" << "3e2" << true << Value(300.0);
    QTest::newRow("1234E-2") << "C" << "1234E-2" << true << Value(12.34);
    //QTest::newRow("string 12.34e+5 + 4.2e2i") << "C" << "12.34e+5 + 4.2e2i"
    //    << true << complex<Number>(12.34e5, 4.2e2);
    //QTest::newRow("string 12.34e+5 + 4.2e+2i") << "C" << "12.34e+5 + 4.2e+2i"
    //    << true << complex<Number>(12.34e5, 4.2e2);
    QTest::newRow("1,4 nl") << "nl" << "1,4" << true << Value(1.4);
    QTest::newRow("1,400 nl") << "nl" << "1,400" << true << Value(1.4);
}

void TestValueParser::testTryParseNumber()
{
    QFETCH(QString, locale);
    QFETCH(QString, str);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);

    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);

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
    }

    // TODO(mek): Return value for invalid dates seems wrong.
    QTest::newRow("ShortDate") << "C" << "2005-06-11" << true
        << Value(QDate(2005, 6, 11), m_calcsettings);
    QTest::newRow("ShortDate yy") << "C" << "05-06-11" << true
        << Value(QDate(5, 6, 11), m_calcsettings);
    QTest::newRow("ShortDate m < 1") << "C" << "2005-00-12" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("ShortDate m > 12") << "C" << "2005-13-12" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("ShortDate d < 1") << "C" << "2005-02-00" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("ShortDate d > 31") << "C" << "2005-02-29" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("ShortDate us") << "us" << "06/11/2005" << true
        << Value(QDate(2005, 6, 11), m_calcsettings);
    QTest::newRow("ShortDate yy us") << "us" << "6/11/05" << true
        << Value(QDate(2005, 6, 11), m_calcsettings);
    QTest::newRow("ShortDate us m < 1") << "us" << "0/11/05" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("ShortDate us m > 12") << "us" << "13/11/05" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("ShortDate us d < 1") << "us" << "2/0/05" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("ShortDate us d > 31") << "us" << "2/29/05" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("ShortDate nl") << "nl" << "11/06/2005" << true
        << Value(QDate(2005, 6, 11), m_calcsettings);
    QTest::newRow("ShortDate yy nl") << "nl" << "11/06/05" << true
        << Value(QDate(2005, 6, 11), m_calcsettings);
    QTest::newRow("ShortDate nl m < 1") << "nl" << "11/0/05" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("ShortDate nl m > 12") << "nl" << "11/13/05" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("ShortDate nl d < 1") << "nl" << "0/2/05" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("ShortDate nl d > 31") << "nl" << "29/2/05" << false
        << Value(QDate(), m_calcsettings);

    QTest::newRow("LongDate") << "C" << "Saturday 01 January 2000" << true
        << Value(QDate(2000, 1, 1), m_calcsettings);
    // TODO(mek): Should this really parse correctly?
    QTest::newRow("LongDate wrong weekday") << "C" << "Friday 01 January 2000" << true
        << Value(QDate(2000, 1, 1), m_calcsettings);
    QTest::newRow("LongDate lowercase") << "C" << "saturday 01 january 2000" << true
        << Value(QDate(2000, 1, 1), m_calcsettings);
    QTest::newRow("LongDate bad day 1") << "C" << "Saturday 0 January 2000" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("LongDate bad day 2") << "C" << "Saturday 32 January 2000" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("LongDate short month") << "C" << "Saturday 1 feb 2000" << true
        << Value(QDate(2000, 2, 1), m_calcsettings);
    QTest::newRow("LongDate bad month") << "C" << "Saturday 1 feburary 2000" << false
        << Value(QDate(), m_calcsettings);

    QTest::newRow("IsoDate") << "C" << "2005-06-11" << true
        << Value(QDate(2005, 6, 11), m_calcsettings);
    QTest::newRow("IsoDate yy") << "C" << "05-06-11" << true
        << Value(QDate(5, 6, 11), m_calcsettings);
    QTest::newRow("IsoDate bad day 1") << "C" << "2005-06-0" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("IsoDate bad day 2") << "C" << "2005-06-32" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("IsoDate bad month 1") << "C" << "2005-0-06" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("IsoDate bad month 2") << "C" << "2005-13-06" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("IsoDate us") << "us" << "2005-06-11" << true
        << Value(QDate(2005, 6, 11), m_calcsettings);
    QTest::newRow("IsoDate yy us") << "us" << "05-06-11" << true
        << Value(QDate(5, 6, 11), m_calcsettings);
    QTest::newRow("IsoDate nl") << "nl" << "2005-06-11" << true
        << Value(QDate(2005, 6, 11), m_calcsettings);
    QTest::newRow("IsoDate yy nl") << "nl" << "05-06-11" << true
        << Value(QDate(5, 6, 11), m_calcsettings);

    QTest::newRow("IsoWeekFormat1") << "C" << "2004-W53-7" << true
        << Value(QDate(2005, 1, 2), m_calcsettings);
    QTest::newRow("IsoWeekFormat2") << "C" << "2004-w53-7" << true
        << Value(QDate(2005, 1, 2), m_calcsettings);
    QTest::newRow("IsoWeekFormat3") << "C" << "2005-W3-4" << true
        << Value(QDate(2005, 1, 20), m_calcsettings);
    QTest::newRow("IsoWeekFormat4") << "C" << "2009-W01-1" << true
        << Value(QDate(2008, 12, 29), m_calcsettings);
    QTest::newRow("IsoWeekFormat error 1") << "C" << "2004-W54-7" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("IsoWeekFormat error 2") << "C" << "2004-W50-0" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("IsoWeekFormat error 3") << "C" << "2004-W50-8" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("IsoWeekFormat error 4") << "C" << "2004-W0-1" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("IsoWeekFormat error 5") << "C" << "2008-W53-1" << false
        << Value(QDate(), m_calcsettings);

    QTest::newRow("IsoOrdinalFormat1") << "C" << "2004-1" << true
        << Value(QDate(2004, 1, 1), m_calcsettings);
    QTest::newRow("IsoOrdinalFormat2") << "C" << "2005-51" << true
        << Value(QDate(2005, 2, 20), m_calcsettings);
    QTest::newRow("IsoOrdinalFormat3") << "C" << "2006-151" << true
        << Value(QDate(2006, 5, 31), m_calcsettings);
    QTest::newRow("IsoOrdinalFormat4") << "C" << "2000-366" << true
        << Value(QDate(2000, 12, 31), m_calcsettings);
    QTest::newRow("IsoOrdinal error 1") << "C" << "2006-0" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("IsoOrdinal error 2") << "C" << "2006-366" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("IsoOrdinal error 3") << "C" << "2000-367" << false
        << Value(QDate(), m_calcsettings);

    QTest::newRow("ShortDate-year us") << "us" << "12/30" << true
        << Value(QDate(QDate::currentDate().year(), 12, 30), m_calcsettings);
    QTest::newRow("ShortDate-year us bad month 1") << "us" << "0/06" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("ShortDate-year us bad month 2") << "us" << "13/06" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("ShortDate-year us bad day 1") << "us" << "11/0" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("ShortDate-year us bad day 2") << "us" << "11/33" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("ShortDate-year nl") << "nl" << "30/12" << true
        << Value(QDate(QDate::currentDate().year(), 12, 30), m_calcsettings);
    QTest::newRow("ShortDate-year nl bad month 1") << "nl" << "06/0" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("ShortDate-year nl bad month 2") << "nl" << "06/13" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("ShortDate-year nl bad day 1") << "nl" << "11/0" << false
        << Value(QDate(), m_calcsettings);
    QTest::newRow("ShortDate-year nl bad day 2") << "nl" << "11/33" << false
        << Value(QDate(), m_calcsettings);

    QTest::newRow("ExcelCompat") << "us" <<"3/4/45" << true
        << Value(QDate(1945, 3, 4), m_calcsettings);

    QTest::newRow("string invalid") << "C" << "not a date" << false
        << Value(QDate(), m_calcsettings);
}

void TestValueParser::testTryParseDate()
{
    QFETCH(QString, locale);
    QFETCH(QString, str);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);

    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);

    bool ok;
    Value result = m_parser->tryParseDate(str, &ok);
    QCOMPARE(ok, expectedOk);
    QCOMPARE(result, expected);
}

void TestValueParser::testTryParseTime_data(bool addCol)
{
    if (addCol) {
        QTest::addColumn<QString>("locale");
        QTest::addColumn<QString>("str");
        QTest::addColumn<bool>("expectedOk");
        QTest::addColumn<Value>("expected");
    }

    QTest::newRow("24hr with seconds") << "C" << "13:14:15" << true
        << Value(QTime(13, 14, 15));
    QTest::newRow("24hr with seconds us") << "us" << "13:14:15" << false << Value();
    QTest::newRow("24hr with seconds nl") << "nl" << "13:14:15" << true
        << Value(QTime(13, 14, 15));
    QTest::newRow("24hr with bad seconds 1") << "C" << "13:14:-1" << false << Value();
    QTest::newRow("24hr with bad seconds 2") << "C" << "13:14:60" << false << Value();
    QTest::newRow("24hr with bad seconds 1 nl") << "nl" << "13:14:-1" << false << Value();
    QTest::newRow("24hr with bad seconds 2 nl") << "nl" << "13:14:60" << false << Value();
    QTest::newRow("24hr w/o seconds") << "C" << "13:14" << true
        << Value(QTime(13, 14));
    QTest::newRow("24hr w/o seconds us") << "us" << "13:14" << false << Value();
    QTest::newRow("24hr w/o seconds nl") << "nl" << "13:14" << true
        << Value(QTime(13, 14));
    QTest::newRow("24hr with bad minutes 1") << "C" << "13:-1:4" << false << Value();
    QTest::newRow("24hr with bad minutes 2") << "C" << "13:60:10" << false << Value();
    QTest::newRow("24hr with bad minutes 1 nl") << "nl" << "13:-1:4" << false << Value();
    QTest::newRow("24hr with bad minutes 2 nl") << "nl" << "13:60:10" << false << Value();
    QTest::newRow("25hr") << "C" << "25:1:4" << true
        << Value(QTime(1, 1, 4));
    QTest::newRow("25hr nl") << "nl" << "25:1:4" << true
        << Value(QTime(1, 1, 4));
    QTest::newRow("0hr") << "C" << "0:14:15" << true
        << Value(QTime(0, 14, 15));
    QTest::newRow("0hr us") << "us" << "0:14:15" << false << Value();
    QTest::newRow("0hr nl") << "nl" << "0:14:15" << true
        << Value(QTime(0, 14, 15));
    QTest::newRow("0hr am") << "C" << "0:14:15 AM" << true
        << Value(QTime(0, 14, 15));
    QTest::newRow("0hr am us") << "us" << "0:14:15 AM" << false << Value();
    QTest::newRow("0hr am nl") << "nl" << "0:14:15 xxAMxx" << true
        << Value(QTime(0, 14, 15));
    QTest::newRow("0hr pm") << "C" << "0:14:15 pM" << true
        << Value(QTime(12, 14, 15));
    QTest::newRow("0hr pm us") << "us" << "0:14:15 PM" << false << Value();
    QTest::newRow("0hr pm nl") << "nl" << "0:14:15 xxPmxx" << true
        << Value(QTime(12, 14, 15));
    QTest::newRow("12hr") << "C" << "10:14:15" << true
        << Value(QTime(10, 14, 15));
    QTest::newRow("12hr us") << "us" << "10:14:15" << false << Value();
    QTest::newRow("12hr nl") << "nl" << "10:14:15" << true
        << Value(QTime(10, 14, 15));
    QTest::newRow("am with seconds") << "C" << "10:11:12 am" << true
        << Value(QTime(10, 11, 12));
    QTest::newRow("am with seconds us") << "us" << "10:11:12 am" << true
        << Value(QTime(10, 11, 12));
    QTest::newRow("am with seconds nl") << "nl" << "10:11:12 xxamxx" << true
        << Value(QTime(10, 11, 12));
    QTest::newRow("am w/o seconds") << "C" << "10:11 am" << true
        << Value(QTime(10, 11));
    QTest::newRow("am w/o seconds us") << "us" << "10:11 am" << true
        << Value(QTime(10, 11));
    QTest::newRow("am w/o seconds nl") << "nl" << "10:11 xxamxx" << true
        << Value(QTime(10, 11));
    QTest::newRow("pm with seconds") << "C" << "10:11:12 pm" << true
        << Value(QTime(22, 11, 12));
    QTest::newRow("pm with seconds us") << "us" << "10:11:12 pm" << true
        << Value(QTime(22, 11, 12));
    QTest::newRow("pm with seconds nl") << "nl" << "10:11:12 xxpmxx" << true
        << Value(QTime(22, 11, 12));
    QTest::newRow("pm w/o seconds") << "C" << "10:11 pm" << true
        << Value(QTime(22, 11));
    QTest::newRow("pm w/o seconds us") << "us" << "10:11 pm" << true
        << Value(QTime(22, 11));
    QTest::newRow("pm w/o seconds nl") << "nl" << "10:11 xxpmxx" << true
        << Value(QTime(22, 11));
    QTest::newRow("negative time") << "C" << "-1:30:10" << true
        << Value(QTime(22, 29, 50));
    QTest::newRow("time with ms") << "C" << "2:3:4.5" << true
        << Value(QTime(2, 3, 4, 5));
    QTest::newRow("time with ms 2") << "C" << "2:3:4.999" << true
        << Value(QTime(2, 3, 4, 999));
    QTest::newRow("time with ms us") << "us" << "2:3:4.5 am" << true
        << Value(QTime(2, 3, 4, 5));
    QTest::newRow("time with ms nl") << "nl" << "2:3:4,5" << true
        << Value(QTime(2, 3, 4, 5));
    QTest::newRow("noon us") << "us" << "12:00pm" << true
        << Value(QTime(12, 0));
    QTest::newRow("midnight us") << "us" << "12:00am" << true
        << Value(QTime(0, 0));
    QTest::newRow("too low ms") << "C" << "2:3:4.-1" << false << Value();
    QTest::newRow("too high ms") << "C" << "2:3:4:1000" << false << Value();
}

void TestValueParser::testTryParseTime()
{
    QFETCH(QString, locale);
    QFETCH(QString, str);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);

    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);

    bool ok;
    Value result = m_parser->tryParseTime(str, &ok);
    QCOMPARE(ok, expectedOk);
    // Negative times are a bit annoying. For now just compare the QTime representations.
    if (result.format() == Value::fmt_Time && expected.format() == Value::fmt_Time)
        QCOMPARE(result.asTime(), expected.asTime());
    else
        QCOMPARE(result, expected);
}

void TestValueParser::testParse_data() {
    QTest::addColumn<QString>("locale");
    QTest::addColumn<QString>("str");
    QTest::addColumn<bool>("expectedOk");
    QTest::addColumn<Value>("expected");

    testTryParseBool_data(false);
    testTryParseNumber_data(false);
    testTryParseDate_data(false);
    testTryParseTime_data(false);

    QTest::newRow("empty") << "C" << "" << true << Value("");
    QTest::newRow("bool with quote") << "C" << "'true" << true << Value("'true");
    QTest::newRow("int with quote") << "C" << "'123" << true << Value("'123");
}

void TestValueParser::testParse() {
    QFETCH(QString, locale);
    QFETCH(QString, str);
    QFETCH(bool, expectedOk);
    QFETCH(Value, expected);

    *m_calcsettings->locale() = KLocale(locale, locale);
    QCOMPARE(m_calcsettings->locale()->country(), locale);

    Value result = m_parser->parse(str);
    if (!expectedOk)
        expected = Value(str);
    // Negative times are a bit annoying. For now just compare the QTime representations.
    if (result.format() == Value::fmt_Time && expected.format() == Value::fmt_Time)
        QCOMPARE(result.asTime(), expected.asTime());
    else
        QCOMPARE(result, expected);
}

QTEST_MAIN(TestValueParser)
