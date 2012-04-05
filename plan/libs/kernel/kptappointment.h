/* This file is part of the KDE project
   Copyright (C) 2005 - 2011 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

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

#include "kplatokernel_export.h"

#include "kptglobal.h"

#include "kptduration.h"
#include "kptdatetime.h"

#include <KoXmlReader.h>

#include <QString>
#include <QList>
#include <QSharedData>

#include <kdebug.h>

class QDomElement;

namespace KPlato
{

class Effort;
class Appointment;
class Node;
class Resource;
class ResourceRequest;
class EffortCost;
class EffortCostMap;
class Schedule;
class XMLLoaderObject;
class DateTimeInterval;

class AppointmentIntervalData : public QSharedData
{
public:
    AppointmentIntervalData() : load( 0 ) {}
    AppointmentIntervalData( const AppointmentIntervalData &other )
        : QSharedData( other ), start( other.start ), end( other.end ), load( other.load ) {}
    ~AppointmentIntervalData() {}

    DateTime start;
    DateTime end;
    double load; //percent
};

class KPLATOKERNEL_EXPORT AppointmentInterval
{
public:
    AppointmentInterval();
    AppointmentInterval(const AppointmentInterval &AppointmentInterval);
    AppointmentInterval(const DateTime &start, const DateTime &end, double load=100);
    ~AppointmentInterval();
    
    Duration effort() const;
    Duration effort(const DateTime &start, const DateTime end) const;
    Duration effort(const QDate &time, bool upto) const;
    
    bool loadXML(KoXmlElement &element, XMLLoaderObject &status);
    void saveXML(QDomElement &element) const;
    
    const DateTime &startTime() const;
    void setStartTime( const DateTime &time );
    const DateTime &endTime() const;
    void setEndTime( const DateTime &time );
    double load() const;
    void setLoad( double load );
    
    bool isValid() const;
    AppointmentInterval firstInterval(const AppointmentInterval &interval, const DateTime &from) const;

    bool operator==( const AppointmentInterval &interval ) const;
    bool operator<( const AppointmentInterval &interval ) const;

    bool intersects( const AppointmentInterval &other ) const;
    AppointmentInterval interval( const DateTime &start, const DateTime &end ) const;

    QString toString() const;

private:
    QSharedDataPointer<AppointmentIntervalData> d;
};
KPLATOKERNEL_EXPORT QDebug operator<<( QDebug dbg, const KPlato::AppointmentInterval& i );


/**
 * This list is sorted after 1) startdatetime, 2) enddatetime.
 * The intervals do not overlap, an interval does not start before the
 * previous interval ends.
 */
class KPLATOKERNEL_EXPORT AppointmentIntervalList
{
public:
    AppointmentIntervalList();
    AppointmentIntervalList( const QMultiMap<QDate, AppointmentInterval> &other );

    /// Add @p interval to the list. Handle overlapping with existsing intervals.
    void add( const AppointmentInterval &interval );
    /// Add an interval to the list. Handle overlapping with existsing intervals.
    void add( const DateTime &st, const DateTime &et, double load );
    /// Load intervals from document
    bool loadXML(KoXmlElement &element, XMLLoaderObject &status);
    /// Save intervals to document
    void saveXML(QDomElement &element) const;
    
    AppointmentIntervalList &operator+=( const AppointmentIntervalList &lst );
    AppointmentIntervalList &operator-=( const AppointmentIntervalList &lst );
    AppointmentIntervalList &operator=( const AppointmentIntervalList &lst );

    /// Returns the intervals in the range @p start, @p end
    AppointmentIntervalList extractIntervals( const DateTime &start, const DateTime &end ) const;
    
    /// Return the total effort
    Duration effort() const;
    /// Return the effort limited to the interval @p start, @p end
    Duration effort(const DateTime &start, const DateTime &end) const;

    QMultiMap<QDate, AppointmentInterval> map();
    const QMultiMap<QDate, AppointmentInterval> &map() const;
    bool isEmpty() const { return m_map.isEmpty(); }
    void clear() { m_map.clear(); }

protected:
    void subtract( const AppointmentInterval &interval );
    void subtract( const DateTime &st, const DateTime &et, double load );

private:
    QMultiMap<QDate, AppointmentInterval> m_map;
};
KPLATOKERNEL_EXPORT QDebug operator<<( QDebug dbg, const KPlato::AppointmentIntervalList& i );

/**
 * A Resource can be scheduled to be used at any time, 
 * this is represented internally with Appointments
 * There is one Appointment per resource-task pair.
 * An appointment can be divided into several intervals, represented with
 * a list of AppointmentInterval.
 * This list is sorted after 1) startdatetime, 2) enddatetime.
 * The intervals do not overlap, an interval does not start before the
 * previous interval ends.
 * An interval is a countinous time interval with the same load. It can span dates.
 */
class KPLATOKERNEL_EXPORT Appointment {
public:
    explicit Appointment();
    Appointment(Schedule *resource, Schedule *node, DateTime start, DateTime end, double load);
    Appointment(Schedule *resource, Schedule *node, DateTime start, Duration duration, double load);
    Appointment( const Appointment &app );
    ~Appointment();

    bool isEmpty() const { return m_intervals.isEmpty(); }
    void clear();

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
    
    void addInterval(const AppointmentInterval &a);
    void addInterval(const DateTime &start, const DateTime &end, double load=100);
    void addInterval(const DateTime &start, const Duration &duration, double load=100);
    void setIntervals(const AppointmentIntervalList &lst);
    
    const AppointmentIntervalList &intervals() const { return m_intervals; }
    int count() const { return m_intervals.map().count(); }
    AppointmentInterval intervalAt( int index ) const { return m_intervals.map().values().value( index ); }
    /// Return intervals between @p start and @p end
    AppointmentIntervalList intervals( const DateTime &start, const DateTime &end ) const;

    bool loadXML(KoXmlElement &element, XMLLoaderObject &status, Schedule &sch);
    void saveXML(QDomElement &element) const;

    /**
     * Returns the planned effort and cost for the interval start to end (inclusive).
     * Only dates with any planned effort is returned.
     * If start or end is not valid, startTime.date() respectivly endTime().date() is used.
     */
    EffortCostMap plannedPrDay(const QDate& start, const QDate& end, EffortCostCalculationType type = ECCT_All) const;
    
    /// Returns the planned effort from start to end
    Duration effort(const DateTime &start, const DateTime &end, EffortCostCalculationType type = ECCT_All) const;
    /// Returns the planned effort from start for the duration
    Duration effort(const DateTime &start, const Duration &duration, EffortCostCalculationType type = ECCT_All) const;

    /// Returns the total planned effort for @p resource on this appointment
    Duration plannedEffort( const Resource *resource, EffortCostCalculationType type = ECCT_All ) const;
    /// Returns the total planned effort for this appointment
    Duration plannedEffort(EffortCostCalculationType type = ECCT_All) const;
    /// Returns the planned effort on the date
    Duration plannedEffort(const QDate &date, EffortCostCalculationType type = ECCT_All) const;
    /// Returns the planned effort for @p resource on the @p date date
    Duration plannedEffort( const Resource *resource, const QDate &date, EffortCostCalculationType type = ECCT_All ) const;
    /// Returns the planned effort upto and including date
    Duration plannedEffortTo(const QDate &date, EffortCostCalculationType type = ECCT_All) const;
    /// Returns the planned effort upto and including date
    Duration plannedEffortTo( const Resource *resource, const QDate &date, EffortCostCalculationType type = ECCT_All ) const;

     /// Calculates the total planned cost for this appointment
    EffortCost plannedCost(EffortCostCalculationType type = ECCT_All) const;
    /// Calculates the planned cost on date
    double plannedCost(const QDate &date, EffortCostCalculationType type = ECCT_All);
    /// Calculates the planned cost upto and including date
    double plannedCostTo(const QDate &date, EffortCostCalculationType type = ECCT_All);

    Appointment &operator=(const Appointment &app);
    Appointment &operator+=(const Appointment &app);
    Appointment operator+(const Appointment &app);
    Appointment &operator-=(const Appointment &app);
    
    void setCalculationMode( int mode ) { m_calculationMode = mode; }
    int calculationMode() const { return m_calculationMode; }
    
    void merge(const Appointment &app);
    Appointment extractIntervals( const DateTimeInterval &interval ) const;

    void setAuxcilliaryInfo( const QString &info ) { m_auxcilliaryInfo = info; }
    QString auxcilliaryInfo() const { return m_auxcilliaryInfo; }
    
protected:
    void copy(const Appointment &app);
    
private:
    Schedule *m_node;
    Schedule *m_resource;
    int m_calculationMode; // Type of appointment
    Duration m_repeatInterval;
    int m_repeatCount;
    QList<Duration*> m_extraRepeats;
    QList<Duration*> m_skipRepeats;

    AppointmentIntervalList m_intervals;
    
    QString m_auxcilliaryInfo;
};


}  //KPlato namespace

#endif
