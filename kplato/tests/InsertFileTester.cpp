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

#include "InsertFileTester.h"

#include "kptcommand.h"
#include "kptpart.h"
#include "kptcalendar.h"
#include "kptresource.h"
#include "kpttask.h"

namespace KPlato
{

void InsertFileTester::init()
{

}

void InsertFileTester::addCalendar( Part &part )
{
    Project &p = part.getProject();
    QUndoCommand *c = new CalendarAddCmd( &p, new Calendar(), 0 );
    part.addCommand( c );
}

void InsertFileTester::testCalendar()
{
    Part part;
    addCalendar( part );
    Project &p = part.getProject();
    QVERIFY( p.calendarCount() == 1 );

    Part part2;
    part2.insertProject( part.getProject(), 0, 0 );
    QVERIFY( part2.getProject().calendarCount() == 1 );
}

void InsertFileTester::addResourceGroup( Part &part )
{
    Project &p = part.getProject();
    QUndoCommand *c = new AddResourceGroupCmd( &p, new ResourceGroup() );
    part.addCommand( c );
}

void InsertFileTester::testResourceGroup()
{
    Part part;
    addResourceGroup( part );
    Project &p = part.getProject();
    QVERIFY( p.resourceGroupCount() == 1 );

    Part part2;
    part2.insertProject( p, 0, 0 );
    QVERIFY( part2.getProject().resourceGroupCount() == 1 );
}

void InsertFileTester::addResource( Part &part )
{
    Project &p = part.getProject();
    QVERIFY( p.resourceGroupAt( 0 ) );
    QUndoCommand *c = new AddResourceCmd( p.resourceGroupAt( 0 ), new Resource() );
    part.addCommand( c );
}

void InsertFileTester::testResource()
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

void InsertFileTester::addTask( Part &part )
{
    Project &p = part.getProject();
    Task *t = new Task();
    t->setId( p.uniqueNodeId() );
    QUndoCommand *c = new TaskAddCmd( &p, t, 0 );
    part.addCommand( c );
}

void InsertFileTester::testTask()
{
    Part part;
    addTask( part );
    Project &p = part.getProject();
    QVERIFY( p.numChildren() == 1 );

    Part part2;
    part2.insertProject( p, 0, 0 );
    QVERIFY( part2.getProject().numChildren() == 1 );
}

} //namespace KPlato

QTEST_KDEMAIN(KPlato::InsertFileTester, GUI)
#include "InsertFileTester.moc"
