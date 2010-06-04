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

#ifndef KPLATORCPSPSCHEDULER_H
#define KPLATORCPSPSCHEDULER_H

#include "kplatorcps_export.h"

#include "kptschedulerplugin.h"

#include "kptdatetime.h"

#include <QThread>
#include <QObject>
#include <QMap>

class ProgressInfo;

class KLocale;

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

class KPLATORCPS_TEST_EXPORT KPlatoRCPSScheduler : public KPlato::SchedulerThread
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
        QMap<int, int> cache;
        qint64 calls;
    };

public:
    KPlatoRCPSScheduler( Project *project, ScheduleManager *sm, QObject *parent = 0 );
    ~KPlatoRCPSScheduler();

    int check();

    int result;

    static int progress_callback( int generations, int duration, void *arg );
    static int duration_callback( int direction, int time, int nominal_duration, void *arg );

    int progress( int generations, int duration );
    int duration( int direction, int time, int nominal_duration, duration_info *info );

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

    void taskFromRCPS( struct rcps_job *job, Task *task, QMap<Node*, QList<ResourceRequest*> > &resourcemap );
    // NOTE:
    // Relation type not checked so only SF works ok
    // Real durations for early-/late start/finish not calculated so values are approximate
    void calculatePertValues( const QMap<Node*, QList<ResourceRequest*> > &map );
    Duration calculateLateStuff( const QMap<Node*, QList<ResourceRequest*> > &map, Task *task );
    Duration calculateEarlyStuff( const QMap<Node*, QList<ResourceRequest*> > &map, Task *task );

    void addResources();
    struct rcps_resource *addResource( KPlato::Resource *resource );
    void addTasks();
    struct rcps_job *addTask( KPlato::Task *task );
    struct rcps_job *addJob( const QString &name, int duration );
    void addDependencies();
    void addDependencies( struct rcps_job *job, Task *task );
    void addRequests();
    void addRequest( struct rcps_job *job, Task *task );

private:
    KLocale *locale() const;

private:
    MainSchedule *m_schedule;
    bool m_recalculate;
    bool m_usePert;
    struct rcps_problem *m_problem;
    DateTime m_starttime;
    qint64 m_timeunit;
    uint m_offsetFromTime_t;
    
    QMap<struct rcps_resource*, Resource*> m_resourcemap;
    QMap<struct rcps_request*, ResourceRequest*> m_requestmap;
    QMap<struct rcps_job*, Task*> m_taskmap;
    struct rcps_job *m_jobstart, *m_jobend;
    
    QList<struct duration_info*> m_duration_info_list;
    
    ProgressInfo *m_progressinfo;
};

#endif // KPLATORCPSPSCHEDULER_H
