/* This file is part of the KDE project
   Copyright (C) 2006 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
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
#include "CalendarTester.h"
#include <kptcalendar.h>
#include <kptdatetime.h>
#include <kptduration.h>
#include <kptmap.h>
#include <kunittest/runner.h>
#include <kunittest/module.h>
#include <QString>

using namespace KUnitTest;

KUNITTEST_MODULE(kunittest_CalendarTester, "Calendar Tester");
KUNITTEST_MODULE_REGISTER_TESTER(CalendarTester);

void CalendarTester::allTests() {
    testSingleDay();
    testWeekdays();
    testCalendarWithParent();
}

void CalendarTester::testSingleDay() {
    KPlato::Calendar t("Test");
    QDate wdate(2006,1,2);
    KPlato::DateTime before = KPlato::DateTime(wdate.addDays(-1));
    KPlato::DateTime after = KPlato::DateTime(wdate.addDays(1));
    QTime t1(8,0,0);
    QTime t2(10,0,0);
    KPlato::DateTime wdt1(wdate, t1);
    KPlato::DateTime wdt2(wdate, t2);
    KPlato::CalendarDay *day = new KPlato::CalendarDay(QDate(2006,1,2), KPlato::Map::Working);
    day->addInterval(QPair<QTime, QTime>(t1, t2));
    VERIFY(t.addDay(day));
    COMPARE(t.findDay(wdate), day);
    VERIFY((t.firstAvailableAfter(after, after.addDays(10))).isValid() == false);
    VERIFY((t.firstAvailableBefore(before, before.addDays(-10))).isValid() == false);
    
    COMPARE(t.firstAvailableAfter(before,after).toString(), wdt1.toString());
    COMPARE(t.firstAvailableBefore(after, before).toString(), wdt2.toString());
    
    VERIFY(t.hasInterval(before, after));
    VERIFY(t.hasInterval(after, before) == false);
    
    VERIFY(t.hasInterval(after, after.addDays(1)) == false);
    VERIFY(t.hasInterval(before, before.addDays(-1)) == false);
    
    KPlato::Duration e(0, 2, 0);
    COMPARE((t.effort(before, after)).toString(), e.toString());
    
}

void CalendarTester::testWeekdays() {
    KPlato::Calendar t("Test");
    QDate wdate(2006,1,4); // wednesday
    KPlato::DateTime before = KPlato::DateTime(wdate.addDays(-2));
    KPlato::DateTime after = KPlato::DateTime(wdate.addDays(2));
    QTime t1(8,0,0);
    QTime t2(10,0,0);
    
    KPlato::CalendarDay *wd1 = t.weekday(2); // wednesday
    VERIFY(wd1 != 0);
    
    wd1->setState(KPlato::Map::Working);
    wd1->addInterval(QPair<QTime, QTime>(t1, t2));

    COMPARE(t.firstAvailableAfter(before, after).toString(), QDateTime(QDate(2006, 1, 4), QTime(8,0,0)).toString());
    COMPARE((t.firstAvailableBefore(after, before)).toString(), QDateTime(QDate(2006, 1, 4), QTime(10,0,0)).toString());
    
    COMPARE(t.firstAvailableAfter(after, KPlato::DateTime(QDate(2006,1,14))).toString(), QDateTime(QDate(2006, 1, 11), QTime(8,0,0)).toString());
    COMPARE(t.firstAvailableBefore(before, KPlato::DateTime(QDate(2005,12,25))).toString(), QDateTime(QDate(2005, 12, 28), QTime(10,0,0)).toString());
    
}

void CalendarTester::testCalendarWithParent() {
    KPlato::Calendar t("Test 3");
    KPlato::Calendar p("Test 3 parent");
    t.setParent(&p);
    QDate wdate(2006,1,2);
    KPlato::DateTime before = KPlato::DateTime(wdate.addDays(-1));
    KPlato::DateTime after = KPlato::DateTime(wdate.addDays(1));
    QTime t1(8,0,0);
    QTime t2(10,0,0);
    KPlato::DateTime wdt1(wdate, t1);
    KPlato::DateTime wdt2(wdate, t2);
    
    KPlato::CalendarDay *day = new KPlato::CalendarDay(QDate(2006,1,2), KPlato::Map::Working);
    day->addInterval(QPair<QTime, QTime>(t1, t2));
    COMPARE(p.addDay(day), true);
    COMPARE(p.findDay(wdate), day);
    
    // same tests as in testSingleDay()
    VERIFY((t.firstAvailableAfter(after, after.addDays(10))).isValid() == false);
    VERIFY((t.firstAvailableBefore(before, before.addDays(-10))).isValid() == false);
    
    COMPARE(t.firstAvailableAfter(before,after).toString(), wdt1.toString());
    COMPARE(t.firstAvailableBefore(after, before).toString(), wdt2.toString());
    
    VERIFY(t.hasInterval(before, after));
    VERIFY(t.hasInterval(after, before) == false);
    
    VERIFY(t.hasInterval(after, after.addDays(1)) == false);
    VERIFY(t.hasInterval(before, before.addDays(-1)) == false);
    
    KPlato::Duration e(0, 2, 0);
    COMPARE((t.effort(before, after)).toString(), e.toString());
    
}

