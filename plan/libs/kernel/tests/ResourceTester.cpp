/* This file is part of the KDE project
   Copyright (C) 2006-2010 Dag Andersen <danders@get2net.dk>

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
#include "ResourceTester.h"
#include "DateTimeTester.h"

#include <kptresource.h>
#include <kptcalendar.h>
#include <kptcommand.h>
#include <kptdatetime.h>
#include <kptduration.h>
#include <kptmap.h>


#include <qtest_kde.h>

#include "kptglobal.h"
#include "kptxmlloaderobject.h"

#include <KoXmlReader.h>

#include "debug.cpp"

#include <QDomDocument>
#include <QDomElement>

namespace KPlato
{

void ResourceTester::testAvailable()
{
    Resource r;
    QVERIFY( ! r.availableFrom().isValid() );
    QVERIFY( ! r.availableUntil().isValid() );
    
    QDateTime qt = QDateTime::currentDateTime();
    DateTime dt = KDateTime( qt );
    qDebug()<<"dt"<<dt;
    
    r.setAvailableFrom( qt );
    Debug::print( &r, "Test setAvailableFrom with QDateTime" );
    DateTime x = r.availableFrom();
    qDebug()<<"------"<<x;
    QCOMPARE( x, dt );
    qDebug()<<"----------------";
    r.setAvailableUntil( qt.addDays( 1 ) );
    Debug::print( &r, "Test setAvailableUntil with QDateTime" );
    QCOMPARE( r.availableUntil(), DateTime( dt.addDays( 1 ) ) );
    qDebug()<<"----------------";
}

void ResourceTester::testSingleDay() {
    Calendar t("Test");
    QDate wdate(2006,1,2);
    DateTime before = DateTime(wdate.addDays(-1), QTime());
    DateTime after = DateTime(wdate.addDays(1), QTime());
    QTime t1(8,0,0);
    QTime t2(10,0,0);
    DateTime wdt1(wdate, t1);
    DateTime wdt2(wdate, t2);
    int length = t1.msecsTo( t2 );
    CalendarDay *day = new CalendarDay(wdate, CalendarDay::Working);
    day->addInterval(TimeInterval(t1, length));
    t.addDay(day);
    QVERIFY(t.findDay(wdate) == day);
    
    Resource r;
    r.setAvailableFrom( before );
    r.setAvailableUntil( after );
    r.setCalendar( &t );
    
    Debug::print( &r, "Test single resource, no group, no project", true );

    QVERIFY(r.availableAfter(after, DateTime( after.addDays(1))).isValid() == false);
    QVERIFY(r.availableBefore(before, DateTime(before.addDays(-1))).isValid() == false);
    
    QVERIFY(r.availableAfter(before, after).isValid());
    
    QVERIFY((r.availableAfter(after, DateTime(after.addDays(10)))).isValid() == false);
    QVERIFY((r.availableBefore(before, DateTime(before.addDays(-10)))).isValid() == false);
    
    QCOMPARE(r.availableAfter(before,after), wdt1);
    QCOMPARE(r.availableBefore(after, before), wdt2);
    
    Duration e(0, 2, 0);
    QCOMPARE( r.effort( before, Duration( 2, 0, 0 ) ).toString(), e.toString());

}

void ResourceTester::team()
{
    Resource team;
    Resource tm1, tm2;

    QVERIFY( team.teamMembers().isEmpty() );
    team.addTeamMember( &tm1 );
    QVERIFY( team.teamMembers().count() == 1 );
    team.removeTeamMember( &tm1 );
    QVERIFY( team.teamMembers().isEmpty() );
    team.addTeamMember( &tm1 );
    team.addTeamMember( &tm2 );
    QVERIFY( team.teamMembers().count() == 2 );
    team.addTeamMember( &tm2 );
    QVERIFY( team.teamMembers().count() == 2 );
    team.removeTeamMember( &tm1 );
    team.removeTeamMember( &tm2 );
    QVERIFY( team.teamMembers().isEmpty() );

    AddResourceTeamCmd ac( &team, &tm1 );
    ac.execute();
    QVERIFY( team.teamMembers().count() == 1 );
    ac.unexecute();
    QVERIFY( team.teamMembers().isEmpty() );
    ac.execute();
    RemoveResourceTeamCmd rc( &team, &tm1 );
    rc.execute();
    QVERIFY( team.teamMembers().isEmpty() );
    rc.unexecute();
    QVERIFY( team.teamMembers().count() == 1 );
    
}

void ResourceTester::required()
{
    Project p;
    AddResourceGroupCmd *c1 = new AddResourceGroupCmd( &p, new ResourceGroup() );
    c1->redo();

    ResourceGroup *g = p.resourceGroups().at( 0 );
    QVERIFY( g );
    delete c1;

    AddResourceCmd *c2 = new AddResourceCmd( g, new Resource() );
    c2->redo();
    Resource *r1 = g->resourceAt( 0 );
    QVERIFY( r1 );
    delete c2;
    c2 = new AddResourceCmd( g, new Resource() );
    c2->redo();
    Resource *r2 = g->resourceAt( 1 );
    QVERIFY( r2 );
    delete c2;
    c2 = new AddResourceCmd( g, new Resource() );
    c2->redo();
    Resource *r3 = g->resourceAt( 2 );
    QVERIFY( r3 );
    delete c2;

    QVERIFY( r1->requiredIds().isEmpty() );
    QVERIFY( r1->requiredResources().isEmpty() );

    r1->addRequiredId( "" ); // not allowed to add empty id
    QVERIFY( r1->requiredIds().isEmpty() );

    r1->addRequiredId( r2->id() );
    QCOMPARE( r1->requiredIds().count(), 1 );
    QCOMPARE( r1->requiredResources().count(), 1 );
    QCOMPARE( r1->requiredResources().at( 0 ), r2 );

    r1->addRequiredId( r3->id() );
    QCOMPARE( r1->requiredIds().count(), 2 );
    QCOMPARE( r1->requiredResources().count(), 2 );
    QCOMPARE( r1->requiredResources().at( 0 ), r2 );
    QCOMPARE( r1->requiredResources().at( 1 ), r3 );

    r1->addRequiredId( r2->id() ); // not allowed to add existing id
    QCOMPARE( r1->requiredIds().count(), 2 );
    QCOMPARE( r1->requiredResources().count(), 2 );
    QCOMPARE( r1->requiredResources().at( 0 ), r2 );
    QCOMPARE( r1->requiredResources().at( 1 ), r3 );

    QStringList lst;
    r1->setRequiredIds( lst );
    QCOMPARE( r1->requiredIds().count(), 0 );
    QCOMPARE( r1->requiredIds().count(), 0 );

    lst << r2->id() << r3->id();
    r1->setRequiredIds( lst );
    QCOMPARE( r1->requiredIds().count(), 2 );
    QCOMPARE( r1->requiredResources().count(), 2 );
    QCOMPARE( r1->requiredResources().at( 0 ), r2 );
    QCOMPARE( r1->requiredResources().at( 1 ), r3 );

    // copy to different project
    Project p2;
    c1 = new AddResourceGroupCmd( &p2, new ResourceGroup( g ) );
    c1->redo();
    delete c1;

    ResourceGroup *g1 = p2.resourceGroupAt( 0 );
    c2 = new AddResourceCmd( g1, new Resource( r1 ) );
    c2->redo();
    Resource *r4 = g1->resourceAt( 0 );
    QVERIFY( r4 );
    delete c2;
    c2 = new AddResourceCmd( g1, new Resource( r2 ) );
    c2->redo();
    Resource *r5 = g1->resourceAt( 1 );
    QVERIFY( r5 );
    delete c2;
    c2 = new AddResourceCmd( g1, new Resource( r3 ) );
    c2->redo();
    Resource *r6 = g1->resourceAt( 2 );
    QVERIFY( r6 );
    delete c2;

    QCOMPARE( r4->requiredIds().count(), 2 );
    QCOMPARE( r4->requiredResources().count(), 2 );
    QCOMPARE( r4->requiredResources().at( 0 ), r5 );
    QCOMPARE( r4->requiredResources().at( 1 ), r6 );

    // using xml
    {
        QDomDocument qdoc;
        QDomElement e = qdoc.createElement( "plan" );
        qdoc.appendChild( e );
        p2.setId( "p2" );
        p2.save( e );

        KoXmlDocument xdoc;
        xdoc.setContent( qdoc.toString() );
        XMLLoaderObject sts;
        sts.setProject( &p );
        sts.setVersion( PLAN_FILE_SYNTAX_VERSION );

        Project p3;
        KoXmlElement xe = xdoc.documentElement().firstChildElement();
        p3.load( xe, sts );

        ResourceGroup *g2 = p3.resourceGroupAt( 0 );
        QVERIFY( g2 );
        QCOMPARE( g2->numResources(), 3 );

        Resource *r7 = g2->resourceAt( 0 );
        QVERIFY( r7 );
        Resource *r8 = g2->resourceAt( 1 );
        QVERIFY( r8 );
        Resource *r9 = g2->resourceAt( 2 );
        QVERIFY( r9 );

        QCOMPARE( r7->requiredIds().count(), 2 );
        QCOMPARE( r7->requiredResources().count(), 2 );
        QCOMPARE( r7->requiredResources().at( 0 ), r8 );
        QCOMPARE( r7->requiredResources().at( 1 ), r9 );
    }
    {
        // required in different group
        Project p4;
        p4.setId( "p4" );

        c1 = new AddResourceGroupCmd( &p4, new ResourceGroup() );
        c1->redo();
        delete c1;

        ResourceGroup *m = new ResourceGroup();
        m->setType( ResourceGroup::Type_Material );
        c1 = new AddResourceGroupCmd( &p4, m );
        c1->redo();
        delete c1;

        ResourceGroup *g3 = p4.resourceGroupAt( 0 );
        c2 = new AddResourceCmd( g3, new Resource() );
        c2->redo();
        Resource *r10 = g3->resourceAt( 0 );
        QVERIFY( r4 );
        delete c2;

        Resource *r11 = new Resource();
        r11->setType( Resource::Type_Material );
        c2 = new AddResourceCmd( m, r11 );
        c2->redo();
        QVERIFY( m->resourceAt( 0 ) == r11 );
        delete c2;
        Resource *r12 = new Resource();
        r12->setType( Resource::Type_Material );
        c2 = new AddResourceCmd( m, r12 );
        c2->redo();
        QVERIFY( m->resourceAt( 1 ) == r12 );
        delete c2;

        r10->addRequiredId( r11->id() );
        r10->addRequiredId( r12->id() );
        QCOMPARE( r10->requiredIds().count(), 2 );
        QCOMPARE( r10->requiredResources().count(), 2 );
        QCOMPARE( r10->requiredResources().at( 0 ), r11 );
        QCOMPARE( r10->requiredResources().at( 1 ), r12 );

        // using xml
        QDomDocument qdoc;
        QDomElement e = qdoc.createElement( "plan" );
        qdoc.appendChild( e );
        p4.save( e );

        KoXmlDocument xdoc;
        xdoc.setContent( qdoc.toString() );
        XMLLoaderObject sts;
        sts.setProject( &p4 );
        sts.setVersion( PLAN_FILE_SYNTAX_VERSION );

        Project p5;
        KoXmlElement xe = xdoc.documentElement().firstChildElement();
        p5.load( xe, sts );

        ResourceGroup *g4 = p5.resourceGroupAt( 0 );
        QVERIFY( g4 );
        QCOMPARE( g4->numResources(), 1 );

        ResourceGroup *g5 = p5.resourceGroupAt( 1 );
        QVERIFY( g5 );
        QCOMPARE( g5->numResources(), 2 );

        Resource *r13 = g4->resourceAt( 0 );
        QVERIFY( r13 );
        QCOMPARE( r13->id(), r10->id() );
        Resource *r14 = g5->resourceAt( 0 );
        QVERIFY( r14 );
        QCOMPARE( r14->id(), r11->id() );
        Resource *r15 = g5->resourceAt( 1 );
        QVERIFY( r15 );
        QCOMPARE( r15->id(), r12->id() );

        QCOMPARE( r13->requiredIds().count(), 2 );
        QCOMPARE( r13->requiredResources().count(), 2 );
        QCOMPARE( r13->requiredResources().at( 0 ), r14 );
        QCOMPARE( r13->requiredResources().at( 1 ), r15 );
    }
}

} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::ResourceTester )

#include "ResourceTester.moc"
