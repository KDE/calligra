/* This file is part of the KDE project
   Copyright (C) 2001 Thomas zander <zander@kde.org>
   Copyright (C) 2004-2007, 2012 Dag Andersen <danders@get2net.dk>

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

#include "kptresource.h"
#include "kptaccount.h"
#include "kptappointment.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptdatetime.h"
#include "kptcalendar.h"
#include "kpteffortcostmap.h"
#include "kptschedule.h"
#include "kptxmlloaderobject.h"
#include "kptdebug.h"

#include <KoXmlReader.h>

#include <kglobal.h>
#include <klocale.h>
#include <ksystemtimezone.h>
#include <ktimezone.h>

//Added by qt3to4:
#include <QList>


namespace KPlato
{

ResourceGroup::ResourceGroup()
    : QObject( 0 ) 
{
    m_project = 0;
    m_type = Type_Work;
    //kDebug(planDbg())<<"("<<this<<")";
}

ResourceGroup::ResourceGroup( const ResourceGroup *group )
    : QObject( 0 ) 
{
    m_project = 0;
    copy( group );
}

ResourceGroup::~ResourceGroup() {
    //kDebug(planDbg())<<"("<<this<<")";
    if (findId() == this) {
        removeId(); // only remove myself (I may be just a working copy)
    }
    foreach ( ResourceGroupRequest* r, m_requests ) {
        r->unregister( this );
    }
    while (!m_resources.isEmpty()) {
        delete m_resources.takeFirst();
    }
    //kDebug(planDbg())<<"("<<this<<")";
}

void ResourceGroup::copy( const ResourceGroup *group )
{
    //m_project = group->m_project; //Don't copy
    m_id = group->m_id;
    m_type = group->m_type;
    m_name = group->m_name;
}

void ResourceGroup::changed() {
    if ( m_project ) {
        m_project->changed( this );
    }
}

void ResourceGroup::setId(const QString& id) {
    //kDebug(planDbg())<<id;
    m_id = id;
}

void ResourceGroup::setName( const QString& n )
{
    m_name = n.trimmed();
    changed();
}

void ResourceGroup::setType( Type type )
{
     m_type = type;
     changed();
}

void ResourceGroup::setType(const QString &type)
{
    if (type == "Work")
        setType( Type_Work );
    else if (type == "Material")
        setType( Type_Material );
    else
        setType( Type_Work );
}

QString ResourceGroup::typeToString( bool trans ) const {
    return typeToStringList( trans ).at( m_type );
}

QStringList ResourceGroup::typeToStringList( bool trans ) {
    // keep theese in the same order as the enum!
    return QStringList() 
            << (trans ? i18n("Work") : QString("Work"))
            << (trans ? i18n("Material") : QString("Material"));
}

void ResourceGroup::setProject( Project *project )
{
    if ( project != m_project ) {
        if ( m_project  ) {
            removeId();
        }
    }
    m_project = project;
    foreach ( Resource *r, m_resources ) {
        r->setProject( project );
    }
}

bool ResourceGroup::isScheduled() const
{
    foreach ( Resource *r, m_resources ) {
        if ( r->isScheduled() ) {
            return true;
        }
    }
    return false;
}

bool ResourceGroup::isBaselined( long id ) const
{
    Q_UNUSED(id);
    foreach ( const Resource *r, m_resources ) {
        if ( r->isBaselined() ) {
            return true;
        }
    }
    return false;
}


void ResourceGroup::addResource(int index, Resource* resource, Risk*) {
    int i = index == -1 ? m_resources.count() : index;
    resource->setParentGroup( this );
    resource->setProject( m_project );
    m_resources.insert(i, resource );
}

Resource *ResourceGroup::takeResource(Resource *resource) {
    Resource *r = 0;
    int i = m_resources.indexOf(resource);
    if (i != -1) {
        r = m_resources.takeAt(i);
        r->setParentGroup( 0 );
        r->setProject( 0 );
    }
    return r;
}

int ResourceGroup::indexOf( const Resource *resource ) const
{
    return m_resources.indexOf( const_cast<Resource*>( resource ) ); //???
}

Risk* ResourceGroup::getRisk(int) {
    return 0L;
}

void ResourceGroup::addRequiredResource(ResourceGroup*) {
}

ResourceGroup* ResourceGroup::getRequiredResource(int) {
    return 0L;
}

void ResourceGroup::deleteRequiredResource(int) {
}

bool ResourceGroup::load(KoXmlElement &element, XMLLoaderObject &status ) {
    //kDebug(planDbg());
    setId(element.attribute("id"));
    m_name = element.attribute("name");
    setType(element.attribute("type"));

    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if (e.tagName() == "resource") {
            // Load the resource
            Resource *child = new Resource();
            if (child->load(e, status)) {
                addResource( -1, child, 0 );
            } else {
                // TODO: Complain about this
                delete child;
            }
        }
    }
    return true;
}

void ResourceGroup::save(QDomElement &element)  const {
    //kDebug(planDbg());

    QDomElement me = element.ownerDocument().createElement("resource-group");
    element.appendChild(me);

    me.setAttribute("id", m_id);
    me.setAttribute("name", m_name);
    me.setAttribute("type", typeToString());

    foreach (Resource *r, m_resources) {
        r->save(me);
    }
}

void ResourceGroup::saveWorkPackageXML( QDomElement &element, const QList<Resource*> lst ) const
{
    QDomElement me = element.ownerDocument().createElement( "resource-group" );
    element.appendChild( me );

    me.setAttribute( "id", m_id );
    me.setAttribute( "name", m_name );

    foreach ( Resource *r, m_resources ) {
        if ( lst.contains( r ) ) {
            r->save( me );
        }
    }
}

void ResourceGroup::initiateCalculation(Schedule &sch) {
    foreach (Resource *r, m_resources) {
        r->initiateCalculation(sch);
    }
    clearNodes();
}

int ResourceGroup::units() const {
    int u = 0;
    foreach ( const Resource *r, m_resources) {
        u += r->units();
    }
    return u;
}

ResourceGroup *ResourceGroup::findId(const QString &id) const {
    return m_project ? m_project->findResourceGroup(id) : 0;
}

bool ResourceGroup::removeId(const QString &id) { 
    return m_project ? m_project->removeResourceGroupId(id): false;
}

void ResourceGroup::insertId(const QString &id) { 
    //kDebug(planDbg());
    if (m_project)
        m_project->insertResourceGroupId(id, this);
}

Appointment ResourceGroup::appointmentIntervals() const {
    Appointment a;
    foreach (Resource *r, m_resources) {
        a += r->appointmentIntervals();
    }
    return a;
}

DateTime ResourceGroup::startTime( long id ) const
{
    DateTime dt;
    foreach ( Resource *r, m_resources ) {
        DateTime t = r->startTime( id );
        if ( ! dt.isValid() || t < dt ) {
            dt = t;
        }
    }
    return dt;
}

DateTime ResourceGroup::endTime( long id ) const
{
    DateTime dt;
    foreach ( Resource *r, m_resources ) {
        DateTime t = r->endTime( id );
        if ( ! dt.isValid() || t > dt ) {
            dt = t;
        }
    }
    return dt;
}


Resource::Resource()
    : QObject( 0 ), // atm QObject is only for casting
    m_project(0),
    m_parent( 0 ),
    m_autoAllocate( false ),
    m_currentSchedule( 0 )
{
    m_type = Type_Work;
    m_units = 100; // %

//     m_availableFrom = DateTime( QDate::currentDate(), QTime( 0, 0, 0 ) );
//     m_availableUntil = m_availableFrom.addYears(2);

    cost.normalRate = 100;
    cost.overtimeRate = 0;
    cost.fixed = 0;
    cost.account = 0;
    m_calendar = 0;
    m_currentSchedule = 0;
    //kDebug(planDbg())<<"("<<this<<")";
    
    // material: by default material is always available
    for ( int i = 1; i <= 7; ++i ) {
        CalendarDay *wd = m_materialCalendar.weekday( i );
        wd->setState( CalendarDay::Working );
        wd->addInterval( TimeInterval( QTime( 0, 0, 0 ), 24*60*60*1000 ) );
    }
}

Resource::Resource(Resource *resource)
    : QObject( 0 ), // atm QObject is only for casting
    m_project( 0 ),
    m_parent( 0 ),
    m_currentSchedule( 0 )
{
    //kDebug(planDbg())<<"("<<this<<") from ("<<resource<<")";
    copy(resource); 
}

Resource::~Resource() {
    //kDebug(planDbg())<<"("<<this<<")";
    if (findId() == this) {
        removeId(); // only remove myself (I may be just a working copy)
    }
    removeRequests();
    foreach ( Schedule *s, m_schedules ) {
        delete s;
    }
    clearExternalAppointments();
    if (cost.account) {
        cost.account->removeRunning(*this);
    }
}

void Resource::removeRequests() {
    foreach (ResourceRequest *r, m_requests) {
        r->setResource(0); // avoid the request to mess with my list
        r->parent()->deleteResourceRequest(r);
    }
    m_requests.clear();
}

void Resource::setId(const QString& id) {
    //kDebug(planDbg())<<id;
    m_id = id;
}

void Resource::copy(Resource *resource) {
    m_project = 0; // NOTE: Don't copy, will be set when added to a project
    //m_appointments = resource->appointments(); // Note
    m_id = resource->id();
    m_name = resource->name();
    m_initials = resource->initials();
    m_email = resource->email();
    m_autoAllocate = resource->m_autoAllocate;
    m_availableFrom = resource->availableFrom();
    m_availableUntil = resource->availableUntil();
    
    m_units = resource->units(); // available units in percent

    m_type = resource->type();

    cost.normalRate = resource->normalRate();
    cost.overtimeRate = resource->overtimeRate();
    cost.account = resource->account();
    m_calendar = resource->m_calendar;

    m_requiredIds = resource->requiredIds();
    m_teamMembers = resource->m_teamMembers;

    // hmmmm
    //m_externalAppointments = resource->m_externalAppointments;
    //m_externalNames = resource->m_externalNames;
}

void Resource::changed()
{
    if ( m_project ) {
        m_project->changed( this );
    }
}

void Resource::setType( Type type )
{
    m_type = type;
    changed();
}

void Resource::setType(const QString &type)
{
    if (type == "Work")
        setType( Type_Work );
    else if (type == "Material")
        setType( Type_Material );
    else if (type == "Team")
        setType( Type_Team );
    else
        setType( Type_Work );
}

QString Resource::typeToString( bool trans ) const {
    return typeToStringList( trans ).at( m_type );
}

QStringList Resource::typeToStringList( bool trans ) {
    // keep theese in the same order as the enum!
    return QStringList() 
            << (trans ? i18nc( "@item:inlistbox resource type", "Work" ) : QString( "Work") )
            << (trans ? i18nc( "@item:inlistbox resource type", "Material" ) : QString( "Material" ) )
            << (trans ? i18nc( "@item:inlistbox resource type", "Team" ) : QString( "Team" ) );
}

void Resource::setName( const QString n )
{
    m_name = n.trimmed();
    changed();
}

void Resource::setInitials( const QString initials )
{
    m_initials = initials.trimmed();
    changed();
}

void Resource::setEmail( const QString email )
{
    m_email = email;
    changed();
}

bool Resource::autoAllocate() const
{
    return m_autoAllocate;
}

void Resource::setAutoAllocate( bool on )
{
    if ( m_autoAllocate != on ) {
        m_autoAllocate = on;
        changed();
    }
}

void Resource::setUnits( int units )
{
    m_units = units;
    m_workinfocache.clear();
    changed();
}

Calendar *Resource::calendar( bool local ) const {
    if ( local || m_calendar ) {
        return m_calendar;
    }
    // No calendar is set, try default calendar
    Calendar *c = 0;
    if ( m_type == Type_Work && project() ) {
        c =  project()->defaultCalendar();
    } else if ( m_type == Type_Material ) {
        c = const_cast<Calendar*>( &m_materialCalendar );
    }
    return c;
}

void Resource::setCalendar( Calendar *calendar )
{
    m_calendar = calendar;
    m_workinfocache.clear();
    changed();
}

DateTime Resource::firstAvailableAfter(const DateTime &, const DateTime & ) const {
    return DateTime();
}

DateTime Resource::getBestAvailableTime(Duration /*duration*/) {
    return DateTime();
}

DateTime Resource::getBestAvailableTime(const DateTime /*after*/, const Duration /*duration*/) {
    return DateTime();
}

bool Resource::load(KoXmlElement &element, XMLLoaderObject &status) {
    //kDebug(planDbg());
    const KLocale *locale = status.project().locale();
    QString s;
    setId(element.attribute("id"));
    m_name = element.attribute("name");
    m_initials = element.attribute("initials");
    m_email = element.attribute("email");
    m_autoAllocate = (bool)(element.attribute( "auto-allocate", "0" ).toInt());
    setType(element.attribute("type"));
    m_calendar = status.project().findCalendar(element.attribute("calendar-id"));
    m_units = element.attribute("units", "100").toInt();
    s = element.attribute("available-from");
    if (!s.isEmpty())
        m_availableFrom = DateTime::fromString(s, status.projectSpec());
    s = element.attribute("available-until");
    if (!s.isEmpty())
        m_availableUntil = DateTime::fromString(s, status.projectSpec());
        
    cost.normalRate = locale->readMoney(element.attribute("normal-rate"));
    cost.overtimeRate = locale->readMoney(element.attribute("overtime-rate"));
    cost.account = status.project().accounts().findAccount(element.attribute("account"));
    
    KoXmlElement e;
    KoXmlElement parent = element.namedItem( "required-resources" ).toElement();
    forEachElement( e, parent ) {
        if (e.nodeName() == "resource") {
            QString id = e.attribute( "id" );
            if ( id.isEmpty() ) {
                kWarning()<<"Missing resource id";
                continue;
            }
            addRequiredId( id );
        }
    }
    parent = element.namedItem( "external-appointments" ).toElement();
    forEachElement( e, parent ) {
        if ( e.nodeName() == "project" ) {
            QString id = e.attribute( "id" );
            if ( id.isEmpty() ) {
                kError()<<"Missing project id";
                continue;
            }
            clearExternalAppointments( id ); // in case...
            AppointmentIntervalList lst;
            lst.loadXML( e, status );
            Appointment *a = new Appointment();
            a->setIntervals( lst );
            a->setAuxcilliaryInfo( e.attribute( "name", "Unknown" ) );
            m_externalAppointments[ id ] = a;
        }
    }
    loadCalendarIntervalsCache( element, status );
    return true;
}

QList<Resource*> Resource::requiredResources() const
{
    QList<Resource*> lst;
    foreach ( const QString &s, m_requiredIds ) {
        Resource *r = findId( s );
        if ( r ) {
            lst << r;
        }
    }
    return lst;
}

void Resource::setRequiredIds( const QStringList &ids )
{
    kDebug(planDbg())<<ids;
    m_requiredIds = ids;
}

void Resource::addRequiredId( const QString &id )
{
    if ( ! id.isEmpty() && ! m_requiredIds.contains( id ) ) {
        m_requiredIds << id;
    }
}


void Resource::setAccount( Account *account )
{
    if ( cost.account ) {
        cost.account->removeRunning(*this);
    }
    cost.account = account;
    changed();
}

void Resource::save(QDomElement &element) const {
    //kDebug(planDbg());
    QDomElement me = element.ownerDocument().createElement("resource");
    element.appendChild(me);

    if (calendar(true))
        me.setAttribute("calendar-id", m_calendar->id());
    me.setAttribute("id", m_id);
    me.setAttribute("name", m_name);
    me.setAttribute("initials", m_initials);
    me.setAttribute("email", m_email);
    me.setAttribute("auto-allocate", m_autoAllocate );
    me.setAttribute("type", typeToString());
    me.setAttribute("units", m_units);
    if ( m_availableFrom.isValid() ) {
        me.setAttribute("available-from", m_availableFrom.toString( Qt::ISODate ));
    }
    if ( m_availableUntil.isValid() ) {
        me.setAttribute("available-until", m_availableUntil.toString( Qt::ISODate ));
    }
    me.setAttribute("normal-rate", m_project->locale()->formatMoney(cost.normalRate));
    me.setAttribute("overtime-rate", m_project->locale()->formatMoney(cost.overtimeRate));
    if ( cost.account ) {
        me.setAttribute("account", cost.account->name());
    }
    
    if ( ! m_requiredIds.isEmpty() ) {
        QDomElement e = me.ownerDocument().createElement("required-resources");
        me.appendChild(e);
        foreach ( const QString &id, m_requiredIds ) {
            QDomElement el = e.ownerDocument().createElement("resource");
            e.appendChild( el );
            el.setAttribute( "id", id );
        }
    }

    if ( ! m_externalAppointments.isEmpty() ) {
        QDomElement e = me.ownerDocument().createElement("external-appointments");
        me.appendChild(e);
        foreach ( const QString &id, m_externalAppointments.uniqueKeys() ) {
            QDomElement el = e.ownerDocument().createElement("project");
            e.appendChild( el );
            el.setAttribute( "id", id );
            el.setAttribute( "name", m_externalAppointments[ id ]->auxcilliaryInfo() );
            m_externalAppointments[ id ]->intervals().saveXML( el );
        }
    }
    saveCalendarIntervalsCache( me );
}

bool Resource::isAvailable(Task * /*task*/) {
    bool busy = false;
/*
    foreach (Appointment *a, m_appointments) {
        if (a->isBusy(task->startTime(), task->endTime())) {
            busy = true;
            break;
        }
    }*/
    return !busy;
}

QList<Appointment*> Resource::appointments( long id ) const {
    Schedule *s = schedule( id );
    if ( s == 0 ) {
        return QList<Appointment*>();
    }
    return s->appointments();
}

bool Resource::addAppointment(Appointment *appointment) {
    if (m_currentSchedule)
        return m_currentSchedule->add(appointment);
    return false;
}

bool Resource::addAppointment(Appointment *appointment, Schedule &main) {
    Schedule *s = findSchedule(main.id());
    if (s == 0) {
        s = createSchedule(&main);
    }
    appointment->setResource(s);
    return s->add(appointment);
}

// called from makeAppointment
void Resource::addAppointment( Schedule *node, const DateTime &start, const DateTime &end, double load )
{
    Q_ASSERT( start < end );
    Schedule *s = findSchedule(node->id());
    if (s == 0) {
        s = createSchedule(node->parent());
    }
    s->setCalculationMode( node->calculationMode() );
    //kDebug(planDbg())<<"id="<<node->id()<<" Mode="<<node->calculationMode()<<""<<start<<end;
    s->addAppointment(node, start, end, load);
}

void Resource::initiateCalculation(Schedule &sch) {
    m_currentSchedule = createSchedule(&sch);
}

Schedule *Resource::schedule( long id ) const
{
    return id == -1 ? m_currentSchedule : findSchedule( id );
}

bool Resource::isBaselined( long id ) const
{
    if ( m_type == Resource::Type_Team ) {
        foreach ( const Resource *r, teamMembers() ) {
            if ( r->isBaselined( id ) ) {
                return true;
            }
        }
        return false;
    }
    Schedule *s = schedule( id );
    return s ? s->isBaselined() : false;
}

Schedule *Resource::findSchedule( long id ) const
{
    if ( m_schedules.contains( id ) ) {
        return m_schedules[ id ];
    }
    if ( id == CURRENTSCHEDULE ) {
        return m_currentSchedule;
    }
    if ( id == BASELINESCHEDULE || id == ANYSCHEDULED ) {
        foreach ( Schedule *s, m_schedules ) {
            if ( s->isBaselined() ) {
                return s;
            }
        }
    }
    if ( id == ANYSCHEDULED ) {
        foreach ( Schedule *s, m_schedules ) {
            if ( s->isScheduled() ) {
                return s;
            }
        }
    }
    return 0;
}

bool Resource::isScheduled() const
{
    foreach ( Schedule *s, m_schedules ) {
        if ( s->isScheduled() ) {
            return true;
        }
    }
    return false;
}

void Resource::deleteSchedule(Schedule *schedule) {
    takeSchedule(schedule);
    delete schedule;
}

void Resource::takeSchedule(const Schedule *schedule) {
    if (schedule == 0)
        return;
    if (m_currentSchedule == schedule)
        m_currentSchedule = 0;
    m_schedules.take(schedule->id());
}

void Resource::addSchedule(Schedule *schedule) {
    if (schedule == 0)
        return;
    m_schedules.remove(schedule->id());
    m_schedules.insert(schedule->id(), schedule);
}

ResourceSchedule *Resource::createSchedule(const QString& name, int type, long id) {
    ResourceSchedule *sch = new ResourceSchedule(this, name, (Schedule::Type)type, id);
    addSchedule(sch);
    return sch;
}

ResourceSchedule *Resource::createSchedule(Schedule *parent) {
    ResourceSchedule *sch = new ResourceSchedule(parent, this);
    //kDebug(planDbg())<<"id="<<sch->id();
    addSchedule(sch);
    return sch;
}

KDateTime::Spec Resource::timeSpec() const
{
    Calendar *cal = calendar();
    if ( cal ) {
        return cal->timeSpec();
    }
    if ( m_project ) {
        return m_project->timeSpec();
    }
    return KDateTime::Spec::LocalZone();
}

DateTimeInterval Resource::requiredAvailable(Schedule *node, const DateTime &start, const DateTime &end ) const
{
    Q_ASSERT( m_currentSchedule );
    DateTimeInterval interval( start, end );
#ifndef PLAN_NLOGDEBUG
    if (m_currentSchedule) m_currentSchedule->logDebug( QString( "Required available in interval: %1" ).arg( interval.toString() ) );
#endif
    DateTime availableFrom = m_availableFrom.isValid() ? m_availableFrom : ( m_project ? m_project->constraintStartTime() : DateTime() );
    DateTime availableUntil = m_availableUntil.isValid() ? m_availableUntil : ( m_project ? m_project->constraintEndTime() : DateTime() );
    DateTimeInterval x = interval.limitedTo( availableFrom, availableUntil );
    if ( calendar() == 0 ) {
#ifndef PLAN_NLOGDEBUG
        if (m_currentSchedule) m_currentSchedule->logDebug( QString( "Required available: no calendar, %1" ).arg( x.toString() ) );
#endif
        return x;
    }
    DateTimeInterval i = m_currentSchedule->firstBookedInterval( x, node );
    if ( i.isValid() ) {
#ifndef PLAN_NLOGDEBUG
        if (m_currentSchedule) m_currentSchedule->logDebug( QString( "Required available: booked, %1" ).arg( i.toString() ) );
#endif
        return i; 
    }
    i = calendar()->firstInterval(x.first, x.second, m_currentSchedule);
#ifndef PLAN_NLOGDEBUG
    if (m_currentSchedule) m_currentSchedule->logDebug( QString( "Required first available in %1:  %2" ).arg( x.toString() ).arg( i.toString() ) );
#endif
    return i;
}

void Resource::makeAppointment(Schedule *node, const DateTime &from, const DateTime &end, int load, const QList<Resource*> &required ) {
    //kDebug(planDbg())<<"node id="<<node->id()<<" mode="<<node->calculationMode()<<""<<from<<" -"<<end;
    KLocale *locale = KGlobal::locale();
    Q_UNUSED(locale);
    if (!from.isValid() || !end.isValid()) {
        m_currentSchedule->logWarning( i18n( "Make appointments: Invalid time" ) );
        return;
    }
    Calendar *cal = calendar();
    if (cal == 0) {
        m_currentSchedule->logWarning( i18n( "Resource %1 has no calendar defined", m_name ) );
        return;
    }
#ifndef PLAN_NLOGDEBUG
    if ( m_currentSchedule ) {
        QStringList lst; foreach ( Resource *r, required ) { lst << r->name(); }
        m_currentSchedule->logDebug( QString( "Make appointments from %1 to %2 load=%4, required: %3" ).arg( from.toString() ).arg( end.toString() ).arg( lst.join(",") ).arg( load ) );
    }
#endif
    AppointmentIntervalList lst = workIntervals( from, end );
    foreach ( const AppointmentInterval &i, lst.map() ) {
        m_currentSchedule->addAppointment( node, i.startTime(), i.endTime(), load );
        foreach ( Resource *r, required ) {
            r->addAppointment( node, i.startTime(), i.endTime(), r->units() ); //FIXME: units may not be correct
        }
    }
}

void Resource::makeAppointment(Schedule *node, int load, const QList<Resource*> &required) {
    //kDebug(planDbg())<<m_name<<": id="<<m_currentSchedule->id()<<" mode="<<m_currentSchedule->calculationMode()<<node->node()->name()<<": id="<<node->id()<<" mode="<<node->calculationMode()<<""<<node->startTime;
    KLocale *locale = KGlobal::locale();
    if (!node->startTime.isValid()) {
        m_currentSchedule->logWarning( i18n( "Make appointments: Node start time is not valid" ) );
        return;
    }
    if (!node->endTime.isValid()) {
        m_currentSchedule->logWarning( i18n( "Make appointments: Node end time is not valid" ) );
        return;
    }
    if ( m_type == Type_Team ) {
#ifndef PLAN_NLOGDEBUG
        m_currentSchedule->logDebug( "Make appointments to team " + m_name );
#endif
        Duration e;
        foreach ( Resource *r, teamMembers() ) {
            r->makeAppointment( node, load, required );
        }
        return;
    }
    node->resourceNotAvailable = false;
    node->workStartTime = DateTime();
    node->workEndTime = DateTime();
    Calendar *cal = calendar();
    if (m_type == Type_Material) {
        DateTime from = availableAfter(node->startTime, node->endTime);
        DateTime end = availableBefore(node->endTime, node->startTime);
        if (!from.isValid() || !end.isValid()) {
            return;
        }
        if (cal == 0) {
            // Allocate the whole period
            addAppointment(node, from, end, m_units);
            return;
        }
        makeAppointment(node, from, end, load);
        return;
    }
    if (!cal) {
        m_currentSchedule->logWarning( i18n( "Resource %1 has no calendar defined", m_name ) );
        return; 
    }
    DateTime time = node->startTime;
    DateTime end = node->endTime;
    time = availableAfter(time, end);
    if (!time.isValid()) {
        m_currentSchedule->logWarning( i18n( "Resource %1 not available in interval: %2 to %3", m_name, locale->formatDateTime( node->startTime ), locale->formatDateTime( end ) ) );
        node->resourceNotAvailable = true;
        return;
    }
    end = availableBefore(end, time);
    foreach ( Resource *r, required ) {
        time = r->availableAfter( time, end );
        end = r->availableBefore( end, time );
        if ( ! ( time.isValid() && end.isValid() ) ) {
#ifndef PLAN_NLOGDEBUG
            if ( m_currentSchedule ) m_currentSchedule->logDebug( "The required resource '" + r->name() + "'is not available in interval:" + node->startTime.toString() + "," + node->endTime.toString() );
#endif
            break;
        }
    }
    if (!end.isValid()) {
        m_currentSchedule->logWarning( i18n( "Resource %1 not available in interval: %2 to %3", m_name, locale->formatDateTime( time ), locale->formatDateTime( node->endTime ) ) );
        node->resourceNotAvailable = true;
        return;
    }
    //kDebug(planDbg())<<time.toString()<<" to"<<end.toString();
    makeAppointment(node, time, end, load, required);
}

AppointmentIntervalList Resource::workIntervals( const DateTime &from, const DateTime &until ) const
{
    return workIntervals( from, until, 0 );
}

AppointmentIntervalList Resource::workIntervals( const DateTime &from, const DateTime &until, Schedule *sch ) const
{
    Calendar *cal = calendar();
    if ( cal == 0 ) {
        return AppointmentIntervalList();
    }
    // update cache
    calendarIntervals( from, until );
    AppointmentIntervalList work = m_workinfocache.intervals.extractIntervals( from, until );
    if ( sch && ! sch->allowOverbooking() ) {
        foreach ( const Appointment *a, sch->appointments( sch->calculationMode() ) ) {
            work -= a->intervals();
        }
        foreach ( const Appointment *a, m_externalAppointments ) {
            work -= a->intervals();
        }
    }
    return work;
}

void Resource::calendarIntervals( const DateTime &from, const DateTime &until ) const
{
    Calendar *cal = calendar();
    if ( cal == 0 ) {
        m_workinfocache.clear();
        return;
    }
    if ( cal->cacheVersion() != m_workinfocache.version ) {
        m_workinfocache.clear();
        m_workinfocache.version = cal->cacheVersion();
    }
    if ( ! m_workinfocache.isValid() ) {
        // First time
//         kDebug(planDbg())<<"First time:"<<from<<until;
        m_workinfocache.start = from;
        m_workinfocache.end = until;
        m_workinfocache.intervals = cal->workIntervals( from, until, m_units );
//         kDebug(planDbg())<<"calendarIntervals (first):"<<m_workinfocache.intervals;
    } else {
        if ( from < m_workinfocache.start ) {
//             kDebug(planDbg())<<"Add to start:"<<from<<m_workinfocache.start;
            m_workinfocache.intervals += cal->workIntervals( from, m_workinfocache.start, m_units );
            m_workinfocache.start = from;
//             kDebug(planDbg())<<"calendarIntervals (start):"<<m_workinfocache.intervals;
        }
        if ( until > m_workinfocache.end ) {
//             kDebug(planDbg())<<"Add to end:"<<m_workinfocache.end<<until;
            m_workinfocache.intervals += cal->workIntervals( m_workinfocache.end, until, m_units );
            m_workinfocache.end = until;
//             kDebug(planDbg())<<"calendarIntervals: (end)"<<m_workinfocache.intervals;
        }
    }
}

bool Resource::loadCalendarIntervalsCache( const KoXmlElement &element, XMLLoaderObject &status )
{
    KoXmlElement e = element.namedItem( "work-intervals-cache" ).toElement();
    if ( e.isNull() ) {
        kError()<<"No 'work-intervals-cache' element";
        return true;
    }
    m_workinfocache.load( e, status );
    return true;
}

void Resource::saveCalendarIntervalsCache( QDomElement &element ) const
{
    QDomElement me = element.ownerDocument().createElement("work-intervals-cache");
    element.appendChild(me);
    m_workinfocache.save( me );
}

DateTime Resource::WorkInfoCache::firstAvailableAfter( const DateTime &time, const DateTime &limit, Calendar *cal, Schedule *sch ) const
{
    QMultiMap<QDate, AppointmentInterval>::const_iterator it = intervals.map().constEnd();
    if ( start.isValid() && start <= time ) {
        // possibly useful cache
        it = intervals.map().lowerBound( time.date() );
    }
    if ( it == intervals.map().constEnd() ) {
        // nothing cached, check the old way
        DateTime t = cal ? cal->firstAvailableAfter( time, limit, sch ) : DateTime();
        return t;
    }
    AppointmentInterval inp( time, limit );
    for ( ; it != intervals.map().constEnd() && it.key() <= limit.date(); ++it ) {
        if ( ! it.value().intersects( inp ) && it.value() < inp ) {
            continue;
        }
        if ( sch ) {
            DateTimeInterval ti = sch->available( DateTimeInterval( it.value().startTime(), it.value().endTime() ) );
            if ( ti.isValid() && ti.first < limit ) {
                ti.first = qMax( ti.first, time );
                return ti.first;
            }
        } else {
            DateTime t = qMax( it.value().startTime(), time );
            return t;
        }
    }
    if ( it == intervals.map().constEnd() ) {
        // ran out of cache, check the old way
        DateTime t = cal ? cal->firstAvailableAfter( time, limit, sch ) : DateTime();
        return t;
    }
    return DateTime();
}

DateTime Resource::WorkInfoCache::firstAvailableBefore( const DateTime &time, const DateTime &limit, Calendar *cal, Schedule *sch ) const
{
    if ( time <= limit ) {
        return DateTime();
    }
    QMultiMap<QDate, AppointmentInterval>::const_iterator it = intervals.map().constBegin();
    if ( time.isValid() && limit.isValid() && end.isValid() && end >= time && ! intervals.isEmpty() ) {
        // possibly useful cache
        it = intervals.map().upperBound( time.date() );
    }
    if ( it == intervals.map().constBegin() ) {
        // nothing cached, check the old way
        DateTime t = cal ? cal->firstAvailableBefore( time, limit, sch ) : DateTime();
        return t;
    }
    AppointmentInterval inp( limit, time );
    for ( --it; it != intervals.map().constBegin() && it.key() >= limit.date(); --it ) {
        if ( ! it.value().intersects( inp ) && inp < it.value() ) {
            continue;
        }
        if ( sch ) {
            DateTimeInterval ti = sch->available( DateTimeInterval( it.value().startTime(), it.value().endTime() ) );
            if ( ti.isValid() && ti.second > limit ) {
                ti.second = qMin( ti.second, time );
                return ti.second;
            }
        } else {
            DateTime t = qMin( it.value().endTime(), time );
            return t;
        }
    }
    if ( it == intervals.map().constBegin() ) {
        // ran out of cache, check the old way
        DateTime t = cal ? cal->firstAvailableBefore( time, limit, sch ) : DateTime();
        return t;
    }
    return DateTime();
}

bool Resource::WorkInfoCache::load( const KoXmlElement &element, XMLLoaderObject &status )
{
    clear();
    version = element.attribute( "version" ).toInt();
    effort = Duration::fromString( element.attribute( "effort" ) );
    start = DateTime::fromString( element.attribute( "start" ) );
    end = DateTime::fromString( element.attribute( "end" ) );
    KoXmlElement e = element.namedItem( "intervals" ).toElement();
    if ( ! e.isNull() ) {
        intervals.loadXML( e, status );
    }
    //kDebug(planDbg())<<*this;
    return true;
}

void Resource::WorkInfoCache::save( QDomElement &element ) const
{
    element.setAttribute( "version", version );
    element.setAttribute( "effort", effort.toString() );
    element.setAttribute( "start", start.toString( Qt::ISODate ) );
    element.setAttribute( "end", end.toString( Qt::ISODate ) );
    QDomElement me = element.ownerDocument().createElement("intervals");
    element.appendChild(me);

    intervals.saveXML( me );
}

Duration Resource::effort( const DateTime& start, const Duration& duration, int units, bool backward, const QList< Resource* >& required ) const
{
    return effort( m_currentSchedule, start, duration, units, backward, required );
}

// the amount of effort we can do within the duration
Duration Resource::effort( Schedule *sch, const DateTime &start, const Duration &duration, int units, bool backward, const QList<Resource*> &required ) const
{
    //kDebug(planDbg())<<m_name<<": ("<<(backward?"B )":"F )")<<start<<" for duration"<<duration.toString(Duration::Format_Day);
#if 0
    if ( sch ) sch->logDebug( QString( "Check effort in interval %1: %2, %3").arg(backward?"backward":"forward").arg( start.toString() ).arg( (backward?start-duration:start+duration).toString() ) );
#endif
    Duration e;
    if ( duration == 0 || m_units == 0 || units == 0 ) {
        kWarning()<<"zero duration or zero units";
        return e;
    }
    if ( m_type == Type_Team ) {
        kError()<<"A team resource cannot deliver any effort";
        return e;
    }
    Calendar *cal = calendar();
    if ( cal == 0 ) {
        if ( sch ) sch->logWarning( i18n( "Resource %1 has no calendar defined", m_name ) );
        return e;
    }
    DateTime from;
    DateTime until;
    if ( backward ) {
        from = availableAfter( start - duration, start, sch );
        until = availableBefore( start, start - duration, sch );
    } else {
        from = availableAfter( start, start + duration, sch );
        until = availableBefore( start + duration, start, sch );
    }
    if ( ! ( from.isValid() && until.isValid() ) ) {
#ifndef PLAN_NLOGDEBUG
        if ( sch ) sch->logDebug( "Resource not available in interval:" + start.toString() + "," + (start+duration).toString() );
#endif
    } else {
        foreach ( Resource *r, required ) {
            from = r->availableAfter( from, until );
            until = r->availableBefore( until, from );
            if ( ! ( from.isValid() && until.isValid() ) ) {
#ifndef PLAN_NLOGDEBUG
                if ( sch ) sch->logDebug( "The required resource '" + r->name() + "'is not available in interval:" + start.toString() + "," + (start+duration).toString() );
#endif
                    break;
            }
        }
    }
    if ( from.isValid() && until.isValid() ) {
#ifndef PLAN_NLOGDEBUG
        if ( sch && until < from ) sch->logDebug( " until < from: until=" + until.toString() + " from=" + from.toString() );
#endif
        e = workIntervals( from, until ).effort( from, until ) * units / 100;
        if ( sch && ( ! sch->allowOverbooking() || sch->allowOverbookingState() == Schedule::OBS_Deny ) ) {
            Duration avail = workIntervals( from, until, sch ).effort( from, until );
            if ( avail < e ) {
                e = avail;
            }
        }
//        e = ( cal->effort( from, until, sch ) ) * m_units / 100;
    }
    //kDebug(planDbg())<<m_name<<start<<" e="<<e.toString(Duration::Format_Day)<<" ("<<m_units<<")";
#ifndef PLAN_NLOGDEBUG
    if ( sch ) sch->logDebug( QString( "effort: %1 for %2 hours = %3" ).arg( start.toString() ).arg( duration.toString( Duration::Format_HourFraction ) ).arg( e.toString( Duration::Format_HourFraction ) ) );
#endif
    return e;
}

DateTime Resource::availableAfter(const DateTime &time, const DateTime limit ) const {
    return availableAfter( time, limit, m_currentSchedule );
}

DateTime Resource::availableBefore(const DateTime &time, const DateTime limit) const {
    return availableBefore( time, limit, m_currentSchedule );
}

DateTime Resource::availableAfter(const DateTime &time, const DateTime limit, Schedule *sch) const {
    DateTime t;
    if (m_units == 0) {
        return t;
    }
    DateTime lmt = m_availableUntil.isValid() ? m_availableUntil : ( m_project ? m_project->constraintEndTime() : DateTime() );
    if (limit.isValid() && limit < lmt) {
        lmt = limit;
    }
    if (time >= lmt) {
        return t;
    }
    Calendar *cal = calendar();
    if (cal == 0) {
        if ( sch ) sch->logWarning( i18n( "Resource %1 has no calendar defined", m_name ) );
        return t;
    }
    DateTime availableFrom = m_availableFrom.isValid() ? m_availableFrom : ( m_project ? m_project->constraintStartTime() : DateTime() );
    t = availableFrom > time ? availableFrom : time;
    if ( t >= lmt ) {
        //kDebug(planDbg())<<t<<lmt;
        return DateTime();
    }
    t = m_workinfocache.firstAvailableAfter( t, lmt, cal, sch );
//    t = cal->firstAvailableAfter(t, lmt, sch);
    //kDebug(planDbg())<<m_currentSchedule<<""<<m_name<<" id="<<m_currentSchedule->id()<<" mode="<<m_currentSchedule->calculationMode()<<" returns:"<<time.toString()<<"="<<t.toString()<<""<<lmt.toString();
    return t;
}

DateTime Resource::availableBefore(const DateTime &time, const DateTime limit, Schedule *sch) const {
    DateTime t;
    if (m_units == 0) {
        return t;
    }
    DateTime lmt = m_availableFrom.isValid() ? m_availableFrom : ( m_project ? m_project->constraintStartTime() : DateTime() );
    if (limit.isValid() && limit > lmt) {
        lmt = limit;
    }
    if (time <= lmt) {
        return t;
    }
    Calendar *cal = calendar();
    if (cal == 0) {
        return t;
    }
    DateTime availableUntil = m_availableUntil.isValid() ? m_availableUntil : ( m_project ? m_project->constraintEndTime() : DateTime() );
    if ( ! availableUntil.isValid() ) {
#ifndef PLAN_NLOGDEBUG
        if ( sch ) sch->logDebug( "availabelUntil is invalid" );
#endif
        t = time;
    } else {
        t = availableUntil < time ? availableUntil : time;
    }
#ifndef PLAN_NLOGDEBUG
    if ( sch && t < lmt ) sch->logDebug( "t < lmt: " + t.toString() + " < " + lmt.toString() );
#endif
    t = m_workinfocache.firstAvailableBefore( t, lmt, cal, sch );
//    t = cal->firstAvailableBefore(t, lmt, sch );
#ifndef PLAN_NLOGDEBUG
    if ( sch && t.isValid() && t < lmt ) sch->logDebug( " t < lmt: t=" + t.toString() + " lmt=" + lmt.toString() );
#endif
    return t;
}

Resource *Resource::findId(const QString &id) const { 
    return m_project ? m_project->findResource(id) : 0; 
}

bool Resource::removeId(const QString &id) { 
    return m_project ? m_project->removeResourceId(id) : false; 
}

void Resource::insertId(const QString &id) { 
    //kDebug(planDbg());
    if (m_project)
        m_project->insertResourceId(id, this); 
}

Calendar *Resource::findCalendar(const QString &id) const { 
    return (m_project ? m_project->findCalendar(id) : 0); 
}

bool Resource::isOverbooked() const {
    return isOverbooked( DateTime(), DateTime() );
}

bool Resource::isOverbooked(const QDate &date) const {
    return isOverbooked( DateTime( date ), DateTime( date.addDays(1 ) ) );
}

bool Resource::isOverbooked(const DateTime &start, const DateTime &end) const {
    //kDebug(planDbg())<<m_name<<":"<<start.toString()<<" -"<<end.toString()<<" cs=("<<m_currentSchedule<<")";
    return m_currentSchedule ? m_currentSchedule->isOverbooked(start, end) : false;
}

Appointment Resource::appointmentIntervals( long id ) const {
    Appointment a;
    Schedule *s = findSchedule( id );
    if ( s == 0 ) {
        return a;
    }
    foreach (Appointment *app, static_cast<ResourceSchedule*>( s )->appointments()) {
        a += *app;
    }
    return a;
}

Appointment Resource::appointmentIntervals() const {
    Appointment a;
    if (m_currentSchedule == 0)
        return a;
    foreach (Appointment *app, m_currentSchedule->appointments()) {
        a += *app;
    }
    return a;
}

EffortCostMap Resource::plannedEffortCostPrDay( const QDate &start, const QDate &end, long id, EffortCostCalculationType typ )
{
    EffortCostMap ec;
    Schedule *s = findSchedule( id );
    if ( s == 0 ) {
        return ec;
    }
    ec = s->plannedEffortCostPrDay( start, end, typ );
    return ec;
}

Duration Resource::plannedEffort( const QDate &date, EffortCostCalculationType typ ) const
{
    return m_currentSchedule ? m_currentSchedule->plannedEffort( date, typ ) : Duration::zeroDuration;
}

void Resource::setProject( Project *project )
{
    if ( project != m_project ) {
        if ( m_project  ) {
            removeId();
        }
    }
    m_project = project;
}

void Resource::addExternalAppointment(const QString& id, Appointment* a)
{
    int row = -1;
    if ( m_externalAppointments.contains( id ) ) {
        int row = m_externalAppointments.keys().indexOf( id );
        emit externalAppointmentToBeRemoved( this, row );
        delete m_externalAppointments.take( id );
        emit externalAppointmentRemoved();
    }
    if ( row == -1 ) {
        m_externalAppointments[ id ] = a;
        row = m_externalAppointments.keys().indexOf( id );
        m_externalAppointments.remove( id );
    }
    emit externalAppointmentToBeAdded( this, row );
    m_externalAppointments[ id ] = a;
    emit externalAppointmentAdded( this, a );
}

void Resource::addExternalAppointment( const QString &id, const QString &name, const DateTime &from, const DateTime &end, double load )
{
    Appointment *a = m_externalAppointments.value( id );
    if ( a == 0 ) {
        a = new Appointment();
        a->setAuxcilliaryInfo( name );
        a->addInterval( from, end, load );
        //kDebug(planDbg())<<m_name<<name<<"new appointment:"<<a<<from<<end<<load;
        m_externalAppointments[ id ] = a;
        int row = m_externalAppointments.keys().indexOf( id );
        m_externalAppointments.remove( id );
        emit externalAppointmentToBeAdded( this, row );
        m_externalAppointments[ id ] = a;
        emit externalAppointmentAdded( this, a );
    } else {
        //kDebug(planDbg())<<m_name<<name<<"new interval:"<<a<<from<<end<<load;
        a->addInterval( from, end, load );
        emit externalAppointmentChanged( this, a );
    }
}

void Resource::subtractExternalAppointment( const QString &id, const DateTime &start, const DateTime &end, double load )
{
    Appointment *a = m_externalAppointments.value( id );
    if ( a ) {
        //kDebug(planDbg())<<m_name<<name<<"new interval:"<<a<<from<<end<<load;
        Appointment app;
        app.addInterval( start, end, load );
        *a -= app;
        emit externalAppointmentChanged( this, a );
    }
}

void Resource::clearExternalAppointments()
{
    foreach ( const QString &id, m_externalAppointments.keys() ) {
        clearExternalAppointments( id );
    }
}

void Resource::clearExternalAppointments( const QString projectId )
{
    while ( m_externalAppointments.contains( projectId ) ) {
        int row = m_externalAppointments.keys().indexOf( projectId );
        emit externalAppointmentToBeRemoved( this, row );
        Appointment *a  = m_externalAppointments.take( projectId );
        emit externalAppointmentRemoved();
        delete a;
    }
}

Appointment *Resource::takeExternalAppointment( const QString &id )
{
    Appointment *a = 0;
    if ( m_externalAppointments.contains( id ) ) {
        int row = m_externalAppointments.keys().indexOf( id );
        emit externalAppointmentToBeRemoved( this, row );
        a = m_externalAppointments.take( id );
        emit externalAppointmentRemoved();
    }
    return a;
}

AppointmentIntervalList Resource::externalAppointments( const QString &id )
{
    if ( ! m_externalAppointments.contains( id ) ) {
        return AppointmentIntervalList();
    }
    return m_externalAppointments[ id ]->intervals();
}

AppointmentIntervalList Resource::externalAppointments( const DateTimeInterval &interval ) const
{
    //kDebug(planDbg())<<m_externalAppointments;
    Appointment app;
    foreach ( Appointment *a, m_externalAppointments ) {
        app += interval.isValid() ? a->extractIntervals( interval ) : *a;
    }
    return app.intervals();
}

QMap<QString, QString> Resource::externalProjects() const
{
    QMap<QString, QString> map;
    for ( QMapIterator<QString, Appointment*> it( m_externalAppointments ); it.hasNext(); ) {
        it.next();
        if ( ! map.contains( it.key() ) ) {
            map[ it.key() ] = it.value()->auxcilliaryInfo();
        }
    }
//     kDebug(planDbg())<<map;
    return map;
}

long Resource::allocationSuitability( const DateTime &time, const Duration &duration, bool backward )
{
    // FIXME: This is not *very* intelligent...
    Duration e;
    if ( m_type == Type_Team ) {
        foreach ( Resource *r, teamMembers() ) {
            e += r->effort( time, duration, 100, backward );
        }
    } else {
        e = effort( time, duration, 100, backward );
    }
    return e.minutes();
}

DateTime Resource::startTime( long id ) const
{
    DateTime dt;
    Schedule *s = schedule( id );
    if ( s == 0 ) {
        return dt;
    }
    foreach ( Appointment *a, s->appointments() ) {
        DateTime t = a->startTime();
        if ( ! dt.isValid() || t < dt ) {
            dt = t;
        }
    }
    return dt;
}

DateTime Resource::endTime( long id ) const
{
    DateTime dt;
    Schedule *s = schedule( id );
    if ( s == 0 ) {
        return dt;
    }
    foreach ( Appointment *a, s->appointments() ) {
        DateTime t = a->endTime();
        if ( ! dt.isValid() || t > dt ) {
            dt = t;
        }
    }
    return dt;
}

QList<Resource*> Resource::teamMembers() const
{
    QList<Resource*> lst;
    foreach ( const QString &s, m_teamMembers ) {
        Resource *r = findId( s );
        if ( r ) {
            lst << r;
        }
    }
    return lst;

}

QStringList Resource::teamMemberIds() const
{
    return m_teamMembers;
}

void Resource::addTeamMemberId( const QString &id )
{
    if ( ! id.isEmpty() && ! m_teamMembers.contains( id ) ) {
        m_teamMembers.append( id );
    }
}

void Resource::removeTeamMemberId( const QString &id )
{
    if ( m_teamMembers.contains( id ) ) {
        m_teamMembers.removeAt( m_teamMembers.indexOf( id ) );
    }
}

QDebug operator<<( QDebug dbg, const KPlato::Resource::WorkInfoCache &c )
{
    dbg.nospace()<<"WorkInfoCache: ["<<" version="<<c.version<<" start="<<c.start.toString( Qt::ISODate )<<" end="<<c.end.toString( Qt::ISODate )<<" intervals="<<c.intervals.map().count();
    if ( ! c.intervals.isEmpty() ) {
        foreach ( const AppointmentInterval &i, c.intervals.map() ) {
        dbg<<endl<<"   "<<i;
        }
    }
    dbg<<"]";
    return dbg;
}

/////////   Risk   /////////
Risk::Risk(Node *n, Resource *r, RiskType rt) {
    m_node=n;
    m_resource=r;
    m_riskType=rt;
}

Risk::~Risk() {
}

ResourceRequest::ResourceRequest(Resource *resource, int units)
    : m_resource(resource),
      m_units(units),
      m_parent(0),
      m_dynamic(false)
{
    if ( resource ) {
        m_required = resource->requiredResources();
    }
    //kDebug(planDbg())<<"("<<this<<") Request to:"<<(resource ? resource->name() : QString("None"));
}

ResourceRequest::ResourceRequest(const ResourceRequest &r)
    : m_resource(r.m_resource),
      m_units(r.m_units),
      m_parent(0),
      m_dynamic(r.m_dynamic),
      m_required(r.m_required)
{
}

ResourceRequest::~ResourceRequest() {
    //kDebug(planDbg())<<"("<<this<<") resource:"<<(m_resource ? m_resource->name() : QString("None"));
    if (m_resource)
        m_resource->unregisterRequest(this);
    m_resource = 0;
    qDeleteAll( m_teamMembers );
}

bool ResourceRequest::load(KoXmlElement &element, Project &project) {
    //kDebug(planDbg());
    m_resource = project.resource(element.attribute("resource-id"));
    if (m_resource == 0) {
        kWarning()<<"The referenced resource does not exist: resource id="<<element.attribute("resource-id");
        return false;
    }
    m_units  = element.attribute("units").toInt();

    KoXmlElement parent = element.namedItem( "required-resources" ).toElement();
    KoXmlElement e;
    forEachElement( e, parent ) {
        if (e.nodeName() == "resource") {
            QString id = e.attribute( "id" );
            if ( id.isEmpty() ) {
                kError()<<"Missing project id";
                continue;
            }
            Resource *r = project.resource(id);
            if (r == 0) {
                kWarning()<<"The referenced resource does not exist: resource id="<<element.attribute("resource-id");
            } else {
                if ( r != m_resource ) {
                    m_required << r;
                }
            }
        }
    }
    return true;
}

void ResourceRequest::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("resource-request");
    element.appendChild(me);
    me.setAttribute("resource-id", m_resource->id());
    me.setAttribute("units", m_units);
    if ( ! m_required.isEmpty() ) {
        QDomElement e = me.ownerDocument().createElement("required-resources");
        me.appendChild(e);
        foreach ( Resource *r, m_required ) {
            QDomElement el = e.ownerDocument().createElement("resource");
            e.appendChild( el );
            el.setAttribute( "id", r->id() );
        }
    }
}

int ResourceRequest::units() const {
    //kDebug(planDbg())<<m_resource->name()<<": units="<<m_units;
    return m_units;
}

void ResourceRequest::setUnits( int value )
{
    m_units = value; changed();
}

Task *ResourceRequest::task() const {
    return m_parent ? m_parent->task() : 0;
}

void ResourceRequest::changed()
{
    if ( task() ) {
        task()->changed();
    }
}

void ResourceRequest::setCurrentSchedulePtr( Schedule *ns )
{
    setCurrentSchedulePtr( m_resource, ns );
}

void ResourceRequest::setCurrentSchedulePtr( Resource *resource, Schedule *ns )
{
    resource->setCurrentSchedulePtr( resourceSchedule( ns, resource ) );
    foreach ( Resource *r, m_required ) {
        r->setCurrentSchedulePtr( resourceSchedule( ns, r ) );
    }
}

Schedule *ResourceRequest::resourceSchedule( Schedule *ns, Resource *res )
{
    if ( ns == 0 ) {
        return 0;
    }
    Resource *r = res == 0 ? resource() : res;
    Schedule *s = r->findSchedule(ns->id());
    if (s == 0) {
        s = r->createSchedule(ns->parent());
    }
    s->setCalculationMode( ns->calculationMode() );
    s->setAllowOverbookingState( ns->allowOverbookingState() );
    static_cast<ResourceSchedule*>( s )->setNodeSchedule( ns );
    //kDebug(planDbg())<<s->name()<<": id="<<s->id()<<" mode="<<s->calculationMode();
    return s;
}

DateTime ResourceRequest::workTimeAfter(const DateTime &dt, Schedule *ns) {
    if ( m_resource->type() == Resource::Type_Work ) {
        DateTime t = availableAfter( dt, ns );
        foreach ( Resource *r, m_required ) {
            if ( ! t.isValid() ) {
                break;
            }
            t = r->availableAfter( t, DateTime(), resourceSchedule( ns, r ) );
        }
        return t;
    } else if ( m_resource->type() == Resource::Type_Team ) {
        return availableAfter( dt, ns );
    }
    return DateTime();
}

DateTime ResourceRequest::workTimeBefore(const DateTime &dt, Schedule *ns) {
    if ( m_resource->type() == Resource::Type_Work ) {
        DateTime t = availableBefore( dt, ns );
        foreach ( Resource *r, m_required ) {
            if ( ! t.isValid() ) {
                break;
            }
            t = r->availableBefore( t, DateTime(), resourceSchedule( ns, r ) );
        }
        return t;
    } else if ( m_resource->type() == Resource::Type_Team ) {
        return availableBefore( dt, ns );
    }
    return DateTime();
}

DateTime ResourceRequest::availableFrom()
{
    DateTime dt = m_resource->availableFrom();
    if ( ! dt.isValid() ) {
        dt = m_resource->project()->constraintStartTime();
    }
    return dt;
}

DateTime ResourceRequest::availableUntil()
{
    DateTime dt = m_resource->availableUntil();
    if ( ! dt.isValid() ) {
        dt = m_resource->project()->constraintEndTime();
    }
    return dt;
}

DateTime ResourceRequest::availableAfter(const DateTime &time, Schedule *ns) {
    if ( m_resource->type() == Resource::Type_Team ) {
        DateTime t;// = m_resource->availableFrom();
        foreach ( Resource *r, m_resource->teamMembers() ) {
            setCurrentSchedulePtr( r, ns );
            DateTime x = r->availableAfter( time );
            if ( x.isValid() ) {
                t = t.isValid() ? qMin( t, x ) : x;
            }
        }
        return t;
    }
    setCurrentSchedulePtr( ns );
    return m_resource->availableAfter( time );
}

DateTime ResourceRequest::availableBefore(const DateTime &time, Schedule *ns) {
    if ( m_resource->type() == Resource::Type_Team ) {
        DateTime t;
        foreach ( Resource *r, m_resource->teamMembers() ) {
            setCurrentSchedulePtr( r, ns );
            DateTime x = r->availableBefore( time );
            if ( x.isValid() ) {
                t = t.isValid() ? qMax( t, x ) : x;
            }
        }
        return t;
    }
    setCurrentSchedulePtr( ns );
    return resource()->availableBefore( time );
}

Duration ResourceRequest::effort( const DateTime &time, const Duration &duration, Schedule *ns, bool backward )
{
    setCurrentSchedulePtr( ns );
    Duration e = m_resource->effort( time, duration, m_units, backward, m_required );
    //kDebug(planDbg())<<m_resource->name()<<time<<duration.toString()<<"delivers:"<<e.toString()<<"request:"<<(m_units/100)<<"parts";
    return e;
}

void ResourceRequest::makeAppointment( Schedule *ns )
{
    if ( m_resource ) {
        setCurrentSchedulePtr( ns );
        m_resource->makeAppointment( ns, ( m_resource->units() * m_units / 100 ), m_required );
    }
}

void ResourceRequest::makeAppointment( Schedule *ns, int amount )
{
    if ( m_resource ) {
        setCurrentSchedulePtr( ns );
        m_resource->makeAppointment( ns, amount, m_required );
    }
}

long ResourceRequest::allocationSuitability( const DateTime &time, const Duration &duration, Schedule *ns, bool backward )
{
    setCurrentSchedulePtr( ns );
    return resource()->allocationSuitability( time, duration, backward );
}

QList<ResourceRequest*> ResourceRequest::teamMembers() const
{
    qDeleteAll( m_teamMembers );
    m_teamMembers.clear();
    if ( m_resource->type() == Resource::Type_Team ) {
        foreach ( Resource *r, m_resource->teamMembers() ) {
            m_teamMembers << new ResourceRequest( r, m_units );
        }
    }
    return m_teamMembers;
}

QDebug &operator<<( QDebug &dbg, const KPlato::ResourceRequest *rr )
{
    return operator<<( dbg, *rr );
}
QDebug &operator<<( QDebug &dbg, const KPlato::ResourceRequest &rr )
{
    dbg<<"ResourceRequest["<<rr.resource()->name()<<']';
    return dbg;
}

/////////
ResourceGroupRequest::ResourceGroupRequest(ResourceGroup *group, int units)
    : m_group(group), m_units(units), m_parent(0) {

    //kDebug(planDbg())<<"Request to:"<<(group ? group->name() : QString("None"));
    if (group)
        group->registerRequest(this);
}

ResourceGroupRequest::ResourceGroupRequest(const ResourceGroupRequest &g)
    : m_group(g.m_group), m_units(g.m_units), m_parent(0)
{
}

ResourceGroupRequest::~ResourceGroupRequest() {
    //kDebug(planDbg());
    if (m_group)
        m_group->unregisterRequest(this);

    while (!m_resourceRequests.isEmpty()) {
        delete m_resourceRequests.takeFirst();
    }
}

void ResourceGroupRequest::addResourceRequest(ResourceRequest *request) {
    //kDebug(planDbg())<<"("<<request<<") to Group:"<<(void*)m_group;
    request->setParent(this);
    m_resourceRequests.append(request);
    request->registerRequest();
    changed();
}

ResourceRequest *ResourceGroupRequest::takeResourceRequest(ResourceRequest *request) {
    if (request)
        request->unregisterRequest();
    ResourceRequest *r = 0;
    int i = m_resourceRequests.indexOf(request);
    if (i != -1) {
        r = m_resourceRequests.takeAt(i);
    }
    changed();
    return r;
}

ResourceRequest *ResourceGroupRequest::find(const Resource *resource) const {
    foreach (ResourceRequest *gr, m_resourceRequests) {
        if (gr->resource() == resource) {
            return gr;
        }
    }
    return 0;
}

ResourceRequest *ResourceGroupRequest::resourceRequest( const QString &name ) {
    foreach (ResourceRequest *r, m_resourceRequests) {
        if (r->resource()->name() == name )
            return r;
    }
    return 0;
}

QStringList ResourceGroupRequest::requestNameList( bool includeGroup ) const {
    QStringList lst;
    if ( includeGroup && m_units > 0 && m_group ) {
        lst << m_group->name();
    }
    foreach ( ResourceRequest *r, m_resourceRequests ) {
        if ( ! r->isDynamicallyAllocated() ) {
            Q_ASSERT( r->resource() );
            lst << r->resource()->name();
        }
    }
    return lst;
}

QList<Resource*> ResourceGroupRequest::requestedResources() const
{
    QList<Resource*> lst;
    foreach ( ResourceRequest *r, m_resourceRequests ) {
        if ( ! r->isDynamicallyAllocated() ) {
            Q_ASSERT( r->resource() );
            lst << r->resource();
        }
    }
    return lst;
}

QList<ResourceRequest*> ResourceGroupRequest::resourceRequests( bool resolveTeam ) const
{
    QList<ResourceRequest*> lst;
    foreach ( ResourceRequest *rr, m_resourceRequests ) {
        if ( resolveTeam && rr->resource()->type() == Resource::Type_Team ) {
            lst += rr->teamMembers();
        } else {
            lst << rr;
        }
    }
    return lst;
}

bool ResourceGroupRequest::load(KoXmlElement &element, XMLLoaderObject &status) {
    //kDebug(planDbg());
    m_group = status.project().findResourceGroup(element.attribute("group-id"));
    if (m_group == 0) {
        kError()<<"The referenced resource group does not exist: group id="<<element.attribute("group-id");
        return false;
    }
    m_group->registerRequest(this);

    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if (e.tagName() == "resource-request") {
            ResourceRequest *r = new ResourceRequest();
            if (r->load(e, status.project()))
                addResourceRequest(r);
            else {
                kError()<<"Failed to load resource request";
                delete r;
            }
        }
    }
    // meaning of m_units changed
    // Pre 0.6.6 the number *included* all requests, now it is in *addition* to resource requests
    m_units  = element.attribute("units").toInt();
    if ( status.version() < "0.6.6" ) {
        int x = m_units - m_resourceRequests.count();
        m_units = x > 0 ? x : 0;
    }
    return true;
}

void ResourceGroupRequest::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("resourcegroup-request");
    element.appendChild(me);
    me.setAttribute("group-id", m_group->id());
    me.setAttribute("units", m_units);
    foreach (ResourceRequest *r, m_resourceRequests)
        r->save(me);
}

int ResourceGroupRequest::units() const {
    return m_units;
}

Duration ResourceGroupRequest::duration(const DateTime &time, const Duration &_effort, Schedule *ns, bool backward) {
    Duration dur;
    if ( m_parent ) {
        dur = m_parent->duration( m_resourceRequests, time, _effort, ns, backward );
    }
    return dur;
}

DateTime ResourceGroupRequest::workTimeAfter(const DateTime &time, Schedule *ns) {
    DateTime start;
    if ( m_resourceRequests.isEmpty() ) {
        return start;
    }
    foreach (ResourceRequest *r, m_resourceRequests) {
        DateTime t = r->workTimeAfter( time, ns );
        if (t.isValid() && (!start.isValid() || t < start))
            start = t;
    }
    if (start.isValid() && start < time)
        start = time;
    //kDebug(planDbg())<<time.toString()<<"="<<start.toString();
    return start;
}

DateTime ResourceGroupRequest::workTimeBefore(const DateTime &time, Schedule *ns) {
    DateTime end;
    if ( m_resourceRequests.isEmpty() ) {
        return end;
    }
    foreach (ResourceRequest *r, m_resourceRequests) {
        DateTime t = r->workTimeBefore( time, ns );
        if (t.isValid() && (!end.isValid() ||t > end))
            end = t;
    }
    if (!end.isValid() || end > time)
        end = time;
    return end;
}

DateTime ResourceGroupRequest::availableAfter(const DateTime &time, Schedule *ns) {
    DateTime start;
    if ( m_resourceRequests.isEmpty() ) {
        return start;
    }
    foreach (ResourceRequest *r, m_resourceRequests) {
        DateTime t = r->availableAfter(time, ns);
        if (t.isValid() && (!start.isValid() || t < start))
            start = t;
    }
    if (start.isValid() && start < time)
        start = time;
    //kDebug(planDbg())<<time.toString()<<"="<<start.toString()<<""<<m_group->name();
    return start;
}

DateTime ResourceGroupRequest::availableBefore(const DateTime &time, Schedule *ns) {
    DateTime end;
    if ( m_resourceRequests.isEmpty() ) {
        return end;
    }
    foreach (ResourceRequest *r, m_resourceRequests) {
        DateTime t = r->availableBefore(time, ns);
        if (t.isValid() && (!end.isValid() || t > end))
            end = t;
    }
    if (!end.isValid() || end > time)
        end = time;
    //kDebug(planDbg())<<time.toString()<<"="<<end.toString()<<""<<m_group->name();
    return end;
}

void ResourceGroupRequest::makeAppointments(Schedule *schedule) {
    //kDebug(planDbg());
    foreach (ResourceRequest *r, m_resourceRequests) {
        r->makeAppointment(schedule);
    }
}

void ResourceGroupRequest::reserve(const DateTime &start, const Duration &duration) {
    m_start = start;
    m_duration = duration;
}

bool ResourceGroupRequest::isEmpty() const {
    return m_resourceRequests.isEmpty() && m_units == 0;
}

Task *ResourceGroupRequest::task() const {
    return m_parent ? m_parent->task() : 0;
}

void ResourceGroupRequest::changed()
{
     if ( m_parent ) 
         m_parent->changed();
}

void ResourceGroupRequest::deleteResourceRequest( ResourceRequest *request )
{
    int i = m_resourceRequests.indexOf( request );
    if ( i != -1 ) {
        m_resourceRequests.removeAt( i );
    }
    delete request;
    changed();
}

void ResourceGroupRequest::resetDynamicAllocations()
{
    QList<ResourceRequest*> lst;
    foreach ( ResourceRequest *r, m_resourceRequests ) {
        if ( r->isDynamicallyAllocated() ) {
            lst << r;
        }
    }
    while ( ! lst.isEmpty() ) {
        deleteResourceRequest( lst.takeFirst() );
    }
}

void ResourceGroupRequest::allocateDynamicRequests( const DateTime &time, const Duration &effort, Schedule *ns, bool backward )
{
    int num = m_units;
    if ( num <= 0 ) {
        return;
    }
    if ( num == m_group->numResources() ) {
        // TODO: allocate all
    }
    Duration e = effort / m_units;
    QMap<long, ResourceRequest*> map;
    foreach ( Resource *r, m_group->resources() ) {
        if ( r->type() == Resource::Type_Team ) {
            continue;
        }
        ResourceRequest *rr = find( r );
        if ( rr ) {
            if ( rr->isDynamicallyAllocated() ) {
                --num; // already allocated
            }
            continue;
        }
        rr = new ResourceRequest( r, r->units() );
        long s = rr->allocationSuitability( time, e, ns, backward );
        if ( s == 0 ) {
            // not suitable at all
            delete rr;
        } else {
            map.insertMulti( s, rr );
        }
    }
    for ( --num; num >= 0 && ! map.isEmpty(); --num ) {
        long key = map.keys().last();
        ResourceRequest *r = map.take( key );
        r->setAllocatedDynaically( true );
        addResourceRequest( r );
        kDebug(planDbg())<<key<<r;
    }
    qDeleteAll( map ); // delete the unused
}

/////////
ResourceRequestCollection::ResourceRequestCollection(Task *task)
    : m_task(task) {
    //kDebug(planDbg())<<this<<(void*)(&task);
}

ResourceRequestCollection::~ResourceRequestCollection() {
    //kDebug(planDbg())<<this;
    while (!m_requests.empty()) {
        delete m_requests.takeFirst();
    }
}

void ResourceRequestCollection::addRequest( ResourceGroupRequest *request )
{
    foreach ( ResourceGroupRequest *r, m_requests ) {
        if ( r->group() == request->group() ) {
            kError()<<"Request to this group already exists";
            kError()<<"Task:"<<m_task->name()<<"Group:"<<request->group()->name();
            Q_ASSERT( false );
        }
    }
    m_requests.append( request );
    request->setParent( this );
    changed();
}

ResourceGroupRequest *ResourceRequestCollection::find(const ResourceGroup *group) const {
    foreach (ResourceGroupRequest *r, m_requests) {
        if (r->group() == group)
            return r; // we assume only one request to the same group
    }
    return 0;
}

ResourceRequest *ResourceRequestCollection::find(const Resource *resource) const {
    ResourceRequest *req = 0;
    QListIterator<ResourceGroupRequest*> it(m_requests);
    while (req == 0 && it.hasNext()) {
        req = it.next()->find(resource);
    }
    return req;
}

ResourceRequest *ResourceRequestCollection::resourceRequest( const QString &name ) const {
    ResourceRequest *req = 0;
    QListIterator<ResourceGroupRequest*> it(m_requests);
    while (req == 0 && it.hasNext()) {
        req = it.next()->resourceRequest( name );
    }
    return req;
}

QStringList ResourceRequestCollection::requestNameList( bool includeGroup ) const {
    QStringList lst;
    foreach ( ResourceGroupRequest *r, m_requests ) {
        lst << r->requestNameList( includeGroup );
    }
    return lst;
}

QList<Resource*> ResourceRequestCollection::requestedResources() const {
    QList<Resource*> lst;
    foreach ( ResourceGroupRequest *g, m_requests ) {
        lst += g->requestedResources();
    }
    return lst;
}

QList<ResourceRequest*> ResourceRequestCollection::resourceRequests( bool resolveTeam ) const {
    QList<ResourceRequest*> lst;
    foreach ( ResourceGroupRequest *g, m_requests ) {
        foreach ( ResourceRequest *r, g->resourceRequests( resolveTeam ) ) {
            lst << r;
        }
    }
    return lst;
}


bool ResourceRequestCollection::contains( const QString &identity ) const {
    QStringList lst = requestNameList();
    return lst.indexOf( QRegExp( identity, Qt::CaseSensitive, QRegExp::FixedString ) ) != -1;
}

ResourceGroupRequest *ResourceRequestCollection::findGroupRequestById( const QString &id ) const
{
    foreach ( ResourceGroupRequest *r, m_requests ) {
        if ( r->group()->id() == id ) {
            return r;
        }
    }
    return 0;
}

// bool ResourceRequestCollection::load(KoXmlElement &element, Project &project) {
//     //kDebug(planDbg());
//     return true;
// }

void ResourceRequestCollection::save(QDomElement &element) const {
    //kDebug(planDbg());
    foreach (ResourceGroupRequest *r, m_requests) {
        r->save(element);
    }
}

// Returns the duration needed by the working resources
// "Material type" of resourcegroups does not (atm) affect the duration.
Duration ResourceRequestCollection::duration(const DateTime &time, const Duration &effort, Schedule *ns, bool backward) {
    //kDebug(planDbg())<<"time="<<time.toString()<<" effort="<<effort.toString(Duration::Format_Day)<<" backward="<<backward;
    if (isEmpty()) {
        return effort;
    }
    Duration dur = effort;
    QList<ResourceRequest*> lst;
    foreach (ResourceGroupRequest *r, m_requests) {
        r->allocateDynamicRequests( time, effort, ns, backward );
        if ( r->group()->type() == ResourceGroup::Type_Work ) {
            lst << r->resourceRequests();
        } else if (r->group()->type() == ResourceGroup::Type_Material) {
            //TODO
        }
    }
    if ( ! lst.isEmpty() ) {
        dur = duration( lst, time, effort, ns, backward );
    }
    return dur;
}

DateTime ResourceRequestCollection::workTimeAfter(const DateTime &time, Schedule *ns) const {
    DateTime start;
    foreach (ResourceGroupRequest *r, m_requests) {
        DateTime t = r->workTimeAfter( time, ns );
        if (t.isValid() && (!start.isValid() || t < start))
            start = t;
    }
    if (start.isValid() && start < time)
        start = time;
    //kDebug(planDbg())<<time.toString()<<"="<<start.toString();
    return start;
}

DateTime ResourceRequestCollection::workTimeBefore(const DateTime &time, Schedule *ns) const {
    DateTime end;
    foreach (ResourceGroupRequest *r, m_requests) {
        DateTime t = r->workTimeBefore( time, ns );
        if (t.isValid() && (!end.isValid() ||t > end))
            end = t;
    }
    if (!end.isValid() || end > time)
        end = time;
    return end;
}

DateTime ResourceRequestCollection::availableAfter(const DateTime &time, Schedule *ns) {
    DateTime start;
    foreach (ResourceGroupRequest *r, m_requests) {
        DateTime t = r->availableAfter(time, ns);
        if (t.isValid() && (!start.isValid() || t < start))
            start = t;
    }
    if (start.isValid() && start < time)
        start = time;
    //kDebug(planDbg())<<time.toString()<<"="<<start.toString();
    return start;
}

DateTime ResourceRequestCollection::availableBefore(const DateTime &time, Schedule *ns) {
    DateTime end;
    foreach (ResourceGroupRequest *r, m_requests) {
        DateTime t = r->availableBefore(time, ns);
        if (t.isValid() && (!end.isValid() ||t > end))
            end = t;
    }
    if (!end.isValid() || end > time)
        end = time;
    return end;
}

DateTime ResourceRequestCollection::workStartAfter(const DateTime &time, Schedule *ns) {
    DateTime start;
    foreach (ResourceGroupRequest *r, m_requests) {
        if ( r->group()->type() != ResourceGroup::Type_Work ) {
            continue;
        }
        DateTime t = r->availableAfter(time, ns);
        if (t.isValid() && (!start.isValid() || t < start))
            start = t;
    }
    if (start.isValid() && start < time)
        start = time;
    //kDebug(planDbg())<<time.toString()<<"="<<start.toString();
    return start;
}

DateTime ResourceRequestCollection::workFinishBefore(const DateTime &time, Schedule *ns) {
    DateTime end;
    foreach (ResourceGroupRequest *r, m_requests) {
        if ( r->group()->type() != ResourceGroup::Type_Work ) {
            continue;
        }
        DateTime t = r->availableBefore(time, ns);
        if (t.isValid() && (!end.isValid() ||t > end))
            end = t;
    }
    if (!end.isValid() || end > time)
        end = time;
    return end;
}


void ResourceRequestCollection::makeAppointments(Schedule *schedule) {
    //kDebug(planDbg());
    foreach (ResourceGroupRequest *r, m_requests) {
        r->makeAppointments(schedule);
    }
}

void ResourceRequestCollection::reserve(const DateTime &start, const Duration &duration) {
    //kDebug(planDbg());
    foreach (ResourceGroupRequest *r, m_requests) {
        r->reserve(start, duration);
    }
}

bool ResourceRequestCollection::isEmpty() const {
    foreach (ResourceGroupRequest *r, m_requests) {
        if (!r->isEmpty())
            return false;
    }
    return true;
}

void ResourceRequestCollection::changed()
{
    //kDebug(planDbg())<<m_task;
    if ( m_task ) {
        m_task->changed();
    }
}

void ResourceRequestCollection::resetDynamicAllocations()
{
    foreach ( ResourceGroupRequest *g, m_requests ) {
        g->resetDynamicAllocations();
    }
}

Duration ResourceRequestCollection::effort( const QList<ResourceRequest*> &lst, const DateTime &time, const Duration &duration, Schedule *ns, bool backward ) const {
    Duration e;
    foreach (ResourceRequest *r, lst) {
        e += r->effort( time, duration, ns, backward );
        //kDebug(planDbg())<<(backward?"(B)":"(F)" )<<r<<": time="<<time<<" dur="<<duration.toString()<<"gave e="<<e.toString();
    }
    //kDebug(planDbg())<<time.toString()<<"d="<<duration.toString()<<": e="<<e.toString();
    return e;
}

int ResourceRequestCollection::numDays(const QList<ResourceRequest*> &lst, const DateTime &time, bool backward) const {
    DateTime t1, t2 = time;
    if (backward) {
        foreach (ResourceRequest *r, lst) {
            t1 = r->availableFrom();
            if (!t2.isValid() || t2 > t1)
                t2 = t1;
        }
        //kDebug(planDbg())<<"bw"<<time.toString()<<":"<<t2.daysTo(time);
        return t2.daysTo(time);
    }
    foreach (ResourceRequest *r, lst) {
        t1 = r->availableUntil();
        if (!t2.isValid() || t2 < t1)
            t2 = t1;
    }
    //kDebug(planDbg())<<"fw"<<time.toString()<<":"<<time.daysTo(t2);
    return time.daysTo(t2);
}

Duration ResourceRequestCollection::duration(const QList<ResourceRequest*> &lst, const DateTime &time, const Duration &_effort, Schedule *ns, bool backward) {
    //kDebug(planDbg())<<"--->"<<(backward?"(B)":"(F)")<<time.toString()<<": effort:"<<_effort.toString(Duration::Format_Day)<<" ("<<_effort.milliseconds()<<")";
#if 0
    if ( ns ) {
        QStringList nl;
        foreach ( ResourceRequest *r, lst ) { nl << r->resource()->name(); }
        ns->logDebug( "Match effort:" + time.toString() + "," + _effort.toString() );
        ns->logDebug( "Resources: " + ( nl.isEmpty() ? QString( "None" ) : nl.join( ", " ) ) );
    }
#endif
    KLocale *locale = KGlobal::locale();
    Duration e;
    if (_effort == Duration::zeroDuration) {
        return e;
    }
    DateTime logtime = time;
    bool match = false;
    DateTime start = time;
    int inc = backward ? -1 : 1;
    DateTime end = start;
    Duration e1;
    int nDays = numDays(lst, time, backward) + 1;
    int day = 0;
    for (day=0; !match && day <= nDays; ++day) {
        // days
        end = end.addDays(inc);
        e1 = effort( lst, start, backward ? start - end : end - start, ns, backward );
        //kDebug(planDbg())<<"["<<i<<"of"<<nDays<<"]"<<(backward?"(B)":"(F):")<<"  start="<<start<<" e+e1="<<(e+e1).toString()<<" match"<<_effort.toString();
        if (e + e1 < _effort) {
            e += e1;
            start = end;
        } else if (e + e1 == _effort) {
            e += e1;
            match = true;
        } else {
            end = start;
            break;
        }
    }
    if ( ! match && day <= nDays ) {
#ifndef PLAN_NLOGDEBUG
        if ( ns ) ns->logDebug( "Days: duration " + logtime.toString() + " - " + end.toString() + " e=" + e.toString() + " (" + (_effort - e).toString() + ')' );
#endif
        logtime = start;
        for (int i=0; !match && i < 24; ++i) {
            // hours
            end = end.addSecs(inc*60*60);
            e1 = effort( lst, start, backward ? start - end : end - start, ns, backward );
            if (e + e1 < _effort) {
                e += e1;
                start = end;
            } else if (e + e1 == _effort) {
                e += e1;
                match = true;
            } else {
                if ( false/*roundToHour*/ && ( _effort - e ) <  ( e + e1 - _effort ) ) {
                    end = start;
                    match = true;
                } else {
                    end = start;
                }
                break;
            }
            //kDebug(planDbg())<<"duration(h)["<<i<<"]"<<(backward?"backward":"forward:")<<" time="<<start.time()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")";
        }
        //kDebug(planDbg())<<"duration"<<(backward?"backward":"forward:")<<start.toString()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")  match="<<match<<" sts="<<sts;
    }
    if ( ! match && day <= nDays ) {
#ifndef PLAN_NLOGDEBUG
        if ( ns ) ns->logDebug( "Hours: duration " + logtime.toString() + " - " + end.toString() + " e=" + e.toString() + " (" + (_effort - e).toString() + ')' );
#endif
        logtime = start;
        for (int i=0; !match && i < 60; ++i) {
            //minutes
            end = end.addSecs(inc*60);
            e1 = effort( lst, start, backward ? start - end : end - start, ns, backward );
            if (e + e1 < _effort) {
                e += e1;
                start = end;
            } else if (e + e1 == _effort) {
                e += e1;
                match = true;
            } else if (e + e1 > _effort) {
                end = start;
                break;
            }
            //kDebug(planDbg())<<"duration(m)"<<(backward?"backward":"forward:")<<"  time="<<start.time().toString()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")";
        }
        //kDebug(planDbg())<<"duration"<<(backward?"backward":"forward:")<<"  start="<<start.toString()<<" e="<<e.toString()<<" match="<<match<<" sts="<<sts;
    }
    // FIXME: better solution
    // If effort to match is reasonably large, accept a match if deviation <= 1 min
    if ( ! match && _effort > 5 * 60000 ) {
        if ( ( _effort - e ) <= 60000 ){
            match = true;
#ifndef PLAN_NLOGDEBUG
            if ( ns ) ns->logDebug( "Deviation match:" + logtime.toString() + " - " + end.toString() + " e=" + e.toString() + " (" + (_effort - e).toString() + ')' );
#endif
        }
    }
    if ( ! match && day <= nDays ) {
#ifndef PLAN_NLOGDEBUG
        if ( ns ) ns->logDebug( "Minutes: duration " + logtime.toString() + " - " + end.toString() + " e=" + e.toString() + " (" + (_effort - e).toString() + ')' );
#endif
        logtime = start;
        for (int i=0; !match && i < 60; ++i) {
            //seconds
            end = end.addSecs(inc);
            e1 = effort( lst, start, backward ? start - end : end - start, ns, backward );
            if (e + e1 < _effort) {
                e += e1;
                start = end;
            } else if (e + e1 == _effort) {
                e += e1;
                match = true;
            } else if (e + e1 > _effort) {
                end = start;
                break;
            }
            //kDebug(planDbg())<<"duration(s)["<<i<<"]"<<(backward?"backward":"forward:")<<" time="<<start.time().toString()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")";
        }
    }
    if ( ! match && day <= nDays ) {
#ifndef PLAN_NLOGDEBUG
        if ( ns ) ns->logDebug( "Seconds: duration " + logtime.toString() + " - " + end.toString() + " e=" + e.toString() + " (" + (_effort - e).toString() + ')' );
#endif
        for (int i=0; !match && i < 1000; ++i) {
            //milliseconds
            end.setTime(end.time().addMSecs(inc));
            e1 = effort( lst, start, backward ? start - end : end - start, ns, backward );
            if (e + e1 < _effort) {
                e += e1;
                start = end;
            } else if (e + e1 == _effort) {
                e += e1;
                match = true;
            } else if (e + e1 > _effort) {
                break;
            }
            //kDebug(planDbg())<<"duration(ms)["<<i<<"]"<<(backward?"backward":"forward:")<<" time="<<start.time().toString()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")";
        }
    }
    if (!match && ns) {
        ns->logError( i18n( "Could not match effort. Want: %1 got: %2", _effort.toString( Duration::Format_Hour ), e.toString( Duration::Format_Hour ) ) );
        foreach (ResourceRequest *r, lst) {
            Resource *res = r->resource();
            ns->logInfo( i18n( "Resource %1 available from %2 to %3", res->name(), locale->formatDateTime( r->availableFrom() ), locale->formatDateTime( r->availableUntil() ) ) );
        }

    }
    DateTime t;
    if (e != Duration::zeroDuration) {
        foreach ( ResourceRequest *r, lst ) {
            DateTime tt;
            if ( backward ) {
                tt = r->availableAfter(end, ns);
                if ( tt.isValid() && ( ! t.isValid() || tt < t ) ) {
                    t = tt;
                }
            } else {
                tt = r->availableBefore(end, ns);
                if ( tt.isValid() && ( ! t.isValid() || tt > t ) ) {
                    t = tt;
                }
            }
        }
    }
    end = t.isValid() ? t : time;
    //kDebug(planDbg())<<"<---"<<(backward?"(B)":"(F)")<<":"<<end.toString()<<"-"<<time.toString()<<"="<<(end - time).toString()<<" effort:"<<_effort.toString(Duration::Format_Day);
    return (end>time?end-time:time-end);
}


}  //KPlato namespace

#include "kptresource.moc"
