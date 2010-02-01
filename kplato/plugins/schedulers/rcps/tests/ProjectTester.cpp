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

#include "KPlatoRCPSScheduler.h"

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
    
    QString s = "Calculate forward, Task: Fixed duration -----------------------------------";
    qDebug()<<s;
    KPlatoRCPSScheduler *rcps = new KPlatoRCPSScheduler( m_project, sm );
    qDebug()<<1;
    QCOMPARE( rcps->kplatoToRCPS(), 0 );
    rcps->doRun();
    qDebug()<<2;
    QCOMPARE( rcps->result, 0 );
    rcps->kplatoFromRCPS();
    qDebug()<<3;
    QCOMPARE( t->startTime(), m_project->startTime() );
    QCOMPARE( t->endTime(), DateTime(t->startTime().addDays( 1 )) );
    
    delete rcps;
    qDebug()<<4;

    s = "Calculate forward, Task: Length -----------------------------------";
    qDebug()<<s;
    t->estimate()->setCalendar( m_calendar );
    rcps = new KPlatoRCPSScheduler( m_project, sm );
    QCOMPARE( rcps->kplatoToRCPS(), 0 );
    rcps->doRun();
    QCOMPARE( rcps->result, 0 );
    rcps->kplatoFromRCPS();

    QCOMPARE( t->startTime(), m_calendar->firstAvailableAfter( m_project->startTime(), m_project->endTime() ) );
    QCOMPARE( t->endTime(), DateTime( t->startTime().addMSecs( length ) ) );

    delete rcps;
    
    s = "Calculate forward, Task: Effort -----------------------------------";
    qDebug()<<s;

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
    
    rcps = new KPlatoRCPSScheduler( m_project, sm );
    QCOMPARE( rcps->kplatoToRCPS(), 0 );
    rcps->doRun();
    QCOMPARE( rcps->result, 0 );
    rcps->kplatoFromRCPS();

    QCOMPARE( t->startTime(), m_calendar->firstAvailableAfter( m_project->startTime(), m_project->endTime() ) );
    QCOMPARE( t->endTime(), DateTime( t->startTime().addMSecs( length ) ) );
}

} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::ProjectTester )

#include "ProjectTester.moc"
