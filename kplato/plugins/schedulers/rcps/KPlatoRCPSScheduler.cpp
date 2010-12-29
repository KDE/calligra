/* This file is part of the KDE project
 * Copyright (C) 2009, 2010 Dag Andersen <danders@get2net.dk>
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
#include <QMutexLocker>

#include <KGlobal>
#include <KLocale>
#include <KDebug>

#define GENERATION_MIN_LIMIT 5000

#define PROGRESS_CALLBACK_FREQUENCY 100
#define PROGRESS_MAX_VALUE 120000
#define PROGRESS_INIT_VALUE 12000
#define PROGRESS_INIT_STEP 2000

/* low weight == late, high weight == early */
#define WEIGHT_ASAP     10
#define WEIGHT_ALAP     1
#define WEIGHT_SNE      10
#define WEIGHT_MSO      10
#define WEIGHT_FNL      8
#define WEIGHT_MFO      3
#define WEIGHT_FI       10

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
    : SchedulerThread( project, sm, parent ),
    result( -1 ),
    m_schedule( 0 ),
    m_recalculate( false ),
    m_usePert( false ),
    m_backward( false ),
    m_problem( 0 ),
    m_timeunit( 60 ),
    m_offsetFromTime_t( 0 ),
    m_progressinfo( new ProgressInfo() )
{
    connect(this, SIGNAL(sigCalculationStarted( Project*, ScheduleManager*)), project, SIGNAL(sigCalculationStarted( Project*, ScheduleManager*)));
    emit sigCalculationStarted( project, sm );

    connect( this, SIGNAL( sigCalculationFinished( Project*, ScheduleManager* ) ), project, SIGNAL( sigCalculationFinished( Project*, ScheduleManager* ) ) );
}

KPlatoRCPSScheduler::~KPlatoRCPSScheduler()
{
    delete m_progressinfo;
    qDeleteAll( m_duration_info_list );
    qDeleteAll( m_weight_info_list );
    rcps_problem_free( m_problem );
}

KLocale *KPlatoRCPSScheduler::locale() const
{
    return KGlobal::locale();
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
    if ( m_haltScheduling ) {
        qDebug()<<"KPlatoRCPSScheduler::progress:"<<"halt";
        return -1;
    }
    if ( m_stopScheduling ) {
        m_schedule->logWarning( i18n( "Scheduling halted after %1 generations", generations ), 1 );
        qDebug()<<"KPlatoRCPSScheduler::progress:"<<"stop";
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
    m_manager->setProgress( m_progressinfo->progress );
    setProgress( m_progressinfo->progress );
    int result = ( generations >= m_progressinfo->base + GENERATION_MIN_LIMIT ? 1 : 0 );
    if ( result ) {
        //qDebug()<<"KPlatoRCPSScheduler::progress, stop after"<<generations<<"generations, progress:"<<m_progressinfo->progress;
        m_schedule->logDebug( QString( "Acceptable solution found after %1 generations" ).arg( generations ), 1 );
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
    if ( m_haltScheduling || m_manager == 0 ) {
        return nominal_duration;
    }
    ++(info->calls);
    if ( info->cache.contains( time ) ) {
        return info->cache[ time ];
    }
    if ( m_manager->recalculate() && info->task->completion().isFinished() ) {
        return 0;
    }
    int dur = 0;
    if ( info->task->constraint() == Node::FixedInterval ) {
        // duration may depend on daylight saving so we need to calculate
        // NOTE: dur may not be correct if time != info->task->constraintStartTime, let's see what happends...
        dur = ( info->task->constraintEndTime() - info->task->constraintStartTime() ).seconds() / m_timeunit;
    } else if ( info->estimatetype == Estimate::Type_Effort ) {
        dur = info->task->requests().duration(
                    info->requests,
                    m_starttime.addSecs( time * m_timeunit ), 
                    info->estimate,
                    0, /*no schedule*/
                    m_backward ? ! direction : direction
                ).seconds() / m_timeunit;
    } else {
        dur = info->task->length( 
                    m_starttime.addSecs( time * m_timeunit ), 
                    info->estimate,
                    0, /*no schedule*/
                    m_backward ? ! direction : direction
                ).seconds() / m_timeunit;
    }
    info->cache[ time ] = dur;
    //info->task->schedule()->logDebug( QString( "Time=%1, duration=%2 ( %3, %4 )" ).arg( time ).arg( dur ).arg( fromRcpsTime( time ).toString() ).arg( Duration( (qint64)(dur) * m_timeunit * 1000 ).toDouble( Duration::Unit_h ) ) );
    return dur;
}

int KPlatoRCPSScheduler::weight_callback( int time, int duration, int nominal_weight, void *arg )
{
    //qDebug()<<"kplato_weight:"<<time<<nominal_duration<<arg;
    Q_ASSERT( arg );
    if ( arg == 0 ) {
        return nominal_weight;
    }
    KPlatoRCPSScheduler::weight_info *info = static_cast<KPlatoRCPSScheduler::weight_info*>( arg );
    return info->self->weight( time, duration, nominal_weight, info );
}

int KPlatoRCPSScheduler::weight( int time, int duration, int nominal_weight, KPlatoRCPSScheduler::weight_info  *info )
{
    if ( m_haltScheduling || m_manager == 0 ) {
        return nominal_weight;
    }
    if ( m_manager->recalculate() && info->task->completion().isFinished() ) {
        return 0;
    }
    int w = info->weight;
    if ( info->isEndJob ) {
        int t = time + duration;
        if ( m_backward ) {
            if ( t < info->targettime ) {
                w = info->targettime - t * 10;
            }
        } else {
            if ( t > info->targettime ) {
                w = t - info->targettime * 10;
            }
        }
    }
    return w;
}

void KPlatoRCPSScheduler::run()
{
    if ( m_haltScheduling ) {
        deleteLater();
        return;
    }
    if ( m_stopScheduling ) {
        return;
    }
    { // mutex -->
        m_projectMutex.lock();
        m_managerMutex.lock();

        m_project = new Project();
        loadProject( m_project, m_pdoc );
        m_project->setName( "Schedule: " + m_project->name() ); //Debug

        m_manager = m_project->scheduleManager( m_mainmanagerId );
        Q_CHECK_PTR( m_manager );
        Q_ASSERT( m_manager->expected() );
        Q_ASSERT( m_manager != m_mainmanager );
        Q_ASSERT( m_manager->scheduleId() == m_mainmanager->scheduleId() );
        Q_ASSERT( m_manager->expected() != m_mainmanager->expected() );
        m_manager->setName( "Schedule: " + m_manager->name() ); //Debug
        m_schedule = m_manager->expected();

        bool x = connect(m_manager, SIGNAL(sigLogAdded(Schedule::Log)), this, SLOT(slotAddLog(Schedule::Log)));
        Q_ASSERT( x );

        m_project->initiateCalculation( *m_schedule );
        m_project->initiateCalculationLists( *m_schedule );

        m_problem = rcps_problem_new();
        rcps_problem_setfitness_mode( m_problem, FITNESS_WEIGHT );

        m_usePert = m_manager->usePert();
        m_recalculate = m_manager->recalculate();
        if ( m_recalculate ) {
            m_starttime =  m_manager->recalculateFrom();
            m_backward = false;
        } else {
            m_backward = m_manager->schedulingDirection();
            m_starttime = m_backward ? m_project->constraintEndTime() : m_project->constraintStartTime();
        }

        m_project->setCurrentSchedule( m_manager->expected()->id() );

        m_schedule->setPhaseName( 0, i18n( "Init" ) );
        if ( ! m_backward && locale() ) {
            m_schedule->logDebug( QString( "Schedule project using RCPS Scheduler, starting at %1" ).arg( locale()->formatDateTime( QDateTime::currentDateTime() ) ), 0 );
            if ( m_recalculate ) {
                m_schedule->logInfo( i18n( "Re-calculate project from start time: %1", locale()->formatDateTime( m_starttime ) ), 0 );
            } else {
                m_schedule->logInfo( i18n( "Schedule project from start time: %1", locale()->formatDateTime( m_starttime ) ), 0 );
            }
        }
        if ( m_backward && locale() ) {
            m_schedule->logDebug( QString( "Schedule project backward using RCPS Scheduler, starting at %1" ).arg( locale()->formatDateTime( QDateTime::currentDateTime() ) ), 0 );
            m_schedule->logInfo( i18n( "Schedule project from end time: %1", locale()->formatDateTime( m_starttime ) ), 0 );
        }

        m_managerMutex.unlock();
        m_projectMutex.unlock();
    } // <--- mutex
    m_progressinfo->progress += PROGRESS_INIT_STEP / 5;
    setProgress( m_progressinfo->progress );
 
    result = kplatoToRCPS();
    if ( result != 0 ) {
        if ( locale() ) {
            m_schedule->logError( i18n( "Failed to build a valid RCPS project" ) );
        }
        setProgress( PROGRESS_MAX_VALUE );
        return;
    }
    m_schedule->setPhaseName( 1, i18n( "Schedule" ) );

    setMaxProgress( PROGRESS_MAX_VALUE );
    solve();
    if ( m_haltScheduling ) {
        deleteLater();
        return;
    }
    if ( result != 0 ) {
        m_schedule->logError( i18n( "Invalid scheduling solution. Result: %1", result ), 1 );
    }
    kplatoFromRCPS();
    setProgress( PROGRESS_MAX_VALUE );
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

    Q_ASSERT( check() == 0 );
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
    
    int r = check();
    return r;
}

void KPlatoRCPSScheduler::taskFromRCPSForward( struct rcps_job *job, Task *task, QMap<Node*, QList<ResourceRequest*> > &resourcemap )
{
    if ( m_haltScheduling || m_manager == 0 ) {
        return;
    }
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
        cs->logDebug( QString( "Task '%1' estimate: %2" ).arg( task->name() ).arg( task->estimate()->value( Estimate::Use_Expected, false ).toString() ), 1 );
        cs->logDebug( QString( "Task '%1' duration called %2 times, cached values: %3" ).arg( rcps_job_getname(job) ).arg( info->calls ).arg( info->cache.count() ) );

        dur = duration_callback( 0, st, rcps_mode_getduration(mode), info );

        for ( QMap<int, int>::ConstIterator it = info->cache.constBegin(); it != info->cache.constEnd(); ++it ) {
            cs->logDebug( QString( "Task '%1' start: %2, duration: %3 (%4, %5 hours)" ).arg( rcps_job_getname(job) ).arg( it.key() ).arg( it.value() ).arg( m_starttime.addSecs( (it.key()*m_timeunit)).toString() ).arg( (double)(it.value())/60.0 ), 1 );
        }
    }
    DateTime start = m_starttime.addSecs(st * m_timeunit);
    DateTime end = start + Duration( dur * m_timeunit * 1000 );
    if ( locale() ) {
        cs->logDebug( QString( "Task '%1' start=%2, duration=%3: %4 - %5" ).arg( rcps_job_getname(job) ).arg( st ).arg( dur ).arg( locale()->formatDateTime( start ) ).arg( locale()->formatDateTime( end ) ), 1 );
    }
    task->setStartTime( start );
    task->setEndTime( end );
    for ( int reqs = 0; reqs < rcps_request_count(mode); ++reqs ) {
        struct rcps_request *req = rcps_request_get(mode, reqs);
        struct rcps_alternative *alt = rcps_alternative_get(req, rcps_request_getalternative_res(req));
        int amount = rcps_alternative_getamount(alt);
        struct rcps_resource *res = rcps_alternative_getresource(alt);

        cs->logDebug( QString( "Job %1: resource %2 is %3 available" ).arg( rcps_job_getname(job) ).arg( rcps_resource_getname(res) ).arg( amount ), 1 );
        
        // do actual appoinments etc
        ResourceRequest *r = m_requestmap.value( req );
        if ( r == 0 ) {
            cs->logWarning( i18n( "No resource request is registered" ), 1 );
            continue;
        }
        resourcemap[ task ] << r;
        cs->logDebug( QString( "Make appointments to resource %1" ).arg( r->resource()->name() ), 1 );
        r->makeAppointment( cs, amount );
    }
    if ( m_recalculate ) {
        if ( task->completion().isFinished() ) {
            task->copySchedule();
            if ( locale() && m_manager ) {
                cs->logDebug( QString( "Task is completed, copied schedule: %2 to %3" ).arg( task->name() ).arg( locale()->formatDateTime( task->startTime() ) ).arg( locale()->formatDateTime( task->endTime() ) ), 1 );
            }
        } else if ( task->completion().isStarted() ) {
            task->copyAppointments( DateTime(), start );
            if ( locale() && m_manager ) {
                cs->logDebug( QString( "Task is %4% completed, copied appointments from %2 to %3" ).arg( task->name() ).arg( locale()->formatDateTime( task->startTime() ) ).arg( locale()->formatDateTime( start ) ).arg( task->completion().percentFinished() ), 1 );
            }
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
    if ( locale() ) {
        cs->logInfo( i18n( "Scheduled task to start at %1 and finish at %2", locale()->formatDateTime( task->startTime() ), locale()->formatDateTime( task->endTime() ) ), 1 );
    }
}

void KPlatoRCPSScheduler::kplatoFromRCPS()
{
    if ( m_backward ) {
        kplatoFromRCPSBackward();
    } else {
        kplatoFromRCPSForward();
    }
}

void KPlatoRCPSScheduler::kplatoFromRCPSForward()
{
    //qDebug()<<"KPlatoRCPSScheduler::kplatoFromRCPSForward:";
    MainSchedule *cs = static_cast<MainSchedule*>( m_project->currentSchedule() );
    QMap<Node*, QList<ResourceRequest*> > resourcemap;
    int count = rcps_job_count(m_problem);
    int step = ( PROGRESS_MAX_VALUE - m_progressinfo->progress ) / count;
    DateTime projectstart = m_starttime.addSecs( rcps_job_getstart_res(m_jobstart) * m_timeunit );
    for ( int i = 0; i < count; ++i ) {
        m_progressinfo->progress += step;
        m_manager->setProgress( m_progressinfo->progress );
        setProgress( m_progressinfo->progress );

        struct rcps_job *job = rcps_job_get( m_problem, i );
        Task *task = m_taskmap.value( job );
        if ( task == 0 ) {
            continue; //might be dummy task for lag, ...
        }
        taskFromRCPSForward( job, task, resourcemap );

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
    cs->logInfo( i18n( "Project scheduled to start at %1 and finish at %2", locale()->formatDateTime( projectstart ), locale()->formatDateTime( end ) ), 1 );

    m_project->adjustSummarytask();
    
    calculatePertValues( resourcemap );

    if ( m_manager ) {
        if ( locale() ) cs->logDebug( QString( "Project scheduling finished at %1" ).arg( QDateTime::currentDateTime().toString() ), 1 );
        m_project->finishCalculation( *m_manager );
        m_manager->scheduleChanged( cs );
    }
}

void KPlatoRCPSScheduler::taskFromRCPSBackward( struct rcps_job *job, Task *task, QMap<Node*, QList<ResourceRequest*> > &resourcemap )
{
    if ( m_haltScheduling || m_manager == 0 ) {
        return;
    }
    Schedule *cs = task->currentSchedule();
    Q_ASSERT( cs );
    struct rcps_mode *mode = rcps_mode_get( job, rcps_job_getmode_res( job ) );
    /* get the duration of this mode */
    KPlatoRCPSScheduler::duration_info *info = static_cast<KPlatoRCPSScheduler::duration_info*>( rcps_mode_get_cbarg( mode ) );
    qint64 dur = 0;
    qint64 st = rcps_job_getstart_res( job );
    if ( info == 0 ) {
        dur = rcps_mode_getduration( mode );
    } else {
        cs->logDebug( QString( "Task '%1' estimate: %2" ).arg( task->name() ).arg( task->estimate()->value( Estimate::Use_Expected, false ).toString() ), 1 );
        cs->logDebug( QString( "Task '%1' duration called %2 times, cached values: %3" ).arg( rcps_job_getname( job ) ).arg( info->calls ).arg( info->cache.count() ) );

        dur = duration_callback( 0, st, rcps_mode_getduration( mode ), info );

        for ( QMap<int, int>::ConstIterator it = info->cache.constBegin(); it != info->cache.constEnd(); ++it ) {
            cs->logDebug( QString( "Task '%1' start: %2, duration: %3 (%4, %5 hours)" ).arg( rcps_job_getname(job) ).arg( it.key() ).arg( it.value() ).arg( m_starttime.addSecs( (it.key()*m_timeunit)).toString() ).arg( (double)(it.value())/60.0 ), 1 );
        }
    }
    DateTime end = fromRcpsTime( st );
    DateTime start = fromRcpsTime( st + dur );
    if ( locale() ) {
        cs->logDebug( QString( "Task '%1' start=%2, duration=%3: %4 - %5" ).arg( rcps_job_getname(job) ).arg( st ).arg( dur ).arg( locale()->formatDateTime( start ) ).arg( locale()->formatDateTime( end ) ), 1 );
    }
    task->setStartTime( start );
    task->setEndTime( end );
    for ( int reqs = 0; reqs < rcps_request_count( mode ); ++reqs ) {
        struct rcps_request *req = rcps_request_get( mode, reqs );
        struct rcps_alternative *alt = rcps_alternative_get( req, rcps_request_getalternative_res( req ) );
        int amount = rcps_alternative_getamount( alt );
        struct rcps_resource *res = rcps_alternative_getresource( alt );

        cs->logDebug( QString( "Job %1: resource %2 is %3 available" ).arg( rcps_job_getname( job ) ).arg( rcps_resource_getname( res ) ).arg( amount ), 1 );

        // do actual appoinments etc
        ResourceRequest *r = m_requestmap.value( req );
        if ( r == 0 ) {
            cs->logWarning( i18n( "No resource request is registered" ), 1 );
            continue;
        }
        resourcemap[ task ] << r;
        cs->logDebug( QString( "Make appointments to resource %1" ).arg( r->resource()->name() ), 1 );
        r->makeAppointment( cs, amount );
    }
    if ( m_recalculate ) {
        if ( task->completion().isFinished() ) {
            task->copySchedule();
            if ( locale() && m_manager ) {
                cs->logDebug( QString( "Task is completed, copied schedule: %2 to %3" ).arg( task->name() ).arg( locale()->formatDateTime( task->startTime() ) ).arg( locale()->formatDateTime( task->endTime() ) ), 1 );
            }
        } else if ( task->completion().isStarted() ) {
            task->copyAppointments( DateTime(), start );
            if ( locale() && m_manager ) {
                cs->logDebug( QString( "Task is %4% completed, copied appointments from %2 to %3" ).arg( task->name() ).arg( locale()->formatDateTime( task->startTime() ) ).arg( locale()->formatDateTime( start ) ).arg( task->completion().percentFinished() ), 1 );
            }
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
    if ( locale() ) {
        cs->logInfo( i18n( "Scheduled task to start at %1 and finish at %2", locale()->formatDateTime( task->startTime() ), locale()->formatDateTime( task->endTime() ) ), 1 );
    }
}


void KPlatoRCPSScheduler::kplatoFromRCPSBackward()
{
    //qDebug()<<"KPlatoRCPSScheduler::kplatoFromRCPSBackward:";
    MainSchedule *cs = static_cast<MainSchedule*>( m_project->currentSchedule() );
    QMap<Node*, QList<ResourceRequest*> > resourcemap;
    int count = rcps_job_count( m_problem );
    int step = ( PROGRESS_MAX_VALUE - m_progressinfo->progress ) / count;
    DateTime projectstart = fromRcpsTime( rcps_job_getstart_res( m_jobend ) );
    for ( int i = 0; i < count; ++i ) {
        m_progressinfo->progress += step;
        m_manager->setProgress( m_progressinfo->progress );
        setProgress( m_progressinfo->progress );

        struct rcps_job *job = rcps_job_get( m_problem, i );
        Task *task = m_taskmap.value( job );
        if ( task == 0 ) {
            continue; //might be dummy task for lag, ...
        }
        taskFromRCPSBackward( job, task, resourcemap );

        if ( projectstart > task->startTime() ) {
            projectstart = task->startTime();
        }
    }
    DateTime end = fromRcpsTime( rcps_job_getstart_res( m_jobstart ) );
    m_project->setStartTime( projectstart );
    m_project->setEndTime( end );
    cs->logInfo( i18n( "Project scheduled to start at %1 and finish at %2", locale()->formatDateTime( projectstart ), locale()->formatDateTime( end ) ), 1 );
    if ( projectstart < m_project->constraintStartTime() ) {
        cs->schedulingError = true;
        cs->logError( i18n( "Must start project early in order to finish in time: %1", locale()->formatDateTime( m_project->constraintStartTime() ) ), 1 );
    }
    m_project->adjustSummarytask();

    calculatePertValues( resourcemap );

    if ( m_manager ) {
        if ( locale() ) cs->logDebug( QString( "Project scheduling finished at %1" ).arg( QDateTime::currentDateTime().toString() ), 1 );
        m_project->finishCalculation( *m_manager );
        m_manager->scheduleChanged( cs );
    }
}

void KPlatoRCPSScheduler::calculatePertValues( const QMap<Node*, QList<ResourceRequest*> > &map )
{
    if ( m_manager ) {
        m_schedule->setPhaseName( 2, i18nc( "Project Evaluation and Review Technique", "PERT" ) );
    }
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
                cs->logDebug( QString( "Resource '%1' available before %2: %3" ).arg( r->resource()->name() ).arg(  m_project->endTime().toString() ).arg( x.toString() ), 2 );
                if ( x.isValid() && x > end ) {
                    end = x;
                }
            }
        } else if ( task->estimate()->calendar() ) {
            end = task->estimate()->calendar()->firstAvailableBefore( m_project->endTime(), task->endTime() );
            cs->logDebug( QString( "Calendar work time before %1: %2" ).arg(  m_project->endTime().toString() ).arg( end.toString() ), 2 );
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
                    cs->logDebug( QString( "Resource '%1' available before %2: %3" ).arg( req->resource()->name() ).arg(  r->child()->startTime().toString() ).arg( y.toString() ), 2 );
                    if ( y.isValid() && y > end ) {
                        end = y;
                    }
                }
            } else if ( task->estimate()->calendar() ) {
                end = task->estimate()->calendar()->firstAvailableBefore( r->child()->startTime(), task->endTime() );
                cs->logDebug( QString( "Calendar work time before %1: %2" ).arg(  r->child()->startTime().toString() ).arg( end.toString() ), 2 );
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
    if ( locale() ) {
        cs->logDebug( QString( "Late start %1, late finish %2, positive float %3" ).arg( locale()->formatDateTime( task->lateStart() ) ).arg( locale()->formatDateTime( task->lateFinish() ) ).arg( pf.toString() ), 2 );
    }
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
                cs->logDebug( QString( "Resource '%1' available after %2 (%4): %3" ).arg( r->resource()->name() ).arg(  m_project->startTime().toString() ).arg( x.toString() ).arg( task->startTime().toString() ), 2 );
                if ( x.isValid() && x < earlystart ) {
                    earlystart = x;
                }
            }
        } else if ( task->estimate()->calendar() ) {
            earlystart = task->estimate()->calendar()->firstAvailableAfter( m_project->startTime(), task->startTime() );
            cs->logDebug( QString( "Calendar work time after %1: %2" ).arg(  m_project->startTime().toString() ).arg( earlystart.toString() ), 2 );
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
                    cs->logDebug( QString( "Resource '%1' available after %2: %3" ).arg( req->resource()->name() ).arg(  r->parent()->endTime().toString() ).arg( y.toString() ), 2 );
                    if ( y.isValid() && y < earlystart ) {
                        earlystart = y;
                    }
                }
            } else if ( task->estimate()->calendar() ) {
                earlystart = task->estimate()->calendar()->firstAvailableAfter( r->parent()->endTime(), task->startTime() );
                cs->logDebug( QString( "Calendar work time after %1: %2" ).arg(  r->parent()->endTime().toString() ).arg( earlystart.toString() ), 2 );
            }
            x = qMin( x, earlystart.isValid() ? task->startTime() - earlystart : task->startTime() - r->parent()->startTime() );
        }
        // TODO must calculate backwards to get late *start* of task
        tot = prev + x;
    }
    task->setEarlyStart( task->startTime() - tot );
    task->setEarlyFinish( task->earlyStart() + ( task->endTime() - task->startTime() ) );
    if ( locale() ) {
        cs->logDebug( QString( "Early start %1, early finish %2" ).arg( locale()->formatDateTime( task->earlyStart() ) ).arg( locale()->formatDateTime( task->earlyFinish() ) ), 2 );
    }
    return tot;
}

struct rcps_resource *KPlatoRCPSScheduler::addResource( KPlato::Resource *r)
{
    if ( m_resourcemap.values().contains( r ) ) {
        kWarning()<<r->name()<<"already exist";
        return 0;
    }
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
        addResource( list.at(i) );
    }
}

int KPlatoRCPSScheduler::toRcpsTime( const DateTime &time ) const
{
    return m_starttime.secsTo( time ) / m_timeunit;
}

DateTime KPlatoRCPSScheduler::fromRcpsTime( int time ) const
{
    return m_starttime.addSecs( ( m_backward ? -time : time ) * m_timeunit );
}

struct rcps_job *KPlatoRCPSScheduler::addTask( KPlato::Task *task )
{
    struct rcps_job *job = rcps_job_new();
    rcps_job_setname( job, task->name().toLocal8Bit().data() );
    rcps_job_add( m_problem, job );
    switch ( task->constraint() ) {
        case Node::ASAP:
            rcps_job_setweight( job, WEIGHT_ASAP );
            break;
        case Node::ALAP:
            rcps_job_setweight( job, WEIGHT_ALAP );
            break;
        case Node::StartNotEarlier:
            rcps_job_setearliest_start( job, toRcpsTime( task->constraintStartTime() ) );
            rcps_job_setweight( job, WEIGHT_SNE );
            break;
        case Node::MustStartOn:
            rcps_job_setearliest_start( job, toRcpsTime( task->constraintStartTime() ) );
            rcps_job_setweight( job, WEIGHT_MFO );
            break;
        case Node::FinishNotLater:
            rcps_job_setweight( job, WEIGHT_FNL );
            break;
        case Node::MustFinishOn:
            rcps_job_setweight( job, WEIGHT_MFO );
            break;
        case Node::FixedInterval:
            rcps_job_setearliest_start( job, toRcpsTime( task->constraintStartTime() ) );
            rcps_job_setweight( job, WEIGHT_FI );
            break;
        default:
            rcps_job_setweight( job, WEIGHT_ASAP );
            break;
    }
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
        addTask( static_cast<Task*>( n ) );
    }
    // Add an end job
    m_jobend = rcps_job_new();
    rcps_job_setname( m_jobend, "RCPS end job" );
    rcps_job_add( m_problem, m_jobend );
    mode = rcps_mode_new();
    rcps_mode_add( m_jobend, mode );
    // add a weight callback
    struct KPlatoRCPSScheduler::weight_info *info = new KPlatoRCPSScheduler::weight_info;
    info->self = this;
    info->weight = 0;
    info->targettime = toRcpsTime( m_targettime );
    info->isEndJob = true;

    rcps_mode_set_weight_cbarg( mode, info );
    m_weight_info_list << info;

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

void KPlatoRCPSScheduler::addDependenciesForward( struct rcps_job *job, KPlato::Task *task )
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

void KPlatoRCPSScheduler::addDependenciesBackward( struct rcps_job *job, KPlato::Task *task )
{
    if ( task->dependParentNodes().isEmpty() && task->parentProxyRelations().isEmpty() ) {
        rcps_job_successor_add( job, m_jobend, SUCCESSOR_FINISH_START );
    }
    if ( task->dependChildNodes().isEmpty() && task->childProxyRelations().isEmpty() ) {
        rcps_job_successor_add( m_jobstart, job, SUCCESSOR_FINISH_START );
    }
    foreach ( Relation *r, task->dependParentNodes() ) {
        Node *n = r->parent();
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
    foreach ( Relation *r, task->parentProxyRelations() ) {
        Node *n = r->parent();
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
        if ( m_backward ) {
            addDependenciesBackward( it.key(), it.value() );
        } else {
            addDependenciesForward( it.key(), it.value() );
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

void KPlatoRCPSScheduler::addRequest( rcps_job *job, Task *task )
{
    kDebug();
    struct rcps_mode *mode = rcps_mode_new();
    rcps_mode_add( job, mode );
    if ( task->type() == Node::Type_Milestone || task->estimate() == 0 || ( m_recalculate && task->completion().isFinished() ) ) {
        rcps_mode_setduration(mode, 0);
        return;
    }
    if ( task->estimate()->type() == Estimate::Type_Duration && task->estimate()->calendar() == 0 ) {
        // Fixed duration, no callback needed
        rcps_mode_setduration(mode, task->estimate()->value( Estimate::Use_Expected, m_usePert ).seconds() / m_timeunit );
        return;
    }
    /* set the argument for the duration callback */
    struct KPlatoRCPSScheduler::duration_info *info = new KPlatoRCPSScheduler::duration_info;
    info->self = this;
    info->calls = 0;
    info->task = task;
    if ( m_recalculate && task->completion().isStarted() ) {
        info->estimate = task->completion().remainingEffort();
    } else {
        info->estimate = task->estimate()->value( Estimate::Use_Expected, m_usePert );
    }
    info->requests = task->requests().resourceRequests(); // returns team members (not team resource itself)
    info->estimatetype = task->estimate()->type();

    rcps_mode_set_cbarg( mode, info );
    m_duration_info_list << info;

    foreach ( ResourceRequest *rr, info->requests ) {
        Resource *r = rr->resource();
        if ( r->type() == Resource::Type_Team ) {
            kWarning()<<"There should not be any request to a team resource:"<<r->name();
            continue;
        }
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
