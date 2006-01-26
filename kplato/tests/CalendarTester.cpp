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
#include <qstring.h>

using namespace KUnitTest;

KUNITTEST_MODULE(kunittest_CalendarTester, "Calendar Tester");
KUNITTEST_MODULE_REGISTER_TESTER(CalendarTester);

void CalendarTester::allTests() {
    testSingleDay();
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
    CHECK(t.addDay(day), true);
    CHECK(t.findDay(wdate), day);
    CHECK((t.firstAvailableAfter(after, after.addDays(10))).isValid(), false);
    CHECK((t.firstAvailableBefore(before, before.addDays(-10))).isValid(), false);
    
    CHECK(t.firstAvailableAfter(before,after).toString(), wdt1.toString());
    CHECK(t.firstAvailableBefore(after, before).toString(), wdt2.toString());
    
    CHECK(t.hasInterval(before, after), true);
    CHECK(t.hasInterval(after, before), false);
    
    CHECK(t.hasInterval(after, after.addDays(1)), false);
    CHECK(t.hasInterval(before, before.addDays(-1)), false);
    
    KPlato::Duration e(0, 2, 0);
    CHECK((t.effort(before, after)).toString(), e.toString());
    
}
