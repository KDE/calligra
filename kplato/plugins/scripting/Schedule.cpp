/* This file is part of the KOffice project
 * Copyright (c) 2008 Dag Andersen <kplato@kde.org>
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

#include <QObject>
#include <QVariant>

#include <Schedule.h>
#include <Project.h>

#include <kptschedule.h>

Scripting::Schedule::Schedule( Scripting::Project *project, KPlato::ScheduleManager *schedule, QObject *parent )
    : QObject( parent ), m_project( project ), m_schedule( schedule )
{
}

qlonglong Scripting::Schedule::id() const
{
    return (qlonglong)m_schedule->id();
}

QString Scripting::Schedule::name() const
{
    return m_schedule->name();
}

QDate Scripting::Schedule::startDate()
{
//    return m_schedule->startTime().dateTime().date();
}

QDate Scripting::Schedule::endDate()
{
//    return m_schedule->endTime().dateTime().date();
}

int Scripting::Schedule::childCount() const
{
    return m_schedule->childCount();
}

QObject *Scripting::Schedule::childAt( int index )
{
    return m_project->schedule( m_schedule->childAt( index ) );
}

// QVariant Scripting::Schedule::data(const QString &property )
// {
//     return data( property, "DisplayRole", "-1" );
// }

// QVariant Scripting::Schedule::data(const QString &property, const QString &role )
// {
//     return m_project->scheduleData( m_schedule, property, role, schedule );
// }

#include "Schedule.moc"
