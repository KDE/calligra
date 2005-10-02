/* This file is part of the KDE project
   Copyright (C) 2003, 2004 Dag Andersen <danders@get2net.dk>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTCALENDAR_H
#define KPTCALENDAR_H

#include "kptmap.h"
#include "kptduration.h"

#include <qptrlist.h>
#include <qpair.h>

class QDomElement;
class QDateTime;
class QTime;
class QDate;
class QString;

namespace KPlato
{

class KPTDateTime;
class KPTProject;

class KPTCalendarDay {

public:
    KPTCalendarDay();
    KPTCalendarDay(int state);
    KPTCalendarDay(QDate date, int state=0);
    KPTCalendarDay(KPTCalendarDay *day);
    ~KPTCalendarDay();

    bool load(QDomElement &element);
    void save(QDomElement &element);

    const QPtrList<QPair<QTime, QTime> > &workingIntervals() const { return m_workingIntervals; }
    void addInterval(QPair<QTime, QTime> *interval);
    void addInterval(QPair<QTime, QTime> interval) { addInterval(new QPair<QTime, QTime>(interval)); }
    void clearIntervals() { m_workingIntervals.clear(); }
    void setIntervals(QPtrList<QPair<QTime, QTime> > intervals) { 
        m_workingIntervals.clear();
        m_workingIntervals = intervals;
    }
    
    bool hasIntervalBefore(const QTime &time) const;
    bool hasIntervalAfter(const QTime &time) const;

    QTime startOfDay() const;
    QTime endOfDay() const;
    
    QDate date() const { return m_date; }
    void setDate(QDate date) { m_date = date; }
    int state() const { return m_state; }
    void setState(int state) { m_state = state; }

    bool operator==(const KPTCalendarDay *day) const;
    bool operator!=(const KPTCalendarDay *day) const;

    /**
     * Returns the amount of 'worktime' that can be done on
     * this day between the times @param start and @param end.
     */
    KPTDuration effort(const QTime &start, const QTime &end);

    /**
     * Returns the actual 'work interval' for the interval @param start to @param end.
     * If no 'work interval' exists, returns the interval start, end.
     * Use @ref hasInterval() to check if a 'work interval' exists.
     */
    QPair<QTime, QTime> interval(const QTime &start, const QTime &end) const;
    /**
     * Returns true if at least a part of a 'work interval' exists 
     * for the interval @param start to @param end.
     */
    bool hasInterval(const QTime &start, const QTime &end) const;
    
    KPTDuration duration() const;
    
    const KPTCalendarDay &copy(const KPTCalendarDay &day);

private:
    QDate m_date; //NOTE: inValid if used for weekdays
    int m_state;
    QPtrList<QPair<QTime, QTime> > m_workingIntervals;

#ifndef NDEBUG
public:
    void printDebug(QCString indent="");
#endif
};

class KPTCalendarWeekdays {

public:
    KPTCalendarWeekdays();
    KPTCalendarWeekdays(KPTCalendarWeekdays *weekdays);
    ~KPTCalendarWeekdays();

    bool load(QDomElement &element);
    void save(QDomElement &element);

    void addWeekday(KPTCalendarDay *day) { m_weekdays.append(day); }
    const QPtrList<KPTCalendarDay> &weekdays() const { return m_weekdays; }
    /**
     * Returns the pointer to KPTCalendarDay for @day or 0 if not defined. 
     * day is 0..6.
     */
    KPTCalendarDay *weekday(int day) const;

    KPTIntMap map();
    
    void setWeekday(KPTIntMap::iterator it, int state) { m_weekdays.at(it.key())->setState(state); }

    int state(int weekday) const;
    void setState(int weekday, int state);
    
    const QPtrList<QPair<QTime, QTime> > &intervals(int weekday) const;
    void setIntervals(int weekday, QPtrList<QPair<QTime, QTime> >intervals);
    void clearIntervals(int weekday);
    
    bool operator==(const KPTCalendarWeekdays *weekdays) const;
    bool operator!=(const KPTCalendarWeekdays *weekdays) const;

    KPTDuration effort(const QDate &date, const QTime &start, const QTime &end);
    
    /**
     * Returns the actual 'work interval' on the weekday defined by @param date
     * for the interval @param start to @param end.
     * If no 'work interval' exists, returns the interval start, end.
     * Use @ref hasInterval() to check if a 'work interval' exists.
     */
    QPair<QTime, QTime> interval(const QDate date, const QTime &start, const QTime &end) const;
    /**
     * Returns true if at least a part of a 'work interval' exists 
     * on the weekday defined by @param date
     * for the interval @param start to @param end.
     */
    bool hasInterval(const QDate date, const QTime &start, const QTime &end) const;
    
    bool hasInterval() const;

    KPTDuration duration() const;
    KPTDuration duration(int weekday) const;

    /// Returns the time when the @param weekday starts
    QTime startOfDay(int weekday) const;
    /// Returns the time when the @param weekday ends
    QTime endOfDay(int weekday) const;

    const KPTCalendarWeekdays &copy(const KPTCalendarWeekdays &weekdays);

private:
    QPtrList<KPTCalendarDay> m_weekdays;

    double m_workHours;

#ifndef NDEBUG
public:
    void printDebug(QCString indent="");
#endif
};

class KPTCalendarWeeks {

public:
    KPTCalendarWeeks();
    KPTCalendarWeeks(KPTCalendarWeeks *weeks);
    ~KPTCalendarWeeks();

    bool load(QDomElement &element);
    void save(QDomElement &element);

    void setWeek(int week, int year, int type);
    void setWeek(KPTWeekMap::iterator it, int state){ m_weeks.insert(it, state); }
    void setWeeks(KPTWeekMap m){ m_weeks = m; }
    KPTWeekMap weeks() const { return m_weeks; }
    
    bool operator==(const KPTCalendarWeeks *w) const { 
        return weeks() == w->weeks(); 
    }
    bool operator!=(const KPTCalendarWeeks *w) const { 
        return weeks() != w->weeks(); 
    }
    
    int state(const QDate &date);

    KPTCalendarWeeks &copy(KPTCalendarWeeks &weeks);

private:
    KPTWeekMap m_weeks;

#ifndef NDEBUG
public:
    void printDebug(QCString indent="");
#endif
};

class KPTStandardWorktime;
class KPTCalendar {

public:
    KPTCalendar();
    KPTCalendar(QString name, KPTCalendar *parent=0);
    KPTCalendar(KPTCalendar *calendar);
    KPTCalendar(KPTStandardWorktime &wt);
    ~KPTCalendar();

    QString name() const { return m_name; }
    void setName(QString name) { m_name = name; }

    KPTCalendar *parent() const { return m_parent; }
    void setParent(KPTCalendar *parent) { m_parent = parent; }
    
    KPTProject *project() const { return m_project; }
    void setProject(KPTProject *project);

    bool isDeleted() const { return m_deleted; }
    void setDeleted(bool yes);

    QString id() const { return m_id; }
    bool setId(QString id);
    void generateId();
    
    bool load(QDomElement &element);
    void save(QDomElement &element);

    KPTCalendarDay *findDay(const QDate &date, bool skipNone=false) const;
    bool addDay(KPTCalendarDay *day) { return m_days.insert(0, day); }
    bool removeDay(KPTCalendarDay *day) { return m_days.removeRef(day); }
    const QPtrList<KPTCalendarDay> &days() const { return m_days; }
    
    void setWeek(int week, int year, int type) { m_weeks->setWeek(week, year, type); }
    void setWeek(KPTWeekMap::iterator it, int state){ m_weeks->setWeek(it, state); }
    KPTWeekMap weekMap() { return m_weeks->weeks(); }
    KPTCalendarWeeks *weeks() { return m_weeks; }
    
    KPTIntMap weekdaysMap() { return m_weekdays->map(); }
    void setWeekday(KPTIntMap::iterator it, int state) { m_weekdays->setWeekday(it, state); }
    KPTCalendarWeekdays *weekdays() { return m_weekdays; }
    KPTCalendarDay *weekday(int day) const { return m_weekdays->weekday(day); }

    QString parentId() const { return m_parentId; }
    void setParentId(QString id) { m_parentId = id; }

    bool hasParent(KPTCalendar *cal);

    /**
     * Returns the amount of 'worktime' that can be done on
     * the date @param date between the times @param start and @param end.
     */
    KPTDuration effort(const QDate &date, const QTime &start, const QTime &end);
    /**
     * Returns the amount of 'worktime' that can be done in the
     * interval from @param start with the duration @param duration
     */
    KPTDuration effort(const KPTDateTime &start, const KPTDuration &duration);

    /**
     * Returns the actual 'work interval' for the interval 
     * starting at @param start with duration @param duration.
     * If no 'work interval' exists, returns the interval start, start+duration.
     * Use @ref hasInterval() to check if a 'work interval' exists.
     */
    QPair<KPTDateTime, KPTDateTime> interval(const KPTDateTime &start, const KPTDateTime &end) const;
    
    bool hasIntervalBefore(const KPTDateTime &time) const;
    bool hasIntervalAfter(const KPTDateTime &time) const;

    /**
     * Returns true if at lesst a part of a 'work interval' exists 
     * for the interval starting at @param start with duration @param duration.
     */
    bool hasInterval(const KPTDateTime &start, const KPTDateTime &end) const;
        
    KPTDateTime availableAfter(const KPTDateTime &time);
    KPTDateTime availableBefore(const KPTDateTime &time);

    KPTCalendar *findCalendar() const { return findCalendar(m_id); }
    KPTCalendar *findCalendar(const QString &id) const;
    bool removeId() { return removeId(m_id); }
    bool removeId(const QString &id);
    void insertId(const QString &id);
    
protected:
    const KPTCalendar &copy(KPTCalendar &calendar);
    void init();
    
private:
    QString m_name;
    KPTCalendar *m_parent;
    KPTProject *m_project;
    bool m_deleted;
    QString m_id;
    QString m_parentId;

    QPtrList<KPTCalendarDay> m_days;
    KPTCalendarWeeks *m_weeks;
    KPTCalendarWeekdays *m_weekdays;

#ifndef NDEBUG
public:
    void printDebug(QCString indent="");
#endif
};

class KPTStandardWorktime
{
public:
    KPTStandardWorktime();
    KPTStandardWorktime(KPTStandardWorktime* worktime);
    ~KPTStandardWorktime();
    
    /// The work time of a normal year.
    KPTDuration durationYear() const;
    /// Set the work time of a normal year.
    void setYear(KPTDuration year) { m_year = year; }
    
    /// The work time of a normal month
    KPTDuration durationMonth() const;
    /// Set the work time of a normal month
    void setMonth(KPTDuration month) { m_month = month; }
    
    /// The work time of a normal week
    KPTDuration durationWeek() const;
    /// Set the work time of a normal week
    void setWeek();
    
    /// The work time of a @param weekday
    KPTDuration durationWeekday(int weekday) const;
    
    /// The work time of a normal day
    KPTDuration durationDay() const;
    /// The number of work time in a normal day
    void setDay();
    
    /// Returns the time when the @param weekday starts
    QTime startOfDay(int weekday) const;
    QTime startOfDay(const QDate &date) const;
    /// Returns the time when the @param weekday ends
    QTime endOfDay(int weekday) const;
    QTime endOfDay(const QDate &date) const;
    
    KPTCalendarDay day() const { return m_day;}
    KPTCalendarWeekdays weekdays() const { return m_weekdays;}
    
    bool load(QDomElement &element);
    void save(QDomElement &element);

    int state(int weekday) const;
    int state(const QDate &date) const;
    void setState(int weekday, int state);
    
    const QPtrList<QPair<QTime, QTime> > &intervals() const { 
        return m_day.workingIntervals(); 
    }
    const QPtrList<QPair<QTime, QTime> > &intervals(int weekday) const { 
        return m_weekdays.intervals(weekday); 
    }
    void setIntervals(QPtrList<QPair<QTime, QTime> >intervals) {
        m_day.setIntervals(intervals); 
    }
    void setIntervals(int weekday, QPtrList<QPair<QTime, QTime> >intervals) { 
        m_weekdays.setIntervals(weekday, intervals); 
    }

    void clearIntervals() { m_day.clearIntervals(); }
    void clearIntervals(int weekday) { m_weekdays.clearIntervals(weekday); }
    
    KPTDateTime workStartAfter(const KPTDateTime &dt) const;
    KPTDateTime workFinishBefore(const KPTDateTime &dt) const;

protected:
    void init();
    
private:
    KPTDuration m_year;
    KPTDuration m_month;
    KPTCalendarDay m_day;
    KPTCalendarWeekdays m_weekdays;
    
#ifndef NDEBUG
public:
    void printDebug(QCString indent="");
#endif
};

}  //KPlato namespace

#endif
