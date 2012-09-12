/* This file is part of the KDE project
 Copyright (C) 2005 - 2011, 2012 Dag Andersen <danders@get2net.dk>

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

#include "kptschedule.h"

#include "kptappointment.h"
#include "kptdatetime.h"
#include "kptduration.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptxmlloaderobject.h"
#include "kptschedulerplugin.h"
#include "kptdebug.h"

#include <KoXmlReader.h>

#include <QDomElement>
#include <QList>
#include <QString>
#include <QStringList>

#include <klocale.h>


namespace KPlato
{

class ScheduleManager;

Schedule::Log::Log( const Node *n, int sev, const QString &msg, int ph )
    : node( n ), resource( 0 ), message( msg ), severity( sev ), phase( ph )
{
    Q_ASSERT( n );
//     kDebug(planDbg())<<*this<<nodeId;
}

Schedule::Log::Log( const Node *n, const Resource *r, int sev, const QString &msg, int ph )
    : node( n ), resource( r ), message( msg ), severity( sev ), phase( ph )
{
    Q_ASSERT( r );
//     kDebug(planDbg())<<*this<<resourceId;
}

Schedule::Schedule()
        : m_type( Expected ),
        m_id( 0 ),
        m_deleted( false ),
        m_parent( 0 ),
        m_obstate( OBS_Parent ),
        m_calculationMode( Schedule::Scheduling ),
        notScheduled( true )
{
    initiateCalculation();
}

Schedule::Schedule( Schedule *parent )
        : m_type( Expected ),
        m_id( 0 ),
        m_deleted( false ),
        m_parent( parent ),
        m_obstate( OBS_Parent ),
        m_calculationMode( Schedule::Scheduling ),
        notScheduled( true )
{

    if ( parent ) {
        m_name = parent->name();
        m_type = parent->type();
        m_id = parent->id();
    }
    initiateCalculation();
    //kDebug(planDbg())<<"("<<this<<") Name: '"<<name<<"' Type="<<type<<" id="<<id;
}

Schedule::Schedule( const QString& name, Type type, long id )
        : m_name( name ),
        m_type( type ),
        m_id( id ),
        m_deleted( false ),
        m_parent( 0 ),
        m_obstate( OBS_Parent ),
        m_calculationMode( Schedule::Scheduling ),
        notScheduled( true )
{
    //kDebug(planDbg())<<"("<<this<<") Name: '"<<name<<"' Type="<<type<<" id="<<id;
    initiateCalculation();
}

Schedule::~Schedule()
{
}

void Schedule::setParent( Schedule *parent )
{
    m_parent = parent;
}

void Schedule::setDeleted( bool on )
{
    //kDebug(planDbg())<<"deleted="<<on;
    m_deleted = on;
    //changed( this ); don't do this!
}

bool Schedule::isDeleted() const
{
    return m_parent == 0 ? m_deleted : m_parent->isDeleted();
}

void Schedule::setType( const QString& type )
{
    m_type = Expected;
    if ( type == "Expected" )
        m_type = Expected;
}

QString Schedule::typeToString( bool translate ) const
{
    if ( translate ) {
        return i18n( "Expected" );
    } else {
        return "Expected";
    }
}

QStringList Schedule::state() const
{
    QStringList lst;
    if ( m_deleted )
        lst << SchedulingState::deleted();
    if ( notScheduled )
        lst << SchedulingState::notScheduled();
    if ( constraintError )
        lst << SchedulingState::constraintsNotMet();
    if ( resourceError )
        lst << SchedulingState::resourceNotAllocated();
    if ( resourceNotAvailable )
        lst << SchedulingState::resourceNotAvailable();
    if ( resourceOverbooked )
        lst << SchedulingState::resourceOverbooked();
    if ( effortNotMet )
        lst << SchedulingState::effortNotMet();
    if ( schedulingError )
        lst << SchedulingState::schedulingError();
    if ( lst.isEmpty() )
        lst << SchedulingState::scheduled();
    return lst;
}

bool Schedule::isBaselined() const
{
    if ( m_parent ) {
        return m_parent->isBaselined();
    }
    return false;
}

bool Schedule::usePert() const
{
    if ( m_parent ) {
        return m_parent->usePert();
    }
    return false;
}

void Schedule::setAllowOverbookingState( Schedule::OBState state )
{
    m_obstate = state;
}

Schedule::OBState Schedule::allowOverbookingState() const
{
    return m_obstate;
}

bool Schedule::allowOverbooking() const
{
    if ( m_obstate == OBS_Parent && m_parent ) {
        return m_parent->allowOverbooking();
    }
    return m_obstate == OBS_Allow;
}

bool Schedule::checkExternalAppointments() const
{
    if ( m_parent ) {
        return m_parent->checkExternalAppointments();
    }
    return false;
}

void Schedule::setScheduled( bool on )
{
    notScheduled = !on;
    changed( this );
}

Duration Schedule::effort( const DateTimeInterval &interval ) const
{
    return interval.second - interval.first;
}

DateTimeInterval Schedule::available( const DateTimeInterval &interval ) const
{
    return DateTimeInterval( interval.first, interval.second );
}

void Schedule::initiateCalculation()
{
    resourceError = false;
    resourceOverbooked = false;
    resourceNotAvailable = false;
    constraintError = false;
    schedulingError = false;
    inCriticalPath = false;
    effortNotMet = false;
    workStartTime = DateTime();
    workEndTime = DateTime();


}

void Schedule::calcResourceOverbooked()
{
    resourceOverbooked = false;
    foreach( Appointment *a, m_appointments ) {
        if ( a->resource() ->isOverbooked( a->startTime(), a->endTime() ) ) {
            resourceOverbooked = true;
            break;
        }
    }
}

DateTimeInterval Schedule::firstBookedInterval( const DateTimeInterval &interval, const Schedule *node  ) const
{
    QList<Appointment*> lst = m_appointments;
    switch ( m_calculationMode ) {
        case CalculateForward: lst = m_forward; break;
        case CalculateBackward: lst = m_backward; break;
        default: break;
    }
    foreach ( Appointment *a, lst ) {
        if ( a->node() == node ) {
            AppointmentIntervalList i = a->intervals( interval.first, interval.second );
            if ( i.isEmpty() ) {
                break;
            }
            return DateTimeInterval( i.map().values().first().startTime(), i.map().values().first().endTime() );
        }
    }
    return DateTimeInterval();
}

QStringList Schedule::overbookedResources() const
{
    QStringList rl;
    foreach( Appointment *a, m_appointments ) {
        if ( a->resource() ->isOverbooked( a->startTime(), a->endTime() ) ) {
            rl += a->resource() ->resource() ->name();
        }
    }
    return rl;
}

QStringList Schedule::resourceNameList() const
{
    return QStringList();
}

QList<Resource*> Schedule::resources() const
{
    return QList<Resource*>();
}

bool Schedule::loadXML( const KoXmlElement &sch, XMLLoaderObject & )
{
    m_name = sch.attribute( "name" );
    setType( sch.attribute( "type" ) );
    m_id = sch.attribute( "id" ).toLong();

    return true;
}

void Schedule::saveXML( QDomElement &element ) const
{
    QDomElement sch = element.ownerDocument().createElement( "schedule" );
    element.appendChild( sch );
    saveCommonXML( sch );
}

void Schedule::saveCommonXML( QDomElement &element ) const
{
    //kDebug(planDbg())<<m_name<<" save schedule";
    element.setAttribute( "name", m_name );
    element.setAttribute( "type", typeToString() );
    element.setAttribute( "id", qlonglong( m_id ) );
}

void Schedule::saveAppointments( QDomElement &element ) const
{
    //kDebug(planDbg());
    QListIterator<Appointment*> it = m_appointments;
    while ( it.hasNext() ) {
        it.next() ->saveXML( element );
    }
}

void Schedule::insertForwardNode( Node *node )
{
    if ( m_parent ) {
        m_parent->insertForwardNode( node );
    }
}

void Schedule::insertBackwardNode( Node *node )
{
    if ( m_parent ) {
        m_parent->insertBackwardNode( node );
    }
}

// used (directly) when appointment wants to attatch itself again
bool Schedule::attatch( Appointment *appointment )
{
    int mode = appointment->calculationMode();
    //kDebug(planDbg())<<appointment<<mode;
    if ( mode == Scheduling ) {
        if ( m_appointments.indexOf( appointment ) != -1 ) {
            kError() << "Appointment already exists" << endl;
            return false;
        }
        m_appointments.append( appointment );
        //if (resource()) kDebug(planDbg())<<appointment<<" For resource '"<<resource()->name()<<"'"<<" count="<<m_appointments.count();
        //if (node()) kDebug(planDbg())<<"("<<this<<")"<<appointment<<" For node '"<<node()->name()<<"'"<<" count="<<m_appointments.count();
        return true;
    }
    if ( mode == CalculateForward ) {
        if ( m_forward.indexOf( appointment ) != -1 ) {
            kError() << "Appointment already exists" << endl;
            return false;
        }
        m_forward.append( appointment );
        //if (resource()) kDebug(planDbg())<<"For resource '"<<resource()->name()<<"'";
        //if (node()) kDebug(planDbg())<<"For node '"<<node()->name()<<"'";
        return true;
    }
    if ( mode == CalculateBackward ) {
        if ( m_backward.indexOf( appointment ) != -1 ) {
            kError() << "Appointment already exists" << endl;
            return false;
        }
        m_backward.append( appointment );
        //if (resource()) kDebug(planDbg())<<"For resource '"<<resource()->name()<<"'";
        //if (node()) kDebug(planDbg())<<"For node '"<<node()->name()<<"'";
        return true;
    }
    kError()<<"Unknown mode: "<<m_calculationMode<<endl;
    return false;
}

// used to add new schedules
bool Schedule::add( Appointment *appointment )
{
    //kDebug(planDbg())<<this;
    appointment->setCalculationMode( m_calculationMode );
    return attatch( appointment );
}

void Schedule::takeAppointment( Appointment *appointment, int mode )
{
    Q_UNUSED(mode);
    //kDebug(planDbg())<<"("<<this<<")"<<mode<<":"<<appointment<<","<<appointment->calculationMode();
    int i = m_forward.indexOf( appointment );
    if ( i != -1 ) {
        m_forward.removeAt( i );
        Q_ASSERT( mode == CalculateForward );
    }
    i = m_backward.indexOf( appointment );
    if ( i != -1 ) {
        m_backward.removeAt( i );
        Q_ASSERT( mode == CalculateBackward );
    }
    i = m_appointments.indexOf( appointment );
    if ( i != -1 ) {
        m_appointments.removeAt( i );
        Q_ASSERT( mode == Scheduling );
    }
}

Appointment *Schedule::findAppointment( Schedule *resource, Schedule *node, int mode )
{
    //kDebug(planDbg())<<this<<" ("<<resourceError<<","<<node<<")"<<mode;
    if ( mode == Scheduling ) {
        foreach( Appointment *a,  m_appointments ) {
            if ( a->node() == node && a->resource() == resource ) {
                return a;
            }
        }
        return 0;
    } else if ( mode == CalculateForward ) {
        foreach( Appointment *a,  m_forward ) {
            if ( a->node() == node && a->resource() == resource ) {
                return a;
            }
        }
    } else if ( mode == CalculateBackward ) {
        foreach( Appointment *a,  m_backward ) {
            if ( a->node() == node && a->resource() == resource ) {
                Q_ASSERT( mode == CalculateBackward );
                return a;
            }
        }
    } else {
        Q_ASSERT( false ); // unknown mode
    }
    return 0;
}

DateTime Schedule::appointmentStartTime() const
{
    DateTime dt;
    foreach ( const Appointment *a, m_appointments ) {
        if ( ! dt.isValid() || dt > a->startTime() ) {
            dt = a->startTime();
        }
    }
    return dt;
}
DateTime Schedule::appointmentEndTime() const
{
    DateTime dt;
    foreach ( const Appointment *a, m_appointments ) {
        if ( ! dt.isValid() || dt > a->endTime() ) {
            dt = a->endTime();
        }
    }
    return dt;
}

bool Schedule::hasAppointments( int which ) const
{
    if ( which == CalculateForward ) {
        return m_forward.isEmpty();
    } else if ( which == CalculateBackward ) {
        return m_backward.isEmpty();
    }
    return m_appointments.isEmpty();
}

QList<Appointment*> Schedule::appointments( int which ) const
{
    if ( which == CalculateForward ) {
        return m_forward;
    } else if ( which == CalculateBackward ) {
        return m_backward;
    }
    return m_appointments;
}

Appointment Schedule::appointmentIntervals( int which, const DateTimeInterval &interval ) const
{
    Appointment app;
    if ( which == Schedule::CalculateForward ) {
        //kDebug(planDbg())<<"list == CalculateForward";
        foreach ( Appointment *a, m_forward ) {
            app += interval.isValid() ? a->extractIntervals( interval ) : *a;
        }
        return app;
    } else if ( which == Schedule::CalculateBackward ) {
        //kDebug(planDbg())<<"list == CalculateBackward";
        foreach ( Appointment *a, m_backward ) {
            app += interval.isValid() ? a->extractIntervals( interval ) : *a;
        }
        //kDebug(planDbg())<<"list == CalculateBackward:"<<m_backward.count();
        return app;
    }
    foreach ( Appointment *a, m_appointments ) {
        app += interval.isValid() ? a->extractIntervals( interval ) : *a;
    }
    return app;
}

void Schedule::copyAppointments( Schedule::CalculationMode from, Schedule::CalculationMode to )
{
    switch ( to ) {
        case Scheduling:
            m_appointments.clear();
            switch( from ) {
                case CalculateForward:
                    m_appointments = m_forward;
                    break;
                case CalculateBackward:
                    m_appointments = m_backward;
                    break;
                default:
                    break;
            }
            break;
        case CalculateForward: break;
        case CalculateBackward: break;
    }
}


EffortCostMap Schedule::bcwsPrDay( EffortCostCalculationType type ) const
{
    //kDebug(planDbg())<<m_name<<m_appointments;
    EffortCostMap ec;
    foreach ( Appointment *a, m_appointments ) {
        ec += a->plannedPrDay( a->startTime().date(), a->endTime().date(), type );
    }
    return ec;
}

EffortCostMap Schedule::plannedEffortCostPrDay( const QDate &start, const QDate &end, EffortCostCalculationType type ) const
{
    //kDebug(planDbg())<<m_name<<m_appointments;
    EffortCostMap ec;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        //kDebug(planDbg())<<m_name;
        ec += it.next() ->plannedPrDay( start, end, type );
    }
    return ec;
}

EffortCostMap Schedule::plannedEffortCostPrDay( const Resource *resource, const QDate &start, const QDate &end, EffortCostCalculationType type ) const
{
    //kDebug(planDbg())<<m_name<<m_appointments;
    EffortCostMap ec;
    foreach ( Appointment *a, m_appointments ) {
        if ( a->resource() && a->resource()->resource() == resource ) {
            ec += a->plannedPrDay( start, end, type );
            break;
        }
    }
    return ec;
}

Duration Schedule::plannedEffort( const Resource *resource, EffortCostCalculationType type ) const
{
    //kDebug(planDbg());
    Duration eff;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        eff += it.next() ->plannedEffort( resource, type );
    }
    return eff;
}

Duration Schedule::plannedEffort( EffortCostCalculationType type ) const
{
    //kDebug(planDbg());
    Duration eff;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        eff += it.next() ->plannedEffort( type );
    }
    return eff;
}

Duration Schedule::plannedEffort( const QDate &date, EffortCostCalculationType type ) const
{
    //kDebug(planDbg());
    Duration eff;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        eff += it.next() ->plannedEffort( date, type );
    }
    return eff;
}

Duration Schedule::plannedEffort( const Resource *resource, const QDate &date, EffortCostCalculationType type ) const
{
    //kDebug(planDbg());
    Duration eff;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        eff += it.next() ->plannedEffort( resource, date, type );
    }
    return eff;
}

Duration Schedule::plannedEffortTo( const QDate &date, EffortCostCalculationType type ) const
{
    //kDebug(planDbg());
    Duration eff;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        eff += it.next() ->plannedEffortTo( date, type );
    }
    return eff;
}

Duration Schedule::plannedEffortTo(  const Resource *resource, const QDate &date, EffortCostCalculationType type ) const
{
    //kDebug(planDbg());
    Duration eff;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        eff += it.next() ->plannedEffortTo( resource, date, type );
    }
    return eff;
}

EffortCost Schedule::plannedCost( EffortCostCalculationType type ) const
{
    //kDebug(planDbg());
    EffortCost c;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        c += it.next() ->plannedCost( type );
    }
    return c;
}

double Schedule::plannedCost( const QDate &date, EffortCostCalculationType type ) const
{
    //kDebug(planDbg());
    double c = 0;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        c += it.next() ->plannedCost( date, type );
    }
    return c;
}

double Schedule::plannedCostTo( const QDate &date, EffortCostCalculationType type ) const
{
    //kDebug(planDbg());
    double c = 0;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        c += it.next() ->plannedCostTo( date, type );
    }
    return c;
}

void Schedule::addLog( Schedule::Log &log )
{
    if ( m_parent ) {
        m_parent->addLog( log );
    }
}

QString Schedule::Log::formatMsg() const
 {
    QString s;
    s += node ? QString( "%1 " ).arg( node->name(), -8 ) : "";
    s += resource ? QString( "%1 ").arg(resource->name(), -8 ) : "";
    s += message;
    return s;
}

//-------------------------------------------------
NodeSchedule::NodeSchedule()
        : Schedule(),
        m_node( 0 )
{
    //kDebug(planDbg())<<"("<<this<<")";
    init();
}

NodeSchedule::NodeSchedule( Node *node, const QString& name, Schedule::Type type, long id )
        : Schedule( name, type, id ),
        m_node( node )
{
    //kDebug(planDbg())<<"node name:"<<node->name();
    init();
}

NodeSchedule::NodeSchedule( Schedule *parent, Node *node )
        : Schedule( parent ),
        m_node( node )
{

    //kDebug(planDbg())<<"node name:"<<node->name();
    init();
}

NodeSchedule::~NodeSchedule()
{
    //kDebug(planDbg())<<this<<""<<m_appointments.count();
    while ( !m_appointments.isEmpty() ) {
        Appointment *a = m_appointments.takeFirst();
        a->setNode( 0 );
        delete a;
    }
    //kDebug(planDbg())<<"forw"<<m_forward.count();
    while ( !m_forward.isEmpty() ) {
        Appointment *a = m_forward.takeFirst();
        a->setNode( 0 );
        delete a;
    }
    //kDebug(planDbg())<<"backw"<<m_backward.count();
    while ( !m_backward.isEmpty() ) {
        Appointment *a = m_backward.takeFirst();
        a->setNode( 0 );
        delete a;
    }
}

void NodeSchedule::init()
{
    resourceError = false;
    resourceOverbooked = false;
    resourceNotAvailable = false;
    constraintError = false;
    schedulingError = false;
    notScheduled = true;
    inCriticalPath = false;
    m_calculationMode = Schedule::Scheduling;
    positiveFloat = Duration::zeroDuration;
    negativeFloat = Duration::zeroDuration;
    freeFloat = Duration::zeroDuration;
}

void NodeSchedule::setDeleted( bool on )
{
    //kDebug(planDbg())<<"deleted="<<on;
    m_deleted = on;
    // set deleted also for possible resource schedules
    QListIterator<Appointment*> it = m_appointments;
    while ( it.hasNext() ) {
        Appointment * a = it.next();
        if ( a->resource() ) {
            a->resource() ->setDeleted( on );
        }
    }
}

bool NodeSchedule::loadXML( const KoXmlElement &sch, XMLLoaderObject &status )
{
    //kDebug(planDbg());
    QString s;
    Schedule::loadXML( sch, status );
    s = sch.attribute( "earlystart" );
    if ( s.isEmpty() ) { // try version < 0.6
        s = sch.attribute( "earlieststart" );
    }
    if ( !s.isEmpty() ) {
        earlyStart = DateTime::fromString( s, status.projectSpec() );
    }
    s = sch.attribute( "latefinish" );
    if ( s.isEmpty() ) { // try version < 0.6
        s = sch.attribute( "latestfinish" );
    }
    if ( !s.isEmpty() ) {
        lateFinish = DateTime::fromString( s, status.projectSpec() );
    }
    s = sch.attribute( "latestart" );
    if ( !s.isEmpty() ) {
        lateStart = DateTime::fromString( s, status.projectSpec() );
    }
    s = sch.attribute( "earlyfinish" );
    if ( !s.isEmpty() ) {
        earlyFinish = DateTime::fromString( s, status.projectSpec() );
    }
    s = sch.attribute( "start" );
    if ( !s.isEmpty() )
        startTime = DateTime::fromString( s, status.projectSpec() );
    s = sch.attribute( "end" );
    if ( !s.isEmpty() )
        endTime = DateTime::fromString( s, status.projectSpec() );
    s = sch.attribute( "start-work" );
    if ( !s.isEmpty() )
        workStartTime = DateTime::fromString( s, status.projectSpec() );
    s = sch.attribute( "end-work" );
    if ( !s.isEmpty() )
        workEndTime = DateTime::fromString( s, status.projectSpec() );
    duration = Duration::fromString( sch.attribute( "duration" ) );

    inCriticalPath = sch.attribute( "in-critical-path", "0" ).toInt();
    resourceError = sch.attribute( "resource-error", "0" ).toInt();
    resourceOverbooked = sch.attribute( "resource-overbooked", "0" ).toInt();
    resourceNotAvailable = sch.attribute( "resource-not-available", "0" ).toInt();
    constraintError = sch.attribute( "scheduling-conflict", "0" ).toInt();
    schedulingError = sch.attribute( "scheduling-error", "0" ).toInt();
    notScheduled = sch.attribute( "not-scheduled", "1" ).toInt();

    positiveFloat = Duration::fromString( sch.attribute( "positive-float" ) );
    negativeFloat = Duration::fromString( sch.attribute( "negative-float" ) );
    freeFloat = Duration::fromString( sch.attribute( "free-float" ) );

    return true;
}

void NodeSchedule::saveXML( QDomElement &element ) const
{
    //kDebug(planDbg());
    QDomElement sch = element.ownerDocument().createElement( "schedule" );
    element.appendChild( sch );
    saveCommonXML( sch );

    if ( earlyStart.isValid() ) {
        sch.setAttribute( "earlystart", earlyStart.toString( Qt::ISODate ) );
    }
    if ( lateStart.isValid() ) {
        sch.setAttribute( "latestart", lateStart.toString( Qt::ISODate ) );
    }
    if ( earlyFinish.isValid() ) {
        sch.setAttribute( "earlyfinish", earlyFinish.toString( Qt::ISODate ) );
    }
    if ( lateFinish.isValid() ) {
        sch.setAttribute( "latefinish", lateFinish.toString( Qt::ISODate ) );
    }
    if ( startTime.isValid() )
        sch.setAttribute( "start", startTime.toString( Qt::ISODate ) );
    if ( endTime.isValid() )
        sch.setAttribute( "end", endTime.toString( Qt::ISODate ) );
    if ( workStartTime.isValid() )
        sch.setAttribute( "start-work", workStartTime.toString( Qt::ISODate ) );
    if ( workEndTime.isValid() )
        sch.setAttribute( "end-work", workEndTime.toString( Qt::ISODate ) );

    sch.setAttribute( "duration", duration.toString() );

    sch.setAttribute( "in-critical-path", inCriticalPath );
    sch.setAttribute( "resource-error", resourceError );
    sch.setAttribute( "resource-overbooked", resourceOverbooked );
    sch.setAttribute( "resource-not-available", resourceNotAvailable );
    sch.setAttribute( "scheduling-conflict", constraintError );
    sch.setAttribute( "scheduling-error", schedulingError );
    sch.setAttribute( "not-scheduled", notScheduled );

    sch.setAttribute( "positive-float", positiveFloat.toString() );
    sch.setAttribute( "negative-float", negativeFloat.toString() );
    sch.setAttribute( "free-float", freeFloat.toString() );
}

void NodeSchedule::addAppointment( Schedule *resource, const DateTime &start, const DateTime &end, double load )
{
    //kDebug(planDbg());
    Appointment * a = findAppointment( resource, this, m_calculationMode );
    if ( a != 0 ) {
        //kDebug(planDbg())<<"Add interval to existing"<<a;
        a->addInterval( start, end, load );
        return ;
    }
    a = new Appointment( resource, this, start, end, load );
    bool result = add( a );
    Q_ASSERT ( result );
    result = resource->add( a );
    Q_ASSERT ( result );
    Q_UNUSED ( result ); // cheating the compiler in release mode to not warn about unused-but-set-variable
    //kDebug(planDbg())<<"Added interval to new"<<a;
}

void NodeSchedule::takeAppointment( Appointment *appointment, int mode )
{
    Schedule::takeAppointment( appointment, mode );
    appointment->setNode( 0 ); // not my appointment anymore
    //kDebug(planDbg())<<"Taken:"<<appointment;
    if ( appointment->resource() )
        appointment->resource() ->takeAppointment( appointment );
}

QList<Resource*> NodeSchedule::resources() const
{
    QList<Resource*> rl;
    foreach( Appointment *a, m_appointments ) {
        rl += a->resource() ->resource();
    }
    return rl;
}

QStringList NodeSchedule::resourceNameList() const
{
    QStringList rl;
    foreach( Appointment *a, m_appointments ) {
        rl += a->resource() ->resource() ->name();
    }
    return rl;
}

void NodeSchedule::logError( const QString &msg, int phase )
{
    Schedule::Log log( m_node, Log::Type_Error, msg, phase );
    if ( m_parent ) {
        m_parent->addLog( log );
    } else {
        addLog( log );
    }
}

void NodeSchedule::logWarning( const QString &msg, int phase )
{
    Schedule::Log log( m_node, Log::Type_Warning, msg, phase );
    if ( m_parent ) {
        m_parent->addLog( log );
    } else {
        addLog( log );
    }
}

void NodeSchedule::logInfo( const QString &msg, int phase )
{
    Schedule::Log log( m_node, Log::Type_Info, msg, phase );
    if ( m_parent ) {
        m_parent->addLog( log );
    } else {
        addLog( log );
    }
}

void NodeSchedule::logDebug( const QString &msg, int phase )
{
    Schedule::Log log( m_node, Log::Type_Debug, msg, phase );
    if ( m_parent ) {
        m_parent->addLog( log );
    } else {
        addLog( log );
    }
}

//-----------------------------------------------
ResourceSchedule::ResourceSchedule()
        : Schedule(),
        m_resource( 0 )
{
    //kDebug(planDbg())<<"("<<this<<")";
}

ResourceSchedule::ResourceSchedule( Resource *resource, const QString& name, Schedule::Type type, long id )
        : Schedule( name, type, id ),
        m_resource( resource ),
        m_parent( 0 ),
        m_nodeSchedule( 0 )
{
    //kDebug(planDbg())<<"resource:"<<resource->name();
}

ResourceSchedule::ResourceSchedule( Schedule *parent, Resource *resource )
        : Schedule( parent ),
        m_resource( resource ),
        m_parent( parent ),
        m_nodeSchedule( 0 )
{
    //kDebug(planDbg())<<"resource:"<<resource->name();
}

ResourceSchedule::~ResourceSchedule()
{
    //kDebug(planDbg())<<this<<""<<m_appointments.count();
    while ( !m_appointments.isEmpty() ) {
        Appointment *a = m_appointments.takeFirst();
        a->setResource( 0 );
        delete a;
    }
    //kDebug(planDbg())<<"forw"<<m_forward.count();
    while ( !m_forward.isEmpty() ) {
        Appointment *a = m_forward.takeFirst();
        a->setResource( 0 );
        delete a;
    }
    //kDebug(planDbg())<<"backw"<<m_backward.count();
    while ( !m_backward.isEmpty() ) {
        Appointment *a = m_backward.takeFirst();
        a->setResource( 0 );
        delete a;
    }
}

// called from the resource
void ResourceSchedule::addAppointment( Schedule *node, const DateTime &start, const DateTime &end, double load )
{
    Q_ASSERT( start < end );
    //kDebug(planDbg())<<"("<<this<<")"<<node<<","<<m_calculationMode;
    Appointment * a = findAppointment( this, node, m_calculationMode );
    if ( a != 0 ) {
        //kDebug(planDbg())<<"Add interval to existing"<<a;
        a->addInterval( start, end, load );
        return ;
    }
    a = new Appointment( this, node, start, end, load );
    bool result = add( a );
    Q_ASSERT ( result == true );
    result = node->add( a );
    Q_ASSERT ( result == true );
    Q_UNUSED ( result );  //don't warn about unused-but-set-variable in release mode
    //kDebug(planDbg())<<"Added interval to new"<<a;
}

void ResourceSchedule::takeAppointment( Appointment *appointment, int mode )
{
    Schedule::takeAppointment( appointment, mode );
    appointment->setResource( 0 );
    //kDebug(planDbg())<<"Taken:"<<appointment;
    if ( appointment->node() )
        appointment->node() ->takeAppointment( appointment );
}

bool ResourceSchedule::isOverbooked() const
{
    return false;
}

bool ResourceSchedule::isOverbooked( const DateTime &start, const DateTime &end ) const
{
    if ( m_resource == 0 )
        return false;
    //kDebug(planDbg())<<start.toString()<<" -"<<end.toString();
    Appointment a = appointmentIntervals();
    foreach ( const AppointmentInterval &i, a.intervals().map() ) {
        if ( ( !end.isValid() || i.startTime() < end ) &&
                ( !start.isValid() || i.endTime() > start ) ) {
            if ( i.load() > m_resource->units() ) {
                //kDebug(planDbg())<<m_name<<" overbooked";
                return true;
            }
        }
        if ( i.startTime() >= end )
            break;
    }
    //kDebug(planDbg())<<m_name<<" not overbooked";
    return false;
}

double ResourceSchedule::normalRatePrHour() const
{
    return m_resource ? m_resource->normalRate() : 0.0;
}

//TODO change to return the booked effort
Duration ResourceSchedule::effort( const DateTimeInterval &interval ) const
{
    Duration eff = interval.second - interval.first;
    if ( allowOverbooking() ) {
        return eff;
    }
    Appointment a;
    if ( checkExternalAppointments() ) {
        a.setIntervals( m_resource->externalAppointments() );
    }
    a.merge( appointmentIntervals( m_calculationMode ) );
    if ( a.isEmpty() || a.startTime() >= interval.second || a.endTime() <= interval.first ) {
        return eff;
    }
    foreach ( const AppointmentInterval &i, a.intervals().map() ) {
        if ( interval.second <= i.startTime() ) {
            break;
        }
        if ( interval.first >= i.startTime() ) {
            DateTime et = i.endTime() < interval.second ? i.endTime() : interval.second;
            eff -= ( et - interval.first ) * ((double)i.load()/100.0 );
        } else {
            DateTime et = i.endTime() < interval.second ? i.endTime() : interval.second;
            eff -= ( et - i.startTime() ) * ((double)i.load()/100.0 );
        }
    }
    return eff;
}

DateTimeInterval ResourceSchedule::available( const DateTimeInterval &interval ) const
{
    //const_cast<ResourceSchedule*>(this)->logDebug( QString( "Schedule available id=%1, Mode=%2: interval=%3 - %4" ).arg(m_id).arg(m_calculationMode).arg(interval.first.toString()).arg(interval.second.toString()) );
    if ( allowOverbooking() ) {
        return DateTimeInterval( interval.first, interval.second );
    }
    Appointment a;
    if ( checkExternalAppointments() ) {
        a.setIntervals( m_resource->externalAppointments( interval ) );
    }
    a.merge( appointmentIntervals( m_calculationMode, interval ) );
    if ( a.isEmpty() || a.startTime() >= interval.second || a.endTime() <= interval.first ) {
        //kDebug(planDbg())<<this<<"id="<<m_id<<"Mode="<<m_calculationMode<<""<<interval.first<<","<<interval.second<<" FREE";
        return DateTimeInterval( interval.first, interval.second );
    }
    //kDebug(planDbg())<<"available:"<<interval<<endl<<a.intervals();
    DateTimeInterval res;
    DateTimeInterval ci = interval;
    int units = m_resource ? m_resource->units() : 100;
    foreach ( const AppointmentInterval &i, a.intervals().map() ) {
        //const_cast<ResourceSchedule*>(this)->logDebug( QString( "Schedule available check interval=%1 - %2" ).arg(i.startTime().toString()).arg(i.endTime().toString()) );
        if ( i.startTime() < ci.second && i.endTime() > ci.first ) {
            // interval intersects appointment
            if ( ci.first >= i.startTime() && ci.second <= i.endTime() ) {
                // interval within appointment
                if ( i.load() < units ) {
                    if ( ! res.first.isValid() ) {
                        res.first = qMax( ci.first, i.startTime() );
                    }
                    res.second = qMin( ci.second, i.endTime() );
                } else {
                    // fully booked
                    if ( res.first.isValid() ) {
                        res.second = i.startTime();
                        if ( res.first >= res.second ) {
                            res = DateTimeInterval();
                        }
                    }
                }
                //kDebug(planDbg())<<"available within:"<<interval<<i<<":"<<ci<<res;
                break;
            }
            DateTime t = i.startTime();
            if ( ci.first < t ) {
                // Interval starts before appointment, so free from interval start
                //kDebug(planDbg())<<"available before:"<<interval<<i<<":"<<ci<<res;
                //const_cast<ResourceSchedule*>(this)->logDebug( QString( "Schedule available t>first: returns interval=%1 - %2" ).arg(ci.first.toString()).arg(t.toString()) );
                if ( ! res.first.isValid() ) {
                    res.first = ci.first;
                }
                res.second = t;
                if ( i.load() < units ) {
                    res.second = qMin( ci.second, i.endTime() );
                    if ( ci.second > i.endTime() ) {
                        ci.first = i.endTime();
                        //kDebug(planDbg())<<"available next 1:"<<interval<<i<<":"<<ci<<res;
                        continue; // check next appointment
                    }
                }
                //kDebug(planDbg())<<"available:"<<interval<<i<<":"<<ci<<res;
                break;
            }
            // interval start >= appointment start
            t = i.endTime();
            if ( t < ci.second ) {
                // check if rest of appointment is free
                if ( units <= i.load() ) {
                    ci.first = t; // fully booked, so move forvard to appointment end
                }
                res = ci;
                //kDebug(planDbg())<<"available next 2:"<<interval<<i<<":"<<ci<<res;
                continue;
            }
            //kDebug(planDbg())<<"available:"<<interval<<i<<":"<<ci<<res;
            Q_ASSERT( false );
        } else if ( i.startTime() >= interval.second ) {
            // no more overlaps
            break;
        }
    }
    //kDebug(planDbg())<<"available: result="<<interval<<":"<<res;
    return res;
}

void ResourceSchedule::logError( const QString &msg, int phase )
{
    if ( m_parent ) {
        Schedule::Log log( m_nodeSchedule ? m_nodeSchedule->node() : 0, m_resource, Log::Type_Error, msg, phase );
        m_parent->addLog( log );
    }
}

void ResourceSchedule::logWarning( const QString &msg, int phase )
{
    if ( m_parent ) {
        Schedule::Log log( m_nodeSchedule ? m_nodeSchedule->node() : 0, m_resource, Log::Type_Warning, msg, phase );
        m_parent->addLog( log );
    }
}

void ResourceSchedule::logInfo( const QString &msg, int phase )
{
    if ( m_parent ) {
        Schedule::Log log( m_nodeSchedule ? m_nodeSchedule->node() : 0, m_resource, Log::Type_Info, msg, phase );
        m_parent->addLog( log );
    }
}

void ResourceSchedule::logDebug( const QString &msg, int phase )
{
    if ( m_parent ) {
        Schedule::Log log( m_nodeSchedule ? m_nodeSchedule->node() : 0, m_resource, Log::Type_Debug, msg, phase );
        m_parent->addLog( log );
    }
}

//--------------------------------------
MainSchedule::MainSchedule()
    : NodeSchedule(),

    m_manager( 0 )
{
    //kDebug(planDbg())<<"("<<this<<")";
    init();
}

MainSchedule::MainSchedule( Node *node, const QString& name, Schedule::Type type, long id )
    : NodeSchedule( node, name, type, id ),
      criticalPathListCached( false ),
      m_manager( 0 ),
      m_currentCriticalPath( 0 )
{
    //kDebug(planDbg())<<"node name:"<<node->name();
    init();
}

MainSchedule::~MainSchedule()
{
    //kDebug(planDbg())<<"("<<this<<")";
}

void MainSchedule::incProgress()
{
    if ( m_manager ) m_manager->incProgress();
}

bool MainSchedule::isBaselined() const
{
    return m_manager == 0 ? false : m_manager->isBaselined();
}

bool MainSchedule::usePert() const
{
    return m_manager == 0 ? false : m_manager->usePert();
}

bool MainSchedule::allowOverbooking() const
{
    return m_manager == 0 ? false : m_manager->allowOverbooking();
}

bool MainSchedule::checkExternalAppointments() const
{
    return m_manager == 0 ? false : m_manager->checkExternalAppointments();
}

void MainSchedule::changed( Schedule *sch )
{
    if ( m_manager ) {
        m_manager->scheduleChanged( static_cast<MainSchedule*>( sch ) );
    }
}

bool MainSchedule::loadXML( const KoXmlElement &sch, XMLLoaderObject &status )
{
    //kDebug(planDbg());
    QString s;
    Schedule::loadXML( sch, status );

    s = sch.attribute( "start" );
    if ( !s.isEmpty() )
        startTime = DateTime::fromString( s, status.projectSpec() );
    s = sch.attribute( "end" );
    if ( !s.isEmpty() )
        endTime = DateTime::fromString( s, status.projectSpec() );

    duration = Duration::fromString( sch.attribute( "duration" ) );
    constraintError = sch.attribute( "scheduling-conflict", "0" ).toInt();
    schedulingError = sch.attribute( "scheduling-error", "0" ).toInt();
    //NOTE: we use "scheduled" as default to match old format without "not-scheduled" element
    notScheduled = sch.attribute( "not-scheduled", "0" ).toInt();

    KoXmlNode n = sch.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement el = n.toElement();
        if ( el.tagName() == "appointment" ) {
            // Load the appointments.
            // Resources and tasks must already be loaded
            Appointment * child = new Appointment();
            if ( !child->loadXML( el, status, *this ) ) {
                // TODO: Complain about this
                kError() << "Failed to load appointment" << endl;
                delete child;
            }
        } else if ( el.tagName() == "criticalpath-list" ) {
            // Tasks must already be loaded
            for ( KoXmlNode n1 = el.firstChild(); ! n1.isNull(); n1 = n1.nextSibling() ) {
                if ( ! n1.isElement() ) {
                    continue;
                }
                KoXmlElement e1 = n1.toElement();
                if ( e1.tagName() != "criticalpath" ) {
                    continue;
                }
                QList<Node*> lst;
                for ( KoXmlNode n2 = e1.firstChild(); ! n2.isNull(); n2 = n2.nextSibling() ) {
                    if ( ! n2.isElement() ) {
                        continue;
                    }
                    KoXmlElement e2 = n2.toElement();
                    if ( e2.tagName() != "node" ) {
                        continue;
                    }
                    QString s = e2.attribute( "id" );
                    Node *node = status.project().findNode( s );
                    if ( node ) {
                        lst.append( node );
                    } else {
                        kError()<<"Failed to find node id="<<s;
                    }
                }
                m_pathlists.append( lst );
            }
            criticalPathListCached = true;
        }
    }
    return true;
}

void MainSchedule::saveXML( QDomElement &element ) const
{
    saveCommonXML( element );

    element.setAttribute( "start", startTime.toString( Qt::ISODate ) );
    element.setAttribute( "end", endTime.toString( Qt::ISODate ) );
    element.setAttribute( "duration", duration.toString() );
    element.setAttribute( "scheduling-conflict", constraintError );
    element.setAttribute( "scheduling-error", schedulingError );
    element.setAttribute( "not-scheduled", notScheduled );

    if ( ! m_pathlists.isEmpty() ) {
        QDomElement lists = element.ownerDocument().createElement( "criticalpath-list" );
        element.appendChild( lists );
        foreach ( QList<Node*> l, m_pathlists ) {
            if ( l.isEmpty() ) {
                continue;
            }
            QDomElement list = lists.ownerDocument().createElement( "criticalpath" );
            lists.appendChild( list );
            foreach ( Node *n, l ) {
                QDomElement el = list.ownerDocument().createElement( "node" );
                list.appendChild( el );
                el.setAttribute( "id", n->id() );
            }
        }
    }
}

DateTime MainSchedule::calculateForward( int use )
{
    DateTime late;
    foreach( Node *n, m_backwardnodes ) {
        DateTime t = n->calculateForward( use );
        if ( !late.isValid() || late < t ) {
            late = t;
        }
    }
    return late;
}

DateTime MainSchedule::calculateBackward( int use )
{
    DateTime early;
    foreach( Node *n, m_forwardnodes ) {
        DateTime t = n->calculateBackward( use );
        if ( !early.isValid() || early > t ) {
            early = t;
        }
    }
    return early;
}

DateTime MainSchedule::scheduleForward( const DateTime &earliest, int use )
{
    DateTime end;
    foreach( Node *n, m_forwardnodes ) {
        DateTime t = n->scheduleForward( earliest, use );
        if ( !end.isValid() || end < t ) {
            end = t;
        }
    }
    return end;
}

DateTime MainSchedule::scheduleBackward( const DateTime &latest, int use )
{
    DateTime start;
    foreach( Node *n, m_backwardnodes ) {
        DateTime t = n->scheduleBackward( latest, use );
        if ( !start.isValid() || start > t ) {
            start = t;
        }
    }
    return start;
}

bool MainSchedule::recalculate() const
{
    return m_manager == 0 ? false : m_manager->recalculate();
}

DateTime MainSchedule::recalculateFrom() const
{
    return m_manager == 0 ? DateTime() : m_manager->recalculateFrom();
}

long MainSchedule::parentScheduleId() const
{
    return m_manager == 0 ? -2 : m_manager->parentScheduleId();
}

void MainSchedule::clearCriticalPathList()
{
    m_pathlists.clear();
    m_currentCriticalPath = 0;
    criticalPathListCached = false;
}

QList<Node*> *MainSchedule::currentCriticalPath() const
{
    return m_currentCriticalPath;
}

void MainSchedule::addCriticalPath( QList<Node*> *lst )
{
    QList<Node*> l;
    if ( lst ) {
        l = *lst;
    }
    m_pathlists.append( l );
    m_currentCriticalPath = &( m_pathlists.last() );
}

void MainSchedule::addCriticalPathNode( Node *node )
{
    if ( m_currentCriticalPath == 0 ) {
        kError()<<"No currentCriticalPath"<<endl;
        return;
    }
    m_currentCriticalPath->append( node );
}

QList<Schedule::Log> MainSchedule::logs() const
{
    return m_log;
}

void MainSchedule::addLog( KPlato::Schedule::Log &log )
{
    Q_ASSERT( log.resource || log.node );
#ifndef NDEBUG
    if ( log.resource ) {
        Q_ASSERT( manager()->project().findResource( log.resource->id() ) == log.resource );
    } else if ( log.node ) {
        Q_ASSERT( manager()->project().findNode( log.node->id() ) == log.node );
    }
#endif
    if ( log.phase == -1 && ! m_log.isEmpty() ) {
        log.phase = m_log.last().phase;
    }
    m_log.append( log );
    if ( m_manager ) {
        m_manager->logAdded( log );
    }
}

//static
QString MainSchedule::logSeverity( int severity )
{
    switch ( severity ) {
        case Log::Type_Debug: return "Debug";//FIXME i18n( "Debug" );
        case Log::Type_Info: return i18n( "Info" );
        case Log::Type_Warning: return i18n( "Warning" );
        case Log::Type_Error: return i18n( "Error" );
        default: break;
    }
    return QString( "Severity %1" ).arg( severity );
}

QStringList MainSchedule::logMessages() const
{
    QStringList lst;
    foreach ( const Schedule::Log &l, m_log ) {
        lst << l.formatMsg();
    }
    return lst;
}

//-----------------------------------------
ScheduleManager::ScheduleManager( Project &project, const QString name )
    : m_project( project),
    m_parent( 0 ),
    m_name( name ),
    m_baselined( false ),
    m_allowOverbooking( false ),
    m_checkExternalAppointments( true ),
    m_usePert( false ),
    m_recalculate( false ),
    m_schedulingDirection( false ),
    m_scheduling( false ),
    m_progress( 0 ),
    m_maxprogress( 0 ),
    m_expected( 0 )
{
    //kDebug(planDbg())<<name;
}

ScheduleManager::~ScheduleManager()
{
    qDeleteAll( m_children );
    setParentManager( 0 );
}

void ScheduleManager::setParentManager( ScheduleManager *sm, int index )
{
    if ( m_parent ) {
        m_parent->removeChild( this );
    }
    m_parent = sm;
    if ( sm ) {
        sm->insertChild( this, index );
    }
}

int ScheduleManager::removeChild( const ScheduleManager *sm )
{
    int i = m_children.indexOf( const_cast<ScheduleManager*>( sm ) );
    if ( i != -1 ) {
        m_children.removeAt( i );
    }
    return i;
}

void ScheduleManager::insertChild( ScheduleManager *sm, int index )
{
    //kDebug(planDbg())<<m_name<<", insert"<<sm->name()<<","<<index;
    if ( index == -1 ) {
        m_children.append( sm );
    } else {
        m_children.insert( index, sm );
    }
}

void ScheduleManager::createSchedules()
{
    setExpected( m_project.createSchedule( m_name, Schedule::Expected ) );
}

int ScheduleManager::indexOf( const ScheduleManager *child ) const
{
    //kDebug(planDbg())<<this<<","<<child;
    return m_children.indexOf( const_cast<ScheduleManager*>( child ) );
}

ScheduleManager *ScheduleManager::findManager( const QString& name ) const
{
    if ( m_name == name ) {
        return const_cast<ScheduleManager*>( this );
    }
    foreach ( ScheduleManager *sm, m_children ) {
        ScheduleManager *m = sm->findManager( name );
        if ( m ) {
            return m;
        }
    }
    return 0;
}

QList<ScheduleManager*> ScheduleManager::allChildren() const
{
    QList<ScheduleManager*> lst;
    foreach ( ScheduleManager *sm, m_children ) {
        lst << sm;
        lst << sm->allChildren();
    }
    return lst;
}

bool ScheduleManager::isParentOf( const ScheduleManager *sm ) const
{
    if ( indexOf( sm ) >= 0 ) {
        return true;
    }
    foreach ( ScheduleManager *p, m_children ) {
        if ( p->isParentOf( sm ) ) {
            return true;
        }
    }
    return false;
}

void ScheduleManager::setName( const QString& name )
{
    m_name = name;
#ifndef NDEBUG
    setObjectName( name );
#endif
    if ( m_expected ) {
        m_expected->setName( name );
        m_project.changed( m_expected );
    }
    m_project.changed( this );
}

bool ScheduleManager::isChildBaselined() const
{
    //kDebug(planDbg())<<on;
    foreach ( ScheduleManager *sm, m_children ) {
        if ( sm->isBaselined() || sm->isChildBaselined() ) {
            return true;
        }
    }
    return false;
}

void ScheduleManager::setBaselined( bool on )
{
    //kDebug(planDbg())<<on;
    m_baselined = on;
    m_project.changed( this );
}

void ScheduleManager::setAllowOverbooking( bool on )
{
    //kDebug(planDbg())<<on;
    m_allowOverbooking = on;
    m_project.changed( this );
}

bool ScheduleManager::allowOverbooking() const
{
    //kDebug(planDbg())<<m_name<<"="<<m_allowOverbooking;
    return m_allowOverbooking;
}

bool ScheduleManager::checkExternalAppointments() const
{
    //kDebug(planDbg())<<m_name<<"="<<m_allowOverbooking;
    return m_checkExternalAppointments;
}

void ScheduleManager::setCheckExternalAppointments( bool on )
{
    //kDebug(planDbg())<<m_name<<"="<<m_checkExternalAppointments;
    m_checkExternalAppointments = on;
}

void ScheduleManager::scheduleChanged( MainSchedule *sch )
{
    m_project.changed( sch );
    m_project.changed( this ); //hmmm, due to aggregated info
}

void ScheduleManager::setUsePert( bool on )
{
    m_usePert = on;
    m_project.changed( this );
}

void ScheduleManager::setSchedulingDirection( bool on )
{
    //kDebug(planDbg())<<on;
    m_schedulingDirection = on;
    m_project.changed( this );
}

void ScheduleManager::setScheduling( bool on )
{
    m_scheduling = on;
    if ( ! on ) {
        m_project.setProgress( 0, this );
    }
    m_project.changed( this );
}

const QList<SchedulerPlugin*> ScheduleManager::schedulerPlugins() const
{
    return m_project.schedulerPlugins().values();
}

QString ScheduleManager::schedulerPluginId() const
{
    return m_schedulerPluginId;
}

void ScheduleManager::setSchedulerPluginId( const QString &id )
{
    m_schedulerPluginId = id;
    m_project.changed( this );
}

SchedulerPlugin *ScheduleManager::schedulerPlugin() const
{
    if ( m_schedulerPluginId.isEmpty() ) {
        // try to avoid crash
        return m_project.schedulerPlugins().value( m_project.schedulerPlugins().keys().value( 0 ) );
    }
    return m_project.schedulerPlugins().value( m_schedulerPluginId );
}

QStringList ScheduleManager::schedulerPluginNames() const
{
    QStringList lst;
    QMap<QString, SchedulerPlugin*>::const_iterator it = m_project.schedulerPlugins().constBegin();
    QMap<QString, SchedulerPlugin*>::const_iterator end = m_project.schedulerPlugins().constEnd();
    for ( ; it != end; ++it ) {
        lst << it.value()->name();
    }
    return lst;
}

int ScheduleManager::schedulerPluginIndex() const
{
    if ( m_schedulerPluginId.isEmpty() ) {
        return 0;
    }
    return m_project.schedulerPlugins().keys().indexOf( m_schedulerPluginId );
}

void ScheduleManager::setSchedulerPlugin( int index )
{
    if ( schedulerPlugin() ) {
        schedulerPlugin()->stopCalculation( this ); // in case...
    }

    m_schedulerPluginId = m_project.schedulerPlugins().keys().value( index );
    kDebug(planDbg())<<index<<m_schedulerPluginId;
    m_project.changed( this );
}

void ScheduleManager::calculateSchedule()
{
    m_calculationresult = CalculationRunning;
    if ( schedulerPlugin() ) {
        schedulerPlugin()->calculate( m_project, this );
    }
}

void ScheduleManager::stopCalculation()
{
    if ( schedulerPlugin() ) {
        schedulerPlugin()->stopCalculation( this );
    }
}

void ScheduleManager::haltCalculation()
{
    if ( schedulerPlugin() ) {
        schedulerPlugin()->haltCalculation( this );
    }
}

void ScheduleManager::setMaxProgress( int value )
{
    m_maxprogress = value;
    emit maxProgressChanged( value );
    m_project.changed( this );
}

void ScheduleManager::setProgress( int value )
{
    m_progress = value;
    emit progressChanged( value );
    m_project.changed( this );
}

void ScheduleManager::setDeleted( bool on )
{
    if ( m_expected ) {
        m_expected->setDeleted( on );
    }
    m_project.changed( this );
}

void ScheduleManager::setExpected( MainSchedule *sch )
{
    //kDebug(planDbg())<<m_expected<<","<<sch;
    if ( m_expected ) {
        m_project.sendScheduleToBeRemoved( m_expected );
        m_expected->setDeleted( true );
        m_project.sendScheduleRemoved( m_expected );
    }
    m_expected = sch;
    if ( sch ) {
        m_project.sendScheduleToBeAdded( this, 0 );
        sch->setManager( this );
        m_expected->setDeleted( false );
        m_project.sendScheduleAdded( sch );
    }
    m_project.changed( this );
}

QStringList ScheduleManager::state() const
{
    QStringList lst;
    if ( isBaselined() ) {
        return lst << i18n( "Baselined" );
    }
    if ( m_scheduling ) {
        return lst << i18n( "Scheduling" );
    }
    if ( m_expected == 0 ) {
        return lst << i18n( "Not scheduled" );
    }
    if ( Schedule *s = m_expected ) {
        if ( s->resourceError || s->resourceOverbooked || s->resourceNotAvailable || s->constraintError || s->schedulingError ) {
            return lst << i18n( "Error" );
        }
        return s->state();
    }
    return lst;
}

void ScheduleManager::incProgress()
{
    m_project.incProgress();
}

void ScheduleManager::logAdded( Schedule::Log &log )
{
    emit sigLogAdded( log );
    int row = expected()->logs().count() - 1;
    emit logInserted( expected(), row, row );
}

void ScheduleManager::slotAddLog( const QList<KPlato::Schedule::Log> &log )
{
    if ( expected() && ! log.isEmpty() ) {
        int first = expected()->logs().count();
        int last = first + log.count() - 1;
        Q_UNUSED(last);
        foreach ( KPlato::Schedule::Log l, log ) {
            expected()->addLog( l );
        }
    }
}

QMap< int, QString > ScheduleManager::phaseNames() const
{
    if ( expected() ) {
        return expected()->phaseNames();
    }
    return QMap<int, QString>();
}

void ScheduleManager::setPhaseNames( const QMap<int, QString> &phasenames )
{
    if ( expected() ) {
        expected()->setPhaseNames( phasenames );
    }
}


bool ScheduleManager::loadXML( KoXmlElement &element, XMLLoaderObject &status )
{
    MainSchedule *sch = 0;
    if ( status.version() <= "0.5" ) {
        m_usePert = false;
        sch = loadMainSchedule( element, status );
        if ( sch ) {
            sch->setManager( this );
            switch ( sch->type() ) {
                case Schedule::Expected: setExpected( sch ); break;
            }
        }
        return true;
    }
    setName( element.attribute( "name" ) );
    m_id = element.attribute( "id" );
    m_usePert = (element.attribute( "distribution" ).toInt()) == 1;
    m_allowOverbooking = (bool)(element.attribute( "overbooking" ).toInt());
    m_checkExternalAppointments = (bool)(element.attribute( "check-external-appointments" ).toInt());
    m_schedulingDirection = (bool)(element.attribute( "scheduling-direction" ).toInt());
    m_baselined = (bool)(element.attribute( "baselined" ).toInt());
    m_schedulerPluginId = element.attribute( "scheduler-plugin-id" );
    m_recalculate = (bool)(element.attribute( "recalculate" ).toInt());
    m_recalculateFrom = DateTime::fromString( element.attribute( "recalculate-from" ), status.projectSpec() );
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        //kDebug(planDbg())<<e.tagName();
        if ( e.tagName() == "schedule" ) {
            sch = loadMainSchedule( e, status );
            if ( sch ) {
                sch->setManager( this );
                switch ( sch->type() ) {
                    case Schedule::Expected: setExpected( sch ); break;
                }
            }
        } else if ( e.tagName() == "plan" ) {
            ScheduleManager *sm = new ScheduleManager( status.project() );
            if ( sm->loadXML( e, status ) ) {
                m_project.addScheduleManager( sm, this );
            } else {
                kError()<<"Failed to load schedule manager"<<endl;
                delete sm;
            }
        }
    }
    return true;
}

MainSchedule *ScheduleManager::loadMainSchedule( KoXmlElement &element, XMLLoaderObject &status ) {
    MainSchedule *sch = new MainSchedule();
    if ( sch->loadXML( element, status ) ) {
        status.project().addSchedule( sch );
        sch->setNode( &(status.project()) );
        status.project().setParentSchedule( sch );
    } else {
        kError() << "Failed to load schedule" << endl;
        delete sch;
        sch = 0;
    }
    return sch;
}

bool ScheduleManager::loadMainSchedule( MainSchedule *schedule, KoXmlElement &element, XMLLoaderObject &status ) {
    long sid = schedule->id();
    if ( schedule->loadXML( element, status ) ) {
        if ( sid != schedule->id() && status.project().findSchedule( sid ) ) {
            status.project().takeSchedule( schedule );
        }
        if ( ! status.project().findSchedule( schedule->id() ) ) {
            status.project().addSchedule( schedule );
        }
        schedule->setNode( &(status.project()) );
        status.project().setParentSchedule( schedule );
        return true;
    }
    return false;
}

void ScheduleManager::saveXML( QDomElement &element ) const
{
    QDomElement el = element.ownerDocument().createElement( "plan" );
    element.appendChild( el );
    el.setAttribute( "name", m_name );
    el.setAttribute( "id", m_id );
    el.setAttribute( "distribution", m_usePert ? 1 : 0 );
    el.setAttribute( "overbooking", m_allowOverbooking );
    el.setAttribute( "check-external-appointments", m_checkExternalAppointments );
    el.setAttribute( "scheduling-direction", m_schedulingDirection );
    el.setAttribute( "baselined", m_baselined );
    el.setAttribute( "scheduler-plugin-id", m_schedulerPluginId );
    el.setAttribute( "recalculate", m_recalculate );
    el.setAttribute( "recalculate-from", m_recalculateFrom.toString( Qt::ISODate ) );
    if ( m_expected && ! m_expected->isDeleted() ) {
        QDomElement schs = el.ownerDocument().createElement( "schedule" );
        el.appendChild( schs );
        m_expected->saveXML( schs );
        m_project.saveAppointments( schs, m_expected->id() );
    }
    foreach ( ScheduleManager *sm, m_children ) {
        sm->saveXML( el );
    }

}

void ScheduleManager::saveWorkPackageXML( QDomElement &element, const Node &node ) const
{
    QDomElement el = element.ownerDocument().createElement( "plan" );
    element.appendChild( el );
    el.setAttribute( "name", m_name );
    el.setAttribute( "id", m_id );
    el.setAttribute( "distribution", m_usePert ? 1 : 0 );
    el.setAttribute( "overbooking", m_allowOverbooking );
    el.setAttribute( "check-external-appointments", m_checkExternalAppointments );
    el.setAttribute( "scheduling-direction", m_schedulingDirection );
    el.setAttribute( "baselined", m_baselined );
    if ( m_expected && ! m_expected->isDeleted() ) { // TODO: should we check isScheduled() ?
        QDomElement schs = el.ownerDocument().createElement( "schedule" );
        el.appendChild( schs );
        m_expected->saveXML( schs );
        Schedule *s = node.findSchedule( m_expected->id() );
        if ( s && ! s->isDeleted() ) {
            s->saveAppointments( schs );
        }
    }
}


} //namespace KPlato

QDebug operator<<( QDebug dbg, const KPlato::Schedule::Log &log )
{
    dbg.nospace()<<"Schedule::Log: "<<log.formatMsg();
    return dbg.space();
}

#include "kptschedule.moc"
