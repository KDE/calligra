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


#include <qtest_kde.h>
#include <kdebug.h>

namespace KPlato
{

void ProjectTester::printDebug( Project *p, Task *t ) const {
    qDebug()<<"Debug info -------------------------------------";
    qDebug()<<"project target start:"<<p->constraintStartTime().toString();
    qDebug()<<"project target end:"<<p->constraintEndTime().toString();
    qDebug()<<"project start time:"<<p->startTime().toString();
    qDebug()<<"project end time:"<<p->endTime().toString();

    qDebug()<<"earlyStart:"<<t->earlyStart().toString();
    qDebug()<<"lateStart:"<<t->lateStart().toString();
    qDebug()<<"earlyFinish:"<<t->earlyFinish().toString();
    qDebug()<<"lateFinish:"<<t->lateFinish().toString();
    qDebug()<<"startTime:"<<t->startTime().toString();
    qDebug()<<"endTime:"<<t->endTime().toString();
    switch ( t->constraint() ) {
        case Node::MustStartOn:
        case Node::StartNotEarlier:
            qDebug()<<"startConstraint:"<<t->constraintStartTime().toString();
            break;
        case Node::FixedInterval:
            qDebug()<<"startConstraint:"<<t->constraintStartTime().toString();
        case Node::MustFinishOn:
        case Node::FinishNotLater:
            qDebug()<<"endConstraint:"<<t->constraintEndTime().toString();
            break;
        default: break;
    }
}

void ProjectTester::printSchedulingLog( const ScheduleManager &sm ) const
{
    qDebug()<<"Scheduling log ---------------------------------";
    foreach ( const QString &s, sm.expected()->logMessages() ) {
        qDebug()<<s;
    }
}

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
    int length = t1.msecsTo( t2 );

    Task *t = m_project->createTask( m_project );
    t->setName( "T1" );
    m_project->addTask( t, m_project );
    t->estimate()->setUnit( Duration::Unit_d );
    t->estimate()->setExpectedEstimate( 1.0 );
    t->estimate()->setType( Estimate::Type_Duration );
    
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
        d->addInterval( t1, length );
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
    
    //kDebug()<<"Calculate forward, Task: ASAP -----------------------------------";
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

    //kDebug()<<"Calculate forward, Task: ASAP, Resource 50% load ------------------";
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
    
    //kDebug()<<"Calculate forward, Task: ASAP, Resource available tomorrow --------";
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
    
    //kDebug()<<"Calculate forward, Task: ALAP -----------------------------------";
    m_project->setConstraintStartTime( DateTime( today, QTime(0,0,0) ) );
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

    //kDebug()<<"Calculate forward, Task: MustStartOn -----------------------------------";
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
    //kDebug()<<"Calculate backward, Task: MustStartOn -----------------------------------";
    m_project->setConstraintEndTime( DateTime( nextweek.addDays( 1 ), QTime() ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( true );
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
    //kDebug()<<"Calculate backwards, Task: MustFinishOn -----------------------------------";
    m_project->setConstraintStartTime( DateTime( today, QTime(0,0,0) ) );
    m_project->setConstraintEndTime( DateTime( nextweek.addDays( 1 ), QTime() ) );
    t->setConstraint( Node::MustFinishOn );
    t->setConstraintEndTime( DateTime( nextweek.addDays( -2 ), t2 ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( true );
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
    //kDebug()<<"Calculate forwards, Task: MustFinishOn -----------------------------------";
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::MustFinishOn );
    t->setConstraintEndTime( DateTime( tomorrow, t2 ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( false );
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
    //kDebug()<<"Calculate forwards, Task: StartNotEarlier -----------------------------------";
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::StartNotEarlier );
    t->setConstraintStartTime( DateTime( today, t2 ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( false );
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
    //kDebug()<<"Calculate backwards, Task: StartNotEarlier -----------------------------------";
    m_project->setConstraintEndTime( DateTime( nextweek, QTime() ) );
    t->setConstraint( Node::StartNotEarlier );
    t->setConstraintStartTime( DateTime( today, t2 ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( true );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    //printDebug( m_project, t );

    QVERIFY( t->earlyStart() <= t->constraintStartTime() );
    QVERIFY( t->lateStart() >=  t->constraintStartTime() );
    QCOMPARE( t->earlyFinish(), t->endTime() );
    QVERIFY( t->lateFinish() <= m_project->constraintEndTime() );
    
    QVERIFY( t->endTime() <= t->lateFinish() );
    QCOMPARE( t->startTime(), t->endTime() - Duration( 0, 8, 0 )  );
    QVERIFY( t->schedulingError() == false );

    // Calculate forward
    //kDebug()<<"Calculate forwards, Task: FinishNotLater -----------------------------------";
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::FinishNotLater );
    t->setConstraintEndTime( DateTime( tomorrow.addDays( 1 ), t2 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( false );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), m_project->startTime() );
    QVERIFY( t->startTime() >= t->earlyStart() );
    QVERIFY( t->startTime() <= t->lateStart() );
    QVERIFY( t->startTime() >= m_project->startTime() );
    QVERIFY( t->endTime() >= t->earlyFinish() );
    QVERIFY( t->endTime() <= t->lateFinish() );
    QVERIFY( t->endTime() <= m_project->endTime() );
    QVERIFY( t->earlyFinish() <= t->constraintEndTime() );
    QVERIFY( t->lateFinish() <= m_project->constraintEndTime() );
    QVERIFY( t->schedulingError() == false );

    // Calculate backward
    //kDebug()<<"Calculate backwards, Task: FinishNotLater -----------------------------------";
    m_project->setConstraintStartTime( DateTime( nextweek, QTime() ) );
    t->setConstraint( Node::FinishNotLater );
    t->setConstraintEndTime( DateTime( tomorrow, t2 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( true );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    //printDebug( m_project, t );

    QCOMPARE( t->earlyStart(), m_project->startTime() );
    QCOMPARE( t->lateStart(),  t->startTime() );
    QCOMPARE( t->earlyFinish(), t->constraintEndTime() );
    QVERIFY( t->lateFinish() <= m_project->constraintEndTime() );

    QCOMPARE( t->startTime(), m_project->startTime() );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 )  );
    QVERIFY( t->schedulingError() == false );

    // Calculate forward
    //kDebug()<<"Calculate forwards, Task: FixedInterval -----------------------------------";
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::FixedInterval );
    t->setConstraintStartTime( DateTime( tomorrow, t1 ) );
    t->setConstraintEndTime( DateTime( tomorrow, t2 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( false );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    //printDebug( m_project, t );

    QCOMPARE( t->earlyStart(), m_project->constraintStartTime() );
    QCOMPARE( t->lateStart(), t->constraintStartTime() );
    QCOMPARE( t->earlyFinish(), t->constraintEndTime() );
    QCOMPARE( t->lateFinish(), t->constraintEndTime() );

    QCOMPARE( t->startTime(), t->constraintStartTime() );
    QCOMPARE( t->endTime(), t->constraintEndTime()  );
    QVERIFY( t->schedulingError() == false );

    // Calculate forward
    //kDebug()<<"Calculate forwards, Task: FixedInterval -----------------------------------";
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::FixedInterval );
    t->setConstraintStartTime( DateTime( tomorrow, QTime() ) ); // outside working hours
    t->setConstraintEndTime( DateTime( tomorrow, t2 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( false );
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
    //kDebug()<<"Calculate forwards, Task: Milestone, ASAP-------------------------";
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::ASAP );
    t->estimate()->clear();
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( false );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    //kDebug()<<t->earlyStart()<<m_project->constraintStartTime();
    //kDebug()<<t->lateStart()<<t->earlyStart();
    //kDebug()<<t->earlyFinish()<<t->earlyStart();
    //kDebug()<<t->lateFinish()<<t->earlyFinish();

    QCOMPARE( t->earlyStart(), m_project->constraintStartTime() );
    QCOMPARE( t->lateStart(), t->earlyStart() );
    QCOMPARE( t->earlyFinish(), t->earlyStart() );
    QCOMPARE( t->lateFinish(), t->earlyFinish() );

    QCOMPARE( t->startTime(), t->earlyStart() );
    QCOMPARE( t->endTime(), t->startTime() );
    QVERIFY( t->schedulingError() == false );

    // Calculate backward
    //kDebug()<<"Calculate backwards, Task: Milestone, ASAP-------------------------";
    m_project->setConstraintEndTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::ASAP );
    t->estimate()->clear();
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( true );
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
    //kDebug()<<"Calculate forwards, Task: Milestone, ALAP-------------------------";
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::ALAP );
    t->estimate()->clear();
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( false );
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
    //kDebug()<<"Calculate backwards, Task: Milestone, ALAP-------------------------";
    m_project->setConstraintEndTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::ALAP );
    t->estimate()->clear();
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( true );
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
    //kDebug()<<"Calculate forwards, Task: Milestone, MustStartOn ------------------";
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::MustStartOn );
    t->setConstraintStartTime( DateTime( tomorrow, t1 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( false );
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
    //kDebug()<<"Calculate backwards, Task: Milestone, MustStartOn ------------------";
    m_project->setConstraintEndTime( DateTime( tomorrow, QTime() ) );
    t->setConstraint( Node::MustStartOn );
    t->setConstraintStartTime( DateTime( today, t1 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( true );
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
    //kDebug()<<"Calculate forwards, Task: Milestone, MustFinishOn ------------------";
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::MustFinishOn );
    t->setConstraintEndTime( DateTime( tomorrow, t1 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( false );
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
    //kDebug()<<"Calculate backwards, Task: Milestone, MustFinishOn ------------------";
    m_project->setConstraintEndTime( DateTime( tomorrow, QTime() ) );
    t->setConstraint( Node::MustFinishOn );
    t->setConstraintEndTime( DateTime( today, t1 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( true );
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
    //kDebug()<<"Calculate forwards, Task: Milestone, StartNotEarlier ---------------";
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::StartNotEarlier );
    t->setConstraintEndTime( DateTime( tomorrow, t1 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( false );
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
    //kDebug()<<"Calculate backwards, Task: Milestone, StartNotEarlier ---------------";
    m_project->setConstraintEndTime( DateTime( tomorrow, QTime() ) );
    t->setConstraint( Node::StartNotEarlier );
    t->setConstraintStartTime( DateTime( today, t1 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( true );
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
    //kDebug()<<"Calculate forwards, Task: Milestone, FinishNotLater ---------------";
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::FinishNotLater );
    t->setConstraintEndTime( DateTime( tomorrow, t1 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( false );
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
    //kDebug()<<"Calculate backwards, Task: Milestone, FinishNotLater ---------------";
    m_project->setConstraintEndTime( DateTime( tomorrow, QTime() ) );
    t->setConstraint( Node::FinishNotLater );
    t->setConstraintEndTime( DateTime( today, t1 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( true );
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
    //kDebug()<<"Calculate forwards, 2 Task, no overbooking ----------";
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
    sm->setSchedulingDirection( false );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    //kDebug()<<"estimate   :"<<t->estimate()->expectedEstimate()<<Duration::unitToString(t->estimate()->unit());
    //kDebug()<<"earlyStart :"<<t->earlyStart();
    //kDebug()<<"lateStart  :"<<t->lateStart();
    //kDebug()<<"earlyFinish:"<<t->earlyFinish();
    //kDebug()<<"lateFinish :"<<t->lateFinish();
    //kDebug()<<"startTime  :"<<t->startTime();
    //kDebug()<<"endTime    :"<<t->endTime();
    
    //kDebug()<<"estimate   :"<<tsk2->estimate()->expectedEstimate()<<Duration::unitToString(tsk2->estimate()->unit());
    //kDebug()<<"earlyStart :"<<tsk2->earlyStart();
    //kDebug()<<"lateStart  :"<<tsk2->lateStart();
    //kDebug()<<"earlyFinish:"<<tsk2->earlyFinish();
    //kDebug()<<"lateFinish :"<<tsk2->lateFinish();
    //kDebug()<<"startTime  :"<<tsk2->startTime();
    //kDebug()<<"endTime    :"<<tsk2->endTime();

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
    //kDebug()<<"Calculate forwards, 2 Task, relation ---------------";
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::ASAP );
    t->estimate()->setUnit( Duration::Unit_d );
    t->estimate()->setExpectedEstimate( 2.0 );
    
    Relation *rel = new Relation( t, tsk2 );
    bool relationAdded = m_project->addRelation( rel );
    QVERIFY( relationAdded );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setAllowOverbooking( true );
    sm->setSchedulingDirection( false );
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

void ProjectTester::scheduleFullday()
{
    Calendar c("Test");
    QTime t1(0,0,0);
    int length = 24*60*60*1000;

    for ( int i = 1; i <= 7; ++i ) {
        CalendarDay *wd1 = c.weekday(i);
        wd1->setState(CalendarDay::Working);
        wd1->addInterval(TimeInterval(t1, length));
    } 
    ResourceGroup *g = new ResourceGroup();
    g->setName( "G1" );
    m_project->addResourceGroup( g );
    Resource *r = new Resource();
    r->setName( "R1" );
    r->setCalendar( &c );
    m_project->addResource( g, r );

    Task *t = m_project->createTask( m_project );
    t->setName( "T1" );
    m_project->addTask( t, m_project );
    t->estimate()->setUnit( Duration::Unit_d );
    t->estimate()->setExpectedEstimate( 3 * 14.0 );
    t->estimate()->setType( Estimate::Type_Effort );
    
    ResourceGroupRequest *gr = new ResourceGroupRequest( g );
    gr->addResourceRequest( new ResourceRequest( r ) );
    t->addRequest( gr );

    ScheduleManager *sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );
    
    QCOMPARE( t->startTime(), m_project->startTime() );
    QCOMPARE( t->endTime(), DateTime(t->startTime().addDays( 14 )) );
    

}

void ProjectTester::scheduleWithExternalAppointments()
{
    Project project;
    project.setName( "P1" );
    DateTime targetstart = DateTime( QDate::currentDate(), QTime(0,0,0) );
    DateTime targetend = DateTime( targetstart.addDays( 3 ) );
    project.setConstraintStartTime( targetstart );
    project.setConstraintEndTime( targetend);

    Calendar c("Test");
    QTime t1(0,0,0);
    int length = 24*60*60*1000;

    for ( int i = 1; i <= 7; ++i ) {
        CalendarDay *wd1 = c.weekday(i);
        wd1->setState(CalendarDay::Working);
        wd1->addInterval(TimeInterval(t1, length));
    } 
    ResourceGroup *g = new ResourceGroup();
    g->setName( "G1" );
    project.addResourceGroup( g );
    Resource *r = new Resource();
    r->setName( "R1" );
    r->setCalendar( &c );
    project.addResource( g, r );

    r->addExternalAppointment( "Ext-1", "External project 1", targetstart, targetstart.addDays( 1 ), 100 );
    r->addExternalAppointment( "Ext-1", "External project 1", targetend.addDays( -1 ), targetend, 100 );

    Task *t = project.createTask( m_project );
    t->setName( "T1" );
    project.addTask( t, &project );
    t->estimate()->setUnit( Duration::Unit_h );
    t->estimate()->setExpectedEstimate( 8.0 );
    t->estimate()->setType( Estimate::Type_Effort );
    
    ResourceGroupRequest *gr = new ResourceGroupRequest( g );
    gr->addResourceRequest( new ResourceRequest( r ) );
    t->addRequest( gr );

    ScheduleManager *sm = project.createScheduleManager( "Test Plan" );
    project.addScheduleManager( sm );
    project.calculate( *sm );
    
    //printSchedulingLog( *sm );

    QCOMPARE( t->startTime(), targetstart + Duration( 1, 0, 0 ) );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );
    
    sm->setAllowOverbooking( true );
    project.calculate( *sm );

    //printSchedulingLog( *sm );

    QCOMPARE( t->startTime(), targetstart );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );

    sm->setAllowOverbooking( false );
    sm->setSchedulingDirection( true ); // backwards
    project.calculate( *sm );

    //printSchedulingLog( *sm );

    QCOMPARE( t->startTime(), targetend - Duration( 1, 8, 0 ) );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );

    sm->setAllowOverbooking( true );
    project.calculate( *sm );

    //printSchedulingLog( *sm );

    QCOMPARE( t->startTime(), targetend - Duration( 0, 8, 0 ) );
    QCOMPARE( t->endTime(), targetend  );

    sm->setAllowOverbooking( false );
    r->clearExternalAppointments();
    project.calculate( *sm );

    //printSchedulingLog( *sm );

    QCOMPARE( t->endTime(), targetend );
    QCOMPARE( t->startTime(),  t->endTime() - Duration( 0, 8, 0 )  );

}


} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::ProjectTester )

#include "ProjectTester.moc"
