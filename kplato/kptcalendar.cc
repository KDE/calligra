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

#include "defs.h"
#include "kptcalendar.h"

#include <qdom.h>
#include <qptrlist.h>

#include <kdebug.h>

/////   KPTCalendarDay   ////
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

const KPTCalendarDay &KPTCalendarDay::copy(const KPTCalendarDay &day) {
    //kdDebug()<<k_funcinfo<<endl;
    m_date = day.date();
    m_state = day.state();
    QPtrListIterator<QPair<QTime, QTime> > it = day.workingIntervals();
    for(; it.current(); ++it) {
        m_workingIntervals.append(new QPair<QTime, QTime>(it.current()->first, it.current()->second));
        //kdDebug()<<k_funcinfo<<"Date "<<m_date<<": "<<it.current()->first<<","<< it.current()->second<<endl;
    }
    return *this;
}

bool KPTCalendarDay::load(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    bool ok;
    if (element.hasAttribute("date"))
        m_date = QDate::fromString(element.attribute("date"));
        
    m_state = QString(element.attribute("state","-1")).toInt(&ok);
    //kdDebug()<<k_funcinfo<<"ok="<<ok<<" state="<<m_state<<endl;
    
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
    if (m_date.isValid()) {
        element.setAttribute("date", m_date.toString());
    }
    element.setAttribute("state", m_state);
    if (m_workingIntervals.count() == 0)
        return;
    
    QDomElement me = element.ownerDocument().createElement("interval");
    element.appendChild(me);
    QPtrListIterator<QPair<QTime, QTime> > it = m_workingIntervals;
    for (; it.current(); ++it) {
        me.setAttribute("start", it.current()->first.toString());
        me.setAttribute("end", it.current()->second.toString());
    }
} 

void KPTCalendarDay::addInterval(QPair<QTime, QTime> *interval) {
    m_workingIntervals.append(interval);
}

bool KPTCalendarDay::operator==(const KPTCalendarDay *day) const {
    return m_state == day->state() && m_workingIntervals == day->workingIntervals();
}
bool KPTCalendarDay::operator!=(const KPTCalendarDay *day) const {
    return m_state != day->state() || m_workingIntervals != day->workingIntervals();
}

/////   KPTCalendarWeekdays   ////
KPTCalendarWeekdays::KPTCalendarWeekdays()
    : m_workHours(40) {

    for (int i=0; i < 7; ++i) {
        m_weekdays.append(new KPTCalendarDay());
    }
}

KPTCalendarWeekdays::KPTCalendarWeekdays(KPTCalendarWeekdays *weekdays) {
    copy(*weekdays);
}

const KPTCalendarWeekdays &KPTCalendarWeekdays::copy(const KPTCalendarWeekdays &weekdays) {
    //kdDebug()<<k_funcinfo<<endl;
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
    day->load(element);
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


/////   KPTCalendarWeek  ////
KPTCalendarWeeks::KPTCalendarWeeks() {
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


/////   KPTCalendar   ////
KPTCalendar::KPTCalendar()
    : m_parent(0),
      m_deleted(false),
      m_id(-1) {

    init();
}

KPTCalendar::KPTCalendar(QString name, KPTCalendar *parent)
    : m_name(name),
      m_parent(parent),
      m_deleted(false),
      m_id(-1) {
    
    init();
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
    m_weeks = new KPTCalendarWeeks();
    m_weekdays = new KPTCalendarWeekdays();
}

bool KPTCalendar::load(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<element.text()<<endl;
    bool ok;
    m_id = QString(element.attribute("id","-1")).toInt(&ok);
    m_parentId = QString(element.attribute("parent","-1")).toInt(&ok);
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
                if (day->load(e))
                    addDay(day);
                else {
                    delete day;
                    kdError()<<k_funcinfo<<"Failed to load calendarDay"<<endl;
                    return false;
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

KPTCalendarDay *KPTCalendar::findDay(const QDate &date) {
    //kdDebug()<<k_funcinfo<<endl;
    KPTCalendarDay *day;
    for (day = m_days.first(); day != 0; day = m_days.next()) {
        if (day->date() == date)
            return day;
    }
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

#ifndef NDEBUG
void KPTCalendarDay::printDebug(QCString indent) {
    QString s[] = {"None", "Non-working", "Working"};
    kdDebug()<<indent<<" "<<m_date<<" = "<<s[m_state]<<endl;
    if (m_state == KPTMap::Working) {
        indent += "  ";
        QPtrListIterator<QPair<QTime, QTime> > it = m_workingIntervals;
        for (; it.current(); ++it) {
            //kdDebug()<<indent<<" Interval: "<<it.current()->first<<" to "<<it.current()->second<<endl;
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
    if (m_parent) kdDebug()<<indent<<"  Parent: "<<m_parent->name()<<endl;
    m_weekdays->printDebug(indent + "  ");
    m_weeks->printDebug(indent + "  ");
    kdDebug()<<indent<<"  Days --------"<<endl;
    QPtrListIterator<KPTCalendarDay> it = m_days;
    for (; it.current(); ++it) {
        it.current()->printDebug(indent + "  ");
    }
}
#endif
