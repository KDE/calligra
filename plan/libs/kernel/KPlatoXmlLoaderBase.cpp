/* This file is part of the KDE project
 Copyright (C) 2010 Dag Andersen <danders@get2net.dk>

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

#include "KPlatoXmlLoaderBase.h"

#include "kptxmlloaderobject.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptcalendar.h"
#include "kptschedule.h"
#include "kptrelation.h"
#include "kptresource.h"
#include "kptaccount.h"
#include "kptappointment.h"

#include "KoXmlReader.h"

#include <KMessageBox>
#include <KTimeZone>
#include <KSystemTimeZones>
#include <KDebug>
#include <kdeversion.h>

#include <QDateTime>

using namespace KPlato;

KPLATOKERNEL_EXPORT int kplatoXmlDebugArea() {
#if KDE_IS_VERSION( 4, 3, 80 )
    static int s_area = KDebug::registerArea( "plan (kplato xml)" );
#else
    static int s_area = 0;
#endif
    return s_area;
}

KPlatoXmlLoaderBase::KPlatoXmlLoaderBase()
{
}

bool KPlatoXmlLoaderBase::load( Project *project, const KoXmlElement &element, XMLLoaderObject &status )
{
    kDebug(kplatoXmlDebugArea())<<"project";
    // load locale first
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if ( e.tagName() == "locale" ) {
            KLocale *l = project->locale();
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
        }
    }
    QList<Calendar*> cals;
    QString s;
    bool ok = false;
    project->setName( element.attribute( "name" ) );
    project->removeId( project->id() );
    project->setId( element.attribute( "id" ) );
    project->registerNodeId( project );
    project->setLeader( element.attribute( "leader" ) );
    project->setDescription( element.attribute( "description" ) );
    KTimeZone tz = KSystemTimeZones::zone( element.attribute( "timezone" ) );
    if ( tz.isValid() ) {
        project->setTimeZone( tz );
    } else kWarning()<<"No timezone specified, using default (local)";
    status.setProjectSpec( project->timeSpec() );

    // Allow for both numeric and text
    s = element.attribute( "scheduling", "0" );
    project->setConstraint( ( Node::ConstraintType ) ( s.toInt( &ok ) ) );
    if ( ! ok )
        project->setConstraint( s );
    if ( project->constraint() != Node::MustStartOn && project->constraint() != Node::MustFinishOn ) {
        kError() << "Illegal constraint: " << project->constraintToString();
        project->setConstraint( Node::MustStartOn );
    }
    s = element.attribute( "start-time" );
    if ( ! s.isEmpty() ) {
        project->setConstraintStartTime( DateTime::fromString( s, project->timeSpec() ) );
    }
    s = element.attribute( "end-time" );
    if ( ! s.isEmpty() ) {
        project->setConstraintEndTime( DateTime::fromString( s, project->timeSpec() ) );
    }
    // Load the project children
    // Do calendars first, they only refrence other calendars
    //kDebug(kplatoXmlDebugArea())<<"Calendars--->";
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
            child->setProject( project );
            if ( load( child, e, status ) ) {
                cals.append( child ); // temporary, reorder later
            } else {
                // TODO: Complain about this
                kError() << "Failed to load calendar";
                delete child;
            }
        } else if ( e.tagName() == "standard-worktime" ) {
            // Load standard worktime
            StandardWorktime * child = new StandardWorktime();
            if ( load( child, e, status ) ) {
                project->setStandardWorktime( child );
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
                project->addCalendar( c, status.baseCalendar() ); // handle pre 0.6 version
                added = true;
                //kDebug(kplatoXmlDebugArea())<<"added to project:"<<c->name();
            } else {
                Calendar *par = project->calendar( c->parentId() );
                if ( par ) {
                    project->addCalendar( c, par );
                    added = true;
                    //kDebug(kplatoXmlDebugArea())<<"added:"<<c->name()<<" to parent:"<<par->name();
                } else {
                    lst.append( c ); // treat later
                    //kDebug(kplatoXmlDebugArea())<<"treat later:"<<c->name();
                }
            }
        }
        cals = lst;
    } while ( added );
    if ( ! cals.isEmpty() ) {
        kError()<<"All calendars not saved!";
    }
    //kDebug(kplatoXmlDebugArea())<<"Calendars<---";
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
            if ( load( child, e, status ) ) {
                project->addResourceGroup( child );
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
            //kDebug(kplatoXmlDebugArea())<<"Sub project--->";
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
            //kDebug(kplatoXmlDebugArea())<<"Task--->";
            // Load the task (and resourcerequests).
            // Depends on resources already loaded
            Task *child = new Task( project );
            if ( load( child, e, status ) ) {
                if ( ! project->addTask( child, project ) ) {
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
        kDebug(kplatoXmlDebugArea())<<n.isElement();
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if ( e.tagName() == "accounts" ) {
            //kDebug(kplatoXmlDebugArea())<<"Accounts--->";
            // Load accounts
            // References tasks
            if ( ! load( project->accounts(), e, status ) ) {
                kError() << "Failed to load accounts";
            }
        } else if ( e.tagName() == "relation" ) {
            //kDebug(kplatoXmlDebugArea())<<"Relation--->";
            // Load the relation
            // References tasks
            Relation * child = new Relation();
            if ( ! load( child, e, status ) ) {
                // TODO: Complain about this
                kError() << "Failed to load relation";
                delete child;
            }
            //kDebug(kplatoXmlDebugArea())<<"Relation<---";
        } else if ( e.tagName() == "schedules" ) {
            kDebug(kplatoXmlDebugArea())<<"Project schedules & task appointments--->";
            // References tasks and resources
            KoXmlNode sn = e.firstChild();
            for ( ; ! sn.isNull(); sn = sn.nextSibling() ) {
                if ( ! sn.isElement() ) {
                    continue;
                }
                KoXmlElement el = sn.toElement();
                //kDebug(kplatoXmlDebugArea())<<el.tagName()<<" Version="<<status.version();
                ScheduleManager *sm = 0;
                bool add = false;
                if ( status.version() <= "0.5" ) {
                    if ( el.tagName() == "schedule" ) {
                        sm = project->findScheduleManagerByName( el.attribute( "name" ) );
                        if ( sm == 0 ) {
                            sm = new ScheduleManager( *project, el.attribute( "name" ) );
                            add = true;
                        }
                    }
                } else if ( el.tagName() == "plan" ) {
                    sm = new ScheduleManager( *project );
                    add = true;
                }
                if ( sm ) {
                    if ( load( sm, el, status ) ) {
                        if ( add )
                            project->addScheduleManager( sm );
                    } else {
                        kError() << "Failed to load schedule manager";
                        delete sm;
                    }
                } else {
                    kDebug(kplatoXmlDebugArea())<<"No schedule manager ?!";
                }
            }
            kDebug(kplatoXmlDebugArea())<<"Project schedules & task appointments<---";
        } else if ( e.tagName() == "resource-teams" ) {
            //kDebug(kplatoXmlDebugArea())<<"Resource teams--->";
            // References other resources
            KoXmlNode tn = e.firstChild();
            for ( ; ! tn.isNull(); tn = tn.nextSibling() ) {
                if ( ! tn.isElement() ) {
                    continue;
                }
                KoXmlElement el = tn.toElement();
                if ( el.tagName() == "team" ) {
                    Resource *r = project->findResource( el.attribute( "team-id" ) );
                    Resource *tm = project->findResource( el.attribute( "member-id" ) );
                    if ( r == 0 || tm == 0 ) {
                        kError()<<"resource-teams: cannot find resources";
                        continue;
                    }
                    if ( r == tm ) {
                        kError()<<"resource-teams: a team cannot be a member of itself";
                        continue;
                    }
                    r->addTeamMemberId( tm->id() );
                } else {
                    kError()<<"resource-teams: unhandled tag"<<el.tagName();
                }
            }
            //kDebug(kplatoXmlDebugArea())<<"Resource teams<---";
        } else if ( e.tagName() == "wbs-definition" ) {
            load( project->wbsDefinition(), e, status );
        } else if ( e.tagName() == "locale" ) {
            // handled earlier
        } else if ( e.tagName() == "resource-group" ) {
            // handled earlier
        } else if ( e.tagName() == "calendar" ) {
            // handled earlier
        } else if ( e.tagName() == "standard-worktime" ) {
            // handled earlier
        } else if ( e.tagName() == "project" ) {
            // handled earlier
        } else if ( e.tagName() == "task" ) {
            // handled earlier
        } else {
            kWarning()<<"Unhandled tag:"<<e.tagName();
        }
    }
    kDebug(kplatoXmlDebugArea())<<"Project loaded:"<<project<<project->name()<<project->allNodes();
    return true;
}

bool KPlatoXmlLoaderBase::load( Task *task, const KoXmlElement &element, XMLLoaderObject &status )
{
    kDebug(kplatoXmlDebugArea())<<"task";
    QString s;
    bool ok = false;
    task->setId( element.attribute("id") );

    task->setName( element.attribute("name") );
    task->setLeader( element.attribute("leader") );
    task->setDescription( element.attribute("description") );
    //kDebug(kplatoXmlDebugArea())<<m_name<<": id="<<m_id;

    // Allow for both numeric and text
    QString constraint = element.attribute("scheduling","0");
    task->setConstraint( (Node::ConstraintType)constraint.toInt(&ok) );
    if ( ! ok ) {
        task->setConstraint( constraint );
    }
    s = element.attribute("constraint-starttime");
    if ( ! s.isEmpty() ) {
        task->setConstraintStartTime( DateTime::fromString( s, status.projectSpec() ) );
    }
    s = element.attribute("constraint-endtime");
    if ( ! s.isEmpty() ) {
        task->setConstraintEndTime( DateTime::fromString( s, status.projectSpec() ) );
    }
    task->setStartupCost( element.attribute("startup-cost", "0.0").toDouble() );
    task->setShutdownCost( element.attribute("shutdown-cost", "0.0").toDouble() );

    // Load the task children
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if (e.tagName() == "project") {
            // Load the subproject
/*                Project *child = new Project(this, status);
            if (child->load(e)) {
                if (!project.addSubTask(child, this)) {
                    delete child;  // TODO: Complain about this
                }
            } else {
                // TODO: Complain about this
                delete child;
            }*/
        } else if (e.tagName() == "task") {
            // Load the task
            Task *child = new Task( task );
            if ( load( child, e, status) ) {
                if ( ! status.project().addSubTask( child, task ) ) {
                    delete child;  // TODO: Complain about this
                }
            } else {
                // TODO: Complain about this
                delete child;
            }
        } else if ( e.tagName() == "resource" ) {
            // tasks don't have resources
        } else if (e.tagName() == "estimate" || ( /*status.version() < "0.6" &&*/ e.tagName() == "effort" ) ) {
            //  Load the estimate
            load( task->estimate(), e, status);
        } else if ( e.tagName() == "resourcegroup-request" ) {
            // Load the resource request
            // Handle multiple requests to same group gracefully (Not really allowed)
            ResourceGroupRequest *r = task->requests().findGroupRequestById( e.attribute( "group-id" ) );
            if ( r ) {
                kWarning()<<"Multiple requests to same group, loading into existing group";
                if ( ! load( r, e, status ) ) {
                    kError()<<"Failed to load resource request";
                }
            } else {
                r = new ResourceGroupRequest();
                if ( load( r, e, status ) ) {
                    task->addRequest(r);
                } else {
                    kError()<<"Failed to load resource request";
                    delete r;
                }
            }
        } else if ( e.tagName() == "workpackage" ) {
            load( task->workPackage(), e, status );
        } else if ( e.tagName() == "progress" ) {
            load( task->completion(), e, status );
        } else if ( e.tagName() == "schedules" ) {
            KoXmlNode n = e.firstChild();
            for ( ; ! n.isNull(); n = n.nextSibling() ) {
                if ( ! n.isElement() ) {
                    continue;
                }
                KoXmlElement el = n.toElement();
                if ( el.tagName() == "schedule" ) {
                    NodeSchedule *sch = new NodeSchedule();
                    if ( loadNodeSchedule( sch, el, status ) ) {
                        sch->setNode( task );
                        task->addSchedule( sch );
                    } else {
                        kError()<<"Failed to load schedule";
                        delete sch;
                    }
                }
            }
        } else if (e.tagName() == "documents") {
            load( task->documents(), e, status );
        } else if ( e.tagName() == "workpackage-log" ) {
            KoXmlNode n = e.firstChild();
            for ( ; ! n.isNull(); n = n.nextSibling() ) {
                if ( ! n.isElement() ) {
                    continue;
                }
                KoXmlElement el = n.toElement();
                if ( el.tagName() == "workpackage" ) {
                    WorkPackage *wp = new WorkPackage( task );
                    if ( loadWpLog( wp, el, status ) ) {
                        task->addWorkPackage( wp );
                    } else {
                        kError()<<"Failed to load logged workpackage";
                        delete wp;
                    }
                }
            }
        }
    }
    //kDebug(kplatoXmlDebugArea())<<m_name<<" loaded";
    return true;
}

bool KPlatoXmlLoaderBase::load( Calendar *calendar, const KoXmlElement &element, XMLLoaderObject &status )
{
    kDebug(kplatoXmlDebugArea())<<"calendar"<<element.text();
    //bool ok;
    calendar->setId( element.attribute( "id" ) );
    calendar->setParentId( element.attribute( "parent" ) );
    calendar->setName( element.attribute( "name","" ) );
    KTimeZone tz = KSystemTimeZones::zone( element.attribute( "timezone" ) );
    if ( tz.isValid() ) {
        calendar->setTimeZone( tz );
    } else kWarning()<<"No timezone specified, use default (local)";
    bool dc = (bool)element.attribute( "default","0" ).toInt();
    if ( dc ) {
        status.project().setDefaultCalendar( calendar );
    }
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if (e.tagName() == "weekday") {
            if ( ! load( calendar->weekdays(), e, status ) ) {
                return false;
            }
        }
        if ( e.tagName() == "day" ) {
            CalendarDay *day = new CalendarDay();
            if ( load( day, e, status ) ) {
                if ( ! day->date().isValid() ) {
                    delete day;
                    kError()<<calendar->name()<<": Failed to load calendarDay - Invalid date";
                } else {
                    CalendarDay *d = calendar->findDay( day->date() );
                    if ( d ) {
                        // already exists, keep the new
                        delete calendar->takeDay(d);
                        kWarning()<<calendar->name()<<" Load calendarDay - Date already exists";
                    }
                    calendar->addDay( day );
                }
            } else {
                delete day;
                kError()<<"Failed to load calendarDay";
                return true; // don't throw away the whole calendar
            }
        }
    }
    return true;
}

bool KPlatoXmlLoaderBase::load( CalendarDay *day, const KoXmlElement &element, XMLLoaderObject &status )
{
    kDebug(kplatoXmlDebugArea())<<"day";
    bool ok=false;
    day->setState( QString( element.attribute( "state", "-1" ) ).toInt( &ok ) );
    if ( day->state() < 0 ) {
        return false;
    }
    //kDebug(kplatoXmlDebugArea())<<" state="<<m_state;
    QString s = element.attribute( "date" );
    if ( ! s.isEmpty() ) {
        day->setDate( QDate::fromString( s, Qt::ISODate ) );
        if ( ! day->date().isValid() ) {
            day->setDate( QDate::fromString( s ) );
        }
    }
    day->clearIntervals();
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if ( e.tagName() == "interval" ) {
            //kDebug(kplatoXmlDebugArea())<<"Interval start="<<e.attribute("start")<<" end="<<e.attribute("end");
            QString st = e.attribute("start");
            if (st.isEmpty() ) {
                kError()<<"Empty interval";
                continue;
            }
            QTime start = QTime::fromString( st );
            int length = 0;
            if ( status.version() <= "0.6.1" ) {
                QString en = e.attribute( "end" );
                if ( en.isEmpty() ) {
                    kError()<<"Invalid interval end";
                    continue;
                }
                QTime end = QTime::fromString( en );
                length = start.msecsTo( end );
            } else {
                length = e.attribute("length", "0").toInt();
            }
            if ( length <= 0 ) {
                kError()<<"Invalid interval length";
                continue;
            }
            day->addInterval( new TimeInterval( start, length ) );
        }
    }
    return true;
}

bool KPlatoXmlLoaderBase::load( CalendarWeekdays *weekdays, const KoXmlElement& element, XMLLoaderObject& status)
{
    kDebug(kplatoXmlDebugArea())<<"weekdays";
    bool ok;
    int dayNo = QString( element.attribute( "day","-1" ) ).toInt( &ok );
    if (dayNo < 0 || dayNo > 6) {
        kError()<<"Illegal weekday: "<<dayNo;
        return true; // we continue anyway
    }
    CalendarDay *day = weekdays->weekday( dayNo + 1 );
    if ( day == 0 ) {
        kError()<<"No weekday: "<<dayNo;
        return false;
    }
    if ( ! load( day, element, status ) ) {
        day->setState( CalendarDay::None );
    }
    return true;

}

bool KPlatoXmlLoaderBase::load( StandardWorktime *swt, const KoXmlElement &element, XMLLoaderObject &status )
{
    kDebug(kplatoXmlDebugArea())<<"swt";
    swt->setYear( Duration::fromString( element.attribute( "year" ), Duration::Format_Hour ) );
    swt->setMonth( Duration::fromString( element.attribute( "month" ), Duration::Format_Hour ) );
    swt->setWeek( Duration::fromString( element.attribute( "week" ), Duration::Format_Hour ) );
    swt->setDay( Duration::fromString( element.attribute( "day" ), Duration::Format_Hour ) );

    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if ( e.tagName() == "calendar" ) {
            // pre 0.6 version stored base calendar in standard worktime
            if ( status.version() >= "0.6" ) {
                kWarning()<<"Old format, calendar in standard worktime";
                kWarning()<<"Tries to load anyway";
            }
            // try to load anyway
            Calendar *calendar = new Calendar;
            if ( load( calendar, e, status ) ) {
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

bool KPlatoXmlLoaderBase::load( Relation *relation, const KoXmlElement &element, XMLLoaderObject &status )
{
    kDebug(kplatoXmlDebugArea())<<"relation";
    relation->setParent( status.project().findNode( element.attribute( "parent-id" ) ) );
    if (relation->parent() == 0) {
        return false;
    }
    relation->setChild( status.project().findNode( element.attribute( "child-id" ) ) );
    if ( relation->child() == 0 ) {
        return false;
    }
    if ( relation->child() == relation->parent() ) {
        kDebug(kplatoXmlDebugArea())<<"child == parent";
        return false;
    }
    if ( ! relation->parent()->legalToLink( relation->child() ) ) {
        return false;
    }
    relation->setType( element.attribute("type") );

    relation->setLag( Duration::fromString( element.attribute( "lag" ) ) );

    if ( ! relation->parent()->addDependChildNode( relation ) ) {
        kError()<<"Failed to add relation: Child="<<relation->child()->name()<<" parent="<<relation->parent()->name();
        return false;
    }
    if ( ! relation->child()->addDependParentNode( relation ) ) {
        relation->parent()->takeDependChildNode( relation );
        kError()<<"Failed to add relation: Child="<<relation->child()->name()<<" parent="<<relation->parent()->name();
        return false;
    }
    //kDebug(kplatoXmlDebugArea())<<"Added relation: Child="<<relation->child()->name()<<" parent="<<relation->parent()->name();
    return true;
}

bool KPlatoXmlLoaderBase::load( ResourceGroup *rg, const KoXmlElement &element, XMLLoaderObject &status )
{
    kDebug(kplatoXmlDebugArea())<<"resource-group";
    rg->setId( element.attribute( "id" ) );
    rg->setName( element.attribute( "name" ) );
    rg->setType( element.attribute( "type" ) );

    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if (e.tagName() == "resource") {
            // Load the resource
            Resource *child = new Resource();
            if ( load( child, e, status ) ) {
                status.project().addResource( rg, child );
            } else {
                // TODO: Complain about this
                delete child;
            }
        }
    }
    return true;
}

bool KPlatoXmlLoaderBase::load( Resource *resource, const KoXmlElement &element, XMLLoaderObject &status )
{
    kDebug(kplatoXmlDebugArea())<<"resource";
    const KLocale *locale = status.project().locale();
    QString s;
    resource->setId( element.attribute( "id" ) );
    resource->setName( element.attribute( "name" ) );
    resource->setInitials( element.attribute( "initials" ) );
    resource->setEmail( element.attribute( "email" ) );
    resource->setType( element.attribute( "type" ) );
    resource->setCalendar( status.project().findCalendar( element.attribute( "calendar-id" ) ) );
    resource->setUnits( element.attribute( "units", "100" ).toInt() );
    s = element.attribute( "available-from" );
    if ( ! s.isEmpty() ) {
        resource->setAvailableFrom( DateTime::fromString( s, status.projectSpec() ) );
    }
    s = element.attribute( "available-until" );
    if ( ! s.isEmpty() ) {
        resource->setAvailableUntil( DateTime::fromString( s, status.projectSpec() ) );
    }
    resource->setNormalRate( locale->readMoney( element.attribute( "normal-rate" ) ) );
    resource->setOvertimeRate( locale->readMoney( element.attribute( "overtime-rate" ) ) );
    resource->setAccount( status.project().accounts().findAccount( element.attribute( "account" ) ) );

    KoXmlElement e;
    KoXmlElement parent = element.namedItem( "required-resources" ).toElement();
    forEachElement( e, parent ) {
        if (e.nodeName() == "resource") {
            QString id = e.attribute( "id" );
            if ( id.isEmpty() ) {
                kWarning()<<"Missing resource id";
                continue;
            }
            resource->addRequiredId( id );
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
            resource->clearExternalAppointments( id ); // in case...
            AppointmentIntervalList lst;
            load( lst, e, status );
            Appointment *a = new Appointment();
            a->setIntervals( lst );
            a->setAuxcilliaryInfo( e.attribute( "name", "Unknown" ) );
            resource->addExternalAppointment( id, a );
        }
    }
    return true;
}

bool KPlatoXmlLoaderBase::load( Accounts &accounts, const KoXmlElement &element, XMLLoaderObject &status )
{
    kDebug(kplatoXmlDebugArea())<<"accounts";
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if (e.tagName() == "account") {
            Account *child = new Account();
            if ( load( child, e, status ) ) {
                accounts.insert( child );
            } else {
                // TODO: Complain about this
                kWarning()<<"Loading failed";
                delete child;
            }
        }
    }
    if (element.hasAttribute("default-account")) {
        accounts.setDefaultAccount( accounts.findAccount( element.attribute( "default-account" ) ) );
        if ( accounts.defaultAccount() == 0) {
            kWarning()<<"Could not find default account.";
        }
    }
    return true;
}

bool KPlatoXmlLoaderBase::load(Account* account, const KoXmlElement& element, XMLLoaderObject& status)
{
    kDebug(kplatoXmlDebugArea())<<"account";
    account->setName( element.attribute( "name" ) );
    account->setDescription( element.attribute( "description" ) );
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if (e.tagName() == "costplace") {
            Account::CostPlace *child = new Account::CostPlace( account );
            if ( load( child, e, status ) ) {
                account->append( child );
            } else {
                delete child;
            }
        } else if ( e.tagName() == "account" ) {
            Account *child = new Account();
            if ( load( child, e, status ) ) {
                account->insert( child );
            } else {
                // TODO: Complain about this
                kWarning()<<"Loading failed";
                delete child;
            }
        }
    }
    return true;
}

bool KPlatoXmlLoaderBase::load(Account::CostPlace* cp, const KoXmlElement& element, XMLLoaderObject& status)
{
    kDebug(kplatoXmlDebugArea())<<"cost-place";
    cp->setObjectId( element.attribute( "object-id" ) );
    if ( cp->objectId().isEmpty() ) {
        // check old format
        cp->setObjectId( element.attribute( "node-id" ) );
        if ( cp->objectId().isEmpty() ) {
            kError()<<"No object id";
            return false;
        }
    }
    cp->setNode( status.project().findNode( cp->objectId() ) );
    if ( cp->node() == 0) {
        cp->setResource( status.project().findResource( cp->objectId() ) );
        if ( cp->resource() == 0 ) {
            kError()<<"Cannot find object with id: "<<cp->objectId();
            return false;
        }
    }
    bool on = (bool)(element.attribute("running-cost").toInt());
    if ( on ) cp->setRunning( on );
    on = (bool)(element.attribute("startup-cost").toInt());
    if ( on ) cp->setStartup( on );
    on = (bool)(element.attribute("shutdown-cost").toInt());
    if ( on ) cp->setShutdown( on );
    return true;
}

bool KPlatoXmlLoaderBase::load( ScheduleManager *manager, const KoXmlElement &element, XMLLoaderObject &status )
{
    kDebug(kplatoXmlDebugArea())<<"schedule-manager";
    MainSchedule *sch = 0;
    if ( status.version() <= "0.5" ) {
        manager->setUsePert( false );
        MainSchedule *sch = loadMainSchedule( manager, element, status );
        if ( sch && sch->type() == Schedule::Expected ) {
            sch->setManager( manager );
            manager->setExpected( sch );
        } else {
            delete sch;
        }
        return true;
    }
    manager->setName( element.attribute( "name" ) );
    manager->setManagerId( element.attribute( "id" ) );
    manager->setUsePert( element.attribute( "distribution" ).toInt() == 1 );
    manager->setAllowOverbooking( (bool)( element.attribute( "overbooking" ).toInt() ) );
    manager->setCheckExternalAppointments( (bool)( element.attribute( "check-external-appointments" ).toInt() ) );
    manager->setSchedulingDirection( (bool)(element.attribute( "scheduling-direction" ).toInt() ) );
    manager->setBaselined( (bool)( element.attribute( "baselined" ).toInt() ) );
    manager->setSchedulerPluginId( element.attribute( "scheduler-plugin-id" ) );
    manager->setRecalculate( (bool)( element.attribute( "recalculate" ).toInt() ) );
    manager->setRecalculateFrom( DateTime::fromString( element.attribute( "recalculate-from" ), status.projectSpec() ) );
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        //kDebug(kplatoXmlDebugArea())<<e.tagName();
        if ( e.tagName() == "schedule" ) {
            sch = loadMainSchedule( manager, e, status );
            if ( sch && sch->type() == Schedule::Expected ) {
                sch->setManager( manager );
                manager->setExpected( sch ); break;
            } else {
                delete sch;
            }
        } else if ( e.tagName() == "plan" ) {
            ScheduleManager *sm = new ScheduleManager( status.project() );
            if ( load( sm, e, status ) ) {
                status.project().addScheduleManager( sm, manager );
            } else {
                kError()<<"Failed to load schedule manager";
                delete sm;
            }
        }
    }
    return true;
}

bool KPlatoXmlLoaderBase::load( Schedule *schedule, const KoXmlElement& element, XMLLoaderObject& /*status*/ )
{
    kDebug(kplatoXmlDebugArea())<<"schedule";
    schedule->setName( element.attribute( "name" ) );
    schedule->setType( element.attribute( "type" ) );
    schedule->setId( element.attribute( "id" ).toLong() );

    return true;
}

MainSchedule* KPlatoXmlLoaderBase::loadMainSchedule( ScheduleManager* /*manager*/, const KoXmlElement& element, XMLLoaderObject& status )
{
    kDebug(kplatoXmlDebugArea())<<"main-schedule";
    MainSchedule *sch = new MainSchedule();
    if ( loadMainSchedule( sch, element, status ) ) {
        status.project().addSchedule( sch );
        sch->setNode( &(status.project()) );
        status.project().setParentSchedule( sch );
        // If it's here, it's scheduled!
        sch->setScheduled( true );
    } else {
        kError() << "Failed to load schedule";
        delete sch;
        sch = 0;
    }
    return sch;
}

bool KPlatoXmlLoaderBase::loadMainSchedule( MainSchedule *ms, const KoXmlElement &element, XMLLoaderObject &status )
{
    kDebug(kplatoXmlDebugArea());
    QString s;
    load( ms, element, status );

    s = element.attribute( "start" );
    if ( !s.isEmpty() ) {
        ms->startTime = DateTime::fromString( s, status.projectSpec() );
    }
    s = element.attribute( "end" );
    if ( !s.isEmpty() ) {
        ms->endTime = DateTime::fromString( s, status.projectSpec() );
    }
    ms->duration = Duration::fromString( element.attribute( "duration" ) );
    ms->constraintError = element.attribute( "scheduling-conflict", "0" ).toInt();

    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement el = n.toElement();
        if ( el.tagName() == "appointment" ) {
            // Load the appointments.
            // Resources and tasks must already be loaded
            Appointment * child = new Appointment();
            if ( ! load( child, el, status, *ms ) ) {
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
                ms->m_pathlists.append( lst );
            }
            ms->criticalPathListCached = true;
        }
    }
    return true;
}

bool KPlatoXmlLoaderBase::loadNodeSchedule(NodeSchedule* schedule, const KoXmlElement &element, XMLLoaderObject& status)
{
    kDebug(kplatoXmlDebugArea())<<"node-schedule";
    QString s;
    load( schedule, element, status );
    s = element.attribute( "earlystart" );
    if ( s.isEmpty() ) { // try version < 0.6
        s = element.attribute( "earlieststart" );
    }
    if ( ! s.isEmpty() ) {
        schedule->earlyStart = DateTime::fromString( s, status.projectSpec() );
    }
    s = element.attribute( "latefinish" );
    if ( s.isEmpty() ) { // try version < 0.6
        s = element.attribute( "latestfinish" );
    }
    if ( ! s.isEmpty() ) {
        schedule->lateFinish = DateTime::fromString( s, status.projectSpec() );
    }
    s = element.attribute( "latestart" );
    if ( ! s.isEmpty() ) {
        schedule->lateStart = DateTime::fromString( s, status.projectSpec() );
    }
    s = element.attribute( "earlyfinish" );
    if ( ! s.isEmpty() ) {
        schedule->earlyFinish = DateTime::fromString( s, status.projectSpec() );
    }
    s = element.attribute( "start" );
    if ( ! s.isEmpty() )
        schedule->startTime = DateTime::fromString( s, status.projectSpec() );
    s = element.attribute( "end" );
    if ( !s.isEmpty() )
        schedule->endTime = DateTime::fromString( s, status.projectSpec() );
    s = element.attribute( "start-work" );
    if ( !s.isEmpty() )
        schedule->workStartTime = DateTime::fromString( s, status.projectSpec() );
    s = element.attribute( "end-work" );
    if ( !s.isEmpty() )
        schedule->workEndTime = DateTime::fromString( s, status.projectSpec() );
    schedule->duration = Duration::fromString( element.attribute( "duration" ) );

    schedule->inCriticalPath = element.attribute( "in-critical-path", "0" ).toInt();
    schedule->resourceError = element.attribute( "resource-error", "0" ).toInt();
    schedule->resourceOverbooked = element.attribute( "resource-overbooked", "0" ).toInt();
    schedule->resourceNotAvailable = element.attribute( "resource-not-available", "0" ).toInt();
    schedule->constraintError = element.attribute( "scheduling-conflict", "0" ).toInt();
    schedule->notScheduled = element.attribute( "not-scheduled", "1" ).toInt();

    schedule->positiveFloat = Duration::fromString( element.attribute( "positive-float" ) );
    schedule->negativeFloat = Duration::fromString( element.attribute( "negative-float" ) );
    schedule->freeFloat = Duration::fromString( element.attribute( "free-float" ) );

    return true;
}

bool KPlatoXmlLoaderBase::load( WBSDefinition &def, const KoXmlElement &element, XMLLoaderObject &/*status*/ )
{
    kDebug(kplatoXmlDebugArea())<<"wbs-def";
    def.setProjectCode( element.attribute( "project-code" ) );
    def.setProjectSeparator( element.attribute( "project-separator" ) );
    def.setLevelsDefEnabled( (bool)element.attribute( "levels-enabled", "0" ).toInt() );
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if ( e.tagName() == "default" ) {
            def.defaultDef().code = e.attribute( "code", "Number" );
            def.defaultDef().separator = e.attribute( "separator", "." );
        } else if (e.tagName() == "levels") {
            KoXmlNode n = e.firstChild();
            for ( ; ! n.isNull(); n = n.nextSibling() ) {
                if ( ! n.isElement() ) {
                    continue;
                }
                KoXmlElement el = n.toElement();
                WBSDefinition::CodeDef d;
                d.code = el.attribute( "code" );
                d.separator = el.attribute( "separator" );
                int lvl = el.attribute( "level", "-1" ).toInt();
                if ( lvl >= 0 ) {
                    def.setLevelsDef( lvl, d );
                } else kError()<<"Invalid levels definition";
            }
        }
    }
    return true;
}

bool KPlatoXmlLoaderBase::load( Documents &documents, const KoXmlElement &element, XMLLoaderObject &status )
{
    kDebug(kplatoXmlDebugArea())<<"documents";
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if (e.tagName() == "document") {
            Document *doc = new Document();
            if ( ! load( doc, e, status ) ) {
                kWarning()<<"Failed to load document";
                status.addMsg( XMLLoaderObject::Errors, "Failed to load document" );
                delete doc;
            } else {
                documents.addDocument( doc );
                status.addMsg( i18n( "Document loaded, URL=%1",  doc->url().url() ) );
            }
        }
    }
    return true;
}

bool KPlatoXmlLoaderBase::load( Document *document, const KoXmlElement &element, XMLLoaderObject &status )
{
    kDebug(kplatoXmlDebugArea())<<"document";
    Q_UNUSED(status);
    document->setUrl( KUrl( element.attribute( "url" ) ) );
    document->setType( ( Document::Type )( element.attribute( "type" ).toInt() ) );
    document->setStatus( element.attribute( "status" ) );
    document->setSendAs( ( Document::SendAs )( element.attribute( "sendas" ).toInt() ) );
    return true;
}

bool KPlatoXmlLoaderBase::load( Estimate* estimate, const KoXmlElement& element, XMLLoaderObject& status )
{
    kDebug(kplatoXmlDebugArea())<<"estimate";
    estimate->setType( element.attribute( "type") );
    estimate->setRisktype( element.attribute( "risk" ) );
    if ( status.version() <= "0.6" ) {
        estimate->setUnit( ( Duration::Unit )( element.attribute( "display-unit", QString().number( Duration::Unit_h ) ).toInt()) );
        QList<qint64> s = status.project().standardWorktime()->scales();
        estimate->setExpectedEstimate( Estimate::scale( Duration::fromString( element.attribute("expected" ) ), estimate->unit(), s ) );
        estimate->setOptimisticEstimate( Estimate::scale(  Duration::fromString( element.attribute("optimistic" ) ), estimate->unit(), s ) );
        estimate->setPessimisticEstimate( Estimate::scale( Duration::fromString( element.attribute("pessimistic") ), estimate->unit(), s ) );
    } else {
        if ( status.version() <= "0.6.2" ) {
            // 0 pos in unit is now Unit_Y, so add 3 to get the correct new unit
            estimate->setUnit( ( Duration::Unit )( element.attribute( "unit", QString().number( Duration::Unit_ms - 3 ) ).toInt() + 3 ) );
        } else {
            estimate->setUnit( Duration::unitFromString( element.attribute( "unit" ) ) );
        }
        estimate->setExpectedEstimate( element.attribute( "expected", "0.0" ).toDouble() );
        estimate->setOptimisticEstimate( element.attribute( "optimistic", "0.0" ).toDouble() );
        estimate->setPessimisticEstimate( element.attribute( "pessimistic", "0.0" ).toDouble() );

        estimate->setCalendar( status.project().findCalendar( element.attribute("calendar-id" ) ) );
    }
    return true;
}

bool KPlatoXmlLoaderBase::load( ResourceGroupRequest* gr, const KoXmlElement& element, XMLLoaderObject& status )
{
    kDebug(kplatoXmlDebugArea())<<"resourcegroup-request";
    gr->setGroup( status.project().findResourceGroup(element.attribute("group-id")) );
    if ( gr->group() == 0 ) {
        kError()<<"The referenced resource group does not exist: group id="<<element.attribute("group-id");
        return false;
    }
    gr->group()->registerRequest( gr );


    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if (e.tagName() == "resource-request") {
            ResourceRequest *r = new ResourceRequest();
            if ( load( r, e, status ) ) {
                gr->addResourceRequest( r );
            } else {
                kError()<<"Failed to load resource request";
                delete r;
            }
        }
    }
    // meaning of m_units changed
    int x = element.attribute("units").toInt() -gr->count();
    gr->setUnits( x > 0 ? x : 0 );

    return true;
}

bool KPlatoXmlLoaderBase::load( ResourceRequest *rr, const KoXmlElement& element, XMLLoaderObject& status)
{
    kDebug(kplatoXmlDebugArea())<<"resource-request";
    rr->setResource( status.project().resource(element.attribute("resource-id")) );
    if (rr->resource() == 0) {
        kWarning()<<"The referenced resource does not exist: resource id="<<element.attribute("resource-id");
        return false;
    }
    rr->setUnits( element.attribute("units").toInt() );

    KoXmlElement parent = element.namedItem( "required-resources" ).toElement();
    KoXmlElement e;
    QList<Resource*> required;
    forEachElement( e, parent ) {
        if (e.nodeName() == "resource") {
            QString id = e.attribute( "id" );
            if ( id.isEmpty() ) {
                kError()<<"Missing project id";
                continue;
            }
            Resource *r = status.project().resource(id);
            if (r == 0) {
                kWarning()<<"The referenced resource does not exist: resource id="<<element.attribute("resource-id");
            } else {
                if ( r != rr->resource() ) {
                    required << r;
                }
            }
        }
    }
    rr->setRequiredResources( required );
    return true;

}

bool KPlatoXmlLoaderBase::load( WorkPackage &wp, const KoXmlElement& element, XMLLoaderObject& status )
{
    kDebug(kplatoXmlDebugArea())<<"workpackage";
    Q_UNUSED(status);
    wp.setOwnerName( element.attribute( "owner" ) );
    wp.setOwnerId( element.attribute( "owner-id" ) );
    return true;
}

bool KPlatoXmlLoaderBase::loadWpLog( WorkPackage *wp, KoXmlElement& element, XMLLoaderObject status )
{
    kDebug(kplatoXmlDebugArea())<<"wplog";
    wp->setOwnerName( element.attribute( "owner" ) );
    wp->setOwnerName( element.attribute( "owner-id" ) );
    wp->setTransmitionStatus( wp->transmitionStatusFromString( element.attribute( "status" ) ) );
    wp->setTransmitionTime( DateTime( KDateTime::fromString( element.attribute( "time" ) ) ) );
    return load( wp->completion(), element, status );
}

bool KPlatoXmlLoaderBase::load( Completion &completion, const KoXmlElement& element, XMLLoaderObject& status )
{
    kDebug(kplatoXmlDebugArea())<<"completion";
    QString s;
    completion.setStarted( (bool)element.attribute("started", "0").toInt() );
    completion.setFinished( (bool)element.attribute("finished", "0").toInt() );
    s = element.attribute("startTime");
    if (!s.isEmpty()) {
        completion.setStartTime( DateTime::fromString(s, status.projectSpec()) );
    }
    s = element.attribute("finishTime");
    if (!s.isEmpty()) {
        completion.setFinishTime( DateTime::fromString(s, status.projectSpec() ) );
    }
    completion.setEntrymode( element.attribute( "entrymode" ) );
    if (status.version() < "0.6") {
        if ( completion.isStarted() ) {
            Completion::Entry *entry = new Completion::Entry( element.attribute("percent-finished", "0").toInt(), Duration::fromString(element.attribute("remaining-effort")),  Duration::fromString(element.attribute("performed-effort")) );
            entry->note = element.attribute("note");
            QDate date = completion.startTime().date();
            if ( completion.isFinished() ) {
                date = completion.finishTime().date();
            }
            // almost the best we can do ;)
            completion.addEntry( date, entry );
        }
    } else {
        KoXmlElement e;
        forEachElement(e, element) {
                if (e.tagName() == "completion-entry") {
                    QDate date;
                    s = e.attribute("date");
                    if ( !s.isEmpty() ) {
                        date = QDate::fromString( s, Qt::ISODate );
                    }
                    if ( !date.isValid() ) {
                        kWarning()<<"Invalid date: "<<date<<s;
                        continue;
                    }
                    Completion::Entry *entry = new Completion::Entry( e.attribute("percent-finished", "0").toInt(), Duration::fromString(e.attribute("remaining-effort")),  Duration::fromString(e.attribute("performed-effort")) );
                    completion.addEntry( date, entry );
                } else if (e.tagName() == "used-effort") {
                    KoXmlElement el;
                    forEachElement(el, e) {
                            if (el.tagName() == "resource") {
                                QString id = el.attribute( "id" );
                                Resource *r = status.project().resource( id );
                                if ( r == 0 ) {
                                    kWarning()<<"Cannot find resource, id="<<id;
                                    continue;
                                }
                                Completion::UsedEffort *ue = new Completion::UsedEffort();
                                completion.addUsedEffort( r, ue );
                                load( ue, el, status );
                            }
                    }
                }
        }
    }
    return true;
}

bool KPlatoXmlLoaderBase::load( Completion::UsedEffort* ue, const KoXmlElement& element, XMLLoaderObject& /*status*/)
{
    kDebug(kplatoXmlDebugArea())<<"used-effort";
    KoXmlElement e;
    forEachElement(e, element) {
        if (e.tagName() == "actual-effort") {
            QDate date = QDate::fromString( e.attribute("date"), Qt::ISODate );
            if ( date.isValid() ) {
                Completion::UsedEffort::ActualEffort a;
                a.setNormalEffort( Duration::fromString( e.attribute( "normal-effort" ) ) );
                a.setOvertimeEffort( Duration::fromString( e.attribute( "overtime-effort" ) ) );
                ue->setEffort( date, a );
            }
        }
    }
    return true;
}

bool KPlatoXmlLoaderBase::load( Appointment *appointment, const KoXmlElement& element, XMLLoaderObject& status, Schedule &sch )
{
    kDebug(kplatoXmlDebugArea())<<"appointment";
    Node *node = status.project().findNode(element.attribute("task-id"));
    if (node == 0) {
        kError()<<"The referenced task does not exists: "<<element.attribute("task-id");
        return false;
    }
    Resource *res = status.project().resource(element.attribute("resource-id"));
    if (res == 0) {
        kError()<<"The referenced resource does not exists: resource id="<<element.attribute("resource-id");
        return false;
    }
    if (!res->addAppointment( appointment, sch ) ) {
        kError()<<"Failed to add appointment to resource: "<<res->name();
        return false;
    }
    if ( ! node->addAppointment( appointment, sch ) ) {
        kError()<<"Failed to add appointment to node: "<<node->name();
        appointment->resource()->takeAppointment( appointment );
        return false;
    }
    //kDebug(kplatoXmlDebugArea())<<"res="<<m_resource<<" node="<<m_node;
    AppointmentIntervalList lst = appointment->intervals();
    load( lst, element, status );
    if ( lst.isEmpty() ) {
        kError()<<"Appointment interval list is empty (added anyway): "<<node->name()<<res->name();
        return false;
    }
    appointment->setIntervals( lst );
    return true;
}

bool KPlatoXmlLoaderBase::load(AppointmentIntervalList& lst, const KoXmlElement& element, XMLLoaderObject& status)
{
    kDebug(kplatoXmlDebugArea())<<"appointment-interval-list";
    KoXmlElement e;
    forEachElement(e, element) {
        if (e.tagName() == "interval") {
            AppointmentInterval a;
            if ( load( a, e, status ) ) {
                lst.add(a);
            } else {
                kError()<<"Could not load interval";
            }
        }
    }
    return true;
}

bool KPlatoXmlLoaderBase::load(AppointmentInterval& interval, const KoXmlElement& element, XMLLoaderObject& status)
{
    bool ok;
    QString s = element.attribute("start");
    if (!s.isEmpty()) {
        interval.setStartTime( DateTime::fromString(s, status.projectSpec()) );
    }
    s = element.attribute("end");
    if (!s.isEmpty()) {
        interval.setEndTime( DateTime::fromString(s, status.projectSpec()) );
    }
    double l = element.attribute("load", "100").toDouble(&ok);
    if (ok) {
        interval.setLoad( l );
    }
    kDebug(kplatoXmlDebugArea())<<"interval:"<<interval;
    return interval.isValid();
}

#include "KPlatoXmlLoaderBase.moc"
