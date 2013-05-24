/* This file is part of the KDE project
   Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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

#include "InsertProjectTester.h"

#include "kptcommand.h"
#include "kptmaindocument.h"
#include "kptpart.h"
#include "kptcalendar.h"
#include "kptresource.h"
#include "kpttask.h"
#include "kptaccount.h"

#include <qtest_kde.h>

namespace KPlato
{

Account *InsertProjectTester::addAccount( MainDocument &part, Account *parent )
{
    Project &p = part.getProject();
    Account *a = new Account();
    QString s = parent == 0 ? "Account" : parent->name();
    a->setName( p.accounts().uniqueId( s ) );
    KUndo2Command *c = new AddAccountCmd( p, a, parent );
    part.addCommand( c );
    return a;
}

void InsertProjectTester::testAccount()
{
    Part pp(0);
    MainDocument part( &pp );
    pp.setDocument( &part );

    addAccount( part );
    Project &p = part.getProject();
    QCOMPARE( p.accounts().accountCount(), 1 );

    Part pp2(0);
    MainDocument part2( &pp2 );
    pp2.setDocument( &part2 );

    part2.insertProject( p, 0, 0 );
    QCOMPARE( part2.getProject().accounts().accountCount(), 1 );

    part2.insertProject( part.getProject(), 0, 0 );
    QCOMPARE( part2.getProject().accounts().accountCount(), 1 );

    Part ppB(0);
    MainDocument partB( &ppB );
    ppB.setDocument( &partB );

    Account *parent = addAccount( partB );
    QCOMPARE( partB.getProject().accounts().accountCount(), 1 );
    addAccount( partB, parent );
    QCOMPARE( partB.getProject().accounts().accountCount(), 1 );
    QCOMPARE( parent->childCount(), 1 );

    part2.insertProject( partB.getProject(), 0, 0 );
    QCOMPARE( part2.getProject().accounts().accountCount(), 1 );
    QCOMPARE( part2.getProject().accounts().accountAt( 0 )->childCount(), 1 );
}

Calendar *InsertProjectTester::addCalendar( MainDocument &part )
{
    Project &p = part.getProject();
    Calendar *c = new Calendar();
    p.setCalendarId( c );
    part.addCommand( new CalendarAddCmd( &p, c, -1, 0 ) );
    return c;
}

void InsertProjectTester::testCalendar()
{
    Part pp(0);
    MainDocument part( &pp );
    pp.setDocument( &part );

    addCalendar( part );
    Project &p = part.getProject();
    QVERIFY( p.calendarCount() == 1 );

    Part pp2(0);
    MainDocument part2( &pp2 );
    pp2.setDocument( &part2 );

    part2.insertProject( part.getProject(), 0, 0 );
    QVERIFY( part2.getProject().calendarCount() == 1 );
    QVERIFY( part2.getProject().defaultCalendar() == 0 );
}

void InsertProjectTester::testDefaultCalendar()
{
    Part pp(0);
    MainDocument part( &pp );
    pp.setDocument( &part );

    Calendar *c = addCalendar( part );
    Project &p = part.getProject();
    p.setDefaultCalendar( c );
    QVERIFY( p.calendarCount() == 1 );
    QCOMPARE( p.defaultCalendar(), c );

    Part pp2(0);
    MainDocument part2( &pp2 );
    pp2.setDocument( &part2 );

    part2.insertProject( p, 0, 0 );
    QVERIFY( part2.getProject().calendarCount() == 1 );
    QCOMPARE( part2.getProject().defaultCalendar(), c );

    Part ppB(0);
    MainDocument partB( &ppB );
    ppB.setDocument( &partB );

    Calendar *c2 = addCalendar( partB );
    partB.getProject().setDefaultCalendar( c2 );
    part2.insertProject( partB.getProject(), 0, 0 );
    QVERIFY( part2.getProject().calendarCount() == 2 );
    QCOMPARE( part2.getProject().defaultCalendar(), c ); // NB: still c, not c2
}

ResourceGroup *InsertProjectTester::addResourceGroup( MainDocument &part )
{
    Project &p = part.getProject();
    ResourceGroup *g = new ResourceGroup();
    KUndo2Command *c = new AddResourceGroupCmd( &p, g );
    part.addCommand( c );
    QString s = QString( "G%1" ).arg( part.getProject().indexOf( g ) );
    g->setName( s );
    return g;
}

void InsertProjectTester::testResourceGroup()
{
    Part pp(0);
    MainDocument part( &pp );
    pp.setDocument( &part );

    addResourceGroup( part );
    Project &p = part.getProject();
    QVERIFY( p.resourceGroupCount() == 1 );

    Part pp2(0);
    MainDocument part2( &pp2 );
    pp2.setDocument( &part2 );
    part2.insertProject( p, 0, 0 );
    QVERIFY( part2.getProject().resourceGroupCount() == 1 );
}

Resource *InsertProjectTester::addResource( MainDocument &part, ResourceGroup *g )
{
    Project &p = part.getProject();
    if ( g == 0 ) {
        g = p.resourceGroupAt( 0 );
    }
    Resource *r = new Resource();
    KUndo2Command *c = new AddResourceCmd( g, r );
    part.addCommand( c );
    QString s = QString( "%1.R%2" ).arg( r->parentGroup()->name() ).arg( r->parentGroup()->indexOf( r ) );
    r->setName( s );
    return r;
}

void InsertProjectTester::testResource()
{
    Part pp(0);
    MainDocument part( &pp );
    pp.setDocument( &part );

    addResourceGroup( part );
    addResource( part );
    Project &p = part.getProject();
    QVERIFY( p.resourceGroupAt( 0 )->numResources() == 1 );

    Part pp2(0);
    MainDocument part2( &pp2 );
    pp2.setDocument( &part2 );
    part2.insertProject( p, 0, 0 );
    QVERIFY( part2.getProject().resourceGroupAt( 0 )->numResources() == 1 );
}

void InsertProjectTester::testTeamResource()
{
    Part pp(0);
    MainDocument part( &pp );
    pp.setDocument( &part );

    addResourceGroup( part );
    Resource *r = addResource( part );
    r->setType( Resource::Type_Team );
    ResourceGroup *tg = addResourceGroup( part );
    Resource *t1 = addResource( part, tg );
    Resource *t2 = addResource( part, tg );
    r->setRequiredIds( QStringList() << t1->id() << t2->id() );
    Project &p = part.getProject();
    QVERIFY( p.resourceGroupAt( 0 )->numResources() == 1 );
    QVERIFY( p.resourceGroupAt( 1 )->numResources() == 2 );
    QList<Resource*> required = p.resourceGroupAt( 0 )->resources().at( 0 )->requiredResources();
    QCOMPARE( required.count(), 2 );
    QCOMPARE( required.at( 0 ), t1 );
    QCOMPARE( required.at( 1 ), t2 );

    Part pp2(0);
    MainDocument part2( &pp2 );
    pp2.setDocument( &part2 );

    part2.insertProject( p, 0, 0 );
    Project &p2 = part2.getProject();
    QVERIFY( p2.resourceGroupAt( 0 )->numResources() == 1 );
    QVERIFY( p2.resourceGroupAt( 1 )->numResources() == 2 );
    required = p2.resourceGroupAt( 0 )->resources().at( 0 )->requiredResources();
    QCOMPARE( required.count(), 2 );
    QCOMPARE( required.at( 0 ), t1 );
    QCOMPARE( required.at( 1 ), t2 );
}

void InsertProjectTester::testResourceAccount()
{
    Part pp(0);
    MainDocument part( &pp );
    pp.setDocument( &part );

    addResourceGroup( part );
    Resource *r = addResource( part );
    Account *a = addAccount( part );
    part.addCommand( new ResourceModifyAccountCmd( *r, r->account(), a ) );

    Project &p = part.getProject();
    QVERIFY( p.resourceGroupAt( 0 )->numResources() == 1 );

    Part pp2(0);
    MainDocument part2( &pp2 );
    pp2.setDocument( &part2 );

    part2.insertProject( p, 0, 0 );
    QVERIFY( part2.getProject().resourceGroupAt( 0 )->numResources() == 1 );
    QVERIFY( part2.getProject().accounts().allAccounts().contains( a ) );
    QCOMPARE( part2.getProject().resourceGroupAt( 0 )->resourceAt( 0 )->account(), a );
}

void InsertProjectTester::testResourceCalendar()
{
    Part pp(0);
    MainDocument part( &pp );
    pp.setDocument( &part );

    Calendar *c = addCalendar( part );
    Project &p = part.getProject();
    QVERIFY( p.calendarCount() == 1 );

    addResourceGroup( part );
    Resource *r = addResource( part );
    part.addCommand( new ModifyResourceCalendarCmd( r, c ) );

    QVERIFY( p.resourceGroupAt( 0 )->numResources() == 1 );

    Part pp2(0);
    MainDocument part2( &pp2 );
    pp2.setDocument( &part2 );

    part2.insertProject( p, 0, 0 );
    QVERIFY( part2.getProject().resourceGroupAt( 0 )->numResources() == 1 );
    QCOMPARE( part2.getProject().allCalendars().count(), 1 );
    QVERIFY( part2.getProject().allCalendars().contains( c ) );
    QCOMPARE( part2.getProject().resourceGroupAt( 0 )->resourceAt( 0 )->calendar( true ), c );
}

Task *InsertProjectTester::addTask( MainDocument &part )
{
    Project &p = part.getProject();
    Task *t = new Task();
    t->setId( p.uniqueNodeId() );
    KUndo2Command *c = new TaskAddCmd( &p, t, 0 );
    part.addCommand( c );
    return t;
}

void InsertProjectTester::testTask()
{
    Part pp(0);
    MainDocument part( &pp );
    pp.setDocument( &part );

    addTask( part );
    Project &p = part.getProject();
    QVERIFY( p.numChildren() == 1 );

    Part pp2(0);
    MainDocument part2( &pp2 );
    pp2.setDocument( &part2 );

    part2.insertProject( p, 0, 0 );
    QVERIFY( part2.getProject().numChildren() == 1 );
}

void InsertProjectTester::addGroupRequest( MainDocument &part )
{
    Project &p = part.getProject();
    Task *t = static_cast<Task*>( p.childNode( 0 ) );
    KUndo2Command *c = new AddResourceGroupRequestCmd( *t, new ResourceGroupRequest( p.resourceGroupAt( 0 ), 1 ) );
    part.addCommand( c );
}

void InsertProjectTester::testGroupRequest()
{
    Part pp(0);
    MainDocument part( &pp );
    pp.setDocument( &part );

    addCalendar( part );
    addResourceGroup( part );
    addResource( part );
    addTask( part );
    addGroupRequest( part );

    Project &p = part.getProject();
    QVERIFY( p.numChildren() == 1 );

    Part pp2(0);
    MainDocument part2( &pp2 );
    pp2.setDocument( &part2 );

    part2.insertProject( p, 0, 0 );
    Project &p2 = part2.getProject();
    QVERIFY( p2.childNode( 0 )->resourceGroupRequest( p2.resourceGroupAt( 0 ) ) != 0 );
}

void InsertProjectTester::addResourceRequest( MainDocument &part )
{
    Project &p = part.getProject();
    ResourceGroupRequest *g = p.childNode( 0 )->requests().requests().at( 0 );
    KUndo2Command *c = new AddResourceRequestCmd( g, new  ResourceRequest( p.resourceGroupAt( 0 )->resourceAt( 0 ), 1  ) );
    part.addCommand( c );
}

void InsertProjectTester::testResourceRequest()
{
    Part pp(0);
    MainDocument part( &pp );
    pp.setDocument( &part );

    addCalendar( part );
    addResourceGroup( part );
    addResource( part );
    addTask( part );
    addGroupRequest( part );
    addResourceRequest( part );

    Project &p = part.getProject();
    Part pp2(0);
    MainDocument part2( &pp2 );
    pp2.setDocument( &part2 );
    part2.insertProject( p, 0, 0 );
    Project &p2 = part2.getProject();
    QVERIFY( p2.childNode( 0 )->requests().find( p2.resourceGroupAt( 0 )->resourceAt( 0 ) ) != 0 );
}

void InsertProjectTester::testTeamResourceRequest()
{
    Part pp(0);
    MainDocument part( &pp );
    pp.setDocument( &part );

    addCalendar( part );
    addResourceGroup( part );
    Resource *r = addResource( part );
    r->setType( Resource::Type_Team );
    ResourceGroup *tg = addResourceGroup( part );
    Resource *t1 = addResource( part, tg );
    r->addTeamMemberId( t1->id() );
    Resource *t2 = addResource( part, tg );
    r->addTeamMemberId( t2->id() );
    addTask( part );
    addGroupRequest( part );
    addResourceRequest( part );

    qDebug()<<"Start test:";
    Part pp2(0);
    MainDocument part2( &pp2 );
    pp2.setDocument( &part2 );
    part2.insertProject( part.getProject(), 0, 0 );
    Project &p2 = part2.getProject();
    ResourceRequest *rr = p2.childNode( 0 )->requests().find( p2.resourceGroupAt( 0 )->resourceAt( 0 ) );
    QVERIFY( rr );
    QCOMPARE( rr->resource(), r );
    QCOMPARE( rr->resource()->teamMembers().count(), 2 );
    QCOMPARE( rr->resource()->teamMembers().at( 0 ), t1 );
    QCOMPARE( rr->resource()->teamMembers().at( 1 ), t2 );
}

Relation *InsertProjectTester::addDependency( MainDocument &part, Task *t1, Task *t2 )
{
    Project &p = part.getProject();
    Relation *r = new Relation( t1, t2 );
    part.addCommand( new AddRelationCmd( p, r ) );
    return r;
}

void InsertProjectTester::testDependencies()
{
    Part pp(0);
    MainDocument part( &pp );
    pp.setDocument( &part );

    Task *t1 = addTask( part );
    Task *t2 = addTask( part );
    QCOMPARE( t1->numDependChildNodes(), 0 );
    QCOMPARE( t2->numDependParentNodes(), 0 );

    Relation *r = addDependency( part, t1, t2 );

    QCOMPARE( t1->numDependChildNodes(), 1 );
    QCOMPARE( t2->numDependParentNodes(), 1 );
    QCOMPARE( t1->getDependChildNode( 0 ), r );
    QCOMPARE( t2->getDependParentNode( 0 ), r );

    Part pp2(0);
    MainDocument part2( &pp2 );
    pp2.setDocument( &part2 );

    QVERIFY( part2.insertProject( part.getProject(), 0, 0 ) );
    Project &p2 = part2.getProject();

    QVERIFY( p2.numChildren() == 2 );
    QCOMPARE( p2.childNode( 0 ), t1 );
    QCOMPARE( p2.childNode( 1 ), t2 );

    QCOMPARE( t1->numDependChildNodes(), 1 );
    QCOMPARE( t2->numDependParentNodes(), 1 );
    QCOMPARE( t1->getDependChildNode( 0 ), r );
    QCOMPARE( t2->getDependParentNode( 0 ), r );
}

void InsertProjectTester::testExistingResourceAccount()
{
    Part pp(0);
    MainDocument part( &pp );
    pp.setDocument( &part );

    addResourceGroup( part );
    Resource *r = addResource( part );
    Account *a = addAccount( part );
    part.addCommand( new ResourceModifyAccountCmd( *r, r->account(), a ) );

    Project &p = part.getProject();
    QVERIFY( p.resourceGroupAt( 0 )->numResources() == 1 );

    QDomDocument doc = part.saveXML();

    Part pp2(0);
    MainDocument part2( &pp2 );
    pp2.setDocument( &part2 );

    part2.insertProject( p, 0, 0 );
    QVERIFY( part2.getProject().resourceGroupAt( 0 )->numResources() == 1 );
    QVERIFY( part2.getProject().accounts().allAccounts().contains( a ) );
    QCOMPARE( part2.getProject().resourceGroupAt( 0 )->resourceAt( 0 )->account(), a );

    part2.addCommand( new ResourceModifyAccountCmd( *(part2.getProject().resourceGroupAt( 0 )->resourceAt( 0 )), part2.getProject().resourceGroupAt( 0 )->resourceAt( 0 )->account(), 0 ) );

    KoXmlDocument xdoc;
    xdoc.setContent( doc.toString() );
    part.loadXML( xdoc, 0 );

    part2.insertProject( part.getProject(), 0, 0 );
    QVERIFY( part2.getProject().resourceGroupAt( 0 )->numResources() == 1 );
    QVERIFY( part2.getProject().accounts().allAccounts().contains( a ) );
    QVERIFY( part2.getProject().resourceGroupAt( 0 )->resourceAt( 0 )->account() == 0 );
}

void InsertProjectTester::testExistingResourceCalendar()
{
    Part pp(0);
    MainDocument part( &pp );
    pp.setDocument( &part );

    Calendar *c = addCalendar( part );
    Project &p = part.getProject();
    QVERIFY( p.calendarCount() == 1 );

    addResourceGroup( part );
    Resource *r = addResource( part );
    part.addCommand( new ModifyResourceCalendarCmd( r, c ) );

    QVERIFY( p.resourceGroupAt( 0 )->numResources() == 1 );

    QDomDocument doc = part.saveXML();

    Part pp2(0);
    MainDocument part2( &pp2 );
    pp2.setDocument( &part2 );

    part2.insertProject( p, 0, 0 );
    QVERIFY( part2.getProject().resourceGroupAt( 0 )->numResources() == 1 );
    QCOMPARE( part2.getProject().allCalendars().count(), 1 );
    QVERIFY( part2.getProject().allCalendars().contains( c ) );
    QCOMPARE( part2.getProject().resourceGroupAt( 0 )->resourceAt( 0 )->calendar( true ), c );

    part2.getProject().resourceGroupAt( 0 )->resourceAt( 0 )->setCalendar( 0 );

    KoXmlDocument xdoc;
    xdoc.setContent( doc.toString() );
    part.loadXML( xdoc, 0 );

    part2.insertProject( part.getProject(), 0, 0 );
    QVERIFY( part2.getProject().resourceGroupAt( 0 )->numResources() == 1 );
    QCOMPARE( part2.getProject().allCalendars().count(), 1 );
    QVERIFY( part2.getProject().allCalendars().contains( c ) );
    QVERIFY( part2.getProject().resourceGroupAt( 0 )->resourceAt( 0 )->calendar( true ) == 0 );
}

void InsertProjectTester::testExistingResourceRequest()
{
    Part pp(0);
    MainDocument part( &pp );
    pp.setDocument( &part );

    addCalendar( part );
    addResourceGroup( part );
    addResource( part );
    addTask( part );
    addGroupRequest( part );
    addResourceRequest( part );

    QDomDocument doc = part.saveXML();

    Project &p = part.getProject();
    Part pp2(0);
    MainDocument part2( &pp2 );
    pp2.setDocument( &part2 );
    part2.insertProject( p, 0, 0 );
    Project &p2 = part2.getProject();
    QVERIFY( p2.childNode( 0 )->requests().find( p2.resourceGroupAt( 0 )->resourceAt( 0 ) ) != 0 );

    KoXmlDocument xdoc;
    xdoc.setContent( doc.toString() );
    part.loadXML( xdoc, 0 );

    part2.insertProject( part.getProject(), 0, 0 );
    QVERIFY( p2.childNode( 0 )->requests().find( p2.resourceGroupAt( 0 )->resourceAt( 0 ) ) != 0 );
    QVERIFY( p2.childNode( 1 )->requests().find( p2.resourceGroupAt( 0 )->resourceAt( 0 ) ) != 0 );
}

void InsertProjectTester::testExistingRequiredResourceRequest()
{
    Part pp(0);
    MainDocument part( &pp );
    pp.setDocument( &part );

    addCalendar( part );
    addResourceGroup( part );
    Resource *r = addResource( part );
    ResourceGroup *g = addResourceGroup( part );
    g->setType( ResourceGroup::Type_Material );
    QList<Resource*> m; m << addResource( part, g );
    m.first()->setType( Resource::Type_Material );
    r->setRequiredIds( QStringList() << m.first()->id() );
    addTask( part );
    addGroupRequest( part );
    addResourceRequest( part );

    QDomDocument doc = part.saveXML();

    Project &p = part.getProject();
    Part pp2(0);
    MainDocument part2( &pp2 );
    pp2.setDocument( &part2 );
    part2.insertProject( p, 0, 0 );
    Project &p2 = part2.getProject();
    ResourceRequest *rr = p2.childNode( 0 )->requests().find( p2.resourceGroupAt( 0 )->resourceAt( 0 ) );
    QVERIFY( rr );
    QVERIFY( ! rr->requiredResources().isEmpty() );
    QCOMPARE( rr->requiredResources().at( 0 ), m.first() );

    KoXmlDocument xdoc;
    xdoc.setContent( doc.toString() );
    part.loadXML( xdoc, 0 );

    part2.insertProject( part.getProject(), 0, 0 );
    rr = p2.childNode( 0 )->requests().find( p2.resourceGroupAt( 0 )->resourceAt( 0 ) );
    QVERIFY( rr );
    QVERIFY( ! rr->requiredResources().isEmpty() );
    QCOMPARE( rr->requiredResources().at( 0 ), m.first() );

    rr = p2.childNode( 1 )->requests().find( p2.resourceGroupAt( 0 )->resourceAt( 0 ) );
    QVERIFY( rr );
    QVERIFY( ! rr->requiredResources().isEmpty() );
    QCOMPARE( rr->requiredResources().at( 0 ), m.first() );
}

void InsertProjectTester::testExistingTeamResourceRequest()
{
    Part pp(0);
    MainDocument part( &pp );
    pp.setDocument( &part );

    addCalendar( part );
    addResourceGroup( part );
    Resource *r = addResource( part );
    r->setName( "R1" );
    r->setType( Resource::Type_Team );
    ResourceGroup *tg = addResourceGroup( part );
    tg->setName( "TG" );
    Resource *t1 = addResource( part, tg );
    t1->setName( "T1" );
    r->addTeamMemberId( t1->id() );
    Resource *t2 = addResource( part, tg );
    t2->setName( "T2" );
    r->addTeamMemberId( t2->id() );
    addTask( part );
    addGroupRequest( part );
    addResourceRequest( part );

    QDomDocument doc = part.saveXML();

    Part pp2(0);
    MainDocument part2( &pp2 );
    pp2.setDocument( &part2 );

    Project &p2 = part2.getProject();
    part2.insertProject( part.getProject(), 0, 0 );
    ResourceRequest *rr = p2.childNode( 0 )->requests().find( p2.resourceGroupAt( 0 )->resourceAt( 0 ) );
    QVERIFY( rr );
    QCOMPARE( rr->resource()->teamMembers().count(), 2 );
    QCOMPARE( rr->resource()->teamMembers().at( 0 ), t1 );
    QCOMPARE( rr->resource()->teamMembers().at( 1 ), t2 );

    KoXmlDocument xdoc;
    xdoc.setContent( doc.toString() );
    part.loadXML( xdoc, 0 );

    part2.insertProject( part.getProject(), 0, 0 );
    QCOMPARE( p2.numChildren(), 2 );

    QVERIFY( ! p2.childNode( 0 )->requests().isEmpty() );
    rr = p2.childNode( 0 )->requests().find( p2.resourceGroupAt( 0 )->resourceAt( 0 ) );
    QVERIFY( rr );
    QCOMPARE( rr->resource()->teamMembers().count(), 2 );
    QCOMPARE( rr->resource()->teamMembers().at( 0 ), t1 );
    QCOMPARE( rr->resource()->teamMembers().at( 1 ), t2 );

    QVERIFY( ! p2.childNode( 1 )->requests().isEmpty() );
    rr = p2.childNode( 1 )->requests().find( p2.resourceGroupAt( 0 )->resourceAt( 0 ) );
    QVERIFY( rr );
    QCOMPARE( rr->resource()->teamMembers().count(), 2 );
    QCOMPARE( rr->resource()->teamMembers().at( 0 ), t1 );
    QCOMPARE( rr->resource()->teamMembers().at( 1 ), t2 );
}

} //namespace KPlato

QTEST_KDEMAIN(KPlato::InsertProjectTester, GUI)
#include "InsertProjectTester.moc"
