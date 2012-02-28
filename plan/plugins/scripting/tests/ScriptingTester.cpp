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
#include "ScriptingTester.h"

#include "TestResult.h"

#include "Module.h"

#include <qtest_kde.h>
#include <Kross/Action>
#include <Kross/Manager>
#include <kdebug.h>

namespace KPlato
{


QStringList ScriptingTester::initTestList()
{
    QStringList scripts;
    scripts << "project_access.py"
        << "account_access.py"
        << "account_readwrite.py"
        << "calendar_access.py"
        << "calendar_readwrite.py"
        << "task_access.py"
        << "task_readwrite.py"
        << "resource_access.py"
        << "resource_readwrite.py"
        << "resource_team.py"
        ;
    return scripts;
}

void ScriptingTester::initTestCase()
{
    Kross::Action *a = new Kross::Action( 0, "PythonDetection" );
    a->setInterpreter( "python" );
    bool py = a->initialize();
    a->finalize();
    if ( ! py ) {
        QEXPECT_FAIL( "", "Python not available, tests will not be executed", Continue );
        QVERIFY( py == true );
    } else {
        m_module = new Scripting::Module( this );
        m_result = new TestResult( this );
        Kross::Manager::self().addObject( m_module, "Plan" );
        Kross::Manager::self().addObject( m_result, "TestResult" );

        QStringList scripts = initTestList();;
        for ( int i = 0; i < scripts.count(); ++i ) {
            //Create a new Kross::Action instance.
            Kross::Action* action = new Kross::Action(0, QString( "%1" ).arg( scripts.at( i ) ) );
            // Set the script file we like to execute.
            action->setFile( QString( "%1/%2" ).arg( FILES_DATA_DIR ).arg( scripts.at( i ) ) );
            m_tests << action;
        }
    }
}

void ScriptingTester::cleanupTestCase()
{
    while ( ! m_tests.isEmpty() ) {
        delete m_tests.takeFirst();
    }
}

void ScriptingTester::test()
{
    qDebug()<<m_tests;
    foreach ( Kross::Action *a, m_tests ) {
        m_result->setResult( false );
        m_result->message = QString( "%1: Failed to run test" ).arg( a->objectName() );
        a->trigger();
        QVERIFY2( m_result->isOk(), m_result->message.toLocal8Bit() );
        qDebug() << "PASS: " << a->objectName();
    }
}

} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::ScriptingTester )

#include "ScriptingTester.moc"
