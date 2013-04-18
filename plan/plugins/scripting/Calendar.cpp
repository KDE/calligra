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

#include "Calendar.h"

#include "Project.h"

#include "kptcalendar.h"

Scripting::Calendar::Calendar( Scripting::Project *project, KPlato::Calendar *calendar, QObject *parent )
    : QObject( parent ), m_project( project ), m_calendar( calendar )
{
}

QObject *Scripting::Calendar::project()
{
    return m_project;
}

QString Scripting::Calendar::id() const
{
    return m_calendar->id();
}

QString Scripting::Calendar::name() const
{
    return m_calendar->name();
}

int Scripting::Calendar::childCount() const
{
    return m_calendar->childCount();
}

QObject *Scripting::Calendar::childAt( int index )
{
    KPlato::Calendar *c = m_calendar->childAt( index );
    return c == 0 ? 0 : m_project->calendar( c );
}


#include "Calendar.moc"
