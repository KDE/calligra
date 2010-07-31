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
#include "ProjectTester.h"

#include "KPlatoRCPSPlugin.h"

#include "kptcommand.h"
#include "kptcalendar.h"
#include "kptdatetime.h"
#include "kptresource.h"
#include "kptnode.h"
#include "kpttask.h"
#include "kptschedule.h"


#include <qtest_kde.h>
#include <kdebug.h>

#include "tests/debug.cpp"

namespace KPlato
{

void ProjectTester::initTestCase()
{
    m_project = new Project();
    m_project->setName( "P1" );
    // standard worktime defines 8 hour day as default
    QVERIFY( m_project->standardWorktime() );
    QCOMPARE( m_project->standardWorktime()->day(), 8.0 );
    m_calendar = new Calendar();
    m_calendar->setDefault( true );
    QTime t1( 9, 0, 0 );
    QTime t2 ( 17, 0, 0 );
    int length = t1.msecsTo( t2 );
    for ( int i=1; i <= 7; ++i ) {
        CalendarDay *d = m_calendar->weekday( i );
        d->setState( CalendarDay::Working );
        d->addInterval( t1, length );
    }
    m_project->addCalendar( m_calendar );

    m_task = 0;
    qDebug()<<"Project:"<<m_project->constraintStartTime()<<m_project->constraintEndTime();
    Debug::print( m_project, "Initiated to:" );
}

void ProjectTester::cleanupTestCase()
{
    delete m_project;
}

void ProjectTester::oneTask()
{
    
    QDate today = QDate::currentDate();
    QDate tomorrow = today.addDays( 1 );
    QDate yesterday = today.addDays( -1 );
    QDate nextweek = today.addDays( 7 );
    QTime t1( 9, 0, 0 );
    QTime t2 ( 17, 0, 0 );
    int length = t1.msecsTo( t2 );

    Task *t = m_project->createTask( m_project );
    t->setName( "T1" );
    m_project->addTask( t, m_project );
    t->estimate()->setUnit( Duration::Unit_d );
    t->estimate()->setExpectedEstimate( 1.0 );
    t->estimate()->setType( Estimate::Type_Duration );

    ScheduleManager *sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    
    KPlatoRCPSPlugin rcps( 0, QVariantList() );

    QString s = "Calculate forward, Task: Fixed duration ------------------------------";

    rcps.calculate( *m_project, sm, true/*nothread*/ );

    Debug::print( m_project, s );
    Debug::print( t, s );

    QCOMPARE( t->startTime(), m_project->startTime() );
    QCOMPARE( t->endTime(), DateTime(t->startTime().addDays( 1 )) );

    s = "Calculate forward, Task: Length --------------------------------------";

    t->estimate()->setCalendar( m_calendar );

    rcps.calculate( *m_project, sm, true/*nothread*/ );

    Debug::print( t, s );

    QCOMPARE( t->startTime(), m_calendar->firstAvailableAfter( m_project->startTime(), m_project->endTime() ) );
    QCOMPARE( t->endTime(), DateTime( t->startTime().addMSecs( length ) ) );

    s = "Calculate forward, Task: Effort --------------------------------------";

    ResourceGroup *g = new ResourceGroup();
    m_project->addResourceGroup( g );
    Resource *r = new Resource();
    r->setAvailableFrom( QDateTime( yesterday, QTime() ) );
    r->setCalendar( m_calendar );
    m_project->addResource( g, r );

    ResourceGroupRequest *gr = new ResourceGroupRequest( g );
    t->addRequest( gr );
    ResourceRequest *rr = new ResourceRequest( r, 100 );
    gr->addResourceRequest( rr );
    t->estimate()->setType( Estimate::Type_Effort );

    rcps.calculate( *m_project, sm, true/*nothread*/ );

    Debug::print( t, s );

    QCOMPARE( t->startTime(), m_calendar->firstAvailableAfter( m_project->startTime(), m_project->endTime() ) );
    QCOMPARE( t->endTime(), DateTime( t->startTime().addMSecs( length ) ) );
}

void ProjectTester::team()
{
    Project project;
    project.setName( "P1" );
    DateTime targetstart = DateTime( QDate( 2010, 5, 1 ), QTime(0,0,0) );
    DateTime targetend = DateTime( targetstart.addDays( 7 ) );
    project.setConstraintStartTime( targetstart );
    project.setConstraintEndTime( targetend);

    Calendar *c = new Calendar("Test");
    QTime t1(8,0,0);
    int length = 8*60*60*1000; // 8 hours

    for ( int i = 1; i <= 7; ++i ) {
        CalendarDay *wd1 = c->weekday(i);
        wd1->setState(CalendarDay::Working);
        wd1->addInterval(TimeInterval(t1, length));
    }
    project.addCalendar( c );

    Task *task1 = project.createTask( &project );
    task1->setName( "T1" );
    project.addTask( task1, &project );
    task1->estimate()->setUnit( Duration::Unit_d );
    task1->estimate()->setExpectedEstimate( 2.0 );
    task1->estimate()->setType( Estimate::Type_Effort );

    KPlatoRCPSPlugin rcps( 0, QVariantList() );

    QString s = "One team with one resource --------";
    qDebug()<<endl<<"Testing:"<<s;
    ResourceGroup *g = new ResourceGroup();
    project.addResourceGroup( g );
    Resource *r1 = new Resource();
    r1->setName( "R1" );
    r1->setCalendar( c );
    project.addResource( g, r1 );

    Resource *r2 = new Resource();
    r2->setName( "Team member" );
    r2->setCalendar( c );
    project.addResource( g, r2 );

    Resource *team = new Resource();
    team->setType( Resource::Type_Team );
    team->setName( "Team" );
    team->addTeamMember( r2 );
    project.addResource( g, team );
    
    ResourceGroupRequest *gr = new ResourceGroupRequest( g );
    task1->addRequest( gr );
    ResourceRequest *tr = new ResourceRequest( team, 100 );
    gr->addResourceRequest( tr );

    ScheduleManager *sm = project.createScheduleManager( "Team" );
    project.addScheduleManager( sm );
    sm->createSchedules();

    rcps.calculate( project, sm, true/*nothread*/ );

//     Debug::print( r1, s);
//     Debug::print( r2, s);
     Debug::print( team, s, false);
     Debug::print( &project, task1, s);
//     Debug::printSchedulingLog( *sm, s );

    DateTime expectedEndTime = targetstart + Duration( 1, 16, 0 );
    QCOMPARE( task1->endTime(), expectedEndTime );

    s = "One team with one resource + one resource --------";
    qDebug()<<endl<<"Testing:"<<s;

    ResourceRequest *rr1 = new ResourceRequest( r1, 100 );
    gr->addResourceRequest( rr1 );

    sm = project.createScheduleManager( "Team + Resource" );
    project.addScheduleManager( sm );
    sm->createSchedules();

    rcps.calculate( project, sm, true/*nothread*/ );

//     Debug::print( r1, s);
//     Debug::print( r2, s);
     Debug::print( team, s, false);
     Debug::print( &project, task1, s);
//     Debug::printSchedulingLog( *sm, s );
    expectedEndTime = targetstart + Duration( 0, 16, 0 );
    QCOMPARE( task1->endTime(), expectedEndTime );

    s = "One team with one resource + one resource, resource available too late --------";
    qDebug()<<endl<<"Testing:"<<s;
    
    r1->setAvailableFrom( targetend );
    r1->setAvailableUntil( targetend.addDays( 7 ) );

    sm = project.createScheduleManager( "Team + Resource not available" );
    project.addScheduleManager( sm );
    sm->createSchedules();
    
    rcps.calculate( project, sm, true/*nothread*/ );

    Debug::print( r1, s);
//    Debug::print( r2, s);
    Debug::print( team, s, false);
    Debug::print( &project, task1, s);
    Debug::printSchedulingLog( *sm, s );
    expectedEndTime = targetstart + Duration( 1, 16, 0 );
    QCOMPARE( task1->endTime(), expectedEndTime );

    s = "One team with two resources --------";
    qDebug()<<endl<<"Testing:"<<s;
    
    r1->removeRequests();
    team->addTeamMember( r1 );
    r1->setAvailableFrom( targetstart );
    r1->setAvailableUntil( targetend );

    sm = project.createScheduleManager( "Team with 2 resources" );
    project.addScheduleManager( sm );
    sm->createSchedules();

    rcps.calculate( project, sm, true/*nothread*/ );

//    Debug::print( r1, s);
//    Debug::print( r2, s);
    Debug::print( team, s, false);
    Debug::print( &project, task1, s);
    Debug::printSchedulingLog( *sm, s );
    expectedEndTime = targetstart + Duration( 0, 16, 0 );
    QCOMPARE( task1->endTime(), expectedEndTime );

    s = "One team with two resources, one resource unavailable --------";
    qDebug()<<endl<<"Testing:"<<s;
    
    r1->setAvailableFrom( targetend );
    r1->setAvailableUntil( targetend.addDays( 2 ) );

    sm = project.createScheduleManager( "Team, one unavailable resource" );
    project.addScheduleManager( sm );
    sm->createSchedules();

    rcps.calculate( project, sm, true/*nothread*/ );

    Debug::print( r1, s);
//    Debug::print( r2, s);
    Debug::print( team, s, false);
    Debug::print( &project, task1, s);
    Debug::printSchedulingLog( *sm, s );
    expectedEndTime = targetstart + Duration( 1, 16, 0 );
    QCOMPARE( task1->endTime(), expectedEndTime );

}

} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::ProjectTester )

#include "ProjectTester.moc"
