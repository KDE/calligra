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
#include <kptcalendar.h>
#include <kptdatetime.h>
#include <kptduration.h>
#include <kptmap.h>

#include <QString>

namespace KPlato
{

void CalendarTester::testSingleDay() {
    Calendar t("Test");
    QDate wdate(2006,1,2);
    DateTime before = DateTime(wdate.addDays(-1));
    DateTime after = DateTime(wdate.addDays(1));
    QTime t1(8,0,0);
    QTime t2(10,0,0);
    DateTime wdt1(wdate, t1);
    DateTime wdt2(wdate, t2);
    CalendarDay *day = new CalendarDay(QDate(2006,1,2), Map::Working);
    day->addInterval(QPair<QTime, QTime>(t1, t2));
    QVERIFY(t.addDay(day));
    QVERIFY(t.findDay(wdate), day);
    QVERIFY((t.firstAvailableAfter(after, after.addDays(10))).isValid() == false);
    QVERIFY((t.firstAvailableBefore(before, before.addDays(-10))).isValid() == false);
    
    QVERIFY(t.firstAvailableAfter(before,after).toString() == wdt1.toString());
    QVERIFY(t.firstAvailableBefore(after, before).toString() == wdt2.toString());
    
    QVERIFY(t.hasInterval(before, after));
    QVERIFY(t.hasInterval(after, before) == false);
    
    QVERIFY(t.hasInterval(after, after.addDays(1)) == false);
    QVERIFY(t.hasInterval(before, before.addDays(-1)) == false);
    
    Duration e(0, 2, 0);
    QVERIFY((t.effort(before, after)).toString() == e.toString());
    
}

void CalendarTester::testWeekdays() {
    Calendar t("Test");
    QDate wdate(2006,1,4); // wednesday
    DateTime before = DateTime(wdate.addDays(-2));
    DateTime after = DateTime(wdate.addDays(2));
    QTime t1(8,0,0);
    QTime t2(10,0,0);
    
    CalendarDay *wd1 = t.weekday(2); // wednesday
    QVERIFY(wd1 != 0);
    
    wd1->setState(Map::Working);
    wd1->addInterval(QPair<QTime, QTime>(t1, t2));

    QVERIFY(t.firstAvailableAfter(before, after).toString() == QDateTime(QDate(2006, 1, 4), QTime(8,0,0)).toString());
    QVERIFY((t.firstAvailableBefore(after, before)).toString() == QDateTime(QDate(2006, 1, 4), QTime(10,0,0)).toString());
    
    QVERIFY(t.firstAvailableAfter(after, DateTime(QDate(2006,1,14))).toString() == QDateTime(QDate(2006, 1, 11), QTime(8,0,0)).toString());
    QVERIFY(t.firstAvailableBefore(before, DateTime(QDate(2005,12,25))).toString() == QDateTime(QDate(2005, 12, 28), QTime(10,0,0)).toString());
    
}

void CalendarTester::testCalendarWithParent() {
    Calendar t("Test 3");
    Calendar p("Test 3 parent");
    t.setParent(&p);
    QDate wdate(2006,1,2);
    DateTime before = DateTime(wdate.addDays(-1));
    DateTime after = DateTime(wdate.addDays(1));
    QTime t1(8,0,0);
    QTime t2(10,0,0);
    DateTime wdt1(wdate, t1);
    DateTime wdt2(wdate, t2);
    
    CalendarDay *day = new CalendarDay(QDate(2006,1,2), Map::Working);
    day->addInterval(QPair<QTime, QTime>(t1, t2));
    QVERIFY(p.addDay(day), true);
    QVERIFY(p.findDay(wdate), day);
    
    // same tests as in testSingleDay()
    QVERIFY((t.firstAvailableAfter(after, after.addDays(10))).isValid() == false);
    QVERIFY((t.firstAvailableBefore(before, before.addDays(-10))).isValid() == false);
    
    QVERIFY(t.firstAvailableAfter(before,after).toString() == wdt1.toString());
    QVERIFY(t.firstAvailableBefore(after, before).toString() == wdt2.toString());
    
    QVERIFY(t.hasInterval(before, after));
    QVERIFY(t.hasInterval(after, before) == false);
    
    QVERIFY(t.hasInterval(after, after.addDays(1)) == false);
    QVERIFY(t.hasInterval(before, before.addDays(-1)) == false);
    
    Duration e(0, 2, 0);
    QVERIFY((t.effort(before, after)).toString() == e.toString());
    
}

} //namespace KPlato

QTEST_MAIN( KPlato::CalendarTester )

#include "CalendarTester.moc"
