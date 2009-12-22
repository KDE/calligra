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
#include "kptrelation.h"

#include <librcps.h>

#include <QString>
#include <QTimer>

#include <KLocale>
#include <KDebug>

#define GENERATION_MIN_LIMIT 5000

#define PROGRESS_CALLBACK_FREQUENCY 100
#define PROGRESS_MAX_VALUE 120000
#define PROGRESS_INIT_VALUE 12000
#define PROGRESS_INIT_STEP 2000

class ProgressInfo
{
public:
    explicit ProgressInfo() : init( true ), base( 0 ), duration( 0 ), progress( 0 ) {}
    bool init;
    int base;
    int duration;
    int progress;
};


KPlatoRCPSScheduler::KPlatoRCPSScheduler( Project *project, ScheduleManager *sm, QObject *parent )
    : QThread(parent),
    result( -1 ),
    m_project( project ),
    m_manager( sm ),
    m_timeunit( 60 ),
    m_progressinfo( new ProgressInfo() ),
    m_stopScheduling(false ),
    m_haltScheduling( false )
{
    m_starttime = sm->recalculate() ? sm->recalculateFrom() : m_project->constraintStartTime();

    connect( this, SIGNAL( started() ), this, SLOT( slotStarted() ) );
    connect( this, SIGNAL( finished() ), this, SLOT( slotFinished() ) );
    
    qDebug()<<"KPlatoRCPSScheduler:"<<m_project->name()<<m_manager->name()<<project->constraintStartTime();
    KLocale *locale = KGlobal::locale();
    m_problem = rcps_problem_new();

    m_manager->createSchedules();
    m_project->setCurrentSchedule( m_manager->expected()->id() );
    MainSchedule *cs = static_cast<MainSchedule*>( m_project->currentSchedule() );
    Q_ASSERT( cs );
    m_project->initiateCalculation( *cs );
    m_project->initiateCalculationLists( *cs );
    cs->setPhaseName( 0, i18n( "Init" ) );
    cs->logInfo( i18n( "Schedule project using RCPS Scheduler, starting at %1", m_project->locale()->formatDateTime( QDateTime::currentDateTime() ) ), 0 );
    cs->logInfo( i18n( "Project start %1", m_project->locale()->formatDateTime( project->constraintStartTime() ) ), 0 );
    
    connect( this, SIGNAL( maxProgress( int ) ), project, SIGNAL( maxProgress( int ) ) );
    connect( this, SIGNAL( sigProgress( int ) ), project, SIGNAL( sigProgress( int ) ) );
    connect( this, SIGNAL( sigCalculationFinished( Project*, ScheduleManager* ) ), project, SIGNAL( sigCalculationFinished( Project*, ScheduleManager* ) ) );
}

KPlatoRCPSScheduler::~KPlatoRCPSScheduler()
{
    delete m_progressinfo;
    qDeleteAll( m_duration_info_list );
    rcps_problem_free( m_problem );
    qDebug()<<"KPlatoRCPSScheduler::~KPlatoRCPSScheduler()";
}

int KPlatoRCPSScheduler::progress_callback( int generations, int duration, void *arg )
{
    if ( arg == 0 ) {
        return -1;
    }
    KPlatoRCPSScheduler *self = static_cast<KPlatoRCPSScheduler*>( arg );
    //qDebug()<<"KPlatoRCPSScheduler::progress_callback"<<generations<<duration<<self;
    return self->progress( generations, duration );
}

int KPlatoRCPSScheduler::progress( int generations, int duration )
{
    //qDebug()<<"KPlatoRCPSScheduler::progress"<<generations;
    if ( m_stopScheduling || m_haltScheduling ) {
        m_manager->expected()->logWarning( i18n( "Scheduling halted after %1 generations" ).arg( generations ), 1 );
        return -1;
    }
    if ( m_progressinfo->init ) {
        if ( generations == 0 ) {
            m_progressinfo->progress += PROGRESS_INIT_STEP;
        } else {
            m_progressinfo->progress = PROGRESS_INIT_VALUE;
            m_progressinfo->init = false;
        }
    } else {
        m_progressinfo->progress = PROGRESS_INIT_VALUE + generations;
    }
    if ( m_progressinfo->duration != duration ) {
        m_progressinfo->duration = duration;
        m_progressinfo->base = generations;
    }
    emit sigProgress( m_progressinfo->progress );
    int result = ( generations >= m_progressinfo->base + GENERATION_MIN_LIMIT ? 1 : 0 );
    if ( result ) {
        qDebug()<<"KPlatoRCPSScheduler::progress, stop after"<<generations<<"generations, progress:"<<m_progressinfo->progress;
        m_manager->expected()->logDebug( QString( "Acceptable solution found after %1 generations" ).arg( generations ), 1 );
    }
    return result;
}

int KPlatoRCPSScheduler::duration_callback( int direction, int time, int nominal_duration, void *arg )
{
    //qDebug()<<"kplato_duration:"<<direction<<time<<nominal_duration<<arg;
    if ( arg == 0 ) {
        return nominal_duration;
    }
    KPlatoRCPSScheduler::duration_info *info = static_cast<KPlatoRCPSScheduler::duration_info*>( arg );
    return info->self->duration( direction, time, nominal_duration, info );
}

int KPlatoRCPSScheduler::duration( int direction, int time, int nominal_duration,  KPlatoRCPSScheduler::duration_info  *info )
{
    ++(info->calls);
    if ( info->cache.contains( time ) ) {
        return info->cache[ time ];
    }
    if ( m_haltScheduling ) {
        return nominal_duration;
    }
    if ( m_manager->recalculate() && info->task->completion().isFinished() ) {
        return 0;
    }
    int dur = 0;
    if ( info->estimatetype == Estimate::Type_Effort ) {
        dur = info->task->requests().duration(
                    info->requests,
                    m_starttime.addSecs( time * m_timeunit ), 
                    info->estimate,
                    0, /*no schedule*/
                    direction
                ).seconds() / m_timeunit;
    } else {
        dur = info->task->length( 
                    m_starttime.addSecs( time * m_timeunit ), 
                    info->estimate,
                    0, /*no schedule*/
                    direction
                ).seconds() / m_timeunit;
    }
    info->cache[ time ] = dur;
    //qDebug()<<info->task->name()<<time<<"("<<m_starttime.addSecs( time * m_timeunit )<<")"<<dur<<"("<<((double)(dur)*m_timeunit/3600.)<<"hours )";
    return dur;
}

void KPlatoRCPSScheduler::doRun()
{
    emit maxProgress( PROGRESS_MAX_VALUE );
    slotStarted();
    run();
    slotFinished();
}

void KPlatoRCPSScheduler::run()
{
    m_manager->expected()->setPhaseName( 1, i18n( "Schedule" ) );
    emit maxProgress( PROGRESS_MAX_VALUE );
    solve();
    if ( result != 0 ) {
        m_manager->expected()->logError( i18n( "Solution is not valid: %1", result, 1 ) );
        return;
    }
    kplatoFromRCPS();
}

void KPlatoRCPSScheduler::slotStarted()
{
    qDebug()<<"KPlatoRCPSScheduler::slotStarted:";
    emit jobStarted( this );
    m_progressinfo->progress += PROGRESS_INIT_STEP / 5;
    emit sigProgress( m_progressinfo->progress );
}

void KPlatoRCPSScheduler::slotFinished()
{
    qDebug()<<"KPlatoRCPSScheduler::slotFinished:";
    emit sigProgress( PROGRESS_MAX_VALUE );
    emit sigCalculationFinished( m_project, m_manager );
    emit jobFinished( this );
}

int KPlatoRCPSScheduler::check()
{
    return rcps_check( m_problem );
}

void KPlatoRCPSScheduler::solve()
{
    struct rcps_solver *s = rcps_solver_new();
    rcps_solver_set_progress_callback(s, PROGRESS_CALLBACK_FREQUENCY, this, &KPlatoRCPSScheduler::progress_callback);
    rcps_solver_set_duration_callback(s, &KPlatoRCPSScheduler::duration_callback );

    rcps_solver_solve( s, m_problem );
    result = rcps_solver_getwarnings( s );
    rcps_solver_free( s );
}

int KPlatoRCPSScheduler::kplatoToRCPS()
{
    addResources();
    addTasks();
    addDependencies();
    addRequests();
    
    return check();
}

void KPlatoRCPSScheduler::taskFromRCPS( struct rcps_job *job, Task *task, QMap<Node*, QList<ResourceRequest*> > &resourcemap )
{
    KLocale *locale = m_project->locale();
    Schedule *cs = task->currentSchedule();
    Q_ASSERT( cs );
    struct rcps_mode *mode = rcps_mode_get(job, rcps_job_getmode_res(job));
    /* get the duration of this mode */
    KPlatoRCPSScheduler::duration_info *info = static_cast<KPlatoRCPSScheduler::duration_info*>( rcps_mode_get_cbarg(mode) );
    qint64 dur = 0;
    qint64 st = rcps_job_getstart_res(job);
    if ( info == 0 ) {
        dur = rcps_mode_getduration(mode);
    } else {
        cs->logDebug( i18n( "Task '%1' estimate: %2", task->name(), task->estimate()->value( Estimate::Use_Expected, false ).toString() ), 1 );
        cs->logDebug( i18n( "Task '%1' duration called %2 times, cached values: %3", rcps_job_getname(job), info->calls, info->cache.count() ) );

        dur = duration_callback( 0, st, rcps_mode_getduration(mode), info );

        for ( QMap<int, int>::ConstIterator it = info->cache.constBegin(); it != info->cache.constEnd(); ++it ) {
            cs->logDebug( i18n( "Task '%1' start: %2, duration: %3 (%4, %5 hours)", rcps_job_getname(job), it.key(), it.value(), m_starttime.addSecs( (it.key()*m_timeunit)).toString(), (double)(it.value())/60.0 ), 1 );
        }
    }
    DateTime start = m_starttime.addSecs(st * m_timeunit);
    DateTime end = start + Duration( dur * m_timeunit * 1000 );
    cs->logDebug( i18n( "Task '%1' start=%2, duration=%3: %4 - %5", rcps_job_getname(job), st, dur, locale->formatDateTime( start ), locale->formatDateTime( end ) ), 1 );

    task->setStartTime( start );
    task->setEndTime( end );
    for ( int reqs = 0; reqs < rcps_request_count(mode); ++reqs ) {
        struct rcps_request *req = rcps_request_get(mode, reqs);
        struct rcps_alternative *alt = rcps_alternative_get(req, rcps_request_getalternative_res(req));
        int amount = rcps_alternative_getamount(alt);
        struct rcps_resource *res = rcps_alternative_getresource(alt);

        cs->logDebug( i18n( "Job %1: resource %2 is %3 available", rcps_job_getname(job), rcps_resource_getname(res), amount ), 1 );
        
        // do actual appoinments etc
        ResourceRequest *r = m_requestmap.value( req );
        if ( r == 0 ) {
            cs->logWarning( i18n( "No resource request is registered" ), 1 );
            continue;
        }
        resourcemap[ task ] << r;
        cs->logInfo( i18n( "Make appointments to resource %1", r->resource()->name() ), 1 );
        r->makeAppointment( cs, amount );
    }
    if ( m_manager->recalculate() ) {
        if ( task->completion().isFinished() ) {
            task->copySchedule();
            cs->logInfo( i18n( "Task is completed, copied schedule: %2 to %3", task->name(), locale->formatDateTime( task->startTime() ), locale->formatDateTime( task->endTime() ) ), 1 );
        } else if ( task->completion().isStarted() ) {
            task->copyAppointments( DateTime(), start );
            cs->logInfo( i18n( "Task is %4% completed, copied appointments from %2 to %3", task->name(), locale->formatDateTime( task->startTime() ), locale->formatDateTime( start ), task->completion().percentFinished() ), 1 );
        }
    }
    cs->setScheduled( true );
    if ( task->estimate()->type() == Estimate::Type_Effort ) {
        if ( task->appointmentStartTime().isValid() ) {
            task->setStartTime( task->appointmentStartTime() );
        }
        if ( task->appointmentEndTime().isValid() ) {
            task->setEndTime( task->appointmentEndTime() );
        }
    } else if ( task->estimate()->calendar() ) {
        DateTime t = task->estimate()->calendar()->firstAvailableAfter( task->startTime(), task->endTime() );
        if ( t.isValid() ) {
            task->setStartTime( t );
        }
        t = task->estimate()->calendar()->firstAvailableBefore( task->endTime(), task->startTime() );
        if ( t.isValid() ) {
            task->setEndTime( t );
        }
    } //else  Fixed duration
    task->setDuration( task->endTime() - task->startTime() );
    cs->logInfo( i18n( "Scheduled from %1 to %2", locale->formatDateTime( task->startTime() ), locale->formatDateTime( task->endTime() ) ), 1 );
}

void KPlatoRCPSScheduler::kplatoFromRCPS()
{
    MainSchedule *cs = static_cast<MainSchedule*>( m_project->currentSchedule() );
    QMap<Node*, QList<ResourceRequest*> > resourcemap;
    int count = rcps_job_count(m_problem);
    int step = ( PROGRESS_MAX_VALUE - m_progressinfo->progress ) / count;
    DateTime projectstart = m_starttime.addSecs( rcps_job_getstart_res(m_jobstart) * m_timeunit );
    for ( int i = 0; i < count; ++i ) {
        m_progressinfo->progress += step;
        emit sigProgress( m_progressinfo->progress );

        struct rcps_job *job = rcps_job_get( m_problem, i );
        Task *task = m_taskmap.value( job );
        if ( task == 0 ) {
            continue; //might be dummy task for lag, ...
        }
        taskFromRCPS( job, task, resourcemap );

        if ( projectstart > task->startTime() ) {
            projectstart = task->startTime();
        }
    }
    qint64 st = rcps_job_getstart_res(m_jobstart) * m_timeunit;
    DateTime start = m_starttime.addSecs( st );
    qint64 et = rcps_job_getstart_res(m_jobend) * m_timeunit;
    DateTime end = m_starttime.addSecs( et );
    m_project->setStartTime( projectstart );
    m_project->setEndTime( end );
    cs->logInfo( i18n( "Project scheduled to start at %1 and finish at %2", m_project->locale()->formatDateTime( projectstart ), m_project->locale()->formatDateTime( end ) ), 1 );

    m_project->adjustSummarytask();
    
    calculatePertValues( resourcemap );

    cs->logDebug( i18n( "Project scheduling finished at %1", QDateTime::currentDateTime().toString() ), 1 );
    m_project->finishCalculation( *m_manager );
    m_manager->scheduleChanged( cs );
}

void KPlatoRCPSScheduler::calculatePertValues( const QMap<Node*, QList<ResourceRequest*> > &map )
{
    m_manager->expected()->setPhaseName( 2, i18n( "PERT" ) );
    foreach ( Node *n, m_project->allNodes() ) {
        if ( n->type() != Node::Type_Task && n->type() != Node::Type_Milestone ) {
            continue;
        }
        if ( n->isStartNode() ) {
            (void)calculateLateStuff( map, static_cast<Task*>( n ) );
        }
        if ( n->isEndNode() ) {
            (void)calculateEarlyStuff( map,  static_cast<Task*>( n ) );
        }
    }
}

Duration KPlatoRCPSScheduler::calculateLateStuff( const QMap<Node*, QList<ResourceRequest*> > &map, Task *task )
{
    Schedule *cs = task->currentSchedule();
    Duration pf = m_project->endTime() - m_project->startTime();
    QList<Relation*> lst = task->dependChildNodes() + task->childProxyRelations();
    if ( lst.isEmpty() ) {
        // end node
        DateTime end = task->endTime();
        if ( task->estimate()->type() == Estimate::Type_Effort ) {
            foreach ( ResourceRequest *r, map.value( static_cast<Node*>( task ) ) ) {
                DateTime x = r->resource()->availableBefore( m_project->endTime(), task->endTime(), 0 );
                if ( cs ) cs->logDebug( QString( "Resource '%1' available before %2: %3" ).arg( r->resource()->name() ).arg(  m_project->endTime().toString() ).arg( x.toString() ), 2 );
                if ( x.isValid() && x > end ) {
                    end = x;
                }
            }
        } else if ( task->estimate()->calendar() ) {
            end = task->estimate()->calendar()->firstAvailableBefore( m_project->endTime(), task->endTime() );
            if ( cs ) cs->logDebug( QString( "Calendar work time before %1: %2" ).arg(  m_project->endTime().toString() ).arg( end.toString() ), 2 );
        }
        // TODO must calculate backwards to get late *start* of task
        pf = end.isValid() ? end - task->endTime() : m_project->endTime() - task->endTime();
        task->setFreeFloat( pf );
    } else {
        Duration prev = pf;
        Duration x = pf;
        foreach ( Relation *r, lst ) {
            prev = qMin( prev, calculateLateStuff( map, static_cast<Task*>( r->child() ) ) );
            DateTime end = task->endTime();
            if ( task->estimate()->type() == Estimate::Type_Effort ) {
                foreach ( ResourceRequest *req, map.value( static_cast<Node*>( task ) ) ) {
                    DateTime y = req->resource()->availableBefore( r->child()->startTime(), task->endTime(), 0 );
                    if ( cs ) cs->logDebug( QString( "Resource '%1' available before %2: %3" ).arg( req->resource()->name() ).arg(  r->child()->startTime().toString() ).arg( y.toString() ), 2 );
                    if ( y.isValid() && y > end ) {
                        end = y;
                    }
                }
            } else if ( task->estimate()->calendar() ) {
                end = task->estimate()->calendar()->firstAvailableBefore( r->child()->startTime(), task->endTime() );
                if ( cs ) cs->logDebug( QString( "Calendar work time before %1: %2" ).arg(  r->child()->startTime().toString() ).arg( end.toString() ), 2 );
            }
            x = qMin( x, end.isValid() ? end - task->endTime() : r->child()->startTime() - task->endTime() );
        }
        task->setFreeFloat( x );
        // TODO must calculate backwards to get late *start* of task
        pf = prev + x;
    }
    task->setPositiveFloat( pf );
    task->setLateFinish( task->endTime() + pf );
    task->setLateStart( task->lateFinish() - ( task->endTime() - task->startTime() ) );
    if ( cs ) cs->logInfo( i18n( "Late start %1, late finish %2, positive float %3", m_project->locale()->formatDateTime( task->lateStart() ), m_project->locale()->formatDateTime( task->lateFinish() ), pf.toString() ), 2 );
    return pf;
}

Duration KPlatoRCPSScheduler::calculateEarlyStuff( const QMap<Node*, QList<ResourceRequest*> > &map, Task *task )
{
    Schedule *cs = task->currentSchedule();
    Duration tot = m_project->endTime() - m_project->startTime();
    QList<Relation*> lst = task->dependParentNodes() + task->parentProxyRelations();
    if ( lst.isEmpty() ) {
        // start node
        DateTime earlystart = task->startTime();
        if ( task->estimate()->type() == Estimate::Type_Effort ) {
            foreach ( ResourceRequest *r, map.value( static_cast<Node*>( task ) ) ) {
                DateTime x = r->resource()->availableAfter( m_project->startTime(), task->startTime(), 0 );
                if ( cs ) cs->logDebug( QString( "Resource '%1' available after %2 (%4): %3" ).arg( r->resource()->name() ).arg(  m_project->startTime().toString() ).arg( x.toString() ).arg( task->startTime().toString() ), 2 );
                if ( x.isValid() && x < earlystart ) {
                    earlystart = x;
                }
            }
        } else if ( task->estimate()->calendar() ) {
            earlystart = task->estimate()->calendar()->firstAvailableAfter( m_project->startTime(), task->startTime() );
            if ( cs ) cs->logDebug( QString( "Calendar work time after %1: %2" ).arg(  m_project->startTime().toString() ).arg( earlystart.toString() ), 2 );
        }
        // TODO must calculate forward to get early *????* of task
        tot = earlystart.isValid() ? task->startTime() - earlystart : task->startTime() - m_project->startTime();
    } else {
        Duration prev = tot;
        Duration x = tot;
        foreach ( Relation *r, lst ) {
            prev = qMin( prev, calculateEarlyStuff( map, static_cast<Task*>( r->parent() ) ) );
            DateTime earlystart = task->startTime();
            if ( task->estimate()->type() == Estimate::Type_Effort ) {
                foreach ( ResourceRequest *req, map.value( static_cast<Node*>( task ) ) ) {
                    DateTime y = req->resource()->availableAfter( r->parent()->endTime(), task->startTime(), 0 );
                    if ( cs ) cs->logDebug( QString( "Resource '%1' available after %2: %3" ).arg( req->resource()->name() ).arg(  r->parent()->endTime().toString() ).arg( y.toString() ), 2 );
                    if ( y.isValid() && y < earlystart ) {
                        earlystart = y;
                    }
                }
            } else if ( task->estimate()->calendar() ) {
                earlystart = task->estimate()->calendar()->firstAvailableAfter( r->parent()->endTime(), task->startTime() );
                if ( cs ) cs->logDebug( QString( "Calendar work time after %1: %2" ).arg(  r->parent()->endTime().toString() ).arg( earlystart.toString() ), 2 );
            }
            x = qMin( x, earlystart.isValid() ? task->startTime() - earlystart : task->startTime() - r->parent()->startTime() );
        }
        // TODO must calculate backwards to get late *start* of task
        tot = prev + x;
    }
    task->setEarlyStart( task->startTime() - tot );
    task->setEarlyFinish( task->earlyStart() + ( task->endTime() - task->startTime() ) );
    if ( cs ) cs->logInfo( i18n( "Early start %1, early finish %2", m_project->locale()->formatDateTime( task->earlyStart() ), m_project->locale()->formatDateTime( task->earlyFinish() ) ), 2 );
    return tot;
}

struct rcps_resource *KPlatoRCPSScheduler::addResource( KPlato::Resource *r)
{
    struct rcps_resource *res = rcps_resource_new();
    rcps_resource_setname( res, r->name().toLocal8Bit().data() );
    rcps_resource_setavail( res, r->units() );
    rcps_resource_add( m_problem, res );
    m_resourcemap[res] = r;
    return res;
}

void KPlatoRCPSScheduler::addResources()
{
    kDebug();
    QList<Resource*> list = m_project->resourceList();
    for (int i = 0; i < list.count(); ++i) {
        struct rcps_resource *res = addResource( list.at(i) );
    }
    for( int i = 0; i < rcps_resource_count( m_problem ); ++i ) {
        kDebug()<<"Resource:"<<rcps_resource_getname( rcps_resource_get(m_problem, i) );
    }
}

struct rcps_job *KPlatoRCPSScheduler::addTask( KPlato::Task *task )
{
    struct rcps_job *job = rcps_job_new();
    rcps_job_setname( job, task->name().toLocal8Bit().data() );
    rcps_job_add( m_problem, job );
    m_taskmap[job] = task;
    return job;
}

void KPlatoRCPSScheduler::addTasks()
{
    kDebug();
    // Add a start job
    m_jobstart = rcps_job_new();
    rcps_job_setname( m_jobstart, "RCPS start job" );
    rcps_job_add( m_problem, m_jobstart );
    struct rcps_mode *mode = rcps_mode_new();
    rcps_mode_add( m_jobstart, mode );

    QList<Node*> list = m_project->allNodes();
    for (int i = 0; i < list.count(); ++i) {
        Node *n = list.at(i);
        if ( n->type() != Node::Type_Task && n->type() != Node::Type_Milestone ) {
            continue;
        }
        struct rcps_job *job = addTask( static_cast<Task*>( n ) );
    }
    // Add an end job
    m_jobend = rcps_job_new();
    rcps_job_setname( m_jobend, "RCPS end job" );
    rcps_job_add( m_problem, m_jobend );
    mode = rcps_mode_new();
    rcps_mode_add( m_jobend, mode );

    for( int i = 0; i < rcps_job_count( m_problem ); ++i ) {
        kDebug()<<"Task:"<<rcps_job_getname( rcps_job_get(m_problem, i) );
    }
}

struct rcps_job *KPlatoRCPSScheduler::addJob( const QString &name, int duration )
{
    struct rcps_job *job = rcps_job_new();
    rcps_job_setname( job, name.toLocal8Bit().data() );
    rcps_job_add( m_problem, job );
    struct rcps_mode *mode = rcps_mode_new();
    rcps_mode_setduration( mode, duration );
    rcps_mode_add( job, mode );
    return job;
}

void KPlatoRCPSScheduler::addDependencies( struct rcps_job *job, KPlato::Task *task )
{
    if ( task->dependParentNodes().isEmpty() && task->parentProxyRelations().isEmpty() ) {
        rcps_job_successor_add( m_jobstart, job, SUCCESSOR_FINISH_START );
    }
    if ( task->dependChildNodes().isEmpty() && task->childProxyRelations().isEmpty() ) {
        rcps_job_successor_add( job, m_jobend, SUCCESSOR_FINISH_START );
    }
    foreach ( Relation *r, task->dependChildNodes() ) {
        Node *n = r->child();
        if ( n == 0 || n->type() == Node::Type_Summarytask ) {
            continue;
        }
        int type = SUCCESSOR_FINISH_START;
        switch ( r->type() ) {
            case Relation::FinishStart: type = SUCCESSOR_FINISH_START; break;
            case Relation::FinishFinish: type = SUCCESSOR_FINISH_FINISH; break;
            case Relation::StartStart: type = SUCCESSOR_START_START; break;
        }
        if ( r->lag() == Duration::zeroDuration ) {
            rcps_job_successor_add( job, m_taskmap.key( static_cast<Task*>( n ) ), type );
        } else {
            // Add a dummy job to represent the lag
            struct rcps_job *dummy = addJob( r->lag().toString(), r->lag().seconds() / m_timeunit );
            rcps_job_successor_add( job, dummy, type );
            int t = type == SUCCESSOR_FINISH_FINISH ? type : SUCCESSOR_FINISH_START;
            rcps_job_successor_add( dummy, m_taskmap.key( static_cast<Task*>( n ) ), t );
        }
    }
    foreach ( Relation *r, task->childProxyRelations() ) {
        Node *n = r->child();
        if ( n == 0 || n->type() == Node::Type_Summarytask ) {
            continue;
        }
        int type = SUCCESSOR_FINISH_START;
        switch ( r->type() ) {
            case Relation::FinishStart: type = SUCCESSOR_FINISH_START; break;
            case Relation::FinishFinish: type = SUCCESSOR_FINISH_FINISH; break;
            case Relation::StartStart: type = SUCCESSOR_START_START; break;
        }
        if ( r->lag() == Duration::zeroDuration ) {
            rcps_job_successor_add( job, m_taskmap.key( static_cast<Task*>( n ) ), type );
        } else {
            // Add a dummy job to represent the lag
            struct rcps_job *dummy = addJob( r->lag().toString(), r->lag().seconds() / m_timeunit );
            rcps_job_successor_add( job, dummy, type );
            int t = type == SUCCESSOR_FINISH_FINISH ? type : SUCCESSOR_FINISH_START;
            rcps_job_successor_add( dummy, m_taskmap.key( static_cast<Task*>( n ) ), t );
        }
    }
}

void KPlatoRCPSScheduler::addDependencies()
{
    kDebug();
    QMap<struct rcps_job*, Task*> ::const_iterator it = m_taskmap.constBegin();
    for ( ; it != m_taskmap.constEnd(); ++it ) {
        addDependencies(  it.key(), it.value() );
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

void KPlatoRCPSScheduler::addRequest( rcps_job *job, Task *task )
{
    kDebug();
    struct rcps_mode *mode = rcps_mode_new();
    rcps_mode_add( job, mode );
    if ( task->type() == Node::Type_Milestone || task->estimate() == 0 || ( m_manager->recalculate() && task->completion().isFinished() ) ) {
        rcps_mode_setduration(mode, 0);
        return;
    }
    if ( task->estimate()->type() == Estimate::Type_Duration && task->estimate()->calendar() == 0 ) {
        // Fixed duration, no callback needed
        rcps_mode_setduration(mode, task->estimate()->value( Estimate::Use_Expected, m_manager->usePert() ).seconds() / m_timeunit );
        return;
    }
    /* set the argument for the duration callback */
    struct KPlatoRCPSScheduler::duration_info *info = new KPlatoRCPSScheduler::duration_info;
    info->self = this;
    info->calls = 0;
    info->task = task;
    info->estimate = task->estimate()->value( Estimate::Use_Expected, m_manager->usePert() );
    info->requests = task->requests().resourceRequests();
    info->estimatetype = task->estimate()->type();

    rcps_mode_set_cbarg( mode, info );
    m_duration_info_list << info;

    foreach ( ResourceRequest *rr, info->requests ) {
        Resource *r = rr->resource();
        struct rcps_request *req = rcps_request_new();
        rcps_request_add( mode, req );
        m_requestmap[ req ] = rr;
        struct rcps_alternative *alt = rcps_alternative_new();
        rcps_alternative_setresource( alt, m_resourcemap.key( r ) );
        rcps_alternative_setamount( alt, (double)r->units() * 100 / rr->units() );
        rcps_alternative_add( req, alt );
    }
}

#include "KPlatoRCPSScheduler.moc"
