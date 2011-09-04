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

#include <QObject>
#include <QVariant>

#include <ResourceGroup.h>
#include <Project.h>

#include <kptresource.h>

Scripting::ResourceGroup::ResourceGroup( Scripting::Project *project, KPlato::ResourceGroup *group, QObject *parent )
    : QObject( parent ), m_project( project ), m_group( group )
{
}

QObject *Scripting::ResourceGroup::project()
{
    return m_project;
}

QString Scripting::ResourceGroup::id()
{
    return m_group->id();
}

QString Scripting::ResourceGroup::type()
{
    return m_group->typeToString();
}

int Scripting::ResourceGroup::resourceCount() const
{
    return m_group->numResources();
}

QObject *Scripting::ResourceGroup::resourceAt( int index ) const
{
    return m_project->resource( m_group->resourceAt( index ) );
}

int Scripting::ResourceGroup::childCount() const
{
    return m_group->numResources();
}

QObject *Scripting::ResourceGroup::childAt( int index ) const
{
    return m_project->resource( m_group->resourceAt( index ) );
}


#include "ResourceGroup.moc"
