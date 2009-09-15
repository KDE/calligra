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

namespace KPlato
{

void InsertProjectTester::init()
{

}

void InsertProjectTester::addCalendar( Part &part )
{
    Project &p = part.getProject();
    QUndoCommand *c = new CalendarAddCmd( &p, new Calendar(), 0 );
    part.addCommand( c );
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
}

void InsertProjectTester::addResourceGroup( Part &part )
{
    Project &p = part.getProject();
    QUndoCommand *c = new AddResourceGroupCmd( &p, new ResourceGroup() );
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

void InsertProjectTester::addResource( Part &part )
{
    Project &p = part.getProject();
    QVERIFY( p.resourceGroupAt( 0 ) );
    QUndoCommand *c = new AddResourceCmd( p.resourceGroupAt( 0 ), new Resource() );
    part.addCommand( c );
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

void InsertProjectTester::addTask( Part &part )
{
    Project &p = part.getProject();
    Task *t = new Task();
    t->setId( p.uniqueNodeId() );
    QUndoCommand *c = new TaskAddCmd( &p, t, 0 );
    part.addCommand( c );
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
    QUndoCommand *c = new AddResourceGroupRequestCmd( *t, new ResourceGroupRequest( p.resourceGroupAt( 0 ), 1 ) );
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
    QUndoCommand *c = new AddResourceRequestCmd( g, new  ResourceRequest( p.resourceGroupAt( 0 )->resourceAt( 0 ), 1  ) );
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

} //namespace KPlato

QTEST_KDEMAIN(KPlato::InsertProjectTester, GUI)
#include "InsertProjectTester.moc"
