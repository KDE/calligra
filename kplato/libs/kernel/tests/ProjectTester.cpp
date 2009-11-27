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

#include "debug.cpp"

namespace KPlato
{

void ProjectTester::printDebug( Calendar *c, const QString &str, bool full ) const {
    qDebug()<<"Debug info: Calendar"<<c->name()<<str;
    for ( int wd = 1; wd <= 7; ++wd ) {
        CalendarDay *d = c->weekday( wd );
        qDebug()<<"   "<<wd<<":"<<d->stateToString( d->state() );
        foreach ( TimeInterval *t,  d->workingIntervals() ) {
            qDebug()<<"      interval:"<<t->first<<t->second;
        }
    }
    foreach ( const CalendarDay *d, c->days() ) {
        qDebug()<<"   "<<d->date()<<":";
        foreach ( TimeInterval *t,  d->workingIntervals() ) {
            qDebug()<<"      interval:"<<t->first<<t->second;
        }
    }
}

void ProjectTester::printDebug( Resource *r, const QString &str, bool full ) const {
    qDebug()<<"Debug info: Resource"<<r->name()<<str;
    qDebug()<<"Group:"<<r->parentGroup()->name()<<"Type:"<<r->parentGroup()->typeToString();
    qDebug()<<"Available:"<<r->availableFrom().toString()<<r->availableUntil().toString()<<r->units()<<"%";
    qDebug()<<"Type:"<<r->typeToString();
    qDebug()<<"External appointments:"<<r->numExternalAppointments();
    foreach ( Appointment *a, r->externalAppointmentList() ) {
        qDebug()<<"   appointment:"<<a->startTime().toString()<<a->endTime().toString();
    }
}

void ProjectTester::printDebug( Project *p, Task *t, const QString &str, bool full ) const {
    qDebug()<<"Debug info: Project"<<p->name()<<str;
    qDebug()<<"project target start:"<<p->constraintStartTime().toString();
    qDebug()<<"  project target end:"<<p->constraintEndTime().toString();
    qDebug()<<"  project start time:"<<p->startTime().toString();
    qDebug()<<"    project end time:"<<p->endTime().toString();
    qDebug()<<"Task"<<t->name()<<t->typeToString()<<t->constraintToString();
    qDebug()<<"     earlyStart:"<<t->earlyStart().toString();
    qDebug()<<"      lateStart:"<<t->lateStart().toString();
    qDebug()<<"    earlyFinish:"<<t->earlyFinish().toString();
    qDebug()<<"     lateFinish:"<<t->lateFinish().toString();
    qDebug()<<"      startTime:"<<t->startTime().toString();
    qDebug()<<"        endTime:"<<t->endTime().toString();
    switch ( t->constraint() ) {
        case Node::MustStartOn:
        case Node::StartNotEarlier:
            qDebug()<<"startConstraint:"<<t->constraintStartTime().toString();
            break;
        case Node::FixedInterval:
            qDebug()<<"startConstraint:"<<t->constraintStartTime().toString();
        case Node::MustFinishOn:
        case Node::FinishNotLater:
            qDebug()<<"  endConstraint:"<<t->constraintEndTime().toString();
            break;
        default: break;
    }
    foreach ( ResourceGroupRequest *gr, t->requests().requests() ) {
        qDebug()<<"Group request:"<<gr->group()->name()<<gr->units();
        foreach ( ResourceRequest *rr, gr->resourceRequests() ) {
            qDebug()<<"  Resource request:"<<rr->resource()->name()<<rr->units()<<"%"<<rr->resource()->availableFrom().toString()<<rr->resource()->availableUntil().toString();
        }
    }
    Schedule *s = t->currentSchedule();
    if ( s == 0 ) {
        return;
    }
    qDebug();
    qDebug()<<"Estimate   :"<<t->estimate()->expectedEstimate()<<Duration::unitToString(t->estimate()->unit());
    foreach ( Appointment *a, s->appointments() ) {
        qDebug()<<"Resource:"<<a->resource()->resource()->name()<<a->startTime().toString()<<a->endTime().toString();
        if ( ! full ) { continue; }
        foreach( const AppointmentInterval &i, a->intervals() ) {
            qDebug()<<"  "<<i.startTime().toString()<<i.endTime().toString()<<i.load();
        }
    }
}

void ProjectTester::printSchedulingLog( const ScheduleManager &sm, const QString &s ) const
{
    qDebug()<<"Scheduling log"<<s;
    qDebug()<<"Scheduling:"<<sm.name()<<(sm.recalculate()?QString("recalculate from %1").arg(sm.recalculateFrom().toString()):"");
    foreach ( const QString &s, sm.expected()->logMessages() ) {
        qDebug()<<s;
    }
}

void ProjectTester::initTestCase()
{
    m_project = new Project();
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
    
    t->estimate()->setCalendar( m_calendar );
    m_project->calculate( *sm );

    QCOMPARE( t->startTime(), m_calendar->firstAvailableAfter( m_project->startTime(), m_project->endTime() ) );
    QCOMPARE( t->endTime(), DateTime( t->startTime().addMSecs( length ) ) );

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
    
    QString s = "Calculate forward, Task: ASAP -----------------------------------";
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );
    
    QCOMPARE( t->earlyStart(), t->requests().workTimeAfter( m_project->startTime() ) );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );
    
    QCOMPARE( t->startTime(), DateTime( today, t1 ) );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );
    QVERIFY( t->schedulingError() == false );

    s = "Calculate forward, Task: ASAP, Resource 50% available -----------------";
    r->setUnits( 50 );
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );
    
    QCOMPARE( t->earlyStart(), t->requests().workTimeAfter( m_project->startTime() ) );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );

    QCOMPARE( t->startTime(), DateTime( today, t1 ) );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 1, 8, 0 ) );
    QVERIFY( t->schedulingError() == false );
    
    s = "Calculate forward, Task: ASAP, Resource 50% available, Request 50% load ---------";
    r->setUnits( 50 );
    rr->setUnits( 50 );
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );
    
//    printDebug( m_project, t, s );
    
    QCOMPARE( t->earlyStart(), t->requests().workTimeAfter( m_project->startTime() ) );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );

    QCOMPARE( t->startTime(), DateTime( today, t1 ) );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 3, 8, 0 ) );
    QVERIFY( t->schedulingError() == false );
    
    s = "Calculate forward, Task: ASAP, Resource available tomorrow --------";
    r->setAvailableFrom( QDateTime( tomorrow, QTime() ) );
    r->setUnits( 100 );
    rr->setUnits( 100 );
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

//    printDebug( m_project, t, s);
//    printSchedulingLog( *sm, s );
    
    QCOMPARE( t->earlyStart(), t->requests().workTimeAfter( m_project->startTime() ) );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );
    
    QCOMPARE( t->startTime(), DateTime( r->availableFrom().date(), t1 ) );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );
    QVERIFY( t->schedulingError() == false );
    
    s = "Calculate forward, Task: ALAP -----------------------------------";
    m_project->setConstraintStartTime( DateTime( today, QTime(0,0,0) ) );
    t->setConstraint( Node::ALAP );
    r->setAvailableFrom( QDateTime( yesterday, QTime() ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );
    
    QCOMPARE( t->earlyStart(), t->requests().workTimeAfter( m_project->startTime() ) );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );
    
    QCOMPARE( t->startTime(), DateTime( today, t1 ) );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );
    QVERIFY( t->schedulingError() == false );

    s = "Calculate forward, Task: MustStartOn -----------------------------------";
    r->setAvailableFrom( QDateTime( yesterday, QTime() ) );
    t->setConstraint( Node::MustStartOn );
    t->setConstraintStartTime( DateTime( nextweek, t1 ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QCOMPARE( t->earlyStart(), t->requests().workTimeAfter( m_project->startTime() ) );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );
    
    QCOMPARE( t->startTime(), DateTime( t->constraintStartTime().date(), t1 ) );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );
    QVERIFY( t->schedulingError() == false );
    
    // Calculate backwards
    s = "Calculate backward, Task: MustStartOn -----------------------------------";
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
    s = "Calculate backwards, Task: MustFinishOn -----------------------------------";
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
    s = "Calculate forwards, Task: MustFinishOn -----------------------------------";
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
    s = "Calculate forwards, Task: StartNotEarlier -----------------------------------";
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::StartNotEarlier );
    t->setConstraintStartTime( DateTime( today, t2 ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( false );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

//    Debug::print( m_project, t, s );
    QCOMPARE( t->earlyStart(), t->requests().workTimeAfter( m_project->startTime() ) );
    QVERIFY( t->lateStart() >=  t->constraintStartTime() );
    QCOMPARE( t->earlyFinish(),t->earlyStart() + Duration( 0, 8, 0 ) );
    QCOMPARE( t->lateFinish(), t->requests().workTimeBefore( m_project->endTime() ) );

    QCOMPARE( t->startTime(), DateTime( tomorrow, t1 ));
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 )  );
    QVERIFY( t->schedulingError() == false );

    // Calculate backward
    s = "Calculate backwards, Task: StartNotEarlier -----------------------------------";
    m_project->setConstraintEndTime( DateTime( nextweek, QTime() ) );
    t->setConstraint( Node::StartNotEarlier );
    t->setConstraintStartTime( DateTime( today, t2 ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( true );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

//    printDebug( m_project, t, s );

    QVERIFY( t->lateStart() >=  t->constraintStartTime() );
    QCOMPARE( t->earlyFinish(), t->endTime() );
    QVERIFY( t->lateFinish() <= m_project->constraintEndTime() );
    
    QVERIFY( t->endTime() <= t->lateFinish() );
    QCOMPARE( t->startTime(), t->endTime() - Duration( 0, 8, 0 )  );
    QVERIFY( t->schedulingError() == true );

    // Calculate forward
    s = "Calculate forwards, Task: FinishNotLater -----------------------------------";
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
    s = "Calculate backwards, Task: FinishNotLater -----------------------------------";
    m_project->setConstraintStartTime( DateTime( nextweek, QTime() ) );
    t->setConstraint( Node::FinishNotLater );
    t->setConstraintEndTime( DateTime( tomorrow, t2 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( true );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    //printDebug( m_project, t, s );

    QCOMPARE( t->earlyStart(), m_project->startTime() );
    QCOMPARE( t->lateStart(),  t->startTime() );
    QCOMPARE( t->earlyFinish(), t->constraintEndTime() );
    QVERIFY( t->lateFinish() <= m_project->constraintEndTime() );

    QCOMPARE( t->startTime(), m_project->startTime() );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 )  );
    QVERIFY( t->schedulingError() == false );

    // Calculate forward
    s = "Calculate forward, Task: FixedInterval -----------------------------------";
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::FixedInterval );
    t->setConstraintStartTime( DateTime( tomorrow, t1 ) );
    t->setConstraintEndTime( DateTime( tomorrow, t2 ) );
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( false );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    //printDebug( m_project, t, s );

    QCOMPARE( t->earlyStart(), m_project->constraintStartTime() );
    QCOMPARE( t->lateStart(), t->constraintStartTime() );
    QCOMPARE( t->earlyFinish(), t->constraintEndTime() );
    QCOMPARE( t->lateFinish(), t->constraintEndTime() );

    QCOMPARE( t->startTime(), t->constraintStartTime() );
    QCOMPARE( t->endTime(), t->constraintEndTime()  );
    QVERIFY( t->schedulingError() == false );

    // Calculate forward
    s = "Calculate forwards, Task: FixedInterval -----------------------------------";
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
    s = "Calculate forwards, Task: Milestone, ASAP-------------------------";
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    t->setConstraint( Node::ASAP );
    t->estimate()->clear();
    
    sm = m_project->createScheduleManager( "Test Plan" );
    sm->setSchedulingDirection( false );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    //printDebug( m_project, t, s );

    QCOMPARE( t->earlyStart(), m_project->constraintStartTime() );
    QCOMPARE( t->lateStart(), t->earlyStart() );
    QCOMPARE( t->earlyFinish(), t->earlyStart() );
    QCOMPARE( t->lateFinish(), t->earlyFinish() );

    QCOMPARE( t->startTime(), t->earlyStart() );
    QCOMPARE( t->endTime(), t->startTime() );
    QVERIFY( t->schedulingError() == false );

    // Calculate backward
    s = "Calculate backwards, Task: Milestone, ASAP-------------------------";
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
    s = "Calculate forwards, Task: Milestone, ALAP-------------------------";
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
    s = "Calculate backwards, Task: Milestone, ALAP-------------------------";
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
    s = "Calculate forwards, Task: Milestone, MustStartOn ------------------";
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
    s = "Calculate backwards, Task: Milestone, MustStartOn ------------------";
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
    s = "Calculate forwards, Task: Milestone, MustFinishOn ------------------";
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
    s = "Calculate backwards, Task: Milestone, MustFinishOn ------------------";
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
    s = "Calculate forwards, Task: Milestone, StartNotEarlier ---------------";
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
    s = "Calculate backwards, Task: Milestone, StartNotEarlier ---------------";
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
    s = "Calculate forwards, Task: Milestone, FinishNotLater ---------------";
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
    s = "Calculate backwards, Task: Milestone, FinishNotLater ---------------";
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
    s = "Calculate forward, 2 Tasks, no overbooking ----------";
    m_project->setConstraintStartTime( DateTime( today, QTime() ) );
    m_project->setConstraintEndTime( DateTime( today, QTime() ).addDays( 4 ) );
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

//     Debug::print( m_project, t, s );
//     Debug::print( m_project, tsk2, s );
//     Debug::printSchedulingLog( *sm, s );

    QCOMPARE( t->earlyStart(), t->requests().workTimeAfter( m_project->constraintStartTime() ) );
    QCOMPARE( t->lateStart(), tsk2->startTime() );
    QCOMPARE( t->earlyFinish(), DateTime( tomorrow, t2 ) );
    QCOMPARE( t->lateFinish(), t->lateFinish() );

    QCOMPARE( t->startTime(), DateTime( today, t1 ) );
    QCOMPARE( t->endTime(), t->earlyFinish() );
    QVERIFY( t->schedulingError() == false );

    QCOMPARE( tsk2->earlyStart(), t->earlyStart() );
    QCOMPARE( tsk2->lateStart(), t->earlyFinish() + Duration( 0, 16, 0 ) );
    QCOMPARE( tsk2->earlyFinish(), DateTime( tomorrow, t2 ) );
    QCOMPARE( tsk2->lateFinish(), t->lateFinish() );

    QCOMPARE( tsk2->startTime(), DateTime( tomorrow.addDays( 1 ), t1 ) );
    QCOMPARE( tsk2->endTime(), tsk2->lateFinish() );
    QVERIFY( tsk2->schedulingError() == false );
    
    QCOMPARE( m_project->endTime(), tsk2->endTime() );

    // Calculate forward
    s = "Calculate forward, 2 Tasks, relation ---------------";
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

//    printDebug( m_project, t, s );
//    printDebug( m_project, tsk2, s );
//    printSchedulingLog( *sm, s );

    QCOMPARE( t->earlyStart(), t->requests().workTimeAfter( m_project->constraintStartTime() ) );
    QCOMPARE( t->lateStart(), DateTime( today, t1 ) );
    QCOMPARE( t->earlyFinish(), DateTime( tomorrow, t2 ) );
    QCOMPARE( t->lateFinish(), t->lateFinish() );

    QCOMPARE( t->startTime(), DateTime( today, t1 ) );
    QCOMPARE( t->endTime(), t->earlyFinish() );
    QVERIFY( t->schedulingError() == false );

    QCOMPARE( tsk2->earlyStart(), tsk2->requests().workTimeAfter( t->earlyFinish() ) );
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
    QString s = "Full day, 1 resource works 24 hours a day -------------";
    Calendar *c = new Calendar("Test");
    QTime t1(0,0,0);
    int length = 24*60*60*1000;

    for ( int i = 1; i <= 7; ++i ) {
        CalendarDay *wd1 = c->weekday(i);
        wd1->setState(CalendarDay::Working);
        wd1->addInterval(TimeInterval(t1, length));
    }
    m_project->addCalendar( c );

    ResourceGroup *g = new ResourceGroup();
    g->setName( "G1" );
    m_project->addResourceGroup( g );
    Resource *r = new Resource();
    r->setName( "R1" );
    r->setCalendar( c );
    r->setAvailableUntil( r->availableFrom().addDays( 21 ) );
    m_project->addResource( g, r );

    Task *t = m_project->createTask( m_project );
    t->setName( "T1" );
    m_project->addTask( t, m_project );
    t->estimate()->setUnit( Duration::Unit_d );
    t->estimate()->setExpectedEstimate( 3 * 14.0 );
    t->estimate()->setType( Estimate::Type_Effort );
    
    ResourceGroupRequest *gr = new ResourceGroupRequest( g );
    gr->addResourceRequest( new ResourceRequest( r, 100 ) );
    t->addRequest( gr );

    ScheduleManager *sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );
    
//     printDebug( c, s );
//     printDebug( m_project, t, s );
//     printDebug( r, s, true );
//     printSchedulingLog( *sm, s );

    QCOMPARE( t->startTime(), m_project->startTime() );
    QCOMPARE( t->endTime(), DateTime(t->startTime().addDays( 14 )) );
    
    s = "Full day, 8 hour shifts, 3 resources ---------------";
    int hour = 60*60*1000;
    Calendar *c1 = new Calendar("Test 1");
    for ( int i = 1; i <= 7; ++i ) {
        CalendarDay *wd1 = c1->weekday(i);
        wd1->setState(CalendarDay::Working);
        wd1->addInterval(TimeInterval(QTime( 6, 0, 0 ), 8*hour));
    }
    m_project->addCalendar( c1 );
    Calendar *c2 = new Calendar("Test 2");
    for ( int i = 1; i <= 7; ++i ) {
        CalendarDay *wd1 = c2->weekday(i);
        wd1->setState(CalendarDay::Working);
        wd1->addInterval(TimeInterval(QTime( 14, 0, 0 ), 8*hour));
    }
    m_project->addCalendar( c2 );
    Calendar *c3 = new Calendar("Test 3");
    for ( int i = 1; i <= 7; ++i ) {
        CalendarDay *wd1 = c3->weekday(i);
        wd1->setState(CalendarDay::Working);
        wd1->addInterval(TimeInterval(QTime( 0, 0, 0 ), 6*hour));
        wd1->addInterval(TimeInterval(QTime( 22, 0, 0 ), 2*hour));
    }
    m_project->addCalendar( c3 );
    
    r->setCalendar( c1 );

    r = new Resource();
    r->setName( "R2" );
    r->setCalendar( c2 );
    gr->addResourceRequest( new ResourceRequest( r, 100 ) );

    r = new Resource();
    r->setName( "R3" );
    r->setCalendar( c3 );
    gr->addResourceRequest( new ResourceRequest( r, 100 ) );

    m_project->calculate( *sm );

//    printDebug( m_project, t, s );
//    printSchedulingLog( *sm, s );

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

    Task *t = project.createTask( &project );
    t->setName( "T1" );
    project.addTask( t, &project );
    t->estimate()->setUnit( Duration::Unit_h );
    t->estimate()->setExpectedEstimate( 8.0 );
    t->estimate()->setType( Estimate::Type_Effort );
    
    ResourceGroupRequest *gr = new ResourceGroupRequest( g );
    gr->addResourceRequest( new ResourceRequest( r, 100 ) );
    t->addRequest( gr );

    ScheduleManager *sm = project.createScheduleManager( "Test Plan" );
    project.addScheduleManager( sm );
    project.calculate( *sm );
    
    QString s = "Schedule with external appointments ----------";
//     printDebug( r, s );
//     printDebug( &project, t, s );
//     printSchedulingLog( *sm );

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

void ProjectTester::reschedule()
{
    Project project;
    project.setName( "P1" );
    DateTime targetstart = DateTime( QDate::currentDate(), QTime(0,0,0) );
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
    ResourceGroup *g = new ResourceGroup();
    g->setName( "G1" );
    project.addResourceGroup( g );
    Resource *r = new Resource();
    r->setName( "R1" );
    r->setCalendar( c );
    project.addResource( g, r );

    QString s = "Re-schedule; schedule tasks T1, T2, T3 ---------------";

    Task *task1 = project.createTask( &project );
    task1->setName( "T1" );
    project.addTask( task1, &project );
    task1->estimate()->setUnit( Duration::Unit_h );
    task1->estimate()->setExpectedEstimate( 8.0 );
    task1->estimate()->setType( Estimate::Type_Effort );
    
    ResourceGroupRequest *gr = new ResourceGroupRequest( g );
    gr->addResourceRequest( new ResourceRequest( r, 100 ) );
    task1->addRequest( gr );

    Task *task2 = project.createTask( &project );
    task2->setName( "T2" );
    project.addTask( task2, &project );
    task2->estimate()->setUnit( Duration::Unit_h );
    task2->estimate()->setExpectedEstimate( 8.0 );
    task2->estimate()->setType( Estimate::Type_Effort );
    
    gr = new ResourceGroupRequest( g );
    gr->addResourceRequest( new ResourceRequest( r, 100 ) );
    task2->addRequest( gr );

    Task *task3 = project.createTask( &project );
    task3->setName( "T3" );
    project.addTask( task3, &project );
    task3->estimate()->setUnit( Duration::Unit_h );
    task3->estimate()->setExpectedEstimate( 8.0 );
    task3->estimate()->setType( Estimate::Type_Effort );
    
    gr = new ResourceGroupRequest( g );
    gr->addResourceRequest( new ResourceRequest( r, 100 ) );
    task3->addRequest( gr );

    Relation *rel = new Relation( task1, task2 );
    project.addRelation( rel );
    rel = new Relation( task1, task3 );
    project.addRelation( rel );

    ScheduleManager *sm = project.createScheduleManager( "Plan" );
    project.addScheduleManager( sm );
    project.calculate( *sm );
    
//    printDebug( &project, task1, s, true );
//    printDebug( &project, task2, s, true );
//    printDebug( &project, task3, s, true );
//    printSchedulingLog( *sm );

    QCOMPARE( task1->startTime(), c->firstAvailableAfter( targetstart, targetend ) );
    QCOMPARE( task1->endTime(), task1->startTime() + Duration( 0, 8, 0 ) );
    
    QCOMPARE( task2->startTime(), c->firstAvailableAfter( task1->endTime(), targetend ) );
    QCOMPARE( task2->endTime(), task2->startTime() + Duration( 0, 8, 0 ) );

    QCOMPARE( task3->startTime(), c->firstAvailableAfter( task2->endTime(), targetend ) );
    QCOMPARE( task3->endTime(), task3->startTime() + Duration( 0, 8, 0 ) );


    DateTime restart = task1->endTime();
    s = QString( "Re-schedule; re-schedule from %1 - tasks T1 (finished), T2, T3 ------" ).arg( restart.toString() );

    task1->completion().setStarted( true );
    task1->completion().setPercentFinished( task1->endTime().date(), 100 );
    task1->completion().setFinished( true );

    ScheduleManager *child = project.createScheduleManager( "Plan.1" );
    project.addScheduleManager( child, sm );
    child->setRecalculate( true );
    child->setRecalculateFrom( restart );
    project.calculate( *child );

//     printDebug( &project, task1, s, true );
//     printDebug( &project, task2, s, true );
//     printDebug( &project, task3, s, true );
//     printSchedulingLog( *child, s );

    QCOMPARE( task1->startTime(), c->firstAvailableAfter( targetstart, targetend ) );
    QCOMPARE( task1->endTime(), task1->startTime() + Duration( 0, 8, 0 ) );
    
    QCOMPARE( task2->startTime(), c->firstAvailableAfter( qMax(task1->endTime(), restart ), targetend ) );
    QCOMPARE( task2->endTime(), task2->startTime() + Duration( 0, 8, 0 ) );

    QCOMPARE( task3->startTime(), c->firstAvailableAfter( task2->endTime(), targetend ) );
    QCOMPARE( task3->endTime(), task3->startTime() + Duration( 0, 8, 0 ) );
}

void ProjectTester::materialResource()
{
    Project project;
    project.setName( "P1" );
    DateTime targetstart = DateTime( QDate::currentDate(), QTime(0,0,0) );
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
    task1->estimate()->setUnit( Duration::Unit_h );
    task1->estimate()->setExpectedEstimate( 8.0 );
    task1->estimate()->setType( Estimate::Type_Effort );

    QString s = "Calculate forward, Task: ASAP, Working + material resource --------";
    qDebug()<<s;
    ResourceGroup *g = new ResourceGroup();
    project.addResourceGroup( g );
    Resource *r = new Resource();
    r->setName( "Work" );
    r->setAvailableFrom( targetstart );
    r->setCalendar( c );
    project.addResource( g, r );

    ResourceGroup *mg = new ResourceGroup();
    mg->setType( ResourceGroup::Type_Material );
    project.addResourceGroup( mg );
    Resource *mr = new Resource();
    mr->setType( Resource::Type_Material );
    mr->setName( "Material" );
    mr->setAvailableFrom( targetstart );
    mr->setCalendar( c );
    project.addResource( mg, mr );

    ResourceGroupRequest *gr = new ResourceGroupRequest( g );
    task1->addRequest( gr );
    ResourceRequest *rr = new ResourceRequest( r, 100 );
    gr->addResourceRequest( rr );

    ResourceGroupRequest *mgr = new ResourceGroupRequest( mg );
    task1->addRequest( mgr );
    ResourceRequest *mrr = new ResourceRequest( mr, 100 );
    mgr->addResourceRequest( mrr );

    ScheduleManager *sm = project.createScheduleManager( "Test Plan" );
    project.addScheduleManager( sm );
    project.calculate( *sm );

//     printDebug( r, s);
//     printDebug( mr, s);
//     printDebug( &project, task1, s);
//     printSchedulingLog( *sm, s );

    QCOMPARE( task1->earlyStart(), task1->requests().workTimeAfter( targetstart ) );
    QVERIFY( task1->lateStart() >=  task1->earlyStart() );
    QVERIFY( task1->earlyFinish() <= task1->endTime() );
    QVERIFY( task1->lateFinish() >= task1->endTime() );
    
    QCOMPARE( task1->startTime(), DateTime( r->availableFrom().date(), t1 ) );
    QCOMPARE( task1->endTime(), task1->startTime() + Duration( 0, 8, 0 ) );
    QVERIFY( task1->schedulingError() == false );
}

void ProjectTester::requiredResource()
{
    Project project;
    project.setName( "P1" );
    DateTime targetstart = DateTime( QDate::currentDate(), QTime(0,0,0) );
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
    task1->estimate()->setUnit( Duration::Unit_h );
    task1->estimate()->setExpectedEstimate( 8.0 );
    task1->estimate()->setType( Estimate::Type_Effort );

    QString s = "Required resource: Working + required material resource --------";
    ResourceGroup *g = new ResourceGroup();
    project.addResourceGroup( g );
    Resource *r = new Resource();
    r->setName( "Work" );
    r->setAvailableFrom( targetstart );
    r->setCalendar( c );
    project.addResource( g, r );

    ResourceGroup *mg = new ResourceGroup();
    mg->setType( ResourceGroup::Type_Material );
    project.addResourceGroup( mg );
    Resource *mr = new Resource();
    mr->setType( Resource::Type_Material );
    mr->setName( "Material" );
    mr->setAvailableFrom( targetstart );
    mr->setCalendar( c );
    project.addResource( mg, mr );

    ResourceGroupRequest *gr = new ResourceGroupRequest( g );
    task1->addRequest( gr );
    ResourceRequest *rr = new ResourceRequest( r, 100 );
    gr->addResourceRequest( rr );

    QList<Resource*> lst; lst << mr;
    rr->setRequiredResources( lst );
    
    ScheduleManager *sm = project.createScheduleManager( "Test Plan" );
    project.addScheduleManager( sm );
    project.calculate( *sm );

//     Debug::print( r, s);
//     Debug::print( mr, s);
//     Debug::print( &project, task1, s);
//     Debug::printSchedulingLog( *sm, s );

    QCOMPARE( task1->earlyStart(), task1->requests().workTimeAfter( targetstart ) );
    QVERIFY( task1->lateStart() >=  task1->earlyStart() );
    QVERIFY( task1->earlyFinish() <= task1->endTime() );
    QVERIFY( task1->lateFinish() >= task1->endTime() );
    
    QCOMPARE( task1->startTime(), DateTime( r->availableFrom().date(), t1 ) );
    QCOMPARE( task1->endTime(), task1->startTime() + Duration( 0, 8, 0 ) );
    QVERIFY( task1->schedulingError() == false );
    
    QList<Appointment*> apps = r->appointments( sm->id() );
    QVERIFY( apps.count() == 1 );
    QCOMPARE( task1->startTime(), apps.first()->startTime() );
    QCOMPARE( task1->endTime(), apps.last()->endTime() );

    apps = mr->appointments( sm->id() );
    QVERIFY( apps.count() == 1 );
    QCOMPARE( task1->startTime(), apps.first()->startTime() );
    QCOMPARE( task1->endTime(), apps.last()->endTime() );
    
    s = "Required resource limits availability --------";
    DateTime tomorrow = targetstart.addDays( 1 );
    mr->setAvailableFrom( tomorrow );
    project.calculate( *sm );

//     Debug::print( r, s);
//     Debug::print( mr, s);
//     Debug::print( &project, task1, s);
//     Debug::printSchedulingLog( *sm, s );

    QCOMPARE( task1->earlyStart(), task1->requests().workTimeAfter( targetstart ) );
    QVERIFY( task1->lateStart() >=  task1->earlyStart() );
    QVERIFY( task1->earlyFinish() <= task1->endTime() );
    QVERIFY( task1->lateFinish() >= task1->endTime() );
    
    QCOMPARE( task1->startTime(), DateTime( mr->availableFrom().date(), t1 ) );
    QCOMPARE( task1->endTime(), task1->startTime() + Duration( 0, 8, 0 ) );
    QVERIFY( task1->schedulingError() == false );
    
    apps = r->appointments( sm->id() );
    QVERIFY( apps.count() == 1 );
    QCOMPARE( task1->startTime(), apps.first()->startTime() );
    QCOMPARE( task1->endTime(), apps.last()->endTime() );

    apps = mr->appointments( sm->id() );
    QVERIFY( apps.count() == 1 );
    QCOMPARE( task1->startTime(), apps.first()->startTime() );
    QCOMPARE( task1->endTime(), apps.last()->endTime() );
}

} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::ProjectTester )

#include "ProjectTester.moc"
