/* This file is part of the KDE project
   Copyright (C) 2010 Dag Andersen <danders@get2net.dk>

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

#include "PerformanceTester.h"
#include "kpttask.h"
#include "kptduration.h"
#include "kpteffortcostmap.h"
#include "kptcommand.h"

#include <qtest_kde.h>
#include <kdebug.h>

#include "debug.cpp"

namespace KPlato
{

void PerformanceTester::cleanup()
{
    delete p1;
    p1 = 0;
    r1 = 0;
    r2 = 0;
    s1 = 0;
    t1 = 0;
    s2 = 0;
    m1 = 0;
}

void PerformanceTester::init()
{
    p1 = new Project();
    p1->setId( p1->uniqueNodeId() );
    p1->registerNodeId( p1 );
    p1->setName( "PerformanceTester" );
    p1->setConstraintStartTime( DateTime( QDateTime::fromString( "2010-11-19T08:00:00", Qt::ISODate ) ) );
    p1->setConstraintEndTime( DateTime( QDateTime::fromString( "2010-11-29T16:00:00", Qt::ISODate ) ) );

    Calendar *c = new Calendar();
    c->setDefault( true );
    QTime ta( 8, 0, 0 );
    QTime tb ( 16, 0, 0 );
    int length = ta.msecsTo( tb );
    for ( int i=1; i <= 7; ++i ) {
        CalendarDay *d = c->weekday( i );
        d->setState( CalendarDay::Working );
        d->addInterval( ta, length );
    }
    p1->addCalendar( c );

    s1 = p1->createTask();
    s1->setName( "S1" );
    p1->addTask( s1, p1 );

    t1 = p1->createTask();
    t1->setName( "T1" );
    t1->estimate()->setUnit( Duration::Unit_d );
    t1->estimate()->setExpectedEstimate( 5.0 );
    t1->estimate()->setType( Estimate::Type_Effort );
    p1->addSubTask( t1, s1 );
    
    s2 = p1->createTask();
    s2->setName( "S2" );
    p1->addTask( s2, p1 );

    m1 = p1->createTask();
    m1->estimate()->setExpectedEstimate( 0 );
    m1->setName( "M1" );
    p1->addSubTask( m1, s2 );
    
    ResourceGroup *g = new ResourceGroup();
    g->setName( "G1" );
    p1->addResourceGroup( g );
    r1 = new Resource();
    r1->setName( "R1" );
    r1->setNormalRate( 1.0 );
    p1->addResource( g, r1 );

    ResourceGroupRequest *gr = new ResourceGroupRequest( g );
    t1->addRequest( gr );
    ResourceRequest *rr = new ResourceRequest( r1, 100 );
    gr->addResourceRequest( rr );

    // material resource
    ResourceGroup *m = new ResourceGroup();
    m->setName( "M1" );
    m->setType( ResourceGroup::Type_Material );
    p1->addResourceGroup( m );
    r2 = new Resource();
    r2->setName( "Material" );
    r2->setType( Resource::Type_Material );
    r2->setCalendar( c ); // limit availablity to working hours
    r2->setNormalRate( 0.0 ); // NOTE
    p1->addResource( m, r2 );

    r3 = new Resource();
    r3->setName( "Material 2" );
    r3->setType( Resource::Type_Material );
    r3->setNormalRate( 6.0 );
    p1->addResource( m, r3 );

    gr = new ResourceGroupRequest( m );
    t1->addRequest( gr );
    rr = new ResourceRequest( r2, 100 );
    gr->addResourceRequest( rr );

    ScheduleManager *sm = p1->createScheduleManager( "S1" );
    p1->addScheduleManager( sm );
    sm->createSchedules();
    p1->calculate( *sm );

    t1->completion().setStarted( true );
    t1->completion().setStartTime( t1->startTime() );

    Debug::print( p1, "Project data", true );
    QCOMPARE( t1->startTime(), p1->mustStartOn() );
    QCOMPARE( t1->endTime(), t1->startTime() + Duration( 4, 8, 0 ) );
}


void PerformanceTester::bcwsPrDayTask()
{
    QDate d = t1->startTime().date().addDays( -1 );
    EffortCostMap ecm = t1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 0, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 0.0 );
    
    d = d.addDays( 1 );
    ecm = t1->bcwsPrDay();
    qDebug()<<ecm;
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) ); // work+materal resource
    QCOMPARE( ecm.costOnDate( d ), 8.0 ); //material resource cost == 0
    
    d = d.addDays( 1 );
    ecm = t1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    ecm = t1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    ecm = t1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    ecm = t1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    ecm = t1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 0, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 0.0 );
    
    // add startup cost
    t1->setStartupCost( 0.5 );
    QCOMPARE( t1->startupCost(), 0.5 );
    d = t1->startTime().date();
    ecm = t1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.5 );

    // add shutdown cost
    t1->setShutdownCost( 0.5 );
    QCOMPARE( t1->shutdownCost(), 0.5 );
    d = t1->endTime().date();
    ecm = t1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.5 );

    // check sub-task
    d = t1->startTime().date();
    ecm = s1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.5 );

    d = s1->endTime().date();
    ecm = s1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.5 );
}

void PerformanceTester::bcwpPrDayTask()
{
    QDate d = t1->startTime().date();
    EffortCostMap ecm = t1->bcwpPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 );
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 0.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 0.0 );
    
    ModifyCompletionPercentFinishedCmd *cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 10 );
    cmd->execute();
    delete cmd;
    ecm = t1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 8.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 4.0 );
    
    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 20 );
    cmd->execute();
    delete cmd;
    ecm = t1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 16.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 30 );
    cmd->execute();
    delete cmd;
    ecm = t1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 24.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 12.0 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 40 );
    cmd->execute();
    delete cmd;
    ecm = t1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 32.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 16.0 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 50 );
    cmd->execute();
    delete cmd;
    ecm = t1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 40.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 20.0 );

    // modify last day
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 100 );
    cmd->execute();
    delete cmd;
    ecm = t1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 80.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 40.0 );

    // re-check
    d = t1->startTime().date();
    ecm = t1->bcwpPrDay();

    QCOMPARE( ecm.bcwpEffortOnDate( d ), 8.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 4.0 );

    d = d.addDays( 1 );
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 16.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 24.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 12.0 );

    d = d.addDays( 1 );
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 32.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 16.0 );

    d = d.addDays( 1 );
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 80.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 40.0 );

    // add startup cost
    t1->completion().setStartTime( t1->startTime() );
    t1->setStartupCost( 0.5 );
    d = t1->startTime().date();
    ecm = t1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 8.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 4.0 + 0.5 ); //10% progress

    // add shutdown cost
    t1->setShutdownCost( 0.5 );
    t1->completion().setFinished( true );
    t1->completion().setFinishTime( t1->endTime() );
    d = t1->endTime().date();
    ecm = t1->bcwpPrDay();
    Debug::print( t1->completion(), t1->name(), "BCWP Performance with shutdown cost" );
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 80.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 40.0 + 0.5 + 0.5 ); //100% progress

    // check sub-task
    d = s1->startTime().date();
    ecm = s1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 8.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 4.0 + 0.5 ); //10% progress

    // add shutdown cost
    d = s1->endTime().date();
    ecm = s1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 80.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 40.0 + 0.5 + 0.5 ); //100% progress
}

void PerformanceTester::acwpPrDayTask()
{
    NamedCommand *cmd = new ModifyCompletionEntrymodeCmd( t1->completion(), Completion::EnterEffortPerResource );
    cmd->execute(); delete cmd;
    Completion::UsedEffort *ue = new Completion::UsedEffort();
    t1->completion().addUsedEffort( r1, ue );
    
    QDate d = t1->startTime().date();
    EffortCostMap ecb = t1->bcwpPrDay();
    EffortCostMap eca = t1->acwp();

    QCOMPARE( ecb.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecb.costOnDate( d ), 8.0 );
    QCOMPARE( ecb.bcwpEffortOnDate( d ), 0.0 );
    QCOMPARE( ecb.bcwpCostOnDate( d ), 0.0 );
    QCOMPARE( eca.hoursOnDate( d ), 0.0 );
    QCOMPARE( eca.costOnDate( d ), 0.0 );
    
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 10 );
    cmd->execute(); delete cmd;
    cmd = new AddCompletionActualEffortCmd( *ue, d, Completion::UsedEffort::ActualEffort( Duration( 0, 8, 0 ) ) );
    cmd->execute(); delete cmd;
    ecb = t1->bcwpPrDay();
    eca = t1->acwp();
//     Debug::print( t1->completion(), t1->name(), QString( "ACWP on date: %1" ).arg( d.toString( Qt::ISODate ) ) );
//     Debug::print( eca, "ACWP" );
    QCOMPARE( ecb.bcwpEffortOnDate( d ), 8.0 );
    QCOMPARE( ecb.bcwpCostOnDate( d ), 4.0 );
    QCOMPARE( eca.hoursOnDate( d ), 8.0 );
    QCOMPARE( eca.costOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 20 );
    cmd->execute(); delete cmd;
    cmd = new AddCompletionActualEffortCmd( *ue, d, Completion::UsedEffort::ActualEffort( Duration( 0, 6, 0 ) ) );
    cmd->execute(); delete cmd;
    ecb = t1->bcwpPrDay();
    eca = t1->acwp();
    QCOMPARE( ecb.bcwpEffortOnDate( d ), 16.0 );
    QCOMPARE( ecb.bcwpCostOnDate( d ), 8.0 );
    QCOMPARE( eca.hoursOnDate( d ), 6.0 );
    QCOMPARE( eca.costOnDate( d ), 6.0 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 30 );
    cmd->execute(); delete cmd;
    cmd = new AddCompletionActualEffortCmd( *ue, d, Completion::UsedEffort::ActualEffort( Duration( 0, 8, 0 ) ) );
    cmd->execute(); delete cmd;
    ecb = t1->bcwpPrDay();
    eca = t1->acwp();
    QCOMPARE( ecb.bcwpEffortOnDate( d ), 24.0 );
    QCOMPARE( ecb.bcwpCostOnDate( d ), 12.0 );
    QCOMPARE( eca.hoursOnDate( d ), 8.0 );
    QCOMPARE( eca.costOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 50 );
    cmd->execute(); delete cmd;
    cmd = new AddCompletionActualEffortCmd( *ue, d, Completion::UsedEffort::ActualEffort( Duration( 0, 8, 0 ) ) );
    cmd->execute(); delete cmd;
    ecb = t1->bcwpPrDay();
    eca = t1->acwp();
    QCOMPARE( ecb.bcwpEffortOnDate( d ), 40.0 );
    QCOMPARE( ecb.bcwpCostOnDate( d ), 20.0 );
    QCOMPARE( eca.hoursOnDate( d ), 8.0 );
    QCOMPARE( eca.costOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 80 );
    cmd->execute(); delete cmd;
    cmd = new AddCompletionActualEffortCmd( *ue, d, Completion::UsedEffort::ActualEffort( Duration( 0, 12, 0 ) ) );
    cmd->execute(); delete cmd;
    ecb = t1->bcwpPrDay();
    eca = t1->acwp();
    QCOMPARE( ecb.bcwpEffortOnDate( d ), 64.0 );
    QCOMPARE( ecb.bcwpCostOnDate( d ), 32.0 );
    QCOMPARE( eca.hoursOnDate( d ), 12.0 );
    QCOMPARE( eca.costOnDate( d ), 12.0 );

    // add startup cost
    t1->completion().setStartTime( t1->startTime() );
    t1->setStartupCost( 0.5 );
    d = t1->startTime().date();
    eca = t1->acwp();
    Debug::print( t1->completion(), t1->name(), "ACWP Performance with startup cost" );
    QCOMPARE( eca.hoursOnDate( d ), 8.0 );
    QCOMPARE( eca.costOnDate( d ), 8.5 );

    // add shutdown cost
    d = t1->endTime().date();
    t1->setShutdownCost( 0.25 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 100 );
    cmd->execute(); delete cmd;
    t1->completion().setFinished( true );
    t1->completion().setFinishTime( t1->endTime() );
    eca = t1->acwp();
    Debug::print( t1->completion(), t1->name(), "ACWP Performance with shutdown cost" );
    QCOMPARE( eca.hoursOnDate( d ), 12.0 );
    QCOMPARE( eca.costOnDate( d ), 12.25 );

    // check sub-task
    d = s1->startTime().date();
    eca = s1->acwp();
    QCOMPARE( eca.hoursOnDate( d ), 8.0 );
    QCOMPARE( eca.costOnDate( d ), 8.5 );

    // add shutdown cost
    d = s1->endTime().date();
    eca = s1->acwp();
    QCOMPARE( eca.hoursOnDate( d ), 12.0 );
    QCOMPARE( eca.costOnDate( d ), 12.25 );
}

void PerformanceTester::bcwsMilestone()
{
    QDate d = m1->startTime().date().addDays( -1 );
    EffortCostMap ecm = m1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration::zeroDuration );
    QCOMPARE( ecm.costOnDate( d ), 0.0 );
    
    d = d.addDays( 1 );
    ecm = m1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration::zeroDuration );
    QCOMPARE( ecm.costOnDate( d ), 0.0 );
    
    // add startup cost
    m1->setStartupCost( 0.5 );
    QCOMPARE( m1->startupCost(), 0.5 );
    d = m1->startTime().date();
    ecm = m1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration::zeroDuration );
    QCOMPARE( ecm.costOnDate( d ), 0.5 );

    // add shutdown cost
    m1->setShutdownCost( 0.25 );
    QCOMPARE( m1->shutdownCost(), 0.25 );
    d = m1->endTime().date();
    ecm = m1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration::zeroDuration );
    QCOMPARE( ecm.costOnDate( d ), 0.75 );

}

void PerformanceTester::bcwpMilestone()
{
    QDate d = m1->startTime().date();
    EffortCostMap ecm = m1->bcwpPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration::zeroDuration );
    QCOMPARE( ecm.costOnDate( d ), 0.0 );
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 0.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 0.0 );
    
    ModifyCompletionPercentFinishedCmd *cmd = new ModifyCompletionPercentFinishedCmd( m1->completion(), d, 100 );
    cmd->execute(); delete cmd;
    ecm = m1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 0.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 0.0 );
    
    // add startup cost
    d = m1->startTime().date();
    m1->completion().setStarted( true );
    m1->completion().setStartTime( m1->startTime() );
    m1->setStartupCost( 0.5 );
    ecm = m1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 0.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 0.5 );

    // add shutdown cost
    d = m1->endTime().date();
    m1->setShutdownCost( 0.25 );
    m1->completion().setFinished( true );
    m1->completion().setFinishTime( m1->endTime() );
    ecm = m1->bcwpPrDay();
    Debug::print( m1->completion(), m1->name(), "BCWP Milestone with shutdown cost" );
    Debug::print( ecm, "BCWP Milestone" );
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 0.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 0.75 );

    // check sub-milestone
    d = s2->startTime().date();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 0.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 0.75 );
}

void PerformanceTester::acwpMilestone()
{
    QDate d = m1->startTime().date();
    EffortCostMap eca = m1->acwp();

    QCOMPARE( eca.hoursOnDate( d ), 0.0 );
    QCOMPARE( eca.costOnDate( d ), 0.0 );

    // add startup cost
    d = m1->startTime().date();
    m1->completion().setStarted( true );
    m1->completion().setStartTime( m1->startTime() );
    m1->setStartupCost( 0.5 );
    eca = m1->acwp();
    Debug::print( m1->completion(), m1->name(), "ACWP Milestone with startup cost" );
    QCOMPARE( eca.hoursOnDate( d ), 0.0 );
    QCOMPARE( eca.costOnDate( d ), 0.5 );

    // add shutdown cost
    d = m1->endTime().date();
    m1->setShutdownCost( 0.25 );
    NamedCommand *cmd = new ModifyCompletionPercentFinishedCmd( m1->completion(), d, 100 );
    cmd->execute(); delete cmd;
    m1->completion().setFinished( true );
    m1->completion().setFinishTime( m1->endTime() );
    eca = m1->acwp();
    Debug::print( m1->completion(), m1->name(), "ACWP Milestone with shutdown cost" );
    QCOMPARE( eca.hoursOnDate( d ), 0.0 );
    QCOMPARE( eca.costOnDate( d ), 0.75 );

    // check sub-milestone
    d = s2->endTime().date();
    eca = s2->acwp();
    QCOMPARE( eca.hoursOnDate( d ), 0.0 );
    QCOMPARE( eca.costOnDate( d ), 0.75 );
}

void PerformanceTester::bcwsPrDayTaskMaterial()
{
    r2->setNormalRate( 0.1 );
    QDate d = t1->startTime().date().addDays( -1 );
    EffortCostMap ecm = t1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 0, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 0.0 );
    
    d = d.addDays( 1 );
    ecm = t1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) ); //material+work resource
    QCOMPARE( ecm.costOnDate( d ), 8.8 );
    
    d = d.addDays( 1 );
    ecm = t1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.8 );

    d = d.addDays( 1 );
    ecm = t1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.8 );

    d = d.addDays( 1 );
    ecm = t1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.8 );

    d = d.addDays( 1 );
    ecm = t1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.8 );

    d = d.addDays( 1 );
    ecm = t1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 0, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 0.0 );
    
    // add startup cost
    t1->setStartupCost( 0.5 );
    QCOMPARE( t1->startupCost(), 0.5 );
    d = t1->startTime().date();
    ecm = t1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 + 0.5 + 0.8 );

    // add shutdown cost
    t1->setShutdownCost( 0.25 );
    QCOMPARE( t1->shutdownCost(), 0.25 );
    d = t1->endTime().date();
    ecm = t1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 + 0.25 + 0.8 );

    // check sub-task
    d = s1->startTime().date();
    ecm = s1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 + 0.5 + 0.8 );

    d = s1->endTime().date();
    ecm = s1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 + 0.25 + 0.8 );
}

void PerformanceTester::bcwpPrDayTaskMaterial()
{
    r2->setNormalRate( 0.1 );

    QDate d = t1->startTime().date();
    EffortCostMap ecm = t1->bcwpPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 + 0.8 );
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 0.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 0.0 );
    
    ModifyCompletionPercentFinishedCmd *cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 10 );
    cmd->execute();
    delete cmd;
    ecm = t1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 8.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 4.0 + 0.4 );
    
    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 20 );
    cmd->execute();
    delete cmd;
    ecm = t1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 16.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 8.0 + 0.8 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 30 );
    cmd->execute();
    delete cmd;
    ecm = t1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 24.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 12.0 + 1.2 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 40 );
    cmd->execute();
    delete cmd;
    ecm = t1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 32.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 16.0 + 1.6 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 50 );
    cmd->execute();
    delete cmd;
    ecm = t1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 40.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 20.0 + 2.0 );

    // modify last day
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 100 );
    cmd->execute();
    delete cmd;
    ecm = t1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 80.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 40.0 + 4.0 );

    // add startup cost
    t1->completion().setStartTime( t1->startTime() );
    t1->setStartupCost( 0.5 );
    d = t1->startTime().date();
    ecm = t1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 8.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 4.0 + 0.5 + 0.4 ); // 10% progress

    // add shutdown cost
    t1->setShutdownCost( 0.25 );
    t1->completion().setFinished( true );
    t1->completion().setFinishTime( t1->endTime() );
    d = t1->endTime().date();
    ecm = t1->bcwpPrDay();
    Debug::print( t1->completion(), t1->name(), "BCWP Material with shutdown cost" );
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 80.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 40.0 + 0.5 + 0.25 + 4.0 ); // 100% progress

    // check sub-task
    d = s1->startTime().date();
    ecm = s1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 8.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 4.0 + 0.5 + 0.4 ); // 10% progress

    d = s1->endTime().date();
    ecm = s1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 80.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 40.0 + 0.5 + 0.25 + 4.0 ); // 100% progress

}

void PerformanceTester::acwpPrDayTaskMaterial()
{
    r2->setNormalRate( 0.1 );

    NamedCommand *cmd = new ModifyCompletionEntrymodeCmd( t1->completion(), Completion::EnterEffortPerResource );
    cmd->execute(); delete cmd;
    Completion::UsedEffort *ue = new Completion::UsedEffort();
    t1->completion().addUsedEffort( r1, ue );
    
    QDate d = t1->startTime().date();
    EffortCostMap ecb = t1->bcwpPrDay();
    EffortCostMap eca = t1->acwp();

    QCOMPARE( ecb.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecb.costOnDate( d ), 8.0 + 0.8 );
    QCOMPARE( ecb.bcwpEffortOnDate( d ), 0.0 );
    QCOMPARE( ecb.bcwpCostOnDate( d ), 0.0 );
    QCOMPARE( eca.hoursOnDate( d ), 0.0 );
    QCOMPARE( eca.costOnDate( d ), 0.0 );
    
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 10 );
    cmd->execute(); delete cmd;
    cmd = new AddCompletionActualEffortCmd( *ue, d, Completion::UsedEffort::ActualEffort( Duration( 0, 8, 0 ) ) );
    cmd->execute(); delete cmd;
    ecb = t1->bcwpPrDay();
    eca = t1->acwp();
//     Debug::print( t1->completion(), t1->name(), QString( "ACWP on date: %1" ).arg( d.toString( Qt::ISODate ) ) );
//     Debug::print( eca, "ACWP" );
    QCOMPARE( ecb.bcwpEffortOnDate( d ), 8.0 );
    QCOMPARE( ecb.bcwpCostOnDate( d ), 4.0 + 0.4 );
    QCOMPARE( eca.hoursOnDate( d ), 8.0 );
    QCOMPARE( eca.costOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 20 );
    cmd->execute(); delete cmd;
    cmd = new AddCompletionActualEffortCmd( *ue, d, Completion::UsedEffort::ActualEffort( Duration( 0, 6, 0 ) ) );
    cmd->execute(); delete cmd;
    ecb = t1->bcwpPrDay();
    eca = t1->acwp();
    QCOMPARE( ecb.bcwpEffortOnDate( d ), 16.0 );
    QCOMPARE( ecb.bcwpCostOnDate( d ), 8.0 + 0.8 );
    QCOMPARE( eca.hoursOnDate( d ), 6.0 );
    QCOMPARE( eca.costOnDate( d ), 6.0 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 30 );
    cmd->execute(); delete cmd;
    cmd = new AddCompletionActualEffortCmd( *ue, d, Completion::UsedEffort::ActualEffort( Duration( 0, 8, 0 ) ) );
    cmd->execute(); delete cmd;
    ecb = t1->bcwpPrDay();
    eca = t1->acwp();
    QCOMPARE( ecb.bcwpEffortOnDate( d ), 24.0 );

    QCOMPARE( ecb.bcwpCostOnDate( d ), 12.0 + 1.2 );
    QCOMPARE( eca.hoursOnDate( d ), 8.0 );
    QCOMPARE( eca.costOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 50 );
    cmd->execute(); delete cmd;
    cmd = new AddCompletionActualEffortCmd( *ue, d, Completion::UsedEffort::ActualEffort( Duration( 0, 8, 0 ) ) );
    cmd->execute(); delete cmd;
    ecb = t1->bcwpPrDay();
    eca = t1->acwp();
    QCOMPARE( ecb.bcwpEffortOnDate( d ), 40.0 );
    QCOMPARE( ecb.bcwpCostOnDate( d ), 20.0 + 2.0 );
    QCOMPARE( eca.hoursOnDate( d ), 8.0 );
    QCOMPARE( eca.costOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 80 );
    cmd->execute(); delete cmd;
    cmd = new AddCompletionActualEffortCmd( *ue, d, Completion::UsedEffort::ActualEffort( Duration( 0, 12, 0 ) ) );
    cmd->execute(); delete cmd;
    ecb = t1->bcwpPrDay();
    eca = t1->acwp();
    QCOMPARE( ecb.bcwpEffortOnDate( d ), 64.0 );
    QCOMPARE( ecb.bcwpCostOnDate( d ), 32.0 + 3.2 );
    QCOMPARE( eca.hoursOnDate( d ), 12.0 );
    QCOMPARE( eca.costOnDate( d ), 12.0 );

    // add startup cost
    t1->completion().setStartTime( t1->startTime() );
    t1->setStartupCost( 0.5 );
    d = t1->startTime().date();
    eca = t1->acwp();
    Debug::print( t1->completion(), t1->name(), "ACWP Performance with startup cost" );
    QCOMPARE( eca.hoursOnDate( d ), 8.0 );
    QCOMPARE( eca.costOnDate( d ), 8.0 + 0.5 ); //NOTE: material not included

    // add shutdown cost
    d = t1->endTime().date();
    t1->setShutdownCost( 0.25 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 100 );
    cmd->execute(); delete cmd;
    t1->completion().setFinished( true );
    t1->completion().setFinishTime( t1->endTime() );
    eca = t1->acwp();
    Debug::print( t1->completion(), t1->name(), "ACWP Performance with shutdown cost" );
    QCOMPARE( eca.hoursOnDate( d ), 12.0 );
    QCOMPARE( eca.costOnDate( d ), 12.0 + 0.25 ); //NOTE: material not included

    // check sub-task
    d = s1->startTime().date();
    eca = s1->acwp();
    QCOMPARE( eca.hoursOnDate( d ), 8.0 );
    QCOMPARE( eca.costOnDate( d ), 8.0 + 0.5 ); //NOTE: material not included

    // add shutdown cost
    d = s1->endTime().date();
    eca = s1->acwp();
    QCOMPARE( eca.hoursOnDate( d ), 12.0 );
    QCOMPARE( eca.costOnDate( d ), 12.0 + 0.25 ); //NOTE: material not included
}

void PerformanceTester::bcwsPrDayProject()
{
    QDate d = p1->startTime().date().addDays( -1 );
    EffortCostMap ecm = p1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 0, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 0.0 );
    
    d = d.addDays( 1 );
    ecm = p1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 );
    
    d = d.addDays( 1 );
    ecm = p1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    ecm = p1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    ecm = p1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    ecm = p1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    ecm = p1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 0, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 0.0 );
    
    // add startup cost to task
    t1->setStartupCost( 0.5 );
    QCOMPARE( t1->startupCost(), 0.5 );
    d = p1->startTime().date();
    ecm = p1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 + 0.5 );

    // add shutdown cost to task
    t1->setShutdownCost( 0.2 );
    QCOMPARE( t1->shutdownCost(), 0.2 );
    d = p1->endTime().date();
    ecm = p1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 + 0.2 );

    // add startup cost to milestone
    m1->setStartupCost( 0.4 );
    QCOMPARE( m1->startupCost(), 0.4 );
    d = p1->startTime().date();
    ecm = p1->bcwsPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 + 0.5 + 0.4 );

    // add shutdown cost to milestone
    m1->setShutdownCost( 0.3 );
    QCOMPARE( m1->shutdownCost(), 0.3 );
    d = p1->startTime().date();
    ecm = p1->bcwsPrDay();
    //Debug::print( p1, "BCWS Project", true );
    //Debug::print( ecm, "BCWS Project" );
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 + 0.5 + 0.4 + 0.3 );
}

void PerformanceTester::bcwpPrDayProject()
{
    QDate d = p1->startTime().date();
    EffortCostMap ecm = p1->bcwpPrDay();
    QCOMPARE( ecm.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecm.costOnDate( d ), 8.0 );
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 0.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 0.0 );
    
    ModifyCompletionPercentFinishedCmd *cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 10 );
    cmd->execute(); delete cmd;
    ecm = p1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 8.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 4.0 );
    
    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 20 );
    cmd->execute(); delete cmd;
    ecm = p1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 16.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 30 );
    cmd->execute(); delete cmd;
    ecm = p1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 24.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 12.0 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 40 );
    cmd->execute(); delete cmd;
    ecm = p1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 32.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 16.0 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 50 );
    cmd->execute(); delete cmd;
    ecm = p1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 40.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 20.0 );

    // modify last day
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 100 );
    cmd->execute(); delete cmd;
    ecm = p1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 80.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 40.0 );

    // re-check
    d = p1->startTime().date();
    ecm = p1->bcwpPrDay();

    QCOMPARE( ecm.bcwpEffortOnDate( d ), 8.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 4.0 );

    d = d.addDays( 1 );
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 16.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 24.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 12.0 );

    d = d.addDays( 1 );
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 32.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 16.0 );

    d = d.addDays( 1 );
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 80.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 40.0 );

    // add startup cost to task
    t1->completion().setStartTime( t1->startTime() );
    t1->setStartupCost( 0.5 );
    d = p1->startTime().date();
    ecm = p1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 8.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 4.0 + 0.5 ); // 10% progress

    // add shutdown cost to task
    t1->setShutdownCost( 0.25 );
    t1->completion().setFinished( true );
    t1->completion().setFinishTime( t1->endTime() );
    d = p1->endTime().date();
    ecm = p1->bcwpPrDay();
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 80.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 40.0 + 0.5 + 0.25 ); // 100% progress

    // check with ECCT_EffortWork
    
    d = p1->startTime().date();
    ecm = p1->bcwpPrDay( CURRENTSCHEDULE, ECCT_EffortWork );
    QCOMPARE( ecm.totalEffort(), Duration( 40.0, Duration::Unit_h ) );
    QCOMPARE( ecm.hoursOnDate( d ), 8.0 ); // hours from r1
    QCOMPARE( ecm.costOnDate( d ), 8.0 + 0.5 ); // cost from r1 (1.0) + r2 (0.0) + startup cost (0.5)
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 4.0 ); // 10% progress
    QCOMPARE( ecm.bcwpCostOnDate( d ), 4.0 + 0.5 ); // 10% progress

    d = p1->endTime().date();
    ecm = p1->bcwpPrDay( CURRENTSCHEDULE, ECCT_EffortWork );
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 40.0 ); // hours from r1
    QCOMPARE( ecm.bcwpCostOnDate( d ), 40.0 + 0.5 + 0.25 ); // 100% progress

    // add a new task with a material resource
    Task *tt = p1->createTask();
    tt->setName( "TT" );
    p1->addTask( tt, p1 );
    tt->estimate()->setUnit( Duration::Unit_d );
    tt->estimate()->setExpectedEstimate( 5.0 );
    tt->estimate()->setType( Estimate::Type_Duration );
    tt->estimate()->setCalendar( p1->calendarAt( 0 ) );

    r3->setNormalRate( 1.0 );

    ResourceGroupRequest *gr = new ResourceGroupRequest( r3->parentGroup() );
    tt->addRequest( gr );
    ResourceRequest *rr = new ResourceRequest( r3, 100 );
    gr->addResourceRequest( rr );

    ScheduleManager *sm = p1->createScheduleManager( "" );
    p1->addScheduleManager( sm );
    sm->createSchedules();
    p1->calculate( *sm );

    QString s = " Material resource, no progress ";
    Debug::print( tt, s, true );

    d = tt->endTime().date();
    ecm = tt->bcwpPrDay( sm->scheduleId(), ECCT_EffortWork );
    Debug::print( ecm, "BCWP: " + tt->name() + s );

    QCOMPARE( ecm.hoursOnDate( d ), 0.0 );
    QCOMPARE( ecm.costOnDate( d ), 16.0 );
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 0.0 );
    QCOMPARE( ecm.bcwpCostOnDate( d ), 0.0 ); // 0% progress

    d = p1->endTime().date();
    ecm = p1->bcwpPrDay( sm->scheduleId(), ECCT_EffortWork );
    Debug::print( p1, s, true );
    Debug::print( ecm, "BCWP Project: " + p1->name() + s );
    QCOMPARE( ecm.bcwpEffortOnDate( d ), 40.0 ); // hours from r1
    QCOMPARE( ecm.bcwpCostOnDate( d ), 40.0 + 0.5 + 0.25 );
}

void PerformanceTester::acwpPrDayProject()
{
    NamedCommand *cmd = new ModifyCompletionEntrymodeCmd( t1->completion(), Completion::EnterEffortPerResource );
    cmd->execute(); delete cmd;
    Completion::UsedEffort *ue = new Completion::UsedEffort();
    t1->completion().addUsedEffort( r1, ue );
    
    QDate d = p1->startTime().date();
    EffortCostMap ecb = p1->bcwpPrDay();
    EffortCostMap eca = p1->acwp();

    QCOMPARE( ecb.effortOnDate( d ), Duration( 0, 16, 0 ) );
    QCOMPARE( ecb.costOnDate( d ), 8.0 );
    QCOMPARE( ecb.bcwpEffortOnDate( d ), 0.0 );
    QCOMPARE( ecb.bcwpCostOnDate( d ), 0.0 );
    QCOMPARE( eca.hoursOnDate( d ), 0.0 );
    QCOMPARE( eca.costOnDate( d ), 0.0 );
    
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 10 );
    cmd->execute(); delete cmd;
    cmd = new AddCompletionActualEffortCmd( *ue, d, Completion::UsedEffort::ActualEffort( Duration( 0, 8, 0 ) ) );
    cmd->execute(); delete cmd;
    ecb = p1->bcwpPrDay();
    eca = p1->acwp();
//     Debug::print( eca, "ACWP Project" );
    QCOMPARE( ecb.bcwpEffortOnDate( d ), 8.0 );
    QCOMPARE( ecb.bcwpCostOnDate( d ), 4.0 );
    QCOMPARE( eca.hoursOnDate( d ), 8.0 );
    QCOMPARE( eca.costOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 20 );
    cmd->execute(); delete cmd;
    cmd = new AddCompletionActualEffortCmd( *ue, d, Completion::UsedEffort::ActualEffort( Duration( 0, 6, 0 ) ) );
    cmd->execute(); delete cmd;
    ecb = p1->bcwpPrDay();
    eca = p1->acwp();
    QCOMPARE( ecb.bcwpEffortOnDate( d ), 16.0 );
    QCOMPARE( ecb.bcwpCostOnDate( d ), 8.0 );
    QCOMPARE( eca.hoursOnDate( d ), 6.0 );
    QCOMPARE( eca.costOnDate( d ), 6.0 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 30 );
    cmd->execute(); delete cmd;
    cmd = new AddCompletionActualEffortCmd( *ue, d, Completion::UsedEffort::ActualEffort( Duration( 0, 8, 0 ) ) );
    cmd->execute(); delete cmd;
    ecb = p1->bcwpPrDay();
    eca = p1->acwp();
    QCOMPARE( ecb.bcwpEffortOnDate( d ), 24.0 );
    QCOMPARE( ecb.bcwpCostOnDate( d ), 12.0 );
    QCOMPARE( eca.hoursOnDate( d ), 8.0 );
    QCOMPARE( eca.costOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 50 );
    cmd->execute(); delete cmd;
    cmd = new AddCompletionActualEffortCmd( *ue, d, Completion::UsedEffort::ActualEffort( Duration( 0, 8, 0 ) ) );
    cmd->execute(); delete cmd;
    ecb = p1->bcwpPrDay();
    eca = p1->acwp();
    QCOMPARE( ecb.bcwpEffortOnDate( d ), 40.0 );
    QCOMPARE( ecb.bcwpCostOnDate( d ), 20.0 );
    QCOMPARE( eca.hoursOnDate( d ), 8.0 );
    QCOMPARE( eca.costOnDate( d ), 8.0 );

    d = d.addDays( 1 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 80 );
    cmd->execute(); delete cmd;
    cmd = new AddCompletionActualEffortCmd( *ue, d, Completion::UsedEffort::ActualEffort( Duration( 0, 12, 0 ) ) );
    cmd->execute(); delete cmd;
    ecb = p1->bcwpPrDay();
    eca = p1->acwp();
    QCOMPARE( ecb.bcwpEffortOnDate( d ), 64.0 );
    QCOMPARE( ecb.bcwpCostOnDate( d ), 32.0 );
    QCOMPARE( eca.hoursOnDate( d ), 12.0 );
    QCOMPARE( eca.costOnDate( d ), 12.0 );

    // add startup cost
    t1->completion().setStartTime( t1->startTime() );
    t1->completion().setStarted( true );
    t1->setStartupCost( 0.5 );
    d = p1->startTime().date();
    eca = p1->acwp();
    QCOMPARE( eca.hoursOnDate( d ), 8.0 );
    QCOMPARE( eca.costOnDate( d ), 8.0 + 0.5 );

    // add shutdown cost
    d = t1->endTime().date();
    t1->setShutdownCost( 0.25 );
    cmd = new ModifyCompletionPercentFinishedCmd( t1->completion(), d, 100 );
    cmd->execute(); delete cmd;
    t1->completion().setFinished( true );
    t1->completion().setFinishTime( t1->endTime() );
    d = p1->endTime().date();
    eca = p1->acwp();
    QCOMPARE( eca.hoursOnDate( d ), 12.0 );
    QCOMPARE( eca.costOnDate( d ), 12.25 );
}

} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::PerformanceTester )

#include "PerformanceTester.moc"
