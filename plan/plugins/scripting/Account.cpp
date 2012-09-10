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

#include "Account.h"
#include "Project.h"

#include <kptaccount.h>

Scripting::Account::Account( Scripting::Project *project, KPlato::Account *account, QObject *parent )
    : QObject( parent ), m_project( project ), m_account( account )
{
}

QObject *Scripting::Account::project()
{
    return m_project;
}

QString Scripting::Account::name() const
{
    return m_account->name();
}

int Scripting::Account::childCount() const
{
    return m_account->childCount();
}

QObject *Scripting::Account::childAt( int index )
{
    return m_project->account( m_account->childAt( index ) );
}

QVariant Scripting::Account::plannedEffortCostPrDay( const QVariant &start, const QVariant &end, const QVariant &schedule )
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
    KPlato::Accounts *a = m_account->list();
    if ( a == 0 ) {
        return QVariant();
    }
    KPlato::EffortCostMap ec = a->plannedCost( *m_account, s, e, schedule.toLongLong() );
    KPlato::EffortCostDayMap::ConstIterator it = ec.days().constBegin();
    for (; it != ec.days().constEnd(); ++it ) {
        map.insert( it.key().toString( Qt::ISODate ), QVariantList() << it.value().effort().toDouble( KPlato::Duration::Unit_h ) << it.value().cost() );
    }
    return map;
}

QVariant Scripting::Account::actualEffortCostPrDay( const QVariant &start, const QVariant &end, const QVariant &schedule )
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
    KPlato::Accounts *a = m_account->list();
    if ( a == 0 ) {
        return QVariant();
    }
    KPlato::EffortCostMap ec = a->actualCost( *m_account, s, e, schedule.toLongLong() );
    KPlato::EffortCostDayMap::ConstIterator it = ec.days().constBegin();
    for (; it != ec.days().constEnd(); ++it ) {
        map.insert( it.key().toString( Qt::ISODate ), QVariantList() << it.value().effort().toDouble( KPlato::Duration::Unit_h ) << it.value().cost() );
    }
    return map;
}

QVariant Scripting::Account::plannedEffortCostPrDay( const QVariant &schedule )
{
    //kDebug(planDbg())<<start<<end<<schedule;
    QVariantMap map;
    KPlato::EffortCostMap ec = m_account->plannedCost( schedule.toLongLong() );
    KPlato::EffortCostDayMap::ConstIterator it = ec.days().constBegin();
    for (; it != ec.days().constEnd(); ++it ) {
        map.insert( it.key().toString( Qt::ISODate ), QVariantList() << it.value().effort().toDouble( KPlato::Duration::Unit_h ) << it.value().cost() );
    }
    return map;
}

QVariant Scripting::Account::actualEffortCostPrDay( const QVariant &schedule )
{
    //kDebug(planDbg())<<start<<end<<schedule;
    QVariantMap map;
    KPlato::EffortCostMap ec = m_account->actualCost( schedule.toLongLong() );
    KPlato::EffortCostDayMap::ConstIterator it = ec.days().constBegin();
    for (; it != ec.days().constEnd(); ++it ) {
        map.insert( it.key().toString( Qt::ISODate ), QVariantList() << it.value().effort().toDouble( KPlato::Duration::Unit_h ) << it.value().cost() );
    }
    return map;
}

#include "Account.moc"
