/* This file is part of the KDE project
 * Copyright (C) 2009 Dag Andersen <danders@get2net.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KPlatoRCPSScheduler.h"

#include "kptproject.h"
#include "kptschedule.h"
#include "kptresource.h"
#include "kpttask.h"

#include <librcps.h>

#include <QString>

#include <KDebug>


KPlatoRCPSScheduler::KPlatoRCPSScheduler( Project &project, ScheduleManager *sm, QObject *parent )
    : QObject(parent),
    m_project( project ),
    m_manager( *sm )
{
    kDebug()<<m_project.name()<<m_manager.name();
    m_problem = rcps_problem_new();

    m_manager.createSchedules();
    m_project.initiateCalculation( *(m_manager.expected()) );
    m_project.initiateCalculationLists( *(m_manager.expected()) );

    addResources();
    addTasks();
    addDependencies();
    addRequests();

    kDebug()<<"Check: "<<rcps_check( m_problem );

    struct rcps_solver *s = rcps_solver_new();
//    rcps_solver_solve( s, m_problem );
    kDebug()<<"Resoult: "<< rcps_solver_getwarnings( s );
    rcps_solver_free( s );
}

KPlatoRCPSScheduler::~KPlatoRCPSScheduler()
{
    rcps_problem_free( m_problem );
    kDebug();
}

void KPlatoRCPSScheduler::addResources()
{
    kDebug();
    QList<Resource*> list = m_project.resourceList();
    for (int i = 0; i < list.count(); ++i) {
        struct rcps_resource *res = rcps_resource_new();
        Resource *r = list.at(i);
        rcps_resource_setname( res, r->name().toLocal8Bit().data() );
        rcps_resource_setavail( res, r->units() / 100 );
        rcps_resource_add( m_problem, res );
        m_resourcemap[res] = r;
    }
    for( int i = 0; i < rcps_resource_count( m_problem ); ++i ) {
        kDebug()<<"Resource:"<<rcps_resource_getname( rcps_resource_get(m_problem, i) );
    }
}

void KPlatoRCPSScheduler::addTasks()
{
    kDebug();
    // Add a start job
    m_jobstart = rcps_job_new();
    rcps_job_setname( m_jobstart, "start" );
    rcps_job_add( m_problem, m_jobstart );

    QList<Node*> list = m_project.allNodes();
    for (int i = 0; i < list.count(); ++i) {
        Node *n = list.at(i);
        if ( n->type() != Node::Type_Task && n->type() != Node::Type_Milestone ) {
            continue;
        }
        Task *t = static_cast<Task*>( n );
        struct rcps_job *job = rcps_job_new();
        rcps_job_setname( job, t->name().toLocal8Bit().data() );
        rcps_job_add( m_problem, job );
        m_taskmap[job] = t;
    }
    // Add an end job
    m_jobend = rcps_job_new();
    rcps_job_setname( m_jobend, "end" );
    rcps_job_add( m_problem, m_jobend );

    for( int i = 0; i < rcps_job_count( m_problem ); ++i ) {
        kDebug()<<"Task:"<<rcps_job_getname( rcps_job_get(m_problem, i) );
    }
}

void KPlatoRCPSScheduler::addDependencies()
{
    kDebug();
    QMap<struct rcps_job*, Task*> ::const_iterator it = m_taskmap.constBegin();
    for ( ; it != m_taskmap.constEnd(); ++it ) {
        if ( it.value()->dependParentNodes().isEmpty() && it.value()->parentProxyRelations().isEmpty() ) {
            rcps_job_successor_add( m_jobstart, it.key() );
        }
        if ( it.value()->dependChildNodes().isEmpty() && it.value()->childProxyRelations().isEmpty() ) {
            rcps_job_successor_add( it.key(), m_jobend );
        }
        foreach ( Relation *r, it.value()->dependChildNodes() ) {
            rcps_job_successor_add( it.key(), m_taskmap.key( static_cast<Task*>( r->child() ) ) );
        }
        foreach ( Relation *r, it.value()->childProxyRelations() ) {
            rcps_job_successor_add( it.key(), m_taskmap.key( static_cast<Task*>( r->child() ) ) );
        }
    }
}

void KPlatoRCPSScheduler::addRequests()
{
    kDebug();
    QMap<struct rcps_job*, Task*> ::const_iterator it = m_taskmap.constBegin();
    for ( ; it != m_taskmap.constEnd(); ++it ) {
        addRequest( it.key(), it.value() );
    }
}

void KPlatoRCPSScheduler::addRequest( rcps_job *job, const Task *task )
{
    kDebug();
    struct rcps_mode *mode = rcps_mode_new();
    rcps_mode_add( job, mode );
    foreach ( Resource *r, task->requestedResources() ) {
        struct rcps_request *req = rcps_request_new();
        rcps_request_add( mode, req );
        struct rcps_alternative *alt = rcps_alternative_new();
        rcps_alternative_setresource( alt, m_resourcemap.key( r ) );
        rcps_alternative_add( req, alt );
    }
}

#include "KPlatoRCPSScheduler.moc"
