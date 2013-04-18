/* This file is part of the KDE project
 * Copyright (C) 2009 Dag Andersen <danders@get2net.dk>
 * Copyright (C) 2011 Dag Andersen <danders@get2net.dk>
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

#ifndef KPLATORCPSSCHEDULER_H
#define KPLATORCPSSCHEDULER_H

#include "kplatorcps_export.h"

#include "kptschedulerplugin.h"

#include "kptdatetime.h"

#include <QThread>
#include <QObject>
#include <QMap>
#include <QList>

class ProgressInfo;

class KLocale;

struct rcps_fitness;

namespace KPlato
{
    class Project;
    class ScheduleManager;
    class Schedule;
    class MainSchedule;
    class Resource;
    class ResourceRequest;
    class Task;
    class Node;
}
using namespace KPlato;

class KPlatoRCPSScheduler : public KPlato::SchedulerThread
{
    Q_OBJECT

private:
    struct duration_info
    {
        KPlatoRCPSScheduler *self;
        Task *task;
        Duration estimate;
        int estimatetype;
        QList<ResourceRequest*> requests;
        // QPair< time, direction >, duration
        QMap<QPair<int, int>, int> cache;
        qint64 calls;
    };

    struct weight_info
    {
        KPlatoRCPSScheduler *self;
        Task *task;
        int targettime;
        bool isEndJob;
        int finish;
    };

    struct fitness_info
    {
        KPlatoRCPSScheduler *self;
        QMultiMap<int, QPair<int, Task*> > map;
        QList<Task*> jobs;
    };

public:
    KPlatoRCPSScheduler( Project *project, ScheduleManager *sm, ulong granularity, QObject *parent = 0 );
    ~KPlatoRCPSScheduler();

    int check();

    int result;

    static int progress_callback( int generations, struct rcps_fitness fitness, void* arg );
    static int duration_callback( int direction, int time, int nominal_duration, void *arg );
    static int weight_callback( int time, int duration, struct rcps_fitness *nominal_weight, void* weight_arg, void* fitness_arg );
    static void *fitness_callback_init( void *arg );
    static int fitness_callback_result( struct rcps_fitness *fitness, void *arg );

    int progress( int generations, struct rcps_fitness fitness );
    int duration( int direction, int time, int nominal_duration, duration_info *info );
    int weight( int time, int duration, struct rcps_fitness *nominal_weight, KPlatoRCPSScheduler::weight_info* info, KPlatoRCPSScheduler::fitness_info *finfo );
    int fitness( struct rcps_fitness *fit, KPlatoRCPSScheduler::fitness_info *info );

    /// Fill project data into RCPS structure
    int kplatoToRCPS();
    /// Fetch project data from RCPS structure
    void kplatoFromRCPS();

signals:
    void sigCalculationStarted( Project*, ScheduleManager* );
    void sigCalculationFinished( Project*, ScheduleManager* );

public slots:
    void solve();

protected:
    void run();

    void kplatoFromRCPSForward();
    void kplatoFromRCPSBackward();
    void taskFromRCPSForward( struct rcps_job *job, Task *task, QMap<Node*, QList<ResourceRequest*> > &resourcemap );
    void taskFromRCPSBackward( struct rcps_job *job, Task *task, QMap<Node*, QList<ResourceRequest*> > &resourcemap );

    // Real durations for early-/late start/finish not calculated so values are approximate
    void calculatePertValues( const QMap<Node*, QList<ResourceRequest*> > &map );
    Duration calculateLateStuff( const QMap<Node*, QList<ResourceRequest*> > &map, Task *task );
    Duration calculateEarlyStuff( const QMap<Node*, QList<ResourceRequest*> > &map, Task *task );

    void adjustSummaryTasks( const QList<Node*> &nodes );

    void addResources();
    struct rcps_resource *addResource( KPlato::Resource *resource );
    void addTasks();
    struct rcps_job *addTask( KPlato::Task *task );
    struct rcps_job *addJob( const QString &name, int duration );
    void addDependencies();
    void addDependenciesForward( struct rcps_job *job, Task *task );
    void addDependenciesBackward( struct rcps_job *job, Task *task );
    void addRequests();
    void addRequest( struct rcps_job *job, Task *task );
    void setConstraints();
    void setWeights();

private:
    KLocale *locale() const;
    int toRcpsTime( const DateTime &time ) const;
    DateTime fromRcpsTime( int time ) const;

private:
    MainSchedule *m_schedule;
    bool m_recalculate;
    bool m_usePert;
    bool m_backward;
    struct rcps_problem *m_problem;
    DateTime m_starttime;
    DateTime m_targettime;
    qint64 m_timeunit;
    uint m_offsetFromTime_t;
    
    QMap<struct rcps_resource*, Resource*> m_resourcemap;
    QMap<struct rcps_request*, ResourceRequest*> m_requestmap;
    QMap<struct rcps_job*, Task*> m_taskmap;
    struct rcps_job *m_jobstart, *m_jobend;
    
    QMap<struct rcps_job*, struct duration_info*> m_duration_info_list;
    QMap<struct rcps_job*, struct weight_info*> m_weight_info_list;

    ProgressInfo *m_progressinfo;
    struct fitness_info fitness_init_arg;
};

#endif // KPLATORCPSPSCHEDULER_H
