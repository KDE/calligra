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

#include <QTest>

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

void SchedulerTester::test()
{
    QString dir = QFINDTESTDATA("data/");
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
qDebug() << "+++++++++++++++++++++++++++calculate-start";
    tj.calculate( project, sm, true/*nothread*/ );
qDebug() << "+++++++++++++++++++++++++++calculate-end";
    
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

QTEST_GUILESS_MAIN( KPlato::SchedulerTester )
