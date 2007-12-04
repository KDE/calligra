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

#include "kptcommand.h"
#include "kptcalendar.h"
#include "kptdatetime.h"
#include "kptresource.h"
#include "kptnode.h"
#include "kpttask.h"
#include "kptschedule.h"

#include <QString>

#include <qtest_kde.h>
#include <kdebug.h>

namespace KPlato
{

void ProjectTester::initTestCase()
{
    m_project = new Project();
    m_task = 0;
}

void ProjectTester::cleanupTestCase()
{
    delete m_project;
}

void ProjectTester::testAddTask()
{
    m_task = m_project->createTask( m_project );
    QVERIFY( m_project->addTask( m_task, m_project ) );
    QVERIFY( m_task->parentNode() == m_project );
    QCOMPARE( m_project->findNode( m_task->id() ), m_task );
    
    m_project->takeTask( m_task );
    delete m_task; m_task = 0;
}

void ProjectTester::testTakeTask()
{
    m_task = m_project->createTask( m_project );
    m_project->addTask( m_task, m_project );
    QCOMPARE( m_project->findNode( m_task->id() ), m_task );
    
    m_project->takeTask( m_task );
    QVERIFY( m_project->findNode( m_task->id() ) == 0 );
    
    delete ( m_task ); m_task = 0;
}

void ProjectTester::testTaskAddCmd()
{
    m_task = m_project->createTask( m_project );
    SubtaskAddCmd *cmd = new SubtaskAddCmd( m_project, m_task, m_project );
    cmd->execute();
    QVERIFY( m_task->parentNode() == m_project );
    QCOMPARE( m_project->findNode( m_task->id() ), m_task );
    cmd->unexecute();
    QVERIFY( m_project->findNode( m_task->id() ) == 0 );
    delete cmd;
    m_task = 0;
}

void ProjectTester::testTaskDeleteCmd()
{
    m_task = m_project->createTask( m_project );
    QVERIFY( m_project->addTask( m_task, m_project ) );
    QVERIFY( m_task->parentNode() == m_project );
    
    NodeDeleteCmd *cmd = new NodeDeleteCmd( m_task );
    cmd->execute();
    QVERIFY( m_project->findNode( m_task->id() ) == 0 );
    
    cmd->unexecute();
    QCOMPARE( m_project->findNode( m_task->id() ), m_task );
    
    cmd->execute();
    delete cmd; 
    m_task = 0;
}

void ProjectTester::schedule()
{
    QDate today = QDate::currentDate();
    QDate tomorrow = today.addDays( 1 );
    QDate yesterday = today.addDays( -1 );
    QDate nextweek = today.addDays( 7 );
    QTime t1( 9, 0, 0 );
    QTime t2 ( 17, 0, 0 );
    
    Task *t = m_project->createTask( m_project );
    t->setName( "T1" );
    m_project->addTask( t, m_project );
    t->estimate()->setUnit( Duration::Unit_d );
    t->estimate()->setExpectedEstimate( 1.0 );
    t->estimate()->setType( Estimate::Type_FixedDuration );
    
    ScheduleManager *sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );
    
    QCOMPARE( t->startTime(), m_project->startTime() );
    QCOMPARE( t->endTime(), DateTime(t->startTime().addDays( 1 )) );
    
    // standard worktime defines 8 hour day as default
    Calendar *c = new Calendar();
    c->setDefault( true );
    for ( int i=1; i <= 7; ++i ) {
        CalendarDay *d = c->weekday( i );
        d->setState( CalendarDay::Working );
        d->addInterval( t1, t2 );
    }
    m_project->addCalendar( c );
    
    ResourceGroup *g = new ResourceGroup();
    m_project->addResourceGroup( g );
    Resource *r = new Resource();
    r->setAvailableFrom( QDateTime( yesterday, QTime() ) );
    r->setCalendar( c );
    m_project->addResource( g, r );

    ResourceGroupRequest *gr = new ResourceGroupRequest( g );
    t->addRequest( gr );
    ResourceRequest *rr = new ResourceRequest( r, 100 );
    gr->addResourceRequest( rr );
    t->estimate()->setType( Estimate::Type_Effort );
    
    kDebug()<<"Calculate forward, Task: ASAP -----------------------------------";
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );
    
    QCOMPARE( t->earlyStart(), m_project->startTime() );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );
    
    QCOMPARE( t->startTime(), DateTime( today, t1 ) );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );
    QVERIFY( t->schedulingError() == false );

    kDebug()<<"Calculate forward, Task: ASAP, Resource 50% load ------------------";
    r->setUnits( 50 );
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );
    
    QCOMPARE( t->earlyStart(), m_project->startTime() );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );

    QCOMPARE( t->startTime(), DateTime( today, t1 ) );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 1, 8, 0 ) );
    QVERIFY( t->schedulingError() == false );
    
    kDebug()<<"Calculate forward, Task: ASAP, Resource available tomorrow --------";
    r->setAvailableFrom( QDateTime( tomorrow, QTime() ) );
    r->setUnits( 100 );
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->startTime() );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );
    
    QCOMPARE( t->startTime(), DateTime( r->availableFrom().date(), t1 ) );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );
    QVERIFY( t->schedulingError() == false );
    
    kDebug()<<"Calculate forward, Task: ALAP -----------------------------------";
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::ALAP );
    r->setAvailableFrom( QDateTime( yesterday, QTime() ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );
    
    QCOMPARE( t->earlyStart(), m_project->startTime() );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );
    
    QCOMPARE( t->startTime(), DateTime( today, t1 ) );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );
    QVERIFY( t->schedulingError() == false );

    kDebug()<<"Calculate forward, Task: MustStartOn -----------------------------------";
    r->setAvailableFrom( QDateTime( yesterday, QTime() ) );
    t->setConstraint( Node::MustStartOn );
    t->setConstraintStartTime( DateTime( nextweek, t1 ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->startTime() );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );
    
    QCOMPARE( t->startTime(), DateTime( t->constraintStartTime().date(), t1 ) );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );
    QVERIFY( t->schedulingError() == false );
    
    // Calculate backwards
    kDebug()<<"Calculate backward, Task: MustStartOn -----------------------------------";
    m_project->setConstraint( Node::MustFinishOn );
    m_project->setConstraintEndTime( DateTime( nextweek.addDays( 1 ), QTime() ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->startTime() );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );

    QCOMPARE( t->startTime(), DateTime( t->constraintStartTime().date(), t1 ) );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );
    QVERIFY( t->schedulingError() == false );

    // Calculate backword
    kDebug()<<"Calculate backwards, Task: MustFinishOn -----------------------------------";
    m_project->setConstraint( Node::MustFinishOn );
    m_project->setConstraintEndTime( DateTime( nextweek.addDays( 1 ), QTime() ) );
    t->setConstraint( Node::MustFinishOn );
    t->setConstraintEndTime( DateTime( nextweek.addDays( -2 ), t2 ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->startTime() );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );

    QCOMPARE( t->endTime(), t->constraintEndTime() );
    QCOMPARE( t->startTime(), t->endTime() - Duration( 0, 8, 0 ) );
    QVERIFY( t->schedulingError() == false );

    // Calculate forward
    kDebug()<<"Calculate forwards, Task: MustFinishOn -----------------------------------";
    m_project->setConstraint( Node::MustStartOn );
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::MustFinishOn );
    t->setConstraintEndTime( DateTime( tomorrow, t2 ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->startTime() );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );

    QCOMPARE( t->endTime(), t->constraintEndTime() );
    QCOMPARE( t->startTime(), t->endTime() - Duration( 0, 8, 0 ) );
    QVERIFY( t->schedulingError() == false );

    // Calculate forward
    kDebug()<<"Calculate forwards, Task: StartNotEarlier -----------------------------------";
    m_project->setConstraint( Node::MustStartOn );
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::StartNotEarlier );
    t->setConstraintStartTime( DateTime( today, t2 ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->startTime() );
    QVERIFY( t->lateStart() >=  t->constraintStartTime() );
    QCOMPARE( t->earlyFinish(), t->endTime() );
    QCOMPARE( t->lateFinish(), m_project->endTime() );

    QCOMPARE( t->startTime(), DateTime( tomorrow, t1 ));
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 )  );
    QVERIFY( t->schedulingError() == false );

    // Calculate backward
    kDebug()<<"Calculate backwards, Task: StartNotEarlier -----------------------------------";
    m_project->setConstraint( Node::MustFinishOn );
    m_project->setConstraintEndTime( DateTime( nextweek, QTime() ) );
    t->setConstraint( Node::StartNotEarlier );
    t->setConstraintStartTime( DateTime( today, t2 ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->startTime() );
    QVERIFY( t->lateStart() >=  t->constraintStartTime() );
    QCOMPARE( t->earlyFinish(), t->endTime() );
    QCOMPARE( t->lateFinish(), m_project->endTime() );
    
    QCOMPARE( t->endTime(), DateTime( nextweek.addDays( -1 ), t2 ));
    QCOMPARE( t->startTime(), t->endTime() - Duration( 0, 8, 0 )  );
    QVERIFY( t->schedulingError() == false );

    // Calculate forward
    kDebug()<<"Calculate forwards, Task: FinishNotLater -----------------------------------";
    m_project->setConstraint( Node::MustStartOn );
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::FinishNotLater );
    t->setConstraintEndTime( DateTime( tomorrow.addDays( 1 ), t2 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->startTime() );
    QCOMPARE( t->lateStart(),  t->startTime() );
    QVERIFY( t->earlyFinish() <= t->constraintEndTime() );
    QCOMPARE( t->lateFinish(), m_project->endTime() );

    QCOMPARE( t->startTime(), DateTime( today, t1 ));
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 )  );
    QVERIFY( t->schedulingError() == false );

    // Calculate backward
    kDebug()<<"Calculate backwards, Task: FinishNotLater -----------------------------------";
    m_project->setConstraint( Node::MustFinishOn );
    m_project->setConstraintStartTime( DateTime( nextweek, QTime() ) );
    t->setConstraint( Node::FinishNotLater );
    t->setConstraintEndTime( DateTime( tomorrow, t2 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->startTime() );
    QCOMPARE( t->lateStart(),  t->startTime() );
    QCOMPARE( t->earlyFinish(), t->constraintEndTime() );
    QCOMPARE( t->lateFinish(), m_project->constraintEndTime() );

    QCOMPARE( t->startTime(), DateTime( tomorrow, t1 ));
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 )  );
    QVERIFY( t->schedulingError() == false );

    // Calculate forward
    kDebug()<<"Calculate forwards, Task: FixedInterval -----------------------------------";
    m_project->setConstraint( Node::MustStartOn );
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::FixedInterval );
    t->setConstraintStartTime( DateTime( tomorrow, t1 ) );
    t->setConstraintEndTime( DateTime( tomorrow, t2 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->constraintStartTime() );
    QCOMPARE( t->lateStart(), t->constraintStartTime() );
    QCOMPARE( t->earlyFinish(), t->constraintEndTime() );
    QCOMPARE( t->lateFinish(), t->constraintEndTime() );

    QCOMPARE( t->startTime(), t->constraintStartTime() );
    QCOMPARE( t->endTime(), t->constraintEndTime()  );
    QVERIFY( t->schedulingError() == false );

    // Calculate forward
    kDebug()<<"Calculate forwards, Task: FixedInterval -----------------------------------";
    m_project->setConstraint( Node::MustStartOn );
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::FixedInterval );
    t->setConstraintStartTime( DateTime( tomorrow, QTime() ) ); // outside working hours
    t->setConstraintEndTime( DateTime( tomorrow, t2 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->constraintStartTime() );
    QCOMPARE( t->lateStart(), t->constraintStartTime() );
    QCOMPARE( t->earlyFinish(), t->constraintEndTime() );
    QCOMPARE( t->lateFinish(), t->constraintEndTime() );

    QCOMPARE( t->startTime(), t->constraintStartTime() );
    QCOMPARE( t->endTime(), t->constraintEndTime() );
    QVERIFY( t->schedulingError() == false );

    // Calculate forward
    kDebug()<<"Calculate forwards, Task: Milestone, ASAP-------------------------";
    m_project->setConstraint( Node::MustStartOn );
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::ASAP );
    t->estimate()->clear();
    
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->constraintStartTime() );
    QCOMPARE( t->lateStart(), t->earlyStart() );
    QCOMPARE( t->earlyFinish(), t->earlyStart() );
    QCOMPARE( t->lateFinish(), t->earlyFinish() );

    QCOMPARE( t->startTime(), t->earlyStart() );
    QCOMPARE( t->endTime(), t->startTime() );
    QVERIFY( t->schedulingError() == false );

    // Calculate backward
    kDebug()<<"Calculate backwards, Task: Milestone, ASAP-------------------------";
    m_project->setConstraint( Node::MustFinishOn );
    m_project->setConstraintEndTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::ASAP );
    t->estimate()->clear();
    
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->constraintStartTime() );
    QCOMPARE( t->lateStart(), t->earlyStart() );
    QCOMPARE( t->earlyFinish(), t->earlyStart() );
    QCOMPARE( t->lateFinish(), t->earlyFinish() );

    QCOMPARE( t->startTime(), t->earlyStart() );
    QCOMPARE( t->endTime(), t->startTime() );
    QVERIFY( t->schedulingError() == false );

    // Calculate forward
    kDebug()<<"Calculate forwards, Task: Milestone, ALAP-------------------------";
    m_project->setConstraint( Node::MustStartOn );
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::ALAP );
    t->estimate()->clear();
    
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->constraintStartTime() );
    QCOMPARE( t->lateStart(), t->earlyStart() );
    QCOMPARE( t->earlyFinish(), t->earlyStart() );
    QCOMPARE( t->lateFinish(), t->earlyFinish() );

    QCOMPARE( t->startTime(), t->earlyStart() );
    QCOMPARE( t->endTime(), t->startTime() );
    QVERIFY( t->schedulingError() == false );

    // Calculate backward
    kDebug()<<"Calculate backwards, Task: Milestone, ALAP-------------------------";
    m_project->setConstraint( Node::MustFinishOn );
    m_project->setConstraintEndTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::ALAP );
    t->estimate()->clear();
    
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->constraintStartTime() );
    QCOMPARE( t->lateStart(), t->earlyStart() );
    QCOMPARE( t->earlyFinish(), t->earlyStart() );
    QCOMPARE( t->lateFinish(), t->earlyFinish() );

    QCOMPARE( t->startTime(), t->earlyStart() );
    QCOMPARE( t->endTime(), t->startTime() );
    QVERIFY( t->schedulingError() == false );

    // Calculate forward
    kDebug()<<"Calculate forwards, Task: Milestone, MustStartOn ------------------";
    m_project->setConstraint( Node::MustStartOn );
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::MustStartOn );
    t->setConstraintStartTime( DateTime( tomorrow, t1 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->constraintStartTime() );
    QCOMPARE( t->lateStart(), t->constraintStartTime() );
    QCOMPARE( t->earlyFinish(), t->lateStart() );
    QCOMPARE( t->lateFinish(), t->earlyFinish() );

    QCOMPARE( t->startTime(), t->constraintStartTime() );
    QCOMPARE( t->endTime(), t->startTime() );
    QVERIFY( t->schedulingError() == false );

    // Calculate backward
    kDebug()<<"Calculate backwards, Task: Milestone, MustStartOn ------------------";
    m_project->setConstraint( Node::MustFinishOn );
    m_project->setConstraintEndTime( DateTime( tomorrow, QTime() ) );
    t->setConstraint( Node::MustStartOn );
    t->setConstraintStartTime( DateTime( today, t1 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), t->constraintStartTime() );
    QCOMPARE( t->lateStart(), t->earlyStart() );
    QCOMPARE( t->earlyFinish(), t->earlyStart() );
    QCOMPARE( t->lateFinish(), m_project->constraintEndTime() );

    QCOMPARE( t->startTime(), t->constraintStartTime() );
    QCOMPARE( t->endTime(), t->startTime() );
    QVERIFY( t->schedulingError() == false );

    // Calculate forward
    kDebug()<<"Calculate forwards, Task: Milestone, MustFinishOn ------------------";
    m_project->setConstraint( Node::MustStartOn );
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::MustFinishOn );
    t->setConstraintEndTime( DateTime( tomorrow, t1 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->constraintStartTime() );
    QCOMPARE( t->lateStart(), t->constraintEndTime() );
    QCOMPARE( t->earlyFinish(), t->lateStart() );
    QCOMPARE( t->lateFinish(), t->earlyFinish() );

    QCOMPARE( t->startTime(), t->constraintEndTime() );
    QCOMPARE( t->endTime(), t->startTime() );
    QVERIFY( t->schedulingError() == false );

    QCOMPARE( m_project->endTime(), t->endTime() );

    // Calculate backward
    kDebug()<<"Calculate backwards, Task: Milestone, MustFinishOn ------------------";
    m_project->setConstraint( Node::MustFinishOn );
    m_project->setConstraintEndTime( DateTime( tomorrow, QTime() ) );
    t->setConstraint( Node::MustFinishOn );
    t->setConstraintEndTime( DateTime( today, t1 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), t->constraintEndTime() );
    QCOMPARE( t->lateStart(), t->constraintEndTime() );
    QCOMPARE( t->earlyFinish(), t->lateStart() );
    QCOMPARE( t->lateFinish(), m_project->constraintEndTime() );

    QCOMPARE( t->startTime(), t->constraintEndTime() );
    QCOMPARE( t->endTime(), t->startTime() );
    QVERIFY( t->schedulingError() == false );

    QCOMPARE( m_project->startTime(), t->startTime() );

    // Calculate forward
    kDebug()<<"Calculate forwards, Task: Milestone, StartNotEarlier ---------------";
    m_project->setConstraint( Node::MustStartOn );
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::StartNotEarlier );
    t->setConstraintEndTime( DateTime( tomorrow, t1 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->constraintStartTime() );
    QCOMPARE( t->lateStart(), t->constraintStartTime() );
    QCOMPARE( t->earlyFinish(), t->lateStart() );
    QCOMPARE( t->lateFinish(), t->earlyFinish() );

    QCOMPARE( t->startTime(), t->constraintStartTime() );
    QCOMPARE( t->endTime(), t->startTime() );
    QVERIFY( t->schedulingError() == false );

    QCOMPARE( m_project->endTime(), t->endTime() );

    // Calculate backward
    kDebug()<<"Calculate backwards, Task: Milestone, StartNotEarlier ---------------";
    m_project->setConstraint( Node::MustFinishOn );
    m_project->setConstraintEndTime( DateTime( tomorrow, QTime() ) );
    t->setConstraint( Node::StartNotEarlier );
    t->setConstraintStartTime( DateTime( today, t1 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), t->constraintStartTime() );
    QCOMPARE( t->lateStart(), t->constraintStartTime() );
    QCOMPARE( t->earlyFinish(), t->lateStart() );
    QCOMPARE( t->lateFinish(), m_project->constraintEndTime() );

    QCOMPARE( t->startTime(), t->constraintStartTime() );
    QCOMPARE( t->endTime(), t->startTime() );
    QVERIFY( t->schedulingError() == false );

    QCOMPARE( m_project->startTime(), t->startTime() );

    // Calculate forward
    kDebug()<<"Calculate forwards, Task: Milestone, FinishNotLater ---------------";
    m_project->setConstraint( Node::MustStartOn );
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::FinishNotLater );
    t->setConstraintEndTime( DateTime( tomorrow, t1 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->constraintStartTime() );
    QCOMPARE( t->lateStart(), t->constraintEndTime() );
    QCOMPARE( t->earlyFinish(), t->lateStart() );
    QCOMPARE( t->lateFinish(), t->earlyFinish() );

    QCOMPARE( t->startTime(), t->constraintEndTime() );
    QCOMPARE( t->endTime(), t->startTime() );
    QVERIFY( t->schedulingError() == false );

    QCOMPARE( m_project->endTime(), t->endTime() );

    // Calculate backward
    kDebug()<<"Calculate backwards, Task: Milestone, FinishNotLater ---------------";
    m_project->setConstraint( Node::MustFinishOn );
    m_project->setConstraintEndTime( DateTime( tomorrow, QTime() ) );
    t->setConstraint( Node::FinishNotLater );
    t->setConstraintEndTime( DateTime( today, t1 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), t->constraintStartTime() );
    QCOMPARE( t->lateStart(), t->earlyStart() );
    QCOMPARE( t->earlyFinish(), t->lateStart() );
    QCOMPARE( t->lateFinish(), m_project->constraintEndTime() );

    QCOMPARE( t->startTime(), t->constraintEndTime() );
    QCOMPARE( t->endTime(), t->startTime() );
    QVERIFY( t->schedulingError() == false );

    QCOMPARE( m_project->startTime(), t->startTime() );

    // Calculate forward
    kDebug()<<"Calculate forwards, 2 Task, no overbooking ----------";
    m_project->setConstraint( Node::MustStartOn );
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::ASAP );
    t->estimate()->setUnit( Duration::Unit_d );
    t->estimate()->setExpectedEstimate( 2.0 );
    
    Task *tsk2 = m_project->createTask( *t, m_project );
    tsk2->setName( "T2" );
    m_project->addTask( tsk2, m_project );
    
    gr = new ResourceGroupRequest( g );
    tsk2->addRequest( gr );
    rr = new ResourceRequest( r, 100 );
    gr->addResourceRequest( rr );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setAllowOverbooking( false );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

//     kDebug()<<"earlyStart :"<<t->earlyStart();
//     kDebug()<<"lateStart  :"<<t->lateStart();
//     kDebug()<<"earlyFinish:"<<t->earlyFinish();
//     kDebug()<<"lateFinish :"<<t->lateFinish();
//     kDebug()<<"startTime  :"<<t->startTime();
//     kDebug()<<"endTime    :"<<t->endTime();
//     
//     kDebug()<<"earlyStart :"<<tsk2->earlyStart();
//     kDebug()<<"lateStart  :"<<tsk2->lateStart();
//     kDebug()<<"earlyFinish:"<<tsk2->earlyFinish();
//     kDebug()<<"lateFinish :"<<tsk2->lateFinish();
//     kDebug()<<"startTime  :"<<tsk2->startTime();
//     kDebug()<<"endTime    :"<<tsk2->endTime();

    QCOMPARE( t->earlyStart(), m_project->constraintStartTime() );
    QCOMPARE( t->lateStart(), tsk2->startTime() );
    QCOMPARE( t->earlyFinish(), DateTime( tomorrow, t2 ) );
    QCOMPARE( t->lateFinish(), t->lateFinish() );

    QCOMPARE( t->startTime(), DateTime( today, t1 ) );
    QCOMPARE( t->endTime(), t->earlyFinish() );
    QVERIFY( t->schedulingError() == false );

    //NOTE: lateStart will be earlier than earlyFinish in this test because
    //      T2 will be scheduled to run earlier than T1 if scheduled ALAP:
    //      T1 is scheduled first so is scheduled to run as late as possible,
    //      T2 will then have to be scheduled to run earlier because the resource
    //      is booked by T1.
    QCOMPARE( tsk2->earlyStart(), t->earlyStart() );
    QCOMPARE( tsk2->lateStart(), DateTime( today, t1 ) );
    QCOMPARE( tsk2->earlyFinish(), DateTime( tomorrow.addDays( 2 ), t2 ) );
    QCOMPARE( tsk2->lateFinish(), tsk2->earlyFinish() );

    QCOMPARE( tsk2->startTime(), DateTime( tomorrow.addDays( 1 ), t1 ) );
    QCOMPARE( tsk2->endTime(), tsk2->earlyFinish() );
    QVERIFY( tsk2->schedulingError() == false );
    
    QCOMPARE( m_project->endTime(), tsk2->endTime() );

    // Calculate forward
    kDebug()<<"Calculate forwards, 2 Task, relation ---------------";
    m_project->setConstraint( Node::MustStartOn );
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::ASAP );
    t->estimate()->setUnit( Duration::Unit_d );
    t->estimate()->setExpectedEstimate( 2.0 );
    
    Relation *rel = new Relation( t, tsk2 );
    bool relationAdded = m_project->addRelation( rel );
    QVERIFY( relationAdded );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setAllowOverbooking( true );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->constraintStartTime() );
    QCOMPARE( t->lateStart(), DateTime( today, t1 ) );
    QCOMPARE( t->earlyFinish(), DateTime( tomorrow, t2 ) );
    QCOMPARE( t->lateFinish(), t->lateFinish() );

    QCOMPARE( t->startTime(), DateTime( today, t1 ) );
    QCOMPARE( t->endTime(), t->earlyFinish() );
    QVERIFY( t->schedulingError() == false );

    QCOMPARE( tsk2->earlyStart(), t->earlyFinish() );
    QCOMPARE( tsk2->lateStart(), DateTime( tomorrow.addDays( 1 ), t1 ) );
    QCOMPARE( tsk2->earlyFinish(), DateTime( tomorrow.addDays( 2 ), t2 ) );
    QCOMPARE( tsk2->lateFinish(), tsk2->earlyFinish() );

    QCOMPARE( tsk2->startTime(), DateTime( tomorrow.addDays( 1 ), t1 ) );
    QCOMPARE( tsk2->endTime(), tsk2->earlyFinish() );
    QVERIFY( tsk2->schedulingError() == false );
    
    QCOMPARE( m_project->endTime(), tsk2->endTime() );

}


} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::ProjectTester )

#include "ProjectTester.moc"
