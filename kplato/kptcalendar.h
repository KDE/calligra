/* This file is part of the KDE project
   Copyright (C) 2003 Dag Andersen <danders@get2net.dk>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KPTCALENDAR_H
#define KPTCALENDAR_H

#include "kptmap.h"

#include <qptrlist.h>
#include <qpair.h>

class QDomElement;
class QTime;
class KPTDuration;
class KPTDateTime;

class KPTCalendarDay {

public:
    KPTCalendarDay(int state=0);
    KPTCalendarDay(QDate date, int state=0);
    KPTCalendarDay(KPTCalendarDay *day);
    ~KPTCalendarDay() {}

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

    QDate date() const { return m_date; }
    void setDate(QDate date) { m_date = date; }
    int state() const { return m_state; }
    void setState(int state) { m_state = state; }

    bool operator==(const KPTCalendarDay *day) const;
    bool operator!=(const KPTCalendarDay *day) const;

    /**
     * Returns the amount of 'worktime' that can be done on
     * this day between the times @parem start and @param end.
     */
    KPTDuration effort(const QTime &start, const QTime &end);

protected:
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
    ~KPTCalendarWeekdays() {}

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

    bool operator==(const KPTCalendarWeekdays *weekdays) const;
    bool operator!=(const KPTCalendarWeekdays *weekdays) const;

    KPTDuration effort(const QDate &date, const QTime &start, const QTime &end);
    
protected:
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
    ~KPTCalendarWeeks() {}

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

protected:
    KPTCalendarWeeks &copy(KPTCalendarWeeks &weeks);

private:
    KPTWeekMap m_weeks;

#ifndef NDEBUG
public:
    void printDebug(QCString indent="");
#endif
};


class KPTCalendar {

public:
    KPTCalendar();
    KPTCalendar(QString name, KPTCalendar *parent=0);
    KPTCalendar(KPTCalendar *calendar);
    ~KPTCalendar() { delete m_weeks; delete m_weekdays; }

    QString name() const { return m_name; }
    void setName(QString name) { m_name = name; }

    KPTCalendar *parent() const { return m_parent; }
    void setParent(KPTCalendar *parent) { m_parent = parent; }

    bool isDeleted() const { return m_deleted; }
    void setDeleted(bool yes) { m_deleted = yes; }

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }
    
    bool load(QDomElement &element);
    void save(QDomElement &element);

    KPTCalendarDay *findDay(const QDate &date);
    void addDay(KPTCalendarDay *day) { m_days.append(day); }
    const QPtrList<KPTCalendarDay> &days() const { return m_days; }
    
    void setWeek(int week, int year, int type) { m_weeks->setWeek(week, year, type); }
    void setWeek(KPTWeekMap::iterator it, int state){ m_weeks->setWeek(it, state); }
    KPTWeekMap weekMap() { return m_weeks->weeks(); }
    KPTCalendarWeeks *weeks() { return m_weeks; }
    
    KPTIntMap weekdaysMap() { return m_weekdays->map(); }
    void setWeekday(KPTIntMap::iterator it, int state) { m_weekdays->setWeekday(it, state); }
    KPTCalendarWeekdays *weekdays() { return m_weekdays; }
    KPTCalendarDay *weekday(int day) const { return m_weekdays->weekday(day); }

    int parentId() const { return m_parentId; }
    void setParentId(int id) { m_parentId == id; }

    bool hasParent(KPTCalendar *cal);

    /**
     * Returns the amount of 'worktime' that can be done on
     * the date @param date between the times @parem start and @param end.
     */
    KPTDuration effort(const QDate &date, const QTime &start, const QTime &end);
    /**
     * Returns the amount of 'worktime' that can be done in the
     * interval from @param start with the duration @param duration
     */
    KPTDuration effort(const KPTDateTime &start, const KPTDuration &duration);

    /**
     * Used for estimation and calculation of effort.
     */
    // FIXME
    double standardDay() { return 8; } // hours
    double standardWeek() { return 5*standardDay(); }  // hours
    double standardMonth() { return 22*standardDay(); } // hours
    double standardYear() { return 220*standardDay(); } // hours
    
protected:
    const KPTCalendar &copy(KPTCalendar &calendar);
    void init();
    
private:
    QString m_name;
    KPTCalendar *m_parent;
    bool m_deleted;
    int m_id;
    int m_parentId;

    QPtrList<KPTCalendarDay> m_days;
    KPTCalendarWeeks *m_weeks;
    KPTCalendarWeekdays *m_weekdays;


#ifndef NDEBUG
public:
    void printDebug(QCString indent="");
#endif
};

#endif
