/* This file is part of the KDE project
   Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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
#include "ScheduleTester.h"

#include "kptdatetime.h"
#include "kptschedule.h"


#include <qtest_kde.h>
#include <kdebug.h>

namespace KPlato
{

void ScheduleTester::initTestCase()
{
    date = QDate::currentDate();
    t1 = QTime( 9, 0, 0 );
    t2 = QTime( 12, 0, 0 );
    t3 = QTime( 17, 0, 0 );
    DateTime dt1(date, t1 );
    DateTime dt2( date, t3 );
    resourceSchedule.addAppointment( &nodeSchedule, dt1, dt2, 100. );
    dt1 = DateTime( date.addDays(1), t1 );
    dt2 = DateTime( date.addDays(1), t2 );
    resourceSchedule.addAppointment( &nodeSchedule, dt1, dt2, 100. );
    dt1 = DateTime( date.addDays(1), t2 );
    dt2 = DateTime( date.addDays(1), t3 );
    resourceSchedule.addAppointment( &nodeSchedule, dt1, dt2, 100. );
    dt1 = DateTime( date.addDays(2), t1 );
    dt2 = DateTime( date.addDays(2), t3 );
    resourceSchedule.addAppointment( &nodeSchedule, dt1, dt2, 100. );
}


void ScheduleTester::available()
{
    // before any interval
    DateTimeInterval i1( DateTime( date.addDays( -1 ), t1 ), DateTime( date.addDays( -1 ), t2 ) );
    QCOMPARE( i1, resourceSchedule.available( i1 ) );

    // upto first interval
    i1 = DateTimeInterval( DateTime( date, QTime( 0, 0, 0 ) ), DateTime( date, t1 ) );
    QCOMPARE( i1, resourceSchedule.available( i1 ) );

    // partly into first interval
    i1 = DateTimeInterval( DateTime( date, QTime( 0, 0, 0 ) ), DateTime( date, t2 ) );
    DateTimeInterval res( DateTime( date, QTime( 0, 0, 0 ) ), DateTime( date, t1 ) );
    QCOMPARE( res, resourceSchedule.available( i1 ) );

    // between two intervals, start at end of first
    i1 = DateTimeInterval( DateTime( date, t3 ), DateTime( date, t3.addSecs( 100 ) ) );
    QCOMPARE( i1, resourceSchedule.available( i1 ) );

    // between two intervals, free of both
    i1 = DateTimeInterval( DateTime( date, t3.addSecs(10 ) ), DateTime( date, t3.addSecs( 100 ) ) );
    QCOMPARE( i1, resourceSchedule.available( i1 ) );

    // between two intervals, fill whole hole
    i1 = DateTimeInterval( DateTime( date, t3 ), DateTime( date.addDays(1), t1 ) );
    QCOMPARE( i1, resourceSchedule.available( i1 ) );

    // between two intervals, end at start of second
    i1 = DateTimeInterval( DateTime( date, t3.addSecs(1) ), DateTime( date.addDays(1), t1 ) );
    QCOMPARE( i1, resourceSchedule.available( i1 ) );

    // start into first interval, end into second -> end of first to start of second
    i1 = DateTimeInterval( DateTime( date, t1.addSecs(1) ), DateTime( date.addDays(1), t1.addSecs( 1 ) ) );
    res = DateTimeInterval( DateTime( date, t3 ), DateTime( date.addDays(1), t1 ) );
    QCOMPARE( res, resourceSchedule.available( i1 ) );

}

void ScheduleTester::busy()
{
    // whole first interval
    DateTimeInterval i1( DateTime( date, t1 ), DateTime( date, t3 ) );
    i1 = resourceSchedule.available( i1 );
    QVERIFY( ! i1.first.isValid() && ! i1.second.isValid() );

    // start into interval, end at end
    i1 = DateTimeInterval( DateTime( date, t1.addSecs(100) ), DateTime( date, t3 ) );
    i1 = resourceSchedule.available( i1 );
    QVERIFY( ! i1.first.isValid() && ! i1.second.isValid() );

    // start/end into interval
    i1 = DateTimeInterval( DateTime( date, t1.addSecs(100) ), DateTime( date, t3.addSecs( -100 ) ) );
    i1 = resourceSchedule.available( i1 );
    QVERIFY( ! i1.first.isValid() && ! i1.second.isValid() );

    // span 2 adjacent intervals, whole intervals
    i1 = DateTimeInterval( DateTime( date.addDays(1), t1 ), DateTime( date.addDays(1), t3 ) );
    i1 = resourceSchedule.available( i1 );
    QVERIFY( ! i1.first.isValid() && ! i1.second.isValid() );

    // span 2 adjacent intervals, start into first
    i1 = DateTimeInterval( DateTime( date.addDays(1), t1.addSecs( 100 ) ), DateTime( date.addDays(1), t3 ) );
    i1 = resourceSchedule.available( i1 );
    QVERIFY( ! i1.first.isValid() && ! i1.second.isValid() );

    // span 2 adjacent intervals, start into first, end into second
    i1 = DateTimeInterval( DateTime( date.addDays(1), t1.addSecs( 100 ) ), DateTime( date.addDays(1), t3.addSecs( -100 ) ) );
    i1 = resourceSchedule.available( i1 );
    QVERIFY( ! i1.first.isValid() && ! i1.second.isValid() );

    // span 2 adjacent intervals, start at first, end into second
    i1 = DateTimeInterval( DateTime( date.addDays(1), t1 ), DateTime( date.addDays(1), t3.addSecs( -100 ) ) );
    i1 = resourceSchedule.available( i1 );
    QVERIFY( ! i1.first.isValid() && ! i1.second.isValid() );

}

} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::ScheduleTester )

#include "ScheduleTester.moc"
