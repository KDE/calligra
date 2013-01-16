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

#include "Node.h"
#include "Project.h"

#include "kptnode.h"
#include "kpteffortcostmap.h"
#include "kptduration.h"

Scripting::Node::Node( Scripting::Project *project, KPlato::Node *node, QObject *parent )
    : QObject( parent ), m_project( project ), m_node( node )
{
}

QObject *Scripting::Node::project()
{
    return m_project;
}

QString Scripting::Node::name()
{
    return m_node->name();
}

QDate Scripting::Node::startDate()
{
    return m_node->startTime().date();
}

QDate Scripting::Node::endDate()
{
    return m_node->endTime().date();
}

QString Scripting::Node::id()
{
    return m_node->id();
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

QObject *Scripting::Node::parentNode()
{
    return m_project->node( m_node->parentNode() );
}

QVariant Scripting::Node::plannedEffortCostPrDay( const QVariant &start, const QVariant &end, const QVariant &schedule )
{
    //kDebug(planDbg())<<start<<end<<schedule;
    QVariantMap map;
    QDate s = start.toDate();
    QDate e = end.toDate();
    if ( ! s.isValid() ) {
        s = QDate::currentDate();
    }
    if ( ! e.isValid() ) {
        e = QDate::currentDate();
    }
    KPlato::EffortCostMap ec = m_node->plannedEffortCostPrDay( s, e, schedule.toLongLong() );
    KPlato::EffortCostDayMap::ConstIterator it = ec.days().constBegin();
    for (; it != ec.days().constEnd(); ++it ) {
        map.insert( it.key().toString( Qt::ISODate ), QVariantList() << it.value().effort().toDouble( KPlato::Duration::Unit_h ) << it.value().cost() );
    }
    return map;
}

QVariant Scripting::Node::bcwsPrDay( const QVariant &schedule ) const
{
    QVariantMap map;
    KPlato::EffortCostMap ec = m_node->bcwsPrDay( schedule.toLongLong() );
    KPlato::EffortCostDayMap::ConstIterator it = ec.days().constBegin();
    for (; it != ec.days().constEnd(); ++it ) {
        map.insert( it.key().toString( Qt::ISODate ), QVariantList() << it.value().effort().toDouble( KPlato::Duration::Unit_h ) << it.value().cost() );
    }
    return map;
}

QVariant Scripting::Node::bcwpPrDay( const QVariant &schedule ) const
{
    QVariantMap map;
    KPlato::EffortCostMap ec = m_node->bcwpPrDay( schedule.toLongLong() );
    KPlato::EffortCostDayMap::ConstIterator it = ec.days().constBegin();
    for (; it != ec.days().constEnd(); ++it ) {
        map.insert( it.key().toString( Qt::ISODate ), QVariantList() << it.value().effort().toDouble( KPlato::Duration::Unit_h ) << it.value().cost() );
    }
    return map;
}

QVariant Scripting::Node::acwpPrDay( const QVariant &schedule ) const
{
    QVariantMap map;
    KPlato::EffortCostMap ec = m_node->acwp( schedule.toLongLong() );
    KPlato::EffortCostDayMap::ConstIterator it = ec.days().constBegin();
    for (; it != ec.days().constEnd(); ++it ) {
        map.insert( it.key().toString( Qt::ISODate ), QVariantList() << it.value().effort().toDouble( KPlato::Duration::Unit_h ) << it.value().cost() );
    }
    return map;
}

#include "Node.moc"
