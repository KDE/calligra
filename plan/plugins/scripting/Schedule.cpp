/* This file is part of the Calligra project
 * Copyright (c) 2008 Dag Andersen <danders@get2net>
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

#include "Schedule.h"

#include "Project.h"

#include <kptschedule.h>

Scripting::Schedule::Schedule( Scripting::Project *project, KPlato::ScheduleManager *schedule, QObject *parent )
    : QObject( parent ), m_project( project ), m_schedule( schedule )
{
}

qlonglong Scripting::Schedule::id() const
{
    return (qlonglong)m_schedule ? m_schedule->scheduleId() : -1;
}

QString Scripting::Schedule::name() const
{
    return m_schedule ? m_schedule->name() : "";
}

bool Scripting::Schedule::isScheduled() const
{
    return m_schedule ? m_schedule->isScheduled() : false;
}

QDate Scripting::Schedule::startDate()
{
    return QDate(); //m_schedule->startTime().dateTime().date();
}

QDate Scripting::Schedule::endDate()
{
    return QDate(); //m_schedule->endTime().dateTime().date();
}

int Scripting::Schedule::childCount() const
{
    return m_schedule ? m_schedule->childCount() : 0;
}

QObject *Scripting::Schedule::childAt( int index )
{
    if ( m_schedule && m_project ) {
        return m_project->schedule( m_schedule->childAt( index ) );
    }
    return 0;
}

#include "Schedule.moc"
