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

#include "kptmap.h"
#include "kptcalendar.h"
#include "kptduration.h"
#include "kptdatetime.h"
#include "kptproject.h"
#include "kptschedule.h"
#include "kptxmlloaderobject.h"

#include <KoXmlReader.h>

#include <qdom.h>
#include <QList>

#include <kdatetime.h>
#include <klocale.h>
#include <ktimezones.h>
#include <ksystemtimezone.h>
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

CalendarDay::CalendarDay(const QDate& date, int state)
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

bool CalendarDay::load( KoXmlElement &element, XMLLoaderObject &status ) {
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
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
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
    return true;
}

void CalendarDay::save(QDomElement &element) const {
    //kDebug()<<k_funcinfo<<m_date.toString()<<endl;
    if (m_state == None)
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

Duration CalendarDay::effort(const QTime &start, const QTime &end, const KDateTime::Spec &spec, Schedule *sch) {
    //kDebug()<<k_funcinfo<<start<<" - "<<end<<endl;
    return effort( m_date, start, end, spec, sch );
}

Duration CalendarDay::effort(const QDate &date, const QTime &start, const QTime &end, const KDateTime::Spec &spec, Schedule *sch) {
    if ( !date.isValid() ) {
        return Duration::zeroDuration;
    }
    //kDebug()<<k_funcinfo<<start.toString()<<" - "<<end.toString()<<endl;
    Duration eff;
    if (m_state != Working) {
        //kDebug()<<k_funcinfo<<"Non working day"<<endl;
        return eff;
    }
    foreach (TimeInterval *i, m_workingIntervals) {
        QTime t1 = start < i->first ? i->first : start;
        QTime t2 = end > i->second ? i->second : end;
        //kDebug()<<k_funcinfo<<"Interval: "<<t1<<" - "<<t2<<endl;
        DateTimeInterval dti( DateTime( date, t1, spec ), DateTime( date, t2, spec ) );
        if ( sch ) {
            dti = sch->available( dti );
            //kDebug()<<k_funcinfo<<"Checked sch: "<<dti.first<<" - "<<dti.second<<endl;
        }
        eff += dti.second - dti.first;
        //kDebug()<<k_funcinfo<<dti.first.toString()<<" - "<<dti.second.toString()<<", effort now "<<eff.toString()<<endl;
    }
    //kDebug()<<k_funcinfo<<(m_date.isValid()?m_date.toString(Qt::ISODate):"Weekday")<<": "<<start.toString()<<" - "<<end.toString()<<": total="<<eff.toString(Duration::Format_Day)<<endl;
    return eff;
}

Duration CalendarDay::workDuration() const
{
    Duration d;
    if (m_state != Working) {
        //kDebug()<<k_funcinfo<<"Non working day"<<endl;
        return d;
    }
    foreach (TimeInterval *i, m_workingIntervals) {
        //kDebug()<<k_funcinfo<<"Interval: "<<i->first<<" - "<<i->second<<endl;
        d += Duration(i->first.msecsTo( i->second ) );
    }
    return d;
}


TimeInterval CalendarDay::interval(const QTime &start, const QTime &end, const KDateTime::Spec &spec, Schedule *sch) const {
    //kDebug()<<k_funcinfo<<endl;
    return interval( m_date, start, end, spec, sch );
}

TimeInterval CalendarDay::interval(const QDate date, const QTime &start, const QTime &end, const KDateTime::Spec &spec, Schedule *sch) const {
    //kDebug()<<k_funcinfo<<endl;
    QTime t1, t2;
    if ( hasInterval() ) {
        foreach (TimeInterval *i, m_workingIntervals) {
            if (start < i->second && end > i->first) {
                QTime t1 = start < i->first ? i->first : start;
                QTime t2 = end > i->second ? i->second : end;
                TimeInterval ti( t1, t2 );
                //kDebug()<<k_funcinfo<<"---->"<<sch<<" "<<date<<", "<<t1<<t2<<endl;
                if ( sch ) {
                    // check if booked
                    //kDebug()<<k_funcinfo<<"---->"<<date<<", "<<t1<<t2<<endl;
                    DateTimeInterval dti( DateTime( date, t1, spec ), DateTime( date, t2, spec ) );
                    dti = sch->available( dti );
                    //kDebug()<<k_funcinfo<<"Checked sch: "<<ti.first<<" - "<<ti.second<<endl;
                    ti = TimeInterval( dti.first.toTimeSpec( spec ).time(), dti.second.toTimeSpec( spec ).time() );
                }
                if ( !ti.first.isNull() && !ti.second.isNull() && ti.first < ti.second ) {
                    //kDebug()<<k_funcinfo<<"true:"<<" "<<ti.first<<" - "<<ti.second<<endl;
                    return ti;
                }
                return TimeInterval( QTime(), QTime() );
            }
        }
    }
    //kError()<<k_funcinfo<<"No interval "<<m_date<<": "<<start<<","<<end<<endl;
    return TimeInterval(t1, t2);
}

bool CalendarDay::hasInterval() const {
    return m_state == Working && m_workingIntervals.count() > 0;
}

bool CalendarDay::hasInterval(const QTime &start, const QTime &end, const KDateTime::Spec &spec, Schedule *sch) const {
    //kDebug()<<k_funcinfo<<(m_date.isValid()?m_date.toString(Qt::ISODate):"Weekday")<<" "<<start.toString()<<" - "<<end.toString()<<endl;
    return hasInterval( m_date, start, end, spec, sch );
}

bool CalendarDay::hasInterval(const QDate date, const QTime &start, const QTime &end, const KDateTime::Spec &spec, Schedule *sch) const {
    //kDebug()<<k_funcinfo<<(m_date.isValid()?m_date.toString(Qt::ISODate):"Weekday")<<" "<<start.toString()<<" - "<<end.toString()<<endl;
    if ( ! hasInterval() ) {
        return false;
    }
    foreach (TimeInterval *i, m_workingIntervals) {
        if (start < i->second && end > i->first) {
            if ( sch ) {
                // check if booked
                QTime t1 = start < i->first ? i->first : start;
                QTime t2 = end > i->second ? i->second : end;
                //kDebug()<<k_funcinfo<<"---->"<<date<<", "<<t1<<t2<<endl;
                DateTimeInterval dti( DateTime( date, t1, spec ), DateTime( date, t2, spec ) );
                dti = sch->available( dti );
                if ( dti.first.isValid() && dti.second.isValid() && dti.first < dti.second ) {
                    //kDebug()<<k_funcinfo<<"true:"<<" "<<dti.first<<dti.second<<endl;
                    return true;
                }
            } else {
                return true;
            }
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

void CalendarDay::removeInterval( TimeInterval *ti )
{
    int i = m_workingIntervals.indexOf( ti );
    if ( i != -1 ) {
        m_workingIntervals.removeAt( i );
    }
}

int CalendarDay::numIntervals() const 
{
    return m_state == Working ? m_workingIntervals.count() : 0;
}

int CalendarDay::indexOf( const TimeInterval *ti ) const
{
    return m_workingIntervals.indexOf( const_cast<TimeInterval*>( ti ) );
}

TimeInterval *CalendarDay::intervalAt( int index ) const
{
    return m_workingIntervals.value( index );
}

/////   CalendarWeekdays   ////
CalendarWeekdays::CalendarWeekdays()
    : m_weekdays()
{

    //kDebug()<<k_funcinfo<<"--->"<<endl;
    for (int i=1; i <= 7; ++i) {
        m_weekdays.insert( i, new CalendarDay() );
    }
    //kDebug()<<k_funcinfo<<"<---"<<endl;
}

CalendarWeekdays::CalendarWeekdays( const CalendarWeekdays *weekdays )
    : m_weekdays() {
    //kDebug()<<k_funcinfo<<"--->"<<endl;
    copy(*weekdays);
    //kDebug()<<k_funcinfo<<"<---"<<endl;
}

CalendarWeekdays::~CalendarWeekdays() {
    foreach ( CalendarDay *d, m_weekdays.values() ) {
        delete d;
    }
    //kDebug()<<k_funcinfo<<endl;
}

const CalendarWeekdays &CalendarWeekdays::copy(const CalendarWeekdays &weekdays) {
    //kDebug()<<k_funcinfo<<endl;
    foreach ( CalendarDay *d, m_weekdays.values() ) {
        delete d;
    }
    m_weekdays.clear();
    QMapIterator<int, CalendarDay*> i( weekdays.weekdayMap() );
    while ( i.hasNext() ) {
        i.next();
        m_weekdays.insert( i.key(), new CalendarDay( i.value() ) );
    }
    return *this;
}

bool CalendarWeekdays::load( KoXmlElement &element, XMLLoaderObject &status ) {
    //kDebug()<<k_funcinfo<<endl;
    bool ok;
    int dayNo = QString(element.attribute("day","-1")).toInt(&ok);
    if (dayNo < 0 || dayNo > 6) {
        kError()<<k_funcinfo<<"Illegal weekday: "<<dayNo<<endl;
        return true; // we continue anyway
    }
    CalendarDay *day = m_weekdays.value( dayNo + 1 );
    if ( day == 0 ) {
        kError()<<k_funcinfo<<"No weekday: "<<dayNo<<endl;
        return false;
    }
    if (!day->load( element, status ) )
        day->setState(CalendarDay::None);
    return true;
}

void CalendarWeekdays::save(QDomElement &element) const {
    //kDebug()<<k_funcinfo<<endl;
    QMapIterator<int, CalendarDay*> i( m_weekdays );
    while ( i.hasNext() ) {
        i.next();
        QDomElement me = element.ownerDocument().createElement("weekday");
        element.appendChild(me);
        me.setAttribute( "day", i.key() - 1 ); // 0 (monday) .. 6 (sunday)
        i.value()->save(me);
    }
}

const QMap<int, CalendarDay*> &CalendarWeekdays::weekdayMap() const
{
    return m_weekdays;
}

IntMap CalendarWeekdays::stateMap() const
{
    IntMap days;
    QMapIterator<int, CalendarDay*> i( m_weekdays );
    while ( i.hasNext() ) {
        i.next();
        if ( i.value()->state() != CalendarDay::None )
            days.insert( i.key(), i.value()->state() );
    }
    return days;
}

int CalendarWeekdays::state(const QDate &date) const {
    return state( date.dayOfWeek() );
}

int CalendarWeekdays::state( int weekday ) const {
    CalendarDay *day = m_weekdays.value( weekday );
    return day ? day->state() : CalendarDay::None;
}

void CalendarWeekdays::setState(int weekday, int state) {
    CalendarDay *day = m_weekdays.value( weekday );
    if ( day == 0 )
        return;
    day->setState(state);
}

const QList<TimeInterval*> &CalendarWeekdays::intervals(int weekday) const { 
    CalendarDay *day = m_weekdays.value( weekday );
    Q_ASSERT(day);
    return day->workingIntervals();
}

void CalendarWeekdays::setIntervals(int weekday, QList<TimeInterval*>intervals) {
    CalendarDay *day = m_weekdays.value( weekday );
    if (day) {
        day->setIntervals( intervals );

    }
}

void CalendarWeekdays::clearIntervals(int weekday) {
    CalendarDay *day = m_weekdays.value( weekday );
    if (day) {
        day->clearIntervals();
    }
}

bool CalendarWeekdays::operator==(const CalendarWeekdays *wd) const {
    if (m_weekdays.count() != wd->weekdays().count()) {
        return false;
    }
    QMapIterator<int, CalendarDay*> i( wd->weekdayMap() );
    while ( i.hasNext() ) {
        i.next();
        CalendarDay *day1 = i.value();
        CalendarDay *day2 = m_weekdays.value( i.key() );
        if (day1 != day2)
            return false;
    }
    return true;
}
bool CalendarWeekdays::operator!=(const CalendarWeekdays *wd) const {
    return operator==( wd ) == false;
}

Duration CalendarWeekdays::effort(const QDate &date, const QTime &start, const QTime &end, const KDateTime::Spec &spec, Schedule *sch) {
    //kDebug()<<k_funcinfo<<"Day of week="<<date.dayOfWeek()<<endl;
    CalendarDay *day = weekday( date.dayOfWeek() );
    if (day && day->state() == CalendarDay::Working) {
        return day->effort(date, start, end, spec, sch);
    }
    return Duration::zeroDuration;
}

TimeInterval CalendarWeekdays::interval(const QDate date, const QTime &start, const QTime &end, const KDateTime::Spec &spec, Schedule *sch) const {
    //kDebug()<<k_funcinfo<<endl;
    CalendarDay *day = weekday( date.dayOfWeek() );
    if (day && day->state() == CalendarDay::Working) {
        if (day->hasInterval(date, start, end, spec, sch)) {
            return day->interval(date, start, end, spec, sch);
        }
    }
    return TimeInterval(QTime(), QTime());
}

bool CalendarWeekdays::hasInterval(const QDate date, const QTime &start, const QTime &end, const KDateTime::Spec &spec, Schedule *sch) const {
    //kDebug()<<k_funcinfo<<date.toString()<<": "<<start.toString()<<" - "<<end.toString()<<endl;
    CalendarDay *day = weekday( date.dayOfWeek() );
    return day && day->hasInterval(date, start, end, spec, sch);
}

bool CalendarWeekdays::hasInterval() const {
    //kDebug()<<k_funcinfo<<endl;
    foreach ( CalendarDay *d, m_weekdays.values() ) {
        if (d->hasInterval())
            return true;
    }
    return false;
}

CalendarDay *CalendarWeekdays::weekday( int day ) const {
    return m_weekdays.value( day );
}

Duration CalendarWeekdays::duration() const {
    Duration dur;
    foreach ( CalendarDay *d, m_weekdays.values() ) {
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

int CalendarWeekdays::indexOf( const CalendarDay *day ) const
{
    return m_weekdays.values().indexOf( const_cast<CalendarDay*>(day) );
}

/////   Calendar   ////

Calendar::Calendar()
    : QObject( 0 ), // don't use parent
      m_parent(0),
      m_project(0),
      m_default( false )
{
    init();
}

Calendar::Calendar(const QString& name, Calendar *parent)
    : QObject( 0 ), // don't use parent
      m_name(name),
      m_parent(parent),
      m_project(0),
      m_days(),
      m_default( false )
{
    init();
}

Calendar::~Calendar() {
    //kDebug()<<k_funcinfo<<"deleting "<<m_name<<endl;
    removeId();
    delete m_weekdays;
    while (!m_days.isEmpty())
        delete m_days.takeFirst();
}
// Not allowed, QObject
// Calendar::Calendar(Calendar *calendar)
//     : m_project(0),
//       m_days() {
//     copy(*calendar);
// }

const Calendar &Calendar::copy( const Calendar &calendar ) {
    m_name = calendar.name();
    m_spec = calendar.timeSpec();
    // m_parent = calendar.parentCal(); 
    // m_id = calendar.id();
    
    foreach (CalendarDay *d, calendar.days()) {
        m_days.append(new CalendarDay(d));
    }
    delete m_weekdays;
    m_weekdays = new CalendarWeekdays(calendar.weekdays());
    return *this;
}

void Calendar::init() {
    m_weekdays = new CalendarWeekdays();
    m_spec = KDateTime::Spec( KSystemTimeZones::local() );
}

void Calendar::setName(const QString& name)
{
    m_name = name;
    if ( m_project ) {
        m_project->changed( this );
    }
}

void Calendar::setParentCal(Calendar *parent)
{
    if ( m_parent ) {
        m_parent->takeCalendar( this );
    }
    m_parent = parent;
    if ( m_parent ) {
        m_parent->addCalendar( this );
    }

}

bool Calendar::isChildOf( const Calendar *cal ) const
{
    Calendar *p = parentCal();
    for (; p != 0; p = p->parentCal() ) {
        if ( cal == p ) {
            return true;
        }
    }
    return false;
}

void Calendar::setProject(Project *project) { 
    m_project = project;
}

void Calendar::setTimeZone( const KTimeZone *tz )
{
    //kDebug()<<k_funcinfo<<tz->name()<<endl;
    m_spec = KDateTime::Spec( tz );
    if ( m_project ) {
        m_project->changed( this );
    }
}

// Note: only project should do this
void Calendar::setId(const QString& id) {
    //kDebug()<<k_funcinfo<<id<<endl;
    m_id = id;
}

void Calendar::addCalendar( Calendar *calendar )
{
    m_calendars.append( calendar );
    calendar->setTimeZone( m_spec.timeZone() );
}

void Calendar::takeCalendar( Calendar *calendar )
{
    int i = indexOf( calendar );
    if ( i != -1 ) {
        m_calendars.removeAt( i );
    }
}

int Calendar::indexOf( const Calendar *calendar ) const
{
    return m_calendars.indexOf( const_cast<Calendar*>(calendar) );
}

bool Calendar::load( KoXmlElement &element, XMLLoaderObject &status ) {
    //kDebug()<<k_funcinfo<<element.text()<<endl;
    //bool ok;
    setId(element.attribute("id"));
    m_parentId = element.attribute("parent");
    m_name = element.attribute("name","");
    const KTimeZone *tz = KSystemTimeZones::zone( element.attribute( "timezone" ) );
    if ( tz ) {
        setTimeZone( tz );
    } else kWarning()<<k_funcinfo<<"No timezone specified, use default (local)"<<endl;
    bool m_default = (bool)element.attribute("default","0").toInt();
    if ( m_default ) {
        status.project().setDefaultCalendar( this );
    }
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if (e.tagName() == "weekday") {
            if ( !m_weekdays->load( e, status ) )
                return false;
        }
        if (e.tagName() == "day") {
            CalendarDay *day = new CalendarDay();
            if ( day->load( e, status ) ) {
                if (!day->date().isValid()) {
                    delete day;
                    kError()<<k_funcinfo<<m_name<<": Failed to load calendarDay - Invalid date"<<endl;
                } else {
                    CalendarDay *d = findDay(day->date());
                    if (d) {
                        // already exists, keep the new
                        delete takeDay(d);
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
    return true;
}

void Calendar::save(QDomElement &element) const {
    //kDebug()<<k_funcinfo<<m_name<<endl;
    QDomElement me = element.ownerDocument().createElement("calendar");
    element.appendChild(me);
    if (m_parent) {
        me.setAttribute("parent", m_parent->id());
    }
    me.setAttribute("name", m_name);
    me.setAttribute("id", m_id);
    if ( m_default ) {
        me.setAttribute("default", m_default);
    }
    me.setAttribute("timezone", m_spec.timeZone()->name() );
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
            if (skipNone  && d->state() == CalendarDay::None) {
                continue; // hmmm, break?
            }
            return d;
        }
    }
    //kDebug()<<k_funcinfo<<date.toString()<<" not found"<<endl;
    return 0;
}

void Calendar::setState( CalendarDay *day, CalendarDay::State state )
{
    day->setState( state );
    emit changed( day );
}

void Calendar::addWorkInterval( CalendarDay *day, TimeInterval *ti )
{
    workIntervalToBeAdded( day, ti, day->numIntervals() );
    day->addInterval( ti );
    workIntervalAdded( day, ti );
}

void Calendar::takeWorkInterval( CalendarDay *day, TimeInterval *ti )
{
    int ix = day->indexOf( ti );
    if ( ix == -1 ) {
        return;
    }
    workIntervalToBeRemoved( day, ti );
    day->removeInterval( ti );
    workIntervalRemoved( day, ti );
    return;
}

void Calendar::setWorkInterval( TimeInterval *ti, const TimeInterval &value )
{
    ti->first = value.first;
    ti->second = value.second;
    emit changed( ti );
}

void Calendar::setDate( CalendarDay *day, const QDate &date )
{
    day->setDate( date );
    emit changed( day );
}

CalendarDay *Calendar::day( const QDate &date ) const
{
    foreach ( CalendarDay *d, m_days ) {
        if ( d->date() == date ) {
            return d;
        }
    }
    return 0;
}

IntMap Calendar::weekdayStateMap() const
{
    return m_weekdays->stateMap();
}

void Calendar::setWeekday( int dayno, const CalendarDay &day )
{
    if ( dayno < 1 || dayno > 7 ) {
        return;
    }
    CalendarDay *wd = weekday( dayno );
    while ( ! wd->workingIntervals().isEmpty() ) {
        TimeInterval *ti = wd->workingIntervals().last();
        emit workIntervalToBeRemoved( wd, ti );
        wd->removeInterval( ti );
        emit workIntervalRemoved( wd, ti );
    }
    wd->setState( day.state() );
    emit changed( wd );
    foreach ( TimeInterval *ti, day.workingIntervals() ) {
        TimeInterval *t = new TimeInterval( ti->first, ti->second );
        emit workIntervalToBeAdded( wd, t, wd->numIntervals() ); // hmmmm
        wd->addInterval( t );
        emit workIntervalAdded( wd, t );
    }
}

bool Calendar::hasParent(Calendar *cal) {
    //kDebug()<<k_funcinfo<<endl;
    if (!m_parent)
        return false;
    if (m_parent == cal)
        return true;
    return m_parent->hasParent(cal);
}

Duration Calendar::effort(const QDate &date, const QTime &start, const QTime &end, Schedule *sch) const {
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
        if (day->state() == CalendarDay::Working) {
            return day->effort(_start, _end, m_spec, sch);
        } else if (day->state() == CalendarDay::NonWorking) {
            return Duration::zeroDuration;
        } else {
            kError()<<k_funcinfo<<"Invalid state: "<<day->state()<<endl;
            return Duration::zeroDuration;
        }
    }
    // check my own weekdays
    if (m_weekdays) {
        if (m_weekdays->state(date) == CalendarDay::Working) {
            return m_weekdays->effort(date, _start, _end, m_spec, sch);
        }
        if (m_weekdays->state(date) == CalendarDay::NonWorking) {
            return Duration::zeroDuration;
        }
    }
    if (m_parent) {
        return m_parent->effort(date, start, end, sch);
    }
    return Duration::zeroDuration;
}

Duration Calendar::effort(const DateTime &start, const DateTime &end, Schedule *sch) const {
    //kDebug()<<k_funcinfo<<m_name<<": "<<start<<" to "<<end<<endl;
    Duration eff;
    if (!start.isValid() || !end.isValid() || end <= start) {
        kError()<<k_funcinfo<<"Illegal datetime: "<<start.toString()<<", "<<end.toString()<<endl;
        return eff;
    }
    // convert to calendar's timezone in case caller use a different timezone
    DateTime s = start.toTimeSpec( m_spec );
    DateTime e = end.toTimeSpec( m_spec );
    QDate date = s.date();
    QTime startTime = s.time();
    QTime endTime = e.time();
    if (e.date() > date) {
        endTime.setHMS(23, 59, 59, 999);
    }
    eff = effort(date, startTime, endTime, sch); // first day
    // Now get all the rest of the days
    for (date = date.addDays(1); date <= e.date(); date = date.addDays(1)) {
        if (date < e.date()) {
             eff += effort(date, QTime(0, 0, 0), endTime, sch); // whole days
        } else {
             eff += effort(date, QTime(0, 0, 0), e.time(), sch); // last day
        }
        //kDebug()<<k_funcinfo<<": eff now="<<eff.toString(Duration::Format_Day)<<endl;
    }
    //kDebug()<<k_funcinfo<<start.date().toString()<<"- "<<end.date().toString()<<": total="<<eff.toString(Duration::Format_Day)<<endl;
    return eff;
}


TimeInterval Calendar::firstInterval(const QDate &date, const QTime &startTime, const QTime &endTime, Schedule *sch) const {
    CalendarDay *day = findDay(date, true);
    if (day) {
        return day->interval(startTime, endTime, m_spec, sch);
    }
    if (m_weekdays) {
        if (m_weekdays->state(date) == CalendarDay::Working) {
            return m_weekdays->interval(date, startTime, endTime, m_spec, sch);
        }
        if (m_weekdays->state(date) == CalendarDay::NonWorking) {
            return TimeInterval(QTime(), QTime());
        }
    }
    if (m_parent) {
        return m_parent->firstInterval(date, startTime, endTime, sch);
    }
    return TimeInterval(QTime(), QTime());
}

DateTimeInterval Calendar::firstInterval(const DateTime &start, const DateTime &end, Schedule *sch) const {
    //kDebug()<<k_funcinfo<<"inp: "<<start.toString()<<" - "<<end.toString()<<endl;
    if (!start.isValid()) {
        kWarning()<<k_funcinfo<<"Invalid start time"<<endl;
        return DateTimeInterval(DateTime(), DateTime());
    }
    if (!end.isValid()) {
        kWarning()<<k_funcinfo<<"Invalid end time"<<endl;
        return DateTimeInterval(DateTime(), DateTime());
    }
    // convert to calendar's timezone in case caller use a different timezone
    DateTime s = DateTime( start.toTimeSpec( m_spec ) );
    DateTime e = DateTime( end.toTimeSpec( m_spec ) );
    //kDebug()<<k_funcinfo<<"tospec: "<<s.toString()<<" - "<<e.toString()<<endl;
    QTime startTime;
    QTime endTime;
    QDate date = start.date();
    for (; date <= e.date(); date = date.addDays(1)) {
        if (date < e.date())
            endTime = QTime(23, 59, 59, 999);
        else
            endTime = e.time();
        if (date > start.date())
            startTime = QTime(0, 0, 0);
        else 
            startTime = s.time();
            
        TimeInterval res = firstInterval(date, startTime, endTime, sch);
        if (!res.first.isNull() && !res.second.isNull() && res.first < res.second) {
            //kDebug()<<k_funcinfo<<"Found an interval ("<<date<<", "<<res.first<<", "<<res.second<<")"<<endl;
            // return result in callers timezone
            KDateTime::Spec es = start.timeSpec();
            DateTimeInterval dti( DateTime( date,res.first, m_spec ).toTimeSpec(es), DateTime( date, res.second, m_spec ).toTimeSpec(es) );
            //kDebug()<<k_funcinfo<<"Result: "<<dti.first.toString()<<", "<<dti.second.toString()<<endl;
            return dti;
        }
    }
    //kError()<<k_funcinfo<<"Didn't find an interval ("<<start<<", "<<end<<")"<<endl;
    return DateTimeInterval(DateTime(), DateTime());
}


bool Calendar::hasInterval(const QDate &date, const QTime &startTime, const QTime &endTime, Schedule *sch) const {
    CalendarDay *day = findDay(date, true);
    if (day) {
        //kDebug()<<k_funcinfo<<m_name<<" "<<date<<": "<<startTime<<" to "<<endTime<<endl;
        return day->hasInterval(startTime, endTime, m_spec, sch);
    } 
    if (m_weekdays) {
        if (m_weekdays->state(date) == CalendarDay::Working) {
            return m_weekdays->hasInterval(date, startTime, endTime, m_spec, sch);
        } else if (m_weekdays->state(date) == CalendarDay::NonWorking) {
            return false;
        }
    }
    if (m_parent) {
        return m_parent->hasInterval(date, startTime, endTime, sch);
    }
    return false;
}

bool Calendar::hasInterval(const DateTime &start, const DateTime &end, Schedule *sch) const {
    //kDebug()<<k_funcinfo<<m_name<<": "<<start<<" - "<<end<<endl;
    if (!start.isValid() || !end.isValid() || end <= start) {
        //kError()<<k_funcinfo<<"Invalid input: "<<(start.isValid()?"":"(start invalid) ")<<(end.isValid()?"":"(end invalid) ")<<(start>end?"":"(start<=end)")<<endl;
        //kDebug()<<kBacktrace(8)<<endl;
        return false;
    }
    // convert to calendar's timezone in case caller use a different timezone
    DateTime s = start.toTimeSpec( m_spec );
    DateTime e = end.toTimeSpec( m_spec );
    QTime startTime;
    QTime endTime;
    QDate date = s.date();
    for (; date <= e.date(); date = date.addDays(1)) {
        if (date < e.date())
            endTime = QTime(23, 59, 59, 999); //Hmmmm
        else
            endTime = e.time();
        if (date > s.date())
            startTime = QTime(0, 0, 0);
        else 
            startTime = s.time();

        if (hasInterval(date, startTime, endTime, sch))
            return true;
    }
    return false;
}

DateTime Calendar::firstAvailableAfter(const DateTime &time, const DateTime &limit, Schedule *sch ) {
    //kDebug()<<k_funcinfo<<m_name<<": check from "<<time<<" limit="<<limit<<endl;
    if (!time.isValid() || !limit.isValid() || time >= limit) {
        kError()<<k_funcinfo<<"Invalid input: "<<(time.isValid()?"":"(time invalid) ")<<(limit.isValid()?"":"(limit invalid) ")<<(time>limit?"":"(time>=limit)")<<endl;
        return DateTime();
    }
    
    if (!hasInterval(time, limit, sch)) {
        return DateTime();
    }
    DateTime t = firstInterval(time, limit, sch).first;
    //kDebug()<<k_funcinfo<<m_name<<": "<<t<<endl;
    return t;
}

DateTime Calendar::firstAvailableBefore(const DateTime &time, const DateTime &limit, Schedule *sch) {
    //kDebug()<<k_funcinfo<<m_name<<": check from "<<time.toString()<<" limit="<<limit.toString()<<endl;
    if (!time.isValid() || !limit.isValid() || time <= limit) {
        kError()<<k_funcinfo<<"Invalid input: "<<(time.isValid()?"":"(time invalid) ")<<(limit.isValid()?"":"(limit invalid) ")<<(time>limit?"":"(time<=limit)")<<endl;
        return DateTime();
    }
    // convert to calendar's timezone in case caller use a different timezone
    DateTime ltime = time.toTimeSpec( m_spec );
    DateTime llimit = limit.toTimeSpec( m_spec );
    DateTime lmt = ltime;
    DateTime t = DateTime(ltime.date(), m_spec); // start of first day
    if (t == lmt)
        t = t.addDays(-1); // in case time == start of day
    if (t < llimit)
        t = llimit;  // always stop at limit (lower boundary)
    DateTime res;
    //kDebug()<<k_funcinfo<<m_name<<": t="<<t<<", "<<lmt<<" limit="<<limit<<endl;
    while (!res.isValid() && t >= llimit) {
        // check intervals for 1 day
        DateTime r = firstInterval(t, lmt, sch).second;
        res = r;
        // Find the last interval
        while(r.isValid() && r < lmt) {
            r = firstInterval(r, lmt, sch).second;
            if (r.isValid() ) {
                res = r;
            }
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
    return res.toTimeSpec( time.timeSpec() ); // return in callers timezone
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

void Calendar::addDay( CalendarDay *day )
{
    emit dayToBeAdded( day, 0 );
    m_days.insert(0, day);
    emit dayAdded( day );
}

CalendarDay *Calendar::takeDay(CalendarDay *day)
{
    int i = m_days.indexOf(day);
    if (i == -1) {
        return 0;
    }
    emit dayToBeRemoved( day );
    m_days.removeAt(i);
    emit dayRemoved( day );
    return day;
}

QList<QPair<CalendarDay*, CalendarDay*> > Calendar::consecutiveVacationDays() const
{
    QList<QPair<CalendarDay*, CalendarDay*> > lst;
    QPair<CalendarDay*, CalendarDay*> interval( 0, 0 );
    foreach ( CalendarDay* day, m_days ) {
        if ( day->state() == CalendarDay::NonWorking ) {
            if ( interval.first == 0 ) {
                interval.first = day;
            }
            interval.second = day;
        } else {
            if ( interval.first != 0 ) {
                lst << QPair<CalendarDay*, CalendarDay*>( interval );
            }
            interval.first = interval.second = 0;
        }
    }
    return lst;
}

QList<CalendarDay*> Calendar::workingDays() const
{
    QList<CalendarDay*> lst;
    foreach ( CalendarDay* day, m_days ) {
        if ( day->state() == CalendarDay::Working ) {
            lst << day;
        }
    }
    return lst;
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
}

bool StandardWorktime::load( KoXmlElement &element, XMLLoaderObject &status ) {
    //kDebug()<<k_funcinfo<<endl;
    m_year = Duration::fromString(element.attribute("year"), Duration::Format_Hour); 
    m_month = Duration::fromString(element.attribute("month"), Duration::Format_Hour); 
    m_week = Duration::fromString(element.attribute("week"), Duration::Format_Hour); 
    m_day = Duration::fromString(element.attribute("day"), Duration::Format_Hour); 
    
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if (e.tagName() == "calendar") {
            // pre 0.6 version stored base calendar in standard worktime
            if ( status.version() >= "0.6" ) {
                kWarning()<<k_funcinfo<<"Old format, calendar in standard worktime"<<endl;
                kWarning()<<k_funcinfo<<"Tries to load anyway"<<endl;
            }
            // try to load anyway
            Calendar *calendar = new Calendar;
            if ( calendar->load( e, status ) ) {
                status.project().addCalendar( calendar );
                calendar->setDefault( true );
                status.project().setDefaultCalendar( calendar ); // hmmm
                status.setBaseCalendar( calendar );
            } else {
                delete calendar;
                kError()<<k_funcinfo<<"Failed to load calendar"<<endl;
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
    
}

#ifndef NDEBUG
void CalendarDay::printDebug(const QString& _indent) {
    QString indent = _indent;
    QString s[] = {"None", "Non-working", "Working"};
    kDebug()<<indent<<" "<<m_date.toString()<<" = "<<s[m_state]<<endl;
    if (m_state == CalendarDay::Working) {
        indent += "  ";
        foreach (TimeInterval *i, m_workingIntervals) {
            kDebug()<<indent<<" Interval: "<<i->first<<" to "<<i->second<<endl;
        }
    }
    
}
void CalendarWeekdays::printDebug(const QString& indent) {
    kDebug()<<indent<<"Weekdays ------"<<endl;
    int c=1;
    foreach ( CalendarDay *d, m_weekdays.values() ) {
        d->printDebug(indent + "  Day " + c++ + ": ");
    }

}
void Calendar::printDebug(const QString& indent) {
    kDebug()<<indent<<"Calendar "<<m_id<<": '"<<m_name<<endl;
    kDebug()<<indent<<"  Parent: "<<(m_parent ? m_parent->name() : "No parent")<<endl;
    m_weekdays->printDebug(indent + "  ");
    kDebug()<<indent<<"  Days --------"<<endl;
    foreach (CalendarDay *d, m_days) {
        d->printDebug(indent + "  ");
    }
}

void StandardWorktime::printDebug(const QString& indent) {
    kDebug()<<indent<<"StandardWorktime "<<endl;
    kDebug()<<indent<<"Year: "<<m_year.toString()<<endl;
    kDebug()<<indent<<"Month: "<<m_month.toString()<<endl;
    kDebug()<<indent<<"Week: "<<m_week.toString()<<endl;
    kDebug()<<indent<<"Day: "<<m_day.toString()<<endl;
}

#endif

}  //KPlato namespace

#include "kptcalendar.moc"
