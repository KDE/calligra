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

void EffortCostMap::insert(const QDate &date, const EffortCost &ec )
{
    Q_ASSERT( date.isValid() );
    m_days[ date ] = ec;
}

EffortCostMap &EffortCostMap::operator=( const EffortCostMap &ec )
{
    m_days = ec.m_days;
    return *this;
}

EffortCostMap &EffortCostMap::operator+=(const EffortCostMap &ec) {
    //kDebug(planDbg())<<"me="<<m_days.count()<<" ec="<<ec.days().count();
    if (ec.isEmpty()) {
        return *this;
    }
    if (isEmpty()) {
        m_days = ec.days();
        return *this;
    }
    EffortCostMap other = ec;
    QDate oed = other.endDate();
    QDate ed = endDate();
    // get bcwp of the last entries
    EffortCost last_oec = other.m_days[oed];
    last_oec.setEffort( Duration::zeroDuration );
    last_oec.setCost( 0.0 );
    EffortCost last_ec = m_days[ed];
    last_ec.setEffort( Duration::zeroDuration );
    last_ec.setCost( 0.0 );
    if ( oed > ed ) {
        // expand my last entry to match other
        for ( QDate d = ed.addDays( 1 ); d <= oed; d = d.addDays( 1 ) ) {
            m_days[ d ] = last_ec ;
        }
    }
    EffortCostDayMap::const_iterator it;
    for(it = ec.days().constBegin(); it != ec.days().constEnd(); ++it) {
        add(it.key(), it.value());
    }
    if ( oed < ed ) {
        // add others last entry to my trailing entries
        for ( QDate d = oed.addDays( 1 ); d <= ed; d = d.addDays( 1 ) ) {
            m_days[ d ] += last_oec;
        }
    }
    return *this;
}

void EffortCostMap::addBcwpCost( const QDate &date, double cost )
{
    EffortCost ec = m_days[ date ];
    ec.setBcwpCost( ec.bcwpCost() + cost );
    m_days[ date ] = ec;
}

double EffortCostMap::bcwpCost( const QDate &date ) const
{
    double v = 0.0;
    for ( EffortCostDayMap::const_iterator it = m_days.constBegin(); it != m_days.constEnd(); ++it ) {
        if ( it.key() > date ) {
            break;
        }
        v = it.value().bcwpCost();
    }
    return v;
}

double EffortCostMap::bcwpEffort( const QDate &date ) const
{
    double v = 0.0;
    for ( EffortCostDayMap::const_iterator it = m_days.constBegin(); it != m_days.constEnd(); ++it ) {
        if ( it.key() > date ) {
            break;
        }
        v = it.value().bcwpEffort();
    }
    return v;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug EffortCostMap::debug( QDebug dbg ) const
{
    dbg.nospace()<<"EffortCostMap[";
    if ( ! m_days.isEmpty() ) {
        dbg<<startDate().toString(Qt::ISODate)<<" "<<endDate().toString(Qt::ISODate)
            <<" total bcws="<<totalEffort().toDouble( Duration::Unit_h )<<", "<<totalCost()<<" bcwp="<<bcwpTotalEffort()<<" "<<bcwpTotalCost();
    }
    dbg.nospace()<<']';
    if ( ! m_days.isEmpty() ) {
        QMap<QDate, KPlato::EffortCost>::ConstIterator it = days().constBegin();
        for ( ; it != days().constEnd(); ++it ) {
            dbg<<endl;
            dbg<<"     "<<it.key().toString(Qt::ISODate)<<" "<<it.value();
        }
        dbg<<endl;
    }
    return dbg;
}
#endif

} //namespace KPlato

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<( QDebug dbg, const KPlato::EffortCost &ec )
{
    dbg.nospace()<<"EffortCost[ bcws effort="<<ec.hours()<<" cost="<<ec.cost()<<" : bcwp effort="<<ec.bcwpEffort()<<" cost="<<ec.bcwpCost()<<"]";
    return dbg;
}
QDebug operator<<( QDebug dbg, const KPlato::EffortCost *ec )
{
    return operator<<( dbg, *ec );
}

QDebug operator<<( QDebug dbg, const KPlato::EffortCostMap &i )
{
    return i.debug( dbg );
}
#endif
