/* This file is part of the KDE project
   Copyright (C) 2012 Dag Andersen <danders@get2net.dk>

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
#include "WorkPackageProxyModelTester.h"
#include "kptcommand.h"
#include "kpttask.h"
#include "kptnode.h"
#include "kptnodeitemmodel.h"

#include <QModelIndex>

#include <qtest_kde.h>
#include <kdebug.h>

#include "../../kernel/tests/debug.cpp"

namespace KPlato
{

Task *WorkPackageProxyModelTester::addTask( Node *parent, int after )
{
    Task *task = project.createTask();
    TaskAddCmd cmd( &project, task, parent->childNode( after ) );
    cmd.redo();
    ResourceGroupRequest *gr = new ResourceGroupRequest( project.resourceGroupAt(0) );
    ResourceRequest *rr = new ResourceRequest( project.resourceGroupAt(0)->resourceAt(0), 100 );
    gr->addResourceRequest( rr );
    AddResourceGroupRequestCmd c( *task, gr );
    c.redo();
    return task;
}

void WorkPackageProxyModelTester::removeTask( Node *task )
{
    NodeDeleteCmd cmd( task );
    cmd.redo();
}

void WorkPackageProxyModelTester::moveTask( Node *task, Node *newParent, int newPos )
{
    NodeMoveCmd cmd( &project, task, newParent, newPos );
    cmd.redo();
}

void WorkPackageProxyModelTester::initTestCase()
{
    project.setName( "P1" );
    project.setId( project.uniqueNodeId() );
    project.registerNodeId( &project );
    Calendar *calendar = new Calendar();
    calendar->setName( "C1" );
    calendar->setDefault( true );
    QTime t1( 9, 0, 0 );
    QTime t2 ( 17, 0, 0 );
    int length = t1.msecsTo( t2 );
    for ( int i=1; i <= 7; ++i ) {
        CalendarDay *d = calendar->weekday( i );
        d->setState( CalendarDay::Working );
        d->addInterval( t1, length );
    }
    project.addCalendar( calendar );

    QCOMPARE( m_model.rowCount(), 0 );
    m_model.setProject( &project );
    QCOMPARE( m_model.rowCount(), 0 );

    ResourceGroup *g = new ResourceGroup();
    g->setName( "G1" );
    project.addResourceGroup( g );
    Resource *r = new Resource();
    r->setName( "R1" );
    project.addResource( g, r );

    sm = project.createScheduleManager();
    m_model.setScheduleManager( sm );
}

void WorkPackageProxyModelTester::testInsert()
{
    Task *task = addTask( &project, 0 );
    task->setName( "T1" );
    QCOMPARE( m_model.baseModel()->rowCount(), 1 );
    QCOMPARE( m_model.rowCount(), 0 ); // filtered, (not scheduled)

    sm->createSchedules();
    project.calculate( *sm );
    QVERIFY( task->isScheduled() );
    QCOMPARE( m_model.rowCount(), 1 );
    QCOMPARE( m_model.index( 0, NodeModel::NodeName ).data().toString(), QString( "T1" ) );

    task = addTask( &project, -1 );
    task->setName( "T2" );
    QCOMPARE( m_model.baseModel()->rowCount(), 2 );
    QCOMPARE( m_model.rowCount(), 1 ); // filtered, (not scheduled)
    sm->createSchedules();
    project.calculate( *sm );
    QVERIFY( task->isScheduled() );
    QCOMPARE( m_model.rowCount(), 2 );
    QCOMPARE( m_model.index( 1, NodeModel::NodeName ).data().toString(), QString( "T2" ) );

    task = addTask( &project, -1 );
    task->setName( "T3" );
    QCOMPARE( m_model.baseModel()->rowCount(), 3 );
    QCOMPARE( m_model.rowCount(), 2 ); // filtered, (not scheduled)
    sm->createSchedules();
    project.calculate( *sm );
    QVERIFY( task->isScheduled() );
    QCOMPARE( m_model.rowCount(), 3 );
    QCOMPARE( m_model.index( 2, NodeModel::NodeName ).data().toString(), QString( "T3" ) );
}

void WorkPackageProxyModelTester::testRemove()
{

    removeTask( project.childNode( 0 ) );
    QCOMPARE( m_model.rowCount(), 2 );
    QCOMPARE( m_model.index( 0, NodeModel::NodeName ).data().toString(), QString( "T2" ) );
    QCOMPARE( m_model.index( 1, NodeModel::NodeName ).data().toString(), QString( "T3" ) );

    removeTask( project.childNode( 1 ) );
    QCOMPARE( m_model.rowCount(), 1 );
    QCOMPARE( m_model.index( 0, NodeModel::NodeName ).data().toString(), QString( "T2" ) );

    removeTask( project.childNode( 0 ) );
    QCOMPARE( m_model.rowCount(), 0 );
}

void WorkPackageProxyModelTester::testMove()
{
    testInsert();
    //Debug::print( &project, "T1, T2, T3 ---------", true );
    moveTask( project.childNode( 0 ), &project, 1 );
    //Debug::print( &project, "T2, T1, T3 ---------", true );
    QCOMPARE( m_model.rowCount(), 3 );
    QCOMPARE( m_model.index( 0, NodeModel::NodeName ).data().toString(), QString( "T2" ) );
    QCOMPARE( m_model.index( 1, NodeModel::NodeName ).data().toString(), QString( "T1" ) );
    QCOMPARE( m_model.index( 2, NodeModel::NodeName ).data().toString(), QString( "T3" ) );

    moveTask( project.childNode( 1 ), &project, 0 );
    QCOMPARE( m_model.index( 0, NodeModel::NodeName ).data().toString(), QString( "T1" ) );
    QCOMPARE( m_model.index( 1, NodeModel::NodeName ).data().toString(), QString( "T2" ) );
    QCOMPARE( m_model.index( 2, NodeModel::NodeName ).data().toString(), QString( "T3" ) );

    moveTask( project.childNode( 1 ), &project, 0 );
    QCOMPARE( m_model.index( 0, NodeModel::NodeName ).data().toString(), QString( "T2" ) );
    QCOMPARE( m_model.index( 1, NodeModel::NodeName ).data().toString(), QString( "T1" ) );
    QCOMPARE( m_model.index( 2, NodeModel::NodeName ).data().toString(), QString( "T3" ) );

    moveTask( project.childNode( 0 ), &project, -1 ); // last
    QCOMPARE( m_model.index( 0, NodeModel::NodeName ).data().toString(), QString( "T1" ) );
    QCOMPARE( m_model.index( 1, NodeModel::NodeName ).data().toString(), QString( "T3" ) );
    QCOMPARE( m_model.index( 2, NodeModel::NodeName ).data().toString(), QString( "T2" ) );
}

void WorkPackageProxyModelTester::testMoveChild()
{
    while ( project.numChildren() > 0 ) {
        Node *t = project.childNode( 0 );
        project.takeTask( t );
        delete t;
    }
    testInsert();
    Task *task = addTask( &project, 1 ); // after 1 = 2 :(
    task->setName( "T4" );
    //Debug::print( &project, "T1, T2, T4, T3 ----", true );
    QCOMPARE( project.numChildren(), 4 );
    QCOMPARE( m_model.rowCount(), 3 ); // filtered, (not scheduled)

    sm->createSchedules();
    project.calculate( *sm );
    //Debug::print( &project, "T1, T2, T4, T3 ----", true );
    QVERIFY( task->isScheduled() );
    QCOMPARE( m_model.rowCount(), 4 );
    QCOMPARE( m_model.index( 0, NodeModel::NodeName ).data().toString(), QString( "T1" ) );
    QCOMPARE( m_model.index( 1, NodeModel::NodeName ).data().toString(), QString( "T2" ) );
    QCOMPARE( m_model.index( 2, NodeModel::NodeName ).data().toString(), QString( "T4" ) );
    QCOMPARE( m_model.index( 3, NodeModel::NodeName ).data().toString(), QString( "T3" ) );

    project.indentTask( project.childNode( 2 ) );
    //Debug::print( &project, "T1, T2 -> T4, T3 ----", true );
    QCOMPARE( m_model.rowCount(), 3 );
    QCOMPARE( m_model.index( 0, NodeModel::NodeName ).data().toString(), QString( "T1" ) );
    QCOMPARE( m_model.index( 1, NodeModel::NodeName ).data().toString(), QString( "T4" ) );
    QCOMPARE( m_model.index( 2, NodeModel::NodeName ).data().toString(), QString( "T3" ) );

    moveTask( project.childNode( 1 )->childNode( 0 ), project.childNode( 0 ), 0 );
    //Debug::print( &project, "T1 -> T4, T2, T3 ----", true );
    QCOMPARE( m_model.rowCount(), 3 );
    QCOMPARE( m_model.index( 0, NodeModel::NodeName ).data().toString(), QString( "T4" ) );
    QCOMPARE( m_model.index( 1, NodeModel::NodeName ).data().toString(), QString( "T2" ) );
    QCOMPARE( m_model.index( 2, NodeModel::NodeName ).data().toString(), QString( "T3" ) );

    moveTask( project.childNode( 0 )->childNode( 0 ), project.childNode( 2 ), 0 );
    //Debug::print( &project, "T1, T2, T3 -> T4 ----", true );
    QCOMPARE( m_model.rowCount(), 3 );
    QCOMPARE( m_model.index( 0, NodeModel::NodeName ).data().toString(), QString( "T1" ) );
    QCOMPARE( m_model.index( 1, NodeModel::NodeName ).data().toString(), QString( "T2" ) );
    QCOMPARE( m_model.index( 2, NodeModel::NodeName ).data().toString(), QString( "T4" ) );

    moveTask( project.childNode( 2 )->childNode( 0 ), &project, 0 );
    //Debug::print( &project, "T4, T1, T2, T3 ----", true );
    QCOMPARE( m_model.rowCount(), 4 );
    QCOMPARE( m_model.index( 0, NodeModel::NodeName ).data().toString(), QString( "T4" ) );
    QCOMPARE( m_model.index( 1, NodeModel::NodeName ).data().toString(), QString( "T1" ) );
    QCOMPARE( m_model.index( 2, NodeModel::NodeName ).data().toString(), QString( "T2" ) );
    QCOMPARE( m_model.index( 3, NodeModel::NodeName ).data().toString(), QString( "T3" ) );
}

} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::WorkPackageProxyModelTester )

#include "WorkPackageProxyModelTester.moc"
