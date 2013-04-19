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
#include "TaskJuggler.h"
#include "Allocation.h"
#include "Project.h"
#include "Interval.h"
#include "Task.h"
#include "Resource.h"
#include "CoreAttributesList.h"
#include "Utility.h"
#include "UsageLimits.h"
#include "debug.h"

#include "kptcommand.h"
#include "kptcalendar.h"
#include "kptdatetime.h"
#include "kptresource.h"
#include "kptnode.h"
#include "kpttask.h"
#include "kptschedule.h"

#include <cstdlib>
#include <QDir>
#include <kglobal.h>
#include <klocale.h>
#include <kcalendarsystem.h>
#include <ksystemtimezone.h>
#include <kdatetime.h>
#include <kconfiggroup.h>
#include <QtDBus>

#include <qtest_kde.h>
#include <kdebug.h>

#include "tests/DateTimeTester.h"

#include "tests/debug.cpp"

namespace KPlato
{

void TaskJuggler::initTimezone()
{
    QVERIFY( m_tmp.exists() );

    QFile f;
    f.setFileName( m_tmp.path() + QLatin1String( "/zone.tab" ) );
    f.open(QIODevice::WriteOnly);
    QTextStream fStream(&f);
    fStream << "DE  +5230+01322 Europe/Berlin\n"
               "EG  +3003+03115 Africa/Cairo\n"
               "FR  +4852+00220 Europe/Paris\n"
               "GB  +512830-0001845 Europe/London   Great Britain\n"
               "US  +340308-1181434 America/Los_Angeles Pacific Time\n";
    f.close();
    QDir dir(m_tmp.path() + "/");
    QVERIFY(dir.mkdir("Africa"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/Cairo"), m_tmp.path() + "/" + QLatin1String("Africa/Cairo"));
    QVERIFY(dir.mkdir("America"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/Los_Angeles"), m_tmp.path() + "/" + QLatin1String("America/Los_Angeles"));
    QVERIFY(dir.mkdir("Europe"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/Berlin"), m_tmp.path() + "/" + QLatin1String("Europe/Berlin"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/London"), m_tmp.path() + "/" + QLatin1String("Europe/London"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/Paris"), m_tmp.path() + "/" + QLatin1String("Europe/Paris"));

    // NOTE: QTEST_KDEMAIN_CORE puts the config file in QDir::homePath() + "/.kde-unit-test"
    //       and hence, this is common to all unit tests
    KConfig config("ktimezonedrc");
    KConfigGroup group(&config, "TimeZones");
    group.writeEntry("ZoneinfoDir", m_tmp.name());
    group.writeEntry("Zonetab", QString(m_tmp.name() + QString::fromLatin1("zone.tab")));
    group.writeEntry("LocalZone", QString::fromLatin1("Europe/Berlin"));
    config.sync();
}

void TaskJuggler::cleanupTimezone()
{
}

void TaskJuggler::initTestCase()
{
    DebugCtrl.setDebugLevel(0);
    DebugCtrl.setDebugMode(0xffff);

    initTimezone();
    qDebug()<<"Time zone initiated";
    project = new TJ::Project();
    qDebug()<<"Project created:"<<project;
    project->setScheduleGranularity( TJ::ONEHOUR ); // seconds

    QDateTime dt = QDateTime::fromString( "2011-07-01 08:00:00", Qt::ISODate );
    project->setStart( dt.toTime_t() );
    project->setEnd( dt.addDays(7).addSecs( -1 ).toTime_t() );

    qDebug()<<project->getStart()<<project->getEnd();

    QList<TJ::Interval*> *lst;
    lst = project->getWorkingHours(0);
    QCOMPARE( lst->count(), 0 );
    qDebug()<<"Sunday:"<<lst;
    lst = project->getWorkingHours(1);
    QCOMPARE( lst->count(), 2 );
    qDebug()<<"Monday:"<<lst;
    lst = project->getWorkingHours(2);
    QCOMPARE( lst->count(), 2 );
    qDebug()<<"Tuesday:"<<lst;
    lst = project->getWorkingHours(3);
    QCOMPARE( lst->count(), 2 );
    qDebug()<<"Wednesday:"<<lst;
    lst = project->getWorkingHours(4);
    QCOMPARE( lst->count(), 2 );
    qDebug()<<"Thursday:"<<lst;
    lst = project->getWorkingHours(5);
    QCOMPARE( lst->count(), 2 );
    qDebug()<<"Friday:"<<lst;
    lst = project->getWorkingHours(6);
    QCOMPARE( lst->count(), 0 );
    qDebug()<<"Saturday:"<<lst;
    
    qDebug()<<"finished";
}

void TaskJuggler::cleanupTestCase()
{
    DebugCtrl.setDebugLevel(0);
    DebugCtrl.setDebugMode(0);

    delete project;
    cleanupTimezone();
}

void TaskJuggler::projectTest()
{
}

void TaskJuggler::oneTask()
{
    qDebug();
    TJ::Task *t = new TJ::Task(project, "T1", "T1 name", 0, QString(), 0);
    qDebug()<<"Task added:"<<t;

    QCOMPARE( t->getId(), QString("T1") );
    QCOMPARE( t->getName(), QString("T1 name") );
    qDebug()<<"finished";
    
    int sc = project->getScenarioIndex( "plan" );
    QCOMPARE( sc, 1 );

    t->setDuration( sc-1, TJ::ONEHOUR );
    QCOMPARE( t->getDuration( sc-1 ), (double)TJ::ONEHOUR );
    
    QVERIFY( ! t->isMilestone() );
}

void TaskJuggler::list()
{
    TJ::CoreAttributesList lst;
    TJ::CoreAttributes *a = new TJ::CoreAttributes(project, "A1", "A1 name", 0);
    a->setSequenceNo(1);
    lst.inSort(a);
    a = new TJ::CoreAttributes(project, "A2", "A2 name", 0);
    a->setSequenceNo(3);
    lst.inSort(a);
    
    QCOMPARE( lst.count(), 2);
    QCOMPARE( lst.at(0)->getId(), QString( "A1" ) ); 
    QCOMPARE( lst.at(1)->getId(), QString( "A2" ) ); 

    a = new TJ::CoreAttributes(project, "A3", "A3 name", 0);
    a->setSequenceNo(2);
    lst.inSort(a);

    QCOMPARE( lst.at(0)->getId(), QString( "A1" ) ); 
    QCOMPARE( lst.at(1)->getId(), QString( "A3" ) ); 
    QCOMPARE( lst.at(2)->getId(), QString( "A2" ) ); 

    lst.setSorting(TJ::CoreAttributesList::SequenceDown, 0);
    lst.setSorting(TJ::CoreAttributesList::SequenceDown, 1);
    lst.setSorting(TJ::CoreAttributesList::SequenceDown, 2);
    lst.sort();
    
    QCOMPARE( lst.at(0)->getId(), QString( "A2" ) ); 
    QCOMPARE( lst.at(1)->getId(), QString( "A3" ) ); 
    QCOMPARE( lst.at(2)->getId(), QString( "A1" ) ); 

    lst.setSorting(TJ::CoreAttributesList::IdDown, 0);
    lst.setSorting(TJ::CoreAttributesList::SequenceDown, 1);
    lst.setSorting(TJ::CoreAttributesList::SequenceDown, 2);
    lst.sort();

    QStringList s; foreach(TJ::CoreAttributes *a, lst) s << a->getId();
    qDebug()<<s;
    QCOMPARE( lst.at(0)->getId(), QString( "A3" ) );
    QCOMPARE( lst.at(1)->getId(), QString( "A2" ) );
    QCOMPARE( lst.at(2)->getId(), QString( "A1" ) );

    while ( ! lst.isEmpty() ) {
        delete lst.takeFirst();
    }
}

void TaskJuggler::oneResource()
{
    TJ::Resource *r = new TJ::Resource( project, "R1", "R1 name", 0 );
    r->setEfficiency( 1.0 );
    for (int day = 0; day < 7; ++day) {
        r->setWorkingHours( day, *(project->getWorkingHours(day)) );
        foreach( TJ::Interval *i, *( r->getWorkingHours()[day] ) ) {
            qDebug()<<day<<":"<<*i;
        }
    }
    QCOMPARE( project->resourceCount(), (uint)1 );
}

void TaskJuggler::allocation()
{
    TJ::Task *t = project->getTask( "T1" );
    QVERIFY( t != 0 );
    TJ::Resource *r = project->getResource( "R1" );
    QVERIFY( r != 0 );

    TJ::Allocation *a = new TJ::Allocation();
    a->addCandidate( r );
    QCOMPARE( a->getCandidates().count(), 1 );
    
    t->addAllocation( a );
}

void TaskJuggler::dependency()
{
    int sc = project->getScenarioIndex( "plan" );
    QCOMPARE( sc, 1);

    TJ::Task *m = new TJ::Task( project, "M1", "M1 name", 0, QString(), 0 );
    m->setMilestone( true );
    m->setSpecifiedStart( sc-1, project->getStart() );

    TJ::Task *t = project->getTask( "T1" );
    QVERIFY( t != 0 );

    TJ::TaskDependency *d = t->addDepends( m->getId() );
    QVERIFY( d != 0 );
}

void TaskJuggler::scheduleResource()
{
    QCOMPARE( project->getMaxScenarios(), 1 );

    TJ::Task *t = project->getTask( "T1" );
    QVERIFY( t != 0 );
    t->setDuration( 0, 0.0 );
    t->setEffort( 0, 5. );
    QCOMPARE( t->getEffort( 0 ), 5. );

    qDebug()<<t->getId()<<"effort="<<t->getEffort( 0 );

    QVERIFY(  project->pass2( true )  );

    QVERIFY( project->scheduleAllScenarios() );

    qDebug()<<QDateTime::fromTime_t( t->getStart( 0 ) )<<QDateTime::fromTime_t( t->getEnd( 0 ) );
    
}

void TaskJuggler::scheduleDependencies()
{
    QString s;
    QDateTime pstart = QDateTime::fromString( "2011-07-01 00:00:00", Qt::ISODate );
    QDateTime pend = pstart.addDays(1);
    {
        s = "Test one ALAP milestone --------------------";
        qDebug()<<s;
        TJ::Project *proj = new TJ::Project();
        proj->setScheduleGranularity( TJ::ONEHOUR ); // seconds

        proj->setStart( pstart.toTime_t() );
        proj->setEnd( pend.toTime_t() );

        TJ::Task *m = new TJ::Task(proj, "M1", "M1", 0, QString(), 0);
        m->setMilestone( true );
        m->setScheduling( TJ::Task::ALAP );
        m->setSpecifiedEnd( 0, proj->getEnd() - 1 );

        QVERIFY( proj->pass2( true ) );
        QVERIFY( proj->scheduleAllScenarios() );

        QDateTime mstart = QDateTime::fromTime_t( m->getStart( 0 ) );
        QDateTime mend = QDateTime::fromTime_t( m->getEnd( 0 ) );
        QCOMPARE( mstart, pend );
        QCOMPARE( mend, pend.addSecs( -1 ) );

        delete proj;
        }
        {
        s = "Test one ALAP milestone + one ASAP task --------------------";
        qDebug()<<s;
        TJ::Project *proj = new TJ::Project();
        proj->setScheduleGranularity( TJ::ONEHOUR ); // seconds

        proj->setStart( pstart.toTime_t() );
        proj->setEnd( pend.toTime_t() );

        TJ::Task *m = new TJ::Task(proj, "M1", "M1", 0, QString(), 0);
        m->setMilestone( true );
        m->setScheduling( TJ::Task::ALAP );
        m->setSpecifiedEnd( 0, proj->getEnd() - 1 );

        TJ::Task *t = new TJ::Task(proj, "T1", "T1", 0, QString(), 0);
        t->setDuration( 0, (double)(TJ::ONEHOUR) / TJ::ONEDAY );
        t->setSpecifiedStart( 0, proj->getStart() );

        QVERIFY( proj->pass2( true ) );
        QVERIFY( proj->scheduleAllScenarios() );

        QDateTime tstart = QDateTime::fromTime_t( t->getStart( 0 ) );
        QDateTime tend = QDateTime::fromTime_t( t->getEnd( 0 ) );
        QCOMPARE( tstart, pstart );
        QCOMPARE( tend, pstart.addSecs( TJ::ONEHOUR - 1 ) );

        QDateTime mstart = QDateTime::fromTime_t( m->getStart( 0 ) );
        QDateTime mend = QDateTime::fromTime_t( m->getEnd( 0 ) );
        QCOMPARE( mstart, pend );
        QCOMPARE( mend, pend.addSecs( -1 ) );

        delete proj;
    }
    {
        s = "Test combination of ASAP/ALAP tasks and milestones --------------------";
        qDebug()<<s;
        TJ::Project *proj = new TJ::Project();
        proj->setScheduleGranularity( 300 ); // seconds

        proj->setStart( pstart.toTime_t() );
        proj->setEnd( pend.toTime_t() );

        TJ::Task *t1 = new TJ::Task(proj, "T1", "T1", 0, QString(), 0);
        t1->setScheduling( TJ::Task::ASAP );
        t1->setSpecifiedStart( 0, proj->getStart() );
        t1->setDuration( 0, (double)(TJ::ONEHOUR) / TJ::ONEDAY );

        TJ::Task *t2 = new TJ::Task(proj, "T2", "T2", 0, QString(), 0);
        t2->setScheduling( TJ::Task::ALAP );
        t2->setDuration( 0, (double)(TJ::ONEHOUR) / TJ::ONEDAY );
        t2->setSpecifiedEnd( 0, proj->getEnd() - 1 );
    //     m->addPrecedes( t->getId() );

        TJ::Task *m1 = new TJ::Task(proj, "M1", "M1", 0, QString(), 0);
        m1->setMilestone( true );
        m1->setScheduling( TJ::Task::ASAP );
        m1->addDepends( t1->getId() );
        m1->addPrecedes( t2->getId() );

        TJ::Task *m2 = new TJ::Task(proj, "M2", "M2", 0, QString(), 0);
        m2->setMilestone( true );
        m2->setScheduling( TJ::Task::ALAP );
        m2->addDepends( t1->getId() );
        m2->addPrecedes( t2->getId() );

        TJ::Task *t3 = new TJ::Task(proj, "T3", "T3", 0, QString(), 0);
        t3->setDuration( 0, (double)(TJ::ONEHOUR) / TJ::ONEDAY );
        t3->addPrecedes( m2->getId() );
        t3->setScheduling( TJ::Task::ALAP ); // since t4 is ALAP, this must be ALAP too

        TJ::Task *t4 = new TJ::Task(proj, "T4", "T4", 0, QString(), 0);
        t4->setDuration( 0, (double)(TJ::ONEHOUR) / TJ::ONEDAY );
        t4->addPrecedes( t3->getId() );
        t4->setScheduling( TJ::Task::ALAP );

        QVERIFY( proj->pass2( true ) );
        QVERIFY( proj->scheduleAllScenarios() );

        QDateTime t1end = QDateTime::fromTime_t( t1->getEnd( 0 ) );
        QDateTime t2start = QDateTime::fromTime_t( t2->getStart( 0 ) );
        QDateTime t3start = QDateTime::fromTime_t( t3->getStart( 0 ) );
        QDateTime t3end = QDateTime::fromTime_t( t3->getEnd( 0 ) );
        QDateTime t4end = QDateTime::fromTime_t( t4->getEnd( 0 ) );
        QDateTime m1end = QDateTime::fromTime_t( m1->getEnd( 0 ) );
        QDateTime m2start = QDateTime::fromTime_t( m2->getStart( 0 ) );
        QDateTime m2end = QDateTime::fromTime_t( m2->getEnd( 0 ) );

        QCOMPARE( m1end, t1end );
        QCOMPARE( m2start, t2start );
        QCOMPARE( m2end, t3end );
        QCOMPARE( t3start, t4end.addSecs(1) );

        delete proj;
    }
    {
        DebugCtrl.setDebugLevel(1000);
        DebugCtrl.setDebugMode(7);
        s = "Test sequeces of ASAP/ALAP milestones --------------------";
        qDebug()<<s;
        TJ::Project *proj = new TJ::Project();
        proj->setScheduleGranularity( 300 ); // seconds

        proj->setStart( pstart.toTime_t() );
        proj->setEnd( pend.toTime_t() );

        // First milestone on project start
        TJ::Task *m1 = new TJ::Task(proj, "Start", "Start", 0, QString(), 0);
        m1->setMilestone( true );
        m1->setScheduling( TJ::Task::ASAP );
        m1->setSpecifiedStart( 0, proj->getStart() );

        TJ::Task *m2 = new TJ::Task(proj, "M2-ASAP", "M2", 0, QString(), 0);
        m2->setMilestone( true );
        m2->setScheduling( TJ::Task::ASAP );

        TJ::Task *m3 = new TJ::Task(proj, "M3-ASAP", "M3", 0, QString(), 0);
        m3->setMilestone( true );
        m3->setScheduling( TJ::Task::ASAP );

        TJ::Task *m7 = new TJ::Task(proj, "M/-ASAP", "M7", 0, QString(), 0);
        m7->setMilestone( true );
        m7->setScheduling( TJ::Task::ASAP );

        TJ::Task *m4 = new TJ::Task(proj, "M4-ALAP", "M4", 0, QString(), 0);
        m4->setScheduling( TJ::Task::ALAP );
        m4->setMilestone( true );

        TJ::Task *m5 = new TJ::Task(proj, "M5-ALAP", "M5", 0, QString(), 0);
        m5->setScheduling( TJ::Task::ALAP );
        m5->setMilestone( true );

        TJ::Task *m8 = new TJ::Task(proj, "M8-ALAP", "M8", 0, QString(), 0);
        m8->setScheduling( TJ::Task::ALAP );
        m8->setMilestone( true );

        // ALAP milestone on project end
        TJ::Task *m6 = new TJ::Task(proj, "End", "End", 0, QString(), 0);
        m6->setMilestone( true );
        m6->setScheduling( TJ::Task::ALAP );
        m6->setSpecifiedEnd( 0, proj->getEnd() - 1 );

        m2->addDepends( m1->getId() );
        m3->addDepends( m2->getId() );
        m7->addDepends( m3->getId() );
        m6->addDepends( m7->getId() );

        m1->addPrecedes( m4->getId() );
        m4->addPrecedes( m5->getId() );
        m5->addPrecedes( m8->getId() );
        m8->addPrecedes( m6->getId() );

        QVERIFY2( proj->pass2( true ), s.toLatin1() );
        QVERIFY2( proj->scheduleAllScenarios(), s.toLatin1() );

        qDebug()<<m1<<"->"<<m2<<"->"<<m3<<"->"<<m7<<"->"<<m6;
        qDebug()<<m1<<"->"<<m4<<"->"<<m5<<"->"<<m8<<"->"<<m6;


        QDateTime m1start = QDateTime::fromTime_t( m1->getStart( 0 ) );
        QDateTime m1end = QDateTime::fromTime_t( m1->getEnd( 0 ) );
        QDateTime m2start = QDateTime::fromTime_t( m2->getStart( 0 ) );
        QDateTime m2end = QDateTime::fromTime_t( m2->getEnd( 0 ) );
        QDateTime m3start = QDateTime::fromTime_t( m3->getStart( 0 ) );
        QDateTime m3end = QDateTime::fromTime_t( m3->getEnd( 0 ) );
        QDateTime m4start = QDateTime::fromTime_t( m4->getStart( 0 ) );
        QDateTime m4end = QDateTime::fromTime_t( m4->getEnd( 0 ) );
        QDateTime m5start = QDateTime::fromTime_t( m5->getStart( 0 ) );
        QDateTime m5end = QDateTime::fromTime_t( m5->getEnd( 0 ) );
        QDateTime m6start = QDateTime::fromTime_t( m6->getStart( 0 ) );
        QDateTime m6end = QDateTime::fromTime_t( m6->getEnd( 0 ) );
        QDateTime m7start = QDateTime::fromTime_t( m7->getStart( 0 ) );
        QDateTime m7end = QDateTime::fromTime_t( m7->getEnd( 0 ) );
        QDateTime m8start = QDateTime::fromTime_t( m8->getStart( 0 ) );
        QDateTime m8end = QDateTime::fromTime_t( m8->getEnd( 0 ) );

        QCOMPARE( m1start, pstart );
        QCOMPARE( m2start, m1start );
        QCOMPARE( m3start, m2start );
        QCOMPARE( m7start, m3start );
        QCOMPARE( m6start, pend );
        QCOMPARE( m8end, m6end );
        QCOMPARE( m5end, m8end );
        QCOMPARE( m4end, m5end );
        
        delete proj;
    }
    {
        DebugCtrl.setDebugLevel(1000);
        DebugCtrl.setDebugMode(7);
        s = "Test sequeces of ASAP/ALAP milestones and tasks ----------------";
        qDebug()<<s;
        TJ::Project *proj = new TJ::Project();
        proj->setScheduleGranularity( 300 ); // seconds

        proj->setStart( pstart.toTime_t() );
        proj->setEnd( pend.toTime_t() );

        // First milestone on project start
        TJ::Task *m1 = new TJ::Task(proj, "Start", "Start", 0, QString(), 0);
        m1->setMilestone( true );
        m1->setScheduling( TJ::Task::ASAP );
        m1->setSpecifiedStart( 0, proj->getStart() );

        // ASAP milestone dependent on m1
        TJ::Task *m2 = new TJ::Task(proj, "M2-ASAP", "M2", 0, QString(), 0);
        m2->setMilestone( true );
        m2->setScheduling( TJ::Task::ASAP );

        // ALAP milestone dependent on m1
        TJ::Task *m3 = new TJ::Task(proj, "M3-ALAP", "M3", 0, QString(), 0);
        m3->setMilestone( true );
        m3->setScheduling( TJ::Task::ALAP );

        TJ::Task *t1 = new TJ::Task(proj, "T1-ASAP", "T1", 0, QString(), 0);
        t1->setScheduling( TJ::Task::ASAP );
        t1->setDuration( 0, (double)(TJ::ONEHOUR) / TJ::ONEDAY );

        TJ::Task *t2 = new TJ::Task(proj, "T2-ALAP", "T2", 0, QString(), 0);
        t2->setScheduling( TJ::Task::ALAP );
        t2->setDuration( 0, (double)(TJ::ONEHOUR) / TJ::ONEDAY );

        // ALAP milestone on project end
        TJ::Task *m4 = new TJ::Task(proj, "End", "End", 0, QString(), 0);
        m4->setMilestone( true );
        m4->setScheduling( TJ::Task::ALAP );
        m4->setSpecifiedEnd( 0, proj->getEnd() - 1 );

        m1->addPrecedes( m2->getId() );
        m2->addDepends( m1->getId() );
        m2->addPrecedes( t2->getId() );
        t2->addDepends( m2->getId() ); // t2 (ALAP) depends on ASAP milestone
        t2->addPrecedes( m4->getId() );
        m4->addDepends( t2->getId() );

        m1->addPrecedes( m3->getId() );
        m3->addDepends( m1->getId() );
        m3->addPrecedes( t1->getId() ); // m3 ALAP
        t1->addDepends( m3->getId() ); // t1 (ASAP) depends on ALAP milestone
        t1->addPrecedes( m4->getId() );
        m4->addDepends( t1->getId() );

        QVERIFY2( proj->pass2( true ), s.toLatin1() );
        QVERIFY2( proj->scheduleAllScenarios(), s.toLatin1() );

        qDebug()<<m1->getId()<<"->"<<m2->getId()<<"->"<<t2->getId()<<"->"<<m4->getId();
        qDebug()<<m1->getId()<<"->"<<m3->getId()<<"->"<<t1->getId()<<"->"<<m4->getId();

        QDateTime t1start = QDateTime::fromTime_t( t1->getStart( 0 ) );
        QDateTime t1end = QDateTime::fromTime_t( t1->getEnd( 0 ) );
        QDateTime t2start = QDateTime::fromTime_t( t2->getStart( 0 ) );
        QDateTime t2end = QDateTime::fromTime_t( t2->getEnd( 0 ) );

        QDateTime m1start = QDateTime::fromTime_t( m1->getStart( 0 ) );
        QDateTime m1end = QDateTime::fromTime_t( m1->getEnd( 0 ) );
        QDateTime m2start = QDateTime::fromTime_t( m2->getStart( 0 ) );
        QDateTime m2end = QDateTime::fromTime_t( m2->getEnd( 0 ) );
        QDateTime m3start = QDateTime::fromTime_t( m3->getStart( 0 ) );
        QDateTime m3end = QDateTime::fromTime_t( m3->getEnd( 0 ) );
        QDateTime m4start = QDateTime::fromTime_t( m4->getStart( 0 ) );
        QDateTime m4end = QDateTime::fromTime_t( m4->getEnd( 0 ) );

        QCOMPARE( m1start, pstart );
        QCOMPARE( m2start, m1start );
        QCOMPARE( m3start, t1start );
        QCOMPARE( t2end, m4end );
        QCOMPARE( m4start, pend );
        
        delete proj;
    }
    {
        DebugCtrl.setDebugLevel(1000);
        DebugCtrl.setDebugMode(7);
        s = "Test backwards ----------------";
        qDebug()<<s;
        TJ::Project *proj = new TJ::Project();
        proj->setScheduleGranularity( 300 ); // seconds

        proj->setStart( pstart.toTime_t() );
        proj->setEnd( pend.toTime_t() );

        // First an ASAP milestone on project start to precede m2
        TJ::Task *m1 = new TJ::Task(proj, "Start", "Start", 0, QString(), 0);
        m1->setMilestone( true );
        m1->setScheduling( TJ::Task::ASAP );
        m1->setSpecifiedStart( 0, proj->getStart() );

        // ALAP milestone dependent on m1 to simulate bacwards
        TJ::Task *m2 = new TJ::Task(proj, "M2-ALAP", "M2", 0, QString(), 0);
        m2->setMilestone( true );
        m2->setScheduling( TJ::Task::ASAP );

        // Then the "project"
        TJ::Task *m3 = new TJ::Task(proj, "M3-ASAP", "M3", 0, QString(), 0);
        m3->setMilestone( true );
        m3->setScheduling( TJ::Task::ASAP );

        TJ::Task *t1 = new TJ::Task(proj, "T1-ASAP", "T1", 0, QString(), 0);
        t1->setScheduling( TJ::Task::ASAP );
        t1->setDuration( 0, (double)(TJ::ONEHOUR) / TJ::ONEDAY );

        TJ::Task *t2 = new TJ::Task(proj, "T2-ALAP", "T2", 0, QString(), 0);
        t2->setScheduling( TJ::Task::ALAP );
        t2->setDuration( 0, (double)(TJ::ONEHOUR) / TJ::ONEDAY );

        // Then an ALAP milestone on project end
        TJ::Task *m4 = new TJ::Task(proj, "End", "End", 0, QString(), 0);
        m4->setMilestone( true );
        m4->setScheduling( TJ::Task::ALAP );
        m4->setSpecifiedEnd( 0, proj->getEnd() - 1 );

        m1->addPrecedes( m2->getId() );
        m2->addPrecedes( m3->getId() );
        m3->addDepends( m2->getId() );
        t1->addDepends( m3->getId() );
        t1->addPrecedes( t2->getId() );
        t2->addPrecedes( m4->getId() );

        qDebug()<<m1<<"->"<<m2<<"->"<<m3<<"->"<<t1<<"->"<<t2<<"->"<<m4;

        QVERIFY2( proj->pass2( true ), s.toLatin1() );
        QVERIFY2( proj->scheduleAllScenarios(), s.toLatin1() );

        qDebug()<<m1<<"->"<<m2<<"->"<<m3<<"->"<<t1<<"->"<<t2<<"->"<<m4;

        QDateTime t1start = QDateTime::fromTime_t( t1->getStart( 0 ) );
        QDateTime t1end = QDateTime::fromTime_t( t1->getEnd( 0 ) );
        QDateTime t2start = QDateTime::fromTime_t( t2->getStart( 0 ) );
        QDateTime t2end = QDateTime::fromTime_t( t2->getEnd( 0 ) );

        QDateTime m1start = QDateTime::fromTime_t( m1->getStart( 0 ) );
        QDateTime m1end = QDateTime::fromTime_t( m1->getEnd( 0 ) );
        QDateTime m2start = QDateTime::fromTime_t( m2->getStart( 0 ) );
        QDateTime m2end = QDateTime::fromTime_t( m2->getEnd( 0 ) );
        QDateTime m3start = QDateTime::fromTime_t( m3->getStart( 0 ) );
        QDateTime m3end = QDateTime::fromTime_t( m3->getEnd( 0 ) );
        QDateTime m4start = QDateTime::fromTime_t( m4->getStart( 0 ) );
        QDateTime m4end = QDateTime::fromTime_t( m4->getEnd( 0 ) );

        QCOMPARE( m1start, pstart );
        QCOMPARE( m2start, m3start );
        QCOMPARE( m3start, t1start );
        QCOMPARE( t2end, m4end );
        QCOMPARE( m4start, pend );
        
        delete proj;
    }
}

void TaskJuggler::scheduleConstraints()
{
    DebugCtrl.setDebugMode( 0 );
    DebugCtrl.setDebugLevel( 100 );

    QString s;
    QDateTime pstart = QDateTime::fromString( "2011-07-01 09:00:00", Qt::ISODate );
    QDateTime pend = pstart.addDays(1);
    {
        s = "Test MustStartOn --------------------";
        qDebug()<<s;
        TJ::Project *proj = new TJ::Project();
        proj->setScheduleGranularity( TJ::ONEHOUR / 2 );

        proj->setStart( pstart.toTime_t() );
        proj->setEnd( pend.toTime_t() );

        TJ::Resource *r = new TJ::Resource( proj, "R1", "R1", 0 );
        r->setEfficiency( 1.0 );
        for (int day = 0; day < 7; ++day) {
            r->setWorkingHours( day, *(proj->getWorkingHours(day)) );
        }

        TJ::Task *m = new TJ::Task(proj, "M1", "M1", 0, QString(), 0);
        m->setMilestone( true );
        m->setScheduling( TJ::Task::ASAP );
        m->setSpecifiedStart( 0, proj->getStart() );

        TJ::Task *t1 = new TJ::Task(proj, "T1", "T1", 0, QString(), 0);
        t1->setSpecifiedStart( 0, proj->getStart() + TJ::ONEHOUR );
        t1->setEffort( 0, 1.0/24.0 );
        TJ::Allocation *a = new TJ::Allocation();
        a->addCandidate( r );
        t1->addAllocation( a );

        QVERIFY( proj->pass2( true ) );
        QVERIFY( proj->scheduleAllScenarios() );

        QDateTime mstart = QDateTime::fromTime_t( m->getStart( 0 ) );
        QDateTime mend = QDateTime::fromTime_t( m->getEnd( 0 ) );
        QDateTime t1start = QDateTime::fromTime_t( t1->getStart( 0 ) );
        QDateTime t1end = QDateTime::fromTime_t( t1->getEnd( 0 ) );
        QCOMPARE( mstart, pstart );
        QCOMPARE( t1start, mstart.addSecs( TJ::ONEHOUR ) );

        delete proj;
    }
    {
        s = "Test one MustStartOn + StartNotEarlier on same time -------------";
        qDebug()<<s;
        TJ::Project *proj = new TJ::Project();
        proj->setScheduleGranularity( TJ::ONEHOUR / 2 );

        proj->setStart( pstart.toTime_t() );
        proj->setEnd( pend.toTime_t() );

        TJ::Resource *r = new TJ::Resource( proj, "R1", "R1", 0 );
        r->setEfficiency( 1.0 );
        for (int day = 0; day < 7; ++day) {
            r->setWorkingHours( day, *(proj->getWorkingHours(day)) );
        }

        TJ::Task *m = new TJ::Task(proj, "M1", "M1", 0, QString(), 0);
        m->setMilestone( true );
        m->setScheduling( TJ::Task::ASAP );
        m->setSpecifiedStart( 0, proj->getStart() );

        TJ::Task *t1 = new TJ::Task(proj, "T1", "T1", 0, QString(), 0);
        t1->setPriority( 600 ); // high prio so it is likely it will be scheduled on time
        t1->setSpecifiedStart( 0, proj->getStart() + TJ::ONEHOUR );
        t1->setEffort( 0, 1.0/24.0 );
        TJ::Allocation *a = new TJ::Allocation();
        a->addCandidate( r );
        t1->addAllocation( a );

        TJ::Task *t2 = new TJ::Task(proj, "T2", "T2", 0, QString(), 0);
        t2->setPriority( 500 ); // less than t1
        t2->setSpecifiedStart( 0, proj->getStart() + TJ::ONEHOUR );
        t2->setEffort( 0, 1.0/24.0 );
        a = new TJ::Allocation();
        a->addCandidate( r );
        t2->addAllocation( a );

        m->addPrecedes( t1->getId() );
        t1->addDepends( m->getId() );
        m->addPrecedes( t2->getId() );
        t2->addDepends( m->getId() );

        QVERIFY2( proj->pass2( true ), s.toLatin1() );
        QVERIFY2( proj->scheduleAllScenarios(), s.toLatin1() );

        QDateTime mstart = QDateTime::fromTime_t( m->getStart( 0 ) );
        QDateTime mend = QDateTime::fromTime_t( m->getEnd( 0 ) );
        QDateTime t1start = QDateTime::fromTime_t( t1->getStart( 0 ) );
        QDateTime t1end = QDateTime::fromTime_t( t1->getEnd( 0 ) );
        QDateTime t2start = QDateTime::fromTime_t( t2->getStart( 0 ) );
        QDateTime t2end = QDateTime::fromTime_t( t2->getEnd( 0 ) );
        QCOMPARE( mstart, pstart );
        QCOMPARE( t1start, mstart.addSecs( TJ::ONEHOUR ) );
        QCOMPARE( t2start, t1end.addSecs( 1 ) );

        delete proj;
    }
    {
        s = "Test one MustStartOn + StartNotEarlier overlapping -------------";
        qDebug()<<s;
        TJ::Project *proj = new TJ::Project();
        proj->setScheduleGranularity( TJ::ONEHOUR / 4 );

        proj->setStart( pstart.toTime_t() );
        proj->setEnd( pend.toTime_t() );

        TJ::Resource *r = new TJ::Resource( proj, "R1", "R1", 0 );
        r->setEfficiency( 1.0 );
        for (int day = 0; day < 7; ++day) {
            r->setWorkingHours( day, *(proj->getWorkingHours(day)) );
        }

        TJ::Task *m = new TJ::Task(proj, "M1", "M1", 0, QString(), 0);
        m->setMilestone( true );
        m->setScheduling( TJ::Task::ASAP );
        m->setSpecifiedStart( 0, proj->getStart() );

        TJ::Task *t1 = new TJ::Task(proj, "T1", "T1", 0, QString(), 0);
        t1->setPriority( 600 ); // high prio so it is likely it will be scheduled on time
        t1->setSpecifiedStart( 0, proj->getStart() + TJ::ONEHOUR );
        t1->setEffort( 0, 1.0 / proj->getDailyWorkingHours() );
        TJ::Allocation *a = new TJ::Allocation();
        a->addCandidate( r );
        t1->addAllocation( a );

        TJ::Task *t2 = new TJ::Task(proj, "T2", "T2", 0, QString(), 0);
        t2->setPriority( 500 ); // less than t1
        t2->setSpecifiedStart( 0, proj->getStart() + TJ::ONEHOUR / 2 );
        t2->setEffort( 0, 1.0 / proj->getDailyWorkingHours() );
        a = new TJ::Allocation();
        a->addCandidate( r );
        t2->addAllocation( a );

        m->addPrecedes( t1->getId() );
        t1->addDepends( m->getId() );
        m->addPrecedes( t2->getId() );
        t2->addDepends( m->getId() );

        QVERIFY2( proj->pass2( true ), s.toLatin1() );
        QVERIFY2( proj->scheduleAllScenarios(), s.toLatin1() );

        QDateTime mstart = QDateTime::fromTime_t( m->getStart( 0 ) );
        QDateTime mend = QDateTime::fromTime_t( m->getEnd( 0 ) );
        QDateTime t1start = QDateTime::fromTime_t( t1->getStart( 0 ) );
        QDateTime t1end = QDateTime::fromTime_t( t1->getEnd( 0 ) );
        QDateTime t2start = QDateTime::fromTime_t( t2->getStart( 0 ) );
        QDateTime t2end = QDateTime::fromTime_t( t2->getEnd( 0 ) );
        QCOMPARE( mstart, pstart );
        QCOMPARE( t1start, mstart.addSecs( TJ::ONEHOUR ) );
        QCOMPARE( t2start, mstart.addSecs( TJ::ONEHOUR / 2 ) );
        QCOMPARE( t2end, t1end.addSecs( TJ::ONEHOUR / 2 ) );

        delete proj;
    }
    {
        s = "Fixed interval with/without allocation-------------";
        qDebug()<<s;
        TJ::Project *proj = new TJ::Project();
        proj->setScheduleGranularity( TJ::ONEHOUR / 4 );

        proj->setStart( pstart.toTime_t() );
        proj->setEnd( pend.toTime_t() );

        TJ::Resource *r = new TJ::Resource( proj, "R1", "R1", 0 );
        r->setEfficiency( 1.0 );
        for (int day = 0; day < 7; ++day) {
            r->setWorkingHours( day, *(proj->getWorkingHours(day)) );
        }

        TJ::Task *m = new TJ::Task(proj, "M1", "M1", 0, QString(), 0);
        m->setMilestone( true );
        m->setScheduling( TJ::Task::ASAP );
        m->setSpecifiedStart( 0, proj->getStart() );

        TJ::Task *t1 = new TJ::Task(proj, "T1", "T1", 0, QString(), 0);
        t1->setPriority( 600 ); // high prio so it is likely it will be scheduled on time
        t1->setSpecifiedStart( 0, proj->getStart() + TJ::ONEHOUR );
        t1->setSpecifiedEnd( 0, proj->getStart() + ( 2*TJ::ONEHOUR) -1 );
        TJ::Allocation *a = new TJ::Allocation();
        a->addCandidate( r );
        t1->addAllocation( a );

        TJ::Task *t2 = new TJ::Task(proj, "T2", "T2", 0, QString(), 0);
        t2->setPriority( 500 ); // less than t1
        t2->setSpecifiedStart( 0, proj->getStart() + TJ::ONEHOUR );
        t1->setSpecifiedEnd( 0, proj->getStart() + ( 2*TJ::ONEHOUR) -1 );
        a = new TJ::Allocation();
        a->addCandidate( r );
        t2->addAllocation( a );

        m->addPrecedes( t1->getId() );
        t1->addDepends( m->getId() );
        m->addPrecedes( t2->getId() );
        t2->addDepends( m->getId() );

        QVERIFY2( proj->pass2( true ), s.toLatin1() );
        QVERIFY2( proj->scheduleAllScenarios(), s.toLatin1() );

        QDateTime mstart = QDateTime::fromTime_t( m->getStart( 0 ) );
        QDateTime mend = QDateTime::fromTime_t( m->getEnd( 0 ) );
        QDateTime t1start = QDateTime::fromTime_t( t1->getStart( 0 ) );
        QDateTime t1end = QDateTime::fromTime_t( t1->getEnd( 0 ) );
        QDateTime t2start = QDateTime::fromTime_t( t2->getStart( 0 ) );
        QDateTime t2end = QDateTime::fromTime_t( t2->getEnd( 0 ) );
        QCOMPARE( mstart, pstart );
        QCOMPARE( t1start, mstart.addSecs( TJ::ONEHOUR ) );
        QCOMPARE( t2start, mstart.addSecs( TJ::ONEHOUR ) );

        delete proj;
    }
}

void TaskJuggler::resourceConflict()
{
    DebugCtrl.setDebugLevel( 0 );
    DebugCtrl.setDebugMode( 0xffff );

    QString s;
    QDateTime pstart = QDateTime::fromString( "2011-07-04 09:00:00", Qt::ISODate );
    QDateTime pend = pstart.addDays(1);
    {
        s = "Test 2 tasks, allocate same resource --------------------";
        qDebug()<<s;
        TJ::Project *proj = new TJ::Project();
        proj->setScheduleGranularity( TJ::ONEHOUR ); // seconds

        proj->setStart( pstart.toTime_t() );
        proj->setEnd( pend.toTime_t() );

        QCOMPARE( QDateTime::fromTime_t( proj->getStart() ), pstart );

        TJ::Resource *r = new TJ::Resource( proj, "R1", "R1", 0 );
        r->setEfficiency( 1.0 );
        for (int day = 0; day < 7; ++day) {
            r->setWorkingHours( day, *(proj->getWorkingHours(day)) );
        }

        TJ::Task *m = new TJ::Task(proj, "M1", "M1", 0, QString(), 0);
        m->setMilestone( true );
        m->setScheduling( TJ::Task::ASAP );
        m->setSpecifiedStart( 0, proj->getStart() );

        TJ::Task *t1 = new TJ::Task(proj, "T1", "T1", 0, QString(), 0);
        t1->setPriority( 100 ); // this should be scheduled before t2
        t1->setEffort( 0, 1.0/24.0 );
        TJ::Allocation *a = new TJ::Allocation();
        a->addCandidate( r );
        t1->addAllocation( a );

        TJ::Task *t2 = new TJ::Task(proj, "T2", "T2", 0, QString(), 0);
        t2->setPriority( 10 );
        t2->setEffort( 0, 1.0/24.0 );
        a = new TJ::Allocation();
        a->addCandidate( r );
        t2->addAllocation( a );

        m->addPrecedes( t1->getId() );
        t1->addDepends( m->getId() );
        m->addPrecedes( t2->getId() );
        t2->addDepends( m->getId() );

        QVERIFY2( proj->pass2( true ), s.toLatin1() );
        QVERIFY2( proj->scheduleAllScenarios(), s.toLatin1() );

        QDateTime mstart = QDateTime::fromTime_t( m->getStart( 0 ) );
        QDateTime mend = QDateTime::fromTime_t( m->getEnd( 0 ) );
        QDateTime t1start = QDateTime::fromTime_t( t1->getStart( 0 ) );
        QDateTime t1end = QDateTime::fromTime_t( t1->getEnd( 0 ) );
        QDateTime t2start = QDateTime::fromTime_t( t2->getStart( 0 ) );
        QDateTime t2end = QDateTime::fromTime_t( t2->getEnd( 0 ) );
        QCOMPARE( mstart, pstart );
        QCOMPARE( t1start, mstart );
        QCOMPARE( t2start, t1end.addSecs( 1 ) );

        delete proj;
    }
}

void TaskJuggler::units()
{
    DebugCtrl.setDebugLevel( 1000 );
    DebugCtrl.setDebugMode( TSDEBUG + RSDEBUG );

    QString s;
    QDateTime pstart = QDateTime::fromString( "2011-07-04 09:00:00", Qt::ISODate );
    QDateTime pend = pstart.addDays(3);
    {
        s = "Test one task, resource 50% using resource limit --------------------";
        qDebug()<<s;
        TJ::Project *proj = new TJ::Project();
        proj->setScheduleGranularity( TJ::ONEHOUR );

        proj->setStart( pstart.toTime_t() );
        proj->setEnd( pend.toTime_t() );

        QCOMPARE( QDateTime::fromTime_t( proj->getStart() ), pstart );

        TJ::Resource *r = new TJ::Resource( proj, "R1", "R1", 0 );
        TJ::UsageLimits *l = new TJ::UsageLimits();
        l->setDailyUnits( 50 );
        r->setLimits( l );
        r->setEfficiency( 1.0 );
        for (int day = 0; day < 7; ++day) {
            r->setWorkingHours( day, *(proj->getWorkingHours(day)) );
        }

        TJ::Task *t1 = new TJ::Task(proj, "T1", "T1", 0, QString(), 0);
        t1->setSpecifiedStart( 0, proj->getStart() );
        t1->setEffort( 0, 1.0 );
        TJ::Allocation *a = new TJ::Allocation();
        a->addCandidate( r );
        t1->addAllocation( a );

        QVERIFY2( proj->pass2( true ), s.toLatin1() );
        QVERIFY2( proj->scheduleAllScenarios(), s.toLatin1() );

        QDateTime t1start = QDateTime::fromTime_t( t1->getStart( 0 ) );
        QDateTime t1end = QDateTime::fromTime_t( t1->getEnd( 0 ) );

        // working hours: 09:00 - 12:00, 13:00 - 18:00
        QCOMPARE( t1start, pstart );
        QCOMPARE( t1end, t1start.addDays( 1 ).addSecs( 5 * TJ::ONEHOUR - 1) ); // remember lunch

        delete proj;
    }
    {
        s = "Test one task, resource 50% using resource efficiency --------------------";
        qDebug()<<s;
        TJ::Project *proj = new TJ::Project();
        proj->setScheduleGranularity( TJ::ONEHOUR / 2 );

        proj->setStart( pstart.toTime_t() );
        proj->setEnd( pend.toTime_t() );

        QCOMPARE( QDateTime::fromTime_t( proj->getStart() ), pstart );

        TJ::Resource *r = new TJ::Resource( proj, "R1", "R1", 0 );
        r->setEfficiency( 0.5 );
        for (int day = 0; day < 7; ++day) {
            r->setWorkingHours( day, *(proj->getWorkingHours(day)) );
        }

        TJ::Task *t1 = new TJ::Task(proj, "T1", "T1", 0, QString(), 0);
        t1->setSpecifiedStart( 0, proj->getStart() );
        t1->setEffort( 0, 1.0 / proj->getDailyWorkingHours() );
        TJ::Allocation *a = new TJ::Allocation();
        a->addCandidate( r );
        t1->addAllocation( a );

        QVERIFY2( proj->pass2( true ), s.toLatin1() );
        QVERIFY2( proj->scheduleAllScenarios(), s.toLatin1() );

        QDateTime t1start = QDateTime::fromTime_t( t1->getStart( 0 ) );
        QDateTime t1end = QDateTime::fromTime_t( t1->getEnd( 0 ) );

        // working hours: 09:00 - 12:00, 13:00 - 18:00
        QCOMPARE( t1start, pstart );
        QCOMPARE( t1end, t1start.addSecs( 2 * TJ::ONEHOUR - 1) );

        delete proj;
    }
    {
        s = "Test one task, allocation limit 50% per day --------------------";
        qDebug()<<s;
        TJ::Project *proj = new TJ::Project();
        proj->setScheduleGranularity( TJ::ONEHOUR / 2 );

        proj->setStart( pstart.toTime_t() );
        proj->setEnd( pend.toTime_t() );

        QCOMPARE( QDateTime::fromTime_t( proj->getStart() ), pstart );

        TJ::Resource *r = new TJ::Resource( proj, "R1", "R1", 0 );
        r->setEfficiency( 1.0 );
        for (int day = 0; day < 7; ++day) {
            r->setWorkingHours( day, *(proj->getWorkingHours(day)) );
        }

        TJ::Task *t1 = new TJ::Task(proj, "T1", "T1", 0, QString(), 0);
        t1->setSpecifiedStart( 0, proj->getStart() );
        t1->setEffort( 0, 1.0 );
        TJ::Allocation *a = new TJ::Allocation();
        a->addCandidate( r );
        TJ::UsageLimits *l = new TJ::UsageLimits();
        l->setDailyUnits( 50 );
        a->setLimits( l );
        t1->addAllocation( a );

        QVERIFY2( proj->pass2( true ), s.toLatin1() );
        QVERIFY2( proj->scheduleAllScenarios(), s.toLatin1() );

        QDateTime t1start = QDateTime::fromTime_t( t1->getStart( 0 ) );
        QDateTime t1end = QDateTime::fromTime_t( t1->getEnd( 0 ) );

        // working hours: 09:00 - 12:00, 13:00 - 18:00
        QCOMPARE( t1start, pstart );
        QCOMPARE( t1end, t1start.addDays( 1 ).addSecs( 5 * TJ::ONEHOUR - 1) ); // remember lunch

        delete proj;
    }
}

} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::TaskJuggler )

#include "TaskJuggler.moc"

