/* This file is part of the KDE project
   Copyright (C) 20012 Dag Andersen <danders@get2net.dk>

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
#include "WorkInfoCacheTester.h"
#include "DateTimeTester.h"

#include "kptresource.h"
#include "kptcalendar.h"
#include "kptdatetime.h"
#include "kptglobal.h"

#include <QTest>
#include <QDir>


#include "debug.cpp"

namespace KPlato
{

QTimeZone createTimeZoneWithOffsetFromSystem(int hours, const QString & name, int *shiftDays)
{
    QTimeZone systemTimeZone = QTimeZone::systemTimeZone();
    int systemOffsetSeconds = systemTimeZone.standardTimeOffset(QDateTime(QDate(1980, 1, 1), QTime(), Qt::UTC));
    int offsetSeconds = systemOffsetSeconds + 3600 * hours;
    if (offsetSeconds >= (12*3600) ) {
        qDebug() << "reducing offset by 24h";
        offsetSeconds -= (24*3600);
        *shiftDays = -1;
    } else if (offsetSeconds <= -(12*3600) ) {
        qDebug() << "increasing offset by 24h";
        offsetSeconds += (24*3600);
        *shiftDays = 1;
    } else {
        *shiftDays = 0;
    }
    qDebug() << "creating timezone for offset" << hours << offsetSeconds << "systemoffset" << systemOffsetSeconds
             << "shiftDays" << *shiftDays;
    return QTimeZone(name.toLatin1(), offsetSeconds, name, name);
}

void WorkInfoCacheTester::basics()
{
    Calendar cal("Test");
    QDate wdate(2012,1,2);
    DateTime before = DateTime(wdate.addDays(-1), QTime());
    DateTime after = DateTime(wdate.addDays(1), QTime());
    QTime t1(8,0,0);
    QTime t2(10,0,0);
    DateTime wdt1(wdate, t1);
    DateTime wdt2(wdate, t2);
    int length = t1.msecsTo( t2 );
    CalendarDay *day = new CalendarDay(wdate, CalendarDay::Working);
    day->addInterval(TimeInterval(t1, length));
    cal.addDay(day);
    QVERIFY(cal.findDay(wdate) == day);

    Resource r;
    r.setCalendar( &cal );
    const Resource::WorkInfoCache &wic = r.workInfoCache();
    QVERIFY( ! wic.isValid() );

    r.calendarIntervals( before, after );
    qDebug()<<wic.intervals.map();
    QCOMPARE( wic.intervals.map().count(), 1 );

    wdt1 = wdt1.addDays( 1 );
    wdt2 = wdt2.addDays( 1 );
    day = new CalendarDay(wdt1.date(), CalendarDay::Working);
    day->addInterval(TimeInterval(t1, length));
    cal.addDay(day);

    r.calendarIntervals( before, after );
    QCOMPARE( wic.intervals.map().count(), 1 );

    after = after.addDays( 1 );
    r.calendarIntervals( wdt1, after );
    
    qDebug()<<wic.intervals.map();
    QCOMPARE( wic.intervals.map().count(), 2 );
}

void WorkInfoCacheTester::addAfter()
{
    Calendar cal("Test");
    QDate wdate(2012,1,2);
    DateTime before = DateTime(wdate.addDays(-1), QTime());
    DateTime after = DateTime(wdate.addDays(1), QTime());
    QTime t1(8,0,0);
    QTime t2(10,0,0);
    QTime t3(12,0,0);
    QTime t4(14,0,0);
    DateTime wdt1(wdate, t1);
    DateTime wdt2(wdate, t2);
    int length = t1.msecsTo( t2 );
    CalendarDay *day = new CalendarDay(wdate, CalendarDay::Working);
    day->addInterval(TimeInterval(t1, length));
    length = t3.msecsTo( t4 );
    day->addInterval(TimeInterval(t3, length));
    cal.addDay(day);
    QVERIFY(cal.findDay(wdate) == day);

    Resource r;
    r.setCalendar( &cal );
    const Resource::WorkInfoCache &wic = r.workInfoCache();
    QVERIFY( ! wic.isValid() );

    r.calendarIntervals( before, after );
    qDebug()<<wic.intervals.map();
    QCOMPARE( wic.intervals.map().count(), 2 );

    wdt1 = wdt1.addDays( 1 );
    wdt2 = wdt2.addDays( 1 );
    day = new CalendarDay(wdt1.date(), CalendarDay::Working);
    day->addInterval(TimeInterval(t1, length));
    cal.addDay(day);

    // wdate: 8-10, 12-14
    // wdate+1: 8-10
    r.calendarIntervals( DateTime( wdate, t1 ), DateTime( wdate, t2 ) );
    QCOMPARE( wic.intervals.map().count(), 1 );

    r.calendarIntervals( DateTime( wdate, t3 ), DateTime( wdate, t4 ) );
    QCOMPARE( wic.intervals.map().count(), 2 );

    r.calendarIntervals( DateTime( wdate.addDays( 1 ), t1 ), DateTime( wdate.addDays( 1 ), t2 ) );
    qDebug()<<wic.intervals.map();
    QCOMPARE( wic.intervals.map().count(), 3 );
}

void WorkInfoCacheTester::addBefore()
{
    Calendar cal("Test");
    QDate wdate(2012,1,2);
    DateTime before = DateTime(wdate.addDays(-1), QTime());
    DateTime after = DateTime(wdate.addDays(1), QTime());
    QTime t1(8,0,0);
    QTime t2(10,0,0);
    QTime t3(12,0,0);
    QTime t4(14,0,0);
    DateTime wdt1(wdate, t1);
    DateTime wdt2(wdate, t2);
    int length = t1.msecsTo( t2 );
    CalendarDay *day = new CalendarDay(wdate, CalendarDay::Working);
    day->addInterval(TimeInterval(t1, length));
    length = t3.msecsTo( t4 );
    day->addInterval(TimeInterval(t3, length));
    cal.addDay(day);
    QVERIFY(cal.findDay(wdate) == day);

    Resource r;
    r.setCalendar( &cal );
    const Resource::WorkInfoCache &wic = r.workInfoCache();
    QVERIFY( ! wic.isValid() );

    r.calendarIntervals( before, after );
    qDebug()<<wic.intervals.map();
    QCOMPARE( wic.intervals.map().count(), 2 );

    wdt1 = wdt1.addDays( 1 );
    wdt2 = wdt2.addDays( 1 );
    day = new CalendarDay(wdt1.date(), CalendarDay::Working);
    day->addInterval(TimeInterval(t1, length));
    cal.addDay(day);

    // wdate: 8-10, 12-14
    // wdate+1: 8-10
    r.calendarIntervals( DateTime( wdate.addDays( 1 ), t1 ), DateTime( wdate.addDays( 1 ), t2 ) );
    qDebug()<<wic.intervals.map();
    QCOMPARE( wic.intervals.map().count(), 1 );

    r.calendarIntervals( DateTime( wdate, t3 ), DateTime( wdate, t4 ) );
    QCOMPARE( wic.intervals.map().count(), 2 );

    r.calendarIntervals( DateTime( wdate, t1 ), DateTime( wdate, t2 ) );
    QCOMPARE( wic.intervals.map().count(), 3 );
}

void WorkInfoCacheTester::addMiddle()
{
    Calendar cal("Test");
    QDate wdate(2012,1,2);
    DateTime before = DateTime(wdate.addDays(-1), QTime());
    DateTime after = DateTime(wdate.addDays(1), QTime());
    QTime t1(8,0,0);
    QTime t2(10,0,0);
    QTime t3(12,0,0);
    QTime t4(14,0,0);
    DateTime wdt1(wdate, t1);
    DateTime wdt2(wdate, t2);
    int length = t1.msecsTo( t2 );
    CalendarDay *day = new CalendarDay(wdate, CalendarDay::Working);
    day->addInterval(TimeInterval(t1, length));
    length = t3.msecsTo( t4 );
    day->addInterval(TimeInterval(t3, length));
    cal.addDay(day);
    QVERIFY(cal.findDay(wdate) == day);

    Resource r;
    r.setCalendar( &cal );
    const Resource::WorkInfoCache &wic = r.workInfoCache();
    QVERIFY( ! wic.isValid() );

    r.calendarIntervals( before, after );
    qDebug()<<wic.intervals.map();
    QCOMPARE( wic.intervals.map().count(), 2 );

    wdt1 = wdt1.addDays( 1 );
    wdt2 = wdt2.addDays( 1 );
    day = new CalendarDay(wdt1.date(), CalendarDay::Working);
    day->addInterval(TimeInterval(t1, length));
    cal.addDay(day);

    // wdate: 8-10, 12-14
    // wdate+1: 8-10
    r.calendarIntervals( DateTime( wdate.addDays( 1 ), t1 ), DateTime( wdate.addDays( 1 ), t2 ) );
    qDebug()<<wic.intervals.map();
    QCOMPARE( wic.intervals.map().count(), 1 );

    // the middle interval will be filled in automatically
    r.calendarIntervals( DateTime( wdate, t1 ), DateTime( wdate, t2 ) );
    QCOMPARE( wic.intervals.map().count(), 3 );
}

void WorkInfoCacheTester::fullDay()
{
    Calendar cal("Test");
    QDate wdate(2012,1,2);

    QTime t1(0,0,0);
    DateTime wdt1(wdate, t1);
    DateTime wdt2(wdate.addDays( 1 ), t1);
    long length = ( wdt2 - wdt1 ).milliseconds();
    CalendarDay *day = new CalendarDay(wdate, CalendarDay::Working);
    day->addInterval(TimeInterval(t1, length));
    cal.addDay(day);
    QVERIFY(cal.findDay(wdate) == day);

    Resource r;
    r.setCalendar( &cal );
    const Resource::WorkInfoCache &wic = r.workInfoCache();
    QVERIFY( ! wic.isValid() );

    r.calendarIntervals( wdt1, wdt2 );
    qDebug()<<wic.intervals.map();
    QCOMPARE( wic.intervals.map().count(), 1 );

    day = new CalendarDay(wdt2.date(), CalendarDay::Working);
    day->addInterval(TimeInterval(t1, length));
    cal.addDay(day);

    r.calendarIntervals( wdt1, DateTime( wdt2.addDays( 2 ) ) );
    qDebug()<<wic.intervals.map();
    QCOMPARE( wic.intervals.map().count(), 2 );
}

void WorkInfoCacheTester::timeZone()
{
    Calendar cal("Test");
    // local zone: Europe/Berlin ( 9 hours from America/Los_Angeles )
    int laShiftDays;
    QTimeZone la = createTimeZoneWithOffsetFromSystem(-9, "DummyLos_Angeles", &laShiftDays);
    QVERIFY( la.isValid() );
    cal.setTimeZone( la );

    QDate wdate(2012,1,2);
    DateTime before = DateTime(wdate.addDays(-1), QTime());
    DateTime after = DateTime(wdate.addDays(1), QTime());
    qDebug() << "before, after" << before << after;
    QTime t1(14,0,0); // 23 LA
    QTime t2(16,0,0); // 01 LA next day
    DateTime wdt1(wdate, t1);
    DateTime wdt2(wdate, t2);
    int length = t1.msecsTo( t2 );
    qDebug() << "wdt1, wdt2" << wdt1 << wdt2 << wdt1.toTimeZone(la) << wdt2.toTimeZone(la);
    CalendarDay *day = new CalendarDay(wdate, CalendarDay::Working);
    day->addInterval(TimeInterval(t1, length));
    cal.addDay(day);
    QVERIFY(cal.findDay(wdate) == day);

    Debug::print( &cal, "DummyLos_Angeles" );
    Resource r;
    r.setCalendar( &cal );
    const Resource::WorkInfoCache &wic = r.workInfoCache();
    QVERIFY( ! wic.isValid() );

    r.calendarIntervals( before, after );
    qDebug()<<wic.intervals.map();
    QCOMPARE( wic.intervals.map().count(), 2 );

    wdate = wdate.addDays( -laShiftDays );
qDebug() << wdate;
qDebug() << wic.intervals.map().value( wdate );
qDebug() << wic.intervals.map().value( wdate ).startTime();
qDebug() << DateTime( wdate, QTime( 23, 0, 0 ) );
    QCOMPARE( wic.intervals.map().value( wdate ).startTime(), DateTime( wdate, QTime( 23, 0, 0 ) ) );
    QCOMPARE( wic.intervals.map().value( wdate ).endTime(), DateTime( wdate.addDays( 1 ), QTime( 0, 0, 0 ) ) );

    wdate = wdate.addDays( 1 );
    QCOMPARE( wic.intervals.map().value( wdate ).startTime(), DateTime( wdate, QTime( 0, 0, 0 ) ) );
    QCOMPARE( wic.intervals.map().value( wdate ).endTime(), DateTime( wdate, QTime( 1, 0, 0 ) ) );
}

} //namespace KPlato

QTEST_GUILESS_MAIN( KPlato::WorkInfoCacheTester )
