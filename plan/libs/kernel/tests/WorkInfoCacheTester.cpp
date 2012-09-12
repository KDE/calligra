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

#include <qtest_kde.h>
#include <QDir>

#include <kglobal.h>
#include <klocale.h>
#include <kcalendarsystem.h>
#include <ksystemtimezone.h>
#include <kdatetime.h>
#include <kconfiggroup.h>


#include "debug.cpp"

namespace KPlato
{

void WorkInfoCacheTester::initTestCase()
{
    QString kdehome = getenv("KDEHOME");
    QDir d(kdehome);
    d.mkpath("workinfocachetester");
    d.cd("workinfocachetester");

    QString dataDir = d.path();
    QFile f;
    f.setFileName( dataDir + QLatin1String( "/zone.tab" ) );
    f.open(QIODevice::WriteOnly);
    QTextStream fStream(&f);
    fStream << "DE  +5230+01322 Europe/Berlin\n"
               "EG  +3003+03115 Africa/Cairo\n"
               "FR  +4852+00220 Europe/Paris\n"
               "GB  +512830-0001845 Europe/London   Great Britain\n"
               "US  +340308-1181434 America/Los_Angeles Pacific Time\n";
    f.close();
    QDir dir(dataDir);
    QVERIFY(dir.mkdir("Africa"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/zoneinfo/Cairo"), dataDir + QLatin1String("/Africa/Cairo"));
    QVERIFY(dir.mkdir("America"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/zoneinfo/Los_Angeles"), dataDir + QLatin1String("/America/Los_Angeles"));
    QVERIFY(dir.mkdir("Europe"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/zoneinfo/Berlin"), dataDir + QLatin1String("/Europe/Berlin"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/zoneinfo/London"), dataDir + QLatin1String("/Europe/London"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/zoneinfo/Paris"), dataDir + QLatin1String("/Europe/Paris"));

    // NOTE: QTEST_KDEMAIN_CORE puts the config file in QDir::homePath() + "/.kde-unit-test"
    //       and hence, this is common to all unit tests
    KConfig config("ktimezonedrc");
    KConfigGroup group(&config, "TimeZones");
    group.writeEntry("ZoneinfoDir", dataDir);
    group.writeEntry("Zonetab", dataDir + QString::fromLatin1("/zone.tab"));
    group.writeEntry("LocalZone", QString::fromLatin1("Europe/Berlin"));
    config.sync();
}

void WorkInfoCacheTester::cleanupTestCase()
{
    QString kdehome = getenv("KDEHOME");
    removeDir( kdehome + "/workinfocachetester/Africa" );
    removeDir( kdehome + "/workinfocachetester/America" );
    removeDir( kdehome + "/workinfocachetester/Europe" );
    removeDir( kdehome + "/workinfocachetester" );
    removeDir( kdehome + "/share/config" );
    QDir().rmpath(kdehome +"/share/workinfocachetester");
}

void WorkInfoCacheTester::removeDir(const QString &dir)
{
    QDir local(dir);
    foreach(const QString &file, local.entryList(QDir::Files))
        if(!local.remove(file))
            qWarning("%s: removing failed", qPrintable( file ));
        QCOMPARE((int)local.entryList(QDir::Files).count(), 0);
    local.cdUp();
    QString subd = dir;
    subd.remove(QRegExp("^.*/"));
    local.rmpath(subd);
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
    KTimeZone la = KSystemTimeZones::zone("America/Los_Angeles");
    QVERIFY( la.isValid() );
    cal.setTimeZone( la );

    QDate wdate(2012,1,2);
    DateTime before = DateTime(wdate.addDays(-1), QTime());
    DateTime after = DateTime(wdate.addDays(1), QTime());
    QTime t1(14,0,0); // 23 LA
    QTime t2(16,0,0); // 01 LA next day
    DateTime wdt1(wdate, t1);
    DateTime wdt2(wdate, t2);
    int length = t1.msecsTo( t2 );
    CalendarDay *day = new CalendarDay(wdate, CalendarDay::Working);
    day->addInterval(TimeInterval(t1, length));
    cal.addDay(day);
    QVERIFY(cal.findDay(wdate) == day);

    Debug::print( &cal, "America/Los_Angeles" );
    Resource r;
    r.setCalendar( &cal );
    const Resource::WorkInfoCache &wic = r.workInfoCache();
    QVERIFY( ! wic.isValid() );

    r.calendarIntervals( before, after );
    qDebug()<<wic.intervals.map();
    QCOMPARE( wic.intervals.map().count(), 2 );

    QCOMPARE( wic.intervals.map().value( wdate ).startTime(), DateTime( wdate, QTime( 23, 0, 0 ) ) );
    QCOMPARE( wic.intervals.map().value( wdate ).endTime(), DateTime( wdate.addDays( 1 ), QTime( 0, 0, 0 ) ) );

    wdate = wdate.addDays( 1 );
    QCOMPARE( wic.intervals.map().value( wdate ).startTime(), DateTime( wdate, QTime( 0, 0, 0 ) ) );
    QCOMPARE( wic.intervals.map().value( wdate ).endTime(), DateTime( wdate, QTime( 1, 0, 0 ) ) );
}

} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::WorkInfoCacheTester )

#include "WorkInfoCacheTester.moc"
