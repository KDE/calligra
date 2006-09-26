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

#ifndef KPTAPPOINTMENT_H
#define KPTAPPOINTMENT_H

#include "kptduration.h"
#include "kptdatetime.h"

#include <qdom.h>
#include <q3intdict.h>
#include <QString>
#include <q3ptrlist.h>

#include <kdebug.h>

class QTime;

namespace KPlato
{

class Risk;
class Effort;
class Appointment;
class Task;
class Node;
class Project;
class Resource;
class ResourceRequest;
class ResourceGroupRequest;
class Calendar;
class ResourceRequestCollection;
class EffortCostMap;
class Schedule;



class AppointmentInterval {
public:
    AppointmentInterval();
    AppointmentInterval(const AppointmentInterval &AppointmentInterval);
    AppointmentInterval(const DateTime &start, const DateTime end, double load=100);
    ~AppointmentInterval();
    
    void set(DateTime &start, DateTime &end, double load=100);
    void set(DateTime &start, Duration &duration, double load=100);
    
    Duration effort() const { return (m_end - m_start) * m_load / 100; }
    Duration effort(const DateTime &start, const DateTime end) const;
    Duration effort(const DateTime &time, bool upto) const;
    
    bool loadXML(QDomElement &element);
    void saveXML(QDomElement &element) const;
    
    const DateTime &startTime() const { return m_start; }
    void setStartTime(const DateTime &time) { m_start = time; }
    const DateTime &endTime() const { return m_end; }
    void setEndTime(const DateTime &time) { m_end = time; }
    double load() const { return m_load; }
    void setLoad(double load) { m_load = load; }
    
    bool isValid() const;
    AppointmentInterval firstInterval(const AppointmentInterval &interval, const DateTime &from) const;

private:
    DateTime m_start;
    DateTime m_end;
    double m_load; //percent
};


/**
 * This list is sorted after 1) startdatetime, 2) enddatetime.
 * The intervals do not overlap, an interval does not start before the
 * previous interval ends.
 */
class AppointmentIntervalList : public Q3PtrList<AppointmentInterval> {
protected:
    int compareItems(Q3PtrCollection::Item item1, Q3PtrCollection::Item item2) {
        AppointmentInterval *i1 = static_cast<AppointmentInterval*>(item1);
        AppointmentInterval *i2 = static_cast<AppointmentInterval*>(item2);
        if (i1->startTime() < i2->startTime()) {
            return -1;
        }
        if (i1->startTime() > i2->startTime()) {
            return 1;
        }
        if (i1->endTime() < i2->endTime()) {
            return -1;
        }
        if (i1->endTime() > i2->endTime()) {
            return 1;
        }
        return 0;
    }
};
typedef Q3PtrListIterator<AppointmentInterval> AppointmentIntervalListIterator;

/**
 * A resource (@ref Resource) can be scheduled to be used at any time, 
 * this is represented internally with Appointments
 * There is one Appointment per resource-task pair.
 * An appointment can be divided into several intervals, represented with
 * a list of AppointmentInterval.
 * This list is sorted after 1) startdatetime, 2) enddatetime.
 * The intervals do not overlap, an interval does not start before the
 * previous interval ends.
 * An interval is a countinous time interval with the same load. It can span dates.
 */
class Appointment {
public:
    Appointment();
    Appointment(Schedule *resource, Schedule *node, DateTime start, DateTime end, double load);
    Appointment(Schedule *resource, Schedule *node, DateTime start, Duration duration, double load);
    ~Appointment();

    // get/set member values.
    Schedule *node() const { return m_node; }
    void setNode(Schedule *n) { m_node = n; }

    Schedule *resource() const { return m_resource; }
    void setResource(Schedule *r) { m_resource = r; }

    DateTime startTime() const;
    DateTime endTime() const;
    double maxLoad() const;
    
    const Duration &repeatInterval() const {return m_repeatInterval;}
    void setRepeatInterval(Duration ri) {m_repeatInterval=ri;}

    int repeatCount() const { return m_repeatCount; }
    void setRepeatCount(int rc) { m_repeatCount=rc; }

    void deleteAppointmentFromRepeatList(DateTime time);
    void addAppointmentToRepeatList(DateTime time);

    bool isBusy(const DateTime &start, const DateTime &end);

    /// attach appointment to resource and node
    bool attach();
    /// detach appointment from resource and node
    void detach();
    
    void addInterval(AppointmentInterval *a);
    void addInterval(AppointmentInterval &a) 
        { addInterval(new AppointmentInterval(a)); }
    void addInterval(const DateTime &start, const DateTime &end, double load=100);
    void addInterval(const DateTime &start, const Duration &duration, double load=100);
    
    const AppointmentIntervalList &intervals() const { return m_intervals; }

    bool loadXML(QDomElement &element, Project &project, Schedule &sch);
    void saveXML(QDomElement &element) const;

    /**
     * Returns the planned effort and cost for the interval start to end (inclusive).
     * Only dates with any planned effort is returned.
     */
    EffortCostMap plannedPrDay(const QDate& start, const QDate& end) const;
    
    /// Returns the planned effort from start to end
    Duration effort(const DateTime &start, const DateTime &end) const;
    /// Returns the planned effort from start for the duration
    Duration effort(const DateTime &start, const Duration &duration) const;
    /// Returns the planned effort from time onwards
    Duration effortFrom(const DateTime &time) const;
    
    /// Returns the total planned effort for this appointment
    Duration plannedEffort() const;
    /// Returns the planned effort on the date
    Duration plannedEffort(const QDate &date) const;
    /// Returns the planned effort upto and including date
    Duration plannedEffortTo(const QDate &date) const;

    /// Returns the total actual effort for this appointment
    Duration actualEffort() const;
    /// Returns the actual effort on the date
    Duration actualEffort(const QDate &date) const;
    /// Returns the actual effort on the date
    Duration actualEffortTo(const QDate &date) const;

     /// Calculates the total planned cost for this appointment
    double plannedCost();
    /// Calculates the planned cost on date
    double plannedCost(const QDate &date);
    /// Calculates the planned cost upto and including date
    double plannedCostTo(const QDate &date);

     /// Calculates the total actual cost for this appointment
    double actualCost();
     /// Calculates the actual cost on date
    double actualCost(const QDate &date);
    /// Calculates the actual cost upto and including date
    double actualCostTo(const QDate &date);

    Appointment &operator=(const Appointment &app);
    Appointment &operator+=(const Appointment &app);
    Appointment operator+(const Appointment &app);
    
    void addActualEffort(QDate date, Duration effort, bool overtime=false);
    
private:
    Schedule *m_node;
    Schedule *m_resource;
    
    Duration m_repeatInterval;
    int m_repeatCount;
    Q3PtrList<Duration> m_extraRepeats;
    Q3PtrList<Duration> m_skipRepeats;

    AppointmentIntervalList m_intervals;
    
    class UsedEffortItem {
    public:
        UsedEffortItem(QDate date, Duration effort, bool overtime=false);
        QDate date();
        Duration effort();
        bool isOvertime();
    private:
        QDate m_date;
        Duration m_effort;
        bool m_overtime;
    };
    class UsedEffort : Q3PtrList<UsedEffortItem> {
    public:
        UsedEffort();
        ~UsedEffort() {}
        void inSort(QDate date, Duration effort, bool overtime=false);
        Duration usedEffort(bool includeOvertime=true) const;
        Duration usedEffort(const QDate &date, bool includeOvertime=true) const;
        Duration usedEffortTo(const QDate &date, bool includeOvertime=true) const;
        Duration usedOvertime() const;
        Duration usedOvertime(const QDate &date) const;
        Duration usedOvertimeTo(const QDate &date) const;
        bool load(QDomElement &element);
        void save(QDomElement &element) const;
    
    protected:
        int compareItems(Q3PtrCollection::Item item1, Q3PtrCollection::Item item2);
    };
    
    UsedEffort m_actualEffort;

#ifndef NDEBUG
public:
        void printDebug(QString ident);
#endif
};


}  //KPlato namespace

#endif
