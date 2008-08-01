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

#include <Resource.h>
#include <Project.h>

#include <kglobal.h>
#include <kptresource.h>
#include <kptappointment.h>
#include <kptdatetime.h>

Scripting::Resource::Resource( Scripting::Project *project, KPlato::Resource *resource, QObject *parent )
    : QObject( parent ), m_project( project ), m_resource( resource )
{
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
    foreach ( KPlato::AppointmentInterval *ai, app.intervals() ) {
        lst << QVariant( QVariantList() << ai->startTime().toString() << ai->endTime().toString() << ai->load() );
    }
    return lst;
}

void Scripting::Resource::addExternalAppointment( const QVariant &id, const QString &name, const QVariantList &lst )
{
    //kDebug()<<id<<name<<lst;
    KPlato::DateTime st = KPlato::DateTime::fromString( lst[0].toString() );
    KPlato::DateTime et = KPlato::DateTime::fromString( lst[1].toString() );
    double load = lst[2].toDouble();
    if ( ! st.isValid() || ! et.isValid() ) {
        return;
    }
    m_resource->addExternalAppointment( id.toString(), name, st, et, load );
}

QVariantList Scripting::Resource::externalAppointments() const
{
    KPlato::AppointmentIntervalList ilst = m_resource->externalAppointments();
    QVariantList lst;
    foreach ( KPlato::AppointmentInterval *ai, ilst ) {
        lst << QVariant( QVariantList() << ai->startTime().toString() << ai->endTime().toString() << ai->load() );
    }
    return lst;
}

void Scripting::Resource::clearExternalAppointments( const QString &id )
{
    m_resource->clearExternalAppointments( id );
}

int Scripting::Resource::childCount() const
{
    return 0;
}

QObject *Scripting::Resource::childAt( int /*index*/ ) const
{
    return 0;
}

#include "Resource.moc"
