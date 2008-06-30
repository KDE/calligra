/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTEFFORTCOST_H
#define KPTEFFORTCOST_H

#include <qdatetime.h>
#include <QMap>

#include "kptduration.h"

#include <kdebug.h>

namespace KPlato
{

class KPLATOKERNEL_EXPORT EffortCost
{
public:
    EffortCost()
        : m_effort(Duration::zeroDuration),
        m_cost(0),
        m_bcwpEffort(0.0),
        m_bcwpCost(0.0)
    {}
    EffortCost(const Duration &effort, const double cost)
        : m_effort(effort),
        m_cost(cost),
        m_bcwpEffort(0.0),
        m_bcwpCost(0.0)
    {
        //kDebug();
    }
    ~EffortCost() {
        //kDebug();
    }
    double hours() const { return m_effort.toDouble( Duration::Unit_h ); }
    Duration effort() const { return m_effort; }
    void setEffort( const Duration &effort ) { m_effort = effort; }
    double cost() const { return m_cost; }
    void setCost(double cost) { m_cost = cost; }
    void setBcwpEffort( double value ) { m_bcwpEffort = value; }
    double bcwpEffort() const { return m_bcwpEffort; }
    void setBcwpCost( double value ) { m_bcwpCost = value; }
    double bcwpCost() const { return m_bcwpCost; }
    void add(const Duration &effort, double cost, double bcwpEffort = 0.0, double bcwpCost = 0.0 ) {
        m_effort += effort;
        m_cost += cost;
        m_bcwpEffort += bcwpEffort;
        m_bcwpCost += bcwpCost;
    }
    EffortCost &operator+=(const EffortCost &ec) {
        add(ec.m_effort, ec.m_cost, ec.m_bcwpEffort, ec.m_bcwpCost);
        return *this;
    }
    
private:
    Duration m_effort;
    double m_cost;
    double m_bcwpEffort;
    double m_bcwpCost;
};

typedef QMap<QDate, EffortCost> EffortCostDayMap;
class EffortCostMap
{
public:
    EffortCostMap()
        : m_days() {
        //kDebug(); 
    }
    ~EffortCostMap() {
        //kDebug();
        m_days.clear();
    }
    
    void clear() { m_days.clear(); }
    
    EffortCost effortCost(const QDate &date) const {
        EffortCost ec;
        if (!date.isValid()) {
            kError()<<"Date not valid"<<endl;
            return ec;
        }
        EffortCostDayMap::const_iterator it = m_days.find(date);
        if (it != m_days.end())
            ec = it.value();
        return ec;
    }
    void insert(const QDate &date, const Duration &effort, const double cost) {
        if (!date.isValid()) {
            kError()<<"Date not valid"<<endl;
            return;
        }
        m_days.insert(date, EffortCost(effort, cost));
    }
    /** 
     * If data for this date already exists add the new values to the old,
     * else the new values are inserted.
     */
    EffortCost &add(const QDate &date, const Duration &effort, const double cost) {
        return add(date, EffortCost(effort, cost));
    }
    /** 
     * If data for this date already exists add the new values to the old,
     * else the new value is inserted.
     */
    EffortCost &add(const QDate &date, const EffortCost &ec) {
        if (!date.isValid()) {
            kError()<<"Date not valid"<<endl;
            return zero();
        }
        //kDebug()<<date.toString();
        return m_days[date] += ec;
    }
    
    bool isEmpty() const {
        return m_days.isEmpty();
    }
    
    const EffortCostDayMap &days() const { return m_days; }
    
    EffortCostMap &operator+=(const EffortCostMap &ec) {
        //kDebug()<<"me="<<m_days.count()<<" ec="<<ec.days().count();
        if (ec.isEmpty()) {
            return *this;
        }
        if (isEmpty()) {
            m_days = ec.days();
            return *this;
        }
        EffortCostDayMap::const_iterator it;
        for(it = ec.days().constBegin(); it != ec.days().constEnd(); ++it) {
            add(it.key(), it.value());
        }
        return *this;
    }
    EffortCost &effortCostOnDate(const QDate &date) {
        return m_days[date];
    }
    /// Return total cost for the next num days starting at date
    double cost(const QDate &date, int num=7) {
        double r=0.0;
        for (int i=0; i < num; ++i) {
            r += costOnDate(date.addDays(i));
        }
        return r;
    }
    double costOnDate(const QDate &date) const {
        if (!date.isValid()) {
            kError()<<"Date not valid"<<endl;
            return 0.0;
        }
        if (m_days.contains(date)) {
            return m_days[date].cost();
        }
        return 0.0;
    }
    Duration effortOnDate(const QDate &date) const {
        if (!date.isValid()) {
            kError()<<"Date not valid"<<endl;
            return Duration::zeroDuration;
        }
        if (m_days.contains(date)) {
            return m_days[date].effort();
        }
        return Duration::zeroDuration;
    }
    double bcwpCostOnDate(const QDate &date) const {
        if (!date.isValid()) {
            kError()<<"Date not valid"<<endl;
            return 0.0;
        }
        if (m_days.contains(date)) {
            return m_days[date].bcwpCost();
        }
        return 0.0;
    }
    double totalCost() const {
        double cost = 0.0;
        EffortCostDayMap::const_iterator it;
        for(it = m_days.constBegin(); it != m_days.constEnd(); ++it) {
            cost += it.value().cost();
        }
        return cost;
    }
    Duration totalEffort() const {
        Duration eff;
        EffortCostDayMap::const_iterator it;
        for(it = m_days.constBegin(); it != m_days.constEnd(); ++it) {
            eff += it.value().effort();
        }
        return eff;
    }
    
    double costTo( const QDate &date ) const {
        double cost = 0.0;
        EffortCostDayMap::const_iterator it;
        for(it = m_days.constBegin(); it != m_days.constEnd(); ++it) {
            if ( it.key() > date ) {
                break;
            }
            cost += it.value().cost();
        }
        return cost;
    }
    Duration effortTo( const QDate &date ) const {
        Duration eff;
        EffortCostDayMap::const_iterator it;
        for(it = m_days.constBegin(); it != m_days.constEnd(); ++it) {
            if ( it.key() > date ) {
                break;
            }
            eff += it.value().effort();
        }
        return eff;
    }

    QDate startDate() const { return m_days.isEmpty() ? QDate() : m_days.keys().first(); }
    QDate endDate() const { return m_days.isEmpty() ? QDate() : m_days.keys().last(); }
    
private:
    EffortCost &zero() { return m_zero; }
    
private:
    EffortCost m_zero;
    EffortCostDayMap m_days;
};

} //namespace KPlato

#endif
