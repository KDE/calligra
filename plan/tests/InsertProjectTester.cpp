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
#include <qtest_kde.h>

#include "InsertProjectTester.h"

#include "kptcommand.h"
#include "kptpart.h"
#include "kptcalendar.h"
#include "kptresource.h"
#include "kpttask.h"
#include "kptaccount.h"

namespace KPlato
{

Account *InsertProjectTester::addAccount( Part &part, Account *parent )
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
    Part part;
    addAccount( part );
    Project &p = part.getProject();
    QCOMPARE( p.accounts().accountCount(), 1 );

    Part part2;
    part2.insertProject( p, 0, 0 );
    QCOMPARE( part2.getProject().accounts().accountCount(), 1 );

    part2.insertProject( part.getProject(), 0, 0 );
    QCOMPARE( part2.getProject().accounts().accountCount(), 1 );

    Part partB;
    Account *parent = addAccount( partB );
    QCOMPARE( partB.getProject().accounts().accountCount(), 1 );
    addAccount( partB, parent );
    QCOMPARE( partB.getProject().accounts().accountCount(), 1 );
    QCOMPARE( parent->childCount(), 1 );

    part2.insertProject( partB.getProject(), 0, 0 );
    QCOMPARE( part2.getProject().accounts().accountCount(), 1 );
    QCOMPARE( part2.getProject().accounts().accountAt( 0 )->childCount(), 1 );
}

Calendar *InsertProjectTester::addCalendar( Part &part )
{
    Project &p = part.getProject();
    Calendar *c = new Calendar();
    p.setCalendarId( c );
    part.addCommand( new CalendarAddCmd( &p, c, -1, 0 ) );
    return c;
}

void InsertProjectTester::testCalendar()
{
    Part part;
    addCalendar( part );
    Project &p = part.getProject();
    QVERIFY( p.calendarCount() == 1 );

    Part part2;
    part2.insertProject( part.getProject(), 0, 0 );
    QVERIFY( part2.getProject().calendarCount() == 1 );
    QVERIFY( part2.getProject().defaultCalendar() == 0 );
}

void InsertProjectTester::testDefaultCalendar()
{
    Part part;
    Calendar *c = addCalendar( part );
    Project &p = part.getProject();
    p.setDefaultCalendar( c );
    QVERIFY( p.calendarCount() == 1 );
    QCOMPARE( p.defaultCalendar(), c );

    Part part2;
    part2.insertProject( p, 0, 0 );
    QVERIFY( part2.getProject().calendarCount() == 1 );
    QCOMPARE( part2.getProject().defaultCalendar(), c );

    Part partB;
    Calendar *c2 = addCalendar( partB );
    partB.getProject().setDefaultCalendar( c2 );
    part2.insertProject( partB.getProject(), 0, 0 );
    QVERIFY( part2.getProject().calendarCount() == 2 );
    QCOMPARE( part2.getProject().defaultCalendar(), c ); // NB: still c, not c2
}

void InsertProjectTester::addResourceGroup( Part &part )
{
    Project &p = part.getProject();
    KUndo2Command *c = new AddResourceGroupCmd( &p, new ResourceGroup() );
    part.addCommand( c );
}

void InsertProjectTester::testResourceGroup()
{
    Part part;
    addResourceGroup( part );
    Project &p = part.getProject();
    QVERIFY( p.resourceGroupCount() == 1 );

    Part part2;
    part2.insertProject( p, 0, 0 );
    QVERIFY( part2.getProject().resourceGroupCount() == 1 );
}

Resource *InsertProjectTester::addResource( Part &part )
{
    Project &p = part.getProject();
    Resource *r = new Resource();
    KUndo2Command *c = new AddResourceCmd( p.resourceGroupAt( 0 ), r );
    part.addCommand( c );
    return r;
}

void InsertProjectTester::testResource()
{
    Part part;
    addResourceGroup( part );
    addResource( part );
    Project &p = part.getProject();
    QVERIFY( p.resourceGroupAt( 0 )->numResources() == 1 );

    Part part2;
    part2.insertProject( p, 0, 0 );
    QVERIFY( part2.getProject().resourceGroupAt( 0 )->numResources() == 1 );
}

void InsertProjectTester::testResourceAccount()
{
    Part part;
    addResourceGroup( part );
    Resource *r = addResource( part );
    Account *a = addAccount( part );
    part.addCommand( new ResourceModifyAccountCmd( *r, r->account(), a ) );

    Project &p = part.getProject();
    QVERIFY( p.resourceGroupAt( 0 )->numResources() == 1 );

    Part part2;
    part2.insertProject( p, 0, 0 );
    QVERIFY( part2.getProject().resourceGroupAt( 0 )->numResources() == 1 );
    QVERIFY( part2.getProject().accounts().allAccounts().contains( a ) );
    QCOMPARE( part2.getProject().resourceGroupAt( 0 )->resourceAt( 0 )->account(), a );
}

Task *InsertProjectTester::addTask( Part &part )
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
    Part part;
    addTask( part );
    Project &p = part.getProject();
    QVERIFY( p.numChildren() == 1 );

    Part part2;
    part2.insertProject( p, 0, 0 );
    QVERIFY( part2.getProject().numChildren() == 1 );
}

void InsertProjectTester::addGroupRequest( Part &part )
{
    Project &p = part.getProject();
    Task *t = static_cast<Task*>( p.childNode( 0 ) );
    KUndo2Command *c = new AddResourceGroupRequestCmd( *t, new ResourceGroupRequest( p.resourceGroupAt( 0 ), 1 ) );
    part.addCommand( c );
}

void InsertProjectTester::testGroupRequest()
{
    Part part;
    addCalendar( part );
    addResourceGroup( part );
    addResource( part );
    addTask( part );
    addGroupRequest( part );

    Project &p = part.getProject();
    QVERIFY( p.numChildren() == 1 );

    Part part2;
    part2.insertProject( p, 0, 0 );
    Project &p2 = part2.getProject();
    QVERIFY( p2.childNode( 0 )->resourceGroupRequest( p2.resourceGroupAt( 0 ) ) != 0 );
}

void InsertProjectTester::addResourceRequest( Part &part )
{
    Project &p = part.getProject();
    ResourceGroupRequest *g = p.childNode( 0 )->requests().requests().at( 0 );
    KUndo2Command *c = new AddResourceRequestCmd( g, new  ResourceRequest( p.resourceGroupAt( 0 )->resourceAt( 0 ), 1  ) );
    part.addCommand( c );
}

void InsertProjectTester::testResourceRequest()
{
    Part part;
    addCalendar( part );
    addResourceGroup( part );
    addResource( part );
    addTask( part );
    addGroupRequest( part );
    addResourceRequest( part );

    Project &p = part.getProject();
    Part part2;
    part2.insertProject( p, 0, 0 );
    Project &p2 = part2.getProject();
    QVERIFY( p2.childNode( 0 )->requests().find( p2.resourceGroupAt( 0 )->resourceAt( 0 ) ) != 0 );
}

Relation *InsertProjectTester::addDependency( Part &part, Task *t1, Task *t2 )
{
    Project &p = part.getProject();
    Relation *r = new Relation( t1, t2 );
    part.addCommand( new AddRelationCmd( p, r ) );
    return r;
}

void InsertProjectTester::testDependencies()
{
    Part part;
    Task *t1 = addTask( part );
    Task *t2 = addTask( part );
    QCOMPARE( t1->numDependChildNodes(), 0 );
    QCOMPARE( t2->numDependParentNodes(), 0 );

    Relation *r = addDependency( part, t1, t2 );

    QCOMPARE( t1->numDependChildNodes(), 1 );
    QCOMPARE( t2->numDependParentNodes(), 1 );
    QCOMPARE( t1->getDependChildNode( 0 ), r );
    QCOMPARE( t2->getDependParentNode( 0 ), r );

    Part part2;
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

} //namespace KPlato

QTEST_KDEMAIN(KPlato::InsertProjectTester, GUI)
#include "InsertProjectTester.moc"
