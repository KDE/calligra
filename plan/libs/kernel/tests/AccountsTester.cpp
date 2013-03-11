/* This file is part of the KDE project
   Copyright (C) 2008 Dag Andersen <danders@get2net.dk>

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
#include <qtest_kde.h>

#include "AccountsTester.h"
#include "kptaccount.h"
#include "kptduration.h"
#include "kptnode.h"
#include "kpttask.h"

#include "debug.cpp"

namespace KPlato
{

void AccountsTester::init()
{
    project = new Project();
    project->setId( project->uniqueCalendarId() );
    project->registerNodeId( project );
    
    today = QDate::currentDate();
    tomorrow = today.addDays( 1 );
    yesterday = today.addDays( -1 );
    nextweek = today.addDays( 7 );
    t1 = QTime( 9, 0, 0 );
    t2 = QTime( 17, 0, 0 );
    length = t1.msecsTo( t2 );

    t = project->createTask();
    t->setName( "T1" );
    project->addTask( t, project );
    t->estimate()->setUnit( Duration::Unit_d );
    t->estimate()->setExpectedEstimate( 1.0 );
    
    sm = project->createScheduleManager( "Test Plan" );
    project->addScheduleManager( sm );

    // standard worktime defines 8 hour day as default
    Calendar *c = new Calendar();
    c->setDefault( true );
    for ( int i=1; i <= 7; ++i ) {
        CalendarDay *d = c->weekday( i );
        d->setState( CalendarDay::Working );
        d->addInterval( t1, length );
    }
    project->addCalendar( c );
    
    ResourceGroup *g = new ResourceGroup();
    project->addResourceGroup( g );
    r = new Resource();
    r->setAvailableFrom( QDateTime( yesterday, QTime() ) );
    r->setCalendar( c );
    r->setNormalRate( 100.0 );
    project->addResource( g, r );

    ResourceGroupRequest *gr = new ResourceGroupRequest( g );
    t->addRequest( gr );
    ResourceRequest *rr = new ResourceRequest( r, 100 );
    gr->addResourceRequest( rr );
    t->estimate()->setType( Estimate::Type_Effort );
    
    //qDebug()<<"Calculate forward, Task: ASAP -----------------------------------";
    project->setConstraintStartTime( DateTime( today, QTime() ) );
    project->setConstraintEndTime( DateTime( tomorrow, QTime() ) );
    sm->createSchedules();
    project->calculate( *sm );
    
    QCOMPARE( t->earlyStart(), t->requests().workTimeAfter( project->startTime() ) );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );
    
    QCOMPARE( t->startTime(), DateTime( today, t1 ) );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );
//###    QVERIFY( t->schedulingError() == false );

    sm->setBaselined( true );
    

}
void AccountsTester::cleanup()
{
    delete project;
    project = 0;
    t = 0;
    r = 0;
    sm = 0;
    topaccount = 0;
}

void AccountsTester::defaultAccount() {
    Account *a = new Account( "Default Account" );
    project->accounts().insert( a );
    project->accounts().setDefaultAccount( a );
    
    EffortCostMap ec = project->accounts().plannedCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 8.0 );
    QCOMPARE( ec.totalCost(), 800.0 );

    ec = project->accounts().actualCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );

    t->completion().setEntrymode( Completion::FollowPlan );
    ec = project->accounts().actualCost( *a, t->startTime().date(), t->endTime().date() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 8.0 );
    QCOMPARE( ec.totalCost(), 800.0 );

    t->completion().setEntrymode( Completion::EnterCompleted );
    ec = project->accounts().actualCost( *a, t->startTime().date(), t->endTime().date() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );
    
    t->completion().setEntrymode( Completion::EnterCompleted );
    t->completion().setStarted( true );
    t->completion().setStartTime( DateTime( tomorrow, QTime() ) );
    t->completion().setPercentFinished( tomorrow, 50 );
    ec = project->accounts().actualCost( *a, t->startTime().date(), tomorrow );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );

    t->completion().setEntrymode( Completion::EnterEffortPerTask );
    t->completion().setActualEffort( tomorrow, Duration( 0, 4, 0 ) );
    ec = project->accounts().actualCost( *a, t->startTime().date(), tomorrow );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 4.0 );
    QCOMPARE( ec.totalCost(), 400.0 );

    t->completion().setEntrymode( Completion::EnterEffortPerResource );
    Completion::UsedEffort *ue = new Completion::UsedEffort();
    Completion::UsedEffort::ActualEffort e(  Duration( 0, 6, 0 ) ) ;
    ue->setEffort( tomorrow, e );
    t->completion().addUsedEffort( r, ue );
    ec = project->accounts().actualCost( *a, t->startTime().date(), tomorrow );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 6.0 );
    QCOMPARE( ec.totalCost(), 600.0 );
    
}

void AccountsTester::costPlaces() {
    EffortCostMap ec;
    Account *top = new Account( "Top account" );
    project->accounts().insert( top );

    Account *a = new Account( "Running account" );
    project->accounts().insert( a, top );
    a->addRunning( *t );
    ec = a->plannedCost( sm->scheduleId() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 8.0 );
    QCOMPARE( ec.totalCost(), 800.0 );

    a = new Account( "Startup account" );
    project->accounts().insert( a, top );
    a->addStartup( *t );
    t->setStartupCost( 200.0 );
    ec = a->plannedCost( sm->scheduleId() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 200.0 );

    a = new Account( "Shutdown cost" );
    project->accounts().insert( a, top );
    a->addShutdown( *t );
    t->setShutdownCost( 300.0 );
    ec = a->plannedCost( sm->scheduleId() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 300.0 );

    ec = top->plannedCost( sm->scheduleId() );

//    Debug::print( top, sm->scheduleId(), "All planned cost in child accounts--------" );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 8.0 );
    QCOMPARE( ec.totalCost(), 1300.0 );
    
    a = new Account( "All cost in one account" );
    project->accounts().insert( a );
    a->addRunning( *t );
    a->addStartup( *t );
    a->addShutdown( *t );
    ec = a->plannedCost( sm->scheduleId() );
//    Debug::print( a, sm->scheduleId(), "All planned cost in one account-----------" );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 8.0 );
    QCOMPARE( ec.totalCost(), 1300.0 );

}

void AccountsTester::startupDefault() {
    Account *a = new Account( "Default Account" );
    project->accounts().insert( a );
    project->accounts().setDefaultAccount( a );
    
    EffortCostMap ec = project->accounts().plannedCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 8.0 );
    QCOMPARE( ec.totalCost(), 800.0 );

    ec = project->accounts().actualCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );

    t->setStartupCost( 25.0 );

    ec = project->accounts().plannedCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 8.0 );
    QCOMPARE( ec.totalCost(), 825.0 );

    qDebug()<< t->completion().entryModeToString();
    ec = project->accounts().actualCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QVERIFY( ! t->completion().isStarted() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );
    
    t->completion().setEntrymode( Completion::EnterCompleted );
    t->completion().setStarted( true );
    t->completion().setStartTime( DateTime( tomorrow, QTime() ) );
    t->completion().setPercentFinished( tomorrow, 50 );
    QVERIFY( t->completion().isStarted() );
    qDebug()<<t->completion().startTime()<<":"<<t->startTime().date()<<tomorrow;
    ec = project->accounts().actualCost( *a, t->startTime().date(), tomorrow );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 25.0 );

    ec = project->accounts().actualCost( *a, QDate(), QDate() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 25.0 );

    t->completion().setEntrymode( Completion::EnterEffortPerTask );
    t->completion().setActualEffort( tomorrow, Duration( 0, 4, 0 ) );
    ec = project->accounts().actualCost( *a, t->startTime().date(), tomorrow );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 4.0 );
    QCOMPARE( ec.totalCost(), 425.0 );

    ec = project->accounts().actualCost( *a, QDate(), QDate() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 4.0 );
    QCOMPARE( ec.totalCost(), 425.0 );
}

void AccountsTester::startupAccount() {
    Account *a = new Account( "Account" );
    project->accounts().insert( a );
    a->addStartup( *t );
    
    // planned wo startup cost
    EffortCostMap ec = project->accounts().plannedCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );

    ec = project->accounts().actualCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );

    // planned with startup cost, no running cost
    t->setStartupCost( 25.0 );
    ec = project->accounts().plannedCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 25.0 );

    // actual, task not started
    ec = project->accounts().actualCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QVERIFY( ! t->completion().isStarted() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );
    
    // start task (tomorrow), no actual effort
    t->completion().setEntrymode( Completion::EnterCompleted );
    t->completion().setStarted( true );
    t->completion().setStartTime( DateTime( tomorrow, QTime() ) );
    QVERIFY( t->completion().isStarted() );

    ec = project->accounts().actualCost( *a, tomorrow, tomorrow );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 25.0 );

    ec = project->accounts().actualCost( *a, QDate(), QDate() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 25.0 );

    // add actual effort, but no running cost
    t->completion().setEntrymode( Completion::EnterEffortPerTask );
    t->completion().setActualEffort( tomorrow, Duration( 0, 4, 0 ) );
    ec = project->accounts().actualCost( *a, t->startTime().date(), tomorrow );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 25.0 );

    ec = project->accounts().actualCost( *a, QDate(), QDate() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 25.0 );

    // add running account
    a->addRunning( *t );
    // planned wo startup cost
    t->completion().setStarted( false );
    t->setStartupCost( 0.0 );

    ec = project->accounts().plannedCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 8.0 );
    QCOMPARE( ec.totalCost(), 800.0 );

    ec = project->accounts().actualCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );

    // planned with startup cost
    t->setStartupCost( 25.0 );
    ec = project->accounts().plannedCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 8.0 );
    QCOMPARE( ec.totalCost(), 825.0 );

    // actual, task not started
    ec = project->accounts().actualCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QVERIFY( ! t->completion().isStarted() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );
    
    // start task (tomorrow), 4h actual effort from before
    t->completion().setEntrymode( Completion::EnterCompleted );
    t->completion().setStarted( true );
    t->completion().setStartTime( DateTime( tomorrow, QTime() ) );
    QVERIFY( t->completion().isStarted() );

    Debug::print( t, "Started tomorrow, 4h actual effort" );
    Debug::print( t->completion(), t->name() );

    ec = project->accounts().actualCost( *a, tomorrow, tomorrow );
    Debug::print( ec );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 4.0 );
    QCOMPARE( ec.totalCost(), 425.0 );

    ec = project->accounts().actualCost( *a, QDate(), QDate() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 4.0 );
    QCOMPARE( ec.totalCost(), 425.0 );
}


void AccountsTester::shutdownAccount() {
    Account *a = new Account( "Account" );
    project->accounts().insert( a );
    a->addShutdown( *t );
    t->completion().setStarted( true );
    t->completion().setStartTime( t->startTime() );

    // planned wo shutdown cost
    EffortCostMap ec = project->accounts().plannedCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );

    ec = project->accounts().actualCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );

    // planned with shutdown cost, no running account
    t->setShutdownCost( 25.0 );
    ec = project->accounts().plannedCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 25.0 );

    // actual, task not finished
    ec = project->accounts().actualCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QVERIFY( ! t->completion().isFinished() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );
    
    // finish task (tomorrow), no actual effort
    t->completion().setEntrymode( Completion::EnterCompleted );
    t->completion().setFinished( true );
    t->completion().setFinishTime( DateTime( tomorrow, QTime() ) );
    QVERIFY( t->completion().isFinished() );

    ec = project->accounts().actualCost( *a, tomorrow, tomorrow );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 25.0 );

    ec = project->accounts().actualCost( *a, QDate(), QDate() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 25.0 );

    // add actual effort, no running account
    t->completion().setEntrymode( Completion::EnterEffortPerTask );
    t->completion().setActualEffort( tomorrow, Duration( 0, 4, 0 ) );
    ec = project->accounts().actualCost( *a, t->startTime().date(), tomorrow );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 25.0 );

    ec = project->accounts().actualCost( *a, QDate(), QDate() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 25.0 );

    // add running account
    a->addRunning( *t );
    t->completion().setFinished( false );
    t->setShutdownCost( 0.0 );
    // planned wo finish cost
    ec = project->accounts().plannedCost( *a, t->startTime().date(), t->endTime().date() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 8.0 );
    QCOMPARE( ec.totalCost(), 800.0 );

    ec = project->accounts().actualCost( *a, t->startTime().date(), t->endTime().date() );
    Debug::print( t, "planned wo finish cost, with running", true );
    Debug::print( t->completion(), t->name() );
    Debug::print( ec );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );

    // planned with startup cost
    t->setShutdownCost( 25.0 );
    ec = project->accounts().plannedCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 8.0 );
    QCOMPARE( ec.totalCost(), 825.0 );

    // actual, task not finished
    ec = project->accounts().actualCost( *a, t->startTime().date(), t->endTime().date() );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QVERIFY( ! t->completion().isFinished() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );
    
    // finish task (tomorrow), 4h actual effort from before
    t->completion().setEntrymode( Completion::EnterCompleted );
    t->completion().setFinished( true );
    t->completion().setFinishTime( DateTime( tomorrow, QTime() ) );
    QVERIFY( t->completion().isFinished() );

    ec = project->accounts().actualCost( *a, tomorrow, tomorrow );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 4.0 );
    QCOMPARE( ec.totalCost(), 425.0 );

    ec = project->accounts().actualCost( *a, QDate(), QDate() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 4.0 );
    QCOMPARE( ec.totalCost(), 425.0 );
}

void AccountsTester::subaccounts()
{
    Account *a1 = new Account( "Account" );
    project->accounts().insert( a1 );
    
    Account *a2 = new Account( "Sub-Account" );
    project->accounts().insert( a2, a1 );

    project->accounts().setDefaultAccount( a2 );
    
    EffortCostMap ec = project->accounts().plannedCost( *a2 );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 8.0 );
    QCOMPARE( ec.totalCost(), 800.0 );

    ec = project->accounts().plannedCost( *a1 );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 8.0 );
    QCOMPARE( ec.totalCost(), 800.0 );

    ec = project->accounts().actualCost( *a2 );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );

    ec = project->accounts().actualCost( *a1 );
    qDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );

    t->completion().setEntrymode( Completion::FollowPlan );
    ec = project->accounts().actualCost( *a2 );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 8.0 );
    QCOMPARE( ec.totalCost(), 800.0 );

    ec = project->accounts().actualCost( *a1 );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 8.0 );
    QCOMPARE( ec.totalCost(), 800.0 );

    t->completion().setEntrymode( Completion::EnterCompleted );
    ec = project->accounts().actualCost( *a2 );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );
    
    ec = project->accounts().actualCost( *a1 );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );

    t->completion().setEntrymode( Completion::EnterEffortPerTask );
    t->completion().setStarted( true );
    t->completion().setStartTime( DateTime( tomorrow, QTime() ) );
    t->completion().setPercentFinished( tomorrow, 50 );
    t->completion().setActualEffort( tomorrow, Duration( 0, 4, 0 ) );
    ec = project->accounts().actualCost( *a2 );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 4.0 );
    QCOMPARE( ec.totalCost(), 400.0 );

    ec = project->accounts().actualCost( *a1 );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 4.0 );
    QCOMPARE( ec.totalCost(), 400.0 );

    t->setStartupCost( 25.0 );
    a1->addStartup( *t );

    ec = project->accounts().actualCost( *a2 );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 4.0 );
    QCOMPARE( ec.totalCost(), 400.0 );

    ec = project->accounts().actualCost( *a1 );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 4.0 );
    QCOMPARE( ec.totalCost(), 425.0 );
    
    project->accounts().setDefaultAccount( a1 );
    
    ec = project->accounts().plannedCost( *a2 );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );

    ec = project->accounts().plannedCost( *a1 );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 8.0 );
    QCOMPARE( ec.totalCost(), 825.0 );

    ec = project->accounts().actualCost( *a2 );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );

    ec = project->accounts().actualCost( *a1 );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 4.0 );
    QCOMPARE( ec.totalCost(), 425.0 );
    
    t->setShutdownCost( 1.0 );
    a2->addShutdown( *t );
    
    ec = project->accounts().plannedCost( *a2 );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 1.0 );

    ec = project->accounts().plannedCost( *a1 );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 8.0 );
    QCOMPARE( ec.totalCost(), 826.0 );
    
    t->completion().setFinished( true );
    t->completion().setFinishTime( DateTime( tomorrow, QTime() ) );
    
    ec = project->accounts().actualCost( *a2 );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 1.0 );

    ec = project->accounts().actualCost( *a1 );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 4.0 );
    QCOMPARE( ec.totalCost(), 426.0 );
}

} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::AccountsTester )

#include "AccountsTester.moc"
