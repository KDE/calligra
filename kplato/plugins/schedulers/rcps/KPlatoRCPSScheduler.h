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

#include <QObject>
#include <QMap>

namespace KPlato
{
    class Project;
    class ScheduleManager;
    class Resource;
    class Task;
}
using namespace KPlato;

class KPlatoRCPSScheduler : public QObject
{
    Q_OBJECT

public:
    KPlatoRCPSScheduler( Project &project, ScheduleManager *sm, QObject *parent = 0 );
    ~KPlatoRCPSScheduler();

protected:
    void addResources();
    void addTasks();
    void addDependencies();
    void addRequests();
    void addRequest( struct rcps_job *job, const Task *task );

private:
    Project &m_project;
    ScheduleManager &m_manager;
    struct rcps_problem *m_problem;

    QMap<struct rcps_resource*, Resource*> m_resourcemap;
    QMap<struct rcps_job*, Task*> m_taskmap;
    struct rcps_job *m_jobstart, *m_jobend;
};

#endif // KPLATORCPSPSCHEDULER_H
