/* This file is part of the KDE project
   Copyright (C) 2003 - 2006 Dag Andersen <danders@get2net.dk>

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
#include <QList>

#include <klocale.h>
#include <kdebug.h>

namespace KPlato
{

/////   CalendarDay   ////
CalendarDay::CalendarDay()
    : m_date(),
      m_state(0),
      m_workingIntervals() {

    //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
}

CalendarDay::CalendarDay(int state)
    : m_date(),
      m_state(state),
      m_workingIntervals() {

    //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
}

CalendarDay::CalendarDay(QDate date, int state)
    : m_date(date),
      m_state(state),
      m_workingIntervals() {

    //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
}

CalendarDay::CalendarDay(CalendarDay *day)
    : m_workingIntervals() {

    //kDebug()<<k_funcinfo<<"("<<this<<") from ("<<day<<")"<<endl;
    copy(*day);
}

CalendarDay::~CalendarDay() {
    //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    while (!m_workingIntervals.isEmpty())
        delete m_workingIntervals.takeFirst();
}

const CalendarDay &CalendarDay::copy(const CalendarDay &day) {
    //kDebug()<<k_funcinfo<<"("<<&day<<") date="<<day.date().toString()<<endl;
    m_date = day.date();
    m_state = day.state();
    m_workingIntervals.clear();
    foreach (TimeInterval *i, day.workingIntervals()) {
        m_workingIntervals.append(new TimeInterval(i->first, i->second));
    }
    return *this;
}

bool CalendarDay::load(QDomElement &element) {
    //kDebug()<<k_funcinfo<<endl;
    bool ok=false;
    m_state = QString(element.attribute("state", "-1")).toInt(&ok);
    if (m_state < 0)
        return false;
    //kDebug()<<k_funcinfo<<" state="<<m_state<<endl;
    QString s = element.attribute("date");
    if (!s.isEmpty()) {
        m_date = QDate::fromString(s, Qt::ISODate);
        if (!m_date.isValid())
            m_date = QDate::fromString(s);
    }
    clearIntervals();
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "interval") {
                //kDebug()<<k_funcinfo<<"Interval start="<<e.attribute("start")<<" end="<<e.attribute("end")<<endl;
                QString st = e.attribute("start");
                QString en = e.attribute("end");
                if (!st.isEmpty() && !en.isEmpty()) {
                    QTime start = QTime::fromString(st);
                    QTime end = QTime::fromString(en);
                    addInterval(new TimeInterval(start,end));
                }
            }
        }
    }
    return true;
}

void CalendarDay::save(QDomElement &element) const {
    //kDebug()<<k_funcinfo<<m_date.toString()<<endl;
    if (m_state == Map::None)
        return;
    if (m_date.isValid()) {
        element.setAttribute("date", m_date.toString(Qt::ISODate));
    }
    element.setAttribute("state", m_state);
    if (m_workingIntervals.count() == 0)
        return;
    
    foreach (TimeInterval *i, m_workingIntervals) {
        QDomElement me = element.ownerDocument().createElement("interval");
        element.appendChild(me);
        me.setAttribute("end", i->second.toString());
        me.setAttribute("start", i->first.toString());
    }
} 

void CalendarDay::addInterval(TimeInterval *interval) {
    m_workingIntervals.append(interval);
}

QTime CalendarDay::startOfDay() const {
    QTime t;
    if (!m_workingIntervals.isEmpty()) {
        QListIterator<TimeInterval*> it = m_workingIntervals;
        t = it.next()->first;
        while (it.hasNext()) {
            TimeInterval *i = it.next();
            if (t > i->first)
                t = i->first;
        }
    }
    return t;
}

QTime CalendarDay::endOfDay() const {
    QTime t;
    if (!m_workingIntervals.isEmpty()) {
        QListIterator<TimeInterval*> it = m_workingIntervals;
        t = it.next()->second;
        while (it.hasNext()) {
            TimeInterval *i = it.next();
            if (t > i->second)
                t = i->second;
        }
    }
    return t;
}
    
bool CalendarDay::operator==(const CalendarDay *day) const {
    return operator==(*day);
}
bool CalendarDay::operator==(const CalendarDay &day) const {
    //kDebug()<<k_funcinfo<<endl;
    if (m_date.isValid() && day.date().isValid()) {
        if (m_date != day.date()) {
            //kDebug()<<k_funcinfo<<m_date.toString()<<" != "<<day.date().toString()<<endl;
            return false;
        }
    } else if (m_date.isValid() != day.date().isValid()) {
        //kDebug()<<k_funcinfo<<"one of the dates is not valid"<<endl;
        return false;
    }
    if (m_state != day.state()) {
        //kDebug()<<k_funcinfo<<m_state<<" != "<<day.state()<<endl;
        return false;
    }
    if (m_workingIntervals.count() != day.workingIntervals().count()) {
        //kDebug()<<k_funcinfo<<m_workingIntervals.count()<<" != "<<day.workingIntervals().count()<<endl;
        return false;
    }
    foreach (TimeInterval *a, m_workingIntervals) {
        bool res = false;
        foreach (TimeInterval *b, day.workingIntervals()) {
            if (a->first == b->first && a->second == b->second) {
                res = true;
                break;
            }
        }
        if (res == false) {
            //kDebug()<<k_funcinfo<<"interval mismatch "<<a->first.toString()<<"-"<<a->second.toString()<<endl;
            return false;
        }
    }
    return true;
}
bool CalendarDay::operator!=(const CalendarDay *day) const {
    return operator!=(*day);
}
bool CalendarDay::operator!=(const CalendarDay &day) const {
    return !operator==(day);
}

Duration CalendarDay::effort(const QTime &start, const QTime &end) {
    //kDebug()<<k_funcinfo<<start.toString()<<" - "<<end.toString()<<endl;
    Duration eff;
    if (m_state != Map::Working) {
        //kDebug()<<k_funcinfo<<"Non working day"<<endl;
        return eff;
    }
    foreach (TimeInterval *i, m_workingIntervals) {
        //kDebug()<<k_funcinfo<<"Interval: "<<it.current()->first.toString()<<" - "<<it.current()->second.toString()<<endl;
        if (end > i->first && start < i->second) {
            DateTime dtStart(QDate::currentDate(), start);
            if (start < i->first) {
                dtStart.setTime(i->first);
            }
            DateTime dtEnd(QDate::currentDate(), end);
            if (end > i->second) {
                dtEnd.setTime(i->second);
            }
            eff += dtEnd - dtStart;
            //kDebug()<<k_funcinfo<<dtStart.time().toString()<<" - "<<dtEnd.time().toString()<<"="<<eff.toString(Duration::Format_Day)<<endl;
        }
    }
    //kDebug()<<k_funcinfo<<(m_date.isValid()?m_date.toString(Qt::ISODate):"Weekday")<<": "<<start.toString()<<" - "<<end.toString()<<": total="<<eff.toString(Duration::Format_Day)<<endl;
    return eff;
}

TimeInterval CalendarDay::interval(const QTime &start, const QTime &end) const {
    //kDebug()<<k_funcinfo<<endl;
    QTime t1, t2;
    if (m_state == Map::Working) {
        foreach (TimeInterval *i, m_workingIntervals) {
            if (start < i->second && end > i->first) {
                t1 = start > i->first ? start : i->first;
                t2 = end < i->second ? end : i->second;
                //kDebug()<<k_funcinfo<<t1.toString()<<" to "<<t2.toString()<<endl;
                return TimeInterval(t1, t2);
            }
        }
    }
    //kError()<<k_funcinfo<<"No interval "<<m_date<<": "<<start<<","<<end<<endl;
    return TimeInterval(t1, t2);
}

bool CalendarDay::hasInterval() const {
    return m_state == Map::Working && m_workingIntervals.count() > 0;
}

bool CalendarDay::hasInterval(const QTime &start, const QTime &end) const {
    //kDebug()<<k_funcinfo<<(m_date.isValid()?m_date.toString(Qt::ISODate):"Weekday")<<" "<<start.toString()<<" - "<<end.toString()<<endl;
    if (m_state != Map::Working) {
        return false;
    }
    foreach (TimeInterval *i, m_workingIntervals) {
        if (start < i->second && end > i->first) {
            //kDebug()<<k_funcinfo<<"true:"<<(m_date.isValid()?m_date.toString(Qt::ISODate):"Weekday")<<" "<<i->first.toString()<<" - "<<i->second.toString()<<endl;
            return true;
        }
    }
    return false;
}

Duration CalendarDay::duration() const {
    Duration dur;
    foreach (TimeInterval *i, m_workingIntervals) {
        DateTime start(QDate::currentDate(), i->first);
        DateTime end(QDate::currentDate(), i->second);
        dur += end - start;
    }
    return dur;
}

/////   CalendarWeekdays   ////
CalendarWeekdays::CalendarWeekdays()
    : m_weekdays(),
      m_workHours(40) {

    //kDebug()<<k_funcinfo<<"--->"<<endl;
    for (int i=0; i < 7; ++i) {
        m_weekdays.append(new CalendarDay());
    }
    //kDebug()<<k_funcinfo<<"<---"<<endl;
}

CalendarWeekdays::CalendarWeekdays(CalendarWeekdays *weekdays)
    : m_weekdays() {
    //kDebug()<<k_funcinfo<<"--->"<<endl;
    copy(*weekdays);
    //kDebug()<<k_funcinfo<<"<---"<<endl;
}

CalendarWeekdays::~CalendarWeekdays() {
    while (!m_weekdays.isEmpty())
        delete m_weekdays.takeFirst();
    //kDebug()<<k_funcinfo<<endl;
}

const CalendarWeekdays &CalendarWeekdays::copy(const CalendarWeekdays &weekdays) {
    //kDebug()<<k_funcinfo<<endl;
    while (!m_weekdays.isEmpty()) {
        delete m_weekdays.takeFirst();
    }
    foreach (CalendarDay *d, weekdays.weekdays()) {
        m_weekdays.append(new CalendarDay(d));
    }
    return *this;
}

bool CalendarWeekdays::load(QDomElement &element) {
    //kDebug()<<k_funcinfo<<endl;
    bool ok;
    int dayNo = QString(element.attribute("day","-1")).toInt(&ok);
    if (dayNo < 0 || dayNo > 6) {
        kError()<<k_funcinfo<<"Illegal weekday: "<<dayNo<<endl;
        return true; // we continue anyway
    }
    CalendarDay *day = m_weekdays.at(dayNo);
    if (!day)
        day = new CalendarDay();
    if (!day->load(element))
        day->setState(Map::None);
    return true;
}

void CalendarWeekdays::save(QDomElement &element) const {
    //kDebug()<<k_funcinfo<<endl;
    int i=0;
    foreach (CalendarDay *d, m_weekdays) {
        QDomElement me = element.ownerDocument().createElement("weekday");
        element.appendChild(me);
        me.setAttribute("day", i++);
        d->save(me);
    }
}    

IntMap CalendarWeekdays::map() {
    IntMap days;
    for (unsigned int i=0; i < m_weekdays.count(); ++i) {
        if (m_weekdays.at(i)->state() > 0)
            days.insert(i+1, m_weekdays.at(i)->state()); //Note: day numbers 1..7
    }
    return days;
}

int CalendarWeekdays::state(const QDate &date) const {
    return state(date.dayOfWeek()-1);
}

int CalendarWeekdays::state(int weekday) const {
    CalendarDay *day = const_cast<CalendarWeekdays*>(this)->m_weekdays.at(weekday);
    return day ? day->state() : Map::None;
}

void CalendarWeekdays::setState(int weekday, int state) {
    CalendarDay *day = m_weekdays.at(weekday);
    if (!day)
        return;
    day->setState(state);
}

const QList<TimeInterval*> &CalendarWeekdays::intervals(int weekday) const { 
    CalendarDay *day = const_cast<CalendarWeekdays*>(this)->m_weekdays.at(weekday);
    Q_ASSERT(day);
    return day->workingIntervals();
}

void CalendarWeekdays::setIntervals(int weekday, QList<TimeInterval*>intervals) {
    CalendarDay *day = m_weekdays.at(weekday);
    if (day)
        day->setIntervals(intervals); 
}

void CalendarWeekdays::clearIntervals(int weekday) {
    CalendarDay *day = m_weekdays.at(weekday);
    if (day)
        day->clearIntervals(); 
}

bool CalendarWeekdays::operator==(const CalendarWeekdays *wd) const {
    if (m_weekdays.count() != wd->weekdays().count())
        return false;
    for (unsigned int i=0; i < m_weekdays.count(); ++i) {
        // is there a better way to get around this const stuff?
        CalendarDay *day1 = const_cast<CalendarWeekdays*>(this)->m_weekdays.at(i);
        CalendarDay *day2 = const_cast<QList<CalendarDay*>&>(wd->weekdays()).at(i);
        if (day1 != day2)
            return false;
    }
    return true;
}
bool CalendarWeekdays::operator!=(const CalendarWeekdays *wd) const {
    if (m_weekdays.count() != wd->weekdays().count())
        return true;
    for (unsigned int i=0; i < m_weekdays.count(); ++i) {
        // is there a better way to get around this const stuff?
        CalendarDay *day1 = const_cast<CalendarWeekdays*>(this)->m_weekdays.at(i);
        CalendarDay *day2 = const_cast<QList<CalendarDay*>&>(wd->weekdays()).at(i);
        if (day1 != day2)
            return true;
    }
    return false;
}

Duration CalendarWeekdays::effort(const QDate &date, const QTime &start, const QTime &end) {
    //kDebug()<<k_funcinfo<<"Day of week="<<date.dayOfWeek()-1<<endl;
    CalendarDay *day = weekday(date.dayOfWeek()-1);
    if (day && day->state() == Map::Working) {
        return day->effort(start, end);
    }
    return Duration::zeroDuration;
}

TimeInterval CalendarWeekdays::interval(const QDate date, const QTime &start, const QTime &end) const {
    //kDebug()<<k_funcinfo<<endl;
    CalendarDay *day = weekday(date.dayOfWeek()-1);
    if (day && day->state() == Map::Working) {
        if (day->hasInterval(start, end)) {
            return day->interval(start, end);
        }
    }
    return TimeInterval(QTime(), QTime());
}

bool CalendarWeekdays::hasInterval(const QDate date, const QTime &start, const QTime &end) const {
    //kDebug()<<k_funcinfo<<date.toString()<<": "<<start.toString()<<" - "<<end.toString()<<endl;
    CalendarDay *day = weekday(date.dayOfWeek()-1);
    return day && day->hasInterval(start, end);
}

bool CalendarWeekdays::hasInterval() const {
    //kDebug()<<k_funcinfo<<endl;
    foreach (CalendarDay *d, m_weekdays) {
        if (d->hasInterval())
            return true;
    }
    return false;
}

CalendarDay *CalendarWeekdays::weekday(int day) const {
    int i=0;
    foreach (CalendarDay *d, m_weekdays) {
        if (i++ == day)
            return d;
    }
    return 0;
}

Duration CalendarWeekdays::duration() const {
    Duration dur;
    foreach (CalendarDay *d, m_weekdays) {
        dur += d->duration();
    }
    return dur;
}

Duration CalendarWeekdays::duration(int _weekday) const {
    CalendarDay *day = weekday(_weekday);
    if (day)
        return day->duration();
    return Duration();
}

QTime CalendarWeekdays::startOfDay(int _weekday) const {
    CalendarDay *day = weekday(_weekday);
    if (day)
        return day->startOfDay();
    return QTime();
}

QTime CalendarWeekdays::endOfDay(int _weekday) const {
    CalendarDay *day = weekday(_weekday);
    if (day)
        return day->endOfDay();
    return QTime();
}
    

/////   Calendar   ////

Calendar::Calendar()
    : m_parent(0),
      m_project(0),
      m_deleted(false) {

    init();
}

Calendar::Calendar(QString name, Calendar *parent)
    : m_name(name),
      m_parent(parent),
      m_project(0),
      m_deleted(false),
      m_days() {
    
    init();
}

Calendar::~Calendar() {
    //kDebug()<<k_funcinfo<<"deleting "<<m_name<<endl;
    removeId();
    delete m_weekdays;
    while (!m_days.isEmpty())
        delete m_days.takeFirst();
}
Calendar::Calendar(Calendar *calendar)
    : m_project(0),
      m_days() {
    copy(*calendar);
}

const Calendar &Calendar::copy(Calendar &calendar) {
    m_name = calendar.name();
    m_parent = calendar.parent();
    m_deleted = calendar.isDeleted();
    m_id = calendar.id();
    
    foreach (CalendarDay *d, calendar.days()) {
        m_days.append(new CalendarDay(d));
    }
    m_weekdays = new CalendarWeekdays(calendar.weekdays());
    return *this;
}

void Calendar::init() {
    m_weekdays = new CalendarWeekdays();
}

void Calendar::setProject(Project *project) { 
    m_project = project;
    generateId();
}

void Calendar::setDeleted(bool yes) {
    if (yes) {
        removeId();
    } else {
        setId(m_id);
    }
    m_deleted = yes;
}
bool Calendar::setId(QString id) {
    //kDebug()<<k_funcinfo<<id<<endl;
    if (id.isEmpty()) {
        kError()<<k_funcinfo<<"id is empty"<<endl;
        m_id = id;
        return false;
    }
    Calendar *c = findCalendar();
    if (c == this) {
        //kDebug()<<k_funcinfo<<"My id found, remove it"<<endl;
        removeId();
    } else if (c) {
        //can happen when making a copy
        kError()<<k_funcinfo<<"My id '"<<m_id<<"' already used for different node: "<<c->name()<<endl;
    }
    if (findCalendar(id)) {
        kError()<<k_funcinfo<<"id '"<<id<<"' is already used for different node: "<<findCalendar(id)->name()<<endl;
        m_id = QString(); // hmmm
        return false;
    }
    m_id = id;
    insertId(id);
    //kDebug()<<k_funcinfo<<m_name<<": inserted id="<<id<<endl;
    return true;
}

void Calendar::generateId() {
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

bool Calendar::load(QDomElement &element) {
    //kDebug()<<k_funcinfo<<element.text()<<endl;
    //bool ok;
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
            if (e.tagName() == "day") {
                CalendarDay *day = new CalendarDay();
                if (day->load(e)) {
                    if (!day->date().isValid()) {
                        delete day;
                        kError()<<k_funcinfo<<m_name<<": Failed to load calendarDay - Invalid date"<<endl;
                    } else {
                        CalendarDay *d = findDay(day->date());
                        if (d) {
                            // already exists, keep the new
                            deleteDay(d);
                            kWarning()<<k_funcinfo<<m_name<<" Load calendarDay - Date already exists"<<endl;
                        }
                        addDay(day);
                    }
                } else {
                    delete day;
                    kError()<<k_funcinfo<<"Failed to load calendarDay"<<endl;
                    return true; //false; don't throw away the whole calendar
                }
            }
        }
    }
    return true;
}

void Calendar::save(QDomElement &element) const {
    //kDebug()<<k_funcinfo<<m_name<<endl;
    if (m_deleted)
        return;
    
    QDomElement me = element.ownerDocument().createElement("calendar");
    element.appendChild(me);
    if (m_parent && !m_parent->isDeleted()) 
        me.setAttribute("parent", m_parent->id());
    me.setAttribute("name", m_name);
    me.setAttribute("id", m_id);
    m_weekdays->save(me);
    foreach (CalendarDay *d, m_days) {
        QDomElement e = me.ownerDocument().createElement("day");
        me.appendChild(e);
        d->save(e);
    }
    
}

CalendarDay *Calendar::findDay(const QDate &date, bool skipNone) const {
    //kDebug()<<k_funcinfo<<date.toString()<<endl;
    foreach (CalendarDay *d, m_days) {
        if (d->date() == date) {
            if (skipNone  && d->state() == Map::None) {
                continue; // hmmm, break?
            }
            return d;
        }
    }
    //kDebug()<<k_funcinfo<<date.toString()<<" not found"<<endl;
    return 0;
}

bool Calendar::hasParent(Calendar *cal) {
    //kDebug()<<k_funcinfo<<endl;
    if (!m_parent)
        return false;
    if (m_parent == cal)
        return true;
    return m_parent->hasParent(cal);
}

Duration Calendar::effort(const QDate &date, const QTime &start, const QTime &end) const {
    //kDebug()<<k_funcinfo<<m_name<<": "<<date.toString(Qt::ISODate)<<" "<<start.toString()<<" - "<<end.toString()<<endl;
    if (start == end) {
        return Duration::zeroDuration;
    }
    QTime _start = start;
    QTime _end = end;
    if (start > end) {
        _start = end;
        _end = start;
    }
    // first, check my own day
    CalendarDay *day = findDay(date, true);
    if (day) {
        if (day->state() == Map::Working) {
            return day->effort(_start, _end);
        } else if (day->state() == Map::NonWorking) {
            return Duration::zeroDuration;
        } else {
            kError()<<k_funcinfo<<"Invalid state: "<<day->state()<<endl;
            return Duration::zeroDuration;
        }
    }
    // check my own weekdays
    if (m_weekdays) {
        if (m_weekdays->state(date) == Map::Working) {
            return m_weekdays->effort(date, _start, _end);
        }
        if (m_weekdays->state(date) == Map::NonWorking) {
            return Duration::zeroDuration;
        }
    }
    if (m_parent && !m_parent->isDeleted()) {
        return m_parent->effort(date, start, end);
    }
    // Check default calendar
    return project()->defaultCalendar()->effort(date, start, end);
}

Duration Calendar::effort(const DateTime &start, const DateTime &end) const {
    //kDebug()<<k_funcinfo<<m_name<<": "<<start<<" to "<<end<<endl;
    Duration eff;
    if (!start.isValid() || !end.isValid() || end <= start) {
        return eff;
    }
    QDate date = start.date();
    QTime startTime = start.time();
    QTime endTime = end.time();
    if (end.date() > date) {
        endTime.setHMS(23, 59, 59, 999);
    }
    eff = effort(date, startTime, endTime); // first day
    // Now get all the rest of the days
    for (date = date.addDays(1); date <= end.date(); date = date.addDays(1)) {
        if (date < end.date())
             eff += effort(date, QTime(), endTime); // whole days
        else 
             eff += effort(date, QTime(), end.time()); // last day
        //kDebug()<<k_funcinfo<<": eff now="<<eff.toString(Duration::Format_Day)<<endl;
    }
    //kDebug()<<k_funcinfo<<start.date().toString()<<"- "<<end.date().toString()<<": total="<<eff.toString(Duration::Format_Day)<<endl;
    return eff;
}


TimeInterval Calendar::firstInterval(const QDate &date, const QTime &startTime, const QTime &endTime) const {
    CalendarDay *day = findDay(date, true);
    if (day) {
        return day->interval(startTime, endTime);
    }
    if (m_weekdays) {
        if (m_weekdays->state(date) == Map::Working) {
            return m_weekdays->interval(date, startTime, endTime);
        }
        if (m_weekdays->state(date) == Map::NonWorking) {
            return TimeInterval(QTime(), QTime());
        }
    }
    if (m_parent && !m_parent->isDeleted()) {
        return m_parent->firstInterval(date, startTime, endTime);
    }
    return project()->defaultCalendar()->firstInterval(date, startTime, endTime);
}

DateTimeInterval Calendar::firstInterval(const DateTime &start, const DateTime &end) const {
    //kDebug()<<k_funcinfo<<start.toString()<<" - "<<end.toString()<<endl;
    if (!start.isValid()) {
        kWarning()<<k_funcinfo<<"Invalid start time"<<endl;
        return DateTimeInterval(DateTime(), DateTime());
    }
    if (!end.isValid()) {
        kWarning()<<k_funcinfo<<"Invalid end time"<<endl;
        return DateTimeInterval(DateTime(), DateTime());
    }
    QTime startTime;
    QTime endTime;
    QDate date = start.date();
    int i=0;
    for (; date <= end.date() && i++ < 10; date = date.addDays(1)) {
        if (date < end.date())
            endTime = QTime(23, 59, 59, 999);
        else
            endTime = end.time();
        if (date > start.date())
            startTime = QTime();
        else 
            startTime = start.time();
            
        TimeInterval res = firstInterval(date, startTime, endTime);
        if (res.first < res.second) {
            return DateTimeInterval(DateTime(date,res.first),DateTime(date, res.second));
        }
    }
    //kError()<<k_funcinfo<<"Didn't find an interval ("<<start<<", "<<end<<")"<<endl;
    return DateTimeInterval(DateTime(), DateTime());
}


bool Calendar::hasInterval(const QDate &date, const QTime &startTime, const QTime &endTime) const {
    CalendarDay *day = findDay(date, true);
    if (day) {
        //kDebug()<<k_funcinfo<<m_name<<" "<<date<<": "<<startTime<<" to "<<endTime<<endl;
        return day->hasInterval(startTime, endTime);
    } 
    if (m_weekdays) {
        if (m_weekdays->state(date) == Map::Working) {
            return m_weekdays->hasInterval(date, startTime, endTime);
        } else if (m_weekdays->state(date) == Map::NonWorking) {
            return false;
        }
    }
    if (m_parent && !m_parent->isDeleted()) {
        return m_parent->hasInterval(date, startTime, endTime);
    }
    return project()->defaultCalendar()->hasInterval(date, startTime, endTime);
}

bool Calendar::hasInterval(const DateTime &start, const DateTime &end) const {
    //kDebug()<<k_funcinfo<<m_name<<": "<<start<<" - "<<end<<endl;
    if (!start.isValid() || !end.isValid() || end <= start) {
        //kError()<<k_funcinfo<<"Invalid input: "<<(start.isValid()?"":"(start invalid) ")<<(end.isValid()?"":"(end invalid) ")<<(start>end?"":"(start<=end)")<<endl;
        //kDebug()<<kBacktrace(8)<<endl;
        return false;
    }
    QTime startTime;
    QTime endTime;
    QDate date = start.date();
    for (; date <= end.date(); date = date.addDays(1)) {
        if (date < end.date())
            endTime = QTime(23, 59, 59, 999);
        else
            endTime = end.time();
        if (date > start.date())
            startTime = QTime();
        else 
            startTime = start.time();

        if (hasInterval(date, startTime, endTime))
            return true;
    }
    return false;
}

DateTime Calendar::firstAvailableAfter(const DateTime &time, const DateTime &limit) {
    //kDebug()<<k_funcinfo<<m_name<<": check from "<<time.toString()<<" limit="<<limit.toString()<<endl;
    if (!time.isValid() || !limit.isValid() || time >= limit) {
        kError()<<k_funcinfo<<"Invalid input: "<<(time.isValid()?"":"(time invalid) ")<<(limit.isValid()?"":"(limit invalid) ")<<(time>limit?"":"(time>=limit)")<<endl;
        return DateTime();
    }
    if (!hasInterval(time, limit)) {
        return DateTime();
    }
    DateTime t = firstInterval(time, limit).first;
    //kDebug()<<k_funcinfo<<m_name<<": "<<t.toString()<<endl;
    return t;
}

DateTime Calendar::firstAvailableBefore(const DateTime &time, const DateTime &limit) {
    //kDebug()<<k_funcinfo<<m_name<<": check from "<<time.toString()<<" limit="<<limit.toString()<<endl;
    if (!time.isValid() || !limit.isValid() || time <= limit) {
        kError()<<k_funcinfo<<"Invalid input: "<<(time.isValid()?"":"(time invalid) ")<<(limit.isValid()?"":"(limit invalid) ")<<(time>limit?"":"(time<=limit)")<<endl;
        return DateTime();
    }
    DateTime lmt = time;
    DateTime t = DateTime(time.date()); // start of first day
    if (t == lmt)
        t = t.addDays(-1); // in case time == start of day
    if (t < limit)
        t = limit;  // always stop at limit (lower boundary)
    DateTime res;
    //kDebug()<<k_funcinfo<<m_name<<": t="<<t<<", "<<lmt<<" limit="<<limit<<endl;
    while (!res.isValid() && t >= limit) {
        // check intervals for 1 day
        DateTime r = firstInterval(t, lmt).second;
        res = r;
        // Find the last interval
        while(r.isValid() && r < lmt) {
            r = firstInterval(r, lmt).second;
            if (r.isValid())
                res = r;
            //kDebug()<<k_funcinfo<<m_name<<": r="<<r<<", "<<lmt<<" res="<<res<<endl;
        }
        if (!res.isValid()) {
            if (t == limit) {
                break;
            }
            lmt = t;
            t = t.addDays(-1);
            if (t < limit) {
                t = limit;
            }
            if (t == lmt)
                break;
        }
    }
    //kDebug()<<k_funcinfo<<m_name<<": "<<res<<endl;
    return res;
}

Calendar *Calendar::findCalendar(const QString &id) const { 
    return (m_project ? m_project->findCalendar(id) : 0); 
}

bool Calendar::removeId(const QString &id) { 
    return (m_project ? m_project->removeCalendarId(id) : false); 
}

void Calendar::insertId(const QString &id){ 
    if (m_project)
        m_project->insertCalendarId(id, this); 
}

/////////////
StandardWorktime::StandardWorktime() {
    init();
}

StandardWorktime::StandardWorktime(StandardWorktime *worktime) {
    if (worktime) {
        m_year = worktime->durationYear();
        m_month = worktime->durationMonth();
        m_week = worktime->durationWeek();
        m_day = worktime->durationDay();
        m_calendar = new Calendar(*(worktime->calendar()));
    } else {
        init();
    }
}

StandardWorktime::~StandardWorktime() {
    //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
}

void StandardWorktime::init() {
    // Some sane default values
    m_year = Duration(0, 1760, 0);
    m_month = Duration(0, 176, 0);
    m_week = Duration(0, 40, 0);
    m_day = Duration(0, 8, 0);
    m_calendar = new Calendar;
    m_calendar->setName(i18n("Base"));
    TimeInterval t = TimeInterval(QTime(8,0,0), QTime(16,0,0));
    for (int i=0; i < 5; ++i) {
        m_calendar->weekday(i)->addInterval(t);
        m_calendar->weekday(i)->setState(Map::Working);
    }
    m_calendar->weekday(5)->setState(Map::NonWorking);
    m_calendar->weekday(6)->setState(Map::NonWorking);
}

bool StandardWorktime::load(QDomElement &element) {
    //kDebug()<<k_funcinfo<<endl;
    m_year = Duration::fromString(element.attribute("year"), Duration::Format_Hour); 
    m_month = Duration::fromString(element.attribute("month"), Duration::Format_Hour); 
    m_week = Duration::fromString(element.attribute("week"), Duration::Format_Hour); 
    m_day = Duration::fromString(element.attribute("day"), Duration::Format_Hour); 
    
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "calendar") {
                delete m_calendar;
                m_calendar = new Calendar;
                m_calendar->load(e);
            }
        }
    }
    return true;
}

void StandardWorktime::save(QDomElement &element) const {
    //kDebug()<<k_funcinfo<<endl;
    QDomElement me = element.ownerDocument().createElement("standard-worktime");
    element.appendChild(me);
    me.setAttribute("year", m_year.toString(Duration::Format_Hour));
    me.setAttribute("month", m_month.toString(Duration::Format_Hour));
    me.setAttribute("week", m_week.toString(Duration::Format_Hour));
    me.setAttribute("day", m_day.toString(Duration::Format_Hour));
    
    m_calendar->save(me);
}

#ifndef NDEBUG
void CalendarDay::printDebug(QString indent) {
    QString s[] = {"None", "Non-working", "Working"};
    kDebug()<<indent<<" "<<m_date.toString()<<" = "<<s[m_state]<<endl;
    if (m_state == Map::Working) {
        indent += "  ";
        foreach (TimeInterval *i, m_workingIntervals) {
            kDebug()<<indent<<" Interval: "<<i->first<<" to "<<i->second<<endl;
        }
    }
    
}
void CalendarWeekdays::printDebug(QString indent) {
    kDebug()<<indent<<"Weekdays ------"<<endl;
    int c=1;
    foreach (CalendarDay *d, m_weekdays) {
        d->printDebug(indent + "  Day " + c++ + ": ");
    }

}
void Calendar::printDebug(QString indent) {
    kDebug()<<indent<<"Calendar "<<m_id<<": '"<<m_name<<"' Deleted="<<m_deleted<<endl;
    kDebug()<<indent<<"  Parent: "<<(m_parent ? m_parent->name() : "No parent")<<endl;
    m_weekdays->printDebug(indent + "  ");
    kDebug()<<indent<<"  Days --------"<<endl;
    foreach (CalendarDay *d, m_days) {
        d->printDebug(indent + "  ");
    }
}

void StandardWorktime::printDebug(QString indent) {
    kDebug()<<indent<<"StandardWorktime "<<endl;
    kDebug()<<indent<<"Year: "<<m_year.toString()<<endl;
    kDebug()<<indent<<"Month: "<<m_month.toString()<<endl;
    kDebug()<<indent<<"Week: "<<m_week.toString()<<endl;
    kDebug()<<indent<<"Day: "<<m_day.toString()<<endl;
}

#endif

}  //KPlato namespace
