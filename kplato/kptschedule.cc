/* This file is part of the KDE project
 Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

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
 Boston, MA 02110-1301, USA.
*/

#include "kptschedule.h"

#include "kptappointment.h"
#include "kptdatetime.h"
#include "kptduration.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kptxmlloaderobject.h"

#include <QDomElement>
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
    //kDebug()<<k_funcinfo<<"("<<this<<") Name: '"<<name<<"' Type="<<type<<" id="<<id<<endl;
}

Schedule::Schedule( const QString& name, Type type, long id )
        : m_name( name ),
        m_type( type ),
        m_id( id ),
        m_deleted( false ),
        m_parent( 0 ),
        m_calculationMode( Schedule::Scheduling )
{

    //kDebug()<<k_funcinfo<<"("<<this<<") Name: '"<<name<<"' Type="<<type<<" id="<<id<<endl;
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
    //kDebug()<<k_funcinfo<<"deleted="<<on<<endl;
    m_deleted = on;
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
        lst << i18n( "Deleted" );
    if ( notScheduled )
        lst << i18n( "Not scheduled" );
    if ( schedulingError )
        lst << i18n( "Cannot fullfill constraints" );
    if ( resourceError )
        lst << i18n( "No resource allocated" );
    if ( resourceNotAvailable )
        lst << i18n( "Resource not available" );
    if ( resourceOverbooked )
        lst << i18n( "Resource overbooked" );
    if ( lst.isEmpty() )
        lst << i18n( "Scheduled" );
    return lst;
}

bool Schedule::usePert() const
{
    if ( m_parent ) {
        return m_parent->usePert();
    }
    return false;
}

bool Schedule::reserveResources() const
{
    if ( m_parent ) {
        return m_parent->reserveResources();
    }
    return false;
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

bool Schedule::loadXML( const QDomElement &sch )
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
    //kDebug()<<k_funcinfo<<m_name<<" save schedule"<<endl;
    element.setAttribute( "name", m_name );
    element.setAttribute( "type", typeToString() );
    element.setAttribute( "id", qlonglong( m_id ) );
}

void Schedule::saveAppointments( QDomElement &element ) const
{
    //kDebug()<<k_funcinfo<<endl;
    QListIterator<Appointment*> it = m_appointments;
    while ( it.hasNext() ) {
        it.next() ->saveXML( element );
    }
}

// used (directly) when appointment wants to attatch itself again
bool Schedule::attatch( Appointment *appointment )
{
    int mode = appointment->calculationMode();
    if ( mode == Scheduling ) {
        if ( m_appointments.indexOf( appointment ) != -1 ) {
            kError() << k_funcinfo << "Appointment already exists" << endl;
            return false;
        }
        m_appointments.append( appointment );
        //if (resource()) kDebug()<<k_funcinfo<<appointment<<" For resource '"<<resource()->name()<<"'"<<" count="<<m_appointments.count()<<endl;
        //if (node()) kDebug()<<k_funcinfo<<"("<<this<<")"<<appointment<<" For node '"<<node()->name()<<"'"<<" count="<<m_appointments.count()<<endl;
        return true;
    }
    if ( mode == CalculateForward ) {
        if ( m_forward.indexOf( appointment ) != -1 ) {
            kError() << k_funcinfo << "Appointment already exists" << endl;
            return false;
        }
        m_forward.append( appointment );
        //if (resource()) kDebug()<<k_funcinfo<<"For resource '"<<resource()->name()<<"'"<<endl;
        //if (node()) kDebug()<<k_funcinfo<<"For node '"<<node()->name()<<"'"<<endl;
        return true;
    }
    if ( mode == CalculateBackward ) {
        if ( m_backward.indexOf( appointment ) != -1 ) {
            kError() << k_funcinfo << "Appointment already exists" << endl;
            return false;
        }
        m_backward.append( appointment );
        //if (resource()) kDebug()<<k_funcinfo<<"For resource '"<<resource()->name()<<"'"<<endl;
        //if (node()) kDebug()<<k_funcinfo<<"For node '"<<node()->name()<<"'"<<endl;
        return true;
    }
    kError()<<k_funcinfo<<"Unknown mode: "<<m_calculationMode<<endl;
    return false;
}

// used to add new schedules
bool Schedule::add( Appointment *appointment )
{
    //kDebug()<<k_funcinfo<<this<<endl;
    appointment->setCalculationMode( m_calculationMode );
    return attatch( appointment );
}

void Schedule::takeAppointment( Appointment *appointment, int mode )
{
    //kDebug()<<k_funcinfo<<"("<<this<<") "<<mode<<": "<<appointment<<", "<<appointment->calculationMode()<<endl;
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
    foreach( Appointment *a,  m_appointments ) {
        if ( a->node() == node && a->resource() == resource ) {
            Q_ASSERT( mode == Scheduling );
            return a;
        }
    }
    foreach( Appointment *a,  m_forward ) {
        if ( a->node() == node && a->resource() == resource ) {
            Q_ASSERT( mode == CalculateForward );
            return a;
        }
    }
    foreach( Appointment *a,  m_backward ) {
        if ( a->node() == node && a->resource() == resource ) {
            Q_ASSERT( mode == CalculateBackward );
            return a;
        }
    }
    return 0;
}

EffortCostMap Schedule::plannedEffortCostPrDay( const QDate &start, const QDate &end ) const
{
    //kDebug()<<k_funcinfo<<m_name<<endl;
    EffortCostMap ec;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        //kDebug()<<k_funcinfo<<m_name<<endl;
        ec += it.next() ->plannedPrDay( start, end );
    }
    return ec;
}

Duration Schedule::plannedEffort() const
{
    //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        eff += it.next() ->plannedEffort();
    }
    return eff;
}

Duration Schedule::plannedEffort( const QDate &date ) const
{
    //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        eff += it.next() ->plannedEffort( date );
    }
    return eff;
}

Duration Schedule::plannedEffortTo( const QDate &date ) const
{
    //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        eff += it.next() ->plannedEffortTo( date );
    }
    return eff;
}

Duration Schedule::actualEffort() const
{
    //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        eff += it.next() ->actualEffort();
    }
    return eff;
}

Duration Schedule::actualEffort( const QDate &date ) const
{
    //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        eff += it.next() ->actualEffort( date );
    }
    return eff;
}

Duration Schedule::actualEffortTo( const QDate &date ) const
{
    //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        eff += it.next() ->actualEffortTo( date );
    }
    return eff;
}

double Schedule::plannedCost() const
{
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        c += it.next() ->plannedCost();
    }
    return c;
}

double Schedule::plannedCost( const QDate &date ) const
{
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        c += it.next() ->plannedCost( date );
    }
    return c;
}

double Schedule::plannedCostTo( const QDate &date ) const
{
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        c += it.next() ->plannedCostTo( date );
    }
    return c;
}

double Schedule::actualCost() const
{
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        c += it.next() ->actualCost();
    }
    return c;
}

double Schedule::actualCost( const QDate &date ) const
{
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        c += it.next() ->actualCost( date );
    }
    return c;
}

double Schedule::actualCostTo( const QDate &date ) const
{
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    QListIterator<Appointment*> it( m_appointments );
    while ( it.hasNext() ) {
        c += it.next() ->actualCostTo( date );
    }
    return c;
}

//-------------------------------------------------
NodeSchedule::NodeSchedule()
        : Schedule(),
        m_node( 0 )
{
    //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    init();
}

NodeSchedule::NodeSchedule( Node *node, const QString& name, Schedule::Type type, long id )
        : Schedule( name, type, id ),
        m_node( node )
{
    //kDebug()<<k_funcinfo<<"node name: "<<node->name()<<endl;
    init();
}

NodeSchedule::NodeSchedule( Schedule *parent, Node *node )
        : Schedule( parent ),
        m_node( node )
{

    //kDebug()<<k_funcinfo<<"node name: "<<node->name()<<endl;
    init();
}

NodeSchedule::~NodeSchedule()
{
    //kDebug()<<k_funcinfo<<this<<" "<<m_appointments.count()<<endl;
    while ( !m_appointments.isEmpty() ) {
        Appointment *a = m_appointments.takeFirst();
        a->setNode( 0 );
        delete a;
    }
    //kDebug()<<k_funcinfo<<"forw "<<m_forward.count()<<endl;
    while ( !m_forward.isEmpty() ) {
        Appointment *a = m_forward.takeFirst();
        a->setNode( 0 );
        delete a;
    }
    //kDebug()<<k_funcinfo<<"backw "<<m_backward.count()<<endl;
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
}

void NodeSchedule::setDeleted( bool on )
{
    //kDebug()<<k_funcinfo<<"deleted="<<on<<endl;
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

bool NodeSchedule::loadXML( const QDomElement &sch )
{
    //kDebug()<<k_funcinfo<<endl;
    QString s;
    Schedule::loadXML( sch );
    s = sch.attribute( "earlieststart" );
    if ( !s.isEmpty() )
        earliestStart = DateTime::fromString( s );
    s = sch.attribute( "latestfinish" );
    if ( !s.isEmpty() )
        latestFinish = DateTime::fromString( s );
    s = sch.attribute( "start" );
    if ( !s.isEmpty() )
        startTime = DateTime::fromString( s );
    s = sch.attribute( "end" );
    if ( !s.isEmpty() )
        endTime = DateTime::fromString( s );
    s = sch.attribute( "start-work" );
    if ( !s.isEmpty() )
        workStartTime = DateTime::fromString( s );
    s = sch.attribute( "end-work" );
    if ( !s.isEmpty() )
        workEndTime = DateTime::fromString( s );
    duration = Duration::fromString( sch.attribute( "duration" ) );

    inCriticalPath = sch.attribute( "in-critical-path", "0" ).toInt();
    resourceError = sch.attribute( "resource-error", "0" ).toInt();
    resourceOverbooked = sch.attribute( "resource-overbooked", "0" ).toInt();
    resourceNotAvailable = sch.attribute( "resource-not-available", "0" ).toInt();
    schedulingError = sch.attribute( "scheduling-conflict", "0" ).toInt();
    notScheduled = sch.attribute( "not-scheduled", "1" ).toInt();

    return true;
}

void NodeSchedule::saveXML( QDomElement &element ) const
{
    //kDebug()<<k_funcinfo<<endl;
    QDomElement sch = element.ownerDocument().createElement( "schedule" );
    element.appendChild( sch );
    saveCommonXML( sch );

    if ( earliestStart.isValid() )
        sch.setAttribute( "earlieststart", earliestStart.toString( Qt::ISODate ) );
    if ( latestFinish.isValid() )
        sch.setAttribute( "latestfinish", latestFinish.toString( Qt::ISODate ) );
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
    sch.setAttribute( "scheduling-conflict", schedulingError );
    sch.setAttribute( "not-scheduled", notScheduled );
}

void NodeSchedule::addAppointment( Schedule *resource, DateTime &start, DateTime &end, double load )
{
    //kDebug()<<k_funcinfo<<endl;
    Appointment * a = findAppointment( resource, this, m_calculationMode );
    if ( a != 0 ) {
        //kDebug()<<k_funcinfo<<"Add interval to existing "<<a<<endl;
        a->addInterval( start, end, load );
        return ;
    }
    a = new Appointment( resource, this, start, end, load );
    Q_ASSERT ( add( a ) == true );
    Q_ASSERT ( resource->add( a ) == true );
    //kDebug()<<k_funcinfo<<"Added interval to new "<<a<<endl;
}

void NodeSchedule::takeAppointment( Appointment *appointment, int mode )
{
    Schedule::takeAppointment( appointment, mode );
    appointment->setNode( 0 ); // not my appointment anymore
    //kDebug()<<k_funcinfo<<"Taken: "<<appointment<<endl;
    if ( appointment->resource() )
        appointment->resource() ->takeAppointment( appointment );
}

//-----------------------------------------------
ResourceSchedule::ResourceSchedule()
        : Schedule(),
        m_resource( 0 )
{
    //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
}

ResourceSchedule::ResourceSchedule( Resource *resource, const QString& name, Schedule::Type type, long id )
        : Schedule( name, type, id ),
        m_resource( resource ),
        m_parent( 0 )
{
    //kDebug()<<k_funcinfo<<"resource: "<<resource->name()<<endl;
}

ResourceSchedule::ResourceSchedule( Schedule *parent, Resource *resource )
        : Schedule( parent ),
        m_resource( resource ),
        m_parent( parent )
{
    //kDebug()<<k_funcinfo<<"resource: "<<resource->name()<<endl;
}

ResourceSchedule::~ResourceSchedule()
{
    //kDebug()<<k_funcinfo<<this<<" "<<m_appointments.count()<<endl;
    while ( !m_appointments.isEmpty() ) {
        Appointment *a = m_appointments.takeFirst();
        a->setResource( 0 );
        delete a;
    }
    //kDebug()<<k_funcinfo<<"forw "<<m_forward.count()<<endl;
    while ( !m_forward.isEmpty() ) {
        Appointment *a = m_forward.takeFirst();
        a->setResource( 0 );
        delete a;
    }
    //kDebug()<<k_funcinfo<<"backw "<<m_backward.count()<<endl;
    while ( !m_backward.isEmpty() ) {
        Appointment *a = m_backward.takeFirst();
        a->setResource( 0 );
        delete a;
    }
}

// called from the resource
void ResourceSchedule::addAppointment( Schedule *node, DateTime &start, DateTime &end, double load )
{
    //kDebug()<<k_funcinfo<<"("<<this<<") "<<node<<", "<<m_calculationMode<<endl;
    Appointment * a = findAppointment( this, node, m_calculationMode );
    if ( a != 0 ) {
        //kDebug()<<k_funcinfo<<"Add interval to existing "<<a<<endl;
        a->addInterval( start, end, load );
        return ;
    }
    a = new Appointment( this, node, start, end, load );
    Q_ASSERT ( add( a ) == true );
    Q_ASSERT ( node->add( a ) == true );
    //kDebug()<<k_funcinfo<<"Added interval to new "<<a<<endl;
}

void ResourceSchedule::takeAppointment( Appointment *appointment, int mode )
{
    Schedule::takeAppointment( appointment, mode );
    appointment->setResource( 0 );
    //kDebug()<<k_funcinfo<<"Taken: "<<appointment<<endl;
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
    //kDebug()<<k_funcinfo<<start.toString()<<" - "<<end.toString()<<endl;
    Appointment a = appointmentIntervals();
    foreach ( AppointmentInterval *i, a.intervals() ) {
        if ( ( !end.isValid() || i->startTime() < end ) &&
                ( !start.isValid() || i->endTime() > start ) ) {
            if ( i->load() > m_resource->units() ) {
                //kDebug()<<k_funcinfo<<m_name<<" overbooked"<<endl;
                return true;
            }
        }
        if ( i->startTime() >= end )
            break;
    }
    //kDebug()<<k_funcinfo<<m_name<<" not overbooked"<<endl;
    return false;
}

Appointment ResourceSchedule::appointmentIntervals() const
{
    Appointment app;
    foreach ( Appointment *a, m_appointments ) {
        app += *a;
    }
    return app;
}

double ResourceSchedule::normalRatePrHour() const
{
    return m_resource ? m_resource->normalRate() : 0.0;
}

//--------------------------------------
MainSchedule::MainSchedule()
    : NodeSchedule(),
    m_manager( 0 )
{
    //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    init();
}

MainSchedule::MainSchedule( Node *node, const QString& name, Schedule::Type type, long id )
    : NodeSchedule( node, name, type, id ),
    m_manager( 0 )
{
    //kDebug()<<k_funcinfo<<"node name: "<<node->name()<<endl;
    init();
}

MainSchedule::~MainSchedule()
{
    //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
}

bool MainSchedule::usePert() const
{
    return m_manager == 0 ? false : m_manager->usePert();
}

bool MainSchedule::reserveResources() const
{
    return m_manager == 0 ? false : m_manager->reserveResources();
}

bool MainSchedule::loadXML( const QDomElement &sch, Project &project )
{
    //kDebug()<<k_funcinfo<<endl;
    QString s;
    Schedule::loadXML( sch );

    s = sch.attribute( "start" );
    if ( !s.isEmpty() )
        startTime = DateTime::fromString( s );
    s = sch.attribute( "end" );
    if ( !s.isEmpty() )
        endTime = DateTime::fromString( s );

    QDomNodeList al = sch.childNodes();
    //kDebug()<<k_funcinfo<<"No of appointments: "<<al.count()<<endl;
    for ( unsigned int i = 0; i < al.count(); ++i ) {
        if ( al.item( i ).isElement() ) {
            QDomElement app = al.item( i ).toElement();
            if ( app.tagName() == "appointment" ) {
                // Load the appointments.
                // Resources and tasks must already be loaded
                Appointment * child = new Appointment();
                if ( !child->loadXML( app, project, *this ) ) {
                    // TODO: Complain about this
                    kError() << k_funcinfo << "Failed to load appointment" << endl;
                    delete child;
                }
            }
        }
    }
    return true;
}

void MainSchedule::saveXML( QDomElement &element ) const
{
    saveCommonXML( element );

    element.setAttribute( "start", startTime.toString( Qt::ISODate ) );
    element.setAttribute( "end", endTime.toString( Qt::ISODate ) );
}

//-----------------------------------------
ScheduleManager::ScheduleManager( Project &project, const QString name )
    : m_project( project),
    m_name( name ),
    m_calculateAll( false ),
    m_usePert( false ),
    m_expected( 0 ),
    m_optimistic( 0 ),
    m_pessimistic( 0 )
{
    //kDebug()<<k_funcinfo<<name<<endl;
}


void ScheduleManager::createSchedules()
{
    m_expected = m_project.createSchedule( m_name, Schedule::Expected );
    m_expected->setManager( this );
    if ( m_calculateAll ) {
        m_optimistic = m_project.createSchedule( m_name, Schedule::Optimistic );
        m_optimistic->setManager( this );
        m_pessimistic = m_project.createSchedule( m_name, Schedule::Pessimistic );
        m_pessimistic->setManager( this );
    } else {
        m_optimistic = 0;
        m_pessimistic = 0;
    }
    m_project.changed( this );
}

void ScheduleManager::setName( QString name )
{
    m_name = name;
    if ( m_expected )
        m_expected->setName( name );
    if ( m_optimistic )
        m_optimistic->setName( name );
    if ( m_pessimistic )
        m_pessimistic->setName( name );

    m_project.changed( this );
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

void ScheduleManager::setDeleted( bool on )
{
    foreach ( MainSchedule *s,  schedules() ) {
        s->setDeleted( on );
    }
    m_project.changed( this );
}

void ScheduleManager::setExpected( MainSchedule *sch )
{
    m_project.changed( this, 1 );
    m_expected = sch;
    m_project.changed( this, 2 );
}

void ScheduleManager::setOptimistic( MainSchedule *sch )
{
    m_project.changed( this, 1 );
    m_optimistic = sch;
    m_project.changed( this, 2 );
}

void ScheduleManager::setPessimistic( MainSchedule *sch )
{ 
    m_project.changed( this, 1 );
    m_pessimistic = sch; 
    m_project.changed( this, 2 );
}

QStringList ScheduleManager::state() const
{
    QStringList lst;
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

int ScheduleManager::indexOf( MainSchedule* sch ) const
{
    return schedules().indexOf( sch );
}

QList<MainSchedule*> ScheduleManager::schedules() const
{
    QList<MainSchedule*> lst;
    if ( m_expected )
        lst << m_expected;
    if ( m_optimistic )
        lst << m_optimistic;
    if ( m_pessimistic )
        lst << m_pessimistic;
    return lst;
}

bool ScheduleManager::loadXML( QDomElement &element, XMLLoaderObject &status )
{
    MainSchedule *sch = 0;
    if ( status.version() <= "0.5" ) {
        m_usePert = false;
        sch = loadMainSchedule( element, status );
        if ( sch ) {
            sch->setManager( this );
            switch ( sch->type() ) {
                case Schedule::Expected: m_expected = sch; break;
                case Schedule::Optimistic: m_optimistic = sch; break;
                case Schedule::Pessimistic: m_pessimistic = sch; break;
            }
            m_calculateAll = schedules().count() > 1;
        }
        return true;
    }
    m_name = element.attribute( "name" );
    m_usePert = (bool)(element.attribute( "distribution" ).toInt());
    QDomNodeList list = element.childNodes();
    for ( unsigned int i = 0; i < list.count(); ++i ) {
        if ( list.item( i ).isElement() ) {
            QDomElement e = list.item( i ).toElement();
            //kDebug()<<k_funcinfo<<e.tagName()<<endl;
            if ( e.tagName() == "schedule" ) {
                sch = loadMainSchedule( e, status );
                if ( sch ) {
                    sch->setManager( this );
                    switch ( sch->type() ) {
                        case Schedule::Expected: m_expected = sch; break;
                        case Schedule::Optimistic: m_optimistic = sch; break;
                        case Schedule::Pessimistic: m_pessimistic = sch; break;
                    }
                }
            }
        }
    }
    m_calculateAll = schedules().count() > 1;
    return true;
}

MainSchedule *ScheduleManager::loadMainSchedule( QDomElement &element, XMLLoaderObject &status ) {
    MainSchedule *sch = new MainSchedule();
    if ( sch->loadXML( element, status.project() ) ) {
        status.project().addSchedule( sch );
        sch->setNode( &(status.project()) );
        status.project().setParentSchedule( sch );
        // If it's here, it's scheduled!
        sch->setScheduled( true );
    } else {
        kError() << k_funcinfo << "Failed to load schedule" << endl;
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
    foreach ( MainSchedule *s, schedules() ) {
        //kDebug()<<k_funcinfo<<m_name<<" id="<<s->id()<<(s->isDeleted()?"  Deleted":"")<<endl;
        if ( !s->isDeleted() && s->isScheduled() ) {
            QDomElement schs = el.ownerDocument().createElement( "schedule" );
            el.appendChild( schs );
            s->saveXML( schs );
            m_project.saveAppointments( schs, s->id() );
        }
    }

}

#ifndef NDEBUG
void Schedule::printDebug( const QString& indent )
{
    kDebug() << indent << "Schedule[" << m_id << "] '" << m_name << "' type: " << typeToString() << " (" << m_type << ")" << ( isDeleted() ? "   Deleted" : "" ) << endl;
}
void NodeSchedule::printDebug( const QString& _indent )
{
    QString indent = _indent;
    Schedule::printDebug( indent );
    indent += "!  ";
    if ( m_parent == 0 )
        kDebug() << indent << "No parent schedule!" << endl;
    if ( !notScheduled ) {
//FIXME: QT3 support
/*        if ( node() )
            kDebug() << indent << "Node: " << node() ->name() << endl;
        else
            kDebug() << indent << "No parent node!" << endl;*/
    }
    kDebug() << indent << "Not scheduled=" << notScheduled << endl;
    kDebug() << indent << "Start time: " << startTime.toString() << endl;
    kDebug() << indent << "End time: " << endTime.toString() << endl;
    kDebug() << indent << "Duration: " << duration.seconds() << QByteArray( " secs" ) << " (" << duration.toString() << ")" << endl;
    kDebug() << indent << "Earliest start: " << earliestStart.toString() << endl;
    kDebug() << indent << "Latest finish: " << latestFinish.toString() << endl;

    kDebug() << indent << "Resource overbooked=" << resourceOverbooked << endl;
    kDebug() << indent << "resourceError=" << resourceError << endl;
    kDebug() << indent << "schedulingError=" << schedulingError << endl;
    kDebug() << indent << "resourceNotAvailable=" << resourceNotAvailable << endl;
    kDebug() << indent << "Resource overbooked=" << resourceOverbooked << endl;
    kDebug() << indent << "  " << overbookedResources() << endl;

    kDebug() << indent << "inCriticalPath=" << inCriticalPath << endl;
    kDebug() << indent << endl;
    kDebug() << indent << "workStartTime=" << workStartTime.toString() << endl;
    kDebug() << indent << "workEndTime=" << workEndTime.toString() << endl;
    kDebug() << indent << endl;
    kDebug() << indent << "Appointments: " << m_appointments.count() << endl;
    QListIterator<Appointment*> it = m_appointments;
    while ( it.hasNext() ) {
        it.next() ->printDebug( indent + "  " );
    }
}
void ResourceSchedule::printDebug( const QString& _indent )
{
    QString indent = _indent;
    Schedule::printDebug( indent );
    indent += "!  ";
    if ( m_parent == 0 )
        kDebug() << indent << "No parent schedule!" << endl;
    if ( resource() )
        kDebug() << indent << "Resource: " << resource() ->name() << endl;
    else
        kDebug() << indent << "No parent resource!" << endl;
    kDebug() << indent << endl;
    kDebug() << indent << "Appointments: " << m_appointments.count() << endl;
}

void MainSchedule::printDebug( const QString& _indent )
{
    QString indent = _indent;
    Schedule::printDebug( indent );
    indent += "!  ";
//FIXME: QT3 support
//     if ( node() )
//         kDebug() << indent << "Node: " << node() ->name() << endl;
//     else
//         kDebug() << indent << "No parent node!" << endl;

    kDebug() << indent << "Not scheduled=" << notScheduled << endl;
    kDebug() << indent << "Start time: " << startTime.toString() << endl;
    kDebug() << indent << "End time: " << endTime.toString() << endl;
    kDebug() << indent << "Duration: " << duration.seconds() << QByteArray( " secs" ) << " (" << duration.toString() << ")" << endl;
    kDebug() << indent << "Earliest start: " << earliestStart.toString() << endl;
    kDebug() << indent << "Latest finish: " << latestFinish.toString() << endl;

    kDebug() << indent << endl;
    kDebug() << indent << "Appointments: " << m_appointments.count() << endl;
    QListIterator<Appointment*> it = m_appointments;
    while ( it.hasNext() ) {
        it.next() ->printDebug( indent + "  " );
    }
}
#endif

} //namespace KPlato

