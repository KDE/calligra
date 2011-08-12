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
#include "kptnode.h"

#include <KUrl>

#include <QFileInfo>

namespace KPlato
{


void InsertFileTester::testVersion_0_6()
{
    Part part;
    QFileInfo file( "version-0-6.kplato" );
    QVERIFY( file.exists() );

    bool res = part.openUrl( KUrl( file.absoluteFilePath() ) );
    QVERIFY( res );

    Project &p = part.getProject();

    QCOMPARE( p.name(), QString( "P1" ) );
    QCOMPARE( p.leader(), QString( "pp" ) );

    QCOMPARE( p.numChildren(), 2 );
    QCOMPARE( p.numResourceGroups(), 1 );
    QCOMPARE( p.resourceList().count(), 1 );
    QCOMPARE( p.allCalendars().count(), 2 );

    Node *n = p.childNode( 0 );
    QCOMPARE( n->type(), (int)Node::Type_Task );
    QCOMPARE( n->name(), QString( "T1" ) );

    n = p.childNode( 1 );
    QCOMPARE( n->type(), (int)Node::Type_Task );
    QCOMPARE( n->name(), QString( "T2" ) );

}

void InsertFileTester::testProject_stats1()
{
    Part part;
    QFileInfo file( "project_stats1.kplato" );
    QVERIFY( file.exists() );

    bool res = part.openUrl( KUrl( file.absoluteFilePath() ) );
    QVERIFY( res );

    Project &p = part.getProject();

    QCOMPARE( p.name(), QString( "P1" ) );
    QCOMPARE( p.leader(), QString( "Robin" ) );

    QCOMPARE( p.numChildren(), 1 );
    QCOMPARE( p.numResourceGroups(), 1 );
    QCOMPARE( p.resourceList().count(), 1 );
    QCOMPARE( p.allCalendars().count(), 1 );

    Node *n = p.childNode( 0 );
    QCOMPARE( n->type(), (int)Node::Type_Task );
    QCOMPARE( n->name(), QString( "T1" ) );

}

void InsertFileTester::testPert1()
{
    Part part;
    QFileInfo file( "pert1.kplato" );
    QVERIFY( file.exists() );

    bool res = part.openUrl( KUrl( file.absoluteFilePath() ) );
    QVERIFY( res );

    Project &p = part.getProject();

    QCOMPARE( p.name(), QString( "PERT 1" ) );
    QCOMPARE( p.leader(), QString( "PM" ) );

    QCOMPARE( p.numChildren(), 7 );
    QCOMPARE( p.numResourceGroups(), 0 );
    QCOMPARE( p.resourceList().count(), 0 );
    QCOMPARE( p.allCalendars().count(), 1 );

    Node *n = p.childNode( 0 );
    QCOMPARE( n->type(), (int)Node::Type_Task );
    QCOMPARE( n->name(), QString( "a" ) );

    n = p.childNode( 1 );
    QCOMPARE( n->type(), (int)Node::Type_Task );
    QCOMPARE( n->name(), QString( "b" ) );

    n = p.childNode( 2 );
    QCOMPARE( n->type(), (int)Node::Type_Task );
    QCOMPARE( n->name(), QString( "c" ) );

    n = p.childNode( 3 );
    QCOMPARE( n->type(), (int)Node::Type_Task );
    QCOMPARE( n->name(), QString( "d" ) );

    n = p.childNode( 4 );
    QCOMPARE( n->type(), (int)Node::Type_Task );
    QCOMPARE( n->name(), QString( "e" ) );

    n = p.childNode( 5 );
    QCOMPARE( n->type(), (int)Node::Type_Task );
    QCOMPARE( n->name(), QString( "f" ) );

    n = p.childNode( 6 );
    QCOMPARE( n->type(), (int)Node::Type_Task );
    QCOMPARE( n->name(), QString( "g" ) );
}

} //namespace KPlato

QTEST_KDEMAIN(KPlato::InsertFileTester, GUI)
#include "InsertFileTester.moc"
