/* This file is part of the KDE project
   Copyright (C) 2010 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kpteffortcostmap.h"

#include <kdebug.h>

namespace KPlato
{

void EffortCost::add(const Duration &effort, double cost, double bcwpEffort, double bcwpCost )
{
    m_effort += effort;
    m_cost += cost;
    m_bcwpEffort += bcwpEffort;
    m_bcwpCost += bcwpCost;
}

//-----------------------
EffortCostMap::EffortCostMap( const EffortCostMap &map )
{
    m_days = map.m_days;
}

EffortCostMap &EffortCostMap::operator=( const EffortCostMap &ec )
{
    m_days = ec.m_days;
    return *this;
}

void EffortCostMap::addBcwpCost( const QDate &date, double cost )
{
    EffortCost ec = m_days[ date ];
    ec.setBcwpCost( ec.bcwpCost() + cost );
    m_days[ date ] = ec;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug EffortCostMap::debug( QDebug dbg ) const
{
    dbg.nospace()<<"EffortCostMap[";
    if ( ! m_days.isEmpty() ) {
        dbg<<startDate().toString(Qt::ISODate)<<endDate().toString(Qt::ISODate)
            <<" total="<<totalEffort().toString()<<totalCost()<<bcwpTotalEffort()<<bcwpTotalCost();
    }
    dbg.nospace()<<']';
    if ( ! m_days.isEmpty() ) {
        QMap<QDate, KPlato::EffortCost>::ConstIterator it = days().constBegin();
        for ( ; it != days().constEnd(); ++it ) {
            dbg<<endl;
            dbg<<"     "<<it.key().toString(Qt::ISODate)<<it.value()<<" ("<<hoursOnDate( it.key() )<<"h)";
        }
        dbg<<endl;
    }
    return dbg;
}
#endif

} //namespace KPlato

#ifndef QT_NO_DEBUG_STREAM
QDebug &operator<<( QDebug &dbg, const KPlato::EffortCost &ec )
{
    dbg<<"EffortCost["<<ec.effort().toString()<<" "<<ec.cost()<<" : "<<ec.bcwpEffort()<<" "<<ec.bcwpCost()<<"]";
    return dbg;
}
QDebug &operator<<( QDebug &dbg, const KPlato::EffortCost *ec )
{
    return operator<<( dbg, *ec );
}

QDebug operator<<( QDebug dbg, const KPlato::EffortCostMap &i )
{
    return i.debug( dbg );
}
#endif
