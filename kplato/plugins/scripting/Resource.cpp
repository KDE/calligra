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

#include <kptresource.h>

Scripting::Resource::Resource( Scripting::Project *project, KPlato::Resource *resource, QObject *parent )
    : QObject( parent ), m_project( project ), m_resource( resource )
{
}

QVariant Scripting::Resource::type()
{
    return m_resource->typeToString();
}

QVariant Scripting::Resource::data(const QString &property )
{
    return data( property, "DisplayRole", "-1" );
}

QVariant Scripting::Resource::data(const QString &property, const QString &role, const QString &schedule )
{
    return m_project->resourceData( m_resource, property, role, schedule );
}

#include "Resource.moc"
