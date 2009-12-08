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

#include "kptcalendar.h"

#include "kptmap.h"
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
#include <ktimezone.h>
#include <ksystemtimezone.h>
#include <kdebug.h>

namespace KPlato
{

/////   CalendarDay   ////
CalendarDay::CalendarDay()
    : m_date(),
      m_state(0),
      m_workingIntervals() {

    //kDebug()<<"("<<this<<")";
}

CalendarDay::CalendarDay(int state)
    : m_date(),
      m_state(state),
      m_workingIntervals() {

    //kDebug()<<"("<<this<<")";
}

CalendarDay::CalendarDay(const QDate& date, int state)
    : m_date(date),
      m_state(state),
      m_workingIntervals() {

    //kDebug()<<"("<<this<<")";
}

CalendarDay::CalendarDay(CalendarDay *day)
    : m_workingIntervals() {

    //kDebug()<<"("<<this<<") from ("<<day<<")";
    copy(*day);
}

CalendarDay::~CalendarDay() {
    //kDebug()<<"("<<this<<")";
    while (!m_workingIntervals.isEmpty())
        delete m_workingIntervals.takeFirst();
}

const CalendarDay &CalendarDay::copy(const CalendarDay &day) {
    //kDebug()<<"("<<&day<<") date="<<day.date().toString();
    m_date = day.date();
    m_state = day.state();
    m_workingIntervals.clear();
    foreach (TimeInterval *i, day.workingIntervals()) {
        m_workingIntervals.append( new TimeInterval( *i ) );
    }
    return *this;
}

bool CalendarDay::load( KoXmlElement &element, XMLLoaderObject &status ) {
    //kDebug();
    bool ok=false;
    m_state = QString(element.attribute("state", "-1")).toInt(&ok);
    if (m_state < 0)
        return false;
    //kDebug()<<" state="<<m_state;
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
            //kDebug()<<"Interval start="<<e.attribute("start")<<" end="<<e.attribute("end");
            QString st = e.attribute("start");
            if (st.isEmpty() ) {
                kError()<<"Empty interval";
                continue;
            }
            QTime start = QTime::fromString(st);
            int length = 0;
            if ( status.version() <= "0.6.1" ) {
                QString en = e.attribute("end");
                if ( en.isEmpty() ) {
                    kError()<<"Invalid interval end";
                    continue;
                }
                QTime end = QTime::fromString(en);
                length = start.msecsTo( end );
            } else {
                length = e.attribute("length", "0").toInt();
            }
            if ( length <= 0 ) {
                kError()<<"Invalid interval length";
                continue;
            }
            addInterval( new TimeInterval( start, length ) );
        }
    }
    return true;
}

void CalendarDay::save(QDomElement &element) const {
    //kDebug()<<m_date.toString();
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
        me.setAttribute("length", i->second);
        me.setAttribute("start", i->first.toString());
    }
} 

void CalendarDay::addInterval(TimeInterval *interval) {
    m_workingIntervals.append(interval);
}

    
bool CalendarDay::operator==(const CalendarDay *day) const {
    return operator==(*day);
}
bool CalendarDay::operator==(const CalendarDay &day) const {
    //kDebug();
    if (m_date.isValid() && day.date().isValid()) {
        if (m_date != day.date()) {
            //kDebug()<<m_date.toString()<<" !="<<day.date().toString();
            return false;
        }
    } else if (m_date.isValid() != day.date().isValid()) {
        //kDebug()<<"one of the dates is not valid";
        return false;
    }
    if (m_state != day.state()) {
        //kDebug()<<m_state<<" !="<<day.state();
        return false;
    }
    if (m_workingIntervals.count() != day.workingIntervals().count()) {
        //kDebug()<<m_workingIntervals.count()<<" !="<<day.workingIntervals().count();
        return false;
    }
    foreach (TimeInterval *a, m_workingIntervals) {
        bool res = false;
        foreach (TimeInterval *b, day.workingIntervals()) {
            if (a == b ) {
                res = true;
                break;
            }
        }
        if (res == false) {
            //kDebug()<<"interval mismatch"<<a->first.toString()<<"-"<<a->second.toString();
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

Duration CalendarDay::effort(const QTime &start, int length, const KDateTime::Spec &spec, Schedule *sch) {
    //kDebug()<<start<<" -"<<end;
    return effort( m_date, start, length, spec, sch );
}

Duration CalendarDay::effort(const QDate &date, const QTime &start, int length, const KDateTime::Spec &spec, Schedule *sch) {
    if ( !date.isValid() ) {
        return Duration::zeroDuration;
    }
    //kDebug()<<start.toString()<<" -"<<end.toString();
    Duration eff;
    if (m_state != Working) {
        //kDebug()<<"Non working day";
        return eff;
    }
    int l = 0;
    foreach (TimeInterval *i, m_workingIntervals) {
        if ( ! i->endsMidnight() && start >= i->endTime() ) {
            //kDebug()<<"Skip:"<<start<<">="<<i->first.addMSecs(i->second);
            continue;
        }
        QTime t1 = start.addMSecs( length );
        if ( t1 != QTime( 0, 0, 0 ) && t1 < i->first ) {
            //kDebug()<<"Skip:"<<t1<<"<"<<i->first;
            continue;
        }
        t1 = qMax( start, i->first );
        if ( i->endsMidnight() ) {
            l = t1.msecsTo( QTime( 23, 59, 59, 999 ) ) + 1;
        } else {
            l = t1.msecsTo( i->endTime() );
        }
        l = qMin( l, length - start.msecsTo( t1 ) );
        if ( l <= 0 ) {
            continue;
        }
        //kDebug()<<"Interval:"<<t1<<"->"<<l;
        DateTime dt1( date, t1, spec );
        DateTimeInterval dti( dt1, dt1.addMSecs( l ) );
        if ( sch ) {
            dti = sch->available( dti ); //FIXME needs an effort method
            //kDebug()<<"Checked sch:"<<dti.first<<" -"<<dti.second;
        }
        eff += dti.second - dti.first;
        //kDebug()<<dti.first.toString()<<" -"<<dti.second.toString()<<", effort now"<<eff.toString();
    }
    //kDebug()<<(m_date.isValid()?m_date.toString(Qt::ISODate):"Weekday")<<":"<<start.toString()<<" -"<<end.toString()<<": total="<<eff.toString(Duration::Format_Day);
    return eff;
}

Duration CalendarDay::workDuration() const
{
    Duration d;
    if (m_state != Working) {
        //kDebug()<<"Non working day";
        return d;
    }
    foreach (TimeInterval *i, m_workingIntervals) {
        //kDebug()<<"Interval:"<<i->first<<" -"<<i->second;
        d += Duration( (qint64)i->second );
    }
    return d;
}


TimeInterval CalendarDay::interval(const QTime &start, int length, const KDateTime::Spec &spec, Schedule *sch) const {
    //kDebug();
    return interval( m_date, start, length, spec, sch );
}

TimeInterval CalendarDay::interval(const QDate date, const QTime &start, int length, const KDateTime::Spec &spec, Schedule *sch) const
{
    //kDebug()<<"Inp:"<<date<<start<<"+"<<length<<"="<<start.addMSecs( length );
    Q_ASSERT( length > 0 );
    Q_ASSERT( QTime(0,0,0).msecsTo( start ) + length <= 1000*60*60*24 );
    QTime t1;
    int l = 0;
    if ( ! hasInterval() ) {
        return TimeInterval();
    }
    foreach (TimeInterval *i, m_workingIntervals) {
        //kDebug()<<"Interval:"<<i->first<<i->second<<i->first.addMSecs(i->second);
        if ( ! i->endsMidnight() && start >= i->endTime() ) {
            //kDebug()<<"Skip:"<<start<<">="<<i->first.addMSecs(i->second);
            continue;
        }
        QTime t1 = start.addMSecs( length );
        if ( t1 != QTime( 0, 0, 0 ) && t1 < i->first ) {
            //kDebug()<<"Skip:"<<t1<<"<"<<i->first;
            continue;
        }
        t1 = qMax( start, i->first );
        if ( i->endsMidnight() ) {
            l = t1.msecsTo( QTime( 23, 59, 59, 999 ) ) + 1;
        } else {
            l = t1.msecsTo( i->endTime() );
        }
        l = qMin( l, length - start.msecsTo( t1 ) );
        if ( l <= 0 ) {
            continue;
        }
        TimeInterval ti( t1, l );
        //kDebug()<<"Day give:"<<date<<","<<t1<<"->"<<l;
        if ( sch ) {
            // check if booked
            //kDebug()<<"Booked?"<<date<<","<<t1<<"+"<<l<<"="<<t1.addMSecs( l );
            DateTime dt1( DateTime( date, t1, spec ) );
            DateTimeInterval dti( dt1, dt1.addMSecs( l ) );
            dti = sch->available( dti );
            //kDebug()<<"Checked sch:"<<ti.first<<","<<ti.second<<"="<<dti;
            dt1 = dti.first.toTimeSpec( spec );
            l = ( DateTime(dti.second.toTimeSpec( spec )) - dt1 ).milliseconds();
            ti = TimeInterval( dt1.time(), l );
        }
        if ( ti.isValid() ) {
            //kDebug()<<"Return:"<<ti.first<<"+"<<ti.second<<"="<<ti.first.addMSecs( ti.second );
            return ti;
        }
    }
    return TimeInterval(t1, l);
}

bool CalendarDay::hasInterval() const
{
    return m_state == Working && m_workingIntervals.count() > 0;
}

bool CalendarDay::hasInterval(const QTime &start, int length, const KDateTime::Spec &spec, Schedule *sch) const {
    //kDebug()<<(m_date.isValid()?m_date.toString(Qt::ISODate):"Weekday")<<""<<start.toString()<<" -"<<end.toString();
    return hasInterval( m_date, start, length, spec, sch );
}

bool CalendarDay::hasInterval(const QDate date, const QTime &start, int length, const KDateTime::Spec &spec, Schedule *sch) const
{
    //kDebug()<<(m_date.isValid()?m_date.toString(Qt::ISODate):"Weekday")<<""<<start<<"->"<<length;
    return interval( date, start, length, spec, sch ).first.isValid();
}

Duration CalendarDay::duration() const {
    Duration dur;
    foreach (TimeInterval *i, m_workingIntervals) {
        dur += Duration( (qint64)i->second );
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

    //kDebug()<<"--->";
    for (int i=1; i <= 7; ++i) {
        m_weekdays.insert( i, new CalendarDay() );
    }
    //kDebug()<<"<---";
}

CalendarWeekdays::CalendarWeekdays( const CalendarWeekdays *weekdays )
    : m_weekdays() {
    //kDebug()<<"--->";
    copy(*weekdays);
    //kDebug()<<"<---";
}

CalendarWeekdays::~CalendarWeekdays() {
    qDeleteAll( m_weekdays );
    //kDebug();
}

const CalendarWeekdays &CalendarWeekdays::copy(const CalendarWeekdays &weekdays) {
    //kDebug();
    qDeleteAll( m_weekdays );
    m_weekdays.clear();
    QMapIterator<int, CalendarDay*> i( weekdays.weekdayMap() );
    while ( i.hasNext() ) {
        i.next();
        m_weekdays.insert( i.key(), new CalendarDay( i.value() ) );
    }
    return *this;
}

bool CalendarWeekdays::load( KoXmlElement &element, XMLLoaderObject &status ) {
    //kDebug();
    bool ok;
    int dayNo = QString(element.attribute("day","-1")).toInt(&ok);
    if (dayNo < 0 || dayNo > 6) {
        kError()<<"Illegal weekday: "<<dayNo;
        return true; // we continue anyway
    }
    CalendarDay *day = m_weekdays.value( dayNo + 1 );
    if ( day == 0 ) {
        kError()<<"No weekday: "<<dayNo;
        return false;
    }
    if (!day->load( element, status ) )
        day->setState(CalendarDay::None);
    return true;
}

void CalendarWeekdays::save(QDomElement &element) const {
    //kDebug();
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

QList<TimeInterval*> CalendarWeekdays::intervals(int weekday) const { 
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

Duration CalendarWeekdays::effort(const QDate &date, const QTime &start, int length, const KDateTime::Spec &spec, Schedule *sch) {
    //kDebug()<<"Day of week="<<date.dayOfWeek();
    Q_ASSERT( QTime(0,0,0).msecsTo( start ) + length <= 1000*60*60*24 );
    CalendarDay *day = weekday( date.dayOfWeek() );
    if (day && day->state() == CalendarDay::Working) {
        return day->effort(date, start, length, spec, sch);
    }
    return Duration::zeroDuration;
}

TimeInterval CalendarWeekdays::interval(const QDate date, const QTime &start, int length, const KDateTime::Spec &spec, Schedule *sch) const
{
    //kDebug();
    CalendarDay *day = weekday( date.dayOfWeek() );
    if (day && day->state() == CalendarDay::Working) {
        return day->interval(date, start, length, spec, sch);
    }
    return TimeInterval();
}

bool CalendarWeekdays::hasInterval(const QDate date, const QTime &start, int length, const KDateTime::Spec &spec, Schedule *sch) const
{
    //kDebug()<<date<<":"<<start<<"+"<<length;
    CalendarDay *day = weekday( date.dayOfWeek() );
    return day && day->hasInterval(date, start, length, spec, sch);
}

bool CalendarWeekdays::hasInterval() const
{
    //kDebug();
    foreach ( CalendarDay *d, m_weekdays ) {
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
    foreach ( CalendarDay *d, m_weekdays ) {
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
    //kDebug()<<"deleting"<<m_name;
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
    m_spec = KDateTime::Spec::LocalZone();
    if ( !m_spec.timeZone().isValid() ) {
        m_spec.setType( KTimeZone() );
    }
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

void Calendar::setTimeZone( const KTimeZone &tz )
{
    //kDebug()<<tz->name();
    m_spec = KDateTime::Spec( tz );
    if ( m_project ) {
        m_project->changed( this );
    }
}

void Calendar::setDefault( bool on )
{
    m_default = on;
    if ( m_project ) {
        m_project->changed( this );
    }
}

// Note: only project should do this
void Calendar::setId(const QString& id) {
    //kDebug()<<id;
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
    //kDebug()<<element.text();
    //bool ok;
    setId(element.attribute("id"));
    m_parentId = element.attribute("parent");
    m_name = element.attribute("name","");
    KTimeZone tz = KSystemTimeZones::zone( element.attribute( "timezone" ) );
    if ( tz.isValid() ) {
        setTimeZone( tz );
    } else kWarning()<<"No timezone specified, use default (local)";
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
                    kError()<<m_name<<": Failed to load calendarDay - Invalid date";
                } else {
                    CalendarDay *d = findDay(day->date());
                    if (d) {
                        // already exists, keep the new
                        delete takeDay(d);
                        kWarning()<<m_name<<" Load calendarDay - Date already exists";
                    }
                    addDay(day);
                }
            } else {
                delete day;
                kError()<<"Failed to load calendarDay";
                return true; //false; don't throw away the whole calendar
            }
        }
    }
    return true;
}

void Calendar::save(QDomElement &element) const {
    //kDebug()<<m_name;
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
    me.setAttribute("timezone", m_spec.timeZone().name() );
    m_weekdays->save(me);
    foreach (CalendarDay *d, m_days) {
        QDomElement e = me.ownerDocument().createElement("day");
        me.appendChild(e);
        d->save(e);
    }
    
}

int Calendar::state(const QDate &date) const
{
    CalendarDay *day = findDay( date );
    if ( day && day->state() != CalendarDay::Undefined ) {
        return day->state();
    }
    day = weekday( date.dayOfWeek() );
    if ( day && day->state() != CalendarDay::Undefined ) {
        return day->state();
    }
    return m_parent ? m_parent->state( date ) : CalendarDay::Undefined;
}

CalendarDay *Calendar::findDay(const QDate &date, bool skipUndefined) const {
    //kDebug()<<date.toString();
    foreach (CalendarDay *d, m_days) {
        if (d->date() == date) {
            if (skipUndefined  && d->state() == CalendarDay::Undefined) {
                continue; // hmmm, break?
            }
            return d;
        }
    }
    //kDebug()<<date.toString()<<" not found";
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
    *ti = value;
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
        TimeInterval *t = new TimeInterval( *ti );
        emit workIntervalToBeAdded( wd, t, wd->numIntervals() ); // hmmmm
        wd->addInterval( t );
        emit workIntervalAdded( wd, t );
    }
}

bool Calendar::hasParent(Calendar *cal) {
    //kDebug();
    if (!m_parent)
        return false;
    if (m_parent == cal)
        return true;
    return m_parent->hasParent(cal);
}

Duration Calendar::effort(const QDate &date, const QTime &start, int length, Schedule *sch) const {
    //kDebug()<<m_name<<":"<<date<<""<<start<<"->"<<length;
    if (length <= 0) {
        return Duration::zeroDuration;
    }
    // first, check my own day
    CalendarDay *day = findDay(date, true);
    if (day) {
        if (day->state() == CalendarDay::Working) {
            return day->effort(start, length, m_spec, sch);
        } else if (day->state() == CalendarDay::NonWorking) {
            return Duration::zeroDuration;
        } else {
            kError()<<"Invalid state: "<<day->state();
            return Duration::zeroDuration;
        }
    }
    // check my own weekdays
    if (m_weekdays) {
        if (m_weekdays->state(date) == CalendarDay::Working) {
            return m_weekdays->effort(date, start, length, m_spec, sch);
        }
        if (m_weekdays->state(date) == CalendarDay::NonWorking) {
            return Duration::zeroDuration;
        }
    }
    if (m_parent) {
        return m_parent->effort(date, start, length, sch);
    }
    return Duration::zeroDuration;
}

Duration Calendar::effort(const DateTime &start, const DateTime &end, Schedule *sch) const {
    //kDebug()<<m_name<<":"<<start<<" to"<<end;
    Duration eff;
    if (!start.isValid() || !end.isValid() || end < start) {
        if ( sch && sch->resource() ) kDebug()<<sch->resource()->name()<<sch->name()<<"Available:"<<sch->resource()->availableFrom()<<sch->resource()->availableUntil();
        kError()<<"Illegal datetime: "<<start<<", "<<end;
        return eff;
    }
    if ( start == end ) {
        //kDebug()<<"start == end";
        return eff;
    }
    // convert to calendar's timezone in case caller use a different timezone
    DateTime s = start.toTimeSpec( m_spec );
    DateTime e = end.toTimeSpec( m_spec );
    QDate date = s.date();
    QTime startTime = s.time();
    QTime endTime = e.time();
    int length = 0;
    if ( date == e.date() ) {
        // single day
        length = startTime.msecsTo( endTime );
        return effort( date, startTime, length, sch );
    }
    length = startTime.msecsTo( QTime( 23, 59, 59, 999 ) ) + 1;
    QTime t0(0, 0, 0);
    int aday = t0.msecsTo( QTime( 23, 59, 59, 999 ) ) + 1;
    eff = effort(date, startTime, length, sch); // first day
    // Now get all the rest of the days
    for (date = date.addDays(1); date <= e.date(); date = date.addDays(1)) {
        if (date < e.date()) {
            eff += effort(date, t0, aday, sch); // whole days
        } else if ( endTime > t0 ) {
            eff += effort(date, t0, t0.msecsTo( endTime ), sch); // last day
        }
        //kDebug()<<": eff now="<<eff.toString(Duration::Format_Day);
    }
    //kDebug()<<start<<"-"<<end<<": total="<<eff.toString();
    return eff;
}


TimeInterval Calendar::firstInterval(const QDate &date, const QTime &startTime, int length, Schedule *sch) const {
    //kDebug();
    CalendarDay *day = findDay(date, true);
    if (day) {
        return day->interval(startTime, length, m_spec, sch);
    }
    if (m_weekdays) {
        if (m_weekdays->state(date) == CalendarDay::Working) {
            //kDebug()<<"Check weekday";
            TimeInterval i = m_weekdays->interval(date, startTime, length, m_spec, sch);
            //kDebug()<<"Checked weekday, got"<<i;
            return i;
        }
        if (m_weekdays->state(date) == CalendarDay::NonWorking) {
            return TimeInterval();
        }
    }
    if (m_parent) {
        //kDebug()<<"Check parent";
        return m_parent->firstInterval(date, startTime, length, sch);
    }
    return TimeInterval();
}

DateTimeInterval Calendar::firstInterval(const DateTime &start, const DateTime &end, Schedule *sch) const {
    //kDebug()<<"inp:"<<start.toString()<<" -"<<end.toString();
    if (!start.isValid()) {
        kWarning()<<"Invalid start time";
        return DateTimeInterval(DateTime(), DateTime());
    }
    if (!end.isValid()) {
        kWarning()<<"Invalid end time";
        return DateTimeInterval(DateTime(), DateTime());
    }
    KDateTime::Spec es = start.timeSpec();
    // convert to calendar's timezone in case caller use a different timezone
    DateTime s = DateTime( start.toTimeSpec( m_spec ) );
    DateTime e = DateTime( end.toTimeSpec( m_spec ) );
    if ( s == e || s > e ) {
        kWarning()<<"Invalid interval";
        return DateTimeInterval();
    }
    TimeInterval res;
    QTime startTime = s.time();
    int length = 0;
    if ( s.date() == e.date() ) {
        // Handle single day
        length = startTime.msecsTo( e.time() );
        if ( length <= 0 ) {
            kWarning()<<"Invalid length"<<length;
            return DateTimeInterval();
        }
        //kDebug()<<"Check single day:"<<s.date()<<s.time()<<length;
        res = firstInterval(s.date(), s.time(), length, sch);
        if ( ! res.isValid() ) {
            return DateTimeInterval();
        }
        DateTime dt1 = DateTime( s.date(), res.first, m_spec ).toTimeSpec(es);
        DateTimeInterval dti( dt1, dt1.addMSecs( res.second ) );
        //kDebug()<<"Got:"<<dti;
        return dti;
    }
    //kDebug()<<"tospec:"<<s.toString()<<" -"<<e.toString();
    // Multiple days
    for ( QDate date = s.date(); date <= e.date(); date = date.addDays(1) ) {
        if (date > s.date()) {
            startTime = QTime(0, 0, 0);
        }
        if (date < e.date()) {
            length = startTime.msecsTo( QTime(23, 59, 59, 999) ) + 1;
        } else {
            length = startTime.msecsTo( e.time() );
        }
        if ( length <= 0 ) {
            break;
        }
        //kDebug()<<"Check:"<<date<<startTime<<"+"<<length<<"="<<startTime.addMSecs( length );
        res = firstInterval(date, startTime, length, sch);
        if ( res.isValid() ) {
            //kDebug()<<"inp:"<<start<<"-"<<end;
            //kDebug()<<"Found an interval ("<<date<<","<<res.first<<","<<res.second<<")";
            // return result in callers timezone
            DateTime dt1 = DateTime( date,res.first, m_spec ).toTimeSpec(es);
            DateTimeInterval dti( dt1, dt1.addMSecs( res.second ) );
            //kDebug()<<"Result:"<<dti.first.toString()<<","<<dti.second.toString();
            return dti;
        }
    }
    //kWarning()<<"Didn't find an interval ("<<start<<", "<<end<<")";
    return DateTimeInterval(DateTime(), DateTime());
}


bool Calendar::hasInterval(const QDate &date, const QTime &startTime, int length, Schedule *sch) const
{
    //kDebug();
    return ! firstInterval( date, startTime, length, sch ).first.isNull();
}

bool Calendar::hasInterval(const DateTime &start, const DateTime &end, Schedule *sch) const {
    //kDebug();
    return ! firstInterval( start, end, sch ).first.isNull();
}

DateTime Calendar::firstAvailableAfter(const DateTime &time, const DateTime &limit, Schedule *sch ) {
    //kDebug()<<m_name<<": check from"<<time<<" limit="<<limit;
    if (!time.isValid() || !limit.isValid() || time > limit) {
        kError()<<"Invalid input: "<<(time.isValid()?"":"(time invalid) ")<<(limit.isValid()?"":"(limit invalid) ")<<(time>limit?"":"(time>limit)");
        return DateTime();
    }
    if ( time == limit ) {
        return DateTime();
    }
    DateTime t = firstInterval(time, limit, sch).first;
    //kDebug()<<m_name<<":"<<t;
    return t;
}

DateTime Calendar::firstAvailableBefore(const DateTime &time, const DateTime &limit, Schedule *sch) {
    //kDebug()<<m_name<<": check from"<<time<<" limit="<<limit;
    if (!time.isValid() || !limit.isValid() || time < limit) {
        kError()<<"Invalid input: "<<(time.isValid()?"":"(time invalid) ")<<(limit.isValid()?"":"(limit invalid) ")<<(time<limit?"":"(time<limit)");
        return DateTime();
    }
    if ( time == limit ) {
        return DateTime();
    }
    // convert to calendar's timezone in case caller use a different timezone
    DateTime ltime = time.toTimeSpec( m_spec );
    DateTime llimit = limit.toTimeSpec( m_spec );
    DateTime lmt = ltime;
    DateTime t = DateTime(ltime.date(), QTime(), m_spec); // start of first day
    if (t == lmt)
        t = t.addDays(-1); // in case time == start of day
    if (t < llimit)
        t = llimit;  // always stop at limit (lower boundary)
    DateTime res;
    //kDebug()<<m_name<<": t="<<t<<","<<lmt<<" limit="<<limit;
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
            //kDebug()<<m_name<<": r="<<r<<","<<lmt<<" res="<<res;
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
    //kDebug()<<m_name<<":"<<res;
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
StandardWorktime::StandardWorktime( Project *project )
    : m_project( project )
 {
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

StandardWorktime::~StandardWorktime()
{
    //kDebug()<<"("<<this<<")";
}

void StandardWorktime::init() {
    // Some sane default values
    m_year = Duration(0, 1760, 0);
    m_month = Duration(0, 176, 0);
    m_week = Duration(0, 40, 0);
    m_day = Duration(0, 8, 0);
}

void StandardWorktime::changed()
{
    if ( m_project ) {
        m_project->changed( this );
    }
}

QList<double> StandardWorktime::scales() const
{
    return QList<double>() << year() / month() << month() / week() << week() / day() << day();
}

bool StandardWorktime::load( KoXmlElement &element, XMLLoaderObject &status ) {
    //kDebug();
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
                kWarning()<<"Old format, calendar in standard worktime";
                kWarning()<<"Tries to load anyway";
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
                kError()<<"Failed to load calendar";
            }
        }
    }
    return true;
}

void StandardWorktime::save(QDomElement &element) const {
    //kDebug();
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
    qDebug()<<indent<<""<<m_date.toString()<<" ="<<s[m_state];
    if (m_state == CalendarDay::Working) {
        indent += "  ";
        foreach (TimeInterval *i, m_workingIntervals) {
            QString end = i->endsMidnight() ? "midnight" : i->endTime().toString( Qt::ISODate );
            qDebug()<<indent<<" Interval:"<<i->first.toString( Qt::ISODate )<<"to"<<end;
        }
    }
    
}
void CalendarWeekdays::printDebug(const QString& indent) {
    qDebug()<<indent<<"Weekdays ------";
    int c=1;
    foreach ( CalendarDay *d, m_weekdays ) {
        d->printDebug(indent + "  Day " + c++ + ": ");
    }

}
void Calendar::printDebug(const QString& indent) {
    qDebug()<<indent<<"Calendar"<<m_id<<":"<<m_name<<(isDefault()?" (Default calendar)":"");
    qDebug()<<indent<<"  Parent:"<<(m_parent ? m_parent->name() :"No parent");
    m_weekdays->printDebug(indent + "  ");
    qDebug()<<indent<<"  Days --------";
    foreach (CalendarDay *d, m_days) {
        d->printDebug(indent + "  ");
    }
}

void StandardWorktime::printDebug(const QString& indent) {
    qDebug()<<indent<<"Estimate conversion";
    qDebug()<<indent<<"Year:"<<m_year.toString()<<"("<<year()<<"hours )";
    qDebug()<<indent<<"Month:"<<m_month.toString()<<"("<<month()<<"hours )";
    qDebug()<<indent<<"Week:"<<m_week.toString()<<"("<<week()<<"hours )";
    qDebug()<<indent<<"Day:"<<m_day.toString()<<"("<<day()<<"hours )";
}

#endif

}  //KPlato namespace

#include "kptcalendar.moc"
