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
#include "Interval.h"
#include "Utility.h"
#include "debug.h"

#include "kptcommand.h"
#include "kptcalendar.h"
#include "kptdatetime.h"
#include "kptresource.h"
#include "kptnode.h"
#include "kpttask.h"
#include "kptschedule.h"

#include <cstdlib>
#include <qtest_kde.h>
#include <QtCore/QDir>
#include <kglobal.h>
#include <klocale.h>
#include <kcalendarsystem.h>
#include <ksystemtimezone.h>
#include <kdatetime.h>
#include <kconfiggroup.h>
#include <QtDBus/QtDBus>

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
    qDebug()<<this<<project;
    delete project;
    qDebug()<<"project deleted";
    cleanupTimezone();
    qDebug()<<"finished";
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
    
    int sc = project->getScenarioIndex( "Plan" );
    t->setDuration( sc, TJ::ONEHOUR );
    QCOMPARE( t->getDuration( sc ), (double)TJ::ONEHOUR );
    
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
    for ( int i = 0; i < project->getMaxScenarios(); ++i ) {
        t->setEffort( i, 5 );
        QCOMPARE( t->getEffort( i ), 5. );
    }
    QVERIFY( ! t->isMilestone() );
    for ( int i = 0; i < project->getMaxScenarios(); ++i ) {
        qDebug()<<t->getId()<<"effort="<<t->getEffort( i )<<"sc="<<i;
    }
    bool res = project->pass2( true );
    QVERIFY( res );

    QVERIFY( ! t->isMilestone() );

    res = project->scheduleAllScenarios();
    QVERIFY( res );

    qDebug()<<QDateTime::fromTime_t( t->getStart( 0 ) )<<QDateTime::fromTime_t( t->getEnd( 0 ) );
    
}

void TaskJuggler::scheduleConstraints()
{
    QString s = "Test one ALAP milestone --------------------";
    qDebug()<<s;
    TJ::Project *proj = new TJ::Project();
    proj->setScheduleGranularity( TJ::ONEHOUR ); // seconds

    QDateTime pstart = QDateTime::fromString( "2011-07-01 00:00:00", Qt::ISODate );
    QDateTime pend = pstart.addDays(1);
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

    s = "Test one ALAP milestone + one ASAP task --------------------";
    qDebug()<<s;
    proj = new TJ::Project();
    proj->setScheduleGranularity( TJ::ONEHOUR ); // seconds

    proj->setStart( pstart.toTime_t() );
    proj->setEnd( pend.toTime_t() );

    m = new TJ::Task(proj, "M1", "M1", 0, QString(), 0);
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

    mstart = QDateTime::fromTime_t( m->getStart( 0 ) );
    mend = QDateTime::fromTime_t( m->getEnd( 0 ) );
    QCOMPARE( mstart, pend );
    QCOMPARE( mend, pend.addSecs( -1 ) );

    delete proj;

    s = "Test combination of ASAP/ALAP tasks and milestones --------------------";
    qDebug()<<s;
    proj = new TJ::Project();
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

}

} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::TaskJuggler )

#include "TaskJuggler.moc"
