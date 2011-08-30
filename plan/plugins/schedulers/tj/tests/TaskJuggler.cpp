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
    cleanupTimezone();
    mDataDir = QDir::homePath() + "/.tj-unit-test/projecttest";
    QVERIFY(QDir().mkpath(mDataDir));
    QFile f;
    f.setFileName(mDataDir + QLatin1String("/zone.tab"));
    f.open(QIODevice::WriteOnly);
    QTextStream fStream(&f);
    fStream << "DE      +5230+01322     Europe/Berlin\n"
               "EG  +3003+03115 Africa/Cairo\n"
               "FR  +4852+00220 Europe/Paris\n"
               "GB  +512830-0001845 Europe/London   Great Britain\n"
               "US  +340308-1181434 America/Los_Angeles Pacific Time\n";
    f.close();
    QDir dir(mDataDir);
    QVERIFY(dir.mkdir("Africa"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/Cairo"), mDataDir + QLatin1String("/Africa/Cairo"));
    QVERIFY(dir.mkdir("America"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/Los_Angeles"), mDataDir + QLatin1String("/America/Los_Angeles"));
    QVERIFY(dir.mkdir("Europe"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/Berlin"), mDataDir + QLatin1String("/Europe/Berlin"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/London"), mDataDir + QLatin1String("/Europe/London"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("/Paris"), mDataDir + QLatin1String("/Europe/Paris"));

    KConfig config("ktimezonedrc");
    KConfigGroup group(&config, "TimeZones");
    group.writeEntry("ZoneinfoDir", mDataDir);
    group.writeEntry("Zonetab", mDataDir + QString::fromLatin1("/zone.tab"));
    group.writeEntry("LocalZone", QString::fromLatin1("Europe/Berlin"));
    config.sync();
}

void TaskJuggler::cleanupTimezone()
{
    removeDir(QLatin1String("projecttest/Africa"));
    removeDir(QLatin1String("projecttest/America"));
    removeDir(QLatin1String("projecttest/Europe"));
    removeDir(QLatin1String("projecttest"));
    removeDir(QLatin1String("share/config"));
    QDir().rmpath(QDir::homePath() + "/.tj-unit-test/share");
}

void TaskJuggler::removeDir(const QString &subdir)
{
    QDir local = QDir::homePath() + QLatin1String("/.tj-unit-test/") + subdir;
    foreach(const QString &file, local.entryList(QDir::Files)) {
        if(!local.remove(file)) {
            qWarning("%s: removing failed", qPrintable( file ));
        }
    }
    QCOMPARE((int)local.entryList(QDir::Files).count(), 0);
    local.cdUp();
    QString subd = subdir;
    subd.remove(QRegExp("^.*/"));
    local.rmpath(subd);
}

void TaskJuggler::initTestCase()
{
    DebugCtrl.setDebugLevel(0);
    DebugCtrl.setDebugMode(0xffff);

    initTimezone();
    qDebug()<<"Time zone initiated";
    project = new TJ::Project();
    qDebug()<<"Project created:"<<project;
    project->setScheduleGranularity( 60 ); // seconds

    QDateTime dt = QDateTime::fromString( "2011-07-01 08:00:00", Qt::ISODate );
    project->setStart(dt.toTime_t());
    project->setEnd(dt.addDays(7).toTime_t());

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

void TaskJuggler::schedule()
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

} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::TaskJuggler )

#include "TaskJuggler.moc"
