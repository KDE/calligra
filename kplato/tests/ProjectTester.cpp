/* This file is part of the KDE project
   Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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
#include "ProjectTester.h"

#include "kpttask.h"
#include "kptcommand.h"

#include <QString>

#include <kdebug.h>

namespace KPlato
{

void ProjectTester::initTestCase()
{
    m_project = new Project();
    m_task = 0;
}

void ProjectTester::cleanupTestCase()
{
    delete m_project;
}

void ProjectTester::testProjectSetup()
{

}

void ProjectTester::testAddTask()
{
    m_task = m_project->createTask( m_project );
    QVERIFY( m_project->addTask( m_task, m_project ) );
    QVERIFY( m_task->parentNode() == m_project );
    QCOMPARE( m_project->findNode( m_task->id() ), m_task );
    
    m_project->takeTask( m_task );
    delete m_task; m_task = 0;
}

void ProjectTester::testTakeTask()
{
    m_task = m_project->createTask( m_project );
    m_project->addTask( m_task, m_project );
    QCOMPARE( m_project->findNode( m_task->id() ), m_task );
    
    m_project->takeTask( m_task );
    QVERIFY( m_project->findNode( m_task->id() ) == 0 );
    
    delete ( m_task ); m_task = 0;
}

void ProjectTester::testTaskAddCmd()
{
    m_task = m_project->createTask( m_project );
    K3Command *cmd = new SubtaskAddCmd( 0, m_project, m_task, m_project );
    cmd->execute();
    QVERIFY( m_task->parentNode() == m_project );
    QCOMPARE( m_project->findNode( m_task->id() ), m_task );
    cmd->unexecute();
    QVERIFY( m_project->findNode( m_task->id() ) == 0 );
    delete cmd;
    m_task = 0;
}

void ProjectTester::testTaskDeleteCmd()
{
    m_task = m_project->createTask( m_project );
    QVERIFY( m_project->addTask( m_task, m_project ) );
    QVERIFY( m_task->parentNode() == m_project );
    
    K3Command *cmd = new NodeDeleteCmd( 0, m_task );
    cmd->execute();
    QVERIFY( m_project->findNode( m_task->id() ) == 0 );
    
    cmd->unexecute();
    QCOMPARE( m_project->findNode( m_task->id() ), m_task );
    
    cmd->execute();
    delete cmd; 
    m_task = 0;
}

} //namespace KPlato

QTEST_MAIN( KPlato::ProjectTester )

#include "ProjectTester.moc"
