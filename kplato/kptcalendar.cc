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

#include "kptcalendar.h"
#include "kptduration.h"
#include "kptdatetime.h"
#include "kptproject.h"

#include <qdom.h>
#include <qptrlist.h>

#include <klocale.h>
#include <kdebug.h>

namespace KPlato
{

/////   KPTCalendarDay   ////
KPTCalendarDay::KPTCalendarDay()
    : m_state(0) {

    m_workingIntervals.setAutoDelete(true);
}

KPTCalendarDay::KPTCalendarDay(int state)
    : m_state(state) {

    m_workingIntervals.setAutoDelete(true);
}

KPTCalendarDay::KPTCalendarDay(QDate date, int state)
    : m_date(date),
      m_state(state) {

    m_workingIntervals.setAutoDelete(true);
}

KPTCalendarDay::KPTCalendarDay(KPTCalendarDay *day) {
    copy(*day);
}

KPTCalendarDay::~KPTCalendarDay() {
    //kdDebug()<<k_funcinfo<<endl;
}

const KPTCalendarDay &KPTCalendarDay::copy(const KPTCalendarDay &day) {
    //kdDebug()<<k_funcinfo<<endl;
    m_date = day.date();
    m_state = day.state();
    m_workingIntervals.clear();
    QPtrListIterator<QPair<QTime, QTime> > it = day.workingIntervals();
    for(; it.current(); ++it) {
        m_workingIntervals.append(new QPair<QTime, QTime>(it.current()->first, it.current()->second));
        //kdDebug()<<k_funcinfo<<"Date "<<m_date<<": "<<it.current()->first<<","<< it.current()->second<<endl;
    }
    return *this;
}

bool KPTCalendarDay::load(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    bool ok=false;
    m_state = QString(element.attribute("state", "-1")).toInt(&ok);
    if (m_state < 0)
        return false;
    //kdDebug()<<k_funcinfo<<" state="<<m_state<<endl;
    if (element.hasAttribute("date"))
        m_date = QDate::fromString(element.attribute("date"));
        
    clearIntervals();
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "interval") {
                //kdDebug()<<k_funcinfo<<"Interval start="<<e.attribute("start")<<" end="<<e.attribute("end")<<endl;
                QTime start = QTime::fromString(e.attribute("start"));
                QTime end = QTime::fromString(e.attribute("end"));
                addInterval(new QPair<QTime, QTime>(start,end));
            }
        }
    }
    return true;
}

void KPTCalendarDay::save(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<m_date.toString()<<endl;
    if (m_state == KPTMap::None)
        return;
    if (m_date.isValid()) {
        element.setAttribute("date", m_date.toString());
    }
    element.setAttribute("state", m_state);
    if (m_workingIntervals.count() == 0)
        return;
    
    QPtrListIterator<QPair<QTime, QTime> > it = m_workingIntervals;
    for (; it.current(); ++it) {
        QDomElement me = element.ownerDocument().createElement("interval");
        element.appendChild(me);
        me.setAttribute("end", it.current()->second.toString());
        me.setAttribute("start", it.current()->first.toString());
    }
} 

void KPTCalendarDay::addInterval(QPair<QTime, QTime> *interval) {
    m_workingIntervals.append(interval);
}

QTime KPTCalendarDay::startOfDay() const {
    QTime t;
    if (!m_workingIntervals.isEmpty()) {
        QPtrListIterator<QPair<QTime, QTime> > it = m_workingIntervals;
        t = it.current()->first;
        for (++it; it.current(); ++it) {
            if (t > it.current()->first)
                t = it.current()->first;
        }
    }
    return t;
}

QTime KPTCalendarDay::endOfDay() const {
    QTime t;
    if (!m_workingIntervals.isEmpty()) {
        QPtrListIterator<QPair<QTime, QTime> > it = m_workingIntervals;
        t = it.current()->second;
        for (++it; it.current(); ++it) {
            if (t > it.current()->second)
                t = it.current()->second;
        }
    }
    return t;
}
    
bool KPTCalendarDay::operator==(const KPTCalendarDay *day) const {
    return m_state == day->state() && m_workingIntervals == day->workingIntervals();
}
bool KPTCalendarDay::operator!=(const KPTCalendarDay *day) const {
    return m_state != day->state() || m_workingIntervals != day->workingIntervals();
}

KPTDuration KPTCalendarDay::effort(const QTime &start, const QTime &end) {
    //kdDebug()<<k_funcinfo<<start.toString()<<" - "<<end.toString()<<endl;
    KPTDuration eff;
    if (m_state != KPTMap::Working) {
        //kdDebug()<<k_funcinfo<<"Non working day"<<endl;
        return eff;
    }
    QPtrListIterator<QPair<QTime, QTime> > it = m_workingIntervals;
    for (; it.current(); ++it) {
        //kdDebug()<<k_funcinfo<<"Interval: "<<it.current()->first.toString()<<" - "<<it.current()->second.toString()<<endl;
        if (end > it.current()->first && start < it.current()->second) {
            KPTDateTime dtStart(QDate::currentDate(), start);
            if (start < it.current()->first) {
                dtStart.setTime(it.current()->first);
            }
            KPTDateTime dtEnd(QDate::currentDate(), end);
            if (end > it.current()->second) {
                dtEnd.setTime(it.current()->second);
            }
            eff += dtEnd - dtStart;
            //kdDebug()<<k_funcinfo<<dtStart.time().toString()<<" - "<<dtEnd.time().toString()<<"="<<eff.toString(KPTDuration::Format_Day)<<endl;
        }
    }
    //kdDebug()<<k_funcinfo<<start.toString()<<" - "<<end.toString()<<": total="<<eff.toString(KPTDuration::Format_Day)<<endl;
    return eff;
}

QPair<QTime, QTime> KPTCalendarDay::interval(const QTime &start, const QTime &end) const {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<QPair<QTime, QTime> > it = m_workingIntervals;
    for (; it.current(); ++it) {
        QTime t1, t2;
        if (start < it.current()->second && end > it.current()->first) {
            start > it.current()->first ? t1 = start : t1 = it.current()->first;
            end < it.current()->second ? t2 = end : t2 = it.current()->second;
            //kdDebug()<<k_funcinfo<<t1.toString()<<" to "<<t2.toString()<<endl;
            return QPair<QTime, QTime>(t1, t2);
        }
    }
    kdError()<<k_funcinfo<<"No interval"<<endl;
    return QPair<QTime, QTime>(start, end); // hmmmm, what else to do?
}

bool KPTCalendarDay::hasInterval(const QTime &start, const QTime &end) const {
    //kdDebug()<<k_funcinfo<<start.toString()<<" - "<<end.toString()<<endl;
    QPtrListIterator<QPair<QTime, QTime> > it = m_workingIntervals;
    for (; it.current(); ++it) {
        if ((start < it.current()->first && end > it.current()->second) || // whole interval
            (start >= it.current()->first && start < it.current()->second) || // starts inside
            (end > it.current()->first && end <= it.current()->second)) // ends inside
        {
            //kdDebug()<<k_funcinfo<<"true:"<<it.current()->first.toString()<<" - "<<it.current()->second.toString()<<endl;
            return true;
        }
    }
    return false;
}

bool KPTCalendarDay::hasIntervalBefore(const QTime &time) const {
    //kdDebug()<<k_funcinfo<<start.toString()<<" - "<<end.toString()<<endl;
    QPtrListIterator<QPair<QTime, QTime> > it = m_workingIntervals;
    for (; it.current(); ++it) {
        if (time > it.current()->first) {
            //kdDebug()<<k_funcinfo<<"true:"<<it.current()->first.toString()<<" - "<<it.current()->second.toString()<<endl;
            return true;
        }
    }
    return false;
}

bool KPTCalendarDay::hasIntervalAfter(const QTime &time) const {
    //kdDebug()<<k_funcinfo<<start.toString()<<" - "<<end.toString()<<endl;
    QPtrListIterator<QPair<QTime, QTime> > it = m_workingIntervals;
    for (; it.current(); ++it) {
        if (time < it.current()->second) {
            //kdDebug()<<k_funcinfo<<"true:"<<it.current()->first.toString()<<" - "<<it.current()->second.toString()<<endl;
            return true;
        }
    }
    return false;
}

KPTDuration KPTCalendarDay::duration() const {
    KPTDuration dur;
    QPtrListIterator<QPair<QTime, QTime> > it = m_workingIntervals;
    for (; it.current(); ++it) {
        KPTDateTime start(QDate::currentDate(), it.current()->first);
        KPTDateTime end(QDate::currentDate(), it.current()->second);
        dur += end - start;
    }
    return dur;
}

/////   KPTCalendarWeekdays   ////
KPTCalendarWeekdays::KPTCalendarWeekdays()
    : m_workHours(40) {

    for (int i=0; i < 7; ++i) {
        m_weekdays.append(new KPTCalendarDay());
    }
    m_weekdays.setAutoDelete(true);
}

KPTCalendarWeekdays::KPTCalendarWeekdays(KPTCalendarWeekdays *weekdays) {
    copy(*weekdays);
}

KPTCalendarWeekdays::~KPTCalendarWeekdays() {
    //kdDebug()<<k_funcinfo<<endl;
}

const KPTCalendarWeekdays &KPTCalendarWeekdays::copy(const KPTCalendarWeekdays &weekdays) {
    //kdDebug()<<k_funcinfo<<endl;
    m_weekdays.clear();
    QPtrListIterator<KPTCalendarDay> it = weekdays.weekdays();
    for (; it.current(); ++it) {
        m_weekdays.append(new KPTCalendarDay(it.current()));
    }
    return *this;
}

bool KPTCalendarWeekdays::load(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    bool ok;
    int dayNo = QString(element.attribute("day","-1")).toInt(&ok);
    if (dayNo < 0 || dayNo > 6) {
        kdError()<<k_funcinfo<<"Illegal weekday: "<<dayNo<<endl;
        return true; // we continue anyway
    }
    KPTCalendarDay *day = m_weekdays.at(dayNo);
    if (!day)
        day = new KPTCalendarDay();
    if (!day->load(element))
        day->setState(KPTMap::None);
    return true;
}

void KPTCalendarWeekdays::save(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<KPTCalendarDay> it = m_weekdays;
    for (int i=0; it.current(); ++it) {
        QDomElement me = element.ownerDocument().createElement("weekday");
        element.appendChild(me);
        me.setAttribute("day", i++);
        it.current()->save(me);
    }
}    

KPTIntMap KPTCalendarWeekdays::map() {
    KPTIntMap days;
    for (unsigned int i=0; i < m_weekdays.count(); ++i) {
        if (m_weekdays.at(i)->state() > 0)
            days.insert(i+1, m_weekdays.at(i)->state()); //Note: day numbers 1..7
    }
    return days;
}

int KPTCalendarWeekdays::state(int weekday) const {
    KPTCalendarDay *day = const_cast<KPTCalendarWeekdays*>(this)->m_weekdays.at(weekday);
    if (!day)
        return 0;
    return day->state();
}

void KPTCalendarWeekdays::setState(int weekday, int state) {
    KPTCalendarDay *day = m_weekdays.at(weekday);
    if (!day)
        return;
    day->setState(state);
}

const QPtrList<QPair<QTime, QTime> > &KPTCalendarWeekdays::intervals(int weekday) const { 
    KPTCalendarDay *day = const_cast<KPTCalendarWeekdays*>(this)->m_weekdays.at(weekday);
    Q_ASSERT(day);
    return day->workingIntervals();
}

void KPTCalendarWeekdays::setIntervals(int weekday, QPtrList<QPair<QTime, QTime> >intervals) {
    KPTCalendarDay *day = m_weekdays.at(weekday);
    if (day)
        day->setIntervals(intervals); 
}

void KPTCalendarWeekdays::clearIntervals(int weekday) {
    KPTCalendarDay *day = m_weekdays.at(weekday);
    if (day)
        day->clearIntervals(); 
}

bool KPTCalendarWeekdays::operator==(const KPTCalendarWeekdays *wd) const {
    if (m_weekdays.count() != wd->weekdays().count())
        return false;
    for (unsigned int i=0; i < m_weekdays.count(); ++i) {
        // is there a better way to get around this const stuff?
        KPTCalendarDay *day1 = const_cast<KPTCalendarWeekdays*>(this)->m_weekdays.at(i);
        KPTCalendarDay *day2 = const_cast<QPtrList<KPTCalendarDay>&>(wd->weekdays()).at(i);
        if (day1 != day2)
            return false;
    }
    return true;
}
bool KPTCalendarWeekdays::operator!=(const KPTCalendarWeekdays *wd) const {
    if (m_weekdays.count() != wd->weekdays().count())
        return true;
    for (unsigned int i=0; i < m_weekdays.count(); ++i) {
        // is there a better way to get around this const stuff?
        KPTCalendarDay *day1 = const_cast<KPTCalendarWeekdays*>(this)->m_weekdays.at(i);
        KPTCalendarDay *day2 = const_cast<QPtrList<KPTCalendarDay>&>(wd->weekdays()).at(i);
        if (day1 != day2)
            return true;
    }
    return false;
}

KPTDuration KPTCalendarWeekdays::effort(const QDate &date, const QTime &start, const QTime &end) {
    //kdDebug()<<k_funcinfo<<"Day of week="<<date.dayOfWeek()-1<<endl;
    KPTDuration eff;
    KPTCalendarDay *day = weekday(date.dayOfWeek()-1);
    if (day) {
        eff = day->effort(start, end);
    }
    return eff;
}

QPair<QTime, QTime> KPTCalendarWeekdays::interval(const QDate date, const QTime &start, const QTime &end) const {
    //kdDebug()<<k_funcinfo<<endl;
    KPTCalendarDay *day = weekday(date.dayOfWeek()-1);
    if (day && day->hasInterval(start, end))
        return day->interval(start, end);
        
    return QPair<QTime, QTime>(start, end); // what to do?
}

bool KPTCalendarWeekdays::hasInterval(const QDate date, const QTime &start, const QTime &end) const {
    //kdDebug()<<k_funcinfo<<date.toString()<<": "<<start.toString()<<" - "<<end.toString()<<endl;
    KPTCalendarDay *day = weekday(date.dayOfWeek()-1);
    return day && day->hasInterval(start, end);
}

bool KPTCalendarWeekdays::hasInterval() const {
    //kdDebug()<<k_funcinfo<<date.toString()<<": "<<start.toString()<<" - "<<end.toString()<<endl;
    QPtrListIterator<KPTCalendarDay> it = m_weekdays;
    for (; it.current(); ++it) {
        if (it.current()->state() == KPTMap::Working)
            return true;
    }
    return false;
}

KPTCalendarDay *KPTCalendarWeekdays::weekday(int day) const {
    QPtrListIterator<KPTCalendarDay> it = m_weekdays;
    for (int i=0; it.current(); ++it, ++i) {
        if (i == day)
            return it.current();
    }
    return 0;
}

KPTDuration KPTCalendarWeekdays::duration() const {
    KPTDuration dur;
    QPtrListIterator<KPTCalendarDay> it = m_weekdays;
    for (; it.current(); ++it) {
        dur += it.current()->duration();
    }
    return dur;
}

KPTDuration KPTCalendarWeekdays::duration(int _weekday) const {
    KPTCalendarDay *day = weekday(_weekday);
    if (day)
        return day->duration();
    return KPTDuration();
}

QTime KPTCalendarWeekdays::startOfDay(int _weekday) const {
    KPTCalendarDay *day = weekday(_weekday);
    if (day)
        return day->startOfDay();
    return QTime();
}

QTime KPTCalendarWeekdays::endOfDay(int _weekday) const {
    KPTCalendarDay *day = weekday(_weekday);
    if (day)
        return day->endOfDay();
    return QTime();
}
    

/////   KPTCalendarWeek  ////
KPTCalendarWeeks::KPTCalendarWeeks() {
}

KPTCalendarWeeks::~KPTCalendarWeeks() {
    //kdDebug()<<k_funcinfo<<endl;
}

KPTCalendarWeeks::KPTCalendarWeeks(KPTCalendarWeeks *weeks) {
    copy(*weeks);
}

KPTCalendarWeeks &KPTCalendarWeeks::copy(KPTCalendarWeeks& weeks) {
    m_weeks = weeks.weeks();
    return *this;
}

bool KPTCalendarWeeks::load(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    bool ok;
    int w = QString(element.attribute("week","-1")).toInt(&ok);
    int y = QString(element.attribute("year","-1")).toInt(&ok);
    int s = QString(element.attribute("state","-1")).toInt(&ok);
    m_weeks.insert(w, y, s);
    return true;
}

void KPTCalendarWeeks::save(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    KPTWeekMap::iterator it;
    for (it = m_weeks.begin(); it != m_weeks.end(); ++it) {
        QDomElement me = element.ownerDocument().createElement("week");
        element.appendChild(me);
        QPair<int, int> w = KPTWeekMap::week(it.key());
        me.setAttribute("week", w.first);
        me.setAttribute("year", w.second);
        me.setAttribute("state", it.data());
    }
}    

void KPTCalendarWeeks::setWeek(int week, int year, int type) {
    m_weeks.insert(week, year, type);
}

int KPTCalendarWeeks::state(const QDate &date) {
    int year;
    int week = date.weekNumber(&year);
    int st = m_weeks.state(week, year);
    //kdDebug()<<k_funcinfo<<week<<", "<<year<<"="<<st<<endl;
    return st;
}

/////   KPTCalendar   ////

KPTCalendar::KPTCalendar()
    : m_parent(0),
      m_project(0),
      m_deleted(false) {

    init();
}

KPTCalendar::KPTCalendar(QString name, KPTCalendar *parent)
    : m_name(name),
      m_parent(parent),
      m_project(0),
      m_deleted(false) {
    
    init();
}

KPTCalendar::KPTCalendar(KPTStandardWorktime &wt)
    : m_name(i18n("Standard calendar")),
      m_parent(0),
      m_project(0),
      m_deleted(false) {
    
    for (int i=-1; i > -1000; i--) {
        if (setId(QString().setNum(i)))
            break;
    }
    m_days.setAutoDelete(true);
    m_weeks = new KPTCalendarWeeks();
    m_weekdays = new KPTCalendarWeekdays(wt.weekdays());
    
}

KPTCalendar::~KPTCalendar() {
    //kdDebug()<<k_funcinfo<<"deleting "<<m_name<<endl;
    removeId();
    delete m_weeks; 
    delete m_weekdays; 
}
KPTCalendar::KPTCalendar(KPTCalendar *calendar) {
    copy(*calendar);
}

const KPTCalendar &KPTCalendar::copy(KPTCalendar &calendar) {
    m_name = calendar.name();
    m_parent = calendar.parent();
    m_deleted = calendar.isDeleted();
    m_id = calendar.id();
    QPtrListIterator<KPTCalendarDay> it = calendar.days();
    for (; it.current(); ++it) {
        m_days.append(new KPTCalendarDay(it.current()));
    }
    m_weeks = new KPTCalendarWeeks(calendar.weeks());
    m_weekdays = new KPTCalendarWeekdays(calendar.weekdays());
    return *this;
}

void KPTCalendar::init() {
    m_days.setAutoDelete(true);
    m_weeks = new KPTCalendarWeeks();
    m_weekdays = new KPTCalendarWeekdays();
}

void KPTCalendar::setProject(KPTProject *project) { 
    m_project = project;
    generateId();
}

void KPTCalendar::setDeleted(bool yes) {
    if (yes) {
        removeId();
    } else {
        setId(m_id);
    }
    m_deleted = yes;
}
bool KPTCalendar::setId(QString id) {
    //kdDebug()<<k_funcinfo<<id<<endl;
    if (id.isEmpty()) {
        kdError()<<k_funcinfo<<"id is empty"<<endl;
        m_id = id;
        return false;
    }
    KPTCalendar *c = findCalendar();
    if (c == this) {
        //kdDebug()<<k_funcinfo<<"My id found, remove it"<<endl;
        removeId();
    } else if (c) {
        //can happen when making a copy
        kdError()<<k_funcinfo<<"My id '"<<m_id<<"' already used for different node: "<<c->name()<<endl;
    }
    if (findCalendar(id)) {
        kdError()<<k_funcinfo<<"id '"<<id<<"' is already used for different node: "<<findCalendar(id)->name()<<endl;
        m_id = QString(); // hmmm
        return false;
    }
    m_id = id;
    insertId(id);
    //kdDebug()<<k_funcinfo<<m_name<<": inserted id="<<id<<endl;
    return true;
}

void KPTCalendar::generateId() {
    if (!m_id.isEmpty()) {
        removeId();
    }
    for (int i=0; i<32000 ; ++i) {
        m_id = m_id.setNum(i);
        if (!findCalendar()) {
            insertId(m_id);
            return;
        }
    }
    m_id = QString();
}

bool KPTCalendar::load(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<element.text()<<endl;
    bool ok;
    setId(element.attribute("id"));
    m_parentId = element.attribute("parent");
    m_name = element.attribute("name","");
    //TODO parent
    
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "weekday") {
                if (!m_weekdays->load(e))
                    return false;
            }
            if (e.tagName() == "week") {
                if (!m_weeks->load(e)) {
                    return false;
                }
            }
            if (e.tagName() == "day") {
                KPTCalendarDay *day = new KPTCalendarDay();
                if (day->load(e)) {
                    if (!day->date().isValid()) {
                        delete day;
                        kdError()<<k_funcinfo<<m_name<<": Failed to load calendarDay - Invalid date"<<endl;
                    } else {
                        KPTCalendarDay *d = findDay(day->date());
                        if (d) {
                            // already exists, keep the new
                            removeDay(d);
                            kdWarning()<<k_funcinfo<<m_name<<" Load calendarDay - Date already exists"<<endl;
                        }
                    }
                    addDay(day);
                } else {
                    delete day;
                    kdError()<<k_funcinfo<<"Failed to load calendarDay"<<endl;
                    return true; //false; don't throw away the whole calendar
                }
            }
        }
    }
    return true;
}

void KPTCalendar::save(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (m_deleted)
        return;
    
    QDomElement me = element.ownerDocument().createElement("calendar");
    element.appendChild(me);
    if (m_parent) 
        me.setAttribute("parent", m_parent->id());
    me.setAttribute("name", m_name);
    me.setAttribute("id", m_id);
    m_weeks->save(me);
    m_weekdays->save(me);
    QPtrListIterator<KPTCalendarDay> it = m_days;
    for (; it.current(); ++it) {
        QDomElement e = me.ownerDocument().createElement("day");
        me.appendChild(e);
        it.current()->save(e);
    }
    
}

KPTCalendarDay *KPTCalendar::findDay(const QDate &date, bool skipNone) const {
    //kdDebug()<<k_funcinfo<<date.toString()<<endl;
    QPtrListIterator<KPTCalendarDay> it = m_days;
    for (; it.current(); ++it) {
        if (skipNone && it.current()->state() == KPTMap::None)
            continue;
        if (it.current()->date() == date)
            return it.current();
    }
    //kdDebug()<<k_funcinfo<<date.toString()<<" not found"<<endl;
    return 0;
}

bool KPTCalendar::hasParent(KPTCalendar *cal) {
    //kdDebug()<<k_funcinfo<<endl;
    if (!m_parent)
        return false;
    if (m_parent == cal)
        return true;
    return m_parent->hasParent(cal);
}

KPTDuration KPTCalendar::effort(const QDate &date, const QTime &start, const QTime &end) {
    //kdDebug()<<k_funcinfo<<endl;
    KPTDuration eff;
    if (start == end)
        return eff;
    QTime _start = start;
    QTime _end = end;
    if (start > end) {
        _start = end;
        _end = start;
    }
    // first, check day
    KPTCalendarDay *day = findDay(date, true);
    if (day) {
        eff = day->effort(_start, _end);
    } else if (m_weeks && m_weeks->state(date) == KPTMap::NonWorking) {
        // nonworking week, no effort
        //kdDebug()<<k_funcinfo<<"Non working week"<<endl;
    } else if (m_weekdays) {
        eff = m_weekdays->effort(date, _start, _end);
    }
    //kdDebug()<<k_funcinfo<<date.toString()<<"="<<eff.toString(KPTDuration::Format_Day)<<endl;
    return eff;
}

KPTDuration KPTCalendar::effort(const KPTDateTime &start, const KPTDuration &duration) {
    //kdDebug()<<k_funcinfo<<start.toString()<<" duration "<<duration.toString()<<endl;
    KPTDuration eff;
    if (duration == KPTDuration::zeroDuration)
        return eff;
    QDate date = start.date();
    QTime startTime = start.time();
    KPTDateTime end = start + duration;
    QTime endTime = end.time();
    if (end.date() > date) {
        endTime.setHMS(23, 59, 59);
    }
    eff = effort(date, startTime, endTime); // first day
    // Now get all the rest of the days
    int i=0; //FIXME: for safety
    for (date = date.addDays(1); i < 50 && date <= end.date(); date = date.addDays(1)) {
        if (date < end.date())
             eff += effort(date, QTime(), QTime(23, 59, 59)); // whole day
        else 
             eff += effort(date, QTime(), end.time());
        //kdDebug()<<k_funcinfo<<": eff now="<<eff.toString(KPTDuration::Format_Day)<<endl;
    }
    //kdDebug()<<k_funcinfo<<start.date().toString()<<"- "<<end.date().toString()<<": total="<<eff.toString(KPTDuration::Format_Day)<<endl;
    return eff;
}

QPair<KPTDateTime, KPTDateTime> KPTCalendar::interval(const KPTDateTime &start, const KPTDateTime &end) const {
    //kdDebug()<<k_funcinfo<<start.toString()<<" - "<<end.toString()<<endl;
    QTime startTime;
    QTime endTime;
    QDate date = start.date();
    for (int i=0; date <= end.date() && i<20; date = date.addDays(1), ++i) { //FIXME
        if (date < end.date())
            endTime = QTime(23, 59, 59);
        else
            endTime = end.time();
        if (date > start.date())
            startTime = QTime();
        else 
            startTime = start.time();
            
        //kdDebug()<<k_funcinfo<<date.toString()<<": "<<startTime.toString()<<" - "<<endTime.toString()<<endl;
        KPTCalendarDay *day = findDay(date, true);
        if (day) {
            if (day->hasInterval(startTime, endTime)) {
                QPair<QTime, QTime> res = day->interval(startTime, endTime);
                //kdDebug()<<k_funcinfo<<res.first.toString()<<" to "<<res.second.toString()<<endl;
                return QPair<KPTDateTime, KPTDateTime>(KPTDateTime(date,res.first), KPTDateTime(date,res.second));
            }
        } else if (m_weeks && m_weeks->state(date) == KPTMap::NonWorking) {
            //kdDebug()<<k_funcinfo<<date.toString()<<" is nonworking day"<<endl;
            continue;
        } else if (m_weekdays && m_weekdays->hasInterval(date, startTime, endTime)) {
            QPair<QTime, QTime> res = m_weekdays->interval(date, startTime, endTime);
            //kdDebug()<<k_funcinfo<<res.first.toString()<<" to "<<res.second.toString()<<endl;
            return QPair<KPTDateTime, KPTDateTime>(KPTDateTime(date,res.first), KPTDateTime(date,res.second));
        }
    }
    kdError()<<k_funcinfo<<"Didn't find an interval"<<endl;
    return QPair<KPTDateTime, KPTDateTime>(start, end); // hmmmm, what else to do?
}

bool KPTCalendar::hasIntervalBefore(const KPTDateTime &time) const {
    if (m_weekdays->hasInterval()) {
        return true;
    }
    QPtrListIterator<KPTCalendarDay> it = m_days;
    for (; it.current(); ++it) {
        if (it.current()->hasIntervalBefore(time.time())) {
            return true; //TODO: check weeks?
        }
    }
    return false;
}
bool KPTCalendar::hasIntervalAfter(const KPTDateTime &time) const {
    if (m_weekdays->hasInterval()) {
        return true;
    }
    QPtrListIterator<KPTCalendarDay> it = m_days;
    for (; it.current(); ++it) {
        if (it.current()->hasIntervalAfter(time.time())) {
            return true; //TODO: check weeks?
        }
    }
    return false;
}

bool KPTCalendar::hasInterval(const KPTDateTime &start, const KPTDateTime &end) const {
    //kdDebug()<<k_funcinfo<<start.toString()<<" - "<<end.toString()<<endl;
    QTime startTime;
    QTime endTime;
    QDate date = start.date();
    for (int i=0; date <= end.date() && i<50; date = date.addDays(1), ++i) { // FIXME
        if (date < end.date())
            endTime = QTime(23, 59, 59);
        else
            endTime = end.time();
        if (date > start.date())
            startTime = QTime();
        else 
            startTime = start.time();
            
        KPTCalendarDay *day = findDay(date, true);
        if (day) {
            if (day->hasInterval(startTime, endTime))
                return true;
        } else if (m_weeks && m_weeks->state(date) == KPTMap::NonWorking) {
             continue;
        } else if (m_weekdays && m_weekdays->hasInterval(date, startTime, endTime)) {
            return true;
        }
    }
    return false;
}

KPTDateTime KPTCalendar::availableAfter(const KPTDateTime &time) {
    //kdDebug()<<k_funcinfo<<m_name<<" "<<time.toString()<<endl;
    KPTDateTime start = time;
    KPTDateTime end(time.date(), QTime(23,59,59));
    KPTDateTime t = time;
    int i=28;
    for (; i > 0 && !hasInterval(start, end); --i) { //FIXME
        end = end.addDays(1);
        start = KPTDateTime(end.date(), QTime());
    }
    if (i > 0)
        t = interval(start, end).first;
    //kdDebug()<<k_funcinfo<<m_name<<" "<<t.toString()<<endl;
    return t;
}

KPTDateTime KPTCalendar::availableBefore(const KPTDateTime &time) {
    //kdDebug()<<k_funcinfo<<time.toString()<<endl;
    KPTDateTime start(time.date(), QTime());
    KPTDateTime end = time;
    KPTDateTime t = time;
    int i=28;
    for (; i > 0 && !hasInterval(start, end); --i) { //FIXME
        start = start.addDays(-1);
        end = KPTDateTime(start.date(), QTime(23,59,59));
    }
    if (i > 0)
        t = interval(start, end).second;
    //kdDebug()<<k_funcinfo<<t.toString()<<endl;
    return t;
}

KPTCalendar *KPTCalendar::findCalendar(const QString &id) const { 
    return (m_project ? m_project->findCalendar(id) : 0); 
}

bool KPTCalendar::removeId(const QString &id) { 
    return (m_project ? m_project->removeCalendarId(id) : false); 
}

void KPTCalendar::insertId(const QString &id){ 
    if (m_project)
        m_project->insertCalendarId(id, this); 
}

/////////////
KPTStandardWorktime::KPTStandardWorktime() {
    init();
}

KPTStandardWorktime::KPTStandardWorktime(KPTStandardWorktime *worktime) {
    if (worktime) {
        m_year = worktime->durationYear();
        m_month = worktime->durationMonth();
        m_day.copy(worktime->day());
        m_weekdays.copy(worktime->weekdays());
    } else {
        init();
    }
}

KPTStandardWorktime::~KPTStandardWorktime() {
    //kdDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
}

void KPTStandardWorktime::init() {
    // Some temporary sane default values
    m_year = KPTDuration(0, 1760, 0);
    m_month = KPTDuration(0, 176, 0);
    m_day.setState(KPTMap::Working);
    m_day.addInterval(new QPair<QTime, QTime>(QTime(8, 0, 0), QTime(16, 0, 0)));
    
    for (int i=0; i<5; ++i) {
        setState(i, KPTMap::Working);
        QPtrList<QPair<QTime, QTime> > l;
        l.append(new QPair<QTime, QTime>(QTime(8, 0, 0), QTime(16, 0, 0)));
        setIntervals(i, l);
    }
    m_weekdays.setState(5, KPTMap::NonWorking);
    m_weekdays.setState(6, KPTMap::NonWorking);
}    

KPTDuration KPTStandardWorktime::durationYear() const {
    return m_year;
}

KPTDuration KPTStandardWorktime::durationMonth() const {
    return m_month;
}

KPTDuration KPTStandardWorktime::durationWeek() const {
    return m_weekdays.duration();
}

void KPTStandardWorktime::setWeek() {
}

KPTDuration KPTStandardWorktime::durationWeekday(int weekday) const {
    return m_weekdays.duration(weekday);
}

KPTDuration KPTStandardWorktime::durationDay() const {
    return m_day.duration();
}

void KPTStandardWorktime::setDay() {
}

QTime KPTStandardWorktime::startOfDay(int weekday) const {
    return m_weekdays.startOfDay(weekday);
}

QTime KPTStandardWorktime::startOfDay(const QDate &date) const {
    return m_weekdays.startOfDay(date.dayOfWeek()-1);
}

QTime KPTStandardWorktime::endOfDay(int weekday) const {
    return m_weekdays.endOfDay(weekday);
}
    
QTime KPTStandardWorktime::endOfDay(const QDate &date) const {
    return m_weekdays.endOfDay(date.dayOfWeek()-1);
}
    
bool KPTStandardWorktime::load(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    m_year = KPTDuration::fromString(element.attribute("year"), KPTDuration::Format_Hour); //FIXME
    m_month = KPTDuration::fromString(element.attribute("month"), KPTDuration::Format_Hour); //FIXME
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "weekday") {
                if (!m_weekdays.load(e))
                    return false;
            }
            if (e.tagName() == "day") {
                if (m_day.load(e)) {
                    if (!m_day.date().isValid()) {
                        kdError()<<k_funcinfo<<"Failed to load calendarDay: Invalid date"<<endl;
                    }
                } else {
                    kdError()<<k_funcinfo<<"Failed to load calendarDay"<<endl;
                    return false;
                }
            }
        }
    }
    return true;
}

void KPTStandardWorktime::save(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    QDomElement me = element.ownerDocument().createElement("standard-worktime");
    element.appendChild(me);
    me.setAttribute("year", m_year.toString(KPTDuration::Format_Hour)); //FIXME
    me.setAttribute("month", m_month.toString(KPTDuration::Format_Hour)); //FIXME
    QDomElement e = me.ownerDocument().createElement("day");
    me.appendChild(e);
    m_day.save(e);
    m_weekdays.save(me);
}

int KPTStandardWorktime::state(int weekday) const {
    return m_weekdays.state(weekday);
}

int KPTStandardWorktime::state(const QDate &date) const {
    return m_weekdays.state(date.dayOfWeek()-1);
}

void KPTStandardWorktime::setState(int weekday, int state) {
    m_weekdays.setState(weekday, state);
}

KPTDateTime KPTStandardWorktime::workStartAfter(const KPTDateTime &dt) const {
    QDate date = dt.date();
    if (state(date) == KPTMap::Working && endOfDay(date) > dt.time())
        return dt;
    date.addDays(1);
    for (int i = 0; i < 6; ++i) {
        if (state(date) == KPTMap::Working)
            return KPTDateTime(date, startOfDay(date));
        date = date.addDays(1);
    }
    return dt;
}

KPTDateTime KPTStandardWorktime::workFinishBefore(const KPTDateTime &dt) const {
    QDate date = dt.date();
    if (state(date) == KPTMap::Working && startOfDay(date) < dt.time())
        return dt;
    date = date.addDays(-1);
    for (int i = 0; i < 6; ++i) {
        if (state(date) == KPTMap::Working)
            return KPTDateTime(date, endOfDay(date));
        date = date.addDays(-1);
    }
    return dt;
}

#ifndef NDEBUG
void KPTCalendarDay::printDebug(QCString indent) {
    QString s[] = {"None", "Non-working", "Working"};
    kdDebug()<<indent<<" "<<m_date.toString()<<" = "<<s[m_state]<<endl;
    if (m_state == KPTMap::Working) {
        indent += "  ";
        QPtrListIterator<QPair<QTime, QTime> > it = m_workingIntervals;
        for (; it.current(); ++it) {
            kdDebug()<<indent<<" Interval: "<<it.current()->first<<" to "<<it.current()->second<<endl;
        }
    }
    
}
void KPTCalendarWeekdays::printDebug(QCString indent) {
    kdDebug()<<indent<<"Weekdays ------"<<endl;
    QPtrListIterator<KPTCalendarDay> it = m_weekdays;
    for (char c='0'; it.current(); ++it) {
        it.current()->printDebug(indent + "  Day " + c++ + ": ");
    }

}
void KPTCalendarWeeks::printDebug(QCString indent) {
    QCString s[] = {"None", "Non-working", "Working"};
    kdDebug()<<indent<<" Weeks ------"<<endl;
    indent += "  ";
    KPTWeekMap::iterator it;
    for (it = m_weeks.begin(); it != m_weeks.end(); ++it) {
        kdDebug()<<indent<<" Week: "<<it.key()<<" = "<<s[it.data()]<<endl;
    }
}
void KPTCalendar::printDebug(QCString indent) {
    kdDebug()<<indent<<"KPTCalendar "<<m_id<<": '"<<m_name<<"' Deleted="<<m_deleted<<endl;
    kdDebug()<<indent<<"  Parent: "<<(m_parent ? m_parent->name() : "No parent")<<endl;
    m_weekdays->printDebug(indent + "  ");
    m_weeks->printDebug(indent + "  ");
    kdDebug()<<indent<<"  Days --------"<<endl;
    QPtrListIterator<KPTCalendarDay> it = m_days;
    for (; it.current(); ++it) {
        it.current()->printDebug(indent + "  ");
    }
}

void KPTStandardWorktime::printDebug(QCString indent) {
    kdDebug()<<indent<<"KPTStandardWorktime "<<endl;
    kdDebug()<<indent<<"Year: "<<m_year.toString()<<endl;
    kdDebug()<<indent<<"Month: "<<m_year.toString()<<endl;
    m_weekdays.printDebug(indent + "  ");
    kdDebug()<<indent<<"   Day ------"<<endl;
    m_day.printDebug(indent + "  ");
}

#endif

}  //KPlato namespace
