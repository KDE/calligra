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
    
    QVERIFY( t->startTime() == m_project->startTime() );
    QVERIFY( t->endTime() == t->startTime().addDays( 1 ) );
    
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
    
    QVERIFY( t->earlyStart() == m_project->startTime() );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );
    
    QVERIFY( t->startTime() == DateTime( today, t1 ) );
    QVERIFY( t->endTime() == t->startTime() + Duration( 0, 8, 0 ) );

    kDebug()<<"Calculate forward, Task: ASAP, Resource 50% load ------------------";
    r->setUnits( 50 );
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );
    
    QVERIFY( t->earlyStart() == m_project->startTime() );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );

    QVERIFY( t->startTime() == DateTime( today, t1 ) );
    QVERIFY( t->endTime() == t->startTime() + Duration( 1, 8, 0 ) );

    r->setAvailableFrom( QDateTime( tomorrow, QTime() ) );
    r->setUnits( 100 );
    
    kDebug()<<"Calculate forward, Task: ASAP, Resource available tomorrow --------";
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QVERIFY( t->earlyStart() == m_project->startTime() );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );
    
    QVERIFY( t->startTime() == DateTime( r->availableFrom().date(), t1 ) );
    QVERIFY( t->endTime() == t->startTime() + Duration( 0, 8, 0 ) );
    
    kDebug()<<"Calculate forward, Task: MustStartOn -----------------------------------";
    r->setAvailableFrom( QDateTime( yesterday, QTime() ) );
    t->setConstraint( Node::MustStartOn );
    t->setConstraintStartTime( DateTime( nextweek, t1 ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QVERIFY( t->earlyStart() == m_project->startTime() );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );
    
    QVERIFY( t->startTime() == DateTime( t->constraintStartTime().date(), t1 ) );
    QVERIFY( t->endTime() == t->startTime() + Duration( 0, 8, 0 ) );
    QVERIFY( t->schedulingError() == false );
    
    // Calculate backwards
    kDebug()<<"Calculate backward, Task: MustStartOn -----------------------------------";
    m_project->setConstraint( Node::MustFinishOn );
    m_project->setConstraintEndTime( DateTime( nextweek.addDays( 1 ), QTime() ) );
    sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );

    QVERIFY( t->earlyStart() == m_project->startTime() );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );

    QVERIFY( t->startTime() == DateTime( t->constraintStartTime().date(), t1 ) );
    QVERIFY( t->endTime() == t->startTime() + Duration( 0, 8, 0 ) );
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

    QVERIFY( t->earlyStart() == m_project->startTime() );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );

    QVERIFY( t->endTime() == t->constraintEndTime() );
    QVERIFY( t->startTime() == t->endTime() - Duration( 0, 8, 0 ) );
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

    QVERIFY( t->earlyStart() == m_project->startTime() );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );

    QVERIFY( t->endTime() == t->constraintEndTime() );
    QVERIFY( t->startTime() == t->endTime() - Duration( 0, 8, 0 ) );
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

    QVERIFY( t->earlyStart() == m_project->startTime() );
    QVERIFY( t->lateStart() >=  t->constraintStartTime() );
    QVERIFY( t->earlyFinish() == t->endTime() );
    QVERIFY( t->lateFinish() == m_project->endTime() );

    QVERIFY( t->startTime() == DateTime( tomorrow, t1 ));
    QVERIFY( t->endTime() == t->startTime() + Duration( 0, 8, 0 )  );
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

    QVERIFY( t->earlyStart() == m_project->startTime() );
    QVERIFY( t->lateStart() >=  t->constraintStartTime() );
    QVERIFY( t->earlyFinish() == t->endTime() );
    QVERIFY( t->lateFinish() == m_project->endTime() );
    
    QVERIFY( t->endTime() == DateTime( nextweek.addDays( -1 ), t2 ));
    QVERIFY( t->startTime() == t->endTime() - Duration( 0, 8, 0 )  );
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

    QVERIFY( t->earlyStart() == m_project->startTime() );
    QVERIFY( t->lateStart() ==  t->startTime() );
    QVERIFY( t->earlyFinish() <= t->constraintEndTime() );
    QVERIFY( t->lateFinish() == m_project->endTime() );

    QVERIFY( t->startTime() == DateTime( today, t1 ));
    QVERIFY( t->endTime() == t->startTime() + Duration( 0, 8, 0 )  );
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

    QVERIFY( t->earlyStart() == m_project->startTime() );
    QVERIFY( t->lateStart() ==  t->startTime() );
    QVERIFY( t->earlyFinish() == t->constraintEndTime() );
    QVERIFY( t->lateFinish() == m_project->constraintEndTime() );

    QVERIFY( t->startTime() == DateTime( tomorrow, t1 ));
    QVERIFY( t->endTime() == t->startTime() + Duration( 0, 8, 0 )  );
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

    QVERIFY( t->earlyStart() == m_project->constraintStartTime() );
    QVERIFY( t->lateStart() == t->constraintStartTime() );
    QVERIFY( t->earlyFinish() == t->constraintEndTime() );
    QVERIFY( t->lateFinish() == t->constraintEndTime() );

    QVERIFY( t->startTime() == t->constraintStartTime() );
    QVERIFY( t->endTime() == t->constraintEndTime()  );
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

    QVERIFY( t->earlyStart() == m_project->constraintStartTime() );
    QVERIFY( t->lateStart() == t->constraintStartTime() );
    QVERIFY( t->earlyFinish() == t->constraintEndTime() );
    QVERIFY( t->lateFinish() == t->constraintEndTime() );

    QVERIFY( t->startTime() == t->constraintStartTime() );
    QVERIFY( t->endTime() == t->constraintEndTime() );
    QVERIFY( t->schedulingError() == false );

}


} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::ProjectTester )

#include "ProjectTester.moc"
