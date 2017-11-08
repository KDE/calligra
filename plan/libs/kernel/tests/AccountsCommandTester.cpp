/* This file is part of the KDE project
   Copyright (C) 2017 Dag Andersen <danders@get2net.dk>

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
#include "AccountsCommandTester.h"

#include "kptdatetime.h"
#include "kptcommand.h"
#include "kptproject.h"
#include "kptcalendar.h"
#include "kptresource.h"
#include "kptnode.h"
#include "kpttask.h"
#include "kptschedule.h"

#include <QTest>
#include <kundo2stack.h>

namespace QTest
{
    template<>
            char *toString(const KPlato::DateTime &dt)
    {
        return toString( dt.toString() );
    }
}


namespace KPlato
{

void AccountsCommandTester::printDebug( long id ) const {
    Project *p = m_project;
    Resource *r = m_resource;
    qDebug()<<"Debug info -------------------------------------";
    qDebug()<<"project start time:"<<p->startTime().toString();
    qDebug()<<"project end time  :"<<p->endTime().toString();

    qDebug()<<"Resource start:"<<r->startTime( id ).toString();
    qDebug()<<"Resource end  :"<<r->endTime( id ).toString();
    qDebug()<<"Appointments:"<<r->numAppointments( id )<<"(internal)";
    foreach ( Appointment *a, r->appointments( id ) ) {
        foreach ( const AppointmentInterval &i, a->intervals().map() ) {
            qDebug()<<"  "<<i.startTime().toString()<<"-"<<i.endTime().toString()<<";"<<i.load();
        }
    }
    qDebug()<<"Appointments:"<<r->numExternalAppointments()<<"(external)";
    foreach ( Appointment *a, r->externalAppointmentList() ) {
        foreach ( const AppointmentInterval &i, a->intervals().map() ) {
            qDebug()<<"  "<<i.startTime().toString()<<"-"<<i.endTime().toString()<<";"<<i.load();
        }
    }
}

void AccountsCommandTester::printSchedulingLog( const ScheduleManager &sm ) const
{
    qDebug()<<"Scheduling log ---------------------------------";
    foreach ( const QString &s, sm.expected()->logMessages() ) {
        qDebug()<<s;
    }
}

void AccountsCommandTester::init()
{
    m_project = new Project();
    m_project->setName( "P1" );
    m_project->setId( m_project->uniqueNodeId() );
    m_project->registerNodeId( m_project );
    DateTime targetstart = DateTime( QDate::currentDate(), QTime(0,0,0) );
    DateTime targetend = DateTime( targetstart.addDays( 3 ) );
    m_project->setConstraintStartTime( targetstart );
    m_project->setConstraintEndTime( targetend);

    // standard worktime defines 8 hour day as default
    QVERIFY( m_project->standardWorktime() );
    QCOMPARE( m_project->standardWorktime()->day(), 8.0 );
    m_calendar = new Calendar( "Test" );
    m_calendar->setDefault( true );
    QTime t1( 9, 0, 0 );
    QTime t2 ( 17, 0, 0 );
    int length = t1.msecsTo( t2 );
    for ( int i=1; i <= 7; ++i ) {
        CalendarDay *d = m_calendar->weekday( i );
        d->setState( CalendarDay::Working );
        d->addInterval( t1, length );
    }
    m_project->addCalendar( m_calendar );


    ResourceGroup *g = new ResourceGroup();
    g->setName( "G1" );
    m_project->addResourceGroup( g );
    m_resource = new Resource();
    m_resource->setName( "R1" );
    m_resource->setCalendar( m_calendar );
    m_project->addResource( g, m_resource );

    m_task = m_project->createTask();
    m_task->setName( "T1" );
    m_project->addTask( m_task, m_project );
    m_task->estimate()->setUnit( Duration::Unit_h );
    m_task->estimate()->setExpectedEstimate( 8.0 );
    m_task->estimate()->setType( Estimate::Type_Effort );
}

void AccountsCommandTester::cleanup()
{
    delete m_project;
}

void AccountsCommandTester::addAccount()
{
    Account *a1 = new Account();
    a1->setName("a1");
    AddAccountCmd *cmd1 = new AddAccountCmd(*m_project, a1);
    cmd1->redo();

    QCOMPARE(m_project->accounts().allAccounts().count(), 1);

    cmd1->undo();
    QCOMPARE(m_project->accounts().allAccounts().count(), 0);

    delete cmd1;

    a1 = new Account();
    a1->setName("a1");
    cmd1 = new AddAccountCmd(*m_project, a1);
    cmd1->redo();

    QCOMPARE(m_project->accounts().allAccounts().count(), 1);

    Account *a2 = new Account();
    a2->setName("a2");
    AddAccountCmd *cmd2 = new AddAccountCmd(*m_project, a2, a1);
    cmd2->redo();

    QCOMPARE(m_project->accounts().allAccounts().count(), 2);

    cmd2->undo();
    QCOMPARE(m_project->accounts().allAccounts().count(), 1);
    cmd1->undo();
    QCOMPARE(m_project->accounts().allAccounts().count(), 0);

    delete cmd2;
    delete cmd1;
}

void AccountsCommandTester::removeAccount()
{
    Account *a1 = new Account();
    a1->setName("a1");
    m_project->accounts().insert(a1);
    QCOMPARE(m_project->accounts().allAccounts().count(), 1);



    RemoveAccountCmd *cmd1 = new RemoveAccountCmd(*m_project, a1);
    cmd1->redo();
    QCOMPARE(m_project->accounts().allAccounts().count(), 0);

    cmd1->undo();
    QCOMPARE(m_project->accounts().allAccounts().count(), 1);

    cmd1->redo();
    delete cmd1;
    QCOMPARE(m_project->accounts().allAccounts().count(), 0);

    a1 = new Account();
    a1->setName("a1");
    m_project->accounts().insert(a1);
    Account *a2 = new Account();
    a2->setName("a2");
    m_project->accounts().insert(a2, a1);
    QCOMPARE(m_project->accounts().allAccounts().count(), 2);

    cmd1 = new RemoveAccountCmd(*m_project, a1);
    cmd1->redo();
    QCOMPARE(m_project->accounts().allAccounts().count(), 0);

    cmd1->undo();
    QCOMPARE(m_project->accounts().allAccounts().count(), 2);

    RemoveAccountCmd *cmd2 = new RemoveAccountCmd(*m_project, a2);
    cmd2->redo();
    QCOMPARE(m_project->accounts().allAccounts().count(), 1);

    cmd2->undo();
    QCOMPARE(m_project->accounts().allAccounts().count(), 2);

    cmd2->redo();
    QCOMPARE(m_project->accounts().allAccounts().count(), 1);

    delete cmd2; // should delete a2
    delete cmd1; // should not delete a1
}

void AccountsCommandTester::costPlace()
{
    KUndo2QStack cmds;
    Account *a1 = new Account();
    a1->setName("a1");
    cmds.push(new AddAccountCmd(*m_project, a1));
    QCOMPARE(m_project->accounts().allAccounts().count(), 1);

    Account *a2 = new Account();
    a2->setName("a2");
    cmds.push(new AddAccountCmd(*m_project, a2));
    QCOMPARE(m_project->accounts().allAccounts().count(), 2);

    Account *a3 = new Account();
    a3->setName("a3");
    cmds.push(new AddAccountCmd(*m_project, a3));
    QCOMPARE(m_project->accounts().allAccounts().count(), 3);

    cmds.push(new NodeModifyRunningAccountCmd(*m_task, 0, a1));
    QCOMPARE(m_task->runningAccount(), a1);
    cmds.push(new NodeModifyStartupAccountCmd(*m_task, 0, a2));
    QCOMPARE(m_task->startupAccount(), a2);
    cmds.push(new NodeModifyShutdownAccountCmd(*m_task, 0, a3));
    QCOMPARE(m_task->shutdownAccount(), a3);

    cmds.push(new RemoveAccountCmd(*m_project, a1));
    QVERIFY(m_task->runningAccount() == 0);
    QCOMPARE(m_task->startupAccount(), a2);
    QCOMPARE(m_task->shutdownAccount(), a3);
    cmds.undo();
    QCOMPARE(m_task->runningAccount(), a1);
    QCOMPARE(m_task->startupAccount(), a2);
    QCOMPARE(m_task->shutdownAccount(), a3);

    cmds.push(new RemoveAccountCmd(*m_project, a2));
    QVERIFY(m_task->startupAccount() == 0);
    QCOMPARE(m_task->runningAccount(), a1);
    QCOMPARE(m_task->shutdownAccount(), a3);
    cmds.undo();
    QCOMPARE(m_task->runningAccount(), a1);
    QCOMPARE(m_task->startupAccount(), a2);
    QCOMPARE(m_task->shutdownAccount(), a3);

    cmds.push(new RemoveAccountCmd(*m_project, a3));
    QVERIFY(m_task->shutdownAccount() == 0);
    QCOMPARE(m_task->runningAccount(), a1);
    QCOMPARE(m_task->startupAccount(), a2);
    cmds.undo();
    QCOMPARE(m_task->runningAccount(), a1);
    QCOMPARE(m_task->startupAccount(), a2);
    QCOMPARE(m_task->shutdownAccount(), a3);

    cmds.push(new ResourceModifyAccountCmd(*m_resource, 0, a1));
    QCOMPARE(m_resource->account(), a1);

    cmds.push(new RemoveAccountCmd(*m_project, a1));
    QVERIFY(m_task->runningAccount() == 0);
    QVERIFY(m_resource->account() == 0);
    cmds.undo();
    QCOMPARE(m_task->runningAccount(), a1);
    QCOMPARE(m_resource->account(), a1);

    // test when same account is used for running/startup/shutdown
    while (cmds.canUndo()) {
        cmds.undo();
    }
    a1 = new Account();
    a1->setName("a1");
    cmds.push(new AddAccountCmd(*m_project, a1));
    QCOMPARE(m_project->accounts().allAccounts().count(), 1);

    cmds.push(new NodeModifyRunningAccountCmd(*m_task, 0, a1));
    QCOMPARE(m_task->runningAccount(), a1);
    cmds.push(new NodeModifyStartupAccountCmd(*m_task, 0, a1));
    QCOMPARE(m_task->startupAccount(), a1);
    cmds.push(new NodeModifyShutdownAccountCmd(*m_task, 0, a1));
    QCOMPARE(m_task->shutdownAccount(), a1);

    cmds.undo();
    QCOMPARE(m_task->runningAccount(), a1);
    QCOMPARE(m_task->startupAccount(), a1);
    QVERIFY(m_task->shutdownAccount() == 0);

    cmds.undo();
    QCOMPARE(m_task->runningAccount(), a1);
    QVERIFY(m_task->startupAccount() == 0);
    QVERIFY(m_task->shutdownAccount() == 0);

    cmds.undo();
    QVERIFY(m_task->runningAccount() == 0);
    QVERIFY(m_task->startupAccount() == 0);
    QVERIFY(m_task->shutdownAccount() == 0);

    cmds.redo();
    QCOMPARE(m_task->runningAccount(), a1);
    QVERIFY(m_task->startupAccount() == 0);
    QVERIFY(m_task->shutdownAccount() == 0);

    cmds.redo();
    QCOMPARE(m_task->runningAccount(), a1);
    QCOMPARE(m_task->startupAccount(), a1);
    QVERIFY(m_task->shutdownAccount() == 0);

    cmds.redo();
    QCOMPARE(m_task->runningAccount(), a1);
    QCOMPARE(m_task->startupAccount(), a1);
    QCOMPARE(m_task->shutdownAccount(), a1);
}

} //namespace KPlato

QTEST_GUILESS_MAIN( KPlato::AccountsCommandTester )
