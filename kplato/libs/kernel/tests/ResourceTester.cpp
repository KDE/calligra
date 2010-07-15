/* This file is part of the KDE project
   Copyright (C) 2006-2010 Dag Andersen <danders@get2net.dk>

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
#include "ResourceTester.h"
#include "DateTimeTester.h"

#include <kptresource.h>
#include <kptcalendar.h>
#include <kptcommand.h>
#include <kptdatetime.h>
#include <kptduration.h>
#include <kptmap.h>


#include <qtest_kde.h>

#include "debug.cpp"

namespace KPlato
{

void ResourceTester::testAvailable()
{
    Resource r;
    QVERIFY( ! r.availableFrom().isValid() );
    QVERIFY( ! r.availableUntil().isValid() );
    
    QDateTime qt = QDateTime::currentDateTime();
    DateTime dt = KDateTime( qt );
    qDebug()<<"dt"<<dt;
    
    r.setAvailableFrom( qt );
    Debug::print( &r, "Test setAvailableFrom with QDateTime" );
    DateTime x = r.availableFrom();
    qDebug()<<"------"<<x;
    QCOMPARE( x, dt );
    qDebug()<<"----------------";
    r.setAvailableUntil( qt.addDays( 1 ) );
    Debug::print( &r, "Test setAvailableUntil with QDateTime" );
    QCOMPARE( r.availableUntil(), DateTime( dt.addDays( 1 ) ) );
    qDebug()<<"----------------";
}

void ResourceTester::testSingleDay() {
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
    
    Resource r;
    r.setAvailableFrom( before );
    r.setAvailableUntil( after );
    r.setCalendar( &t );
    
    Debug::print( &r, "Test single resource, no group, no project", true );

    QVERIFY(r.availableAfter(after, DateTime( after.addDays(1))).isValid() == false);
    QVERIFY(r.availableBefore(before, DateTime(before.addDays(-1))).isValid() == false);
    
    QVERIFY(r.availableAfter(before, after).isValid());
    
    QVERIFY((r.availableAfter(after, DateTime(after.addDays(10)))).isValid() == false);
    QVERIFY((r.availableBefore(before, DateTime(before.addDays(-10)))).isValid() == false);
    
    QCOMPARE(r.availableAfter(before,after), wdt1);
    QCOMPARE(r.availableBefore(after, before), wdt2);
    
    Duration e(0, 2, 0);
    QCOMPARE( r.effort( before, Duration( 2, 0, 0 ) ).toString(), e.toString());

}

void ResourceTester::team()
{
    Resource team;
    Resource tm1, tm2;

    QVERIFY( team.teamMembers().isEmpty() );
    team.addTeamMember( &tm1 );
    QVERIFY( team.teamMembers().count() == 1 );
    team.removeTeamMember( &tm1 );
    QVERIFY( team.teamMembers().isEmpty() );
    team.addTeamMember( &tm1 );
    team.addTeamMember( &tm2 );
    QVERIFY( team.teamMembers().count() == 2 );
    team.addTeamMember( &tm2 );
    QVERIFY( team.teamMembers().count() == 2 );
    team.removeTeamMember( &tm1 );
    team.removeTeamMember( &tm2 );
    QVERIFY( team.teamMembers().isEmpty() );

    AddResourceTeamCmd ac( &team, &tm1 );
    ac.execute();
    QVERIFY( team.teamMembers().count() == 1 );
    ac.unexecute();
    QVERIFY( team.teamMembers().isEmpty() );
    ac.execute();
    RemoveResourceTeamCmd rc( &team, &tm1 );
    rc.execute();
    QVERIFY( team.teamMembers().isEmpty() );
    rc.unexecute();
    QVERIFY( team.teamMembers().count() == 1 );
    
}

} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::ResourceTester )

#include "ResourceTester.moc"
