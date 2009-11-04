/* This file is part of the KDE project
 Copyright (C) 2001 Thomas zander <zander@kde.org>
 Copyright (C) 2004 - 2007 Dag Andersen <danders@get2net.dk>
 Copyright (C) 2007 Florian Piquemal <flotueur@yahoo.fr>
 Copyright (C) 2007 Alexis Ménard <darktears31@gmail.com>

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

#include "kptproject.h"
#include "kptappointment.h"
#include "kpttask.h"
#include "kptdatetime.h"
#include "kpteffortcostmap.h"
#include "kptschedule.h"
#include "kptwbsdefinition.h"
#include "kptxmlloaderobject.h"
#include "kptschedulerplugin.h"

#include <KoXmlReader.h>

#include <qdom.h>
#include <QString>
#include <qdatetime.h>
#include <qbrush.h>
#include <QList>

#include <kdatetime.h>
#include <kdebug.h>
#include <ksystemtimezone.h>
#include <ktimezone.h>
#include <krandom.h>

namespace KPlato
{

Project::Project( Node *parent )
        : Node( parent ),
        m_accounts( *this ),
        m_defaultCalendar( 0 ),
        emptyConfig( new ConfigBase() ),
        m_config( emptyConfig ),
        m_schedulerPlugins()
{
    //kDebug()<<"("<<this<<")";
    init();
}

Project::Project( ConfigBase &config, Node *parent )
        : Node( parent ),
        m_accounts( *this ),
        m_defaultCalendar( 0 ),
        emptyConfig( 0 ),
        m_config( &config ),
        m_schedulerPlugins()
{
    //kDebug()<<"("<<this<<")";
    init();
}

void Project::init()
{
    m_constraint = Node::MustStartOn;
    m_standardWorktime = new StandardWorktime();
    m_spec = KDateTime::Spec::LocalZone();
    if ( !m_spec.timeZone().isValid() ) {
        m_spec.setType( KTimeZone() );
    }
    //kDebug()<<m_spec.timeZone();
    if ( m_parent == 0 ) {
        // set sensible defaults for a project wo parent
        m_constraintStartTime = DateTime( QDate::currentDate(), QTime(), m_spec );
        m_constraintEndTime = m_constraintStartTime.addYears( 2 );
    }
}


Project::~Project()
{
    disconnect(); // NOTE: may be a problem if somebody uses the destroyd() signal
    delete m_standardWorktime;
    while ( !m_resourceGroups.isEmpty() )
        delete m_resourceGroups.takeFirst();
    while ( !m_calendars.isEmpty() )
        delete m_calendars.takeFirst();
    while ( !m_managers.isEmpty() )
        delete m_managers.takeFirst();
    
    m_config = 0; //not mine, don't delete
    delete emptyConfig;
}

int Project::type() const { return Node::Type_Project; }

void Project::generateUniqueNodeIds()
{
    foreach ( Node *n, nodeIdDict ) {
        QString uid = uniqueNodeId();
        nodeIdDict.remove( n->id() );
        n->setId( uid );
        nodeIdDict[ uid ] = n;
    }
}

void Project::generateUniqueIds()
{
    generateUniqueNodeIds();

    foreach ( ResourceGroup *g, resourceGroupIdDict ) {
        resourceGroupIdDict.remove( g->id() );
        g->setId( uniqueResourceGroupId() );
        resourceGroupIdDict[ g->id() ] = g;
    }
    foreach ( Resource *r, resourceIdDict ) {
        resourceIdDict.remove( r->id() );
        r->setId( uniqueResourceId() );
        resourceIdDict[ r->id() ] = r;
    }
    foreach ( Calendar *c, calendarIdDict ) {
        calendarIdDict.remove( c->id() );
        c->setId( uniqueCalendarId() );
        calendarIdDict[ c->id() ] = c;
    }
}

void Project::calculate( Schedule *schedule, const DateTime &dt )
{
    if ( schedule == 0 ) {
        kError() << "Schedule == 0, cannot calculate";
        return ;
    }
    m_currentSchedule = schedule;
    calculate( dt );
}

void Project::calculate( const DateTime &dt )
{
    if ( m_currentSchedule == 0 ) {
        kError() << "No current schedule to calculate";
        return ;
    }
    KLocale *locale = KGlobal::locale();
    DateTime time = dt.isValid() ? dt : DateTime( KDateTime::currentLocalDateTime() );
    MainSchedule *cs = static_cast<MainSchedule*>( m_currentSchedule );
    Estimate::Use estType = ( Estimate::Use ) cs->type();
    if ( type() == Type_Project ) {
        cs->setPhaseName( 0, i18n( "Init" ) );
        cs->logInfo( i18n( "Schedule project from: %1", locale->formatDateTime( dt ) ), 0 );
        initiateCalculation( *cs );
        initiateCalculationLists( *cs ); // must be after initiateCalculation() !!
        propagateEarliestStart( time );
        // Calculate lateFinish from time. If a task has started, remaingEffort is used.
        cs->setPhaseName( 1, i18nc( "Schedule project forward", "Forward" ) );
        cs->logInfo( i18n( "Calculate finish" ), 1 );
        cs->lateFinish = calculateForward( estType );
        cs->lateFinish = checkEndConstraints( cs->lateFinish );
        propagateLatestFinish( cs->lateFinish );
        // Calculate earlyFinish. If a task has started, remaingEffort is used.
        cs->setPhaseName( 2, i18nc( "Schedule project backward","Backward" ) );
        cs->logInfo( i18n( "Calculate start" ), 2 );
        cs->calculateBackward( estType );
        // Schedule. If a task has started, remaingEffort is used and appointments are copied from parent
        cs->setPhaseName( 3, i18n( "Schedule" ) );
        cs->logInfo( i18n( "Schedule tasks forward" ), 3 );
        cs->endTime = scheduleForward( cs->startTime, estType );
        cs->logInfo( i18n( "Scheduled finish: %1", locale->formatDateTime( cs->endTime ) ), 3 );
        if ( cs->endTime > m_constraintEndTime ) {
            cs->logError( i18n( "Could not finish project in time: %1", locale->formatDateTime( m_constraintEndTime ) ), 3 );
        } else if ( cs->endTime == m_constraintEndTime ) {
            cs->logWarning( i18n( "Finished project exactly on time: %1", locale->formatDateTime( m_constraintEndTime ) ), 3 );
        } else {
            cs->logInfo( i18n( "Finished project before time: %1", locale->formatDateTime( m_constraintEndTime ) ), 3 );
        }
        calcCriticalPath( false );
        calcResourceOverbooked();
        cs->notScheduled = false;
        calcFreeFloat();
        emit scheduleChanged( cs );
        emit changed();
    } else if ( type() == Type_Subproject ) {
        kWarning() << "Subprojects not implemented";
    } else {
        kError() << "Illegal project type: " << type();
    }
}

void Project::calculate( ScheduleManager &sm )
{
    m_progress = 0;
    int nodes = 0;
    foreach ( Node *n, nodeIdDict ) {
        if ( n->type() == Node::Type_Task || n->type() == Node::Type_Milestone ) {
            nodes++;
        }
    }
    int maxprogress = nodes * 3;
    //kDebug();
    if ( sm.recalculate() ) {
        emit maxProgress( maxprogress );
        incProgress();
        sm.setCalculateAll( false );
        sm.createSchedules();
        if ( sm.parentManager() ) {
            sm.expected()->startTime = sm.parentManager()->expected()->startTime;
            sm.expected()->earlyStart = sm.parentManager()->expected()->earlyStart;
        }
        incProgress();
        calculate( sm.expected(), sm.recalculateFrom() );
    } else {
        if ( sm.optimistic() ) {
            maxprogress += nodes * 3;
        }
        if ( sm.pessimistic() ) {
            maxprogress += nodes * 3;
        }
        emit maxProgress( maxprogress );
        sm.createSchedules();
        calculate( sm.expected() );
        emit scheduleChanged( sm.expected() );
        if ( sm.optimistic() ) {
            calculate( sm.optimistic() );
        }
        if ( sm.pessimistic() ) {
            calculate( sm.pessimistic() );
        }
        setCurrentSchedule( sm.expected()->id() );
    }
    emit sigProgress( maxprogress );
    emit scheduleManagerChanged( &sm );
    emit projectCalculated( &sm );
    emit changed();
}

void Project::calculate( Schedule *schedule )
{
    if ( schedule == 0 ) {
        kError() << "Schedule == 0, cannot calculate";
        return ;
    }
    m_currentSchedule = schedule;
    calculate();
}

void Project::calculate()
{
    if ( m_currentSchedule == 0 ) {
        kError() << "No current schedule to calculate";
        return ;
    }
    MainSchedule *cs = static_cast<MainSchedule*>( m_currentSchedule );
    bool backwards = false;
    if ( cs->manager() ) {
        backwards = cs->manager()->schedulingDirection();
    }
    KLocale *locale = KGlobal::locale();
    Estimate::Use estType = ( Estimate::Use ) cs->type();
    if ( type() == Type_Project ) {
        initiateCalculation( *cs );
        initiateCalculationLists( *cs ); // must be after initiateCalculation() !!
        if ( ! backwards ) {
            cs->setPhaseName( 0, i18n( "Init" ) );
            cs->logInfo( i18n( "Schedule project forward from: %1", locale->formatDateTime( m_constraintStartTime ) ), 0 );
            cs->startTime = m_constraintStartTime;
            cs->earlyStart = m_constraintStartTime;
            // Calculate from start time
            propagateEarliestStart( cs->earlyStart );
            cs->setPhaseName( 1, i18nc( "Schedule project forward", "Forward" ) );
            cs->logInfo( i18n( "Calculate late finish" ), 1 );
            cs->lateFinish = calculateForward( estType );
            cs->lateFinish = checkEndConstraints( cs->lateFinish );
            propagateLatestFinish( cs->lateFinish );
            cs->setPhaseName( 2, i18nc( "Schedule project backward", "Backward" ) );
            cs->logInfo( i18n( "Calculate early start" ), 2 );
            cs->calculateBackward( estType );
            cs->setPhaseName( 3, i18n( "Schedule" ) );
            cs->logInfo( i18n( "Schedule tasks forward" ), 3 );
            cs->endTime = scheduleForward( cs->startTime, estType );
            cs->duration = cs->endTime - cs->startTime;
            cs->logInfo( i18n( "Scheduled finish: %1", locale->formatDateTime( cs->endTime ) ), 3 );
            if ( cs->endTime > m_constraintEndTime ) {
                cs->schedulingError = true;
                cs->logError( i18n( "Could not finish project in time: %1", locale->formatDateTime( m_constraintEndTime ) ), 3 );
            } else if ( cs->endTime == m_constraintEndTime ) {
                cs->logWarning( i18n( "Finished project exactly on time: %1", locale->formatDateTime( m_constraintEndTime ) ), 3 );
            } else {
                cs->logInfo( i18n( "Finished project before time: %1", locale->formatDateTime( m_constraintEndTime ) ), 3 );
            }
            calcCriticalPath( false );
        } else {
            cs->setPhaseName( 0, i18n( "Init" ) );
            cs->logInfo( i18n( "Schedule project backward from: %1", locale->formatDateTime( m_constraintEndTime ) ), 0 );
            // Calculate from end time
            propagateLatestFinish( m_constraintEndTime );
            cs->setPhaseName( 1, i18nc( "Schedule project backward", "Backward" ) );
            cs->logInfo( i18n( "Calculate early start" ), 1 );
            cs->earlyStart = calculateBackward( estType );
            cs->earlyStart = checkStartConstraints( cs->earlyStart );
            propagateEarliestStart( cs->earlyStart );
            cs->setPhaseName( 2, i18nc( "Schedule project forward", "Forward" ) );
            cs->logInfo( i18n( "Calculate late finish" ), 2 );
            cs->lateFinish = cs->calculateForward( estType );
            cs->setPhaseName( 3, i18n( "Schedule" ) );
            cs->logInfo( i18n( "Schedule tasks backward" ), 3 );
            cs->startTime = scheduleBackward( cs->lateFinish, estType );
            cs->endTime = cs->startTime;
            foreach ( Node *n, allNodes() ) {
                if ( n->type() == Type_Task || n->type() == Type_Milestone ) {
                    DateTime e = n->endTime( cs->id() );
                    if ( cs->endTime <  e ) {
                        cs->endTime = e;
                    }
                }
            }
            if ( cs->endTime > m_constraintEndTime ) {
                cs->schedulingError = true;
                cs->logError( i18n( "Failed to finish project within target time" ), 3 );
            }
            cs->duration = cs->endTime - cs->startTime;
            cs->logInfo( i18n( "Scheduled start: %1, target time: %2", locale->formatDateTime( cs->startTime ), locale->formatDateTime( m_constraintStartTime) ), 3 );
            if ( cs->startTime < m_constraintStartTime ) {
                cs->schedulingError = true;
                cs->logError( i18n( "Must start project early in order to finish in time: %1", locale->formatDateTime( m_constraintStartTime) ), 3 );
            } else if ( cs->startTime == m_constraintStartTime ) {
                cs->logWarning( i18n( "Start project exactly on time: %1", locale->formatDateTime( m_constraintStartTime ) ), 3 );
            } else {
                cs->logInfo( i18n( "Can start project later than time: %1", locale->formatDateTime( m_constraintStartTime ) ), 3 );
            }
            calcCriticalPath( true );
        }
        //makeAppointments();
        calcResourceOverbooked();
        cs->notScheduled = false;
        calcFreeFloat();
        emit scheduleChanged( cs );
        emit changed();
    } else if ( type() == Type_Subproject ) {
        kWarning() << "Subprojects not implemented";
    } else {
        kError() << "Illegal project type: " << type();
    }
}

void Project::incProgress()
{
    m_progress += 1;
    emit sigProgress( m_progress );
}

bool Project::calcCriticalPath( bool fromEnd )
{
    //kDebug();
    MainSchedule *cs = static_cast<MainSchedule*>( m_currentSchedule );
    if ( cs == 0 ) {
        return false;
    }
    if ( fromEnd ) {
        QListIterator<Node*> startnodes = cs->startNodes();
        while ( startnodes.hasNext() ) {
            startnodes.next() ->calcCriticalPath( fromEnd );
        }
    } else {
        QListIterator<Node*> endnodes = cs->endNodes();
        while ( endnodes.hasNext() ) {
            endnodes.next() ->calcCriticalPath( fromEnd );
        }
    }
    calcCriticalPathList( cs );
    return false;
}

void Project::calcCriticalPathList( MainSchedule *cs )
{
    //kDebug()<<m_name<<", "<<cs->name();
    cs->clearCriticalPathList();
    foreach ( Node *n, allNodes() ) {
        if ( n->numDependParentNodes() == 0 && n->inCriticalPath( cs->id() ) ) {
            cs->addCriticalPath();
            cs->addCriticalPathNode( n );
            calcCriticalPathList( cs, n );
        }
    }
    cs->criticalPathListCached = true;
    //kDebug()<<*(criticalPathList( cs->id() ));
}

void Project::calcCriticalPathList( MainSchedule *cs, Node *node )
{
    //kDebug()<<node->name()<<", "<<cs->id();
    bool newPath = false;
    QList<Node*> lst = *( cs->currentCriticalPath() );
    foreach ( Relation *r, node->dependChildNodes() ) {
        if ( r->child()->inCriticalPath( cs->id() ) ) {
            if ( newPath ) {
                cs->addCriticalPath( &lst );
                //kDebug()<<node->name()<<" new path";
            }
            cs->addCriticalPathNode( r->child() );
            calcCriticalPathList( cs, r->child() );
            newPath = true;
        }
    }
}

const QList< QList<Node*> > *Project::criticalPathList( long id )
{
    Schedule *s = schedule( id );
    if ( s == 0 ) {
        //kDebug()<<"No schedule with id="<<id;
        return 0;
    }
    MainSchedule *ms = static_cast<MainSchedule*>( s );
    if ( ! ms->criticalPathListCached ) {
        initiateCalculationLists( *ms );
        calcCriticalPathList( ms );
    }
    return ms->criticalPathList();
}

QList<Node*> Project::criticalPath( long id, int index )
{
    Schedule *s = schedule( id );
    if ( s == 0 ) {
        //kDebug()<<"No schedule with id="<<id;
        return QList<Node*>();
    }
    MainSchedule *ms = static_cast<MainSchedule*>( s );
    if ( ! ms->criticalPathListCached ) {
        initiateCalculationLists( *ms );
        calcCriticalPathList( ms );
    }
    return ms->criticalPath( index );
}

DateTime Project::startTime( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->startTime : m_constraintStartTime;
}

DateTime Project::endTime(  long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->endTime : m_constraintEndTime;
}

Duration Project::duration( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->duration : Duration::zeroDuration;
}

Duration *Project::getRandomDuration()
{
    return 0L;
}

DateTime Project::checkStartConstraints( const DateTime &dt ) const
{
    DateTime t = dt;
    foreach ( Node *n, nodeIdDict ) {
        if ( n->type() == Node::Type_Task || n->type() == Node::Type_Milestone ) {
            switch ( n->constraint() ) {
                case Node::FixedInterval:
                case Node::StartNotEarlier:
                case Node::MustStartOn:
                        t = qMin( t, qMax( n->constraintStartTime(), m_constraintStartTime ) );
                        break;
                default: break;
            }
        }
    }
    return t;
}

DateTime Project::checkEndConstraints( const DateTime &dt ) const
{
    DateTime t = dt;
    foreach ( Node *n, nodeIdDict ) {
        if ( n->type() == Node::Type_Task || n->type() == Node::Type_Milestone ) {
            switch ( n->constraint() ) {
                case Node::FixedInterval:
                case Node::FinishNotLater:
                case Node::MustFinishOn:
                        t = qMax( t, qMin( n->constraintEndTime(), m_constraintEndTime ) );
                        break;
                default: break;
            }
        }
    }
    return t;
}

DateTime Project::calculateForward( int use )
{
    //kDebug()<<m_name;
    MainSchedule *cs = static_cast<MainSchedule*>( m_currentSchedule );
    if ( cs == 0 ) {
        return DateTime();
    }
    if ( type() == Node::Type_Project ) {
        // Do constrained first
        foreach ( Node *n, cs->hardConstraints() ) {
            n->calculateEarlyFinish( use );
        }
        // Follow *parent* relations back and
        // calculate forwards following the child relations
        DateTime finish;
        DateTime time;
        foreach ( Node *n, cs->endNodes() ) {
            time = n->calculateForward( use );
            if ( !finish.isValid() || time > finish )
                finish = time;
        }
        //kDebug()<<m_name<<" finish="<<finish.toString();
        return finish;
    } else {
        //TODO: subproject
    }
    return DateTime();
}

DateTime Project::calculateBackward( int use )
{
    //kDebug()<<m_name;
    MainSchedule *cs = static_cast<MainSchedule*>( m_currentSchedule );
    if ( cs == 0 ) {
        return DateTime();
    }
    if ( type() == Node::Type_Project ) {
        // Do constrained first
        foreach ( Node *n, cs->hardConstraints() ) {
            n->calculateLateStart( use );
        }
        // Follow *child* relations back and
        // calculate backwards following parent relation
        DateTime start;
        DateTime time;
        foreach ( Node *n, cs->startNodes() ) {
            time = n->calculateBackward( use );
            if ( !start.isValid() || time < start )
                start = time;
        }
        //kDebug()<<m_name<<" start="<<start.toString();
        return start;
    } else {
        //TODO: subproject
    }
    return DateTime();
}

DateTime Project::scheduleForward( const DateTime &earliest, int use )
{
    MainSchedule *cs = static_cast<MainSchedule*>( m_currentSchedule );
    if ( cs == 0 ) {
        return DateTime();
    }
    resetVisited();
    DateTime end = earliest;
    DateTime time;
    QListIterator<Node*> it( cs->endNodes() );
    while ( it.hasNext() ) {
        time = it.next() ->scheduleForward( earliest, use );
        if ( time > end )
            end = time;
    }
    // Fix summarytasks
    adjustSummarytask();
    return end;
}

DateTime Project::scheduleBackward( const DateTime &latest, int use )
{
    MainSchedule *cs = static_cast<MainSchedule*>( m_currentSchedule );
    if ( cs == 0 ) {
        return DateTime();
    }
    resetVisited();
    DateTime start = latest;
    DateTime time;
    QListIterator<Node*> it( cs->startNodes() );
    while ( it.hasNext() ) {
        time = it.next() ->scheduleBackward( latest, use );
        if ( time < start )
            start = time;
    }
    // Fix summarytasks
    adjustSummarytask();
    return start;
}

void Project::adjustSummarytask()
{
    MainSchedule *cs = static_cast<MainSchedule*>( m_currentSchedule );
    if ( cs == 0 ) {
        return;
    }
    QListIterator<Node*> it( cs->summaryTasks() );
    while ( it.hasNext() ) {
        it.next() ->adjustSummarytask();
    }
}

void Project::initiateCalculation( MainSchedule &sch )
{
    //kDebug()<<m_name;
    // clear all resource appointments
    m_visitedForward = false;
    m_visitedBackward = false;
    QListIterator<ResourceGroup*> git( m_resourceGroups );
    while ( git.hasNext() ) {
        git.next() ->initiateCalculation( sch );
    }
    Node::initiateCalculation( sch );
}

void Project::initiateCalculationLists( MainSchedule &sch )
{
    //kDebug()<<m_name;
    sch.clearNodes();
    if ( type() == Node::Type_Project ) {
        QListIterator<Node*> it = childNodeIterator();
        while ( it.hasNext() ) {
            it.next() ->initiateCalculationLists( sch );
        }
    } else {
        //TODO: subproject
    }
}

bool Project::load( KoXmlElement &element, XMLLoaderObject &status )
{
    //kDebug()<<"--->";
    // load locale first
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if ( e.tagName() == "locale" ) {
            KLocale *l = locale();
            l->setCurrencySymbol( e.attribute( "currency-symbol", l->currencySymbol() ) );

//            l->setMonetaryDecimalSymbol( e.attribute( "monetary-decimal-symbol", l->monetaryDecimalSymbol() ) );

//            l->setMonetaryThousandsSeparator( e.attribute( "monetary-thousands-separator", l->monetaryThousandsSeparator() ) );
            if ( e.hasAttribute( "positive-monetary-sign-position" ) ) {
                l->setPositiveMonetarySignPosition( (KLocale::SignPosition)( e.attribute( "positive-monetary-sign-position" ).toInt() ) );
            }
            if ( e.hasAttribute( "positive-prefix-currency-symbol" ) ) {
                l->setPositivePrefixCurrencySymbol( e.attribute( "positive-prefix-currency-symbol" ).toInt() );
            }
            if ( e.hasAttribute( "negative-monetary-sign-position" ) ) {
                l->setNegativeMonetarySignPosition( (KLocale::SignPosition)( e.attribute( "negative-monetary-sign-position" ).toInt() ) );
            }
            if ( e.hasAttribute( "negative-prefix-currency-symbol" ) ) {
                l->setNegativePrefixCurrencySymbol( e.attribute( "negative-prefix-currency-symbol" ).toInt() );
            }
            //qDebug()<<"project load:"<<l->currencySymbol();
        }
    }
    QList<Calendar*> cals;
    QString s;
    bool ok = false;
    QString id = element.attribute( "id" );
    if ( !setId( id ) ) {
        kWarning() << "Id must be unique: " << id;
    }
    m_name = element.attribute( "name" );
    m_leader = element.attribute( "leader" );
    m_description = element.attribute( "description" );
    KTimeZone tz = KSystemTimeZones::zone( element.attribute( "timezone" ) );
    if ( tz.isValid() ) {
        m_spec = KDateTime::Spec( tz );
    } else kWarning()<<"No timezone specified, using default (local)";
    status.setProjectSpec( m_spec );
    
    // Allow for both numeric and text
    s = element.attribute( "scheduling", "0" );
    m_constraint = ( Node::ConstraintType ) s.toInt( &ok );
    if ( !ok )
        setConstraint( s );
    if ( m_constraint != Node::MustStartOn &&
            m_constraint != Node::MustFinishOn ) {
        kError() << "Illegal constraint: " << constraintToString();
        setConstraint( Node::MustStartOn );
    }
    s = element.attribute( "start-time" );
    if ( !s.isEmpty() )
        m_constraintStartTime = DateTime::fromString( s, m_spec );
    s = element.attribute( "end-time" );
    if ( !s.isEmpty() )
        m_constraintEndTime = DateTime::fromString( s, m_spec );

    // Load the project children
    // Do calendars first, they only refrence other calendars
    //kDebug()<<"Calendars--->";
    n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if ( e.tagName() == "calendar" ) {
            // Load the calendar.
            // Referenced by resources
            Calendar * child = new Calendar();
            child->setProject( this );
            if ( child->load( e, status ) ) {
                cals.append( child ); // temporary, reorder later
            } else {
                // TODO: Complain about this
                kError() << "Failed to load calendar";
                delete child;
            }
        } else if ( e.tagName() == "standard-worktime" ) {
            // Load standard worktime
            StandardWorktime * child = new StandardWorktime();
            if ( child->load( e, status ) ) {
                setStandardWorktime( child );
            } else {
                kError() << "Failed to load standard worktime";
                delete child;
            }
        }
    }
    // calendars references calendars in arbritary saved order
    bool added = false;
    do {
        added = false;
        QList<Calendar*> lst;
        while ( !cals.isEmpty() ) {
            Calendar *c = cals.takeFirst();
            if ( c->parentId().isEmpty() ) {
                addCalendar( c, status.baseCalendar() ); // handle pre 0.6 version
                added = true;
                //kDebug()<<"added to project:"<<c->name();
            } else {
                Calendar *par = calendar( c->parentId() );
                if ( par ) {
                    addCalendar( c, par );
                    added = true;
                    //kDebug()<<"added:"<<c->name()<<" to parent:"<<par->name();
                } else {
                    lst.append( c ); // treat later
                    //kDebug()<<"treat later:"<<c->name();
                }
            }
        }
        cals = lst;
    } while ( added );
    if ( ! cals.isEmpty() ) {
        kError()<<"All calendars not saved!";
    }
    //kDebug()<<"Calendars<---";
    // Resource groups and resources, can reference calendars
    n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if ( e.tagName() == "resource-group" ) {
            // Load the resources
            // References calendars
            ResourceGroup * child = new ResourceGroup();
            if ( child->load( e, status ) ) {
                addResourceGroup( child );
            } else {
                // TODO: Complain about this
                delete child;
            }
        }
    }
    // The main stuff
    n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if ( e.tagName() == "project" ) {
            //kDebug()<<"Sub project--->";
/*                // Load the subproject
            Project * child = new Project( this );
            if ( child->load( e ) ) {
                if ( !addTask( child, this ) ) {
                    delete child; // TODO: Complain about this
                }
            } else {
                // TODO: Complain about this
                delete child;
            }*/
        } else if ( e.tagName() == "task" ) {
            //kDebug()<<"Task--->";
            // Load the task (and resourcerequests).
            // Depends on resources already loaded
            Task * child = new Task( this );
            if ( child->load( e, status ) ) {
                if ( !addTask( child, this ) ) {
                    delete child; // TODO: Complain about this
                }
            } else {
                // TODO: Complain about this
                delete child;
            }
        }
    }
    // These go last
    n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if ( e.tagName() == "accounts" ) {
            //kDebug()<<"Accounts--->";
            // Load accounts
            // References tasks
            if ( !m_accounts.load( e, *this ) ) {
                kError() << "Failed to load accounts";
            }
        } else if ( e.tagName() == "relation" ) {
            //kDebug()<<"Relation--->";
            // Load the relation
            // References tasks
            Relation * child = new Relation();
            if ( !child->load( e, *this ) ) {
                // TODO: Complain about this
                kError() << "Failed to load relation";
                delete child;
            }
            //kDebug()<<"Relation<---";
        } else if ( e.tagName() == "schedules" ) {
            //kDebug()<<"Project schedules & task appointments--->";
            // References tasks and resources
            n = e.firstChild();
            for ( ; ! n.isNull(); n = n.nextSibling() ) {
                if ( ! n.isElement() ) {
                    continue;
                }
                KoXmlElement el = n.toElement();
                //kDebug()<<el.tagName()<<" Version="<<status.version();
                ScheduleManager *sm = 0;
                bool add = false;
                if ( status.version() <= "0.5" ) {
                    if ( el.tagName() == "schedule" ) {
                        sm = findScheduleManager( el.attribute( "name" ) );
                        if ( sm == 0 ) {
                            sm = new ScheduleManager( *this, el.attribute( "name" ) );
                            add = true;
                        }
                    }
                } else if ( el.tagName() == "plan" ) {
                    sm = new ScheduleManager( *this );
                    add = true;
                }
                if ( sm ) {
                    if ( sm->loadXML( el, status ) ) {
                        if ( add )
                            addScheduleManager( sm );
                    } else {
                        kError() << "Failed to load schedule manager";
                        delete sm;
                    }
                } else {
                    kDebug()<<"No schedule manager ?!";
                }
            }
            //kDebug()<<"Node schedules<---";
        } else if ( e.tagName() == "wbs-definition" ) {
            m_wbsDefinition.loadXML( e, status );
        } else {
            kWarning()<<"Unhandled tag:"<<e.tagName();
        }
    }
    //kDebug()<<"<---";
    return true;
}

void Project::save( QDomElement &element ) const
{
    QDomElement me = element.ownerDocument().createElement( "project" );
    element.appendChild( me );

    me.setAttribute( "name", m_name );
    me.setAttribute( "leader", m_leader );
    me.setAttribute( "id", m_id );
    me.setAttribute( "description", m_description );
    me.setAttribute( "timezone", m_spec.timeZone().name() );
    
    me.setAttribute( "scheduling", constraintToString() );
    me.setAttribute( "start-time", m_constraintStartTime.toString( KDateTime::ISODate ) );
    me.setAttribute( "end-time", m_constraintEndTime.toString( KDateTime::ISODate ) );

    m_wbsDefinition.saveXML( me );
    
    QDomElement loc = me.ownerDocument().createElement( "locale" );
    me.appendChild( loc );
    const KLocale *l = locale();
    loc.setAttribute( "currency-symbol", l->currencySymbol() );
//    loc.setAttribute( "monetary-decimal-symbol", l->monetaryDecimalSymbol() );
//    loc.setAttribute( "monetary-thousands-separator", l->monetaryThousandsSeparator() );
    loc.setAttribute( "positive-monetary-sign-position", l->positiveMonetarySignPosition() );
    loc.setAttribute( "positive-prefix-currency-symbol", l->positivePrefixCurrencySymbol() );
    loc.setAttribute( "negative-monetary-sign-position", l->negativeMonetarySignPosition() );
    loc.setAttribute( "negative-prefix-currency-symbol", l->negativePrefixCurrencySymbol() );
    
    m_accounts.save( me );

    // save calendars
    foreach ( Calendar *c, calendarIdDict ) {
        c->save( me );
    }
    // save standard worktime
    if ( m_standardWorktime )
        m_standardWorktime->save( me );

    // save project resources, must be after calendars
    QListIterator<ResourceGroup*> git( m_resourceGroups );
    while ( git.hasNext() ) {
        git.next() ->save( me );
    }

    // Only save parent relations
    QListIterator<Relation*> it( m_dependParentNodes );
    while ( it.hasNext() ) {
        it.next() ->save( me );
    }

    for ( int i = 0; i < numChildren(); i++ )
        // Save all children
        childNode( i ) ->save( me );

    // Now we can save relations assuming no tasks have relations outside the project
    QListIterator<Node*> nodes( m_nodes );
    while ( nodes.hasNext() ) {
        nodes.next() ->saveRelations( me );
    }

    if ( !m_managers.isEmpty() ) {
        QDomElement el = me.ownerDocument().createElement( "schedules" );
        me.appendChild( el );
        foreach ( ScheduleManager *sm, m_managers ) {
            sm->saveXML( el );
        }
    }
}

void Project::saveWorkPackageXML( QDomElement &element, const Node *node, long id ) const
{
    QDomElement me = element.ownerDocument().createElement( "project" );
    element.appendChild( me );

    me.setAttribute( "name", m_name );
    me.setAttribute( "leader", m_leader );
    me.setAttribute( "id", m_id );
    me.setAttribute( "description", m_description );
    me.setAttribute( "timezone", m_spec.timeZone().name() );
    
    me.setAttribute( "scheduling", constraintToString() );
    me.setAttribute( "start-time", m_constraintStartTime.toString( KDateTime::ISODate ) );
    me.setAttribute( "end-time", m_constraintEndTime.toString( KDateTime::ISODate ) );

    QListIterator<ResourceGroup*> git( m_resourceGroups );
    while ( git.hasNext() ) {
        git.next() ->saveWorkPackageXML( me, node->assignedResources( id ) );
    }

    if ( node == 0 ) {
        return;
    }
    node->saveWorkPackageXML( me, id );

    ScheduleManager *sm = scheduleManager( id );
    if ( sm ) {
        QDomElement el = me.ownerDocument().createElement( "schedules" );
        me.appendChild( el );
        sm->saveWorkPackageXML( el, *node );
    }
}

void Project::setParentSchedule( Schedule *sch )
{
    QListIterator<Node*> it = m_nodes;
    while ( it.hasNext() ) {
        it.next() ->setParentSchedule( sch );
    }
}

void Project::addResourceGroup( ResourceGroup *group, int index )
{
    int i = index == -1 ? m_resourceGroups.count() : index;
    emit resourceGroupToBeAdded( group, i );
    m_resourceGroups.insert( i, group );
    setResourceGroupId( group );
    group->setProject( this );
    foreach ( Resource *r, group->resources() ) {
        setResourceId( r );
        r->setProject( this );
    }
    emit resourceGroupAdded( group );
    emit changed();
}

ResourceGroup *Project::takeResourceGroup( ResourceGroup *group )
{
    int i = m_resourceGroups.indexOf( group );
    Q_ASSERT( i != -1 );
    if ( i == -1 ) {
        return 0;
    }
    emit resourceGroupToBeRemoved( group );
    ResourceGroup *g = m_resourceGroups.takeAt( i );
    Q_ASSERT( group == g );
    g->setProject( 0 );
    removeResourceGroupId( g->id() );
    foreach ( Resource *r, g->resources() ) {
        r->setProject( 0 );
        removeResourceId( r->id() );
    }
    emit resourceGroupRemoved( g );
    emit changed();
    return g;
}

QList<ResourceGroup*> &Project::resourceGroups()
{
    return m_resourceGroups;
}

void Project::addResource( ResourceGroup *group, Resource *resource, int index )
{
    int i = index == -1 ? group->numResources() : index;
    emit resourceToBeAdded( group, i );
    group->addResource( i, resource, 0 );
    setResourceId( resource );
    emit resourceAdded( resource );
    emit changed();
}

Resource *Project::takeResource( ResourceGroup *group, Resource *resource )
{
    emit resourceToBeRemoved( resource );
    bool result = removeResourceId( resource->id() );
    Q_ASSERT( result == true );
    resource->removeRequests(); // not valid anymore
    Resource *r = group->takeResource( resource );
    Q_ASSERT( resource == r );
    emit resourceRemoved( resource );
    emit changed();
    return r;
}

void Project::moveResource( ResourceGroup *group, Resource *resource )
{
    if ( group == resource->parentGroup() ) {
        return;
    }
    takeResource( resource->parentGroup(), resource );
    addResource( group, resource );
    return;
}


bool Project::addTask( Node* task, Node* position )
{
    // we want to add a task at the given position. => the new node will
    // become next sibling right after position.
    if ( 0 == position ) {
        return addSubTask( task, this );
    }
    //kDebug()<<"Add"<<task->name()<<" after"<<position->name();
    // in case we want to add to the main project, we make it child element
    // of the root element.
    if ( Node::Type_Project == position->type() ) {
        return addSubTask( task, position );
    }
    // find the position
    // we have to tell the parent that we want to delete one of its children
    Node* parentNode = position->parentNode();
    if ( !parentNode ) {
        kDebug() <<"parent node not found???";
        return false;
    }
    int index = parentNode->findChildNode( position );
    if ( -1 == index ) {
        // ok, it does not exist
        kDebug() <<"Task not found???";
        return false;
    }
    return addSubTask( task, index + 1, parentNode );
}

bool Project::addSubTask( Node* task, Node* parent )
{
    // append task to parent
    return addSubTask( task, -1, parent );
}

bool Project::addSubTask( Node* task, int index, Node* parent, bool emitSignal )
{
    // we want to add a subtask to the node "parent" at the given index.
    // If parent is 0, add to this
    Node *p = parent;
    if ( 0 == p ) {
        p = this;
    }
    if ( !registerNodeId( task ) ) {
        kError() << "Failed to register node id, can not add subtask: " << task->name();
        return false;
    }
    int i = index == -1 ? p->numChildren() : index;
    if ( emitSignal ) emit nodeToBeAdded( p, i );
    p->insertChildNode( i, task );
    connect( this, SIGNAL( standardWorktimeChanged( StandardWorktime* ) ), task, SLOT( slotStandardWorktimeChanged( StandardWorktime* ) ) );
    if ( emitSignal ) {
        emit nodeAdded( task );
        emit changed();
    }
    if ( p != this && p->numChildren() == 1 ) {
        emit nodeChanged( p );
    }
    return true;
}

void Project::takeTask( Node *node, bool emitSignal )
{
    //kDebug()<<node->name();
    Node * parent = node->parentNode();
    if ( parent == 0 ) {
        kDebug() <<"Node must have a parent!";
        return;
    }
    removeId( node->id() );
    if ( emitSignal ) emit nodeToBeRemoved( node );
    disconnect( this, SIGNAL( standardWorktimeChanged( StandardWorktime* ) ), node, SLOT( slotStandardWorktimeChanged( StandardWorktime* ) ) );
    parent->takeChildNode( node );
    if ( emitSignal ) {
        emit nodeRemoved( node );
        emit changed();
    }
    //kDebug()<<node->name()<<"removed";
    if ( parent != this && parent->type() != Node::Type_Summarytask ) {
        emit nodeChanged( parent );
    }
}

bool Project::canMoveTask( Node* node, Node *newParent )
{
    //kDebug()<<node->name()<<" to"<<newParent->name();
    Node *p = newParent;
    while ( p && p != this ) {
        if ( ! node->canMoveTo( p ) ) {
            return false;
        }
        p = p->parentNode();
    }
    return true;
}

bool Project::moveTask( Node* node, Node *newParent, int newPos )
{
    //kDebug()<<node->name()<<" to"<<newParent->name()<<","<<newPos;
    if ( ! canMoveTask( node, newParent ) ) {
        return false;
    }
    Node *oldParent = node->parentNode();
    const Node *before = newParent->childNode( newPos );
    emit nodeToBeMoved( node );
    takeTask( node, false );
    int i = before == 0 ? newParent->numChildren() : newPos;
    addSubTask( node, i, newParent, false );
    emit nodeMoved( node );
    if ( oldParent != this && oldParent->numChildren() == 0 ) {
        emit nodeChanged( oldParent );
    }
    if ( newParent != this && newParent->numChildren() == 1 ) {
        emit nodeChanged( newParent );
    }
    return true;
}

bool Project::canIndentTask( Node* node )
{
    if ( 0 == node ) {
        // should always be != 0. At least we would get the Project,
        // but you never know who might change that, so better be careful
        return false;
    }
    if ( node->type() == Node::Type_Project ) {
        //kDebug()<<"The root node cannot be indented";
        return false;
    }
    // we have to find the parent of task to manipulate its list of children
    Node* parentNode = node->parentNode();
    if ( !parentNode ) {
        return false;
    }
    if ( parentNode->findChildNode( node ) == -1 ) {
        kError() << "Tasknot found???";
        return false;
    }
    Node *sib = node->siblingBefore();
    if ( !sib ) {
        //kDebug()<<"new parent node not found";
        return false;
    }
    if ( node->findParentRelation( sib ) || node->findChildRelation( sib ) ) {
        //kDebug()<<"Cannot have relations to parent";
        return false;
    }
    return true;
}

bool Project::indentTask( Node* node, int index )
{
    if ( canIndentTask( node ) ) {
        Node * newParent = node->siblingBefore();
        int i = index == -1 ? newParent->numChildren() : index;
        moveTask( node, newParent, i );
        //kDebug();
        return true;
    }
    return false;
}

bool Project::canUnindentTask( Node* node )
{
    if ( 0 == node ) {
        // is always != 0. At least we would get the Project, but you
        // never know who might change that, so better be careful
        return false;
    }
    if ( Node::Type_Project == node->type() ) {
        //kDebug()<<"The root node cannot be unindented";
        return false;
    }
    // we have to find the parent of task to manipulate its list of children
    // and we need the parent's parent too
    Node* parentNode = node->parentNode();
    if ( !parentNode ) {
        return false;
    }
    Node* grandParentNode = parentNode->parentNode();
    if ( !grandParentNode ) {
        //kDebug()<<"This node already is at the top level";
        return false;
    }
    int index = parentNode->findChildNode( node );
    if ( -1 == index ) {
        kError() << "Tasknot found???";
        return false;
    }
    return true;
}

bool Project::unindentTask( Node* node )
{
    if ( canUnindentTask( node ) ) {
        Node * parentNode = node->parentNode();
        Node *grandParentNode = parentNode->parentNode();
        int i = grandParentNode->indexOf( parentNode ) + 1;
        if ( i == 0 )  {
            i = grandParentNode->numChildren();
        }
        moveTask( node, grandParentNode, i );
        //kDebug();
        return true;
    }
    return false;
}

bool Project::canMoveTaskUp( Node* node )
{
    if ( node == 0 )
        return false; // safety
    // we have to find the parent of task to manipulate its list of children
    Node* parentNode = node->parentNode();
    if ( !parentNode ) {
        //kDebug()<<"No parent found";
        return false;
    }
    if ( parentNode->findChildNode( node ) == -1 ) {
        kError() << "Tasknot found???";
        return false;
    }
    if ( node->siblingBefore() ) {
        return true;
    }
    return false;
}

bool Project::moveTaskUp( Node* node )
{
    if ( canMoveTaskUp( node ) ) {
        moveTask( node, node->parentNode(), node->parentNode()->indexOf( node ) - 1 );
        return true;
    }
    return false;
}

bool Project::canMoveTaskDown( Node* node )
{
    if ( node == 0 )
        return false; // safety
    // we have to find the parent of task to manipulate its list of children
    Node* parentNode = node->parentNode();
    if ( !parentNode ) {
        return false;
    }
    if ( parentNode->findChildNode( node ) == -1 ) {
        kError() << "Tasknot found???";
        return false;
    }
    if ( node->siblingAfter() ) {
        return true;
    }
    return false;
}

bool Project::moveTaskDown( Node* node )
{
    if ( canMoveTaskDown( node ) ) {
        moveTask( node, node->parentNode(), node->parentNode()->indexOf( node ) + 1 );
        return true;
    }
    return false;
}

Task *Project::createTask( Node* parent )
{
    Task * node = new Task( parent );
    node->setId( uniqueNodeId() );
    return node;
}

Task *Project::createTask( const Task &def, Node* parent )
{
    Task * node = new Task( def, parent );
    node->setId( uniqueNodeId() );
    return node;
}

QString Project::uniqueNodeId( int seed )
{
    QString ident = KRandom::randomString( 10 );
    int i = seed;
    while ( findNode( ident ) ) {
        ident = KRandom::randomString( 10 );
    }
    return ident;
}

QString Project::uniqueNodeId( const QList<QString> &existingIds, int seed )
{
    QString id = uniqueNodeId( seed );
    while ( existingIds.contains( id ) ) {
        id = uniqueNodeId( seed );
    }
    return id;
}

bool Project::removeId( const QString &id )
{
    //kDebug() <<"id=" << id;
    return ( m_parent ? m_parent->removeId( id ) : nodeIdDict.remove( id ) );
}

void Project::insertId( const QString &id, Node *node )
{
    //kDebug() <<"id=" << id <<"" << node->name();
    if ( m_parent == 0 )
        return ( void ) nodeIdDict.insert( id, node );
    m_parent->insertId( id, node );
}

bool Project::registerNodeId( Node *node )
{
    if ( node->id().isEmpty() ) {
        kError() << "Id is empty.";
        return false;
    }
    Node *rn = findNode( node->id() );
    if ( rn == 0 ) {
        insertId( node->id(), node );
        return true;
    }
    if ( rn != node ) {
        kError() << "Id already exists for different task: " << node->id();
        return false;
    }
    return true;
}

QList<Node*> Project::allNodes()
{
    QList<Node*> lst = nodeIdDict.values();
    int me = lst.indexOf( this );
    if ( me != -1 ) {
        lst.removeAt( me );
    }
    return lst;
}
    
bool Project::setResourceGroupId( ResourceGroup *group )
{
    if ( group == 0 ) {
        return false;
    }
    if ( ! group->id().isEmpty() ) {
        ResourceGroup *g = findResourceGroup( group->id() );
        if ( group == g ) {
            return true;
        } else if ( g == 0 ) {
            insertResourceGroupId( group->id(), group );
            return true;;
        }
    }
    QString id = uniqueResourceGroupId();
    group->setId( id );
    if ( id.isEmpty() ) {
        return false;
    }
    insertResourceGroupId( id, group );
    return true;
}

QString Project::uniqueResourceGroupId() const {
    QString id = KRandom::randomString( 10 );
    while ( resourceGroupIdDict.contains( id ) ) {
        id = KRandom::randomString( 10 );
    }
    return id;
}

ResourceGroup *Project::group( const QString& id )
{
    return findResourceGroup( id );
}

ResourceGroup *Project::groupByName( const QString& name ) const
{
    foreach ( const QString &k, resourceGroupIdDict.keys() ) {
        ResourceGroup *g = resourceGroupIdDict[ k ];
        if ( g->name() == name ) {
            Q_ASSERT( k == g->id() );
            return g;
        }
    }
    return 0;
}

void Project::insertResourceId( const QString &id, Resource *resource )
{
    resourceIdDict.insert( id, resource );
}

bool Project::removeResourceId( const QString &id )
{
    return resourceIdDict.remove( id );
}

bool Project::setResourceId( Resource *resource )
{
    if ( resource == 0 ) {
        return false;
    }
    if ( ! resource->id().isEmpty() ) {
        Resource *r = findResource( resource->id() );
        if ( resource == r ) {
            return true;
        } else if ( r == 0 ) {
            insertResourceId( resource->id(), resource );
            return true;;
        }
    }
    QString id = uniqueResourceId();
    resource->setId( id );
    if ( id.isEmpty() ) {
        return false;
    }
    insertResourceId( id, resource );
    return true;
}

QString Project::uniqueResourceId() const {
    QString id = KRandom::randomString( 10 );
    while ( resourceIdDict.contains( id ) ) {
        id = KRandom::randomString( 10 );
    }
    return id;
}

Resource *Project::resource( const QString& id )
{
    return findResource( id );
}

Resource *Project::resourceByName( const QString& name ) const
{
    foreach ( const QString &k, resourceIdDict.keys() ) {
        Resource *r = resourceIdDict[ k ];
        if ( r->name() == name ) {
            Q_ASSERT( k == r->id() );
            return r;
        }
    }
    return 0;
}

QStringList Project::resourceNameList() const
{
    QStringList lst;
    foreach ( Resource *r, resourceIdDict ) {
        lst << r->name();
    }
    return lst;
}

EffortCostMap Project::plannedEffortCostPrDay( const QDate & start, const QDate &end, long id ) const
{
    //kDebug()<<start<<end<<id;
    EffortCostMap ec;
    QListIterator<Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        ec += it.next() ->plannedEffortCostPrDay( start, end, id );
    }
    return ec;
}

EffortCostMap Project::actualEffortCostPrDay( const QDate & start, const QDate &end, long id ) const
{
    //kDebug()<<start<<end<<id;
    EffortCostMap ec;
    QListIterator<Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        ec += it.next() ->actualEffortCostPrDay( start, end, id );
    }
    return ec;
}

// Returns the total planned effort for this project (or subproject)
Duration Project::plannedEffort( long id ) const
{
    //kDebug();
    Duration eff;
    QListIterator<Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        eff += it.next() ->plannedEffort(id);
    }
    return eff;
}

// Returns the total planned effort for this project (or subproject) on date
Duration Project::plannedEffort( const QDate &date, long id ) const
{
    //kDebug();
    Duration eff;
    QListIterator<Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        eff += it.next() ->plannedEffort( date, id );
    }
    return eff;
}

// Returns the total planned effort for this project (or subproject) upto and including date
Duration Project::plannedEffortTo( const QDate &date, long id ) const
{
    //kDebug();
    Duration eff;
    QListIterator<Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        eff += it.next() ->plannedEffortTo( date, id );
    }
    return eff;
}

// Returns the total actual effort for this project (or subproject)
Duration Project::actualEffort() const
{
    //kDebug();
    Duration eff;
    QListIterator<Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        eff += it.next() ->actualEffort();
    }
    return eff;
}

// Returns the total actual effort for this project (or subproject) on date
Duration Project::actualEffort( const QDate &date ) const
{
    //kDebug();
    Duration eff;
    QListIterator<Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        eff += it.next() ->actualEffort( date );
    }
    return eff;
}

// Returns the total actual effort for this project (or subproject) upto and including date
Duration Project::actualEffortTo( const QDate &date ) const
{
    //kDebug();
    Duration eff;
    QListIterator
    <Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        eff += it.next() ->actualEffortTo( date );
    }
    return eff;
}

EffortCost Project::plannedCost( long id ) const
{
    //kDebug();
    EffortCost c;
    QListIterator
    <Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        c += it.next() ->plannedCost(id);
    }
    return c;
}

// Returns the total planned effort for this project (or subproject) on date
double Project::plannedCost( const QDate &date, long id ) const
{
    //kDebug();
    double c = 0;
    QListIterator
    <Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        c += it.next() ->plannedCost( date, id );
    }
    return c;
}

// Returns the total planned effort for this project (or subproject) upto and including date
double Project::plannedCostTo( const QDate &date, long id ) const
{
    //kDebug();
    double c = 0;
    QListIterator
    <Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        c += it.next() ->plannedCostTo( date, id );
    }
    return c;
}

double Project::actualCost() const
{
    //kDebug();
    double c = 0;
    QListIterator
    <Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        c += it.next() ->actualCost();
    }
    return c;
}

// Returns the total planned effort for this project (or subproject) on date
double Project::actualCost( const QDate &date ) const
{
    //kDebug();
    double c = 0;
    QListIterator
    <Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        c += it.next() ->actualCost( date );
    }
    return c;
}

// Returns the total planned effort for this project (or subproject) upto and including date
EffortCost Project::actualCostTo( const QDate &date ) const
{
    //kDebug();
    EffortCost c;
    QListIterator<Node*> it( childNodeIterator() );
    while ( it.hasNext() ) {
        c += it.next() ->actualCostTo( date );
    }
    return c;
}

Duration Project::budgetedWorkPerformed( const QDate &date, long id ) const
{
    //kDebug();
    Duration e;
    foreach (Node *n, childNodeIterator()) {
        e += n->budgetedWorkPerformed( date, id );
    }
    return e;
}

double Project::budgetedCostPerformed( const QDate &date, long id ) const
{
    //kDebug();
    double c = 0.0;
    foreach (Node *n, childNodeIterator()) {
        c += n->budgetedCostPerformed( date, id );
    }
    return c;
}

double Project::effortPerformanceIndex( const QDate &date, long id ) const
{
    //kDebug();
    kDebug()<<date<<id;
    Duration b = budgetedWorkPerformed( date, id );
    if ( b == Duration::zeroDuration ) {
        return 1.0;
    }
    Duration a = actualEffortTo( date );
    if ( b == Duration::zeroDuration ) {
        return 1.0;
    }
    return b.toDouble() / a.toDouble();
}

double Project::schedulePerformanceIndex( const QDate &date, long id ) const
{
    //kDebug();
    double r = 1.0;
    double s = bcws( date, id );
    double p = bcwp( date, id );
    if ( s > 0.0 ) {
        r = p / s;
    }
    kDebug()<<s<<p<<r;
    return r;
}

double Project::bcws( const QDate &date, long id ) const
{
    //kDebug();
    double c = plannedCostTo( date, id );
    kDebug()<<c;
    return c;
}

double Project::bcwp( long id ) const
{
    QDate date = QDate::currentDate();
    return bcwp( date, id );
}

double Project::bcwp( const QDate &date, long id ) const
{
    kDebug()<<date<<id;
    QDate start = startTime( id ).date();
    QDate end = endTime( id ).date();
    EffortCostMap plan = plannedEffortCostPrDay( start, end, id );
    EffortCostMap actual = actualEffortCostPrDay( start, (end > date ? end : date), id );

    double budgetAtCompletion;
    double plannedCompleted;
    double actualCompleted;
    double budgetedCompleted;
    bool useEffort = false; //FIXME
    if ( useEffort ) {
        budgetAtCompletion = plan.totalEffort().toDouble( Duration::Unit_h );
        plannedCompleted = plan.effortTo( date ).toDouble( Duration::Unit_h );
        //actualCompleted = actual.effortTo( date ).toDouble( Duration::Unit_h );
        actualCompleted = actualEffortTo( date ).toDouble( Duration::Unit_h );
        budgetedCompleted = budgetedWorkPerformed( date, id ).toDouble( Duration::Unit_h );
    } else {
        budgetAtCompletion = plan.totalCost();
        plannedCompleted = plan.costTo( date );
        actualCompleted = actual.costTo( date );
        budgetedCompleted = budgetedCostPerformed( date, id );
    }
    double percentageCompletion = budgetedCompleted / budgetAtCompletion;
    
    double c = budgetAtCompletion * percentageCompletion; //??
    kDebug()<<percentageCompletion<<budgetAtCompletion<<budgetedCompleted<<plannedCompleted;
    return c;
}

void Project::addCalendar( Calendar *calendar, Calendar *parent )
{
    Q_ASSERT( calendar != 0 );
    //kDebug()<<calendar->name()<<","<<(parent?parent->name():"No parent");
    int row = parent == 0 ? m_calendars.count() : parent->calendars().count();
    emit calendarToBeAdded( parent, row );
    calendar->setProject( this );
    if ( parent == 0 ) {
        calendar->setParentCal( 0 ); // in case
        m_calendars.append( calendar );
    } else {
        calendar->setParentCal( parent );
    }
    if ( calendar->isDefault() ) {
        setDefaultCalendar( calendar );
    }
    setCalendarId( calendar );
    emit calendarAdded( calendar );
    emit changed();
}

void Project::takeCalendar( Calendar *calendar )
{
    emit calendarToBeRemoved( calendar );
    removeCalendarId( calendar->id() );
    if ( calendar == m_defaultCalendar ) {
        m_defaultCalendar = 0;
    }
    if ( calendar->parentCal() == 0 ) {
        int i = indexOf( calendar );
        if ( i != -1 ) {
            m_calendars.removeAt( i );
        }
    } else {
        calendar->setParentCal( 0 );
    }
    emit calendarRemoved( calendar );
    calendar->setProject( 0 );
    emit changed();
}

int Project::indexOf( const Calendar *calendar ) const
{
    return m_calendars.indexOf( const_cast<Calendar*>(calendar) );
}

Calendar *Project::calendar( const QString& id ) const
{
    return findCalendar( id );
}

Calendar *Project::calendarByName( const QString& name ) const
{
    foreach( Calendar *c, calendarIdDict ) {
        if ( c->name() == name ) {
            return c;
        }
    }
    return 0;
}

const QList<Calendar*> &Project::calendars() const
{
    return m_calendars;
}

QList<Calendar*> Project::allCalendars() const
{
    return calendarIdDict.values();
}

QStringList Project::calendarNames() const
{
    QStringList lst;
    foreach( Calendar *c, calendarIdDict ) {
        lst << c->name();
    }
    return lst;
}

bool Project::setCalendarId( Calendar *calendar )
{
    if ( calendar == 0 ) {
        return false;
    }
    if ( ! calendar->id().isEmpty() ) {
        Calendar *c = findCalendar( calendar->id() );
        if ( calendar == c ) {
            return true;
        } else if ( c == 0 ) {
            insertCalendarId( calendar->id(), calendar );
            return true;;
        }
    }
    QString id = uniqueCalendarId();
    calendar->setId( id );
    if ( id.isEmpty() ) {
        return false;
    }
    insertCalendarId( id, calendar );
    return true;
}

QString Project::uniqueCalendarId() const {
    QString id = KRandom::randomString( 10 );
    while ( calendarIdDict.contains( id ) ) {
        id = KRandom::randomString( 10 );
    }
    return id;
}

void Project::setDefaultCalendar( Calendar *cal )
{
    if ( m_defaultCalendar ) {
        m_defaultCalendar->setDefault( false );
    }
    m_defaultCalendar = cal;
    if ( cal ) {
        cal->setDefault( true );
    }
    emit defaultCalendarChanged( cal );
    emit changed();
}

void Project::setStandardWorktime( StandardWorktime * worktime )
{
    if ( m_standardWorktime != worktime ) {
        delete m_standardWorktime;
        m_standardWorktime = worktime;
        m_standardWorktime->setProject( this );
        emit standardWorktimeChanged( worktime );
    }
}

bool Project::linkExists( const Node *par, const Node *child ) const
{
    if ( par == 0 || child == 0 || par == child || par->isDependChildOf( child ) ) {
        return false;
    }
    foreach ( Relation *r, par->dependChildNodes() ) {
        if ( r->child() == child ) {
            return true;
        }
    }
    return false;
}

bool Project::legalToLink( const Node *par, const Node *child ) const
{
    //kDebug()<<par.name()<<" ("<<par.numDependParentNodes()<<" parents)"<<child.name()<<" ("<<child.numDependChildNodes()<<" children)";

    if ( par == 0 || child == 0 || par == child || par->isDependChildOf( child ) ) {
        return false;
    }
    if ( linkExists( par, child ) ) {
        return false;
    }
    bool legal = true;
    // see if par/child is related
    if ( legal && ( par->isParentOf( child ) || child->isParentOf( par ) ) ) {
        legal = false;
    }
    if ( legal )
        legal = legalChildren( par, child );
    if ( legal )
        legal = legalParents( par, child );

    if ( legal ) {
        foreach ( Node *p, par->childNodeIterator() ) {
            if ( ! legalToLink( p, child ) ) {
                return false;
            }
        }
    }
    return legal;
}

bool Project::legalParents( const Node *par, const Node *child ) const
{
    bool legal = true;
    //kDebug()<<par->name()<<" ("<<par->numDependParentNodes()<<" parents)"<<child->name()<<" ("<<child->numDependChildNodes()<<" children)";
    for ( int i = 0; i < par->numDependParentNodes() && legal; ++i ) {
        Node *pNode = par->getDependParentNode( i ) ->parent();
        if ( child->isParentOf( pNode ) || pNode->isParentOf( child ) ) {
            //kDebug()<<"Found:"<<pNode->name()<<" is related to"<<child->name();
            legal = false;
        } else {
            legal = legalChildren( pNode, child );
        }
        if ( legal )
            legal = legalParents( pNode, child );
    }
    return legal;
}

bool Project::legalChildren( const Node *par, const Node *child ) const
{
    bool legal = true;
    //kDebug()<<par->name()<<" ("<<par->numDependParentNodes()<<" parents)"<<child->name()<<" ("<<child->numDependChildNodes()<<" children)";
    for ( int j = 0; j < child->numDependChildNodes() && legal; ++j ) {
        Node *cNode = child->getDependChildNode( j ) ->child();
        if ( par->isParentOf( cNode ) || cNode->isParentOf( par ) ) {
            //kDebug()<<"Found:"<<par->name()<<" is related to"<<cNode->name();
            legal = false;
        } else {
            legal = legalChildren( par, cNode );
        }
    }
    return legal;
}

WBSDefinition &Project::wbsDefinition()
{
    return m_wbsDefinition;
}

void Project::setWbsDefinition( const WBSDefinition &def )
{
    //kDebug();
    m_wbsDefinition = def;
    emit wbsDefinitionChanged();
    emit changed();
}

QString Project::generateWBSCode( QList<int> &indexes ) const
{
    QString code = m_wbsDefinition.projectCode();
    if ( ! code.isEmpty() && ! indexes.isEmpty() ) {
        code += m_wbsDefinition.projectSeparator();
    }
    int level = 1;
    foreach ( int index, indexes ) {
        code += m_wbsDefinition.code( index + 1, level  );
        if ( level < indexes.count() ) {
            // not last level, add separator also
            code += m_wbsDefinition.separator( level );
        }
        ++level;
    }
    //kDebug()<<code;
    return code;
}

void Project::setCurrentSchedule( long id )
{
    //kDebug();
    setCurrentSchedulePtr( findSchedule( id ) );
    Node::setCurrentSchedule( id );
    QHash<QString, Resource*> hash = resourceIdDict;
    foreach ( Resource * r, hash ) {
        r->setCurrentSchedule( id );
    }
    emit currentScheduleChanged();
    emit changed();
}

ScheduleManager *Project::scheduleManager( long id ) const
{
    foreach ( ScheduleManager *sm, m_managers ) {
        if ( sm->id() == id ) {
            return sm;
        }
    }
    return 0;
}

ScheduleManager *Project::findScheduleManager( const QString &name ) const
{
    //kDebug();
    ScheduleManager *m = 0;
    foreach( ScheduleManager *sm, m_managers ) {
        m = sm->findManager( name );
        if ( m ) {
            break;
        }
    }
    return m;
}

QList<ScheduleManager*> Project::allScheduleManagers() const
{
    QList<ScheduleManager*> lst;
    foreach ( ScheduleManager *sm, m_managers ) {
        lst << sm;
        lst << sm->allChildren();
    }
    return lst;
}

QString Project::uniqueScheduleName() const {
    //kDebug();
    QString n = i18n( "Plan" );
    bool unique = findScheduleManager( n ) == 0;
    if ( unique ) {
        return n;
    }
    n += " %1";
    int i = 1;
    for ( ; true; ++i ) {
        unique = findScheduleManager( n.arg( i ) ) == 0;;
        if ( unique ) {
            break;
        }
    }
    return n.arg( i );
}

void Project::addScheduleManager( ScheduleManager *sm, ScheduleManager *parent )
{
    if ( parent == 0 ) {
        emit scheduleManagerToBeAdded( parent, m_managers.count() );
        m_managers.append( sm );
    } else {
        emit scheduleManagerToBeAdded( parent, parent->children().count() );
        sm->setParentManager( parent );
    }
    emit scheduleManagerAdded( sm );
    emit changed();
    //kDebug()<<"Added:"<<sm->name()<<", now"<<m_managers.count();
}

int Project::takeScheduleManager( ScheduleManager *sm )
{
    foreach ( ScheduleManager *s, sm->children() ) {
        takeScheduleManager( s );
    }
    int index = -1;
    if ( sm->parentManager() ) {
        int index = sm->parentManager()->indexOf( sm );
        if ( index >= 0 ) {
            emit scheduleManagerToBeRemoved( sm );
            sm->setParentManager( 0 );
            emit scheduleManagerRemoved( sm );
            emit changed();
        }
    } else {
        index = indexOf( sm );
        if ( index >= 0 ) {
            emit scheduleManagerToBeRemoved( sm );
            m_managers.removeAt( indexOf( sm ) );
            emit scheduleManagerRemoved( sm );
            emit changed();
        }
    }
    return index;
}

bool Project::isScheduleManager( void *ptr ) const
{
    const ScheduleManager *sm = static_cast<ScheduleManager*>( ptr );
    if ( indexOf( sm ) >= 0 ) {
        return true;
    }
    foreach ( ScheduleManager *p, m_managers ) {
        if ( p->isParentOf( sm ) ) {
            return true;
        }
    }
    return false;
}

ScheduleManager *Project::createScheduleManager( const QString name )
{
    //kDebug()<<name;
    ScheduleManager *sm = new ScheduleManager( *this, name );
    return sm;
}

ScheduleManager *Project::createScheduleManager()
{
    //kDebug();
    return createScheduleManager( uniqueScheduleName() );
}

bool Project::isBaselined( long id ) const
{
    if ( id == ANYSCHEDULED ) {
        foreach ( ScheduleManager *sm, allScheduleManagers() ) {
            if ( sm->isBaselined() ) {
                return true;
            }
        }
        return false;
    }
    Schedule *s = schedule( id );
    return s == 0 ? false : s->isBaselined();
}

MainSchedule *Project::createSchedule( const QString& name, Schedule::Type type )
{
    //kDebug()<<"No of schedules:"<<m_schedules.count();
    MainSchedule *sch = new MainSchedule();
    sch->setName( name );
    sch->setType( type );
    addMainSchedule( sch );
    return sch;
}

void Project::addMainSchedule( MainSchedule *sch )
{
    if ( sch == 0 ) {
        return;
    }
    //kDebug()<<"No of schedules:"<<m_schedules.count();
    long i = 1; // keep this positive (negative values are special...)
    while ( m_schedules.contains( i ) ) {
        ++i;
    }
    sch->setId( i );
    sch->setNode( this );
    addSchedule( sch );
}

bool Project::removeCalendarId( const QString &id )
{
    //kDebug() <<"id=" << id;
    return calendarIdDict.remove( id );
}

void Project::insertCalendarId( const QString &id, Calendar *calendar )
{
    //kDebug() <<"id=" << id <<":" << calendar->name();
    calendarIdDict.insert( id, calendar );
}

void Project::changed( Node *node )
{
    if ( m_parent == 0 ) {
        emit nodeChanged( node );
        emit changed();
        return;
    }
    Node::changed( node );
}

void Project::changed( ResourceGroup *group )
{
    //kDebug();
    emit resourceGroupChanged( group );
    emit changed();
}

void Project::changed( ScheduleManager *sm )
{
    emit scheduleManagerChanged( sm );
    emit changed();
}

void Project::changed( MainSchedule *sch )
{
    //kDebug()<<sch->id();
    emit scheduleChanged( sch );
    emit changed();
}

void Project::sendScheduleToBeAdded( const ScheduleManager *sm, int row )
{
    emit scheduleToBeAdded( sm, row );
}

void Project::sendScheduleAdded( const MainSchedule *sch )
{
    //kDebug()<<sch->id();
    emit scheduleAdded( sch );
    emit changed();
}

void Project::sendScheduleToBeRemoved( const MainSchedule *sch )
{
    //kDebug()<<sch->id();
    emit scheduleToBeRemoved( sch );
}

void Project::sendScheduleRemoved( const MainSchedule *sch )
{
    //kDebug()<<sch->id();
    emit scheduleRemoved( sch );
    emit changed();
}

void Project::changed( Resource *resource )
{
    emit resourceChanged( resource );
    emit changed();
}

void Project::changed( Calendar *cal )
{
    emit calendarChanged( cal );
    emit changed();
}

void Project::changed( StandardWorktime *w )
{
    emit standardWorktimeChanged( w );
    emit changed();
}

bool Project::addRelation( Relation *rel, bool check )
{
    if ( rel->parent() == 0 || rel->child() == 0 ) {
        return false;
    }
    if ( check && !legalToLink( rel->parent(), rel->child() ) ) {
        return false;
    }
    emit relationToBeAdded( rel, rel->parent()->numDependChildNodes(), rel->child()->numDependParentNodes() );
    rel->parent()->addDependChildNode( rel );
    rel->child()->addDependParentNode( rel );
    emit relationAdded( rel );
    emit changed();
    return true;
}

void Project::takeRelation( Relation *rel )
{
    emit relationToBeRemoved( rel );
    rel->parent() ->takeDependChildNode( rel );
    rel->child() ->takeDependParentNode( rel );
    emit relationRemoved( rel );
    emit changed();
}

void Project::setRelationType( Relation *rel, Relation::Type type )
{
    rel->setType( type );
    emit relationModified( rel );
    emit changed();
}

void Project::setRelationLag( Relation *rel, const Duration &lag )
{
    rel->setLag( lag );
    emit relationModified( rel );
    emit changed();
}

QList<Node*> Project::flatNodeList( Node *parent )
{
    QList<Node*> lst;
    Node *p = parent == 0 ? this : parent;
    //kDebug()<<p->name()<<lst.count();
    foreach ( Node *n, p->childNodeIterator() ) {
        lst.append( n );
        if ( n->numChildren() > 0 ) {
            lst += flatNodeList( n );
        }
    }
    return lst;
}

void Project::setSchedulerPlugins( const QMap<QString, SchedulerPlugin*> &plugins )
{
    m_schedulerPlugins = plugins;
    kDebug()<<m_schedulerPlugins;
}

void Project::emitLocaleChanged()
{
    emit localeChanged();
}

#ifndef NDEBUG
void Project::printDebug( bool children, const QByteArray& _indent )
{
    QByteArray indent = _indent;
    qDebug() << indent <<"+ Project node:" << Node::name()<<" id="<<id();
    indent += '!';
    QListIterator<ResourceGroup*> it( resourceGroups() );
    while ( it.hasNext() )
        it.next() ->printDebug( indent );

    Node::printDebug( children, indent );
}
void Project::printCalendarDebug( const QByteArray& _indent )
{
    QByteArray indent = _indent;
    qDebug() << indent <<"-------- Calendars debug printout --------";
    foreach ( Calendar *c, calendarIdDict ) {
        c->printDebug( indent + "--" );
        qDebug();
    }
    if ( m_standardWorktime )
        m_standardWorktime->printDebug();
}
#endif


}  //KPlato namespace

#include "kptproject.moc"
