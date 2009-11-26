/* This file is part of the KDE project
   Copyright (C) 2003 - 2007 Dag Andersen <danders@get2net.dk>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTCALENDAR_H
#define KPTCALENDAR_H

#include <kptdatetime.h>
#include "kptduration.h"
#include "kplatokernel_export.h"

#include <QPair>
#include <QList>
#include <QMap>
#include <QStringList>

#include <kglobal.h>
#include <klocale.h>
#include <ktimezone.h>
#include <kdebug.h>

#include <KoXmlReader.h>

class QDomElement;
class QTime;
class QDate;
class QString;

/// The main namespace.
namespace KPlato
{

class Calendar;
class Project;
class IntMap;
class DateTime;
class Project;
class Schedule;
class XMLLoaderObject;

class KPLATOKERNEL_EXPORT DateTimeInterval : public QPair<DateTime, DateTime>
{
public:
    DateTimeInterval()
    : QPair<DateTime, DateTime>()
    {}
    DateTimeInterval( const DateTime &t1, const DateTime &t2 )
    : QPair<DateTime, DateTime>( t1, t2 )
    {}
    DateTimeInterval &operator=( const DateTimeInterval &other ) {
        first = other.first; second = other.second;
        return *this;
    }
    bool isValid() const { return first.isValid() && second.isValid(); }
    void limitTo( const DateTime &start, const DateTime &end ) {
        if ( ! first.isValid() || ( start.isValid() && start > first ) ) {
            first = start;
        }
        if ( ! second.isValid() || ( end.isValid() && end < second ) ) {
            second = end;
        }
        if ( isValid() && first > second ) {
            first = second = DateTime();
        }
    }
    void limitTo( const DateTimeInterval &interval ) {
        limitTo( interval.first, interval.second );
    }

    DateTimeInterval limitedTo( const DateTime &start, const DateTime &end ) const {
        DateTimeInterval i = *this;
        i.limitTo( start, end );
        return i;
    }
    DateTimeInterval limitedTo( const DateTimeInterval &interval ) const {
        return limitedTo( interval.first, interval.second );
    }
    QString toString() const {
        return QString( "%1 to %2" )
                    .arg( first.isValid()?first.toString():"''" )
                    .arg( second.isValid()?second.toString():"''" );
    }
};

/// TimeInterval is defined as a start time and a length.
/// The end time (start + length) must not exceed midnight
class KPLATOKERNEL_EXPORT TimeInterval : public QPair<QTime, int>
{
public:
    TimeInterval()
    : QPair<QTime, int>( QTime(), -1 )
    {}
    explicit TimeInterval( const QPair<QTime, int> &value )
    : QPair<QTime, int>( value )
    {
        init();
    }
    TimeInterval( const QTime &start, int length )
    : QPair<QTime, int>( start, length )
    {
        init();
    }
    TimeInterval( const TimeInterval &value )
    : QPair<QTime, int>( value.first, value.second )
    {
        init();
    }
    /// Return the intervals start time
    QTime startTime() const { return first; }
    /// Return the intervals calculated end time. Note: It may return QTime(0,0,0)
    QTime endTime() const { return first.addMSecs( second ); }
    double hours() const { return (double)(second) / ( 1000. * 60. * 60. ); }
    /// Returns true if this interval ends at midnight, and thus endTime() returns QTime(0,0,0)
    bool endsMidnight() const { return endTime() == QTime( 0, 0, 0 ); }

    bool isValid() const { return first.isValid() && second > 0; }
    bool isNull() const { return first.isNull() || second < 0; }

    TimeInterval &operator=( const TimeInterval &ti ) { 
        first = ti.first;
        second = ti.second;
        return *this;
    }
    /// Returns true if the intervals overlap in any way
    bool intersects( const TimeInterval &ti ) const {
        if ( ! isValid() || ! ti.isValid() ) {
            return false;
        }
        if ( endsMidnight() && ti.endsMidnight() ) {
            return true;
        }
        if ( endsMidnight() ) {
            return first < ti.endTime();
        }
        if ( ti.endsMidnight() ) {
            return ti.first < endTime();
        }
        return ( first < ti.endTime() && endTime() > ti.first ) || ( ti.first < endTime() && ti.endTime() > first );
    }
protected:
    void init()
    {
        int s = QTime( 0, 0, 0 ).msecsTo( first );
        if ( ( s + second ) > 86400000 ) {
            second = 86400000 - s;
            kError()<<"Overflow, limiting length to"<<second;
        }
    }
};


class KPLATOKERNEL_EXPORT CalendarDay {

public:
    enum State { Undefined = 0,
                 None=Undefined, // depreciated
                 NonWorking=1, Working=2 };
    
    CalendarDay();
    CalendarDay(int state);
    explicit CalendarDay(const QDate& date, int state=0);
    CalendarDay(CalendarDay *day);
    ~CalendarDay();

    bool load( KoXmlElement &element, XMLLoaderObject &status );
    void save(QDomElement &element) const;

    const QList<TimeInterval*> &workingIntervals() const { return m_workingIntervals; }
    QList<TimeInterval*> workingIntervals() { return m_workingIntervals; }
    void addInterval( const QTime &t1, int length ) { addInterval( new TimeInterval( t1, length ) ); }
    void addInterval(TimeInterval *interval);
    void addInterval(TimeInterval interval) { addInterval(new TimeInterval(interval)); }
    void clearIntervals() { m_workingIntervals.clear(); }
    void setIntervals(QList<TimeInterval*> intervals) { 
        m_workingIntervals.clear();
        m_workingIntervals = intervals;
    }
    void removeInterval( TimeInterval *interval );
    TimeInterval *intervalAt( int index ) const;
    int indexOf( const TimeInterval *ti ) const;
    int numIntervals() const;
    
    QDate date() const { return m_date; }
    void setDate(const QDate& date) { m_date = date; }
    int state() const { return m_state; }
    void setState(int state) { m_state = state; }

    bool operator==(const CalendarDay *day) const;
    bool operator==(const CalendarDay &day) const;
    bool operator!=(const CalendarDay *day) const;
    bool operator!=(const CalendarDay &day) const;

    Duration workDuration() const;
    /**
     * Returns the amount of 'worktime' that can be done on
     * this day between the times start and end.
     */
    Duration effort(const QTime &start, int length, const KDateTime::Spec &spec, Schedule *sch=0);
    /**
     * Returns the amount of 'worktime' that can be done on
     * this day between the times start and end.
     */
    Duration effort(const QDate &date, const QTime &start, int length, const KDateTime::Spec &spec, Schedule *sch=0);

    /**
     * Returns the actual 'work interval' for the interval start to end.
     * If no 'work interval' exists, returns the interval start, end.
     * Use @ref hasInterval() to check if a 'work interval' exists.
     */
    TimeInterval interval(const QTime &start, int length, const KDateTime::Spec &spec,  Schedule *sch=0) const;
    
    /**
     * Returns the actual 'work interval' for the interval start to end.
     * If no 'work interval' exists, returns the interval start, end.
     * Use @ref hasInterval() to check if a 'work interval' exists.
     */
    TimeInterval interval(const QDate date, const QTime &start, int length, const KDateTime::Spec &spec, Schedule *sch=0) const;
    
    bool hasInterval() const;

    /**
     * Returns true if at least a part of a 'work interval' exists 
     * for the interval start to end.
     */
    bool hasInterval(const QTime &start, int length, const KDateTime::Spec &spec, Schedule *sch=0) const;
    
    /**
     * Returns true if at least a part of a 'work interval' exists 
     * for the interval @p start to @p start + @p length.
     * Assumes this day is date. (Used by weekday hasInterval().)
     * If @p sch is not 0, the schedule is checked for availability.
     */
    bool hasInterval(const QDate date, const QTime &start, int length, const KDateTime::Spec &spec, Schedule *sch=0) const;
    
    Duration duration() const;
    
    const CalendarDay &copy(const CalendarDay &day);

    static QString stateToString( int st, bool trans = false ) {
        if ( st == None ) {
            return trans ? i18n( "Undefined" ) : "Undefined";
        } else if ( st == NonWorking ) {
            return trans ? i18n( "Non-working" ) : "Non-working";
        } else if ( st == Working ) {
            return trans ?  i18n( "Working" ) : "Working";
        }
        return QString();
    }
    static QStringList stateList( bool trans = false ) {
        QStringList lst;
        return trans
            ? lst << i18n( "Undefined" ) << i18n( "Non-working" ) << i18n( "Working" )
            : lst << "Undefined" << "Non-working" << "Working";
    }

private:
    Calendar *m_calendar;
    QDate m_date; //NOTE: inValid if used for weekdays
    int m_state;
    QList<TimeInterval*> m_workingIntervals;

#ifndef NDEBUG
public:
    void printDebug(const QString& indent=QString());
#endif
};

class KPLATOKERNEL_EXPORT CalendarWeekdays {

public:
    CalendarWeekdays();
    CalendarWeekdays( const CalendarWeekdays *weekdays );
    ~CalendarWeekdays();

    bool load( KoXmlElement &element, XMLLoaderObject &status );
    void save(QDomElement &element) const;

    const QList<CalendarDay*> weekdays() const 
        { QList<CalendarDay*> lst = m_weekdays.values(); return lst; }
    /**
     * Returns the pointer to CalendarDay for day.
     * @param day The weekday number, must be between 1 (monday) and 7 (sunday)
     */
    CalendarDay *weekday(int day) const;
    CalendarDay *weekday(const QDate &date) const { return weekday(date.dayOfWeek()-1); }
    
    const QMap<int, CalendarDay*> &weekdayMap() const;
    
    IntMap stateMap() const;
    
//    void setWeekday(IntMap::iterator it, int state) { m_weekdays.at(it.key())->setState(state); }

    int state(const QDate &date) const;
    int state(int weekday) const;
    void setState(int weekday, int state);
    
    QList<TimeInterval*> intervals(int weekday) const;
    void setIntervals(int weekday, QList<TimeInterval*>intervals);
    void clearIntervals(int weekday);
    
    bool operator==(const CalendarWeekdays *weekdays) const;
    bool operator!=(const CalendarWeekdays *weekdays) const;

    Duration effort(const QDate &date, const QTime &start, int length, const KDateTime::Spec &spec, Schedule *sch=0);
    
    /**
     * Returns the actual 'work interval' on the weekday defined by date
     * for the interval @p start to @p start + @p length.
     * If no 'work interval' exists, returns the interval start, end.
     * Use @ref hasInterval() to check if a 'work interval' exists.
     * If @p sch is not 0, the schedule is checked for availability.
     */
    TimeInterval interval(const QDate date, const QTime &start, int length, const KDateTime::Spec &spec, Schedule *sch) const;
    /**
     * Returns true if at least a part of a 'work interval' exists 
     * on the weekday defined by date for the interval start to end.
     */
    bool hasInterval(const QDate date, const QTime &start, int length, const KDateTime::Spec &spec, Schedule *sch) const;
    bool hasInterval() const;

    Duration duration() const;
    Duration duration(int weekday) const;

    const CalendarWeekdays &copy(const CalendarWeekdays &weekdays);

    int indexOf( const CalendarDay *day ) const;
    
private:
    Calendar *m_calendar;
    QMap<int, CalendarDay*> m_weekdays;

#ifndef NDEBUG
public:
    void printDebug(const QString& indent=QString());
#endif
};

/**
 * Calendar defines the working and nonworking days and hours.
 * A day can have the three states Undefined, NonWorking, or Working.
 * A calendar can have a parent calendar that defines the days that are 
 * undefined in this calendar. 
 * If a calendar have no parent, an undefined day defaults to Nonworking.
 * A Working day has one or more work intervals to define the work hours.
 *
 * The definition can consist of two parts: Weekdays and Day.
 * Day has highest priority.
 *
 * A typical calendar hierarchy could include calendars on 4 levels:
 *  1. Definition of normal weekdays and national holidays/vacation days.
 *  2. Definition of the company's special workdays/-time and vacation days.
 *  3. Definitions for groups of resources.
 *  4. Definitions for individual resources.
 *
 * A calendar can define a timezone different from the projects.
 * This enables planning with resources that does not recide in the same place.
 *
 */
class KPLATOKERNEL_EXPORT Calendar : public QObject
{
    Q_OBJECT
public:
    Calendar();
    explicit Calendar(const QString& name, Calendar *parent=0);
    //Calendar( const Calendar &c ); QObject doesn't allow a copy constructor
    ~Calendar();

    const Calendar &operator=(const Calendar &calendar ) { return copy( calendar ); }

    QString name() const { return m_name; }
    void setName(const QString& name);

    Calendar *parentCal() const { return m_parent; }
    /**
     * Set parent calendar to @p parent.
     * Removes myself from current parent and
     * inserts myself as child to new parent.
     */
    void setParentCal(Calendar *parent);
    
    bool isChildOf( const Calendar *cal ) const;
    
    Project *project() const { return m_project; }
    void setProject(Project *project);

    QString id() const { return m_id; }
    void setId(const QString& id);
    
    const QList<Calendar*> &calendars() const { return m_calendars; }
    void addCalendar( Calendar *calendar );
    void takeCalendar( Calendar *calendar );
    int indexOf( const Calendar *calendar ) const;
    /// Return number of children
    int childCount() const { return m_calendars.count(); }
    /// Return child calendar at @p index, 0 if index out of bounds
    Calendar *childAt( int index ) const { return m_calendars.value( index ); }
    
    bool load( KoXmlElement &element, XMLLoaderObject &status );
    void save(QDomElement &element) const;

    int state(const QDate &date) const;
    void setState( CalendarDay *day, CalendarDay::State state );
    void addWorkInterval( CalendarDay *day, TimeInterval *ti );
    void takeWorkInterval( CalendarDay *day, TimeInterval *ti );
    void setWorkInterval( TimeInterval *ti, const TimeInterval &value );

    /**
     * Find the definition for the day @p date.
     * If @p skipUndefined = true the day is NOT returned if it has state Undefined.
     */
    CalendarDay *findDay(const QDate &date, bool skipUndefined=false) const;
    void addDay(CalendarDay *day);
    CalendarDay *takeDay(CalendarDay *day);
    const QList<CalendarDay*> &days() const { return m_days; }
    QList<QPair<CalendarDay*, CalendarDay*> > consecutiveVacationDays() const;
    QList<CalendarDay*> workingDays() const;
    int indexOf( const CalendarDay *day ) const { return m_days.indexOf( const_cast<CalendarDay*>( day ) ); }
    CalendarDay *dayAt( int index ) { return m_days.value( index ); }
    int numDays() const { return m_days.count(); }
    void setDate( CalendarDay *day, const QDate &date );
    CalendarDay *day( const QDate &date ) const;
    
    IntMap weekdayStateMap() const;
    
    CalendarWeekdays *weekdays() const { return m_weekdays; }
    CalendarDay *weekday(int day) const { return m_weekdays->weekday(day); }
    int indexOfWeekday( const CalendarDay *day ) const { return m_weekdays->indexOf( day ); }
    const QList<CalendarDay*> weekdayList() const { return m_weekdays->weekdays(); }
    int numWeekdays() const { return weekdayList().count(); }
    
    /// Sets the @p weekday data to the data in @p day
    void setWeekday( int weekday, const CalendarDay &day );
    
    QString parentId() const { return m_parentId; }
    void setParentId(const QString& id) { m_parentId = id; }
    bool hasParent(Calendar *cal);

    /**
     * Returns the amount of 'worktime' that can be done in the
     * interval from @p start to @p end
     * If @p sch is not 0, the schedule is checked for availability.
     */
    Duration effort(const DateTime &start, const DateTime &end, Schedule *sch=0) const;

    /**
     * Returns the first 'work interval' for the interval 
     * starting at @p start and ending at @p end.
     * If no 'work interval' exists, returns an interval with invalid DateTime.
     * You can also use @ref hasInterval() to check if a 'work interval' exists.
     * If @p sch is not 0, the schedule is checked for availability.
     */
    DateTimeInterval firstInterval(const DateTime &start, const DateTime &end, Schedule *sch=0) const;
    
    /**
     * Returns true if at least a part of a 'work interval' exists 
     * for the interval starting at @p start and ending at @p end.
     * If @p sch is not 0, the schedule is checked for availability.
     */
    bool hasInterval(const DateTime &start, const DateTime &end, Schedule *sch=0) const;
        
    /** 
     * Find the first available time after @p time before @p limit.
     * Return invalid datetime if not available.
     * If @p sch is not 0, the schedule is checked for availability.
     */
    DateTime firstAvailableAfter(const DateTime &time, const DateTime &limit, Schedule *sch = 0);
    /** 
     * Find the first available time backwards from @p time. Search until @p limit.
     * Return invalid datetime if not available.
     * If @p sch is not 0, the schedule is checked for availability.
     */
    DateTime firstAvailableBefore(const DateTime &time, const DateTime &limit, Schedule *sch = 0);

    Calendar *findCalendar() const { return findCalendar(m_id); }
    Calendar *findCalendar(const QString &id) const;
    bool removeId() { return removeId(m_id); }
    bool removeId(const QString &id);
    void insertId(const QString &id);

    const KDateTime::Spec &timeSpec() const { return m_spec; }
    KTimeZone timeZone() const { return m_spec.timeZone(); }
    void setTimeZone( const KTimeZone &tz );
    
    void setDefault( bool on );
    bool isDefault() const { return m_default; }
    
signals:
    void changed( Calendar* );
    void changed( CalendarDay* );
    void changed( TimeInterval* );
    
    void weekdayToBeAdded( CalendarDay *day, int index );
    void weekdayAdded( CalendarDay *day );
    void weekdayToBeRemoved( CalendarDay *day );
    void weekdayRemoved( CalendarDay *day );
    
    void dayToBeAdded( CalendarDay *day, int index );
    void dayAdded( CalendarDay *day );
    void dayToBeRemoved( CalendarDay *day );
    void dayRemoved( CalendarDay *day );
    
    void workIntervalToBeAdded( CalendarDay*, TimeInterval*, int index );
    void workIntervalAdded( CalendarDay*, TimeInterval* );
    void workIntervalToBeRemoved( CalendarDay*, TimeInterval* );
    void workIntervalRemoved( CalendarDay*, TimeInterval* );

protected:
    void init();
    const Calendar &copy(const Calendar &calendar);
    
    /**
     * Returns the amount of 'worktime' that can be done on
     * the @p date between the times @p start and @p start + @p length.
     * The date and times are in timespecification @p spec.
     * If @p sch is not 0, the schedule is checked for availability.
     */
    Duration effort(const QDate &date, const QTime &start, int length, Schedule *sch=0) const;
    /**
     * Returns the first 'work interval' on date for the interval 
     * starting at @p start and ending at @p start + @p length.
     * If no 'work interval' exists, returns a null interval.
     * You can also use @ref hasInterval() to check if a 'work interval' exists.
     * The date and times are in timespecification spec.
     * If @p sch is not 0, the schedule is checked for availability.
     */
    TimeInterval firstInterval(const QDate &date, const QTime &start, int length, Schedule *sch=0) const;
    
    /**
     * Returns true if at least a part of a 'work interval' exists 
     * for the interval on date, starting at @p start and ending at @p start + @p length.
     * If @p sch is not 0, the schedule is checked for availability.
     */
    bool hasInterval(const QDate &date, const QTime &start, int length, Schedule *sch=0) const;

private:
    QString m_name;
    Calendar *m_parent;
    Project *m_project;
    bool m_deleted;
    QString m_id;
    QString m_parentId;

    QList<CalendarDay*> m_days;
    CalendarWeekdays *m_weekdays;

    QList<Calendar*> m_calendars;
    
    KDateTime::Spec m_spec;
    bool m_default; // this is the default calendar, only used for save/load
    
#ifndef NDEBUG
public:
    void printDebug(const QString& indent=QString());
#endif
};

class KPLATOKERNEL_EXPORT StandardWorktime
{
public:
    StandardWorktime( Project *project = 0 );
    StandardWorktime(StandardWorktime* worktime);
    ~StandardWorktime();

    /// Set Project
    void setProject( Project *project ) { m_project = project; }
    /// The work time of a normal year.
    Duration durationYear() const { return m_year; }
    /// The work time of a normal year.
    double year() const { return m_year.toDouble(Duration::Unit_h); }
    /// Set the work time of a normal year.
    void setYear(const Duration year) { m_year = year; }
    /// Set the work time of a normal year.
    void setYear(double hours) { m_year = Duration((qint64)(hours*60.0*60.0*1000.0)); }
    
    /// The work time of a normal month
    Duration durationMonth() const { return m_month; }
    /// The work time of a normal month
    double month() const  { return m_month.toDouble(Duration::Unit_h); }
    /// Set the work time of a normal month
    void setMonth(const Duration month) { m_month = month; }
    /// Set the work time of a normal month
    void setMonth(double hours) { m_month = Duration((qint64)(hours*60.0*60.0*1000.0)); }
    
    /// The work time of a normal week
    Duration durationWeek() const { return m_week; }
    /// The work time of a normal week
    double week() const { return m_week.toDouble(Duration::Unit_h); }
    /// Set the work time of a normal week
    void setWeek(const Duration week) { m_week = week; }
    /// Set the work time of a normal week
    void setWeek(double hours) { m_week = Duration((qint64)(hours*60.0*60.0*1000.0)); }
    
    /// The work time of a normal day
    Duration durationDay() const { return m_day; }
    /// The work time of a normal day
    double day() const { return m_day.toDouble(Duration::Unit_h); }
    /// Set the work time of a normal day
    void setDay(const Duration day) { m_day = day; changed(); }
    /// Set the work time of a normal day
    void setDay(double hours) { m_day = Duration(hours, Duration::Unit_h); changed(); }
    
    QList<double> scales() const;

    bool load( KoXmlElement &element, XMLLoaderObject &status );
    void save(QDomElement &element) const;

    void changed();

protected:
    void init();
    
private:
    Project *m_project;
    Duration m_year;
    Duration m_month;
    Duration m_week;
    Duration m_day;
    
#ifndef NDEBUG
public:
    void printDebug(const QString& indent=QString());
#endif
};

}  //KPlato namespace

#endif
