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

namespace KPlato
{

void AccountsTester::init()
{
    today = QDate::currentDate();
    tomorrow = today.addDays( 1 );
    yesterday = today.addDays( -1 );
    nextweek = today.addDays( 7 );
    t1 = QTime( 9, 0, 0 );
    t2 = QTime( 17, 0, 0 );
    length = t1.msecsTo( t2 );

    t = project.createTask( &project );
    t->setName( "T1" );
    project.addTask( t, &project );
    t->estimate()->setUnit( Duration::Unit_d );
    t->estimate()->setExpectedEstimate( 1.0 );
    
    ScheduleManager *sm = project.createScheduleManager( "Test Plan" );
    project.addScheduleManager( sm );
    
    // standard worktime defines 8 hour day as default
    Calendar *c = new Calendar();
    c->setDefault( true );
    for ( int i=1; i <= 7; ++i ) {
        CalendarDay *d = c->weekday( i );
        d->setState( CalendarDay::Working );
        d->addInterval( t1, length );
    }
    project.addCalendar( c );
    
    ResourceGroup *g = new ResourceGroup();
    project.addResourceGroup( g );
    r = new Resource();
    r->setAvailableFrom( QDateTime( yesterday, QTime() ) );
    r->setCalendar( c );
    r->setNormalRate( 100.0 );
    project.addResource( g, r );

    ResourceGroupRequest *gr = new ResourceGroupRequest( g );
    t->addRequest( gr );
    ResourceRequest *rr = new ResourceRequest( r, 100 );
    gr->addResourceRequest( rr );
    t->estimate()->setType( Estimate::Type_Effort );
    
    //kDebug()<<"Calculate forward, Task: ASAP -----------------------------------";
    project.setConstraintStartTime( DateTime( today, QTime() ) );
    project.setConstraintEndTime( DateTime( tomorrow, QTime() ) );
    project.calculate( *sm );
    
    QCOMPARE( t->earlyStart(), project.startTime() );
    QVERIFY( t->lateStart() >=  t->earlyStart() );
    QVERIFY( t->earlyFinish() <= t->endTime() );
    QVERIFY( t->lateFinish() >= t->endTime() );
    
    QCOMPARE( t->startTime(), DateTime( today, t1 ) );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );
    QVERIFY( t->schedulingError() == false );

    sm->setBaselined( true );
    

}
void AccountsTester::add() {
    Account *a = new Account( "Test Account" );
    project.accounts().insert( a );
    project.accounts().setDefaultAccount( a );
    
    EffortCostMap ec = project.accounts().plannedCost( *a, t->startTime().date(), t->endTime().date() );
    kDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 8.0 );
    QCOMPARE( ec.totalCost(), 800.0 );

    ec = project.accounts().actualCost( *a, t->startTime().date(), t->endTime().date() );
    kDebug()<<t->startTime()<<t->endTime()<<ec.totalEffort().toDouble( Duration::Unit_h );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );

    t->completion().setEntrymode( Completion::FollowPlan );
    ec = project.accounts().actualCost( *a, t->startTime().date(), t->endTime().date() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 8.0 );
    QCOMPARE( ec.totalCost(), 800.0 );

    t->completion().setEntrymode( Completion::EnterCompleted );
    ec = project.accounts().actualCost( *a, t->startTime().date(), t->endTime().date() );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );
    
    t->completion().setEntrymode( Completion::EnterCompleted );
    t->completion().setStarted( true );
    t->completion().setStartTime( DateTime( tomorrow, QTime() ) );
    t->completion().setPercentFinished( tomorrow, 50 );
    ec = project.accounts().actualCost( *a, t->startTime().date(), tomorrow );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 0.0 );
    QCOMPARE( ec.totalCost(), 0.0 );

    t->completion().setEntrymode( Completion::EnterEffortPerTask );
    t->completion().setActualEffort( tomorrow, Duration( 0, 4, 0 ) );
    ec = project.accounts().actualCost( *a, t->startTime().date(), tomorrow );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 4.0 );
    QCOMPARE( ec.totalCost(), 400.0 );

    t->completion().setEntrymode( Completion::EnterEffortPerResource );
    Completion::UsedEffort *ue = new Completion::UsedEffort();
    ue->setEffort( tomorrow, new Completion::UsedEffort::ActualEffort(  Duration( 0, 6, 0 ) ) );
    t->completion().addUsedEffort( r, ue );
    ec = project.accounts().actualCost( *a, t->startTime().date(), tomorrow );
    QCOMPARE( ec.totalEffort().toDouble( Duration::Unit_h ), 6.0 );
    QCOMPARE( ec.totalCost(), 600.0 );
}


} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::AccountsTester )

#include "AccountsTester.moc"
