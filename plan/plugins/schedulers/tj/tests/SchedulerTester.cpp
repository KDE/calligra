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
#include "SchedulerTester.h"

#include "PlanTJPlugin.h"

#include "kptcommand.h"
#include "kptcalendar.h"
#include "kptdatetime.h"
#include "kptresource.h"
#include "kptnode.h"
#include "kpttask.h"
#include "kptschedule.h"
#include "kptxmlloaderobject.h"

#include <cstdlib>
#include <qtest_kde.h>
#include <QDir>
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

QStringList SchedulerTester::data()
{
    return QStringList()
            << "test1.plan"
            << "startnotearlier.plan"
            ;
}

void SchedulerTester::initTimezone()
{
    QString kdehome = getenv("KDEHOME");
    QDir d(kdehome);
    d.mkpath("tjtest");
    d.cd("tjtest");

    QString dataDir = d.path();
    qDebug()<<dataDir;
    QFile f;
    f.setFileName(dataDir + QLatin1String( "/zone.tab" ) );
    f.open(QIODevice::WriteOnly);
    QTextStream fStream(&f);
    fStream << "DE  +5230+01322 Europe/Berlin\n"
               "EG  +3003+03115 Africa/Cairo\n"
               "FR  +4852+00220 Europe/Paris\n"
               "GB  +512830-0001845 Europe/London   Great Britain\n"
               "US  +340308-1181434 America/Los_Angeles Pacific Time\n";
    f.close();
    QVERIFY(QFile::exists( dataDir + QLatin1String("/zone.tab")));
    QDir dir(dataDir);
    QVERIFY(dir.mkdir("Africa"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("zoneinfo/Cairo"), dataDir + QLatin1String("/Africa/Cairo"));
    QVERIFY(QFile::exists( dataDir + QLatin1String("/Africa/Cairo")));
    QVERIFY(dir.mkdir("America"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("zoneinfo/Los_Angeles"), dataDir + QLatin1String("/America/Los_Angeles"));
    QVERIFY(QFile::exists( dataDir + QLatin1String("/America/Los_Angeles")));
    QVERIFY(dir.mkdir("Europe"));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("zoneinfo/Berlin"), dataDir + QLatin1String("/Europe/Berlin"));
    QVERIFY(QFile::exists( dataDir + QLatin1String("/Europe/Berlin")));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("zoneinfo/London"),dataDir + QLatin1String("/Europe/London"));
    QVERIFY(QFile::exists( dataDir + QLatin1String("/Europe/London")));
    QFile::copy(QString::fromLatin1(KDESRCDIR) + QLatin1String("zoneinfo/Paris"), dataDir + QLatin1String("/Europe/Paris"));
    QVERIFY(QFile::exists( dataDir + QLatin1String("/Europe/Paris")));

    // NOTE: QTEST_KDEMAIN_CORE puts the config file in QDir::homePath() + "/.kde-unit-test"
    //       and hence, this is common to all unit tests
    KConfig config("ktimezonedrc");
    KConfigGroup group(&config, "TimeZones");
    group.writeEntry("ZoneinfoDir", dataDir);
    group.writeEntry("Zonetab", dataDir + QString::fromLatin1("/zone.tab"));
    group.writeEntry("LocalZone", QString::fromLatin1("Europe/Berlin"));
    config.sync();
}

void SchedulerTester::cleanupTimezone()
{
    QString kdehome = getenv("KDEHOME");
    removeDir( kdehome + "/tjtest/Africa" );
    removeDir( kdehome + "/tjtest/America" );
    removeDir( kdehome + "/tjtest/Europe" );
    removeDir( kdehome + "/tjtest" );
    removeDir( kdehome + "/share/config" );
    QDir().rmpath(kdehome +"/share/tjtest");
}

void SchedulerTester::removeDir(const QString &dir)
{
    QDir local(dir);
    foreach(const QString &file, local.entryList(QDir::Files))
        if(!local.remove(file))
            qWarning("%s: removing failed", qPrintable( file ));
        QCOMPARE((int)local.entryList(QDir::Files).count(), 0);
    local.cdUp();
    QString subd = dir;
    subd.remove(QRegExp("^.*/"));
    local.rmpath(subd);
}

void SchedulerTester::initTestCase()
{
    initTimezone();
}

void SchedulerTester::cleanupTestCase()
{
    cleanupTimezone();
}

void SchedulerTester::test()
{
    QString dir = QString::fromLatin1( KDESRCDIR ) + "data/";
    foreach ( const QString &fname, data() ) {
        qDebug()<<"Testing file:"<<fname;
        QFile file( dir + fname );
        QVERIFY2( file.open( QIODevice::ReadOnly ), fname.toLatin1() );

        KoXmlDocument doc;
        QString error;
        bool setContent;
        int line, column;
        if ( ! ( setContent = doc.setContent( &file, &error, &line, &column ) ) ) {
            file.close();
            QString s = QString( "%1: %2 Line %3, column %4" ).arg( fname ).arg( error ).arg( line ).arg( column );
            QVERIFY2( setContent, s.toLatin1() );
        }
        file.close();

        testProject( fname, doc );
    }
}

void SchedulerTester::testProject( const QString &fname, const KoXmlDocument &doc )
{
    
    KoXmlElement pel = doc.documentElement().namedItem( "project" ).toElement();
    if ( pel.isNull() ) {
        QString s = QString( "%1: Cannot find 'project' element" ).arg( fname );
        QVERIFY2( pel.isNull(), s.toLatin1() );        
    }
    Project project;
    XMLLoaderObject status;
    status.setProject( &project );
    status.setVersion( doc.documentElement().attribute( "version", PLAN_FILE_SYNTAX_VERSION ) );
    bool projectLoad = project.load( pel, status );
    if ( ! projectLoad ) {
        QString s = QString( "%1: Failed to load project" ).arg( fname );
        QVERIFY2( projectLoad, s.toLatin1() );        
    }
    QString s = project.description();
    if ( ! s.isEmpty() ) {
        qDebug();
        qDebug()<<project.description();
        qDebug();
    }

    ScheduleManager *manager = project.scheduleManagers().value( 0 );
    s = QString( "%1: No schedule to compare with" ).arg( fname );
    QVERIFY2( manager, s.toLatin1() );

    ScheduleManager *sm = project.createScheduleManager( "Test Plan" );
    project.addScheduleManager( sm );

    PlanTJPlugin tj( 0, QVariantList() );
    tj.calculate( project, sm, true/*nothread*/ );
    
    s = QString( "%1: Scheduling failed" ).arg( fname );
    QVERIFY2( sm->calculationResult() == ScheduleManager::CalculationDone, s.toLatin1() );

    long id1 = manager->scheduleId();
    long id2 = sm->scheduleId();
    s = QString( "%1: Compare project schedules:\n Expected: %2\n   Result: %3" )
            .arg( fname )
            .arg( project.startTime( id1 ).toString( Qt::ISODate ) )
            .arg( project.startTime( id2 ).toString( Qt::ISODate ) );
    QVERIFY2( project.startTime( id1 ) == project.startTime( id2 ), s.toLatin1() ); 
    
    foreach ( Node *n, project.allNodes() ) {
        compare( fname, n, id1, id2 );
    }
}

void SchedulerTester::compare( const QString &fname, Node *n, long id1, long id2 )
{
    QString s = QString( "%1: '%2' Compare task schedules:\n Expected: %3\n   Result: %4" ).arg( fname ).arg( n->name() );
    QVERIFY2( n->startTime( id1 ) == n->startTime( id2 ), (s.arg(n->startTime( id1 ).toString(Qt::ISODate)).arg(n->startTime( id2 ).toString(Qt::ISODate))).toLatin1() );
    QVERIFY2( n->endTime( id1 ) == n->endTime( id2 ), (s.arg(n->endTime( id1 ).toString(Qt::ISODate)).arg(n->endTime( id2 ).toString(Qt::ISODate))).toLatin1() );
}

} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::SchedulerTester )

#include "SchedulerTester.moc"

