/* This file is part of the KDE project
   Copyright (C) 2007 - 2011 Dag Andersen <danders@get2net.dk>

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

#include <kglobal.h>
#include <klocale.h>
#include <kcalendarsystem.h>
#include <ksystemtimezone.h>
#include <kdatetime.h>
#include <kconfiggroup.h>

#include <QDir>
#include <QtDBus>

#include <qtest_kde.h>
#include <kdebug.h>

#include "tests/DateTimeTester.h"

#include "tests/debug.cpp"

namespace KPlato
{

void ProjectTester::initTimezone()
{
    QVERIFY( m_tmp.exists() );

    QFile f;
    f.setFileName( m_tmp.name() + QLatin1String( "zone.tab" ) );
    f.open(QIODevice::WriteOnly);
    QTextStream fStream(&f);
    fStream << "DE  +5230+01322 Europe/Berlin\n"
               "EG  +3003+03115 Africa/Cairo\n"
               "FR  +4852+00220 Europe/Paris\n"
               "GB  +512830-0001845 Europe/London   Great Britain\n"
               "US  +340308-1181434 America/Los_Angeles Pacific Time\n";
    f.close();
    QDir dir(m_tmp.name());
    QVERIFY(dir.mkdir("Africa"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/Cairo"), m_tmp.name() + QLatin1String("Africa/Cairo"));
    QVERIFY(dir.mkdir("America"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/Los_Angeles"), m_tmp.name() + QLatin1String("America/Los_Angeles"));
    QVERIFY(dir.mkdir("Europe"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/Berlin"), m_tmp.name() + QLatin1String("Europe/Berlin"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/London"), m_tmp.name() + QLatin1String("Europe/London"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/Paris"), m_tmp.name() + QLatin1String("Europe/Paris"));

    // NOTE: QTEST_KDEMAIN_CORE puts the config file in QDir::homePath() + "/.kde-unit-test"
    //       and hence, this is common to all unit tests
    KConfig config("ktimezonedrc");
    KConfigGroup group(&config, "TimeZones");
    group.writeEntry("ZoneinfoDir", m_tmp.name());
    group.writeEntry("Zonetab", m_tmp.name() + QString::fromLatin1("zone.tab"));
    group.writeEntry("LocalZone", QString::fromLatin1("Europe/Berlin"));
    config.sync();
}

void ProjectTester::cleanupTimezone()
{
}

static ResourceGroup *createWorkResources( Project &p, int count )
{
    ResourceGroup *g = new ResourceGroup();
    g->setName( "G1" );
    p.addResourceGroup( g );

    for ( int i = 0; i < count; ++i ) {
        Resource *r = new Resource();
        r->setName( QString( "R%1" ).arg( i + 1 ) );
        p.addResource( g, r );
    }
    return g;
}

static void createRequest( Task *t, Resource *r )
{
    ResourceGroupRequest *gr = t->requests().find( r->parentGroup() );
    if ( gr == 0 ) {
        gr = new ResourceGroupRequest( r->parentGroup() );
    }
    t->addRequest( gr );
    ResourceRequest *rr = new ResourceRequest( r, 100 );
    gr->addResourceRequest( rr );
}

static Calendar *createCalendar( Project &p )
{
    Calendar *c = new Calendar();
    c->setDefault( true );
    QTime t1( 9, 0, 0 );
    QTime t2 ( 17, 0, 0 );
    int length = t1.msecsTo( t2 );
    for ( int i=1; i <= 7; ++i ) {
        CalendarDay *d = c->weekday( i );
        d->setState( CalendarDay::Working );
        d->addInterval( t1, length );
    }
    p.addCalendar( c );
    return c;
}

void ProjectTester::initTestCase()
{
    initTimezone();

    m_project = new Project();
    m_project->setName( "P1" );
    m_project->setId( m_project->uniqueNodeId() );
    m_project->registerNodeId( m_project );
    m_project->setConstraintStartTime( DateTime::fromString( "2012-02-01 00:00" ) );
    m_project->setConstraintEndTime( m_project->constraintStartTime().addDays( 7 ) );
    // standard worktime defines 8 hour day as default
    QVERIFY( m_project->standardWorktime() );
    QCOMPARE( m_project->standardWorktime()->day(), 8.0 );

    m_calendar = createCalendar( *m_project );

    m_task = 0;
    qDebug()<<"Project:"<<m_project->constraintStartTime()<<m_project->constraintEndTime();
    Debug::print( m_project, "Initiated to:" );
}

void ProjectTester::cleanupTestCase()
{
    qDebug()<<this<<m_project;
    delete m_project;
    cleanupTimezone();
}

void ProjectTester::oneTask()
{
    QDate today = QDate::fromString( "2012-02-01", Qt::ISODate );
    QDate tomorrow = today.addDays( 1 );
    QDate yesterday = today.addDays( -1 );
    QTime t1( 9, 0, 0 );
    QTime t2 ( 17, 0, 0 );
    int length = t1.msecsTo( t2 );

    Task *t = m_project->createTask();
    t->setName( "T1" );
    m_project->addTask( t, m_project );
    t->estimate()->setUnit( Duration::Unit_d );
    t->estimate()->setExpectedEstimate( 1.0 );
    t->estimate()->setType( Estimate::Type_Duration );

    ScheduleManager *sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    

    QString s = "Calculate forward, Task: Fixed duration ------------------------------";
    qDebug()<<s;

    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( *m_project, sm, true/*nothread*/ );
    }
    Debug::printSchedulingLog( *sm, s );
    Debug::print( m_project, s );
    Debug::print( t, s );

    QCOMPARE( t->startTime(), m_project->startTime() );
    QCOMPARE( t->endTime(), DateTime(t->startTime().addDays( 1 )) );

    s = "Calculate forward, Task: Length --------------------------------------";
    qDebug()<<s;

    t->estimate()->setCalendar( m_calendar );

    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( *m_project, sm, true/*nothread*/ );
    }

    Debug::print( m_calendar, s );
    Debug::print( m_project, s, true );

    QCOMPARE( t->startTime(), m_calendar->firstAvailableAfter( m_project->startTime(), m_project->endTime() ) );
    QCOMPARE( t->endTime(), DateTime( t->startTime().addMSecs( length ) ) );

    s = "Calculate forward, Task: Effort --------------------------------------";
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

    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( *m_project, sm, true/*nothread*/ );
    }

    Debug::print( t, s );

    QCOMPARE( t->startTime(), m_calendar->firstAvailableAfter( m_project->startTime(), m_project->endTime() ) );
    QCOMPARE( t->endTime(), DateTime( t->startTime().addMSecs( length ) ) );


    s = "Calculate forward, Task: MustStartOn --------------------------------------";
    qDebug()<<s;

    t->setConstraint( Node::MustStartOn );
    t->setConstraintStartTime( DateTime( tomorrow, t1 ) );

    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( *m_project, sm, true/*nothread*/ );
    }

    Debug::print( t, s );

    QCOMPARE( t->startTime(), t->constraintStartTime() );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );

    s = "Calculate backward, Task: MustStartOn --------------------------------------";
    qDebug()<<s;

    sm->setSchedulingDirection( true );

    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( *m_project, sm, true/*nothread*/ );
    }

    Debug::print( t, s );
    Debug::printSchedulingLog( *sm, s );

    QCOMPARE( t->startTime(), t->constraintStartTime() );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );

    s = "Calculate backward, Task: StartNotEarlier --------------------------------------";
    qDebug()<<s;

    t->setConstraint( Node::StartNotEarlier );

    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( *m_project, sm, true/*nothread*/ );
    }

    Debug::print( t, s );

    QCOMPARE( t->startTime(), t->constraintStartTime() );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );

    s = "Calculate forward, Task: StartNotEarlier --------------------------------------";
    qDebug()<<s;

    sm->setSchedulingDirection( false );
    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( *m_project, sm, true/*nothread*/ );
    }

    Debug::print( t, s );

    QCOMPARE( t->startTime(), t->constraintStartTime() );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );

    s = "Calculate forward, Task: MustFinishOn --------------------------------------";
    qDebug()<<s;

    t->setConstraint( Node::MustFinishOn );
    t->setConstraintEndTime( DateTime( tomorrow, t2 ) );

    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( *m_project, sm, true/*nothread*/ );
    }

    Debug::print( t, s );

    QCOMPARE( t->endTime(), t->constraintEndTime() );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );

    s = "Calculate backward, Task: MustFinishOn --------------------------------------";
    qDebug()<<s;

    sm->setSchedulingDirection( true );
    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( *m_project, sm, true/*nothread*/ );
    }

    Debug::print( t, s );

    QCOMPARE( t->endTime(), t->constraintEndTime() );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );

    s = "Calculate backward, Task: FinishNotLater --------------------------------------";
    qDebug()<<s;

    t->setConstraint( Node::FinishNotLater );

    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( *m_project, sm, true/*nothread*/ );
    }

    Debug::print( t, s );

    QCOMPARE( t->endTime(), t->constraintEndTime() );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );

    s = "Calculate forward, Task: FinishNotLater --------------------------------------";
    qDebug()<<s;

    sm->setSchedulingDirection( false );
    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( *m_project, sm, true/*nothread*/ );
    }

    Debug::print( m_project, s, true );

    QVERIFY( t->endTime() <= t->constraintEndTime() );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );

    s = "Calculate forward, Task: FixedInterval --------------------------------------";
    qDebug()<<s;

    t->setConstraint( Node ::FixedInterval );

    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( *m_project, sm, true/*nothread*/ );
    }

    Debug::print( t, s );

    QCOMPARE( t->startTime(), t->constraintStartTime() );
    QCOMPARE( t->endTime(), t->constraintEndTime() );

    s = "Calculate backward, Task: FixedInterval --------------------------------------";
    qDebug()<<s;

    sm->setSchedulingDirection( true );
    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( *m_project, sm, true/*nothread*/ );
    }

    Debug::print( t, s );

    QCOMPARE( t->startTime(), t->constraintStartTime() );
    QCOMPARE( t->endTime(), t->constraintEndTime() );

}

void ProjectTester::team()
{
    Project project;
    project.setName( "P1" );
    project.setId( project.uniqueNodeId() );
    project.registerNodeId( &project );
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

    Task *task1 = project.createTask();
    task1->setName( "T1" );
    project.addTask( task1, &project );
    task1->estimate()->setUnit( Duration::Unit_d );
    task1->estimate()->setExpectedEstimate( 2.0 );
    task1->estimate()->setType( Estimate::Type_Effort );

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
    team->addTeamMemberId( r2->id() );
    project.addResource( g, team );
    
    ResourceGroupRequest *gr = new ResourceGroupRequest( g );
    task1->addRequest( gr );
    ResourceRequest *tr = new ResourceRequest( team, 100 );
    gr->addResourceRequest( tr );

    ScheduleManager *sm = project.createScheduleManager( "Team" );
    project.addScheduleManager( sm );
    sm->createSchedules();

    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( project, sm, true/*nothread*/ );
    }

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

    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( project, sm, true/*nothread*/ );
    }

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
    
    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( project, sm, true/*nothread*/ );
    }

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
    team->addTeamMemberId( r1->id() );
    r1->setAvailableFrom( targetstart );
    r1->setAvailableUntil( targetend );

    sm = project.createScheduleManager( "Team with 2 resources" );
    project.addScheduleManager( sm );
    sm->createSchedules();

    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( project, sm, true/*nothread*/ );
    }

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

    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( project, sm, true/*nothread*/ );
    }

    Debug::print( r1, s);
//    Debug::print( r2, s);
    Debug::print( team, s, false);
    Debug::print( &project, task1, s);
    Debug::printSchedulingLog( *sm, s );
    expectedEndTime = targetstart + Duration( 1, 16, 0 );
    QCOMPARE( task1->endTime(), expectedEndTime );

}

void ProjectTester::mustStartOn()
{
    return;
    Project project;
    project.setId( project.uniqueNodeId() );
    project.registerNodeId( &project );
    project.setConstraintStartTime( DateTime::fromString( "2011-01-01T00:00:00" ) );
    project.setConstraintEndTime( DateTime::fromString( "2011-01-12T00:00:00" ) );

    createCalendar( project );

    ResourceGroup *g = createWorkResources( project, 1 );

    Task *t = project.createTask();
    t->setName( "T1" );
    project.addTask( t, &project );
    t->estimate()->setUnit( Duration::Unit_h );
    t->estimate()->setExpectedEstimate( 1.0 );
    t->estimate()->setType( Estimate::Type_Effort );
    createRequest( t, g->resourceAt( 0 ) );

    t->setConstraint( Node::MustStartOn );
    t->setConstraintStartTime( DateTime::fromString( "2011-01-01T11:00:00" ) );

    ScheduleManager *sm = project.createScheduleManager( "Test Plan" );
    project.addScheduleManager( sm );

    QString s = "Calculate forward, Task: MustStartOn ------------------------------";
    qDebug()<<s;
    Debug::print( t, s );


    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( project, sm, true/*nothread*/ );
    }

    Debug::print( &project, s );
    Debug::print( t, s );

    QCOMPARE( t->startTime(), t->constraintStartTime() );

    s = "Calculate forward, 2 Tasks ------------------------------";
    qDebug()<<s;

    Task *t2 = project.createTask();
    t2->setName( "T2" );
    project.addTask( t2, &project );
    t2->estimate()->setUnit( Duration::Unit_d );
    t2->estimate()->setExpectedEstimate( 10.0 );
    t2->estimate()->setType( Estimate::Type_Effort );

    createRequest( t2, g->resourceAt( 0 ) );

    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( project, sm, true/*nothread*/ );
    }

    Debug::print( &project, s );
    Debug::print( t, s );
    Debug::print( t2, s );

    QCOMPARE( t->startTime(), t->constraintStartTime() );

    s = "Calculate backward, 2 Tasks ------------------------------";
    qDebug()<<s;

    sm->setSchedulingDirection( true );

    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( project, sm, true/*nothread*/ );
    }
    Debug::print( &project, s );
    Debug::print( t, s );
    Debug::print( t2, s );

    QCOMPARE( t->startTime(), t->constraintStartTime() );
}

void ProjectTester::startNotEarlier()
{
    Project project;
    project.setId( project.uniqueNodeId() );
    project.registerNodeId( &project );
    project.setConstraintStartTime( DateTime::fromString( "2011-01-01T00:00:00" ) );
    project.setConstraintEndTime( DateTime::fromString( "2011-01-12T00:00:00" ) );

    createCalendar( project );

    ResourceGroup *g = createWorkResources( project, 1 );

    Task *t = project.createTask();
    t->setName( "T1" );
    project.addTask( t, &project );
    t->estimate()->setUnit( Duration::Unit_h );
    t->estimate()->setExpectedEstimate( 1.0 );
    t->estimate()->setType( Estimate::Type_Effort );
    createRequest( t, g->resourceAt( 0 ) );

    t->setConstraint( Node::StartNotEarlier );
    t->setConstraintStartTime( DateTime::fromString( "2011-01-02T11:00:00" ) );

    ScheduleManager *sm = project.createScheduleManager( "Test Plan" );
    project.addScheduleManager( sm );

    QString s = "Calculate forward, Task: StartNotEarlier ------------------------------";
    qDebug()<<s;
    Debug::print( t, s );

    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( project, sm, true/*nothread*/ );
    }

    Debug::print( &project, s );
    Debug::print( t, s );

    QVERIFY( t->startTime() >= t->constraintStartTime() );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 1, 0 ) );

    s = "Calculate forward, 2 Tasks ------------------------------";
    qDebug()<<s;

    Task *t2 = project.createTask();
    t2->setName( "T2" );
    project.addTask( t2, &project );
    t2->estimate()->setUnit( Duration::Unit_d );
    t2->estimate()->setExpectedEstimate( 7.0 );
    t2->estimate()->setType( Estimate::Type_Effort );

    createRequest( t2, g->resourceAt( 0 ) );

    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( project, sm, true/*nothread*/ );
    }

    Debug::print( &project, s );
    Debug::print( t, s );
    Debug::print( t2, s );

    QVERIFY( t->startTime() >= t->constraintStartTime() );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 1, 0 ) );

    s = "Calculate backward, 2 Tasks ------------------------------";
    qDebug()<<s;

    sm->setSchedulingDirection( true );

    {
        KPlatoRCPSPlugin rcps( 0, QVariantList() );
        rcps.calculate( project, sm, true/*nothread*/ );
    }

    Debug::print( &project, s );
    Debug::print( t, s );
    Debug::print( t2, s );
    Debug::printSchedulingLog( *sm, s );
    
    QVERIFY( t->startTime() >= t->constraintStartTime() );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 1, 0 ) );
}

} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::ProjectTester )

#include "ProjectTester.moc"
