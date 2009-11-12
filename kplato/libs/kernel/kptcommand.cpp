/* This file is part of the KDE project
  Copyright (C) 2004 - 2007 Dag Andersen <danders@get2net.dk>

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

#include "kptcommand.h"
#include "kptaccount.h"
#include "kptappointment.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptcalendar.h"
#include "kptrelation.h"
#include "kptresource.h"
#include "kptdocuments.h"

#include <kdebug.h>
#include <klocale.h>

#include <QHash>
#include <QMap>
#include <QApplication>

namespace KPlato
{

void NamedCommand::setSchDeleted()
{
    QMap<Schedule*, bool>::Iterator it;
    for ( it = m_schedules.begin(); it != m_schedules.end(); ++it ) {
        //kDebug() << it.key() ->id() <<":" << it.value();
        it.key() ->setDeleted( it.value() );
    }
}
void NamedCommand::setSchDeleted( bool state )
{
    QMap<Schedule*, bool>::Iterator it;
    for ( it = m_schedules.begin(); it != m_schedules.end(); ++it ) {
        //kDebug() << it.key() ->id() <<":" << state;
        it.key() ->setDeleted( state );
    }
}
void NamedCommand::setSchScheduled()
{
    QMap<Schedule*, bool>::Iterator it;
    for ( it = m_schedules.begin(); it != m_schedules.end(); ++it ) {
        //kDebug() << it.key() ->name() <<":" << it.value();
        it.key() ->setScheduled( it.value() );
    }
}
void NamedCommand::setSchScheduled( bool state )
{
    QMap<Schedule*, bool>::Iterator it;
    for ( it = m_schedules.begin(); it != m_schedules.end(); ++it ) {
        //kDebug() << it.key() ->name() <<":" << state;
        it.key() ->setScheduled( state );
    }
}
void NamedCommand::addSchScheduled( Schedule *sch )
{
    //kDebug() << sch->id() <<":" << sch->isScheduled();
    m_schedules.insert( sch, sch->isScheduled() );
    foreach ( Appointment * a, sch->appointments() ) {
        if ( a->node() == sch ) {
            m_schedules.insert( a->resource(), a->resource() ->isScheduled() );
        } else if ( a->resource() == sch ) {
            m_schedules.insert( a->node(), a->node() ->isScheduled() );
        }
    }
}
void NamedCommand::addSchDeleted( Schedule *sch )
{
    //kDebug() << sch->id() <<":" << sch->isDeleted();
    m_schedules.insert( sch, sch->isDeleted() );
    foreach ( Appointment * a, sch->appointments() ) {
        if ( a->node() == sch ) {
            m_schedules.insert( a->resource(), a->resource() ->isDeleted() );
        } else if ( a->resource() == sch ) {
            m_schedules.insert( a->node(), a->node() ->isDeleted() );
        }
    }
}

//---------
MacroCommand::~MacroCommand()
{
    while ( ! cmds.isEmpty() ) {
        delete cmds.takeLast();
    }
}

void MacroCommand::addCommand( QUndoCommand *cmd )
{
    cmds.append( cmd );
}

void MacroCommand::execute()
{
    foreach ( QUndoCommand *c, cmds ) {
        c->redo();
    }
}

void MacroCommand::unexecute()
{
    for (int i = cmds.count() - 1; i >= 0; --i) {
        cmds.at( i )->undo();
    }
}

//-------------------------------------------------
CalendarAddCmd::CalendarAddCmd( Project *project, Calendar *cal, Calendar *parent, const QString& name )
        : NamedCommand( name ),
        m_project( project ),
        m_cal( cal ),
        m_parent( parent ),
        m_mine( true )
{
    //kDebug()<<cal->name();
    Q_ASSERT( project != 0 );
}
CalendarAddCmd::~CalendarAddCmd()
{
    if ( m_mine )
        delete m_cal;
}
void CalendarAddCmd::execute()
{
    if ( m_project ) {
        m_project->addCalendar( m_cal, m_parent );
        m_mine = false;
    }

    //kDebug()<<m_cal->name()<<" added to:"<<m_project->name();
}

void CalendarAddCmd::unexecute()
{
    if ( m_project ) {
        m_project->takeCalendar( m_cal );
        m_mine = true;
    }

    //kDebug()<<m_cal->name();
}

CalendarRemoveCmd::CalendarRemoveCmd( Project *project, Calendar *cal, const QString& name )
        : NamedCommand( name ),
        m_project( project ),
        m_parent( cal->parentCal() ),
        m_cal( cal ),
        m_mine( false ),
        m_cmd( new MacroCommand("") )
{
    Q_ASSERT( project != 0 );

/*    foreach ( Schedule * s, project->schedules() ) {
        addSchScheduled( s );
    }*/
    foreach ( Resource *r, project->resourceList() ) {
        if ( r->calendar( true ) == cal ) {
            m_cmd->addCommand( new ModifyResourceCalendarCmd( r, 0 ) );
            break;
        }
    }
    if ( project->defaultCalendar() == cal ) {
        m_cmd->addCommand( new ProjectModifyDefaultCalendarCmd( project, 0 ) );
    }
    foreach ( Calendar *c, cal->calendars() ) {
        m_cmd->addCommand( new CalendarRemoveCmd( project, c ) );
    }
}
CalendarRemoveCmd::~CalendarRemoveCmd()
{
    delete m_cmd;
    if ( m_mine )
        delete m_cal;
}
void CalendarRemoveCmd::execute()
{
//    setSchScheduled( false );
    m_cmd->execute();
    m_project->takeCalendar( m_cal );
    m_mine = true;

}
void CalendarRemoveCmd::unexecute()
{
    m_project->addCalendar( m_cal, m_parent );
    m_cmd->unexecute();
//    setSchScheduled();
    m_mine = false;

}

CalendarModifyNameCmd::CalendarModifyNameCmd( Calendar *cal, const QString& newvalue, const QString& name )
        : NamedCommand( name ),
        m_cal( cal )
{

    m_oldvalue = cal->name();
    m_newvalue = newvalue;
    //kDebug()<<cal->name();
}
void CalendarModifyNameCmd::execute()
{
    m_cal->setName( m_newvalue );

    //kDebug()<<m_cal->name();
}
void CalendarModifyNameCmd::unexecute()
{
    m_cal->setName( m_oldvalue );

    //kDebug()<<m_cal->name();
}

CalendarModifyParentCmd::CalendarModifyParentCmd( Project *project, Calendar *cal, Calendar *newvalue, const QString& name )
        : NamedCommand( name ),
        m_project( project ),
        m_cal( cal ),
        m_cmd( new MacroCommand( "" ) )
{
    m_oldvalue = cal->parentCal();
    m_newvalue = newvalue;
    if ( newvalue ) {
        m_cmd->addCommand( new CalendarModifyTimeZoneCmd( cal, newvalue->timeZone() ) );
    }
    //kDebug()<<cal->name();
}
CalendarModifyParentCmd::~CalendarModifyParentCmd()
{
    delete m_cmd;
}
void CalendarModifyParentCmd::execute()
{
    m_project->takeCalendar( m_cal );
    m_project->addCalendar( m_cal, m_newvalue );
    m_cmd->execute();
//    setSchScheduled( false );

}
void CalendarModifyParentCmd::unexecute()
{
    m_cmd->unexecute();
    m_project->takeCalendar( m_cal );
    m_project->addCalendar( m_cal, m_oldvalue );
//    setSchScheduled();

}

CalendarModifyTimeZoneCmd::CalendarModifyTimeZoneCmd( Calendar *cal, const KTimeZone &value, const QString& name )
        : NamedCommand( name ),
        m_cal( cal ),
        m_newvalue( value ),
        m_cmd( new MacroCommand( "" ) )
{
    m_oldvalue = cal->timeZone();
    foreach ( Calendar *c, cal->calendars() ) {
        m_cmd->addCommand( new CalendarModifyTimeZoneCmd( c, value ) );
    }
    //kDebug()<<cal->name();
}
CalendarModifyTimeZoneCmd::~CalendarModifyTimeZoneCmd()
{
    delete m_cmd;
}
void CalendarModifyTimeZoneCmd::execute()
{
    m_cmd->execute();
    m_cal->setTimeZone( m_newvalue );
//    setSchScheduled( false );

}
void CalendarModifyTimeZoneCmd::unexecute()
{
    m_cal->setTimeZone( m_oldvalue );
    m_cmd->unexecute();
//    setSchScheduled();

}

CalendarAddDayCmd::CalendarAddDayCmd( Calendar *cal, CalendarDay *newvalue, const QString& name )
        : NamedCommand( name ),
        m_cal( cal ),
        m_mine( true )
{

    m_newvalue = newvalue;
    //kDebug()<<cal->name();
}
CalendarAddDayCmd::~CalendarAddDayCmd()
{
    //kDebug();
    if ( m_mine )
        delete m_newvalue;
}
void CalendarAddDayCmd::execute()
{
    //kDebug()<<m_cal->name();
    m_cal->addDay( m_newvalue );
    m_mine = false;
//    setSchScheduled( false );

}
void CalendarAddDayCmd::unexecute()
{
    //kDebug()<<m_cal->name();
    m_cal->takeDay( m_newvalue );
    m_mine = true;
//    setSchScheduled();

}

CalendarRemoveDayCmd::CalendarRemoveDayCmd( Calendar *cal,CalendarDay *day, const QString& name )
        : NamedCommand( name ),
        m_cal( cal ),
        m_value( day ),
        m_mine( false )
{
    //kDebug()<<cal->name();
    // TODO check if any resources uses this calendar
    init();
}
CalendarRemoveDayCmd::CalendarRemoveDayCmd( Calendar *cal, const QDate &day, const QString& name )
        : NamedCommand( name ),
        m_cal( cal ),
        m_mine( false )
{

    m_value = cal->findDay( day );
    //kDebug()<<cal->name();
    // TODO check if any resources uses this calendar
    init();
}
void CalendarRemoveDayCmd::init()
{
}
void CalendarRemoveDayCmd::execute()
{
    //kDebug()<<m_cal->name();
    m_cal->takeDay( m_value );
    m_mine = true;
//    setSchScheduled( false );

}
void CalendarRemoveDayCmd::unexecute()
{
    //kDebug()<<m_cal->name();
    m_cal->addDay( m_value );
    m_mine = false;
//    setSchScheduled();

}

CalendarModifyDayCmd::CalendarModifyDayCmd( Calendar *cal, CalendarDay *value, const QString& name )
        : NamedCommand( name ),
        m_cal( cal ),
        m_mine( true )
{

    m_newvalue = value;
    m_oldvalue = cal->findDay( value->date() );
    //kDebug()<<cal->name()<<" old:("<<m_oldvalue<<") new:("<<m_newvalue<<")";
}
CalendarModifyDayCmd::~CalendarModifyDayCmd()
{
    //kDebug();
    if ( m_mine ) {
        delete m_newvalue;
    } else {
        delete m_oldvalue;
    }
}
void CalendarModifyDayCmd::execute()
{
    //kDebug();
    m_cal->takeDay( m_oldvalue );
    m_cal->addDay( m_newvalue );
    m_mine = false;
//    setSchScheduled( false );

}
void CalendarModifyDayCmd::unexecute()
{
    //kDebug();
    m_cal->takeDay( m_newvalue );
    m_cal->addDay( m_oldvalue );
    m_mine = true;
//    setSchScheduled();

}

CalendarModifyStateCmd::CalendarModifyStateCmd( Calendar *calendar, CalendarDay *day, CalendarDay::State value, const QString& name )
        : NamedCommand( name ),
        m_calendar( calendar ),
        m_day( day ),
        m_cmd( new MacroCommand( "" ) )
{

    m_newvalue = value;
    m_oldvalue = (CalendarDay::State)day->state();
    if ( value != CalendarDay::Working ) {
        foreach ( TimeInterval *ti, day->workingIntervals() ) {
            m_cmd->addCommand( new CalendarRemoveTimeIntervalCmd( calendar, day, ti ) );
        }
    }
}
CalendarModifyStateCmd::~CalendarModifyStateCmd()
{
    delete m_cmd;
}
void CalendarModifyStateCmd::execute()
{
    //kDebug();
    m_cmd->execute();
    m_calendar->setState( m_day, m_newvalue );

}
void CalendarModifyStateCmd::unexecute()
{
    //kDebug();
    m_calendar->setState( m_day, m_oldvalue );
    m_cmd->unexecute();

}

CalendarModifyTimeIntervalCmd::CalendarModifyTimeIntervalCmd( Calendar *calendar, TimeInterval &newvalue, TimeInterval *value, const QString& name )
        : NamedCommand( name ),
        m_calendar( calendar )
{

    m_value = value; // keep pointer
    m_oldvalue = *value; // save value
    m_newvalue = newvalue;
}
void CalendarModifyTimeIntervalCmd::execute()
{
    //kDebug();
    m_calendar->setWorkInterval( m_value, m_newvalue );

}
void CalendarModifyTimeIntervalCmd::unexecute()
{
    //kDebug();
    m_calendar->setWorkInterval( m_value, m_oldvalue );

}

CalendarAddTimeIntervalCmd::CalendarAddTimeIntervalCmd( Calendar *calendar, CalendarDay *day, TimeInterval *value, const QString& name )
    : NamedCommand( name ),
    m_calendar( calendar ),
    m_day( day ),
    m_value( value ),
    m_mine( true )
{
}
CalendarAddTimeIntervalCmd::~CalendarAddTimeIntervalCmd()
{
    if ( m_mine )
        delete m_value;
}
void CalendarAddTimeIntervalCmd::execute()
{
    //kDebug();
    m_calendar->addWorkInterval( m_day, m_value );
    m_mine = false;

}
void CalendarAddTimeIntervalCmd::unexecute()
{
    //kDebug();
    m_calendar->takeWorkInterval( m_day, m_value );
    m_mine = true;

}

CalendarRemoveTimeIntervalCmd::CalendarRemoveTimeIntervalCmd( Calendar *calendar, CalendarDay *day, TimeInterval *value, const QString& name )
    : CalendarAddTimeIntervalCmd( calendar, day, value, name )
{
    m_mine = false ;
}
void CalendarRemoveTimeIntervalCmd::execute()
{
    CalendarAddTimeIntervalCmd::unexecute();
}
void CalendarRemoveTimeIntervalCmd::unexecute()
{
    CalendarAddTimeIntervalCmd::execute();
}

CalendarModifyWeekdayCmd::CalendarModifyWeekdayCmd( Calendar *cal, int weekday, CalendarDay *value, const QString& name )
        : NamedCommand( name ),
        m_weekday( weekday ),
        m_cal( cal ),
        m_value( value ),
        m_orig( *( cal->weekday( weekday ) ) )
{

    //kDebug() << cal->name() <<" (" << value <<")";
}
CalendarModifyWeekdayCmd::~CalendarModifyWeekdayCmd()
{
    //kDebug() << m_weekday <<":" << m_value;
    delete m_value;

}
void CalendarModifyWeekdayCmd::execute()
{
    m_cal->setWeekday( m_weekday, *m_value );
//    setSchScheduled( false );

}
void CalendarModifyWeekdayCmd::unexecute()
{
    m_cal->setWeekday( m_weekday, m_orig );
//    setSchScheduled();

}

CalendarModifyDateCmd::CalendarModifyDateCmd( Calendar *cal, CalendarDay *day, QDate &value, const QString& name )
    : NamedCommand( name ),
    m_cal( cal ),
    m_day( day ),
    m_newvalue( value ),
    m_oldvalue( day->date() )
{
    //kDebug() << cal->name() <<" (" << value <<")";
}
void CalendarModifyDateCmd::execute()
{
    m_cal->setDate( m_day, m_newvalue );
//    setSchScheduled( false );

}
void CalendarModifyDateCmd::unexecute()
{
    m_cal->setDate( m_day, m_oldvalue );
//    setSchScheduled();

}

ProjectModifyDefaultCalendarCmd::ProjectModifyDefaultCalendarCmd( Project *project, Calendar *cal, const QString& name )
    : NamedCommand( name ),
    m_project( project ),
    m_newvalue( cal ),
    m_oldvalue( project->defaultCalendar() )
{
    //kDebug() << cal->name() <<" (" << value <<")";
}
void ProjectModifyDefaultCalendarCmd::execute()
{
    m_project->setDefaultCalendar( m_newvalue );

}
void ProjectModifyDefaultCalendarCmd::unexecute()
{
    m_project->setDefaultCalendar( m_oldvalue );

}

NodeDeleteCmd::NodeDeleteCmd( Node *node, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        m_index( -1 )
{

    m_parent = node->parentNode();
    m_mine = false;
    
    m_project = static_cast<Project*>( node->projectNode() );
    if ( m_project ) {
        foreach ( Schedule * s, m_project->schedules() ) {
            if ( s && s->isScheduled() ) {
                // Only invalidate schedules this node is part of
                Schedule *ns = node->findSchedule( s->id() );
                if ( ns && ! ns->isDeleted() ) {
                    addSchScheduled( s );
                }
            }
        }
    }
    m_cmd = new MacroCommand( "" );
    foreach ( Relation * r, node->dependChildNodes() ) {
        m_cmd->addCommand( new DeleteRelationCmd( *m_project, r ) );
    }
    foreach ( Relation * r, node->dependParentNodes() ) {
        m_cmd->addCommand( new DeleteRelationCmd( *m_project, r ) );
    }
    QList<Node*> lst = node->childNodeIterator();
    for ( int i = lst.count(); i > 0; --i ) {
        m_cmd->addCommand( new NodeDeleteCmd( lst[ i - 1 ] ) );
    }
    if ( node->runningAccount() ) {
        m_cmd->addCommand( new NodeModifyRunningAccountCmd( *node, node->runningAccount(), 0 ) );
    }
    if ( node->startupAccount() ) {
        m_cmd->addCommand( new NodeModifyRunningAccountCmd( *node, node->startupAccount(), 0 ) );
    }
    if ( node->shutdownAccount() ) {
        m_cmd->addCommand( new NodeModifyRunningAccountCmd( *node, node->shutdownAccount(), 0 ) );
    }

}
NodeDeleteCmd::~NodeDeleteCmd()
{
    if ( m_mine )
        delete m_node;
    delete m_cmd;
    while ( !m_appointments.isEmpty() )
        delete m_appointments.takeFirst();
}
void NodeDeleteCmd::execute()
{
    if ( m_parent && m_project ) {
        m_index = m_parent->findChildNode( m_node );
        //kDebug()<<m_node->name()<<""<<m_index;
/*        foreach ( Appointment * a, m_node->appointments() ) {
            a->detach();
            m_appointments.append( a );
        }*/
        if ( m_cmd ) {
            m_cmd->execute();
        }
        m_project->takeTask( m_node );
        m_mine = true;
        setSchScheduled( false );
    
    }
}
void NodeDeleteCmd::unexecute()
{
    if ( m_parent && m_project ) {
        //kDebug()<<m_node->name()<<""<<m_index;
        m_project->addSubTask( m_node, m_index, m_parent );
        if ( m_cmd ) {
            m_cmd->unexecute();
        }
/*        while ( !m_appointments.isEmpty() ) {
            m_appointments.takeFirst() ->attach();
        }*/
        m_mine = false;
        setSchScheduled();
    
    }
}

TaskAddCmd::TaskAddCmd( Project *project, Node *node, Node *after, const QString& name )
        : NamedCommand( name ),
        m_project( project ),
        m_node( node ),
        m_after( after ),
        m_added( false )
{

    // set some reasonable defaults for normally calculated values
    if ( after && after->parentNode() && after->parentNode() != project ) {
        node->setStartTime( after->parentNode() ->startTime() );
        node->setEndTime( node->startTime() + node->duration() );
    } else {
        if ( project->constraint() == Node::MustFinishOn ) {
            node->setEndTime( project->endTime() );
            node->setStartTime( node->endTime() - node->duration() );
        } else {
            node->setStartTime( project->startTime() );
            node->setEndTime( node->startTime() + node->duration() );
        }
    }
    node->setEarlyStart( node->startTime() );
    node->setLateFinish( node->endTime() );
    node->setWorkStartTime( node->startTime() );
    node->setWorkEndTime( node->endTime() );
}
TaskAddCmd::~TaskAddCmd()
{
    if ( !m_added )
        delete m_node;
}
void TaskAddCmd::execute()
{
    //kDebug()<<m_node->name();
    m_project->addTask( m_node, m_after );
    m_added = true;


}
void TaskAddCmd::unexecute()
{
    m_project->takeTask( m_node );
    m_added = false;


}

SubtaskAddCmd::SubtaskAddCmd( Project *project, Node *node, Node *parent, const QString& name )
        : NamedCommand( name ),
        m_project( project ),
        m_node( node ),
        m_parent( parent ),
        m_added( false ),
        m_cmd( 0 )
{

    // set some reasonable defaults for normally calculated values
    node->setStartTime( parent->startTime() );
    node->setEndTime( node->startTime() + node->duration() );
    node->setEarlyStart( node->startTime() );
    node->setLateFinish( node->endTime() );
    node->setWorkStartTime( node->startTime() );
    node->setWorkEndTime( node->endTime() );
    
    // Summarytasks can't have resources, so remove resource requests from the new parent
    foreach ( ResourceGroupRequest *r, parent->requests().requests() ) {
        if ( m_cmd == 0 ) m_cmd = new MacroCommand( "" );
        m_cmd->addCommand( new RemoveResourceGroupRequestCmd( r ) );
    }
}
SubtaskAddCmd::~SubtaskAddCmd()
{
    delete m_cmd;
    if ( !m_added )
        delete m_node;
}
void SubtaskAddCmd::execute()
{
    m_project->addSubTask( m_node, m_parent );
    if ( m_cmd ) {
        m_cmd->execute();
    }
    m_added = true;


}
void SubtaskAddCmd::unexecute()
{
    m_project->takeTask( m_node );
    if ( m_cmd ) {
        m_cmd->unexecute();
    }
    m_added = false;


}

NodeModifyNameCmd::NodeModifyNameCmd( Node &node, const QString& nodename, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        newName( nodename ),
        oldName( node.name() )
{
}
void NodeModifyNameCmd::execute()
{
    m_node.setName( newName );


}
void NodeModifyNameCmd::unexecute()
{
    m_node.setName( oldName );


}

NodeModifyLeaderCmd::NodeModifyLeaderCmd( Node &node, const QString& leader, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        newLeader( leader ),
        oldLeader( node.leader() )
{
}
void NodeModifyLeaderCmd::execute()
{
    m_node.setLeader( newLeader );


}
void NodeModifyLeaderCmd::unexecute()
{
    m_node.setLeader( oldLeader );


}

NodeModifyDescriptionCmd::NodeModifyDescriptionCmd( Node &node, const QString& description, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        newDescription( description ),
        oldDescription( node.description() )
{
}
void NodeModifyDescriptionCmd::execute()
{
    m_node.setDescription( newDescription );


}
void NodeModifyDescriptionCmd::unexecute()
{
    m_node.setDescription( oldDescription );


}

NodeModifyConstraintCmd::NodeModifyConstraintCmd( Node &node, Node::ConstraintType c, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        newConstraint( c ),
        oldConstraint( static_cast<Node::ConstraintType>( node.constraint() ) )
{

/*    foreach ( Schedule * s, node.schedules() ) {
        addSchScheduled( s );
    }*/
}
void NodeModifyConstraintCmd::execute()
{
    m_node.setConstraint( newConstraint );
//    setSchScheduled( false );

}
void NodeModifyConstraintCmd::unexecute()
{
    m_node.setConstraint( oldConstraint );
//    setSchScheduled();

}

NodeModifyConstraintStartTimeCmd::NodeModifyConstraintStartTimeCmd( Node &node, const QDateTime& dt, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        newTime( dt ),
        oldTime( node.constraintStartTime() )
{
    if ( node.projectNode() ) {
        m_spec = static_cast<Project*>( node.projectNode() )->timeSpec();
    }
}
void NodeModifyConstraintStartTimeCmd::execute()
{
    m_node.setConstraintStartTime( DateTime( newTime, m_spec ) );

}
void NodeModifyConstraintStartTimeCmd::unexecute()
{
    m_node.setConstraintStartTime( oldTime );

}

NodeModifyConstraintEndTimeCmd::NodeModifyConstraintEndTimeCmd( Node &node, const QDateTime& dt, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        newTime( dt ),
        oldTime( node.constraintEndTime() )
{
    if ( node.projectNode() ) {
        m_spec = static_cast<Project*>( node.projectNode() )->timeSpec();
    }
}
void NodeModifyConstraintEndTimeCmd::execute()
{
    m_node.setConstraintEndTime( DateTime( newTime, m_spec ) );
//    setSchScheduled( false );

}
void NodeModifyConstraintEndTimeCmd::unexecute()
{
    m_node.setConstraintEndTime( oldTime );
//    setSchScheduled();

}

NodeModifyStartTimeCmd::NodeModifyStartTimeCmd( Node &node, const QDateTime& dt, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        newTime( dt ),
        oldTime( node.startTime() )
{
    m_spec = static_cast<Project*>( node.projectNode() )->timeSpec();
}
void NodeModifyStartTimeCmd::execute()
{
    m_node.setStartTime( DateTime( newTime, m_spec ) );


}
void NodeModifyStartTimeCmd::unexecute()
{
    m_node.setStartTime( oldTime );


}

NodeModifyEndTimeCmd::NodeModifyEndTimeCmd( Node &node, const QDateTime& dt, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        newTime( dt ),
        oldTime( node.endTime() )
{
    m_spec = static_cast<Project*>( node.projectNode() )->timeSpec();
}
void NodeModifyEndTimeCmd::execute()
{
    m_node.setEndTime( DateTime( newTime, m_spec ) );


}
void NodeModifyEndTimeCmd::unexecute()
{
    m_node.setEndTime( oldTime );


}

NodeModifyIdCmd::NodeModifyIdCmd( Node &node, const QString& id, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        newId( id ),
        oldId( node.id() )
{
}
void NodeModifyIdCmd::execute()
{
    m_node.setId( newId );


}
void NodeModifyIdCmd::unexecute()
{
    m_node.setId( oldId );


}

NodeIndentCmd::NodeIndentCmd( Node &node, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        m_newparent( 0 ),
        m_newindex( -1 ),
        m_cmd( 0 )
{
}
NodeIndentCmd::~NodeIndentCmd()
{
    delete m_cmd;
}
void NodeIndentCmd::execute()
{
    m_oldparent = m_node.parentNode();
    m_oldindex = m_oldparent->findChildNode( &m_node );
    Project *p = dynamic_cast<Project *>( m_node.projectNode() );
    if ( p && p->indentTask( &m_node, m_newindex ) ) {
        m_newparent = m_node.parentNode();
        m_newindex = m_newparent->findChildNode( &m_node );
        // Summarytasks can't have resources, so remove resource requests from the new parent
        if ( m_cmd == 0 ) {
            foreach ( ResourceGroupRequest *r, m_newparent->requests().requests() ) {
                if ( m_cmd == 0 ) m_cmd = new MacroCommand( "" );
                m_cmd->addCommand( new RemoveResourceGroupRequestCmd( r ) );
            }
        }
        if ( m_cmd ) {
            m_cmd->execute();
        }
    }


}
void NodeIndentCmd::unexecute()
{
    Project * p = dynamic_cast<Project *>( m_node.projectNode() );
    if ( m_newindex != -1 && p && p->unindentTask( &m_node ) ) {
        m_newindex = -1;
        if ( m_cmd ) {
            m_cmd->unexecute();
        }
    }


}

NodeUnindentCmd::NodeUnindentCmd( Node &node, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        m_newparent( 0 ),
        m_newindex( -1 )
{}
void NodeUnindentCmd::execute()
{
    m_oldparent = m_node.parentNode();
    m_oldindex = m_oldparent->findChildNode( &m_node );
    Project *p = dynamic_cast<Project *>( m_node.projectNode() );
    if ( p && p->unindentTask( &m_node ) ) {
        m_newparent = m_node.parentNode();
        m_newindex = m_newparent->findChildNode( &m_node );
    }


}
void NodeUnindentCmd::unexecute()
{
    Project * p = dynamic_cast<Project *>( m_node.projectNode() );
    if ( m_newindex != -1 && p && p->indentTask( &m_node, m_oldindex ) ) {
        m_newindex = -1;
    }


}

NodeMoveUpCmd::NodeMoveUpCmd( Node &node, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        m_moved( false )
{

    m_project = static_cast<Project *>( m_node.projectNode() );
}
void NodeMoveUpCmd::execute()
{
    if ( m_project ) {
        m_moved = m_project->moveTaskUp( &m_node );
    }


}
void NodeMoveUpCmd::unexecute()
{
    if ( m_project && m_moved ) {
        m_project->moveTaskDown( &m_node );
    }
    m_moved = false;

}

NodeMoveDownCmd::NodeMoveDownCmd( Node &node, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        m_moved( false )
{

    m_project = static_cast<Project *>( m_node.projectNode() );
}
void NodeMoveDownCmd::execute()
{
    if ( m_project ) {
        m_moved = m_project->moveTaskDown( &m_node );
    }

}
void NodeMoveDownCmd::unexecute()
{
    if ( m_project && m_moved ) {
        m_project->moveTaskUp( &m_node );
    }
    m_moved = false;

}

NodeMoveCmd::NodeMoveCmd( Project *project, Node *node, Node *newParent, int newPos, const QString& name )
    : NamedCommand( name ),
    m_project( project ),
    m_node( node ),
    m_newparent( newParent ),
    m_newpos( newPos ),
    m_moved( false ),
    m_cmd( 0 )
{
    m_oldparent = node->parentNode();
    Q_ASSERT( m_oldparent );
    m_oldpos = m_oldparent->indexOf( node );
    
    // Summarytasks can't have resources, so remove resource requests from the new parent
    foreach ( ResourceGroupRequest *r, newParent->requests().requests() ) {
        if ( m_cmd == 0 ) m_cmd = new MacroCommand( "" );
        m_cmd->addCommand( new RemoveResourceGroupRequestCmd( r ) );
    }
    // TODO appointments ??
}
NodeMoveCmd::~NodeMoveCmd()
{
    delete m_cmd;
}
void NodeMoveCmd::execute()
{
    if ( m_project ) {
        m_moved = m_project->moveTask( m_node, m_newparent, m_newpos );
        if ( m_moved && m_cmd ) {
            m_cmd->execute();
        }
    }

}
void NodeMoveCmd::unexecute()
{
    if ( m_project && m_moved ) {
        m_moved = m_project->moveTask( m_node, m_oldparent, m_oldpos );
        if ( m_moved && m_cmd ) {
            m_cmd->unexecute();
        }
    }
    m_moved = false;

}

AddRelationCmd::AddRelationCmd( Project &project, Relation *rel, const QString& name )
        : NamedCommand( name ),
        m_rel( rel ),
        m_project( project )
{
    m_taken = true;
}
AddRelationCmd::~AddRelationCmd()
{
    if ( m_taken )
        delete m_rel;
}
void AddRelationCmd::execute()
{
    //kDebug()<<m_rel->parent()<<" to"<<m_rel->child();
    m_taken = false;
    m_project.addRelation( m_rel, false );
//    setSchScheduled( false );

}
void AddRelationCmd::unexecute()
{
    m_taken = true;
    m_project.takeRelation( m_rel );
//    setSchScheduled();

}

DeleteRelationCmd::DeleteRelationCmd( Project &project, Relation *rel, const QString& name )
        : NamedCommand( name ),
        m_rel( rel ),
        m_project( project )
{
    m_taken = false;
}
DeleteRelationCmd::~DeleteRelationCmd()
{
    if ( m_taken )
        delete m_rel;
}
void DeleteRelationCmd::execute()
{
    //kDebug()<<m_rel->parent()<<" to"<<m_rel->child();
    m_taken = true;
    m_project.takeRelation( m_rel );
//    setSchScheduled( false );

}
void DeleteRelationCmd::unexecute()
{
    m_taken = false;
    m_project.addRelation( m_rel, false );
//    setSchScheduled();

}

ModifyRelationTypeCmd::ModifyRelationTypeCmd( Relation *rel, Relation::Type type, const QString& name )
        : NamedCommand( name ),
        m_rel( rel ),
        m_newtype( type )
{

    m_oldtype = rel->type();
    m_project = dynamic_cast<Project*>( rel->parent() ->projectNode() );
/*    if ( m_project ) {
        foreach ( Schedule * s, m_project->schedules() ) {
            addSchScheduled( s );
        }
    }*/
}
void ModifyRelationTypeCmd::execute()
{
    if ( m_project ) {
        m_project->setRelationType( m_rel, m_newtype );
//        setSchScheduled( false );
    
    }
}
void ModifyRelationTypeCmd::unexecute()
{
    if ( m_project ) {
        m_project->setRelationType( m_rel, m_oldtype );
//        setSchScheduled();
    
    }
}

ModifyRelationLagCmd::ModifyRelationLagCmd( Relation *rel, Duration lag, const QString& name )
        : NamedCommand( name ),
        m_rel( rel ),
        m_newlag( lag )
{

    m_oldlag = rel->lag();
    m_project = dynamic_cast<Project*>( rel->parent() ->projectNode() );
/*    if ( m_project ) {
        foreach ( Schedule * s, m_project->schedules() ) {
            addSchScheduled( s );
        }
    }*/
}
void ModifyRelationLagCmd::execute()
{
    if ( m_project ) {
        m_project->setRelationLag( m_rel, m_newlag );
//        setSchScheduled( false );
    
    }
}
void ModifyRelationLagCmd::unexecute()
{
    if ( m_project ) {
        m_project->setRelationLag( m_rel, m_oldlag );
//        setSchScheduled();
    
    }
}

AddResourceRequestCmd::AddResourceRequestCmd( ResourceGroupRequest *group, ResourceRequest *request, const QString& name )
        : NamedCommand( name ),
        m_group( group ),
        m_request( request )
{

    m_mine = true;
}
AddResourceRequestCmd::~AddResourceRequestCmd()
{
    if ( m_mine )
        delete m_request;
}
void AddResourceRequestCmd::execute()
{
    //kDebug()<<"group="<<m_group<<" req="<<m_request;
    m_group->addResourceRequest( m_request );
    m_mine = false;
//    setSchScheduled( false );

}
void AddResourceRequestCmd::unexecute()
{
    //kDebug()<<"group="<<m_group<<" req="<<m_request;
    m_group->takeResourceRequest( m_request );
    m_mine = true;
//    setSchScheduled();

}

RemoveResourceRequestCmd::RemoveResourceRequestCmd( ResourceGroupRequest *group, ResourceRequest *request, const QString& name )
        : NamedCommand( name ),
        m_group( group ),
        m_request( request )
{

    m_mine = false;
    //kDebug()<<"group req="<<group<<" req="<<request<<" to gr="<<m_group->group();
/*    Task *t = request->task();
    if ( t ) { // safety, something is seriously wrong!
        foreach ( Schedule * s, t->schedules() ) {
            addSchScheduled( s );
        }
    }*/
}
RemoveResourceRequestCmd::~RemoveResourceRequestCmd()
{
    if ( m_mine )
        delete m_request;
}
void RemoveResourceRequestCmd::execute()
{
    m_group->takeResourceRequest( m_request );
    m_mine = true;
//    setSchScheduled( false );

}
void RemoveResourceRequestCmd::unexecute()
{
    m_group->addResourceRequest( m_request );
    m_mine = false;
//    setSchScheduled();

}

ModifyResourceRequestUnitsCmd::ModifyResourceRequestUnitsCmd( ResourceRequest *request, int oldvalue, int newvalue, const QString& name )
    : NamedCommand( name ),
    m_request( request ),
    m_oldvalue( oldvalue ),
    m_newvalue( newvalue )
{
}
void ModifyResourceRequestUnitsCmd::execute()
{
    m_request->setUnits( m_newvalue );
}
void ModifyResourceRequestUnitsCmd::unexecute()
{
    m_request->setUnits( m_oldvalue );
}

ModifyResourceGroupRequestUnitsCmd::ModifyResourceGroupRequestUnitsCmd( ResourceGroupRequest *request, int oldvalue, int newvalue, const QString& name )
    : NamedCommand( name ),
    m_request( request ),
    m_oldvalue( oldvalue ),
    m_newvalue( newvalue )
{
}
void ModifyResourceGroupRequestUnitsCmd::execute()
{
    m_request->setUnits( m_newvalue );
}
void ModifyResourceGroupRequestUnitsCmd::unexecute()
{
    m_request->setUnits( m_oldvalue );
}


ModifyEstimateCmd::ModifyEstimateCmd( Node &node, double oldvalue, double newvalue, const QString& name )
    : NamedCommand( name ),
    m_estimate( node.estimate() ),
    m_oldvalue( oldvalue ),
    m_newvalue( newvalue ),
    m_cmd( 0 )
{
/*    foreach ( Schedule * s, node.schedules() ) {
        addSchScheduled( s );
    }*/
    if ( newvalue == 0.0 ) {
        // Milestones can't have resources, so remove resource requests
        foreach ( ResourceGroupRequest *r, node.requests().requests() ) {
            if ( m_cmd == 0 ) m_cmd = new MacroCommand( "" );
            m_cmd->addCommand( new RemoveResourceGroupRequestCmd( r ) );
        }
    }
}
ModifyEstimateCmd::~ModifyEstimateCmd()
{
    delete m_cmd;
}
void ModifyEstimateCmd::execute()
{
    int pess = m_estimate->pessimisticRatio();
    int opt = m_estimate->optimisticRatio();
    m_estimate->setExpectedEstimate( m_newvalue );
    if ( m_cmd ) {
        m_cmd->execute();
    }
    m_estimate->setPessimisticRatio( pess );
    m_estimate->setOptimisticRatio( opt );

}
void ModifyEstimateCmd::unexecute()
{
    int pess = m_estimate->pessimisticRatio();
    int opt = m_estimate->optimisticRatio();
    m_estimate->setExpectedEstimate( m_oldvalue );
    if ( m_cmd ) {
        m_cmd->unexecute();
    }
    m_estimate->setPessimisticRatio( pess );
    m_estimate->setOptimisticRatio( opt );
}

EstimateModifyOptimisticRatioCmd::EstimateModifyOptimisticRatioCmd( Node &node, int oldvalue, int newvalue, const QString& name )
        : NamedCommand( name ),
        m_estimate( node.estimate() ),
        m_oldvalue( oldvalue ),
        m_newvalue( newvalue )
{

/*    foreach ( Schedule * s, node.schedules() ) {
        addSchScheduled( s );
    }*/
}
void EstimateModifyOptimisticRatioCmd::execute()
{
    m_estimate->setOptimisticRatio( m_newvalue );
//    setSchScheduled( false );

}
void EstimateModifyOptimisticRatioCmd::unexecute()
{
    m_estimate->setOptimisticRatio( m_oldvalue );
//    setSchScheduled();

}

EstimateModifyPessimisticRatioCmd::EstimateModifyPessimisticRatioCmd( Node &node, int oldvalue, int newvalue, const QString& name )
        : NamedCommand( name ),
        m_estimate( node.estimate() ),
        m_oldvalue( oldvalue ),
        m_newvalue( newvalue )
{

/*    foreach ( Schedule * s, node.schedules() ) {
        addSchScheduled( s );
    }*/
}
void EstimateModifyPessimisticRatioCmd::execute()
{
    m_estimate->setPessimisticRatio( m_newvalue );
//    setSchScheduled( false );

}
void EstimateModifyPessimisticRatioCmd::unexecute()
{
    m_estimate->setPessimisticRatio( m_oldvalue );
//    setSchScheduled();

}

ModifyEstimateTypeCmd::ModifyEstimateTypeCmd( Node &node, int oldvalue, int newvalue, const QString& name )
        : NamedCommand( name ),
        m_estimate( node.estimate() ),
        m_oldvalue( oldvalue ),
        m_newvalue( newvalue )
{

/*    foreach ( Schedule * s, node.schedules() ) {
        addSchScheduled( s );
    }*/
}
void ModifyEstimateTypeCmd::execute()
{
    m_estimate->setType( static_cast<Estimate::Type>( m_newvalue ) );
//    setSchScheduled( false );

}
void ModifyEstimateTypeCmd::unexecute()
{
    m_estimate->setType( static_cast<Estimate::Type>( m_oldvalue ) );
//    setSchScheduled();

}

ModifyEstimateCalendarCmd::ModifyEstimateCalendarCmd( Node &node, Calendar *oldvalue, Calendar *newvalue, const QString& name )
    : NamedCommand( name ),
        m_estimate( node.estimate() ),
        m_oldvalue( oldvalue ),
        m_newvalue( newvalue )
{
}
void ModifyEstimateCalendarCmd::execute()
{
    m_estimate->setCalendar( m_newvalue );
}
void ModifyEstimateCalendarCmd::unexecute()
{
    m_estimate->setCalendar( m_oldvalue );
}

ModifyEstimateUnitCmd::ModifyEstimateUnitCmd( Node &node, Duration::Unit oldvalue, Duration::Unit newvalue, const QString& name )
        : NamedCommand( name ),
        m_estimate( node.estimate() ),
        m_oldvalue( oldvalue ),
        m_newvalue( newvalue )
{
}
void ModifyEstimateUnitCmd::execute()
{
    m_estimate->setUnit( m_newvalue );

}
void ModifyEstimateUnitCmd::unexecute()
{
    m_estimate->setUnit( m_oldvalue );
}

EstimateModifyRiskCmd::EstimateModifyRiskCmd( Node &node, int oldvalue, int newvalue, const QString& name )
        : NamedCommand( name ),
        m_estimate( node.estimate() ),
        m_oldvalue( oldvalue ),
        m_newvalue( newvalue )
{

/*    foreach ( Schedule * s, node.schedules() ) {
        addSchScheduled( s );
    }*/
}
void EstimateModifyRiskCmd::execute()
{
    m_estimate->setRisktype( static_cast<Estimate::Risktype>( m_newvalue ) );
//    setSchScheduled( false );

}
void EstimateModifyRiskCmd::unexecute()
{
    m_estimate->setRisktype( static_cast<Estimate::Risktype>( m_oldvalue ) );
//    setSchScheduled();

}

AddResourceGroupRequestCmd::AddResourceGroupRequestCmd( Task &task, ResourceGroupRequest *request, const QString& name )
        : NamedCommand( name ),
        m_task( task ),
        m_request( request )
{

    m_mine = true;
}
void AddResourceGroupRequestCmd::execute()
{
    //kDebug()<<"group="<<m_request;
    m_task.addRequest( m_request );
    m_mine = false;


}
void AddResourceGroupRequestCmd::unexecute()
{
    //kDebug()<<"group="<<m_request;
    m_task.takeRequest( m_request ); // group should now be empty of resourceRequests
    m_mine = true;


}

RemoveResourceGroupRequestCmd::RemoveResourceGroupRequestCmd( ResourceGroupRequest *request, const QString& name )
        : NamedCommand( name ),
        m_task( *(request->parent() ->task()) ),
        m_request( request )
{

    m_mine = false;
}

RemoveResourceGroupRequestCmd::RemoveResourceGroupRequestCmd( Task &task, ResourceGroupRequest *request, const QString& name )
        : NamedCommand( name ),
        m_task( task ),
        m_request( request )
{

    m_mine = false;
}
void RemoveResourceGroupRequestCmd::execute()
{
    //kDebug()<<"group="<<m_request;
    m_task.takeRequest( m_request ); // group should now be empty of resourceRequests
    m_mine = true;


}
void RemoveResourceGroupRequestCmd::unexecute()
{
    //kDebug()<<"group="<<m_request;
    m_task.addRequest( m_request );
    m_mine = false;


}

AddResourceCmd::AddResourceCmd( ResourceGroup *group, Resource *resource, const QString& name )
        : NamedCommand( name ),
        m_group( group ),
        m_resource( resource )
{
    m_index = group->indexOf( resource );
    m_mine = true;
}
AddResourceCmd::~AddResourceCmd()
{
    if ( m_mine ) {
        //kDebug()<<"delete:"<<m_resource;
        delete m_resource;
    }
}
void AddResourceCmd::execute()
{
    Q_ASSERT( m_group->project() );
    if ( m_group->project() ) {
        m_group->project()->addResource( m_group, m_resource, m_index );
        m_mine = false;
        //kDebug()<<"added:"<<m_resource;
    }

}
void AddResourceCmd::unexecute()
{
    Q_ASSERT( m_group->project() );
    if ( m_group->project() ) {
        m_group->project()->takeResource( m_group, m_resource );
        //kDebug()<<"removed:"<<m_resource;
        m_mine = true;
    }

    Q_ASSERT( m_group->project() );
}

RemoveResourceCmd::RemoveResourceCmd( ResourceGroup *group, Resource *resource, const QString& name )
        : AddResourceCmd( group, resource, name )
{
    //kDebug()<<resource;
    m_mine = false;
    m_requests = m_resource->requests();

    if ( group->project() ) {
        foreach ( Schedule * s, group->project()->schedules() ) {
            Schedule *rs = resource->findSchedule( s->id() );
            if ( rs && ! rs->isDeleted() ) {
                kDebug()<<s->name();
                addSchScheduled( s );
            }
        }
    }
}
RemoveResourceCmd::~RemoveResourceCmd()
{
    while ( !m_appointments.isEmpty() )
        delete m_appointments.takeFirst();
}
void RemoveResourceCmd::execute()
{
    foreach ( ResourceRequest * r, m_requests ) {
        r->parent() ->takeResourceRequest( r );
        //kDebug()<<"Remove request for"<<r->resource()->name();
    }
/*    foreach ( Appointment * a, m_resource->appointments() ) {
        m_appointments.append( a );
    }
    foreach ( Appointment * a, m_appointments ) {
        a->detach(); //NOTE: removes from m_resource->appointments()
        //kDebug()<<"detached:"<<a;
    }*/
    AddResourceCmd::unexecute();
    setSchScheduled( false );
}
void RemoveResourceCmd::unexecute()
{
/*    while ( !m_appointments.isEmpty() ) {
        //kDebug()<<"attach:"<<m_appointments.first();
        m_appointments.takeFirst() ->attach();
    }*/
    foreach ( ResourceRequest * r, m_requests ) {
        r->parent() ->addResourceRequest( r );
        //kDebug()<<"Add request for"<<r->resource()->name();
    }
    AddResourceCmd::execute();
    setSchScheduled();
}

MoveResourceCmd::MoveResourceCmd( ResourceGroup *group, Resource *resource, const QString& name )
    : NamedCommand( name ),
    m_project( *(group->project()) ),
    m_resource( resource ),
    m_oldvalue( resource->parentGroup() ),
    m_newvalue( group )
{
    foreach ( ResourceRequest * r, resource->requests() ) {
        cmd.addCommand( new RemoveResourceRequestCmd( r->parent(), r ) );
    }
}
void MoveResourceCmd::execute()
{
    cmd.execute();
    m_project.moveResource( m_newvalue, m_resource );
}
void MoveResourceCmd::unexecute()
{
    m_project.moveResource( m_oldvalue, m_resource );
    cmd.unexecute();
}

ModifyResourceNameCmd::ModifyResourceNameCmd( Resource *resource, const QString& value, const QString& name )
        : NamedCommand( name ),
        m_resource( resource ),
        m_newvalue( value )
{
    m_oldvalue = resource->name();
}
void ModifyResourceNameCmd::execute()
{
    m_resource->setName( m_newvalue );


}
void ModifyResourceNameCmd::unexecute()
{
    m_resource->setName( m_oldvalue );


}
ModifyResourceInitialsCmd::ModifyResourceInitialsCmd( Resource *resource, const QString& value, const QString& name )
        : NamedCommand( name ),
        m_resource( resource ),
        m_newvalue( value )
{
    m_oldvalue = resource->initials();
}
void ModifyResourceInitialsCmd::execute()
{
    m_resource->setInitials( m_newvalue );


}
void ModifyResourceInitialsCmd::unexecute()
{
    m_resource->setInitials( m_oldvalue );


}
ModifyResourceEmailCmd::ModifyResourceEmailCmd( Resource *resource, const QString& value, const QString& name )
        : NamedCommand( name ),
        m_resource( resource ),
        m_newvalue( value )
{
    m_oldvalue = resource->email();
}
void ModifyResourceEmailCmd::execute()
{
    m_resource->setEmail( m_newvalue );


}
void ModifyResourceEmailCmd::unexecute()
{
    m_resource->setEmail( m_oldvalue );


}
ModifyResourceTypeCmd::ModifyResourceTypeCmd( Resource *resource, int value, const QString& name )
        : NamedCommand( name ),
        m_resource( resource ),
        m_newvalue( value )
{
    m_oldvalue = resource->type();

/*    foreach ( Schedule * s, resource->schedules() ) {
        addSchScheduled( s );
    }*/
}
void ModifyResourceTypeCmd::execute()
{
    m_resource->setType( ( Resource::Type ) m_newvalue );
//    setSchScheduled( false );

}
void ModifyResourceTypeCmd::unexecute()
{
    m_resource->setType( ( Resource::Type ) m_oldvalue );
//    setSchScheduled();

}
ModifyResourceUnitsCmd::ModifyResourceUnitsCmd( Resource *resource, int value, const QString& name )
        : NamedCommand( name ),
        m_resource( resource ),
        m_newvalue( value )
{
    m_oldvalue = resource->units();

/*    foreach ( Schedule * s, resource->schedules() ) {
        addSchScheduled( s );
    }*/
}
void ModifyResourceUnitsCmd::execute()
{
    m_resource->setUnits( m_newvalue );
//    setSchScheduled( false );

}
void ModifyResourceUnitsCmd::unexecute()
{
    m_resource->setUnits( m_oldvalue );
//    setSchScheduled();

}

ModifyResourceAvailableFromCmd::ModifyResourceAvailableFromCmd( Resource *resource, const QDateTime& value, const QString& name )
        : NamedCommand( name ),
        m_resource( resource ),
        m_newvalue( value )
{
    m_oldvalue = resource->availableFrom();
    m_spec = resource->timeSpec();
/*    DateTime v = DateTime( value, m_spec );
    if ( resource->project() ) {
        DateTime s;
        DateTime e;
        foreach ( Schedule * rs, resource->schedules() ) {
            Schedule * sch = resource->project() ->findSchedule( rs->id() );
            if ( sch ) {
                s = sch->start();
                e = sch->end();
                //kDebug() <<"old=" << m_oldvalue <<" new=" << value <<" s=" << s <<" e=" << e;
            }
            if ( !s.isValid() || !e.isValid() || ( ( m_oldvalue > s || v > s ) && ( m_oldvalue < e || v < e ) ) ) {
                addSchScheduled( rs );
            }
        }
    }*/
}
void ModifyResourceAvailableFromCmd::execute()
{
    m_resource->setAvailableFrom( DateTime( m_newvalue, m_spec ) );
//    setSchScheduled( false );
 //FIXME
}
void ModifyResourceAvailableFromCmd::unexecute()
{
    m_resource->setAvailableFrom( m_oldvalue );
//    setSchScheduled();
 //FIXME
}

ModifyResourceAvailableUntilCmd::ModifyResourceAvailableUntilCmd( Resource *resource, const QDateTime& value, const QString& name )
        : NamedCommand( name ),
        m_resource( resource ),
        m_newvalue( value )
{
    m_oldvalue = resource->availableUntil();
    m_spec = resource->timeSpec();
/*    DateTime v = DateTime( value, m_spec );
    if ( resource->project() ) {
        DateTime s;
        DateTime e;
        foreach ( Schedule * rs, resource->schedules() ) {
            Schedule * sch = resource->project() ->findSchedule( rs->id() );
            if ( sch ) {
                s = sch->start();
                e = sch->end();
                //kDebug() <<"old=" << m_oldvalue <<" new=" << value <<" s=" << s <<" e=" << e;
            }
            if ( !s.isValid() || !e.isValid() || ( ( m_oldvalue > s || v > s ) && ( m_oldvalue < e || v < e ) ) ) {
                addSchScheduled( rs );
            }
        }
    }*/
}
void ModifyResourceAvailableUntilCmd::execute()
{
    m_resource->setAvailableUntil( DateTime( m_newvalue, m_spec ) );
//    setSchScheduled( false );
 //FIXME
}
void ModifyResourceAvailableUntilCmd::unexecute()
{
    m_resource->setAvailableUntil( m_oldvalue );
//    setSchScheduled();
 //FIXME
}

ModifyResourceNormalRateCmd::ModifyResourceNormalRateCmd( Resource *resource, double value, const QString& name )
        : NamedCommand( name ),
        m_resource( resource ),
        m_newvalue( value )
{
    m_oldvalue = resource->normalRate();
}
void ModifyResourceNormalRateCmd::execute()
{
    m_resource->setNormalRate( m_newvalue );


}
void ModifyResourceNormalRateCmd::unexecute()
{
    m_resource->setNormalRate( m_oldvalue );


}
ModifyResourceOvertimeRateCmd::ModifyResourceOvertimeRateCmd( Resource *resource, double value, const QString& name )
        : NamedCommand( name ),
        m_resource( resource ),
        m_newvalue( value )
{
    m_oldvalue = resource->overtimeRate();
}
void ModifyResourceOvertimeRateCmd::execute()
{
    m_resource->setOvertimeRate( m_newvalue );


}
void ModifyResourceOvertimeRateCmd::unexecute()
{
    m_resource->setOvertimeRate( m_oldvalue );


}

ModifyResourceCalendarCmd::ModifyResourceCalendarCmd( Resource *resource, Calendar *value, const QString& name )
        : NamedCommand( name ),
        m_resource( resource ),
        m_newvalue( value )
{
    m_oldvalue = resource->calendar( true );

/*    foreach ( Schedule * s, resource->schedules() ) {
        addSchScheduled( s );
    }*/
}
void ModifyResourceCalendarCmd::execute()
{
    m_resource->setCalendar( m_newvalue );
//    setSchScheduled( false );

}
void ModifyResourceCalendarCmd::unexecute()
{
    m_resource->setCalendar( m_oldvalue );
//    setSchScheduled();

}

RemoveResourceGroupCmd::RemoveResourceGroupCmd( Project *project, ResourceGroup *group, const QString& name )
        : NamedCommand( name ),
        m_group( group ),
        m_project( project ),
        m_cmd( 0 )
{
    m_index = project->indexOf( group );
    m_mine = false;
    if ( !m_group->requests().isEmpty() ) {
        m_cmd = new MacroCommand("");
        foreach( ResourceGroupRequest * r, m_group->requests() ) {
            m_cmd->addCommand( new RemoveResourceGroupRequestCmd( r ) );
        }
    }
}
RemoveResourceGroupCmd::~RemoveResourceGroupCmd()
{
    delete m_cmd;
    if ( m_mine )
        delete m_group;
}
void RemoveResourceGroupCmd::execute()
{
    // remove all requests to this group
    if ( m_cmd ) {
        m_cmd->execute();
    }
    if ( m_project )
        m_project->takeResourceGroup( m_group );
    m_mine = true;


}
void RemoveResourceGroupCmd::unexecute()
{
    if ( m_project )
        m_project->addResourceGroup( m_group, m_index );

    m_mine = false;
    // add all requests
    if ( m_cmd ) {
        m_cmd->unexecute();
    }

}

AddResourceGroupCmd::AddResourceGroupCmd( Project *project, ResourceGroup *group, const QString& name )
        : RemoveResourceGroupCmd( project, group, name )
{
    m_mine = true;
}
void AddResourceGroupCmd::execute()
{
    RemoveResourceGroupCmd::unexecute();
}
void AddResourceGroupCmd::unexecute()
{
    RemoveResourceGroupCmd::execute();
}

ModifyResourceGroupNameCmd::ModifyResourceGroupNameCmd( ResourceGroup *group, const QString& value, const QString& name )
        : NamedCommand( name ),
        m_group( group ),
        m_newvalue( value )
{
    m_oldvalue = group->name();
}
void ModifyResourceGroupNameCmd::execute()
{
    m_group->setName( m_newvalue );


}
void ModifyResourceGroupNameCmd::unexecute()
{
    m_group->setName( m_oldvalue );


}

ModifyResourceGroupTypeCmd::ModifyResourceGroupTypeCmd( ResourceGroup *group, int value, const QString& name )
    : NamedCommand( name ),
        m_group( group ),
        m_newvalue( value )
{
    m_oldvalue = group->type();
}
void ModifyResourceGroupTypeCmd::execute()
{
    m_group->setType( static_cast<ResourceGroup::Type>( m_newvalue) );


}
void ModifyResourceGroupTypeCmd::unexecute()
{
    m_group->setType( static_cast<ResourceGroup::Type>( m_oldvalue ) );


}

ModifyCompletionEntrymodeCmd::ModifyCompletionEntrymodeCmd( Completion &completion, Completion::Entrymode value, const QString& name )
        : NamedCommand( name ),
        m_completion( completion ),
        oldvalue( m_completion.entrymode() ),
        newvalue( value )
{
}
void ModifyCompletionEntrymodeCmd::execute()
{
    m_completion.setEntrymode( newvalue );
}
void ModifyCompletionEntrymodeCmd::unexecute()
{
    m_completion.setEntrymode( oldvalue );
}

ModifyCompletionPercentFinishedCmd::ModifyCompletionPercentFinishedCmd( Completion &completion, const QDate &date, int value, const QString& name )
    : NamedCommand( name ),
    m_completion( completion ),
    m_date( date ),
    m_newvalue( value ),
    m_oldvalue( completion.percentFinished( date ) )
{
    if ( ! completion.entries().contains( date ) ) {
        Completion::Entry *e = new Completion::Entry();
        Completion::Entry *latest = completion.entry( completion.entryDate() );
        if ( latest ) {
            *e = *latest;
        }
        cmd.addCommand( new AddCompletionEntryCmd( completion, date, e ) );
    }

}
void ModifyCompletionPercentFinishedCmd::execute()
{
    cmd.execute();
    m_completion.setPercentFinished( m_date, m_newvalue );
}
void ModifyCompletionPercentFinishedCmd::unexecute()
{
    m_completion.setPercentFinished( m_date, m_oldvalue );
    cmd.unexecute();
}

ModifyCompletionRemainingEffortCmd::ModifyCompletionRemainingEffortCmd( Completion &completion, const QDate &date, const Duration &value, const QString &name )
    : NamedCommand( name ),
    m_completion( completion ),
    m_date( date ),
    m_newvalue( value ),
    m_oldvalue( completion.remainingEffort( date ) )
{
    if ( ! completion.entries().contains( date ) ) {
        Completion::Entry *e = new Completion::Entry();
        Completion::Entry *latest = completion.entry( completion.entryDate() );
        if ( latest ) {
            *e = *latest;
        }
        cmd.addCommand( new AddCompletionEntryCmd( completion, date, e ) );
    }

}
void ModifyCompletionRemainingEffortCmd::execute()
{
    cmd.execute();
    m_completion.setRemainingEffort( m_date, m_newvalue );
}
void ModifyCompletionRemainingEffortCmd::unexecute()
{
    m_completion.setRemainingEffort( m_date, m_oldvalue );
    cmd.unexecute();
}

ModifyCompletionActualEffortCmd::ModifyCompletionActualEffortCmd( Completion &completion, const QDate &date, const Duration &value, const QString &name )
    : NamedCommand( name ),
    m_completion( completion ),
    m_date( date ),
    m_newvalue( value ),
    m_oldvalue( completion.actualEffort( date ) )
{
    if ( ! completion.entries().contains( date ) ) {
        Completion::Entry *e = new Completion::Entry();
        Completion::Entry *latest = completion.entry( completion.entryDate() );
        if ( latest ) {
            *e = *latest;
        }
        cmd.addCommand( new AddCompletionEntryCmd( completion, date, e ) );
    }

}
void ModifyCompletionActualEffortCmd::execute()
{
    cmd.execute();
    m_completion.setActualEffort( m_date, m_newvalue );
}
void ModifyCompletionActualEffortCmd::unexecute()
{
    m_completion.setActualEffort( m_date, m_oldvalue );
    cmd.unexecute();
}

ModifyCompletionStartedCmd::ModifyCompletionStartedCmd( Completion &completion, bool value, const QString& name )
        : NamedCommand( name ),
        m_completion( completion ),
        oldvalue( m_completion.isStarted() ),
        newvalue( value )
{
}
void ModifyCompletionStartedCmd::execute()
{
    m_completion.setStarted( newvalue );


}
void ModifyCompletionStartedCmd::unexecute()
{
    m_completion.setStarted( oldvalue );


}

ModifyCompletionFinishedCmd::ModifyCompletionFinishedCmd( Completion &completion, bool value, const QString& name )
        : NamedCommand( name ),
        m_completion( completion ),
        oldvalue( m_completion.isFinished() ),
        newvalue( value )
{
}
void ModifyCompletionFinishedCmd::execute()
{
    m_completion.setFinished( newvalue );


}
void ModifyCompletionFinishedCmd::unexecute()
{
    m_completion.setFinished( oldvalue );


}

ModifyCompletionStartTimeCmd::ModifyCompletionStartTimeCmd( Completion &completion, const QDateTime &value, const QString& name )
        : NamedCommand( name ),
        m_completion( completion ),
        oldvalue( m_completion.startTime() ),
        newvalue( value )
{
    m_spec = static_cast<Project*>( completion.node()->projectNode() )->timeSpec();
}
void ModifyCompletionStartTimeCmd::execute()
{
    m_completion.setStartTime( DateTime( newvalue, m_spec ) );


}
void ModifyCompletionStartTimeCmd::unexecute()
{
    m_completion.setStartTime( oldvalue );


}

ModifyCompletionFinishTimeCmd::ModifyCompletionFinishTimeCmd( Completion &completion, const QDateTime &value, const QString& name )
        : NamedCommand( name ),
        m_completion( completion ),
        oldvalue( m_completion.finishTime() ),
        newvalue( value )
{
    m_spec = static_cast<Project*>( completion.node()->projectNode() )->timeSpec();
}
void ModifyCompletionFinishTimeCmd::execute()
{
    m_completion.setFinishTime( DateTime( newvalue, m_spec ) );


}
void ModifyCompletionFinishTimeCmd::unexecute()
{
    m_completion.setFinishTime( oldvalue );


}

AddCompletionEntryCmd::AddCompletionEntryCmd( Completion &completion, const QDate &date, Completion::Entry *value, const QString& name )
        : NamedCommand( name ),
        m_completion( completion ),
        m_date( date ),
        newvalue( value ),
        m_newmine( true )
{
}
AddCompletionEntryCmd::~AddCompletionEntryCmd()
{
    if ( m_newmine )
        delete newvalue;
}
void AddCompletionEntryCmd::execute()
{
    Q_ASSERT( ! m_completion.entries().contains( m_date ) );
    m_completion.addEntry( m_date, newvalue );
    m_newmine = false;

}
void AddCompletionEntryCmd::unexecute()
{
    m_completion.takeEntry( m_date );
    m_newmine = true;

}

RemoveCompletionEntryCmd::RemoveCompletionEntryCmd( Completion &completion, const QDate &date, const QString& name )
        : NamedCommand( name ),
        m_completion( completion ),
        m_date( date ),
        m_mine( false )
{
    value = m_completion.entry( date );
}
RemoveCompletionEntryCmd::~RemoveCompletionEntryCmd()
{
    if ( m_mine )
        delete value;
}
void RemoveCompletionEntryCmd::execute()
{
    Q_ASSERT( m_completion.entries().contains( m_date ) );
    if ( value ) {
        m_completion.takeEntry( m_date );
        m_mine = true;
    }

}
void RemoveCompletionEntryCmd::unexecute()
{
    if ( value ) {
        m_completion.addEntry( m_date, value );
    }
    m_mine = false;

}


ModifyCompletionEntryCmd::ModifyCompletionEntryCmd( Completion &completion, const QDate &date, Completion::Entry *value, const QString& name )
        : NamedCommand( name )
{
    cmd = new MacroCommand("");
    cmd->addCommand( new RemoveCompletionEntryCmd( completion, date ) );
    cmd->addCommand( new AddCompletionEntryCmd( completion, date, value ) );
}
ModifyCompletionEntryCmd::~ModifyCompletionEntryCmd()
{
    delete cmd;
}
void ModifyCompletionEntryCmd::execute()
{
    cmd->execute();
}
void ModifyCompletionEntryCmd::unexecute()
{
    cmd->unexecute();
}

AddCompletionUsedEffortCmd::AddCompletionUsedEffortCmd( Completion &completion, const Resource *resource, Completion::UsedEffort *value, const QString& name )
        : NamedCommand( name ),
        m_completion( completion ),
        m_resource( resource ),
        newvalue( value ),
        m_newmine( true ),
        m_oldmine( false)
{
    oldvalue = m_completion.usedEffort( resource );
}
AddCompletionUsedEffortCmd::~AddCompletionUsedEffortCmd()
{
    if ( m_oldmine )
        delete oldvalue;
    if ( m_newmine )
        delete newvalue;
}
void AddCompletionUsedEffortCmd::execute()
{
    if ( oldvalue ) {
        m_completion.takeUsedEffort( m_resource );
        m_oldmine = true;
    }
    m_completion.addUsedEffort( m_resource, newvalue );
    m_newmine = false;

}
void AddCompletionUsedEffortCmd::unexecute()
{
    m_completion.takeUsedEffort( m_resource );
    if ( oldvalue ) {
        m_completion.addUsedEffort( m_resource, oldvalue );
    }
    m_newmine = true;
    m_oldmine = false;

}

AddCompletionActualEffortCmd::AddCompletionActualEffortCmd( Completion::UsedEffort &ue, const QDate &date, Completion::UsedEffort::ActualEffort *value, const QString& name )
        : NamedCommand( name ),
        m_usedEffort( ue ),
        m_date( date ),
        newvalue( value ),
        m_newmine( true ),
        m_oldmine( false)
{
    oldvalue = ue.effort( date );
}
AddCompletionActualEffortCmd::~AddCompletionActualEffortCmd()
{
    if ( m_oldmine )
        delete oldvalue;
    if ( m_newmine )
        delete newvalue;
}
void AddCompletionActualEffortCmd::execute()
{
    if ( oldvalue ) {
        m_usedEffort.takeEffort( m_date );
        m_oldmine = true;
    }
    m_usedEffort.setEffort( m_date, newvalue );
    m_newmine = false;

}
void AddCompletionActualEffortCmd::unexecute()
{
    m_usedEffort.takeEffort( m_date );
    if ( oldvalue ) {
        m_usedEffort.setEffort( m_date, oldvalue );
    }
    m_newmine = true;
    m_oldmine = false;

}

AddAccountCmd::AddAccountCmd( Project &project, Account *account, const QString& parent, const QString& name )
        : NamedCommand( name ),
        m_project( project ),
        m_account( account ),
        m_parent( 0 ),
        m_parentName( parent )
{
    m_mine = true;
}

AddAccountCmd::AddAccountCmd( Project &project, Account *account, Account *parent, const QString& name )
        : NamedCommand( name ),
        m_project( project ),
        m_account( account ),
        m_parent( parent )
{
    m_mine = true;
}

AddAccountCmd::~AddAccountCmd()
{
    if ( m_mine )
        delete m_account;
}

void AddAccountCmd::execute()
{
    if ( m_parent == 0 && !m_parentName.isEmpty() ) {
        m_parent = m_project.accounts().findAccount( m_parentName );
    }
    m_project.accounts().insert( m_account, m_parent );


    m_mine = false;
}
void AddAccountCmd::unexecute()
{
    m_project.accounts().take( m_account );


    m_mine = true;
}

RemoveAccountCmd::RemoveAccountCmd( Project &project, Account *account, const QString& name )
        : NamedCommand( name ),
        m_project( project ),
        m_account( account ),
        m_parent( account->parent() )
{
    if ( m_parent ) {
        m_index = m_parent->accountList().indexOf( account );
    } else {
        m_index = project.accounts().accountList().indexOf( account );
    }
    m_mine = false;
    m_isDefault = account == project.accounts().defaultAccount();
}

RemoveAccountCmd::~RemoveAccountCmd()
{
    if ( m_mine )
        delete m_account;
}

void RemoveAccountCmd::execute()
{
    if ( m_isDefault ) {
        m_project.accounts().setDefaultAccount( 0 );
    }
    m_project.accounts().take( m_account );


    m_mine = true;
}
void RemoveAccountCmd::unexecute()
{
    m_project.accounts().insert( m_account, m_parent, m_index );
    if ( m_isDefault ) {
        m_project.accounts().setDefaultAccount( m_account );
    }

    m_mine = false;
}

RenameAccountCmd::RenameAccountCmd( Account *account, const QString& value, const QString& name )
        : NamedCommand( name ),
        m_account( account )
{
    m_oldvalue = account->name();
    m_newvalue = value;
}

void RenameAccountCmd::execute()
{
    m_account->setName( m_newvalue );

}
void RenameAccountCmd::unexecute()
{
    m_account->setName( m_oldvalue );

}

ModifyAccountDescriptionCmd::ModifyAccountDescriptionCmd( Account *account, const QString& value, const QString& name )
        : NamedCommand( name ),
        m_account( account )
{
    m_oldvalue = account->description();
    m_newvalue = value;
}

void ModifyAccountDescriptionCmd::execute()
{
    m_account->setDescription( m_newvalue );

}
void ModifyAccountDescriptionCmd::unexecute()
{
    m_account->setDescription( m_oldvalue );

}


NodeModifyStartupCostCmd::NodeModifyStartupCostCmd( Node &node, double value, const QString& name )
        : NamedCommand( name ),
        m_node( node )
{
    m_oldvalue = node.startupCost();
    m_newvalue = value;
}

void NodeModifyStartupCostCmd::execute()
{
    m_node.setStartupCost( m_newvalue );

}
void NodeModifyStartupCostCmd::unexecute()
{
    m_node.setStartupCost( m_oldvalue );

}

NodeModifyShutdownCostCmd::NodeModifyShutdownCostCmd( Node &node, double value, const QString& name )
        : NamedCommand( name ),
        m_node( node )
{
    m_oldvalue = node.startupCost();
    m_newvalue = value;
}

void NodeModifyShutdownCostCmd::execute()
{
    m_node.setShutdownCost( m_newvalue );

}
void NodeModifyShutdownCostCmd::unexecute()
{
    m_node.setShutdownCost( m_oldvalue );

}

NodeModifyRunningAccountCmd::NodeModifyRunningAccountCmd( Node &node, Account *oldvalue, Account *newvalue, const QString& name )
        : NamedCommand( name ),
        m_node( node )
{
    m_oldvalue = oldvalue;
    m_newvalue = newvalue;
    //kDebug();
}
void NodeModifyRunningAccountCmd::execute()
{
    //kDebug();
    if ( m_oldvalue ) {
        m_oldvalue->removeRunning( m_node );
    }
    if ( m_newvalue ) {
        m_newvalue->addRunning( m_node );
    }

}
void NodeModifyRunningAccountCmd::unexecute()
{
    //kDebug();
    if ( m_newvalue ) {
        m_newvalue->removeRunning( m_node );
    }
    if ( m_oldvalue ) {
        m_oldvalue->addRunning( m_node );
    }

}

NodeModifyStartupAccountCmd::NodeModifyStartupAccountCmd( Node &node, Account *oldvalue, Account *newvalue, const QString& name )
        : NamedCommand( name ),
        m_node( node )
{
    m_oldvalue = oldvalue;
    m_newvalue = newvalue;
    //kDebug();
}

void NodeModifyStartupAccountCmd::execute()
{
    //kDebug();
    if ( m_oldvalue ) {
        m_oldvalue->removeStartup( m_node );
    }
    if ( m_newvalue ) {
        m_newvalue->addStartup( m_node );
    }

}
void NodeModifyStartupAccountCmd::unexecute()
{
    //kDebug();
    if ( m_newvalue ) {
        m_newvalue->removeStartup( m_node );
    }
    if ( m_oldvalue ) {
        m_oldvalue->addStartup( m_node );
    }

}

NodeModifyShutdownAccountCmd::NodeModifyShutdownAccountCmd( Node &node, Account *oldvalue, Account *newvalue, const QString& name )
        : NamedCommand( name ),
        m_node( node )
{
    m_oldvalue = oldvalue;
    m_newvalue = newvalue;
    //kDebug();
}

void NodeModifyShutdownAccountCmd::execute()
{
    //kDebug();
    if ( m_oldvalue ) {
        m_oldvalue->removeShutdown( m_node );
    }
    if ( m_newvalue ) {
        m_newvalue->addShutdown( m_node );
    }

}
void NodeModifyShutdownAccountCmd::unexecute()
{
    //kDebug();
    if ( m_newvalue ) {
        m_newvalue->removeShutdown( m_node );
    }
    if ( m_oldvalue ) {
        m_oldvalue->addShutdown( m_node );
    }

}

ModifyDefaultAccountCmd::ModifyDefaultAccountCmd( Accounts &acc, Account *oldvalue, Account *newvalue, const QString& name )
        : NamedCommand( name ),
        m_accounts( acc )
{
    m_oldvalue = oldvalue;
    m_newvalue = newvalue;
    //kDebug();
}

void ModifyDefaultAccountCmd::execute()
{
    //kDebug();
    m_accounts.setDefaultAccount( m_newvalue );

}
void ModifyDefaultAccountCmd::unexecute()
{
    //kDebug();
    m_accounts.setDefaultAccount( m_oldvalue );

}

ProjectModifyConstraintCmd::ProjectModifyConstraintCmd( Project &node, Node::ConstraintType c, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        newConstraint( c ),
        oldConstraint( static_cast<Node::ConstraintType>( node.constraint() ) )
{

/*    foreach ( Schedule * s, node.schedules() ) {
        addSchScheduled( s );
    }*/
}
void ProjectModifyConstraintCmd::execute()
{
    m_node.setConstraint( newConstraint );
//    setSchScheduled( false );

}
void ProjectModifyConstraintCmd::unexecute()
{
    m_node.setConstraint( oldConstraint );
//    setSchScheduled();

}

ProjectModifyStartTimeCmd::ProjectModifyStartTimeCmd( Project &node, const QDateTime& dt, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        newTime( dt ),
        oldTime( node.startTime() )
{
    m_spec = node.timeSpec();
/*    foreach ( Schedule * s, node.schedules() ) {
        addSchScheduled( s );
    }*/
}

void ProjectModifyStartTimeCmd::execute()
{
    m_node.setConstraintStartTime( DateTime( newTime, m_spec ) );
//    setSchScheduled( false );

}
void ProjectModifyStartTimeCmd::unexecute()
{
    m_node.setConstraintStartTime( oldTime );
//    setSchScheduled();

}

ProjectModifyEndTimeCmd::ProjectModifyEndTimeCmd( Project &node, const QDateTime& dt, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        newTime( dt ),
        oldTime( node.endTime() )
{
    m_spec = node.timeSpec();
/*    foreach ( Schedule * s, node.schedules() ) {
        addSchScheduled( s );
    }*/
}
void ProjectModifyEndTimeCmd::execute()
{
    m_node.setEndTime( DateTime( newTime, m_spec ) );
    m_node.setConstraintEndTime( DateTime( newTime, m_spec ) );
//    setSchScheduled( false );

}
void ProjectModifyEndTimeCmd::unexecute()
{
    m_node.setConstraintEndTime( oldTime );
//    setSchScheduled();

}

//----------------------------
AddScheduleManagerCmd::AddScheduleManagerCmd( Project &node, ScheduleManager *sm, const QString& name )
    : NamedCommand( name ),
    m_node( node ),
    m_parent( sm->parentManager() ),
    m_sm( sm ),
    m_index( -1 ),
    m_exp( sm->expected() ),
    m_opt( sm->optimistic() ),
    m_pess( sm->pessimistic() ),
    m_mine( true)
{
}

AddScheduleManagerCmd::AddScheduleManagerCmd( ScheduleManager *parent, ScheduleManager *sm, const QString& name )
    : NamedCommand( name ),
    m_node( parent->project() ),
    m_parent( parent ),
    m_sm( sm ),
    m_index( -1 ),
    m_exp( sm->expected() ),
    m_opt( sm->optimistic() ),
    m_pess( sm->pessimistic() ),
    m_mine( true)
{
}

AddScheduleManagerCmd::~AddScheduleManagerCmd()
{
    if ( m_mine ) {
        m_sm->setParentManager( 0 );
        delete m_sm;
    }
}

void AddScheduleManagerCmd::execute()
{
    m_node.addScheduleManager( m_sm, m_parent );
    m_sm->setExpected( m_exp );
    m_sm->setOptimistic( m_opt );
    m_sm->setPessimistic( m_pess );
    m_mine = false;
}

void AddScheduleManagerCmd::unexecute()
{
    m_index = m_node.takeScheduleManager( m_sm );
    m_sm->setExpected( 0 );
    m_sm->setOptimistic( 0 );
    m_sm->setPessimistic( 0 );
    m_mine = true;
}

DeleteScheduleManagerCmd::DeleteScheduleManagerCmd( Project &node, ScheduleManager *sm, const QString& name )
    : AddScheduleManagerCmd( node, sm, name )
{
    m_mine = false;
    foreach ( ScheduleManager *s, sm->children() ) {
        cmd.addCommand( new DeleteScheduleManagerCmd( node, s ) );
    }
}

void DeleteScheduleManagerCmd::execute()
{
    cmd.execute();
    AddScheduleManagerCmd::unexecute();
}

void DeleteScheduleManagerCmd::unexecute()
{
    AddScheduleManagerCmd::execute();
    cmd.unexecute();
}

ModifyScheduleManagerNameCmd::ModifyScheduleManagerNameCmd( ScheduleManager &sm, const QString& value, const QString& name )
    : NamedCommand( name ),
    m_sm( sm ),
    oldvalue( sm.name() ),
    newvalue( value )
{
}

void ModifyScheduleManagerNameCmd::execute()
{
    m_sm.setName( newvalue );
}

void ModifyScheduleManagerNameCmd::unexecute()
{
    m_sm.setName( oldvalue );
}

ModifyScheduleManagerAllowOverbookingCmd::ModifyScheduleManagerAllowOverbookingCmd( ScheduleManager &sm, bool value, const QString& name )
    : NamedCommand( name ),
    m_sm( sm ),
    oldvalue( sm.allowOverbooking() ),
    newvalue( value )
{
}

void ModifyScheduleManagerAllowOverbookingCmd::execute()
{
    m_sm.setAllowOverbooking( newvalue );
}

void ModifyScheduleManagerAllowOverbookingCmd::unexecute()
{
    m_sm.setAllowOverbooking( oldvalue );
}

ModifyScheduleManagerDistributionCmd::ModifyScheduleManagerDistributionCmd( ScheduleManager &sm, bool value, const QString& name )
    : NamedCommand( name ),
    m_sm( sm ),
    oldvalue( sm.usePert() ),
    newvalue( value )
{
}

void ModifyScheduleManagerDistributionCmd::execute()
{
    m_sm.setUsePert( newvalue );
}

void ModifyScheduleManagerDistributionCmd::unexecute()
{
    m_sm.setUsePert( oldvalue );
}

ModifyScheduleManagerCalculateAllCmd::ModifyScheduleManagerCalculateAllCmd( ScheduleManager &sm, bool value, const QString& name )
    : NamedCommand( name ),
    m_sm( sm ),
    oldvalue( sm.calculateAll() ),
    newvalue( value )
{
}

void ModifyScheduleManagerCalculateAllCmd::execute()
{
    m_sm.setCalculateAll( newvalue );
}

void ModifyScheduleManagerCalculateAllCmd::unexecute()
{
    m_sm.setCalculateAll( oldvalue );
}

ModifyScheduleManagerSchedulingDirectionCmd::ModifyScheduleManagerSchedulingDirectionCmd( ScheduleManager &sm, bool value, const QString& name )
    : NamedCommand( name ),
    m_sm( sm ),
    oldvalue( sm.schedulingDirection() ),
    newvalue( value )
{
}

void ModifyScheduleManagerSchedulingDirectionCmd::execute()
{
    m_sm.setSchedulingDirection( newvalue );
}

void ModifyScheduleManagerSchedulingDirectionCmd::unexecute()
{
    m_sm.setSchedulingDirection( oldvalue );
}

ModifyScheduleManagerSchedulerCmd::ModifyScheduleManagerSchedulerCmd( ScheduleManager &sm, int value, const QString& name )
    : NamedCommand( name ),
    m_sm( sm ),
    oldvalue( sm.schedulerPluginIndex() ),
    newvalue( value )
{
}

void ModifyScheduleManagerSchedulerCmd::execute()
{
    m_sm.setSchedulerPlugin( newvalue );
}

void ModifyScheduleManagerSchedulerCmd::unexecute()
{
    m_sm.setSchedulerPlugin( oldvalue );
}

CalculateScheduleCmd::CalculateScheduleCmd( Project &node, ScheduleManager &sm, const QString& name )
    : NamedCommand( name ),
    m_node( node ),
    m_sm( sm ),
    m_first( true ),
    m_oldexpected( m_sm.expected() ),
    m_oldoptimistic( m_sm.optimistic() ),
    m_oldpessimistic( m_sm.pessimistic() ),
    m_newexpected( 0 ),
    m_newoptimistic( 0 ),
    m_newpessimistic( 0 )
{
}

void CalculateScheduleCmd::execute()
{
    if ( m_first ) {
        m_first = false;
        m_sm.calculateSchedule();
        m_newexpected = m_sm.expected();
        m_newoptimistic = m_sm.optimistic();
        m_newpessimistic = m_sm.pessimistic();
        return;
    }
    m_sm.setExpected( m_newexpected );
    m_sm.setOptimistic( m_newoptimistic );
    m_sm.setPessimistic( m_newpessimistic );
}

void CalculateScheduleCmd::unexecute()
{
    m_sm.setExpected( m_oldexpected );
    m_sm.setOptimistic( m_oldoptimistic );
    m_sm.setPessimistic( m_oldpessimistic );
}

//------------------------
BaselineScheduleCmd::BaselineScheduleCmd( ScheduleManager &sm, const QString& name )
    : NamedCommand( name ),
    m_sm( sm )
{
}

void BaselineScheduleCmd::execute()
{
    m_sm.setBaselined( true );
}

void BaselineScheduleCmd::unexecute()
{
    m_sm.setBaselined( false );
}

ResetBaselineScheduleCmd::ResetBaselineScheduleCmd( ScheduleManager &sm, const QString& name )
    : NamedCommand( name ),
    m_sm( sm )
{
}

void ResetBaselineScheduleCmd::execute()
{
    m_sm.setBaselined( false );
}

void ResetBaselineScheduleCmd::unexecute()
{
    m_sm.setBaselined( true );
}

//------------------------
ModifyStandardWorktimeYearCmd::ModifyStandardWorktimeYearCmd( StandardWorktime *wt, double oldvalue, double newvalue, const QString& name )
        : NamedCommand( name ),
        swt( wt ),
        m_oldvalue( oldvalue ),
        m_newvalue( newvalue )
{
}
void ModifyStandardWorktimeYearCmd::execute()
{
    swt->setYear( m_newvalue );

}
void ModifyStandardWorktimeYearCmd::unexecute()
{
    swt->setYear( m_oldvalue );

}

ModifyStandardWorktimeMonthCmd::ModifyStandardWorktimeMonthCmd( StandardWorktime *wt, double oldvalue, double newvalue, const QString& name )
        : NamedCommand( name ),
        swt( wt ),
        m_oldvalue( oldvalue ),
        m_newvalue( newvalue )
{
}
void ModifyStandardWorktimeMonthCmd::execute()
{
    swt->setMonth( m_newvalue );

}
void ModifyStandardWorktimeMonthCmd::unexecute()
{
    swt->setMonth( m_oldvalue );

}

ModifyStandardWorktimeWeekCmd::ModifyStandardWorktimeWeekCmd( StandardWorktime *wt, double oldvalue, double newvalue, const QString& name )
        : NamedCommand( name ),
        swt( wt ),
        m_oldvalue( oldvalue ),
        m_newvalue( newvalue )
{
}
void ModifyStandardWorktimeWeekCmd::execute()
{
    swt->setWeek( m_newvalue );

}
void ModifyStandardWorktimeWeekCmd::unexecute()
{
    swt->setWeek( m_oldvalue );

}

ModifyStandardWorktimeDayCmd::ModifyStandardWorktimeDayCmd( StandardWorktime *wt, double oldvalue, double newvalue, const QString& name )
        : NamedCommand( name ),
        swt( wt ),
        m_oldvalue( oldvalue ),
        m_newvalue( newvalue )
{
}

void ModifyStandardWorktimeDayCmd::execute()
{
    swt->setDay( m_newvalue );

}
void ModifyStandardWorktimeDayCmd::unexecute()
{
    swt->setDay( m_oldvalue );

}

//----------------
DocumentAddCmd::DocumentAddCmd( Documents &docs, Document *value, const QString& name )
    : NamedCommand( name ),
    m_docs( docs ),
    m_mine( true )
{
    Q_ASSERT( value );
    m_value = value;
}
DocumentAddCmd::~DocumentAddCmd()
{
    //kDebug();
    if ( m_mine )
        delete m_value;
}
void DocumentAddCmd::execute()
{
    m_docs.addDocument( m_value );
    m_mine = false;
}
void DocumentAddCmd::unexecute()
{
    m_docs.takeDocument( m_value );
    m_mine = true;
}

//----------------
DocumentRemoveCmd::DocumentRemoveCmd( Documents &docs, Document *value, const QString& name )
    : NamedCommand( name ),
    m_docs( docs ),
    m_mine( false )
{
    Q_ASSERT( value );
    m_value = value;
}
DocumentRemoveCmd::~DocumentRemoveCmd()
{
    //kDebug();
    if ( m_mine )
        delete m_value;
}
void DocumentRemoveCmd::execute()
{
    m_docs.takeDocument( m_value );
    m_mine = true;
}
void DocumentRemoveCmd::unexecute()
{
    m_docs.addDocument( m_value );
    m_mine = false;
}

//----------------
DocumentModifyUrlCmd::DocumentModifyUrlCmd( Document *doc, const KUrl &value, const QString& name )
    : NamedCommand( name ),
    m_doc( doc )
{
    Q_ASSERT( doc );
    m_value = value;
    m_oldvalue = doc->url();
}
void DocumentModifyUrlCmd::execute()
{
    m_doc->setUrl( m_value );
}
void DocumentModifyUrlCmd::unexecute()
{
    m_doc->setUrl( m_oldvalue );
}

//----------------
DocumentModifyTypeCmd::DocumentModifyTypeCmd( Document *doc, Document::Type value, const QString& name )
    : NamedCommand( name ),
    m_doc( doc )
{
    Q_ASSERT( doc );
    m_value = value;
    m_oldvalue = doc->type();
}
void DocumentModifyTypeCmd::execute()
{
    m_doc->setType( m_value );
}
void DocumentModifyTypeCmd::unexecute()
{
    m_doc->setType( m_oldvalue );
}

//----------------
DocumentModifyStatusCmd::DocumentModifyStatusCmd( Document *doc, const QString &value, const QString& name )
    : NamedCommand( name ),
    m_doc( doc )
{
    Q_ASSERT( doc );
    m_value = value;
    m_oldvalue = doc->type();
}
void DocumentModifyStatusCmd::execute()
{
    m_doc->setStatus( m_value );
}
void DocumentModifyStatusCmd::unexecute()
{
    m_doc->setStatus( m_oldvalue );
}

//----------------
DocumentModifySendAsCmd::DocumentModifySendAsCmd( Document *doc, Document::SendAs value, const QString& name )
    : NamedCommand( name ),
    m_doc( doc )
{
    Q_ASSERT( doc );
    m_value = value;
    m_oldvalue = doc->sendAs();
}
void DocumentModifySendAsCmd::execute()
{
    m_doc->setSendAs( m_value );
}
void DocumentModifySendAsCmd::unexecute()
{
    m_doc->setSendAs( m_oldvalue );
}

//----------------
WBSDefinitionModifyCmd::WBSDefinitionModifyCmd( Project &project, const WBSDefinition value, const QString& name )
    : NamedCommand( name ),
    m_project( project )
{
    m_newvalue = value;
    m_oldvalue = m_project.wbsDefinition();
}
void WBSDefinitionModifyCmd::execute()
{
    m_project.setWbsDefinition( m_newvalue );
}
void WBSDefinitionModifyCmd::unexecute()
{
    m_project.setWbsDefinition( m_oldvalue );
}

//----------------
InsertProjectCmd::InsertProjectCmd( Project &project, Node *parent, Node *after, const QString& name )
    : MacroCommand( name ),
    m_project( static_cast<Project*>( parent->projectNode() ) ),
    m_parent( parent )
{
    Q_ASSERT( &project != m_project );

    qDebug()<<"InsertProjectCmd: Merge calendars";
    QMap<Calendar*, Calendar*> unusedCalendars;
    foreach ( Calendar *c, project.calendars() ) {
        addCalendars( c, 0, unusedCalendars );
    }
    // fixup resources pointing to unused calendars
    foreach ( Resource *r, project.resourceList() ) {
        if ( unusedCalendars.contains( r->calendar() ) ) {
            r->setCalendar( unusedCalendars[ r->calendar() ] );
        }
    }
    qDebug()<<"InsertProjectCmd: Get requests:";
    // get all requests before resources are merged
    QMap<ResourceGroupRequest*, QPair<Node *, ResourceGroup*> > greqs;
    QMap<ResourceGroupRequest*, QPair<ResourceRequest*, Resource*> > rreqs;
    foreach ( Node *n, project.allNodes() ) {
        QList<ResourceRequest*> resReq;
        if ( n->type() != (int)Node::Type_Task || n->requests().isEmpty() ) {
            continue;
        }
        while ( ResourceGroupRequest *gr = n->requests().requests().value( 0 ) ) {
            //qDebug()<<"InsertProjectCmd: "<<gr->group()<<existingGroups;
            while ( ResourceRequest *rr = gr->resourceRequests().value( 0 ) ) {
                rreqs.insertMulti( gr, QPair<ResourceRequest*, Resource*>( rr, rr->resource() ) );
                // all resource requests shall be reinserted
                rr->unregisterRequest();
                gr->takeResourceRequest( rr );
            }
            // all group requests shall be reinserted
            greqs[ gr ] = QPair<Node*, ResourceGroup*>( n, gr->group() );
            gr->group()->unregisterRequest( gr );
            int i = n->requests().takeRequest( gr );
            Q_ASSERT( i >= 0 );
        }
        qDebug()<<"InsertProjectCmd groupRequest:"<<n->requests().requests().count();
    }
    qDebug()<<"InsertProjectCmd: Merge resources";
    //qDebug()<<"InsertProjectCmd: project groups"<<project.resourceGroups();
    //qDebug()<<"InsertProjectCmd: m_project grps"<<m_project->resourceGroups();
    QMap<ResourceGroup*, ResourceGroup*> existingGroups;
    QMap<Resource*, Resource*> existingResources;
    foreach ( ResourceGroup *g, project.resourceGroups() ) {
        ResourceGroup *gr = m_project->findResourceGroup( g->id() );
        if ( gr == 0 ) {
            //qDebug()<<"InsertProjectCmd: new group, add to m_project"<<g<<g->name();
            addCommand( new AddResourceGroupCmd( m_project, g, QString("ResourceGroup") ) );
            gr = g;
        } else {
            //qDebug()<<"InsertProjectCmd: group exists"<<g->name()<<gr->name();
            existingGroups[ gr ] = g;
        }
        foreach ( Resource *r, g->resources() ) {
            ////qDebug()<<"InsertProjectCmd: res sch"<<r->schedules();
            while ( Schedule *s = r->schedules().values().value( 0 ) ) {
                r->deleteSchedule( s ); // schedules not handled
            }
            Resource *res = m_project->findResource( r->id() );
            if ( res == 0 ) {
                //qDebug()<<"InsertProjectCmd: new resource, add to m_project"<<r<<r->name();
                addCommand( new AddResourceCmd( gr, r, "Resource" ) );
            } else {
                //qDebug()<<"InsertProjectCmd: resource exists"<<r->name()<<res->name();
                existingResources[ res ] = r;
            }
        }
    }
    qDebug()<<"InsertProjectCmd: Insert requests:";
    //qDebug()<<"InsertProjectCmd: existingGroups"<<existingGroups;
    // Requests: clean up requests to resources already in m_project
    int gi = 0;
    int ri = 0;
    foreach ( ResourceGroupRequest *gr, greqs.keys() ) {
        //qDebug()<<"InsertProjectCmd: "<<gr->group()<<existingGroups;
        QPair<Node*, ResourceGroup*> pair = greqs[ gr ];
        Node *n = pair.first;
        ResourceGroup *newGroup = pair.second;
        if ( existingGroups.values().contains( newGroup ) ) {
            newGroup = existingGroups.keys().value( existingGroups.values().indexOf( newGroup ) );
        }
        gr->setGroup( newGroup );
        addCommand( new AddResourceGroupRequestCmd( static_cast<Task&>( *n ), gr, QString("Group %1").arg( ++gi ) ) );

        QMap<ResourceGroupRequest*, QPair<ResourceRequest*, Resource*> >::const_iterator i = rreqs.constFind( gr );
        for ( ; i != rreqs.constEnd() && i.key() == gr; ++i ) {
            ResourceRequest *rr = i.value().first;
            Resource *newRes = i.value().second;
            if ( existingResources.values().contains( newRes ) ) {
                newRes = existingResources.keys().value( existingResources.values().indexOf( newRes ) );
            }
            // all resource requests shall be reinserted
            qDebug()<<"InsertProjectCmd resourceRequest:"<<n->name()<<rr->resource()->name()<<(newRes==rr->resource()?"keep":"swap");
            rr->setResource( newRes );
            addCommand( new AddResourceRequestCmd( gr, rr, QString("Resource %1").arg( ++ri ) ) );
        }
        qDebug()<<"InsertProjectCmd groupRequest:"<<n->requests().requests().count();
    }
    // Add nodes ( ids are unique, no need to check )
    Node *node_after = after;
    for ( int i = 0; i < project.numChildren(); ++i ) {
        Node *n = project.childNode( i );
        Q_ASSERT( n );
        //qDebug()<<"InsertProjectCmd: node sch"<<n->schedules();
        while ( Schedule *s = n->schedules().values().value( 0 ) ) {
            n->takeSchedule( s ); // schedules not handled
            delete s;
        }
        n->setParentNode( 0 );
        //qDebug()<<"InsertProjectCmd: add"<<project.name()<<"->"<<n->name();
        if ( node_after ) {
            addCommand( new TaskAddCmd( m_project, n, node_after, "Task" ) );
            node_after = n;
        } else {
            qDebug()<<"InsertProjectCmd: add subtask"<<parent->name()<<"->"<<n->name();
            addCommand( new SubtaskAddCmd( m_project, n, parent, "Subtask" ) );
        }
        addChildNodes( n );
    }
    // Dependencies:
    foreach ( Node *n, project.allNodes() ) {
        foreach ( Relation *r, n->dependChildNodes() ) {
            addCommand( new AddRelationCmd( *m_project, new Relation( r ) ) );
        }
    }
    // Remove nodes from project so they are not deleted
    while ( Node *ch = project.childNode( 0 ) ) {
        //qDebug()<<"InsertProjectCmd: remove"<<project.name()<<"->"<<ch->name();
        project.takeChildNode( ch );
    }
    foreach ( Node *n, project.allNodes() ) {
        project.removeId( n->id() );
    }

    // Remove calendars from project
    while ( project.calendarCount() > 0 ) {
        project.takeCalendar( project.calendarAt( 0 ) );
    }
    qDeleteAll( unusedCalendars.keys() );

    while ( project.numResourceGroups() > 0 ) {
        ResourceGroup *g = project.resourceGroupAt( 0 );
        while ( g->numResources() > 0 ) {
            g->takeResource( g->resourceAt( 0 ) );
        }
        project.takeResourceGroup( g );
    }
    qDeleteAll( existingResources ); // deletes unused resources
    qDeleteAll( existingGroups ); // deletes unused resource groups
}

void InsertProjectCmd::addCalendars( Calendar *calendar, Calendar *parent, QMap<Calendar*, Calendar*> &map ) {
    Calendar *par = 0;
    if ( parent ) {
        par = m_project->findCalendar( parent->id() );
    }
    if ( par == 0 ) {
        par = parent;
    }
    Calendar *cal = m_project->findCalendar( calendar->id() );
    if ( cal == 0 ) {
        addCommand( new CalendarAddCmd( m_project, calendar, par, "Calendar" ) );
    } else {
        map[ calendar ] = cal;
    }
    foreach ( Calendar *c, calendar->calendars() ) {
        addCalendars( c, calendar, map );
    }
}

void InsertProjectCmd::addChildNodes( Node *node ) {
    // schedules not handled
    //qDebug()<<"InsertProjectCmd: node sch"<<node->schedules();
    while ( Schedule *s = node->schedules().values().value( 0 ) ) {
        node->takeSchedule( s ); // schedules not handled
        delete s;
    }
    foreach ( Node *n, node->childNodeIterator() ) {
        n->setParentNode( 0 );
        //qDebug()<<"addChildNodes: add"<<node->name()<<"->"<<n->name();
        addCommand( new SubtaskAddCmd( m_project, n, node, "Subtask" ) );
        addChildNodes( n );
    }
    // Remove child nodes so they are not added twice
    while ( Node *ch = node->childNode( 0 ) ) {
        //qDebug()<<"addChildNodes: remove"<<node->name()<<"->"<<ch->name();
        node->takeChildNode( ch );
    }
}

void InsertProjectCmd::execute()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    MacroCommand::execute();
    QApplication::restoreOverrideCursor();
}
void InsertProjectCmd::unexecute()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    MacroCommand::unexecute();
    QApplication::restoreOverrideCursor();
}

WorkPackageAddCmd::WorkPackageAddCmd( Project *project, Node *node, WorkPackage *value, const QString& name )
    : NamedCommand( name ),
    m_project( project ),
    m_node( node ),
    m_wp( value ),
    m_mine( true )
{
}
WorkPackageAddCmd::~WorkPackageAddCmd()
{
    if ( m_mine ) {
        delete m_wp;
    }
}
void WorkPackageAddCmd::execute()
{
    // FIXME use project
    //m_project->addWorkPackage( m_node, m_wp );
    static_cast<Task*>( m_node )->addWorkPackage( m_wp );
}
void WorkPackageAddCmd::unexecute()
{
    // FIXME use project
    //m_project->removeWorkPackage( m_node, m_wp );
    static_cast<Task*>( m_node )->removeWorkPackage( m_wp );
}

ModifyProjectLocaleCmd::ModifyProjectLocaleCmd(  Project &project, const QString& name )
    : MacroCommand( name ),
    m_project( project )
{
};
void ModifyProjectLocaleCmd::execute()
{
    MacroCommand::execute();
    m_project.emitLocaleChanged();
}
void ModifyProjectLocaleCmd::unexecute()
{
    MacroCommand::unexecute();
    m_project.emitLocaleChanged();
}

ModifyCurrencySymolCmd::ModifyCurrencySymolCmd(  KLocale *locale, const QString &value, const QString& name )
    : NamedCommand( name ),
    m_locale( locale ),
    m_newvalue( value ),
    m_oldvalue( locale->currencySymbol() )
{
};
void ModifyCurrencySymolCmd::execute()
{
    m_locale->setCurrencySymbol( m_newvalue );
}
void ModifyCurrencySymolCmd::unexecute()
{
    m_locale->setCurrencySymbol( m_oldvalue );
}

ModifyCurrencyFractionalDigitsCmd::ModifyCurrencyFractionalDigitsCmd(  KLocale *locale, int value, const QString& name )
    : NamedCommand( name ),
    m_locale( locale ),
    m_newvalue( value ),
    m_oldvalue( locale->fracDigits() )
{
};
void ModifyCurrencyFractionalDigitsCmd::execute()
{
    m_locale->setFracDigits( m_newvalue );
}
void ModifyCurrencyFractionalDigitsCmd::unexecute()
{
    m_locale->setFracDigits( m_oldvalue );
}

ModifyPositivePrefixCurrencySymolCmd::ModifyPositivePrefixCurrencySymolCmd(  KLocale *locale, bool value, const QString& name )
    : NamedCommand( name ),
    m_locale( locale ),
    m_newvalue( value ),
    m_oldvalue( locale->positivePrefixCurrencySymbol() )
{
};
void ModifyPositivePrefixCurrencySymolCmd::execute()
{
    m_locale->setPositivePrefixCurrencySymbol( m_newvalue );
}
void ModifyPositivePrefixCurrencySymolCmd::unexecute()
{
    m_locale->setPositivePrefixCurrencySymbol( m_oldvalue );
}

ModifyNegativePrefixCurrencySymolCmd::ModifyNegativePrefixCurrencySymolCmd(  KLocale *locale, bool value, const QString& name )
    : NamedCommand( name ),
    m_locale( locale ),
    m_newvalue( value ),
    m_oldvalue( locale->negativePrefixCurrencySymbol() )
{
};
void ModifyNegativePrefixCurrencySymolCmd::execute()
{
    m_locale->setNegativePrefixCurrencySymbol( m_newvalue );
}
void ModifyNegativePrefixCurrencySymolCmd::unexecute()
{
    m_locale->setNegativePrefixCurrencySymbol( m_oldvalue );
}

ModifyPositiveMonetarySignPositionCmd ::ModifyPositiveMonetarySignPositionCmd (  KLocale *locale, int value, const QString& name )
    : NamedCommand( name ),
    m_locale( locale ),
    m_newvalue( value ),
    m_oldvalue( locale->positiveMonetarySignPosition() )
{
};
void ModifyPositiveMonetarySignPositionCmd ::execute()
{
    m_locale->setPositiveMonetarySignPosition( (KLocale::SignPosition)m_newvalue );
}
void ModifyPositiveMonetarySignPositionCmd ::unexecute()
{
    m_locale->setPositiveMonetarySignPosition( (KLocale::SignPosition)m_oldvalue );
}

ModifyNegativeMonetarySignPositionCmd ::ModifyNegativeMonetarySignPositionCmd (  KLocale *locale, int value, const QString& name )
    : NamedCommand( name ),
    m_locale( locale ),
    m_newvalue( value ),
    m_oldvalue( locale->negativeMonetarySignPosition() )
{
};
void ModifyNegativeMonetarySignPositionCmd ::execute()
{
    m_locale->setNegativeMonetarySignPosition( (KLocale::SignPosition)m_newvalue );
}
void ModifyNegativeMonetarySignPositionCmd ::unexecute()
{
    m_locale->setNegativeMonetarySignPosition( (KLocale::SignPosition)m_oldvalue );
}

}  //KPlato namespace
