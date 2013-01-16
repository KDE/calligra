/* This file is part of the Calligra project
 * Copyright (c) 2008, 2011 Dag Andersen <danders@get2net>
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

#include "Resource.h"
#include "Project.h"

#include "kptresource.h"
#include "kptappointment.h"
#include "kptdatetime.h"
#include "kptcommand.h"

#include <kglobal.h>

Scripting::Resource::Resource( Scripting::Project *project, KPlato::Resource *resource, QObject *parent )
    : QObject( parent ), m_project( project ), m_resource( resource )
{
}

QObject *Scripting::Resource::project()
{
    return m_project;
}

QVariant Scripting::Resource::type()
{
    return m_resource->typeToString();
}

QString Scripting::Resource::id() const
{
    return m_resource->id();
}

QVariantList Scripting::Resource::appointmentIntervals( qlonglong schedule ) const
{
    KPlato::Appointment app = m_resource->appointmentIntervals( schedule );
    QVariantList lst;
    foreach ( const KPlato::AppointmentInterval &ai, app.intervals().map() ) {
        lst << QVariant( QVariantList() << ai.startTime().toString() << ai.endTime().toString() << ai.load() );
    }
    return lst;
}

void Scripting::Resource::addExternalAppointment( const QVariant &id, const QString &name, const QVariantList &lst )
{
    m_project->addExternalAppointment( this, id, name, lst );
}

QVariantList Scripting::Resource::externalAppointments() const
{
    KPlato::AppointmentIntervalList ilst = m_resource->externalAppointments();
    QVariantList lst;
    foreach ( const KPlato::AppointmentInterval &ai, ilst.map() ) {
        lst << QVariant( QVariantList() << ai.startTime().toString() << ai.endTime().toString() << ai.load() );
    }
    return lst;
}

void Scripting::Resource::clearExternalAppointments( const QString &id )
{
    m_project->clearExternalAppointments( this, id );
}

int Scripting::Resource::childCount() const
{
    return kplatoResource()->type() == KPlato::Resource::Type_Team ? kplatoResource()->teamMembers().count() : 0;
}

QObject *Scripting::Resource::childAt( int index ) const
{
    if ( kplatoResource()->type() == KPlato::Resource::Type_Team ) {
        return m_project->resource( kplatoResource()->teamMembers().value( index ) );
    }
    return 0;
}

void Scripting::Resource::setChildren( const QList<QObject*> &children )
{
    qDebug()<<"setTeamMembers:"<<children;
    KPlato::Resource *team = kplatoResource();
    // atm. only teams have children
    if ( team->type() != KPlato::Resource::Type_Team ) {
        return;
    }
    KPlato::MacroCommand *cmd = new KPlato::MacroCommand( i18nc( "(qtundo_format)", "Set resource team members" ) );
    foreach ( const QString &id, team->teamMemberIds() ) {
       cmd->addCommand( new KPlato::RemoveResourceTeamCmd( team, id ) );
    }
    foreach ( QObject *o, children ) {
        Resource *r = qobject_cast<Resource*>( o );
        if ( r && r->kplatoResource() ) {
            cmd->addCommand( new KPlato::AddResourceTeamCmd( team, r->kplatoResource()->id() ) );
        }
    }
    if ( ! cmd->isEmpty() ) {
        m_project->addCommand( cmd );
    }
    qDebug()<<"setTeamMembers:"<<team->teamMembers();
}

#include "Resource.moc"
