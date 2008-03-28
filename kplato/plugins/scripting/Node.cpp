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

#include "Node.h"
#include "Project.h"

#include "kptnode.h"
#include "kpteffortcostmap.h"
#include "kptduration.h"

Scripting::Node::Node( Scripting::Project *project, KPlato::Node *node, QObject *parent )
    : QObject( parent ), m_project( project ), m_node( node )
{
}

QDate Scripting::Node::startDate()
{
    return m_node->startTime().dateTime().date();
}

QDate Scripting::Node::endDate()
{
    return m_node->endTime().dateTime().date();
}

QVariant Scripting::Node::type()
{
    return m_node->typeToString();
}

int Scripting::Node::childCount() const
{
    return m_node->numChildren();
}

QObject *Scripting::Node::childAt( int index )
{
    return m_project->node( m_node->childNode( index ) );
}

QVariant Scripting::Node::data(const QString &property )
{
    return data( property, "DisplayRole", "-1" );
}

QVariant Scripting::Node::data(const QString &property, const QString &role, const QString &schedule )
{
    return m_project->nodeData( m_node, property, role, schedule );
}

QVariant Scripting::Node::plannedEffortCostPrDay( const QVariant &start, const QVariant &end, const QString &schedule )
{
    QVariantMap map;
    QDate s = start.toDate();
    QDate e = end.toDate();
    if ( ! s.isValid() ) {
        s = QDate::currentDate();
    }
    if ( e.isValid() ) {
        e = QDate::currentDate();
    }
    KPlato::EffortCostMap ec = m_node->plannedEffortCostPrDay( s, e, 2 );
    KPlato::EffortCostDayMap::ConstIterator it = ec.days().constBegin();
    for (; it != ec.days().constEnd(); ++it ) {
        map.insert( it.key().toString( Qt::ISODate ), QVariantList() << it.value().effort().toDouble( KPlato::Duration::Unit_h ) << it.value().cost() );
    }
    return map;
}
