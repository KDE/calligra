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

    QCOMPARE( p.numChildren(), 2 );
}

void InsertFileTester::testProject_stats1()
{
    Part part;
    QFileInfo file( "project_stats1.kplato" );
    QVERIFY( file.exists() );

    bool res = part.openUrl( KUrl( file.absoluteFilePath() ) );
    QVERIFY( res );
}

void InsertFileTester::testPert1()
{
    Part part;
    QFileInfo file( "pert1.kplato" );
    QVERIFY( file.exists() );

    bool res = part.openUrl( KUrl( file.absoluteFilePath() ) );
    QVERIFY( res );
}

} //namespace KPlato

QTEST_KDEMAIN(KPlato::InsertFileTester, GUI)
#include "InsertFileTester.moc"
