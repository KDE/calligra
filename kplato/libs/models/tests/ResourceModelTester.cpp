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
#include "ResourceModelTester.h"

#include "kptcommand.h"
#include "kptcalendar.h"
#include "kptdatetime.h"
#include "kptresource.h"
#include "kptnode.h"
#include "kpttask.h"
#include "kptschedule.h"
#include "kptappointment.h"

#include <QModelIndex>

#include <qtest_kde.h>
#include <kdebug.h>

namespace KPlato
{

void ResourceModelTester::printDebug( long id ) const {
    Project *p = m_project;
    Resource *r = m_resource;
    qDebug()<<"Debug info -------------------------------------";
    qDebug()<<"project start time:"<<p->startTime().toString();
    qDebug()<<"project end time  :"<<p->endTime().toString();

    qDebug()<<"Resource start:"<<r->startTime( id ).toString();
    qDebug()<<"Resource end  :"<<r->endTime( id ).toString();
    qDebug()<<"Appointments:"<<r->numAppointments( id )<<"(internal)";
    foreach ( Appointment *a, r->appointments( id ) ) {
        foreach ( const AppointmentInterval &i, a->intervals() ) {
            qDebug()<<"  "<<i.startTime().toString()<<"-"<<i.endTime().toString()<<";"<<i.load();
        }
    }
    qDebug()<<"Appointments:"<<r->numExternalAppointments()<<"(external)";
    foreach ( Appointment *a, r->externalAppointmentList() ) {
        foreach ( const AppointmentInterval &i, a->intervals() ) {
            qDebug()<<"  "<<i.startTime().toString()<<"-"<<i.endTime().toString()<<";"<<i.load();
        }
    }
}

void ResourceModelTester::printSchedulingLog( const ScheduleManager &sm ) const
{
    qDebug()<<"Scheduling log ---------------------------------";
    foreach ( const QString &s, sm.expected()->logMessages() ) {
        qDebug()<<s;
    }
}

void ResourceModelTester::initTestCase()
{
    m_project = new Project();
    m_project->setName( "P1" );
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

    m_task = m_project->createTask( m_project );
    m_task->setName( "T1" );
    m_project->addTask( m_task, m_project );
    m_task->estimate()->setUnit( Duration::Unit_h );
    m_task->estimate()->setExpectedEstimate( 8.0 );
    m_task->estimate()->setType( Estimate::Type_Effort );


    ResourceGroupRequest *gr = new ResourceGroupRequest( g );
    gr->addResourceRequest( new ResourceRequest( m_resource, 100 ) );
    m_task->addRequest( gr );

    m_model.setProject( m_project );

    QModelIndex idx;
    int rows = m_model.rowCount( idx );
    QCOMPARE( rows, 1 );
    idx = m_model.index( 0, 0, idx );
    QCOMPARE( g->name(), m_model.data( idx ).toString() );
    
    rows = m_model.rowCount( idx );
    QCOMPARE( rows, 1 );
    idx = m_model.index( 0, 0, idx );
    QCOMPARE( m_resource->name(), m_model.data( idx ).toString() );

    idx = m_model.parent( idx );
    QCOMPARE( g->name(), m_model.data( idx ).toString() );
}

void ResourceModelTester::cleanupTestCase()
{
    delete m_project;
}

void ResourceModelTester::internalAppointments()
{
    ScheduleManager *sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );
    long id = sm->id();
    m_model.setScheduleManager( sm );

    //printDebug( sm->id() );

    QModelIndex idx;
    // resource group
    int rows = m_model.rowCount( idx );
    QCOMPARE( rows, 1 );
    QModelIndex gidx = m_model.index( 0, 0, idx );
    QVERIFY( gidx.isValid() );

    // reosurce
    rows = m_model.rowCount( gidx );
    QCOMPARE( rows, 1 );
    QModelIndex ridx = m_model.index( 0, 0, gidx );
    QCOMPARE( m_resource->name(), m_model.data( ridx ).toString() );

    ridx = m_model.index( m_resource );
    QVERIFY( ridx.isValid() );

    // appointment
    rows = m_model.rowCount( ridx );
    QCOMPARE( rows, m_resource->numAppointments( id ) );
    QCOMPARE( rows, 1 );

    QModelIndex aidx = m_model.index( 0, 0, ridx ); // first appointment
    QVERIFY( aidx.isValid() );
    rows = m_model.rowCount( aidx ); // num intervals
    QCOMPARE( rows, 1 );

    // interval
    QModelIndex iidx = m_model.index( 0, 0, aidx ); // first interval
    QVERIFY( iidx.isValid() );
    rows = m_model.rowCount( iidx ); // intervals don't have children
    QCOMPARE( rows, 0 );

    // appointment
    idx = m_model.parent( iidx );
    QCOMPARE( idx, aidx );
    // resource
    idx = m_model.parent( aidx );
    QCOMPARE( idx, ridx );

    // resource group
    idx = m_model.parent( ridx );
    QCOMPARE( idx, gidx );

    // top
    idx = m_model.parent( gidx );
    QVERIFY( ! idx.isValid() );

}

void ResourceModelTester::externalAppointments()
{
    DateTime targetstart = m_project->constraintStartTime();
    DateTime targetend = m_project->constraintEndTime();
    Task *t = m_task;
    Resource *r = m_resource;

    r->addExternalAppointment( "Ext-1", "External project 1", targetstart, targetstart.addDays( 1 ), 100 );
    r->addExternalAppointment( "Ext-1", "External project 1", targetend.addDays( -1 ), targetend, 100 );

    ScheduleManager *sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    m_project->calculate( *sm );
    long id = sm->id();
    m_model.setScheduleManager( sm );
    //printSchedulingLog( *sm );
    //printDebug( sm->id() );
    
    // resource group
    QModelIndex idx;
    int rows = m_model.rowCount( idx );
    QCOMPARE( rows, 1 );
    idx = m_model.index( 0, 0, idx );
    QVERIFY( idx.isValid() );
    QVERIFY( ! m_model.parent( idx ).isValid() );

    // resource
    rows = m_model.rowCount( idx );
    QCOMPARE( rows, 1 );
    QModelIndex ridx = m_model.index( 0, 0, idx );
    QCOMPARE( m_resource->name(), m_model.data( ridx ).toString() );
    QCOMPARE( ridx.parent(), idx );

    ridx = m_model.index( m_resource );
    QVERIFY( ridx.isValid() );

    // appointments
    rows = m_model.rowCount( ridx );
    QCOMPARE( rows, m_resource->numAppointments( id ) + m_resource->numExternalAppointments() );
    QCOMPARE( rows, 2 ); // one internal, one external

    // internal appointment
    QModelIndex aidx = m_model.index( 0, 0, ridx ); // first appointment (internal)
    QVERIFY( aidx.isValid() );
    rows = m_model.rowCount( aidx ); // num intervals
    QCOMPARE( rows, 1 );
    QCOMPARE( aidx.parent(), ridx );

    QModelIndex iidx = m_model.index( 0, 0, aidx ); // first interval
    QVERIFY( iidx.isValid() );
    rows = m_model.rowCount( iidx ); // intervals don't have children
    QCOMPARE( rows, 0 );
    QCOMPARE( iidx.parent(), aidx );

    // external appointment
    aidx = m_model.index( 1, 0, ridx ); // second appointment (external)
    QVERIFY( aidx.isValid() );
    rows = m_model.rowCount( aidx ); // num intervals
    QCOMPARE( rows, 2 );
    QCOMPARE( aidx.parent(), ridx );

    iidx = m_model.index( 0, 0, aidx ); // first interval
    QVERIFY( iidx.isValid() );
    rows = m_model.rowCount( iidx ); // intervals don't have children
    QCOMPARE( rows, 0 );
    QCOMPARE( iidx.parent(), aidx );

    iidx = m_model.index( 1, 0, aidx ); // second interval
    QVERIFY( iidx.isValid() );
    rows = m_model.rowCount( iidx ); // intervals don't have children
    QCOMPARE( rows, 0 );
    QCOMPARE( iidx.parent(), aidx );

    QCOMPARE( t->startTime(), m_calendar->firstAvailableAfter( targetstart + Duration( 1, 0, 0 ), t->endTime() ) );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );
    
}

void ResourceModelTester::externalOverbook()
{
    DateTime targetstart = m_project->constraintStartTime();
    DateTime targetend = m_project->constraintEndTime();
    Task *t = m_task;
    Resource *r = m_resource;

    r->addExternalAppointment( "Ext-1", "External project 1", targetstart, targetstart.addDays( 1 ), 100 );
    r->addExternalAppointment( "Ext-1", "External project 1", targetend.addDays( -1 ), targetend, 100 );

    ScheduleManager *sm = m_project->createScheduleManager( "Test Plan" );
    m_project->addScheduleManager( sm );
    sm->setAllowOverbooking( true );
    m_project->calculate( *sm );
    long id = sm->id();
    m_model.setScheduleManager( sm );
    //printSchedulingLog( *sm );
    //printDebug( id );

    // resource group
    QModelIndex idx;
    int rows = m_model.rowCount( idx );
    QCOMPARE( rows, 1 );
    idx = m_model.index( 0, 0, idx );
    QVERIFY( idx.isValid() );
    
    // resource
    rows = m_model.rowCount( idx );
    QCOMPARE( rows, 1 );
    idx = m_model.index( 0, 0, idx );
    QCOMPARE( m_resource->name(), m_model.data( idx ).toString() );

    idx = m_model.index( m_resource );
    QVERIFY( idx.isValid() );

    // appointments
    rows = m_model.rowCount( idx );
    QCOMPARE( rows, m_resource->numAppointments( id ) + m_resource->numExternalAppointments() );
    QCOMPARE( rows, 2 ); // one internal, one external

    // internal appointment
    QModelIndex aidx = m_model.index( 0, 0, idx ); // first appointment (internal)
    QVERIFY( aidx.isValid() );
    rows = m_model.rowCount( aidx ); // num intervals
    QCOMPARE( rows, 1 );

    QModelIndex iidx = m_model.index( 0, 0, aidx ); // first interval
    QVERIFY( iidx.isValid() );
    rows = m_model.rowCount( iidx ); // intervals don't have children
    QCOMPARE( rows, 0 );

    // external appointment
    aidx = m_model.index( 1, 0, idx ); // second appointment (external)
    QVERIFY( aidx.isValid() );
    rows = m_model.rowCount( aidx ); // num intervals
    QCOMPARE( rows, 2 );

    iidx = m_model.index( 0, 0, aidx ); // first interval
    QVERIFY( iidx.isValid() );
    rows = m_model.rowCount( iidx ); // intervals don't have children
    QCOMPARE( rows, 0 );

    iidx = m_model.index( 1, 0, aidx ); // second interval
    QVERIFY( iidx.isValid() );
    rows = m_model.rowCount( iidx ); // intervals don't have children
    QCOMPARE( rows, 0 );


    QCOMPARE( t->startTime(), m_calendar->firstAvailableAfter( targetstart, t->endTime() ) );
    QCOMPARE( t->endTime(), t->startTime() + Duration( 0, 8, 0 ) );

}


} //namespace KPlato

QTEST_KDEMAIN_CORE( KPlato::ResourceModelTester )

#include "ResourceModelTester.moc"
