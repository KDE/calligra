/* This file is part of the KDE project
 * Copyright (C) 2009, 2010, 2012 Dag Andersen <danders@get2net.dk>
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
#include "kptdebug.h"

#include <librcps.h>

#include <QString>
#include <QTimer>
#include <QMutexLocker>

#include <KGlobal>
#include <KLocale>

#include <iostream>

#define GENERATION_MIN_LIMIT 5000

#define PROGRESS_CALLBACK_FREQUENCY 100
#define PROGRESS_MAX_VALUE 120000
#define PROGRESS_INIT_VALUE 12000
#define PROGRESS_INIT_STEP 2000

/* low weight == late, high weight == early */
#define WEIGHT_ASAP         50
#define WEIGHT_ALAP         1
#define WEIGHT_CONSTRAINT   1000
#define WEIGHT_FINISH       1000

#define GROUP_TARGETTIME    1
#define GROUP_CONSTRAINT    2


class ProgressInfo
{
public:
    explicit ProgressInfo() : init( true ), base( 0 ), progress( 0 )
    {
        fitness.group = 0;
        fitness.weight = 0;
    }
    bool init;
    int base;
    int progress;
    struct rcps_fitness fitness;
};


KPlatoRCPSScheduler::KPlatoRCPSScheduler( Project *project, ScheduleManager *sm, ulong granularity, QObject *parent )
    : SchedulerThread( project, sm, parent ),
    result( -1 ),
    m_schedule( 0 ),
    m_recalculate( false ),
    m_usePert( false ),
    m_backward( false ),
    m_problem( 0 ),
    m_timeunit( granularity / 1000 ),
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

int KPlatoRCPSScheduler::progress_callback( int generations, struct rcps_fitness fitness, void *arg )
{
    if ( arg == 0 ) {
        return -1;
    }
    KPlatoRCPSScheduler *self = static_cast<KPlatoRCPSScheduler*>( arg );
    //kDebug(planDbg())<<"KPlatoRCPSScheduler::progress_callback"<<generations<<fitness<<arg;
    return self->progress( generations, fitness );
}

int KPlatoRCPSScheduler::progress( int generations, struct rcps_fitness fitness )
{
    if ( m_haltScheduling ) {
        kDebug(planDbg())<<"KPlatoRCPSScheduler::progress:"<<"halt";
        return -1;
    }
    if ( m_stopScheduling ) {
        m_schedule->logWarning( i18n( "Scheduling halted after %1 generations", generations ), 1 );
        kDebug(planDbg())<<"KPlatoRCPSScheduler::progress:"<<"stop";
        return -1;
    }
//     std::cout << "Progress after: " << generations << " generations\n";
    if ( m_progressinfo->init ) {
        if ( generations == 0 ) {
            m_progressinfo->progress += PROGRESS_INIT_STEP;
        } else {
            m_progressinfo->progress = PROGRESS_INIT_VALUE;
            m_progressinfo->init = false;
//             std::cout << "Population generated: "<< generations << "\n";
        }
    } else {
        m_progressinfo->progress = PROGRESS_INIT_VALUE + generations;
    }
    // detect change in fitness
    if ( rcps_fitness_cmp( &m_progressinfo->fitness, &fitness ) != 0 ) {
//         std::cout << "Fitness changed in generation: " << generations << " group=["<<m_progressinfo->fitness.group<<"->"<<fitness.group<<"]"<<" weight=["<<m_progressinfo->fitness.weight<<"->"<<fitness.weight<<"]\n";
        m_progressinfo->fitness = fitness;
        m_progressinfo->base = generations;
    }
    m_manager->setProgress( m_progressinfo->progress );
    setProgress( m_progressinfo->progress );
    // stop if fitness does not change in GENERATION_MIN_LIMIT generations
/*    int result = ( generations >= m_progressinfo->base + GENERATION_MIN_LIMIT ? 1 : 0 );
    if ( result ) {
        //kDebug(planDbg())<<"KPlatoRCPSScheduler::progress, stop after"<<generations<<"generations, progress:"<<m_progressinfo->progress;
        m_schedule->logDebug( QString( "Acceptable solution found after %1 generations" ).arg( generations ), 1 );
        std::cout << "Acceptable solution found after " << generations << " generations\n";
    }*/
    return 0;
}

int KPlatoRCPSScheduler::duration_callback( int direction, int time, int nominal_duration, void *arg )
{
    //kDebug(planDbg())<<"kplato_duration:"<<direction<<time<<nominal_duration<<arg;
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
    if ( info->cache.contains( QPair<int, int>( time, direction ) ) ) {
        return info->cache[ QPair<int, int>( time, direction ) ];
    }
    if ( m_manager->recalculate() && info->task->completion().isFinished() ) {
        return 0;
    }
    int dur = 0;
    if ( info->task->constraint() == Node::FixedInterval ) {
        // duration may depend on daylight saving so we need to calculate
        // NOTE: dur may not be correct if time != info->task->constraintStartTime, let's see what happends...
        dur = ( info->task->constraintEndTime() - info->task->constraintStartTime() ).seconds() / m_timeunit;
        info->task->schedule()->logDebug( QString( "Fixed interval: Time=%1, duration=%2 ( %3, %4 )" ).arg( time ).arg( dur ).arg( fromRcpsTime( time ).toString() ).arg( Duration( (qint64)(dur) * m_timeunit * 1000 ).toDouble( Duration::Unit_h ) ) );
    } else if ( info->estimatetype == Estimate::Type_Effort ) {
        if ( info->requests.isEmpty() ) {
            dur = info->estimate.seconds() / m_timeunit;
        } else {
            dur = info->task->requests().duration(
                        info->requests,
                        fromRcpsTime( time ),
                        info->estimate,
                        0, /*no schedule*/
                        m_backward ? ! direction : direction
                    ).seconds() / m_timeunit;
            //kDebug(planDbg())<<info->task->name()<< QString( "duration_callback effort: backward=%5, direction=%6 (direction=%7); Time=%1, duration=%2 ( %3, %4 )" ).arg( time ).arg( dur ).arg( fromRcpsTime( time ).toString() ).arg( Duration( (qint64)(dur) * m_timeunit * 1000 ).toDouble( Duration::Unit_h ) ).arg( m_backward ).arg( direction ).arg( m_backward ? !direction : direction );
        }
    } else {
        dur = info->task->length( 
                    fromRcpsTime( time ),
                    info->estimate,
                    0, /*no schedule*/
                    m_backward ? ! direction : direction
                ).seconds() / m_timeunit;
    }
    info->cache[ QPair<int, int>( time, direction ) ] = dur;
    info->task->schedule()->logDebug( QString( "duration_callback: Time=%1, duration=%2 ( %3, %4 )" ).arg( time ).arg( dur ).arg( fromRcpsTime( time ).toString() ).arg( Duration( (qint64)(dur) * m_timeunit * 1000 ).toDouble( Duration::Unit_h ) ) );
    return dur;
}

int KPlatoRCPSScheduler::weight_callback( int time, int duration, struct rcps_fitness *nominal_weight, void* weight_arg, void* fitness_arg )
{
    //kDebug(planDbg())<<"kplato_weight:"<<time<<nominal_weight<<arg;
    if ( weight_arg == 0 ) {
        nominal_weight->weight *= time;
        return 0;
    }
    KPlatoRCPSScheduler::weight_info *winfo = static_cast<KPlatoRCPSScheduler::weight_info*>( weight_arg );
    KPlatoRCPSScheduler::fitness_info *finfo = static_cast<KPlatoRCPSScheduler::fitness_info*>( fitness_arg );
    return winfo->self->weight( time, duration, nominal_weight, winfo, finfo );
}

void *KPlatoRCPSScheduler::fitness_callback_init( void *arg )
{
    Q_ASSERT( arg );
    KPlatoRCPSScheduler::fitness_info *info = static_cast<KPlatoRCPSScheduler::fitness_info*>( arg );
    Q_ASSERT( info );
    fitness_info *finfo = new fitness_info;
    finfo->self = info->self;
//     kDebug(planDbg())<<info->self;
    return finfo;
}

int KPlatoRCPSScheduler::fitness_callback_result( struct rcps_fitness *fit, void *arg )
{
    KPlatoRCPSScheduler::fitness_info *info = static_cast<KPlatoRCPSScheduler::fitness_info*>( arg );
    info->self->fitness( fit, info );
    delete info;
    return 0;
}

int KPlatoRCPSScheduler::fitness( struct rcps_fitness *fit, KPlatoRCPSScheduler::fitness_info *info )
{
/*    std::cout << ">-------------------------------------------\n";
    std::cout << "Sequence: ";
    foreach ( Task *t, info->jobs ) { std::cout << (t ? t->name().toLocal8Bit().data() : "End") << ", "; }
    std::cout << "\n";
    kDebug(planDbg())<<info->map;*/
    QMultiMap<int, QPair<int, Task*> >::const_iterator it = info->map.constFind( GROUP_CONSTRAINT );
    if ( it != info->map.constEnd() ) {
        // constraint
        fit->group = GROUP_CONSTRAINT;
        for ( ; it.key() == GROUP_CONSTRAINT && it != info->map.constEnd(); ++it ) {
            fit->weight += it.value().first;
            QString s = it.value().second ? it.value().second->name() : "End node";
//             std::cout << s.toLocal8Bit().data() << ": group=" << it.key() << " weight=" << it.value().first << "\n";
//             m_schedule->logDebug( QString( "%3: %1 %2" ).arg( it.key() ).arg( it.value().first ).arg( it.value().second->name() ) );
        }
//         std::cout << "Result: group= " << fit->group << " weight=" << fit->weight << "\n--------------------------\n";
        return 0;
    }
    it = info->map.constFind( GROUP_TARGETTIME );
    if ( it != info->map.constEnd() ) {
        // missed target time
        fit->group = GROUP_TARGETTIME;
        for ( ; it.key() == GROUP_TARGETTIME && it != info->map.constEnd(); ++it ) {
            fit->weight += it.value().first;
            QString s = it.value().second ? it.value().second->name() : "End node";
//             std::cout << s.toLocal8Bit().data() << ": group=" << it.key() << " weight=" << it.value().first << "\n";
//             m_schedule->logDebug( QString( "%3: %1 %2" ).arg( it.key() ).arg( it.value().first ).arg( it.value().second->name() ) );
        }
//         std::cout << "Result: group= " << fit->group << " weight=" << fit->weight << "\n--------------------------\n";
        return 0;
    }
    fit->group = 0;
    for ( it = info->map.constBegin(); it != info->map.constEnd(); ++it ) {
        fit->weight += it.value().first;
        QString s = it.value().second ? it.value().second->name() : "End node";
//         std::cout << s.toLocal8Bit().data() << ": group=" << it.key() << " weight=" << it.value().first << "\n";
//        m_schedule->logDebug( QString( "%3: %1 %2" ).arg( it.key() ).arg( it.value().first ).arg( it.value().second->name() ) );
    }
//     std::cout << "Result: group= " << fit->group << " weight=" << fit->weight << "\n--------------------------\n";
    return 0;
}

int KPlatoRCPSScheduler::weight( int time, int duration, struct rcps_fitness *nominal_weight, KPlatoRCPSScheduler::weight_info* info, KPlatoRCPSScheduler::fitness_info* finfo )
{
    if ( m_haltScheduling || m_manager == 0 ) {
        return 0;
    }
    if ( m_manager->recalculate() && info->task->completion().isFinished() ) {
        return 0;
    }
    struct rcps_fitness &f = *nominal_weight;
    f.group = 0;
    f.weight = time;
    if ( info->isEndJob ) {
        if ( info->finish == 0 ) {
            info->finish = time;
/*            const char *s = QString( "First  : %1 %2 %3 End job" ).arg( time, 10 ).arg( duration, 10 ).arg( w, 10 ).toLatin1();
            std::cout<<s<<"\n";*/
        }
/*        w = WEIGHT_FINISH * info->finish / ( time > 0 ? time : 1 );
        if ( time > info->targettime ) {
            w = w + ( WEIGHT_CONSTRAINT * ( time - info->targettime ) );
        }*/
        if ( time > info->targettime ) {
            f.group = GROUP_TARGETTIME;
            f.weight = time - info->targettime;
        }

/*        const char *s = QString( "End job: %1 %2 %3 End job target: %4" ).arg( time, 10 ).arg( duration, 10 ).arg( w, 10 ).arg( info->targettime ).toLatin1();
        std::cout<<s<<"\n";*/
    } else {
        if ( m_backward ) {
            switch ( info->task->constraint() ) {
                case Node::FinishNotLater:
                    if ( info->targettime > time ) {
                        f.group = GROUP_CONSTRAINT;
                        f.weight = WEIGHT_CONSTRAINT * ( info->targettime - time );
                    }
                    break;
                case Node::MustFinishOn:
                    if ( info->targettime != time ) {
                        f.group = GROUP_CONSTRAINT;
                        f.weight = WEIGHT_CONSTRAINT * abs( info->targettime - time );
                    }
                    break;
                case Node::StartNotEarlier:
                    if ( info->targettime < time ) {
                        f.group = GROUP_CONSTRAINT;
                        f.weight = WEIGHT_CONSTRAINT * ( time - info->targettime );
                    }
                    break;
                case Node::MustStartOn:
                case Node::FixedInterval:
                    if ( info->targettime != time ) {
                        f.group = GROUP_CONSTRAINT;
                        f.weight = WEIGHT_CONSTRAINT * abs( info->targettime - time );
                    }
                    break;
                default:
                    break;
            }
/*            const char *s = QString( "Backwrd: %1 %2 %3 %4 (target: %5)" ).arg( time, 10 ).arg( duration, 10 ).arg( w, 10 ).arg( info->task->name() ).arg( info->targettime ).toLatin1();
            std::cout<<s<<"\n";*/
        } else {
            switch ( info->task->constraint() ) {
                case Node::StartNotEarlier:
                    if ( time < info->targettime ) {
                        f.group = GROUP_CONSTRAINT;
                        f.weight = WEIGHT_CONSTRAINT * ( info->targettime - time );
                    }
                    break;
                case Node::MustStartOn:
                case Node::FixedInterval:
                    if ( info->targettime != time ) {
                        f.group = GROUP_CONSTRAINT;
                        f.weight = WEIGHT_CONSTRAINT * ( abs( info->targettime - time ) );
                    }
                    break;
                case Node::FinishNotLater:
//                     std::cout << "FNL " << info->task->name().toLocal8Bit().data() << ": end="<<time+duration<<" target="<<info->targettime<<"\n";
                    if ( time + duration > info->targettime ) {
                        f.group = GROUP_CONSTRAINT;
                        f.weight = WEIGHT_CONSTRAINT * ( time - info->targettime );
                    }
//                     std::cout << info->task->name().toLocal8Bit().data() << ": group=" << f.group << " weight=" << f.weight << "\n";
                    break;
                case Node::MustFinishOn:
//                     std::cout << "MSO " << info->task->name().toLocal8Bit().data() << ": end="<<time+duration<<" target="<<info->targettime<<"\n";
                    if ( info->targettime != time + duration ) {
                        f.group = GROUP_CONSTRAINT;
                        f.weight = WEIGHT_CONSTRAINT * abs( info->targettime - time );
                    }
//                     std::cout << info->task->name().toLocal8Bit().data() << ": group=" << f.group << " weight=" << f.weight << "\n";
                    break;
                default:
                    break;
            }
/*            const char *s = QString( "Forward: %1 %2 %3 %4 (target: %5)" ).arg( time, 10 ).arg( duration, 10 ).arg( w, 10 ).arg( info->task->name() ).arg( info->targettime ).toLatin1();
            std::cout<<s<<"\n";*/
        }
    }
//     QString s = info->task ? info->task->name() : "End node";
    if ( finfo ) {
        finfo->map.insert( f.group, QPair<int, Task*>( f.weight, info->task ) );
        finfo->jobs << info->task;
//         kDebug(planDbg())<<s<<":"<<finfo->map;
    }// else kDebug(planDbg())<<s<<":"<<"No finfo!";
/*    std::cout << "Weight: " << s.toLocal8Bit().data() << ": group=" << f.group << " weight=" << f.weight << "\n";*/
    return 0;
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
        m_project->stopcalculation = false;
        m_manager = m_project->scheduleManager( m_mainmanagerId );
        Q_CHECK_PTR( m_manager );
        Q_ASSERT( m_manager->expected() );
        Q_ASSERT( m_manager != m_mainmanager );
        Q_ASSERT( m_manager->scheduleId() == m_mainmanager->scheduleId() );
        Q_ASSERT( m_manager->expected() != m_mainmanager->expected() );
        m_manager->setName( "Schedule: " + m_manager->name() ); //Debug
        m_schedule = m_manager->expected();

        connect(m_manager, SIGNAL(sigLogAdded(Schedule::Log)), this, SLOT(slotAddLog(Schedule::Log)));

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
        m_targettime = m_backward ? m_project->constraintStartTime() : m_project->constraintEndTime();

        m_project->setCurrentSchedule( m_manager->expected()->id() );

        m_schedule->setPhaseName( 0, i18n( "Init" ) );
        if ( ! m_backward && locale() ) {
            m_schedule->logDebug( QString( "Schedule project using RCPS Scheduler, starting at %1, granularity %2 sec" ).arg( QDateTime::currentDateTime().toString() ).arg( m_timeunit ), 0 );
            if ( m_recalculate ) {
                m_schedule->logInfo( i18n( "Re-calculate project from start time: %1", locale()->formatDateTime( m_starttime ) ), 0 );
            } else {
                m_schedule->logInfo( i18n( "Schedule project from start time: %1", locale()->formatDateTime( m_starttime ) ), 0 );
            }
        }
        if ( m_backward && locale() ) {
            m_schedule->logDebug( QString( "Schedule project backward using RCPS Scheduler, starting at %1, granularity %2 sec" ).arg( locale()->formatDateTime( QDateTime::currentDateTime() ) ).arg( m_timeunit ), 0 );
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
    kDebug(planDbg())<<"KPlatoRCPSScheduler::solve()";
    struct rcps_solver *s = rcps_solver_new();
    rcps_solver_set_progress_callback(s, PROGRESS_CALLBACK_FREQUENCY, this, &KPlatoRCPSScheduler::progress_callback);
    rcps_solver_set_duration_callback(s, &KPlatoRCPSScheduler::duration_callback );

    rcps_problem_set_weight_callback( m_problem, &KPlatoRCPSScheduler::weight_callback );
    fitness_init_arg.self = this;
    rcps_problem_set_fitness_callback( m_problem, &KPlatoRCPSScheduler::fitness_callback_init, &fitness_init_arg, &KPlatoRCPSScheduler::fitness_callback_result );

    Q_ASSERT( check() == 0 );

    rcps_solver_setparam( s, SOLVER_PARAM_POPSIZE, 1000 );

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
    setWeights();
    setConstraints();

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

        for ( QMap<QPair<int, int>, int>::ConstIterator it = info->cache.constBegin(); it != info->cache.constEnd(); ++it ) {
            cs->logDebug( QString( "Task '%1' start: %2, duration: %3 (%4, %5 hours)" ).arg( rcps_job_getname(job) ).arg( it.key().first ).arg( it.value() ).arg( fromRcpsTime( it.key().first ).toString() ).arg( (double)(it.value())/60.0 ), 1 );
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
        if ( info && info->requests.isEmpty() ) {
            cs->setResourceError( true );
            cs->logError( i18n( "No resource has been allocated" ), 1 );
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
    //kDebug(planDbg())<<"KPlatoRCPSScheduler::kplatoFromRCPSForward:";
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

    adjustSummaryTasks( m_schedule->summaryTasks() );
    
    calculatePertValues( resourcemap );

    cs->logInfo( i18n( "Project scheduled to start at %1 and finish at %2", locale()->formatDateTime( projectstart ), locale()->formatDateTime( end ) ), 1 );

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

        for ( QMap<QPair<int, int>, int>::ConstIterator it = info->cache.constBegin(); it != info->cache.constEnd(); ++it ) {
            cs->logDebug( QString( "Task '%1' start: %2, duration: %3 (%4, %5 hours)" ).arg( rcps_job_getname(job) ).arg( it.key().first ).arg( it.value() ).arg( fromRcpsTime( it.key().first ).toString() ).arg( (double)(it.value())/60.0 ), 1 );
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
        if ( info && info->requests.isEmpty() ) {
            cs->setResourceError( true );
            cs->logError( i18n( "No resource has been allocated" ), 1 );
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
    //kDebug(planDbg())<<"KPlatoRCPSScheduler::kplatoFromRCPSBackward:";
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
        cs->setConstraintError( true );
        cs->logError( i18n( "Must start project early in order to finish in time: %1", locale()->formatDateTime( m_project->constraintStartTime() ) ), 1 );
    }
    adjustSummaryTasks( m_schedule->summaryTasks() );

    calculatePertValues( resourcemap );

    if ( m_manager ) {
        if ( locale() ) cs->logDebug( QString( "Project scheduling finished at %1" ).arg( QDateTime::currentDateTime().toString() ), 1 );
        m_project->finishCalculation( *m_manager );
        m_manager->scheduleChanged( cs );
    }
}

void KPlatoRCPSScheduler::adjustSummaryTasks( const QList<Node*> &nodes )
{
    foreach ( Node *n, nodes ) {
        adjustSummaryTasks( n->childNodeIterator() );
        if ( n->parentNode()->type() == Node::Type_Summarytask ) {
            DateTime pt = n->parentNode()->startTime();
            DateTime nt = n->startTime();
            if ( ! pt.isValid() || pt > nt ) {
                n->parentNode()->setStartTime( nt );
            }
            pt = n->parentNode()->endTime();
            nt = n->endTime();
            if ( ! pt.isValid() || pt < nt ) {
                n->parentNode()->setEndTime( nt );
            }
        }
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
        Task *t = static_cast<Task*>( n );
        if ( n->isStartNode() ) {
            (void)calculateLateStuff( map, static_cast<Task*>( n ) );
        }
        if ( n->isEndNode() ) {
            (void)calculateEarlyStuff( map,  static_cast<Task*>( n ) );
        }
        switch ( n->constraint() ) {
            case Node::StartNotEarlier:
                n->schedule()->setNegativeFloat( n->startTime() < n->constraintStartTime()
                            ? n->startTime() - n->constraintStartTime()
                            :  Duration::zeroDuration );
                break;
            case Node::MustStartOn:
            case Node::FixedInterval:
                n->schedule()->setNegativeFloat( n->startTime() > n->constraintStartTime()
                            ? n->startTime() - n->constraintStartTime()
                            :  n->constraintStartTime() - n->startTime() );
                break;
            case Node::FinishNotLater:
                n->schedule()->setNegativeFloat( n->endTime() > n->constraintEndTime()
                                ? n->endTime() - n->constraintEndTime()
                                : Duration::zeroDuration );
                break;
            case Node::MustFinishOn:
                n->schedule()->setNegativeFloat( n->endTime() > n->constraintEndTime()
                                ? n->endTime() - n->constraintEndTime()
                                : n->constraintEndTime() - n->endTime() );
                break;
            default:
                break;
        }
        if ( t->negativeFloat() != 0 ) {
            n->schedule()->setConstraintError( true );
            n->schedule()->logError( i18nc( "1=type of constraint", "%1: Failed to meet constraint. Negative float=%2", n->constraintToString( true ), locale()->formatDuration( t->negativeFloat().milliseconds() ) ) );
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
    kDebug(planDbg());
    QList<Resource*> list = m_project->resourceList();
    for (int i = 0; i < list.count(); ++i) {
        addResource( list.at(i) );
    }
}

int KPlatoRCPSScheduler::toRcpsTime( const DateTime &time ) const
{
    return ( m_backward ?  time.secsTo( m_starttime ) : m_starttime.secsTo( time ) ) / m_timeunit;
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
    m_taskmap[job] = task;
    return job;
}

void KPlatoRCPSScheduler::addTasks()
{
    kDebug(planDbg());
    // Add a start job
    m_jobstart = rcps_job_new();
    rcps_job_setname( m_jobstart, "RCPS start job" );
    rcps_job_add( m_problem, m_jobstart );
    struct rcps_mode *mode = rcps_mode_new();
    rcps_mode_add( m_jobstart, mode );

    QList<Node*> list = m_project->allNodes();
    for (int i = 0; i < list.count(); ++i) {
        Node *n = list.at(i);
        switch ( n->type() ) {
            case Node::Type_Summarytask:
                m_schedule->insertSummaryTask( n );
                break;
            case Node::Type_Task:
            case Node::Type_Milestone:
                addTask( static_cast<Task*>( n ) );
                break;
            default:
                break;
        }
    }
    // Add an end job
    m_jobend = rcps_job_new();
    rcps_job_setname( m_jobend, "RCPS end job" );
    rcps_job_add( m_problem, m_jobend );
    mode = rcps_mode_new();
    rcps_mode_add( m_jobend, mode );
    // add a weight callback argument
    struct KPlatoRCPSScheduler::weight_info *info = new KPlatoRCPSScheduler::weight_info;
    info->self = this;
    info->task = 0;
    info->targettime = toRcpsTime( m_targettime );
    info->isEndJob = true;
    info->finish = 0;

    rcps_mode_set_weight_cbarg( mode, info );
    m_weight_info_list[ m_jobend ] = info;

    for( int i = 0; i < rcps_job_count( m_problem ); ++i ) {
        kDebug(planDbg())<<"Task:"<<rcps_job_getname( rcps_job_get(m_problem, i) );
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
        //kDebug(planDbg())<<rcps_job_getname( job )<<"->"<<rcps_job_getname( m_jobend );
    }
    if ( task->dependChildNodes().isEmpty() && task->childProxyRelations().isEmpty() ) {
        rcps_job_successor_add( m_jobstart, job, SUCCESSOR_FINISH_START );
        //kDebug(planDbg())<<rcps_job_getname( m_jobstart )<<"->"<<rcps_job_getname( job );
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
            //kDebug(planDbg())<<rcps_job_getname( job )<<"->"<<rcps_job_getname( m_taskmap.key( static_cast<Task*>( n ) ) )<<type;
        } else {
            // Add a dummy job to represent the lag
            struct rcps_job *dummy = addJob( r->lag().toString(), r->lag().seconds() / m_timeunit );
            rcps_job_successor_add( job, dummy, type );
            kDebug(planDbg())<<rcps_job_getname( job )<<"->"<<"dummy lag"<<type;
            int t = type == SUCCESSOR_FINISH_FINISH ? type : SUCCESSOR_FINISH_START;
            rcps_job_successor_add( dummy, m_taskmap.key( static_cast<Task*>( n ) ), t );
            //kDebug(planDbg())<<"dummy lag"<<"->"<<rcps_job_getname( m_taskmap.key( static_cast<Task*>( n ) ) )<<t;
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
            //kDebug(planDbg())<<rcps_job_getname( job )<<"->"<<rcps_job_getname( m_taskmap.key( static_cast<Task*>( n ) ) )<<type;
        } else {
            // Add a dummy job to represent the lag
            struct rcps_job *dummy = addJob( r->lag().toString(), r->lag().seconds() / m_timeunit );
            rcps_job_successor_add( job, dummy, type );
            kDebug(planDbg())<<rcps_job_getname( job )<<"->"<<"dummy lag"<<type;
            int t = type == SUCCESSOR_FINISH_FINISH ? type : SUCCESSOR_FINISH_START;
            rcps_job_successor_add( dummy, m_taskmap.key( static_cast<Task*>( n ) ), t );
            //kDebug(planDbg())<<"dummy lag"<<"->"<<rcps_job_getname( m_taskmap.key( static_cast<Task*>( n ) ) )<<t;
        }
    }
}

void KPlatoRCPSScheduler::addDependencies()
{
    kDebug(planDbg());
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
    kDebug(planDbg());
    QMap<struct rcps_job*, Task*> ::const_iterator it = m_taskmap.constBegin();
    for ( ; it != m_taskmap.constEnd(); ++it ) {
        addRequest( it.key(), it.value() );
    }
}

void KPlatoRCPSScheduler::addRequest( rcps_job *job, Task *task )
{
    kDebug(planDbg());
    struct rcps_mode *mode = rcps_mode_new();
    rcps_mode_add( job, mode );
    // add a weight callback argument
    struct KPlatoRCPSScheduler::weight_info *wi = new KPlatoRCPSScheduler::weight_info;
    wi->self = this;
    wi->task = task;
    wi->targettime = 0;
    wi->isEndJob = false;
    wi->finish = 0;

    rcps_mode_set_weight_cbarg( mode, wi );
    m_weight_info_list[ job ] = wi;

    if ( task->constraint() != Node::FixedInterval ) {
        if ( task->type() == Node::Type_Milestone || task->estimate() == 0 || ( m_recalculate && task->completion().isFinished() ) ) {
            rcps_mode_setduration(mode, 0);
            return;
        }
        if ( task->estimate()->type() == Estimate::Type_Duration && task->estimate()->calendar() == 0 ) {
            // Fixed duration, no duration callback needed
            rcps_mode_setduration(mode, task->estimate()->value( Estimate::Use_Expected, m_usePert ).seconds() / m_timeunit );
            return;
        }
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
    m_duration_info_list[ job ] = info;


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
        rcps_alternative_setamount( alt, (double)rr->units() * 100 / r->units() );
        rcps_alternative_add( req, alt );
    }
}

void KPlatoRCPSScheduler::setConstraints()
{
    for ( QMap<struct rcps_job*, Task*>::iterator it = m_taskmap.begin(); it != m_taskmap.end(); ++it ) {
        Task *task = it.value();
        struct rcps_job *job = it.key();
        struct weight_info *wi = m_weight_info_list.value( job );
        struct duration_info *di = m_duration_info_list.value( job );
        switch ( task->constraint() ) {
            case Node::MustStartOn:
            case Node::StartNotEarlier:
                wi->targettime = toRcpsTime( task->constraintStartTime() );
                if ( m_backward ) {
                    int d = 0;
                    if ( di ) {
                        // as m_backward == true, DURATION_BACKWARD in rcps means forward in plan
                        d = duration( DURATION_BACKWARD, wi->targettime, 0, di );
                    }
                    wi->targettime -= d;
                }
                rcps_job_setearliest_start( job, wi->targettime );
                task->currentSchedule()->logDebug( QString( "%2 %3 %4: %5 (rcps=%6)")
                            .arg( task->constraintToString() )
                            .arg( m_backward?"backward":"forward")
                            .arg( task->constraintStartTime().toString() )
                            .arg( fromRcpsTime( wi->targettime ).toString() )
                            .arg( wi->targettime ) );
                break;
            case Node::MustFinishOn:
                wi->targettime = toRcpsTime( task->constraintEndTime() );
                if ( ! m_backward ) {
                    int d = 0;
                    if ( di ) {
                        d = duration( DURATION_BACKWARD, wi->targettime, 0, di );
                    }
                    rcps_job_setearliest_start( job, wi->targettime - d );
                }
                break;
            case Node::FinishNotLater:
                wi->targettime = toRcpsTime( task->constraintEndTime() );
                if ( m_backward ) {
                    rcps_job_setearliest_start( job, wi->targettime );
                }
                break;
            case Node::FixedInterval:
                wi->targettime = m_backward ? toRcpsTime( task->constraintEndTime() ) : toRcpsTime( task->constraintStartTime() );
                rcps_job_setearliest_start( job, wi->targettime );
                break;
            default:
                break;
        }
    }
}

void KPlatoRCPSScheduler::setWeights()
{
    for ( QMap<struct rcps_job*, Task*>::iterator it = m_taskmap.begin(); it != m_taskmap.end(); ++it ) {
        Task *task = it.value();
        struct rcps_job *job = it.key();
        if ( m_backward ) {
            switch ( task->constraint() ) {
                case Node::ASAP:
                    rcps_job_setweight( job, WEIGHT_ALAP );
                    break;
                case Node::ALAP:
                    rcps_job_setweight( job, WEIGHT_ASAP );
                    break;
                case Node::StartNotEarlier:
                    rcps_job_setweight( job, WEIGHT_CONSTRAINT );
                    break;
                case Node::MustStartOn:
                    rcps_job_setweight( job, WEIGHT_CONSTRAINT );
                    break;
                case Node::FinishNotLater:
                    rcps_job_setearliest_start( job, toRcpsTime( task->constraintEndTime() ) );
                    rcps_job_setweight( job, WEIGHT_CONSTRAINT );
                    break;
                case Node::MustFinishOn:
                    rcps_job_setearliest_start( job, toRcpsTime( task->constraintEndTime() ) );
                    rcps_job_setweight( job, WEIGHT_CONSTRAINT );
                    break;
                case Node::FixedInterval:
                    rcps_job_setearliest_start( job, toRcpsTime( task->constraintEndTime() ) );
                    rcps_job_setweight( job, WEIGHT_CONSTRAINT );
                    break;
                default:
                    rcps_job_setweight( job, WEIGHT_ASAP );
                    break;
            }
        } else {
            switch ( task->constraint() ) {
                case Node::ASAP:
                    rcps_job_setweight( job, WEIGHT_ASAP );
                    break;
                case Node::ALAP:
                    rcps_job_setweight( job, WEIGHT_ALAP );
                    break;
                case Node::StartNotEarlier:
                    rcps_job_setearliest_start( job, toRcpsTime( task->constraintStartTime() ) );
                    rcps_job_setweight( job, WEIGHT_CONSTRAINT );
                    break;
                case Node::MustStartOn:
                    rcps_job_setearliest_start( job, toRcpsTime( task->constraintStartTime() ) );
                    rcps_job_setweight( job, WEIGHT_CONSTRAINT );
                    break;
                case Node::FinishNotLater:
                    rcps_job_setweight( job, WEIGHT_CONSTRAINT );
                    break;
                case Node::MustFinishOn:
                    rcps_job_setweight( job, WEIGHT_CONSTRAINT );
                    break;
                case Node::FixedInterval:
                    rcps_job_setearliest_start( job, toRcpsTime( task->constraintStartTime() ) );
                    rcps_job_setweight( job, WEIGHT_CONSTRAINT );
                    break;
                default:
                    rcps_job_setweight( job, WEIGHT_ASAP );
                    break;
            }
        }
    }
}

#include "KPlatoRCPSScheduler.moc"
