/* This file is part of the KDE project
 Copyright (C) 2005 - 2007 Dag Andersen <danders@get2net.dk>

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

#include <KoXmlReader.h>

#include <QDomElement>
#include <QList>
#include <QString>
#include <QStringList>

#include <klocale.h>
#include <kdebug.h>

namespace KPlato
{

class ScheduleManager;
    
Schedule::Schedule()
        : m_type( Expected ),
        m_id( 0 ),
        m_deleted( false ),
        m_parent( 0 ),
        m_calculationMode( Schedule::Scheduling )
{}

Schedule::Schedule( Schedule *parent )
        : m_type( Expected ),
        m_id( 0 ),
        m_deleted( false ),
        m_parent( parent ),
        m_calculationMode( Schedule::Scheduling )
{

    if ( parent ) {
        m_name = parent->name();
        m_type = parent->type();
        m_id = parent->id();
    }
    //kDebug()<<"("<<this<<") Name: '"<<name<<"' Type="<<type<<" id="<<id;
}

Schedule::Schedule( const QString& name, Type type, long id )
        : m_name( name ),
        m_type( type ),
        m_id( id ),
        m_deleted( false ),
        m_parent( 0 ),
        m_calculationMode( Schedule::Scheduling )
{

    //kDebug()<<"("<<this<<") Name: '"<<name<<"' Type="<<type<<" id="<<id;
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
    //kDebug()<<"deleted="<<on;
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
    else if ( type == "Optimistic" )
        m_type = Optimistic;
    else if ( type == "Pessimistic" )
        m_type = Pessimistic;
}

QString Schedule::typeToString( bool translate ) const
{
    if ( translate ) {
        if ( m_type == Expected )
            return i18n( "Expected" );
        if ( m_type == Optimistic )
            return i18n( "Optimistic" );
        if ( m_type == Pessimistic )
            return i18n( "Pessimistic" );
        return i18n( "Expected" );
    } else {
        if ( m_type == Expected )
            return "Expected";
        if ( m_type == Optimistic )
            return "Optimistic";
        if ( m_type == Pessimistic )
            return "Pessimistic";
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
    if ( schedulingError )
        lst << SchedulingState::constraintsNotMet();
    if ( resourceError )
        lst << SchedulingState::resourceNotAllocated();
    if ( resourceNotAvailable )
        lst << SchedulingState::resourceNotAvailable();
    if ( resourceOverbooked )
        lst << SchedulingState::resourceOverbooked();
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

void Schedule::setAllowOverbooking( bool state )
{
    if ( m_parent )
        m_parent->setAllowOverbooking( state );
}

bool Schedule::allowOverbooking() const
{
    if ( m_parent ) {
        return m_parent->allowOverbooking();
    }
    return false;
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
    schedulingError = false;
    inCriticalPath = false;
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
            return DateTimeInterval( i.values().first().startTime(), i.values().first().endTime() );
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
    //kDebug()<<m_name<<" save schedule";
    element.setAttribute( "name", m_name );
    element.setAttribute( "type", typeToString() );
    element.setAttribute( "id", qlonglong( m_id ) );
}

void Schedule::saveAppointments( QDomElement &element ) const
{
    //kDebug();
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
    //kDebug()<<appointment<<mode;
    if ( mode == Scheduling ) {
        if ( m_appointments.indexOf( appointment ) != -1 ) {
            kError() << "Appointment already exists" << endl;
            return false;
        }
        m_appointments.append( appointment );
        //if (resource()) kDebug()<<appointment<<" For resource '"<<resource()->name()<<"'"<<" count="<<m_appointments.count();
        //if (node()) kDebug()<<"("<<this<<")"<<appointment<<" For node '"<<node()->name()<<"'"<<" count="<<m_appointments.count();
        return true;
    }
    if ( mode == CalculateForward ) {
        if ( m_forward.indexOf( appointment ) != -1 ) {
            kError() << "Appointment already exists" << endl;
            return false;
        }
        m_forward.append( appointment );
        //if (resource()) kDebug()<<"For resource '"<<resource()->name()<<"'";
        //if (node()) kDebug()<<"For node '"<<node()->name()<<"'";
        return true;
    }
    if ( mode == CalculateBackward ) {
        if ( m_backward.indexOf( appointment ) != -1 ) {
            kError() << "Appointment already exists" << endl;
            return false;
        }
        m_backward.append( appointment );
        //if (resource()) kDebug()<<"For resource '"<<resource()->name()<<"'";
        //if (node()) kDebug()<<"For node '"<<node()->name()<<"'";
        return true;
    }
    kError()<<"Unknown mode: "<<m_calculationMode<<endl;
    return false;
}

// used to add new schedules
bool Schedule::add( Appointment *appointment )
{
    //kDebug()<<this;
    appointment->setCalculationMode( m_calculationMode );
    return attatch( appointment );
}

void Schedule::takeAppointment( Appointment *appointment, int mode )
{
    Q_UNUSED(mode);
    //kDebug()<<"("<<this<<")"<<mode<<":"<<appointment<<","<<appointment->calculationMode();
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
    //kDebug()<<this<<" ("<<resourceError<<","<<node<<")"<<mode;
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

QList<Appointment*> &Schedule::appointments( int which)
{
    if ( which == CalculateForward ) {
        return m_forward;
    } else if ( which == CalculateBackward ) {
        return m_backward;
    }
    return appointments();
}

Appointment Schedule::appointmentIntervals( int which ) const
{
    Appointment app;
    if ( which == Schedule::CalculateForward ) {
        //kDebug()<<"list == CalculateForward";
        foreach ( Appointment *a, m_forward ) {
            app += *a;
        }
        return app;
    } else if ( which == Schedule::CalculateBackward ) {
        //kDebug()<<"list == CalculateBackward";
        foreach ( Appointment *a, m_backward ) {
            app += *a;
        }
        //kDebug()<<"list == CalculateBackward:"<<m_backward.count();
        return app;
    }
    foreach ( Appointment *a, m_appointments ) {
        app += *a;
    }
    return app;
}


EffortCostMap Schedule::bcwsPrDay() const
{
    //kDebug()<<m_name<<m_appointments;
    EffortCostMap ec;
    foreach ( Appointment *a, m_appointments ) {
        ec += a->plannedPrDay( a->startTime().date(), a->endTime().date() );
    }
    return ec;
}

EffortCostMap Schedule::plannedEffortCostPrDay( const QDate &start, const QDate &end ) const
{
    //kDebug()<<m_name<<m_appointments;
    EffortCostMap ec;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        //kDebug()<<m_name;
        ec += it.next() ->plannedPrDay( start, end );
    }
    return ec;
}

Duration Schedule::plannedEffort() const
{
    //kDebug();
    Duration eff;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        eff += it.next() ->plannedEffort();
    }
    return eff;
}

Duration Schedule::plannedEffort( const QDate &date ) const
{
    //kDebug();
    Duration eff;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        eff += it.next() ->plannedEffort( date );
    }
    return eff;
}

Duration Schedule::plannedEffortTo( const QDate &date ) const
{
    //kDebug();
    Duration eff;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        eff += it.next() ->plannedEffortTo( date );
    }
    return eff;
}

EffortCost Schedule::plannedCost() const
{
    //kDebug();
    EffortCost c;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        c += it.next() ->plannedCost();
    }
    return c;
}

double Schedule::plannedCost( const QDate &date ) const
{
    //kDebug();
    double c = 0;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        c += it.next() ->plannedCost( date );
    }
    return c;
}

double Schedule::plannedCostTo( const QDate &date ) const
{
    //kDebug();
    double c = 0;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        c += it.next() ->plannedCostTo( date );
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
    //kDebug()<<"("<<this<<")";
    init();
}

NodeSchedule::NodeSchedule( Node *node, const QString& name, Schedule::Type type, long id )
        : Schedule( name, type, id ),
        m_node( node )
{
    //kDebug()<<"node name:"<<node->name();
    init();
}

NodeSchedule::NodeSchedule( Schedule *parent, Node *node )
        : Schedule( parent ),
        m_node( node )
{

    //kDebug()<<"node name:"<<node->name();
    init();
}

NodeSchedule::~NodeSchedule()
{
    //kDebug()<<this<<""<<m_appointments.count();
    while ( !m_appointments.isEmpty() ) {
        Appointment *a = m_appointments.takeFirst();
        a->setNode( 0 );
        delete a;
    }
    //kDebug()<<"forw"<<m_forward.count();
    while ( !m_forward.isEmpty() ) {
        Appointment *a = m_forward.takeFirst();
        a->setNode( 0 );
        delete a;
    }
    //kDebug()<<"backw"<<m_backward.count();
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
    //kDebug()<<"deleted="<<on;
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
    //kDebug();
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
    schedulingError = sch.attribute( "scheduling-conflict", "0" ).toInt();
    notScheduled = sch.attribute( "not-scheduled", "1" ).toInt();

    positiveFloat = Duration::fromString( sch.attribute( "positive-float" ) );
    negativeFloat = Duration::fromString( sch.attribute( "negative-float" ) );
    freeFloat = Duration::fromString( sch.attribute( "free-float" ) );

    return true;
}

void NodeSchedule::saveXML( QDomElement &element ) const
{
    //kDebug();
    QDomElement sch = element.ownerDocument().createElement( "schedule" );
    element.appendChild( sch );
    saveCommonXML( sch );

    if ( earlyStart.isValid() ) {
        sch.setAttribute( "earlystart", earlyStart.toString( KDateTime::ISODate ) );
    }
    if ( lateStart.isValid() ) {
        sch.setAttribute( "latestart", lateStart.toString( KDateTime::ISODate ) );
    }
    if ( earlyFinish.isValid() ) {
        sch.setAttribute( "earlyfinish", earlyFinish.toString( KDateTime::ISODate ) );
    }
    if ( lateFinish.isValid() ) {
        sch.setAttribute( "latefinish", lateFinish.toString( KDateTime::ISODate ) );
    }
    if ( startTime.isValid() )
        sch.setAttribute( "start", startTime.toString( KDateTime::ISODate ) );
    if ( endTime.isValid() )
        sch.setAttribute( "end", endTime.toString( KDateTime::ISODate ) );
    if ( workStartTime.isValid() )
        sch.setAttribute( "start-work", workStartTime.toString( KDateTime::ISODate ) );
    if ( workEndTime.isValid() )
        sch.setAttribute( "end-work", workEndTime.toString( KDateTime::ISODate ) );

    sch.setAttribute( "duration", duration.toString() );

    sch.setAttribute( "in-critical-path", inCriticalPath );
    sch.setAttribute( "resource-error", resourceError );
    sch.setAttribute( "resource-overbooked", resourceOverbooked );
    sch.setAttribute( "resource-not-available", resourceNotAvailable );
    sch.setAttribute( "scheduling-conflict", schedulingError );
    sch.setAttribute( "not-scheduled", notScheduled );
    
    sch.setAttribute( "positive-float", positiveFloat.toString() );
    sch.setAttribute( "negative-float", negativeFloat.toString() );
    sch.setAttribute( "free-float", freeFloat.toString() );
}

void NodeSchedule::addAppointment( Schedule *resource, DateTime &start, DateTime &end, double load )
{
    //kDebug();
    Appointment * a = findAppointment( resource, this, m_calculationMode );
    if ( a != 0 ) {
        //kDebug()<<"Add interval to existing"<<a;
        a->addInterval( start, end, load );
        return ;
    }
    a = new Appointment( resource, this, start, end, load );
    Q_ASSERT ( add( a ) == true );
    Q_ASSERT ( resource->add( a ) == true );
    //kDebug()<<"Added interval to new"<<a;
}

void NodeSchedule::takeAppointment( Appointment *appointment, int mode )
{
    Schedule::takeAppointment( appointment, mode );
    appointment->setNode( 0 ); // not my appointment anymore
    //kDebug()<<"Taken:"<<appointment;
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
    //kDebug()<<"("<<this<<")";
}

ResourceSchedule::ResourceSchedule( Resource *resource, const QString& name, Schedule::Type type, long id )
        : Schedule( name, type, id ),
        m_resource( resource ),
        m_parent( 0 ),
        m_nodeSchedule( 0 )
{
    //kDebug()<<"resource:"<<resource->name();
}

ResourceSchedule::ResourceSchedule( Schedule *parent, Resource *resource )
        : Schedule( parent ),
        m_resource( resource ),
        m_parent( parent ),
        m_nodeSchedule( 0 )
{
    //kDebug()<<"resource:"<<resource->name();
}

ResourceSchedule::~ResourceSchedule()
{
    //kDebug()<<this<<""<<m_appointments.count();
    while ( !m_appointments.isEmpty() ) {
        Appointment *a = m_appointments.takeFirst();
        a->setResource( 0 );
        delete a;
    }
    //kDebug()<<"forw"<<m_forward.count();
    while ( !m_forward.isEmpty() ) {
        Appointment *a = m_forward.takeFirst();
        a->setResource( 0 );
        delete a;
    }
    //kDebug()<<"backw"<<m_backward.count();
    while ( !m_backward.isEmpty() ) {
        Appointment *a = m_backward.takeFirst();
        a->setResource( 0 );
        delete a;
    }
}

// called from the resource
void ResourceSchedule::addAppointment( Schedule *node, DateTime &start, DateTime &end, double load )
{
    Q_ASSERT( start < end );
    //kDebug()<<"("<<this<<")"<<node<<","<<m_calculationMode;
    Appointment * a = findAppointment( this, node, m_calculationMode );
    if ( a != 0 ) {
        //kDebug()<<"Add interval to existing"<<a;
        a->addInterval( start, end, load );
        return ;
    }
    a = new Appointment( this, node, start, end, load );
    bool result = add( a );
    Q_ASSERT ( result == true );
    result = node->add( a );
    Q_ASSERT ( result == true );
    //kDebug()<<"Added interval to new"<<a;
}

void ResourceSchedule::takeAppointment( Appointment *appointment, int mode )
{
    Schedule::takeAppointment( appointment, mode );
    appointment->setResource( 0 );
    //kDebug()<<"Taken:"<<appointment;
    if ( appointment->node() )
        appointment->node() ->takeAppointment( appointment );
}

bool ResourceSchedule::isOverbooked() const
{
    return false;
}

bool ResourceSchedule::isOverbooked( const KDateTime &start, const KDateTime &end ) const
{
    if ( m_resource == 0 )
        return false;
    //kDebug()<<start.toString()<<" -"<<end.toString();
    Appointment a = appointmentIntervals();
    foreach ( const AppointmentInterval &i, a.intervals() ) {
        if ( ( !end.isValid() || i.startTime() < end ) &&
                ( !start.isValid() || i.endTime() > start ) ) {
            if ( i.load() > m_resource->units() ) {
                //kDebug()<<m_name<<" overbooked";
                return true;
            }
        }
        if ( i.startTime() >= end )
            break;
    }
    //kDebug()<<m_name<<" not overbooked";
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
    foreach ( const AppointmentInterval &i, a.intervals() ) {
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
        a.setIntervals( m_resource->externalAppointments() );
    }
    a.merge( appointmentIntervals( m_calculationMode ) );
    if ( a.isEmpty() || a.startTime() >= interval.second || a.endTime() <= interval.first ) {
        //kDebug()<<this<<"id="<<m_id<<"Mode="<<m_calculationMode<<""<<interval.first<<","<<interval.second<<" FREE";
        return DateTimeInterval( interval.first, interval.second );
    }
    if ( interval.first >= a.endTime() || interval.second <= a.startTime() ) {
        // free
        return DateTimeInterval( interval.first, interval.second );
    }
    DateTimeInterval ci = interval;
    foreach ( const AppointmentInterval &i, a.intervals() ) {
        //const_cast<ResourceSchedule*>(this)->logDebug( QString( "Schedule available check interval=%1 - %2" ).arg(i.startTime().toString()).arg(i.endTime().toString()) );
        if ( i.startTime() < ci.second && i.endTime() > ci.first ) {
            if ( ci.first >= i.startTime() && ci.second <= i.endTime() ) {
                //busy
                return DateTimeInterval( DateTime(), DateTime() );
            }
            DateTime t = i.startTime();
            if ( t > ci.first ) {
                //kDebug()<<this<<"id="<<m_id<<"Mode="<<m_calculationMode<<""<<interval.first<<","<<t<<" PART";
                //const_cast<ResourceSchedule*>(this)->logDebug( QString( "Schedule available t>first: returns interval=%1 - %2" ).arg(ci.first.toString()).arg(t.toString()) );
                return DateTimeInterval( ci.first, t );
            }
            t = i.endTime();
            if ( t < ci.second ) {
                ci.first = t;
                continue;
            }
            if ( i.startTime() <= ci.first && i.endTime() >= ci.second ) {
                //kDebug()<<this<<"id="<<m_id<<"Mode="<<m_calculationMode<<""<<interval.first<<","<<interval.second<<" BUSY";
                return DateTimeInterval( DateTime(), DateTime() );
            }
            ci.first = qMax( ci.first, i.endTime() );
            if ( ci.first >= ci.second ) {
                return DateTimeInterval( DateTime(), DateTime() ); //busy
            }
        }
    }
    //kDebug()<<this<<"id="<<m_id<<"Mode="<<m_calculationMode<<""<<interval.first<<","<<interval.second<<" FREE";
    return DateTimeInterval( ci.first, ci.second );
}

void ResourceSchedule::logError( const QString &msg, int phase )
{
    if ( m_parent ) {
        Schedule::Log log( m_nodeSchedule->node(), m_resource, Log::Type_Error, msg, phase );
        m_parent->addLog( log );
    }
}

void ResourceSchedule::logWarning( const QString &msg, int phase )
{
    if ( m_parent ) {
        Schedule::Log log( m_nodeSchedule->node(), m_resource, Log::Type_Warning, msg, phase );
        m_parent->addLog( log );
    }
}

void ResourceSchedule::logInfo( const QString &msg, int phase )
{
    if ( m_parent ) {
        Schedule::Log log( m_nodeSchedule->node(), m_resource, Log::Type_Info, msg, phase );
        m_parent->addLog( log );
    }
}

void ResourceSchedule::logDebug( const QString &msg, int phase )
{
    if ( m_parent ) {
        Schedule::Log log( m_nodeSchedule->node(), m_resource, Log::Type_Debug, msg, phase );
        m_parent->addLog( log );
    }
}

//--------------------------------------
MainSchedule::MainSchedule()
    : NodeSchedule(),
    m_manager( 0 )
{
    //kDebug()<<"("<<this<<")";
    init();
}

MainSchedule::MainSchedule( Node *node, const QString& name, Schedule::Type type, long id )
    : NodeSchedule( node, name, type, id ),
    m_manager( 0 ),
    m_currentCriticalPath( 0 ),
    criticalPathListCached( false )
{
    //kDebug()<<"node name:"<<node->name();
    init();
}

MainSchedule::~MainSchedule()
{
    //kDebug()<<"("<<this<<")";
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

void MainSchedule::setAllowOverbooking( bool state )
{
    if ( m_manager )
        m_manager->setAllowOverbooking( state );
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
    //kDebug();
    QString s;
    Schedule::loadXML( sch, status );

    s = sch.attribute( "start" );
    if ( !s.isEmpty() )
        startTime = DateTime::fromString( s, status.projectSpec() );
    s = sch.attribute( "end" );
    if ( !s.isEmpty() )
        endTime = DateTime::fromString( s, status.projectSpec() );
    
    duration = Duration::fromString( sch.attribute( "duration" ) );
    schedulingError = sch.attribute( "scheduling-conflict", "0" ).toInt();

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
                    kDebug()<<"node";
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

    element.setAttribute( "start", startTime.toString( KDateTime::ISODate ) );
    element.setAttribute( "end", endTime.toString( KDateTime::ISODate ) );
    element.setAttribute( "duration", duration.toString() );
    element.setAttribute( "scheduling-conflict", schedulingError );
    
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

QList<Schedule::Log> MainSchedule::logs()
{
    return m_log;
}

const QList<Schedule::Log> &MainSchedule::logs() const
{
    return m_log;
}

void MainSchedule::addLog( Schedule::Log &log )
{
    if ( log.phase == -1 && ! m_log.isEmpty() ) {
        log.phase = m_log.last().phase;
    }
    m_log.append( log );
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
    m_calculateAll( false ),
    m_usePert( false ),
    m_recalculate( false ),
    m_schedulingDirection( false ),
    m_scheduling( false ),
    m_expected( 0 ),
    m_optimistic( 0 ),
    m_pessimistic( 0 )
{
    //kDebug()<<name;
}

ScheduleManager::~ScheduleManager()
{
    qDeleteAll( m_children );
    setParentManager( 0 );
}

void ScheduleManager::setParentManager( ScheduleManager *sm )
{
    if ( m_parent ) {
        m_parent->removeChild( this );
    }
    m_parent = sm;
    if ( sm ) {
        sm->insertChild( this );
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
    //kDebug()<<m_name<<", insert"<<sm->name()<<","<<index;
    if ( index == -1 ) {
        m_children.append( sm );
    } else {
        m_children.insert( index, sm );
    }
}

void ScheduleManager::createSchedules()
{
    setExpected( m_project.createSchedule( m_name, Schedule::Expected ) );
    if ( m_calculateAll ) {
        setOptimistic( m_project.createSchedule( m_name, Schedule::Optimistic ) );
        setPessimistic( m_project.createSchedule( m_name, Schedule::Pessimistic ) );
    } else {
        setOptimistic( 0 );
        setPessimistic( 0 );
    }
}

int ScheduleManager::indexOf( const ScheduleManager *child ) const
{
    //kDebug()<<this<<","<<child;
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
    if ( m_expected ) {
        m_expected->setName( name );
        m_project.changed( m_expected );
    }
    if ( m_optimistic ) {
        m_optimistic->setName( name );
        m_project.changed( m_optimistic );
    }
    if ( m_pessimistic ) {
        m_pessimistic->setName( name );
        m_project.changed( m_pessimistic );
    }
    m_project.changed( this );
}

bool ScheduleManager::isChildBaselined() const
{
    //kDebug()<<on;
    foreach ( ScheduleManager *sm, m_children ) {
        if ( sm->isBaselined() || sm->isChildBaselined() ) {
            return true;
        }
    }
    return false;
}

void ScheduleManager::setBaselined( bool on )
{
    //kDebug()<<on;
    m_baselined = on;
    m_project.changed( this );
}

void ScheduleManager::setAllowOverbooking( bool on )
{
    //kDebug()<<on;
    m_allowOverbooking = on;
    m_project.changed( this );
}

bool ScheduleManager::allowOverbooking() const
{
    //kDebug()<<m_name<<"="<<m_allowOverbooking;
    return m_allowOverbooking;
}

bool ScheduleManager::checkExternalAppointments() const
{
    //kDebug()<<m_name<<"="<<m_allowOverbooking;
    return m_checkExternalAppointments;
}

void ScheduleManager::setCheckExternalAppointments( bool on )
{
    //kDebug()<<m_name<<"="<<m_checkExternalAppointments;
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

void ScheduleManager::setCalculateAll( bool on )
{
     m_calculateAll = on;
     m_project.changed( this );
}

void ScheduleManager::setSchedulingDirection( bool on )
{
    //kDebug()<<on;
    m_schedulingDirection = on;
    m_project.changed( this );
}

void ScheduleManager::setScheduling( bool on )
{
    //kDebug()<<on;
    m_scheduling = on;
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
        return m_project.schedulerPlugins().value( m_project.schedulerPlugins().keys().at( 0 ) );
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
    m_schedulerPluginId = m_project.schedulerPlugins().keys().at( index );
    kDebug()<<index<<m_schedulerPluginId;
    m_project.changed( this );
}

void ScheduleManager::calculateSchedule()
{
    schedulerPlugin()->calculate( m_project, this );
}

void ScheduleManager::setDeleted( bool on )
{
    if ( m_expected ) {
        m_expected->setDeleted( on );
    }
    if ( m_optimistic ) {
        m_optimistic->setDeleted( on );
    }
    if ( m_pessimistic ) {
        m_pessimistic->setDeleted( on );
    }
    m_project.changed( this );
}

void ScheduleManager::setExpected( MainSchedule *sch )
{
    //kDebug()<<m_expected<<","<<sch;
    if ( m_expected ) {
        int i = m_schedules.indexOf( m_expected );
        m_project.sendScheduleToBeRemoved( m_expected );
        m_expected->setDeleted( true );
        m_schedules.removeAt( i );
        m_project.sendScheduleRemoved( m_expected );
    }
    m_expected = sch;
    if ( sch ) {
        m_project.sendScheduleToBeAdded( this, 0 );
        sch->setManager( this );
        m_schedules.insert( 0, sch );
        m_expected->setDeleted( false );
        m_project.sendScheduleAdded( sch );
    }
    Q_ASSERT( m_schedules.count() <= 3 );
    m_project.changed( this );
}

void ScheduleManager::setOptimistic( MainSchedule *sch )
{
    if ( m_optimistic ) {
        int i = m_schedules.indexOf( m_optimistic );
        m_project.sendScheduleToBeRemoved( m_optimistic );
        m_optimistic->setDeleted( true );
        m_schedules.removeAt( i );
        m_project.sendScheduleRemoved( m_optimistic );
    }
    m_optimistic = sch;
    if ( sch ) {
        int i = m_schedules.count() >= 1 ? 1 : 0;
        m_project.sendScheduleToBeAdded( this, i );
        sch->setManager( this );
        m_schedules.insert( i, sch );
        m_optimistic->setDeleted( false );
        m_project.sendScheduleAdded( sch );
    }
    Q_ASSERT( m_schedules.count() <= 3 );
    m_project.changed( this );
}

void ScheduleManager::setPessimistic( MainSchedule *sch )
{
    if ( m_pessimistic ) {
        int i = m_schedules.indexOf( m_pessimistic );
        m_project.sendScheduleToBeRemoved( m_pessimistic );
        m_pessimistic->setDeleted( true );
        m_schedules.removeAt( i );
        m_project.sendScheduleRemoved( m_pessimistic );
    }
    m_pessimistic = sch;
    if ( sch ) {
        int i = m_schedules.count() >= 2 ? 2 : m_schedules.count();
        m_project.sendScheduleToBeAdded( this, i );
        sch->setManager( this );
        m_schedules.insert( i, sch );
        m_pessimistic->setDeleted( false );
        m_project.sendScheduleAdded( sch );
    }
    Q_ASSERT( m_schedules.count() <= 3 );
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
    if ( m_expected == 0 && m_optimistic == 0 && m_pessimistic == 0 ) {
        return lst << i18n( "Not scheduled" );
    }
    if ( Schedule *s = m_pessimistic ) {
        if ( s->resourceError || s->resourceOverbooked || s->resourceNotAvailable || s->schedulingError ) {
            return lst << i18n( "Error" );
        }
    }
    if ( Schedule *s = m_optimistic ) {
        if ( s->resourceError || s->resourceOverbooked || s->resourceNotAvailable || s->schedulingError ) {
            return lst << i18n( "Error" );
        }
    }
    if ( Schedule *s = m_expected ) {
        if ( s->resourceError || s->resourceOverbooked || s->resourceNotAvailable || s->schedulingError ) {
            return lst << i18n( "Error" );
        }
        return s->state();
    }
    return lst;
}

int ScheduleManager::numSchedules() const
{
    return schedules().count();
}

int ScheduleManager::indexOf( const MainSchedule* sch ) const
{
    return schedules().indexOf( const_cast<MainSchedule*>(sch) );
}

QList<MainSchedule*> ScheduleManager::schedules() const
{
    return m_schedules;
}

void ScheduleManager::incProgress()
{
    m_project.incProgress();
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
                case Schedule::Optimistic: setOptimistic( sch ); break;
                case Schedule::Pessimistic: setPessimistic( sch ); break;
            }
            m_calculateAll = schedules().count() > 1;
        }
        return true;
    }
    m_name = element.attribute( "name" );
    m_usePert = (bool)(element.attribute( "distribution" ).toInt());
    m_allowOverbooking = (bool)(element.attribute( "overbooking" ).toInt());
    m_checkExternalAppointments = (bool)(element.attribute( "check-external-appointments" ).toInt());
    m_schedulingDirection = (bool)(element.attribute( "scheduling-direction" ).toInt());
    m_baselined = (bool)(element.attribute( "baselined" ).toInt());
    m_schedulerPluginId = element.attribute( "scheduler-plugin-id" );
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        //kDebug()<<e.tagName();
        if ( e.tagName() == "schedule" ) {
            sch = loadMainSchedule( e, status );
            if ( sch ) {
                sch->setManager( this );
                switch ( sch->type() ) {
                    case Schedule::Expected: setExpected( sch ); break;
                    case Schedule::Optimistic: setOptimistic( sch ); break;
                    case Schedule::Pessimistic: setPessimistic( sch ); break;
                }
            }
        } else if ( e.tagName() == "plan" ) {
            ScheduleManager *sm = new ScheduleManager( status.project() );
            if ( sm->loadXML( e, status ) ) {
                sm->setParentManager( this );
            } else {
                kError()<<"Failed to load schedule manager"<<endl;
                delete sm;
            }
        }
    }
    m_calculateAll = schedules().count() > 1;
    return true;
}

MainSchedule *ScheduleManager::loadMainSchedule( KoXmlElement &element, XMLLoaderObject &status ) {
    MainSchedule *sch = new MainSchedule();
    if ( sch->loadXML( element, status ) ) {
        status.project().addSchedule( sch );
        sch->setNode( &(status.project()) );
        status.project().setParentSchedule( sch );
        // If it's here, it's scheduled!
        sch->setScheduled( true );
    } else {
        kError() << "Failed to load schedule" << endl;
        delete sch;
        sch = 0;
    }
    return sch;
}

void ScheduleManager::saveXML( QDomElement &element ) const
{
    QDomElement el = element.ownerDocument().createElement( "plan" );
    element.appendChild( el );
    el.setAttribute( "name", m_name );
    el.setAttribute( "distribution", m_usePert );
    el.setAttribute( "overbooking", m_allowOverbooking );
    el.setAttribute( "check-external-appointments", m_checkExternalAppointments );
    el.setAttribute( "scheduling-direction", m_schedulingDirection );
    el.setAttribute( "baselined", m_baselined );
    el.setAttribute( "scheduler-plugin-id", m_schedulerPluginId );
    foreach ( MainSchedule *s, schedules() ) {
        //kDebug()<<m_name<<" id="<<s->id()<<(s->isDeleted()?"  Deleted":"");
        if ( !s->isDeleted() && s->isScheduled() ) {
            QDomElement schs = el.ownerDocument().createElement( "schedule" );
            el.appendChild( schs );
            s->saveXML( schs );
            m_project.saveAppointments( schs, s->id() );
        }
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
    el.setAttribute( "distribution", m_usePert );
    el.setAttribute( "overbooking", m_allowOverbooking );
    el.setAttribute( "check-external-appointments", m_checkExternalAppointments );
    el.setAttribute( "scheduling-direction", m_schedulingDirection );
    el.setAttribute( "baselined", m_baselined );
    if ( m_expected && ! m_expected->isDeleted() ) {
        QDomElement schs = el.ownerDocument().createElement( "schedule" );
        el.appendChild( schs );
        m_expected->saveXML( schs );
        Schedule *s = node.findSchedule( m_expected->id() );
        if ( s && ! s->isDeleted() ) {
            s->saveAppointments( schs );
        }
    }
}


#ifndef NDEBUG
void Schedule::printDebug( const QString& indent )
{
    kDebug() << indent <<"Schedule[" << m_id <<"] '" << m_name <<"' type:" << typeToString() <<" (" << m_type <<")" << ( isDeleted() ?"   Deleted" :"" );
}
void NodeSchedule::printDebug( const QString& _indent )
{
    QString indent = _indent;
    Schedule::printDebug( indent );
    indent += "!  ";
    if ( m_parent == 0 )
        kDebug() << indent <<"No parent schedule!";
    if ( !notScheduled ) {
//FIXME: QT3 support
/*        if ( node() )
            kDebug() << indent <<"Node:" << node() ->name();
        else
            kDebug() << indent <<"No parent node!";*/
    }
    kDebug() << indent <<"Not scheduled=" << notScheduled;
    kDebug() << indent <<"Start time:" << startTime.toString();
    kDebug() << indent <<"End time:" << endTime.toString();
    kDebug() << indent <<"Duration:" << duration.seconds() << QByteArray(" secs" ) <<" (" << duration.toString() <<")";
    kDebug() << indent <<"Earliest start:" << earlyStart.toString();
    kDebug() << indent <<"Latest finish:" << lateFinish.toString();

    kDebug() << indent <<"schedulingError=" << schedulingError;
    Task *t = qobject_cast<Task*>( m_node );
    if ( t ) {
        kDebug() << indent <<"Effort not met="<<t->effortMetError();
    }
    kDebug() << indent <<"resourceError=" << resourceError;
    kDebug() << indent <<"resourceNotAvailable=" << resourceNotAvailable;
    kDebug() << indent <<"Resource overbooked=" << resourceOverbooked;
    if ( resourceOverbooked ) {
        kDebug() << indent <<"" << overbookedResources();
    }

    kDebug() << indent <<"inCriticalPath=" << inCriticalPath;
    kDebug() << indent;
    kDebug() << indent <<"workStartTime=" << workStartTime.toString();
    kDebug() << indent <<"workEndTime=" << workEndTime.toString();
    kDebug() << indent;
    kDebug() << indent <<"Appointments (Scheduling):" << m_appointments.count();
    foreach( Appointment *a, m_appointments ) {
        a->printDebug( indent + "  " );
    }
    kDebug() << indent <<"Appointments (Forward):" << m_forward.count();
    foreach( Appointment *a, m_forward ) {
        a->printDebug( indent + "  " );
    }
    kDebug() << indent <<"Appointments (Backward):" << m_backward.count();
    foreach( Appointment *a, m_backward ) {
        a->printDebug( indent + "  " );
    }
}
void ResourceSchedule::printDebug( const QString& _indent )
{
    QString indent = _indent;
    Schedule::printDebug( indent );
    indent += "!  ";
    if ( m_parent == 0 )
        kDebug() << indent <<"No parent schedule!";
    if ( resource() )
        kDebug() << indent <<"Resource:" << resource() ->name();
    else
        kDebug() << indent <<"No parent resource!";
    kDebug() << indent;
    kDebug() << indent <<"Appointments:" << m_appointments.count();
    kDebug() << indent;
    kDebug() << indent <<"Appointments (Scheduling):" << m_appointments.count();
    foreach( Appointment *a, m_appointments ) {
        a->printDebug( indent + "  " );
    }
    kDebug() << indent;
    kDebug() << indent <<"Appointments (Total):";
    appointmentIntervals().printDebug( indent + "  " );
    kDebug() << indent;
    kDebug() << indent <<"Appointments (Forward):" << m_forward.count();
    foreach( Appointment *a, m_forward ) {
        a->printDebug( indent + "  " );
    }
    kDebug() << indent <<"Appointments (Backward):" << m_backward.count();
    foreach( Appointment *a, m_backward ) {
        a->printDebug( indent + "  " );
    }
}

void MainSchedule::printDebug( const QString& _indent )
{
    QString indent = _indent;
    Schedule::printDebug( indent );
    indent += "!  ";
//FIXME: QT3 support
//     if ( node() )
//         kDebug() << indent <<"Node:" << node() ->name();
//     else
//         kDebug() << indent <<"No parent node!";

    kDebug() << indent <<"Not scheduled=" << notScheduled;
    kDebug() << indent <<"Start time:" << startTime.toString();
    kDebug() << indent <<"End time:" << endTime.toString();
    kDebug() << indent <<"Duration:" << duration.seconds() << QByteArray(" secs" ) <<" (" << duration.toString() <<")";
    kDebug() << indent <<"Earliest start:" << earlyStart.toString();
    kDebug() << indent <<"Latest finish:" << lateFinish.toString();

    kDebug() << indent;
    kDebug() << indent <<"Appointments:" << m_appointments.count();
    QListIterator<Appointment*> it = m_appointments;
    while ( it.hasNext() ) {
        it.next() ->printDebug( indent + "  " );
    }
    kDebug()<< indent << "Scheduling log:"<<m_log.count();
    if ( ! m_log.isEmpty() ) {
        indent += "!-- ";
        QList<Schedule::Log>::ConstIterator it;
        for (it = m_log.constBegin(); it != m_log.constEnd(); ++it) {
            kDebug()<< indent << logPhase( (*it).phase ) << (*it).node->name() << ((*it).resource? (*it).resource->name():"") << (*it).message;
        }
    }
}
#endif

} //namespace KPlato

