/* This file is part of the KDE project
   Copyright (C) 2006-2007 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include "CalendarTester.h"
#include "DateTimeTester.h"
#include <kptcalendar.h>
#include <kptdatetime.h>
#include <kptduration.h>
#include <kptmap.h>
#include "kptappointment.h"

#include <QTimeZone>
#include <QDateTime>

#include <QTest>

#include "debug.cpp"

namespace KPlato
{

QTimeZone createTimeZoneWithOffsetFromSystem(int hours, const QString & name)
{
    QTimeZone systemTimeZone = QTimeZone::systemTimeZone();
    int systemOffsetSeconds = systemTimeZone.standardTimeOffset(QDateTime(QDate(1980, 1, 1), QTime(), Qt::UTC));
    int offsetSeconds = systemOffsetSeconds + 3600 * hours;
    if (offsetSeconds >= (14*3600) ) {
        offsetSeconds -= (24*3600);
    } else if (offsetSeconds <= -(14*3600) ) {
        offsetSeconds += (24*3600);
    }
    return QTimeZone(name.toLatin1(), offsetSeconds, name, name);
}

void CalendarTester::testSingleDay() {
    Calendar t("Test");
    QDate wdate(2006,1,2);
    DateTime before = DateTime(wdate.addDays(-1), QTime());
    DateTime after = DateTime(wdate.addDays(1), QTime());
    QTime t1(8,0,0);
    QTime t2(10,0,0);
    DateTime wdt1(wdate, t1);
    DateTime wdt2(wdate, t2);
    int length = t1.msecsTo( t2 );
    CalendarDay *day = new CalendarDay(wdate, CalendarDay::Working);
    day->addInterval(TimeInterval(t1, length));
    t.addDay(day);
    QVERIFY(t.findDay(wdate) == day);

    QVERIFY(t.hasInterval(after, DateTime( after.addDays(1))) == false);
    QVERIFY(t.hasInterval(before, DateTime(before.addDays(-1))) == false);

    QVERIFY(t.hasInterval(after, before) == false);
    QVERIFY(t.hasInterval(before, after));

    QVERIFY((t.firstAvailableAfter(after, DateTime(after.addDays(10)))).isValid() == false);
    QVERIFY((t.firstAvailableBefore(before, DateTime(before.addDays(-10)))).isValid() == false);

    QCOMPARE(t.firstAvailableAfter(before,after), wdt1);
    QCOMPARE(t.firstAvailableBefore(after, before), wdt2);

    Duration e(0, 2, 0);
    QCOMPARE((t.effort(before, after)).toString(), e.toString());
}

void CalendarTester::testWeekdays() {
    Calendar t("Test");
    QDate wdate(2006,1,4); // wednesday
    DateTime before = DateTime(wdate.addDays(-2), QTime());
    DateTime after = DateTime(wdate.addDays(2), QTime());
    QTime t1(8,0,0);
    QTime t2(10,0,0);
    int length = t1.msecsTo( t2 );

    CalendarDay *wd1 = t.weekday(Qt::Wednesday);
    QVERIFY(wd1 != 0);

    wd1->setState(CalendarDay::Working);
    wd1->addInterval(TimeInterval(t1, length));

    QCOMPARE(t.firstAvailableAfter(before, after), DateTime(QDate(2006, 1, 4), QTime(8,0,0)));
    QCOMPARE((t.firstAvailableBefore(after, before)), DateTime(QDate(2006, 1, 4), QTime(10,0,0)));

    QCOMPARE(t.firstAvailableAfter(after, DateTime(QDate(QDate(2006,1,14)), QTime())), DateTime(QDate(2006, 1, 11), QTime(8,0,0)));
    QCOMPARE(t.firstAvailableBefore(before, DateTime(QDate(2005,12,25), QTime())), DateTime(QDate(2005, 12, 28), QTime(10,0,0)));
}

void CalendarTester::testCalendarWithParent() {
    Calendar p("Test 3 parent");
    Calendar t("Test 3");
    t.setParentCal(&p);
    QDate wdate(2006,1,2);
    DateTime before = DateTime(wdate.addDays(-1), QTime());
    DateTime after = DateTime(wdate.addDays(1), QTime());
    QTime t1(8,0,0);
    QTime t2(10,0,0);
    int length = t1.msecsTo( t2 );
    DateTime wdt1(wdate, t1);
    DateTime wdt2(wdate, t2);

    CalendarDay *day = new CalendarDay(wdate, CalendarDay::Working);
    day->addInterval(TimeInterval(t1, length));
    p.addDay(day);
    QVERIFY(p.findDay(wdate) == day);

    //same tests as in testSingleDay()
    QVERIFY(t.hasInterval(after, DateTime(after.addDays(1))) == false);
    QVERIFY(t.hasInterval(before, DateTime(before.addDays(-1))) == false);

    QVERIFY(t.hasInterval(after, before) == false);
    QVERIFY(t.hasInterval(before, after));

    QVERIFY((t.firstAvailableAfter(after, DateTime(after.addDays(10)))).isValid() == false);
    QVERIFY((t.firstAvailableBefore(before, DateTime(before.addDays(-10)))).isValid() == false);

    QVERIFY(t.firstAvailableAfter(before, after).isValid());
    QVERIFY(t.firstAvailableBefore(after, before).isValid());

    QCOMPARE(t.firstAvailableAfter(before,after), wdt1);
    QCOMPARE(t.firstAvailableBefore(after, before), wdt2);

    Duration e(0, 2, 0);
    QCOMPARE((t.effort(before, after)).toString(), e.toString());
}

void CalendarTester::testTimezone()
{
    Calendar t("Test");
    QDate wdate(2006,1,2);
    DateTime before = DateTime(wdate.addDays(-1), QTime());
    DateTime after = DateTime(wdate.addDays(1), QTime());
    QTime t1(8,0,0);
    QTime t2(10,0,0);
    DateTime wdt1(wdate, t1);
    DateTime wdt2(wdate, t2);
    int length = t1.msecsTo( t2 );
    CalendarDay *day = new CalendarDay(wdate, CalendarDay::Working);
    day->addInterval(TimeInterval(t1, length));
    t.addDay(day);
    Debug::print( &t, "Time zone testing" );
    QVERIFY(t.findDay(wdate) == day);

    // local zone: Europe/Berlin ( 1 hours from London )
    QTimeZone lo = createTimeZoneWithOffsetFromSystem(-1, "DummyLondon");
    QVERIFY( lo.isValid() );
    QDateTime dt1 = QDateTime( wdate, t1, lo ).addSecs( -2 * 3600 );
    QDateTime dt2 = QDateTime( wdate, t2, lo ).addSecs( 0 * 3600 );

    qDebug()<<QDateTime( wdt1 )<<QDateTime( wdt2 );
    qDebug()<<dt1<<dt2<<"("<<dt1.toLocalTime()<<dt2.toLocalTime()<<")";
    QCOMPARE(t.firstAvailableAfter( DateTime( dt1 ), after ), wdt1 );
    QCOMPARE(t.firstAvailableBefore( DateTime( dt2 ), before ), wdt2 );

    Duration e(0, 2, 0);
    QCOMPARE( t.effort( DateTime( dt1 ), DateTime( dt2 ) ).toString(), e.toString() );

    // local zone: Europe/Berlin ( 9 hours from America/Los_Angeles )
    QTimeZone la = createTimeZoneWithOffsetFromSystem(-9, "DummyLos_Angeles");
    QVERIFY( la.isValid() );
    QDateTime dt3 = QDateTime( wdate, t1, la ).addSecs( -10 * 3600 );
    QDateTime dt4 = QDateTime( wdate, t2, la ).addSecs( -8 * 3600 );

    qDebug()<<QDateTime( wdt1 )<<QDateTime( wdt2 );
    qDebug()<<dt3<<dt4<<"("<<dt3.toLocalTime()<<dt4.toLocalTime()<<")";
    QCOMPARE(t.firstAvailableAfter( DateTime( dt3 ), after ), wdt1 );
    QCOMPARE(t.firstAvailableBefore( DateTime( dt4 ), before ), wdt2 );

    QCOMPARE( t.effort( DateTime( dt3 ), DateTime( dt4 ) ).toString(), e.toString() );

    QString s = "Test Cairo:";
    qDebug()<<s;
    // local zone: Europe/Berlin ( 1 hour from cairo )
    QTimeZone ca = createTimeZoneWithOffsetFromSystem(1, "DummyCairo");
    QDateTime dt5 = QDateTime( wdate, t1, ca ).addSecs( 0 * 3600 );
    QDateTime dt6 = QDateTime( wdate, t2, ca ).addSecs( 2 * 3600 );

    qDebug()<<QDateTime( wdt1 )<<QDateTime( wdt2 );
    qDebug()<<dt5<<dt6<<"("<<dt5.toLocalTime()<<dt6.toLocalTime()<<")";
    QCOMPARE(t.firstAvailableAfter( DateTime( dt5 ), after ), wdt1 );
    QCOMPARE(t.firstAvailableBefore( DateTime( dt6 ), before ), wdt2 );

    QCOMPARE( t.effort( DateTime( dt5 ), DateTime( dt6 ) ).toString(), e.toString() );
}

void CalendarTester::workIntervals()
{
    Calendar t("Test");
    QDate wdate(2006,1,2);
    DateTime before = DateTime(wdate.addDays(-1), QTime());
    DateTime after = DateTime(wdate.addDays(1), QTime());
    QTime t1(8,0,0);
    QTime t2(10,0,0);
    DateTime wdt1( wdate, t1 );
    DateTime wdt2( wdate, t2 );
    int length = t1.msecsTo( t2 );
    CalendarDay *day = new CalendarDay(wdate, CalendarDay::Working);
    day->addInterval( TimeInterval( t1, length ) );
    t.addDay(day);
    QVERIFY(t.findDay(wdate) == day);

    AppointmentIntervalList lst = t.workIntervals( before, after, 100. );
    QCOMPARE( lst.map().count(), 1 );
    QCOMPARE( wdate, lst.map().values().first().startTime().date() );
    QCOMPARE( t1, lst.map().values().first().startTime().time() );
    QCOMPARE( wdate, lst.map().values().first().endTime().date() );
    QCOMPARE( t2, lst.map().values().first().endTime().time() );
    QCOMPARE( 100., lst.map().values().first().load() );

    QTime t3( 12, 0, 0 );
    day->addInterval( TimeInterval( t3, length ) );

    lst = t.workIntervals( before, after, 100. );
    Debug::print( lst );
    QCOMPARE( lst.map().count(), 2 );
    QCOMPARE( wdate, lst.map().values().first().startTime().date() );
    QCOMPARE( t1, lst.map().values().first().startTime().time() );
    QCOMPARE( wdate, lst.map().values().first().endTime().date() );
    QCOMPARE( t2, lst.map().values().first().endTime().time() );
    QCOMPARE( 100., lst.map().values().first().load() );

    QCOMPARE( wdate, lst.map().values().at( 1 ).startTime().date() );
    QCOMPARE( t3, lst.map().values().at( 1 ).startTime().time() );
    QCOMPARE( wdate, lst.map().values().at( 1 ).endTime().date() );
    QCOMPARE( t3.addMSecs( length ), lst.map().values().at( 1 ).endTime().time() );
    QCOMPARE( 100., lst.map().values().at( 1 ).load() );

    // add interval before the existing
    QTime t4( 5, 30, 0 );
    day->addInterval( TimeInterval( t4, length ) );

    lst = t.workIntervals( before, after, 100. );
    Debug::print( lst );
    QCOMPARE( lst.map().count(), 3 );
    QCOMPARE( wdate, lst.map().values().first().startTime().date() );
    QCOMPARE( t4, lst.map().values().first().startTime().time() );
    QCOMPARE( wdate, lst.map().values().first().endTime().date() );
    QCOMPARE( t4.addMSecs( length ), lst.map().values().first().endTime().time() );
    QCOMPARE( 100., lst.map().values().first().load() );

    QCOMPARE( wdate, lst.map().values().at( 1 ).startTime().date() );
    QCOMPARE( t1, lst.map().values().at( 1 ).startTime().time() );
    QCOMPARE( wdate, lst.map().values().at( 1 ).endTime().date() );
    QCOMPARE( t2, lst.map().values().at( 1 ).endTime().time() );
    QCOMPARE( 100., lst.map().values().at( 1 ).load() );

    QCOMPARE( wdate, lst.map().values().at( 2 ).startTime().date() );
    QCOMPARE( t3, lst.map().values().at( 2 ).startTime().time() );
    QCOMPARE( wdate, lst.map().values().at( 2 ).endTime().date() );
    QCOMPARE( t3.addMSecs( length ), lst.map().values().at( 2 ).endTime().time() );
    QCOMPARE( 100., lst.map().values().at( 2 ).load() );
}

void CalendarTester::workIntervalsFullDays()
{
    Calendar t("Test");
    QDate wdate(2006,1,2);
    DateTime before = DateTime(wdate.addDays(-1), QTime());
    DateTime after = DateTime(wdate.addDays(10), QTime());

    CalendarDay *day = new CalendarDay( wdate, CalendarDay::Working );
    day->addInterval( TimeInterval( QTime( 0, 0, 0), 24*60*60*1000 ) );
    t.addDay(day);

    QCOMPARE( day->numIntervals(), 1 );
    QVERIFY( day->timeIntervals().first()->endsMidnight() );

    DateTime start = day->start();
    DateTime end = day->end();

    QCOMPARE( t.workIntervals( start, end, 100. ).map().count(), 1 );
    QCOMPARE( t.workIntervals( before, after, 100. ).map().count(), 1 );

    day = new CalendarDay( wdate.addDays( 1 ), CalendarDay::Working );
    day->addInterval( TimeInterval( QTime( 0, 0, 0), 24*60*60*1000 ) );
    t.addDay( day );

    end = day->end();

    QCOMPARE( t.workIntervals( start, end, 100. ).map().count(), 2 );
    QCOMPARE( t.workIntervals( before, after, 100. ).map().count(), 2 );

    day = new CalendarDay( wdate.addDays( 2 ), CalendarDay::Working );
    day->addInterval( TimeInterval( QTime( 0, 0, 0), 24*60*60*1000 ) );
    t.addDay( day );

    end = day->end();

    QCOMPARE( t.workIntervals( start, end, 100. ).map().count(), 3 );
    QCOMPARE( t.workIntervals( before, after, 100. ).map().count(), 3 );

}

} //namespace KPlato

QTEST_GUILESS_MAIN( KPlato::CalendarTester )
