/* This file is part of the KDE project
  Copyright (C) 2004 - 2007 Dag Andersen <danders@get2net.dk>
 Copyright (C) 2011 Dag Andersen <danders@get2net.dk>

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
#include <kptdebug.h>

#include <klocale.h>
#include <kdeversion.h>

#include <QHash>
#include <QMap>
#include <QApplication>


static int insertProjectCmdDba() {
#if KDE_IS_VERSION( 4, 3, 80 )
    static int s_area = KDebug::registerArea( "plan (InsertProjectCmd)" );
#else
    static int s_area = 0;
#endif
    return s_area;
}

namespace KPlato
{

void NamedCommand::setSchScheduled()
{
    QMap<Schedule*, bool>::Iterator it;
    for ( it = m_schedules.begin(); it != m_schedules.end(); ++it ) {
        //kDebug(planDbg()) << it.key() ->name() <<":" << it.value();
        it.key() ->setScheduled( it.value() );
    }
}
void NamedCommand::setSchScheduled( bool state )
{
    QMap<Schedule*, bool>::Iterator it;
    for ( it = m_schedules.begin(); it != m_schedules.end(); ++it ) {
        //kDebug(planDbg()) << it.key() ->name() <<":" << state;
        it.key() ->setScheduled( state );
    }
}
void NamedCommand::addSchScheduled( Schedule *sch )
{
    //kDebug(planDbg()) << sch->id() <<":" << sch->isScheduled();
    m_schedules.insert( sch, sch->isScheduled() );
    foreach ( Appointment * a, sch->appointments() ) {
        if ( a->node() == sch ) {
            m_schedules.insert( a->resource(), a->resource() ->isScheduled() );
        } else if ( a->resource() == sch ) {
            m_schedules.insert( a->node(), a->node() ->isScheduled() );
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

void MacroCommand::addCommand( KUndo2Command *cmd )
{
    cmds.append( cmd );
}

void MacroCommand::execute()
{
    foreach ( KUndo2Command *c, cmds ) {
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
CalendarAddCmd::CalendarAddCmd( Project *project, Calendar *cal, int pos, Calendar *parent, const QString& name )
        : NamedCommand( name ),
        m_project( project ),
        m_cal( cal ),
        m_pos( pos ),
        m_parent( parent ),
        m_mine( true )
{
    //kDebug(planDbg())<<cal->name();
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
        m_project->addCalendar( m_cal, m_parent, m_pos );
        m_mine = false;
    }

    //kDebug(planDbg())<<m_cal->name()<<" added to:"<<m_project->name();
}

void CalendarAddCmd::unexecute()
{
    if ( m_project ) {
        m_project->takeCalendar( m_cal );
        m_mine = true;
    }

    //kDebug(planDbg())<<m_cal->name();
}

CalendarRemoveCmd::CalendarRemoveCmd( Project *project, Calendar *cal, const QString& name )
        : NamedCommand( name ),
        m_project( project ),
        m_parent( cal->parentCal() ),
        m_cal( cal ),
        m_index( -1 ),
        m_mine( false ),
        m_cmd( new MacroCommand("") )
{
    Q_ASSERT( project != 0 );

    m_index = m_parent ? m_parent->indexOf( cal ) : project->indexOf( cal );

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
    m_cmd->execute();
    m_project->takeCalendar( m_cal );
    m_mine = true;

}
void CalendarRemoveCmd::unexecute()
{
    m_project->addCalendar( m_cal, m_parent, m_index );
    m_cmd->unexecute();
    m_mine = false;

}

CalendarMoveCmd::CalendarMoveCmd( Project *project, Calendar *cal, int position, Calendar *parent, const QString& name )
        : NamedCommand( name ),
        m_project( project ),
        m_cal( cal ),
        m_newpos( position ),
        m_newparent( parent ),
        m_oldparent( cal->parentCal() )
{
    //kDebug(planDbg())<<cal->name();
    Q_ASSERT( project != 0 );

    m_oldpos = m_oldparent ? m_oldparent->indexOf( cal ) : project->indexOf( cal );
}
void CalendarMoveCmd::execute()
{
    m_project->takeCalendar( m_cal );
    m_project->addCalendar( m_cal, m_newparent, m_newpos );
}

void CalendarMoveCmd::unexecute()
{
    m_project->takeCalendar( m_cal );
    m_project->addCalendar( m_cal, m_oldparent, m_oldpos );
}

CalendarModifyNameCmd::CalendarModifyNameCmd( Calendar *cal, const QString& newvalue, const QString& name )
        : NamedCommand( name ),
        m_cal( cal )
{

    m_oldvalue = cal->name();
    m_newvalue = newvalue;
    //kDebug(planDbg())<<cal->name();
}
void CalendarModifyNameCmd::execute()
{
    m_cal->setName( m_newvalue );

    //kDebug(planDbg())<<m_cal->name();
}
void CalendarModifyNameCmd::unexecute()
{
    m_cal->setName( m_oldvalue );

    //kDebug(planDbg())<<m_cal->name();
}

CalendarModifyParentCmd::CalendarModifyParentCmd( Project *project, Calendar *cal, Calendar *newvalue, const QString& name )
        : NamedCommand( name ),
        m_project( project ),
        m_cal( cal ),
        m_cmd( new MacroCommand( "" ) ),
        m_oldindex( -1 ),
        m_newindex( -1 )
{
    m_oldvalue = cal->parentCal();
    m_newvalue = newvalue;
    m_oldindex = m_oldvalue ? m_oldvalue->indexOf( cal ) : m_project->indexOf( cal );

    if ( newvalue ) {
        m_cmd->addCommand( new CalendarModifyTimeZoneCmd( cal, newvalue->timeZone() ) );
    }
    //kDebug(planDbg())<<cal->name();
}
CalendarModifyParentCmd::~CalendarModifyParentCmd()
{
    delete m_cmd;
}
void CalendarModifyParentCmd::execute()
{
    m_project->takeCalendar( m_cal );
    m_project->addCalendar( m_cal, m_newvalue, m_newindex );
    m_cmd->execute();
}
void CalendarModifyParentCmd::unexecute()
{
    m_cmd->unexecute();
    m_project->takeCalendar( m_cal );
    m_project->addCalendar( m_cal, m_oldvalue, m_oldindex );
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
    //kDebug(planDbg())<<cal->name();
}
CalendarModifyTimeZoneCmd::~CalendarModifyTimeZoneCmd()
{
    delete m_cmd;
}
void CalendarModifyTimeZoneCmd::execute()
{
    m_cmd->execute();
    m_cal->setTimeZone( m_newvalue );
}
void CalendarModifyTimeZoneCmd::unexecute()
{
    m_cal->setTimeZone( m_oldvalue );
    m_cmd->unexecute();
}

CalendarAddDayCmd::CalendarAddDayCmd( Calendar *cal, CalendarDay *newvalue, const QString& name )
        : NamedCommand( name ),
        m_cal( cal ),
        m_mine( true )
{

    m_newvalue = newvalue;
    //kDebug(planDbg())<<cal->name();
}
CalendarAddDayCmd::~CalendarAddDayCmd()
{
    //kDebug(planDbg());
    if ( m_mine )
        delete m_newvalue;
}
void CalendarAddDayCmd::execute()
{
    //kDebug(planDbg())<<m_cal->name();
    m_cal->addDay( m_newvalue );
    m_mine = false;
}
void CalendarAddDayCmd::unexecute()
{
    //kDebug(planDbg())<<m_cal->name();
    m_cal->takeDay( m_newvalue );
    m_mine = true;
}

CalendarRemoveDayCmd::CalendarRemoveDayCmd( Calendar *cal,CalendarDay *day, const QString& name )
        : NamedCommand( name ),
        m_cal( cal ),
        m_value( day ),
        m_mine( false )
{
    //kDebug(planDbg())<<cal->name();
    // TODO check if any resources uses this calendar
    init();
}
CalendarRemoveDayCmd::CalendarRemoveDayCmd( Calendar *cal, const QDate &day, const QString& name )
        : NamedCommand( name ),
        m_cal( cal ),
        m_mine( false )
{

    m_value = cal->findDay( day );
    //kDebug(planDbg())<<cal->name();
    // TODO check if any resources uses this calendar
    init();
}
void CalendarRemoveDayCmd::init()
{
}
void CalendarRemoveDayCmd::execute()
{
    //kDebug(planDbg())<<m_cal->name();
    m_cal->takeDay( m_value );
    m_mine = true;
}
void CalendarRemoveDayCmd::unexecute()
{
    //kDebug(planDbg())<<m_cal->name();
    m_cal->addDay( m_value );
    m_mine = false;
}

CalendarModifyDayCmd::CalendarModifyDayCmd( Calendar *cal, CalendarDay *value, const QString& name )
        : NamedCommand( name ),
        m_cal( cal ),
        m_mine( true )
{

    m_newvalue = value;
    m_oldvalue = cal->findDay( value->date() );
    //kDebug(planDbg())<<cal->name()<<" old:("<<m_oldvalue<<") new:("<<m_newvalue<<")";
}
CalendarModifyDayCmd::~CalendarModifyDayCmd()
{
    //kDebug(planDbg());
    if ( m_mine ) {
        delete m_newvalue;
    } else {
        delete m_oldvalue;
    }
}
void CalendarModifyDayCmd::execute()
{
    //kDebug(planDbg());
    if ( m_oldvalue ) {
        m_cal->takeDay( m_oldvalue );
    }
    m_cal->addDay( m_newvalue );
    m_mine = false;
}
void CalendarModifyDayCmd::unexecute()
{
    //kDebug(planDbg());
    m_cal->takeDay( m_newvalue );
    if ( m_oldvalue ) {
        m_cal->addDay( m_oldvalue );
    }
    m_mine = true;
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
        foreach ( TimeInterval *ti, day->timeIntervals() ) {
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
    //kDebug(planDbg());
    m_cmd->execute();
    m_calendar->setState( m_day, m_newvalue );

}
void CalendarModifyStateCmd::unexecute()
{
    //kDebug(planDbg());
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
    //kDebug(planDbg());
    m_calendar->setWorkInterval( m_value, m_newvalue );

}
void CalendarModifyTimeIntervalCmd::unexecute()
{
    //kDebug(planDbg());
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
    //kDebug(planDbg());
    m_calendar->addWorkInterval( m_day, m_value );
    m_mine = false;

}
void CalendarAddTimeIntervalCmd::unexecute()
{
    //kDebug(planDbg());
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

    //kDebug(planDbg()) << cal->name() <<" (" << value <<")";
}
CalendarModifyWeekdayCmd::~CalendarModifyWeekdayCmd()
{
    //kDebug(planDbg()) << m_weekday <<":" << m_value;
    delete m_value;

}
void CalendarModifyWeekdayCmd::execute()
{
    m_cal->setWeekday( m_weekday, *m_value );
}
void CalendarModifyWeekdayCmd::unexecute()
{
    m_cal->setWeekday( m_weekday, m_orig );
}

CalendarModifyDateCmd::CalendarModifyDateCmd( Calendar *cal, CalendarDay *day, const QDate &value, const QString& name )
    : NamedCommand( name ),
    m_cal( cal ),
    m_day( day ),
    m_newvalue( value ),
    m_oldvalue( day->date() )
{
    //kDebug(planDbg()) << cal->name() <<" (" << value <<")";
}
void CalendarModifyDateCmd::execute()
{
    m_cal->setDate( m_day, m_newvalue );
}
void CalendarModifyDateCmd::unexecute()
{
    m_cal->setDate( m_day, m_oldvalue );
}

ProjectModifyDefaultCalendarCmd::ProjectModifyDefaultCalendarCmd( Project *project, Calendar *cal, const QString& name )
    : NamedCommand( name ),
    m_project( project ),
    m_newvalue( cal ),
    m_oldvalue( project->defaultCalendar() )
{
    //kDebug(planDbg()) << cal->name() <<" (" << value <<")";
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
        //kDebug(planDbg())<<m_node->name()<<""<<m_index;
        if ( m_relCmd.isEmpty() ) {
            // Only add delete relation commands if we (still) have relations
            // The other node might have deleted them...
            foreach ( Relation * r, m_node->dependChildNodes() ) {
                m_relCmd.addCommand( new DeleteRelationCmd( *m_project, r ) );
            }
            foreach ( Relation * r, m_node->dependParentNodes() ) {
                m_relCmd.addCommand( new DeleteRelationCmd( *m_project, r ) );
            }
        }
        m_relCmd.execute();
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
        //kDebug(planDbg())<<m_node->name()<<""<<m_index;
        m_project->addSubTask( m_node, m_index, m_parent );
        if ( m_cmd ) {
            m_cmd->unexecute();
        }
        m_relCmd.unexecute();
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
    //kDebug(planDbg())<<m_node->name();
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
    // Also remove accounts
    if ( parent->runningAccount() ) {
        if ( m_cmd == 0 ) m_cmd = new MacroCommand( "" );
        m_cmd->addCommand( new NodeModifyRunningAccountCmd( *parent, parent->runningAccount(), 0 ) );
    }
    if ( parent->startupAccount() ) {
        if ( m_cmd == 0 ) m_cmd = new MacroCommand( "" );
        m_cmd->addCommand( new NodeModifyStartupAccountCmd( *parent, parent->startupAccount(), 0 ) );
    }
    if ( parent->shutdownAccount() ) {
        if ( m_cmd == 0 ) m_cmd = new MacroCommand( "" );
        m_cmd->addCommand( new NodeModifyShutdownAccountCmd( *parent, parent->shutdownAccount(), 0 ) );
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
}
void NodeModifyConstraintCmd::execute()
{
    m_node.setConstraint( newConstraint );
}
void NodeModifyConstraintCmd::unexecute()
{
    m_node.setConstraint( oldConstraint );
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
}
void NodeModifyConstraintEndTimeCmd::unexecute()
{
    m_node.setConstraintEndTime( oldTime );
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
            // Also remove accounts
            if ( m_newparent->runningAccount() ) {
                if ( m_cmd == 0 ) m_cmd = new MacroCommand( "" );
                m_cmd->addCommand( new NodeModifyRunningAccountCmd( *m_newparent, m_newparent->runningAccount(), 0 ) );
            }
            if ( m_newparent->startupAccount() ) {
                if ( m_cmd == 0 ) m_cmd = new MacroCommand( "" );
                m_cmd->addCommand( new NodeModifyStartupAccountCmd( *m_newparent, m_newparent->startupAccount(), 0 ) );
            }
            if ( m_newparent->shutdownAccount() ) {
                if ( m_cmd == 0 ) m_cmd = new MacroCommand( "" );
                m_cmd->addCommand( new NodeModifyShutdownAccountCmd( *m_newparent, m_newparent->shutdownAccount(), 0 ) );
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
    m_moved( false )
{
    m_oldparent = node->parentNode();
    Q_ASSERT( m_oldparent );
}
void NodeMoveCmd::execute()
{
    if ( m_project ) {
        m_oldpos = m_oldparent->indexOf( m_node );
        m_moved = m_project->moveTask( m_node, m_newparent, m_newpos );
        if ( m_moved ) {
            if ( m_cmd.isEmpty() ) {
                // Summarytasks can't have resources, so remove resource requests from the new parent
                foreach ( ResourceGroupRequest *r, m_newparent->requests().requests() ) {
                    m_cmd.addCommand( new RemoveResourceGroupRequestCmd( r ) );
                }
                // TODO appointments ??
            }
            m_cmd.execute();
        }
    }
}
void NodeMoveCmd::unexecute()
{
    if ( m_project && m_moved ) {
        m_moved = m_project->moveTask( m_node, m_oldparent, m_oldpos );
        m_cmd.unexecute();
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
    //kDebug(planDbg())<<m_rel->parent()<<" to"<<m_rel->child();
    m_taken = false;
    m_project.addRelation( m_rel, false );
}
void AddRelationCmd::unexecute()
{
    m_taken = true;
    m_project.takeRelation( m_rel );
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
    //kDebug(planDbg())<<m_rel->parent()<<" to"<<m_rel->child();
    m_taken = true;
    m_project.takeRelation( m_rel );
}
void DeleteRelationCmd::unexecute()
{
    m_taken = false;
    m_project.addRelation( m_rel, false );
}

ModifyRelationTypeCmd::ModifyRelationTypeCmd( Relation *rel, Relation::Type type, const QString& name )
        : NamedCommand( name ),
        m_rel( rel ),
        m_newtype( type )
{

    m_oldtype = rel->type();
    m_project = dynamic_cast<Project*>( rel->parent() ->projectNode() );
}
void ModifyRelationTypeCmd::execute()
{
    if ( m_project ) {
        m_project->setRelationType( m_rel, m_newtype );
    }
}
void ModifyRelationTypeCmd::unexecute()
{
    if ( m_project ) {
        m_project->setRelationType( m_rel, m_oldtype );
    }
}

ModifyRelationLagCmd::ModifyRelationLagCmd( Relation *rel, Duration lag, const QString& name )
        : NamedCommand( name ),
        m_rel( rel ),
        m_newlag( lag )
{

    m_oldlag = rel->lag();
    m_project = dynamic_cast<Project*>( rel->parent() ->projectNode() );
}
void ModifyRelationLagCmd::execute()
{
    if ( m_project ) {
        m_project->setRelationLag( m_rel, m_newlag );
    }
}
void ModifyRelationLagCmd::unexecute()
{
    if ( m_project ) {
        m_project->setRelationLag( m_rel, m_oldlag );
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
    //kDebug(planDbg())<<"group="<<m_group<<" req="<<m_request;
    m_group->addResourceRequest( m_request );
    m_mine = false;
}
void AddResourceRequestCmd::unexecute()
{
    //kDebug(planDbg())<<"group="<<m_group<<" req="<<m_request;
    m_group->takeResourceRequest( m_request );
    m_mine = true;
}

RemoveResourceRequestCmd::RemoveResourceRequestCmd( ResourceGroupRequest *group, ResourceRequest *request, const QString& name )
        : NamedCommand( name ),
        m_group( group ),
        m_request( request )
{

    m_mine = false;
    //kDebug(planDbg())<<"group req="<<group<<" req="<<request<<" to gr="<<m_group->group();
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
}
void RemoveResourceRequestCmd::unexecute()
{
    m_group->addResourceRequest( m_request );
    m_mine = false;
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

ModifyResourceRequestRequiredCmd::ModifyResourceRequestRequiredCmd( ResourceRequest *request, const QList<Resource*> &value, const QString& name )
    : NamedCommand( name ),
    m_request( request ),
    m_newvalue( value )
{
    m_oldvalue = request->requiredResources();
}
void ModifyResourceRequestRequiredCmd::execute()
{
    m_request->setRequiredResources( m_newvalue );
}
void ModifyResourceRequestRequiredCmd::unexecute()
{
    m_request->setRequiredResources( m_oldvalue );
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
    m_optimistic( node.estimate()->optimisticRatio() ),
    m_pessimistic( node.estimate()->pessimisticRatio() ),
    m_cmd( 0 )
{
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
    m_estimate->setExpectedEstimate( m_newvalue );
    if ( m_cmd ) {
        m_cmd->execute();
    }
    m_estimate->setPessimisticRatio( m_pessimistic );
    m_estimate->setOptimisticRatio( m_optimistic );
}
void ModifyEstimateCmd::unexecute()
{
    m_estimate->setExpectedEstimate( m_oldvalue );
    if ( m_cmd ) {
        m_cmd->unexecute();
    }
    m_estimate->setPessimisticRatio( m_pessimistic );
    m_estimate->setOptimisticRatio( m_optimistic );
}

EstimateModifyOptimisticRatioCmd::EstimateModifyOptimisticRatioCmd( Node &node, int oldvalue, int newvalue, const QString& name )
        : NamedCommand( name ),
        m_estimate( node.estimate() ),
        m_oldvalue( oldvalue ),
        m_newvalue( newvalue )
{
}
void EstimateModifyOptimisticRatioCmd::execute()
{
    m_estimate->setOptimisticRatio( m_newvalue );
}
void EstimateModifyOptimisticRatioCmd::unexecute()
{
    m_estimate->setOptimisticRatio( m_oldvalue );
}

EstimateModifyPessimisticRatioCmd::EstimateModifyPessimisticRatioCmd( Node &node, int oldvalue, int newvalue, const QString& name )
        : NamedCommand( name ),
        m_estimate( node.estimate() ),
        m_oldvalue( oldvalue ),
        m_newvalue( newvalue )
{
}
void EstimateModifyPessimisticRatioCmd::execute()
{
    m_estimate->setPessimisticRatio( m_newvalue );
}
void EstimateModifyPessimisticRatioCmd::unexecute()
{
    m_estimate->setPessimisticRatio( m_oldvalue );
}

ModifyEstimateTypeCmd::ModifyEstimateTypeCmd( Node &node, int oldvalue, int newvalue, const QString& name )
        : NamedCommand( name ),
        m_estimate( node.estimate() ),
        m_oldvalue( oldvalue ),
        m_newvalue( newvalue )
{
}
void ModifyEstimateTypeCmd::execute()
{
    m_estimate->setType( static_cast<Estimate::Type>( m_newvalue ) );
}
void ModifyEstimateTypeCmd::unexecute()
{
    m_estimate->setType( static_cast<Estimate::Type>( m_oldvalue ) );
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
}
void EstimateModifyRiskCmd::execute()
{
    m_estimate->setRisktype( static_cast<Estimate::Risktype>( m_newvalue ) );
}
void EstimateModifyRiskCmd::unexecute()
{
    m_estimate->setRisktype( static_cast<Estimate::Risktype>( m_oldvalue ) );
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
    //kDebug(planDbg())<<"group="<<m_request;
    m_task.addRequest( m_request );
    m_mine = false;


}
void AddResourceGroupRequestCmd::unexecute()
{
    //kDebug(planDbg())<<"group="<<m_request;
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
    //kDebug(planDbg())<<"group="<<m_request;
    m_task.takeRequest( m_request ); // group should now be empty of resourceRequests
    m_mine = true;


}
void RemoveResourceGroupRequestCmd::unexecute()
{
    //kDebug(planDbg())<<"group="<<m_request;
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
        //kDebug(planDbg())<<"delete:"<<m_resource;
        delete m_resource;
    }
}
void AddResourceCmd::execute()
{
    Q_ASSERT( m_group->project() );
    if ( m_group->project() ) {
        m_group->project()->addResource( m_group, m_resource, m_index );
        m_mine = false;
        //kDebug(planDbg())<<"added:"<<m_resource;
    }

}
void AddResourceCmd::unexecute()
{
    Q_ASSERT( m_group->project() );
    if ( m_group->project() ) {
        m_group->project()->takeResource( m_group, m_resource );
        //kDebug(planDbg())<<"removed:"<<m_resource;
        m_mine = true;
    }

    Q_ASSERT( m_group->project() );
}

RemoveResourceCmd::RemoveResourceCmd( ResourceGroup *group, Resource *resource, const QString& name )
        : AddResourceCmd( group, resource, name )
{
    //kDebug(planDbg())<<resource;
    m_mine = false;
    m_requests = m_resource->requests();

    if ( group->project() ) {
        foreach ( Schedule * s, group->project()->schedules() ) {
            Schedule *rs = resource->findSchedule( s->id() );
            if ( rs && ! rs->isDeleted() ) {
                kDebug(planDbg())<<s->name();
                addSchScheduled( s );
            }
        }
    }
    if ( resource->account() ) {
        m_cmd.addCommand( new ResourceModifyAccountCmd( *resource, resource->account(), 0 ) );
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
        //kDebug(planDbg())<<"Remove request for"<<r->resource()->name();
    }
    AddResourceCmd::unexecute();
    m_cmd.execute();
    setSchScheduled( false );
}
void RemoveResourceCmd::unexecute()
{
    foreach ( ResourceRequest * r, m_requests ) {
        r->parent() ->addResourceRequest( r );
        //kDebug(planDbg())<<"Add request for"<<r->resource()->name();
    }
    m_cmd.unexecute();
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
ModifyResourceAutoAllocateCmd::ModifyResourceAutoAllocateCmd( Resource *resource,bool value, const QString& name )
    : NamedCommand( name ),
    m_resource( resource ),
    m_newvalue( value )
{
    m_oldvalue = resource->autoAllocate();
}
void ModifyResourceAutoAllocateCmd::execute()
{
    m_resource->setAutoAllocate( m_newvalue );
}
void ModifyResourceAutoAllocateCmd::unexecute()
{
    m_resource->setAutoAllocate( m_oldvalue );
}
ModifyResourceTypeCmd::ModifyResourceTypeCmd( Resource *resource, int value, const QString& name )
        : NamedCommand( name ),
        m_resource( resource ),
        m_newvalue( value )
{
    m_oldvalue = resource->type();
}
void ModifyResourceTypeCmd::execute()
{
    m_resource->setType( ( Resource::Type ) m_newvalue );
}
void ModifyResourceTypeCmd::unexecute()
{
    m_resource->setType( ( Resource::Type ) m_oldvalue );
}
ModifyResourceUnitsCmd::ModifyResourceUnitsCmd( Resource *resource, int value, const QString& name )
        : NamedCommand( name ),
        m_resource( resource ),
        m_newvalue( value )
{
    m_oldvalue = resource->units();
}
void ModifyResourceUnitsCmd::execute()
{
    m_resource->setUnits( m_newvalue );
}
void ModifyResourceUnitsCmd::unexecute()
{
    m_resource->setUnits( m_oldvalue );
}

ModifyResourceAvailableFromCmd::ModifyResourceAvailableFromCmd( Resource *resource, const QDateTime& value, const QString& name )
        : NamedCommand( name ),
        m_resource( resource ),
        m_newvalue( value )
{
    m_oldvalue = resource->availableFrom();
    m_spec = resource->timeSpec();
}
void ModifyResourceAvailableFromCmd::execute()
{
    m_resource->setAvailableFrom( DateTime( m_newvalue, m_spec ) );
}
void ModifyResourceAvailableFromCmd::unexecute()
{
    m_resource->setAvailableFrom( m_oldvalue );
}

ModifyResourceAvailableUntilCmd::ModifyResourceAvailableUntilCmd( Resource *resource, const QDateTime& value, const QString& name )
        : NamedCommand( name ),
        m_resource( resource ),
        m_newvalue( value )
{
    m_oldvalue = resource->availableUntil();
    m_spec = resource->timeSpec();
}
void ModifyResourceAvailableUntilCmd::execute()
{
    m_resource->setAvailableUntil( DateTime( m_newvalue, m_spec ) );
}
void ModifyResourceAvailableUntilCmd::unexecute()
{
    m_resource->setAvailableUntil( m_oldvalue );
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
}
void ModifyResourceCalendarCmd::execute()
{
    m_resource->setCalendar( m_newvalue );
}
void ModifyResourceCalendarCmd::unexecute()
{
    m_resource->setCalendar( m_oldvalue );
}

ModifyRequiredResourcesCmd::ModifyRequiredResourcesCmd( Resource *resource, const QStringList &value, const QString& name )
        : NamedCommand( name ),
        m_resource( resource ),
        m_newvalue( value )
{
    m_oldvalue = resource->requiredIds();
}
void ModifyRequiredResourcesCmd::execute()
{
    m_resource->setRequiredIds( m_newvalue );
}
void ModifyRequiredResourcesCmd::unexecute()
{
    m_resource->setRequiredIds( m_oldvalue );
}

AddResourceTeamCmd::AddResourceTeamCmd( Resource *team, const QString &member, const QString& name )
    : NamedCommand( name ),
    m_team( team ),
    m_member( member )
{
}
void AddResourceTeamCmd::execute()
{
    m_team->addTeamMemberId( m_member );
}
void AddResourceTeamCmd::unexecute()
{
    m_team->removeTeamMemberId( m_member );
}

RemoveResourceTeamCmd::RemoveResourceTeamCmd( Resource *team, const QString &member, const QString& name )
    : NamedCommand( name ),
    m_team( team ),
    m_member( member )
{
}
void RemoveResourceTeamCmd::execute()
{
    m_team->removeTeamMemberId( m_member );
}
void RemoveResourceTeamCmd::unexecute()
{
    m_team->addTeamMemberId( m_member );
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
    kDebug(planDbg())<<m_mine<<value;
    if ( m_mine )
        delete value;
}
void RemoveCompletionEntryCmd::execute()
{
    if ( ! m_completion.entries().contains( m_date ) ) {
        kWarning()<<"Completion entries does not contain date:"<<m_date;
    }
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

AddCompletionActualEffortCmd::AddCompletionActualEffortCmd( Completion::UsedEffort &ue, const QDate &date, const Completion::UsedEffort::ActualEffort &value, const QString& name )
        : NamedCommand( name ),
        m_usedEffort( ue ),
        m_date( date ),
        newvalue( value )
{
    oldvalue = ue.effort( date );
}
AddCompletionActualEffortCmd::~AddCompletionActualEffortCmd()
{
}
void AddCompletionActualEffortCmd::execute()
{
    m_usedEffort.takeEffort( m_date );
    if ( newvalue.effort() > 0 ) {
        m_usedEffort.setEffort( m_date, newvalue );
    }

}
void AddCompletionActualEffortCmd::unexecute()
{
    m_usedEffort.takeEffort( m_date );
    if ( oldvalue.effort() > 0 ) {
        m_usedEffort.setEffort( m_date, oldvalue );
    }
}

AddAccountCmd::AddAccountCmd( Project &project, Account *account, const QString& parent, int index, const QString& name )
        : NamedCommand( name ),
        m_project( project ),
        m_account( account ),
        m_parent( 0 ),
        m_index( index ),
        m_parentName( parent )
{
    m_mine = true;
}

AddAccountCmd::AddAccountCmd( Project &project, Account *account, Account *parent, int index, const QString& name )
        : NamedCommand( name ),
        m_project( project ),
        m_account( account ),
        m_parent( parent ),
        m_index( index )
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
    m_project.accounts().insert( m_account, m_parent, m_index );


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
    m_oldvalue = node.shutdownCost();
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
    //kDebug(planDbg());
}
void NodeModifyRunningAccountCmd::execute()
{
    //kDebug(planDbg());
    if ( m_oldvalue ) {
        m_oldvalue->removeRunning( m_node );
    }
    if ( m_newvalue ) {
        m_newvalue->addRunning( m_node );
    }

}
void NodeModifyRunningAccountCmd::unexecute()
{
    //kDebug(planDbg());
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
    //kDebug(planDbg());
}

void NodeModifyStartupAccountCmd::execute()
{
    //kDebug(planDbg());
    if ( m_oldvalue ) {
        m_oldvalue->removeStartup( m_node );
    }
    if ( m_newvalue ) {
        m_newvalue->addStartup( m_node );
    }

}
void NodeModifyStartupAccountCmd::unexecute()
{
    //kDebug(planDbg());
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
    //kDebug(planDbg());
}

void NodeModifyShutdownAccountCmd::execute()
{
    //kDebug(planDbg());
    if ( m_oldvalue ) {
        m_oldvalue->removeShutdown( m_node );
    }
    if ( m_newvalue ) {
        m_newvalue->addShutdown( m_node );
    }

}
void NodeModifyShutdownAccountCmd::unexecute()
{
    //kDebug(planDbg());
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
    //kDebug(planDbg());
}

void ModifyDefaultAccountCmd::execute()
{
    //kDebug(planDbg());
    m_accounts.setDefaultAccount( m_newvalue );

}
void ModifyDefaultAccountCmd::unexecute()
{
    //kDebug(planDbg());
    m_accounts.setDefaultAccount( m_oldvalue );

}

ResourceModifyAccountCmd::ResourceModifyAccountCmd( Resource &resource,  Account *oldvalue, Account *newvalue, const QString& name )
    : NamedCommand( name ),
    m_resource( resource )
{
    m_oldvalue = oldvalue;
    m_newvalue = newvalue;
}
void ResourceModifyAccountCmd::execute()
{
    //kDebug(planDbg());
    if ( m_oldvalue ) {
        m_oldvalue->removeRunning( m_resource );
    }
    if ( m_newvalue ) {
        m_newvalue->addRunning( m_resource );
    }
}
void ResourceModifyAccountCmd::unexecute()
{
    //kDebug(planDbg());
    if ( m_newvalue ) {
        m_newvalue->removeRunning( m_resource );
    }
    if ( m_oldvalue ) {
        m_oldvalue->addRunning( m_resource );
    }
}

ProjectModifyConstraintCmd::ProjectModifyConstraintCmd( Project &node, Node::ConstraintType c, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        newConstraint( c ),
        oldConstraint( static_cast<Node::ConstraintType>( node.constraint() ) )
{
}
void ProjectModifyConstraintCmd::execute()
{
    m_node.setConstraint( newConstraint );
}
void ProjectModifyConstraintCmd::unexecute()
{
    m_node.setConstraint( oldConstraint );
}

ProjectModifyStartTimeCmd::ProjectModifyStartTimeCmd( Project &node, const QDateTime& dt, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        newTime( dt ),
        oldTime( node.startTime() )
{
    m_spec = node.timeSpec();
}

void ProjectModifyStartTimeCmd::execute()
{
    m_node.setConstraintStartTime( DateTime( newTime, m_spec ) );
}
void ProjectModifyStartTimeCmd::unexecute()
{
    m_node.setConstraintStartTime( oldTime );
}

ProjectModifyEndTimeCmd::ProjectModifyEndTimeCmd( Project &node, const QDateTime& dt, const QString& name )
        : NamedCommand( name ),
        m_node( node ),
        newTime( dt ),
        oldTime( node.endTime() )
{
    m_spec = node.timeSpec();
}
void ProjectModifyEndTimeCmd::execute()
{
    m_node.setEndTime( DateTime( newTime, m_spec ) );
    m_node.setConstraintEndTime( DateTime( newTime, m_spec ) );
}
void ProjectModifyEndTimeCmd::unexecute()
{
    m_node.setConstraintEndTime( oldTime );
}

//----------------------------
AddScheduleManagerCmd::AddScheduleManagerCmd( Project &node, ScheduleManager *sm, int index, const QString& name )
    : NamedCommand( name ),
    m_node( node ),
    m_parent( sm->parentManager() ),
    m_sm( sm ),
    m_index( index ),
    m_exp( sm->expected() ),
    m_mine( true)
{
}

AddScheduleManagerCmd::AddScheduleManagerCmd( ScheduleManager *parent, ScheduleManager *sm, int index, const QString& name )
    : NamedCommand( name ),
    m_node( parent->project() ),
    m_parent( parent ),
    m_sm( sm ),
    m_index( index ),
    m_exp( sm->expected() ),
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
    m_node.addScheduleManager( m_sm, m_parent, m_index );
    m_sm->setExpected( m_exp );
    m_mine = false;
}

void AddScheduleManagerCmd::unexecute()
{
    m_node.takeScheduleManager( m_sm );
    m_sm->setExpected( 0 );
    m_mine = true;
}

DeleteScheduleManagerCmd::DeleteScheduleManagerCmd( Project &node, ScheduleManager *sm, const QString& name )
    : AddScheduleManagerCmd( node, sm, -1, name )
{
    m_mine = false;
    m_index = m_parent ? m_parent->indexOf( sm ) : node.indexOf( sm );
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

MoveScheduleManagerCmd::MoveScheduleManagerCmd( ScheduleManager *sm, ScheduleManager *newparent, int newindex, const QString& name )
    : NamedCommand( name ),
    m_sm( sm ),
    m_oldparent( sm->parentManager() ),
    m_newparent( newparent ),
    m_newindex( newindex )
{
    m_oldindex = sm->parentManager() ? sm->parentManager()->indexOf( sm ) : sm->project().indexOf( sm );
}

void MoveScheduleManagerCmd::execute()
{
    m_sm->project().moveScheduleManager( m_sm, m_newparent, m_newindex );
}

void MoveScheduleManagerCmd::unexecute()
{
    m_sm->project().moveScheduleManager( m_sm, m_oldparent, m_oldindex );
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

CalculateScheduleCmd::CalculateScheduleCmd( Project &node, ScheduleManager *sm, const QString& name )
    : NamedCommand( name ),
    m_node( node ),
    m_sm( sm ),
    m_first( true ),
    m_oldexpected( m_sm->expected() ),
    m_newexpected( 0 )
{
}

void CalculateScheduleCmd::execute()
{
    Q_ASSERT( m_sm );
    if ( m_first ) {
        m_sm->calculateSchedule();
        if ( m_sm->calculationResult() != ScheduleManager::CalculationCanceled ) {
            m_first = false;
        }
        m_newexpected = m_sm->expected();
    } else {
        m_sm->setExpected( m_newexpected );
    }
}

void CalculateScheduleCmd::unexecute()
{
    if ( m_sm->scheduling() ) {
        // terminate scheduling
        QApplication::setOverrideCursor( Qt::WaitCursor );
        m_sm->haltCalculation();
        m_first = true;
        QApplication::restoreOverrideCursor();

    }
    m_sm->setExpected( m_oldexpected );
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
    //kDebug(planDbg());
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
    //kDebug(planDbg());
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
DocumentModifyNameCmd::DocumentModifyNameCmd( Document *doc, const QString &value, const QString& name )
    : NamedCommand( name ),
    m_doc( doc )
{
    Q_ASSERT( doc );
    m_value = value;
    m_oldvalue = doc->name();
}
void DocumentModifyNameCmd::execute()
{
    m_doc->setName( m_value );
}
void DocumentModifyNameCmd::unexecute()
{
    m_doc->setName( m_oldvalue );
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
DocumentModifySendAsCmd::DocumentModifySendAsCmd( Document *doc, const Document::SendAs value, const QString& name )
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

    if ( m_project->defaultCalendar() ) {
        project.setDefaultCalendar( 0 ); // or else m_project default calendar may be overwitten
    }
    QString defaultAccount;
    if ( ! m_project->accounts().defaultAccount() && project.accounts().defaultAccount() ) {
        defaultAccount = project.accounts().defaultAccount()->name();
    }

    QMap<Node*, QString> startupaccountmap;
    QMap<Node*, QString> shutdownaccountmap;
    QMap<Node*, QString> runningaccountmap;
    QMap<Node*, QString> nodecalendarmap;

    // remove unhandled info in tasks and get accounts and calendars
    foreach ( Node *n, project.allNodes() ) {
        if ( n->type() == Node::Type_Task ) {
            Task *t = static_cast<Task*>( n );
            t->workPackage().clear();
            while ( ! t->workPackageLog().isEmpty() ) {
                WorkPackage *wp = t->workPackageLog().at( 0 );
                t->removeWorkPackage( wp );
                delete wp;
            }
        }
        if ( n->startupAccount() ) {
            startupaccountmap.insert( n, n->startupAccount()->name() );
            n->setStartupAccount( 0 );
        }
        if ( n->shutdownAccount() ) {
            shutdownaccountmap.insert( n, n->shutdownAccount()->name() );
            n->setShutdownAccount( 0 );
        }
        if ( n->runningAccount() ) {
            runningaccountmap.insert( n, n->runningAccount()->name() );
            n->setRunningAccount( 0 );
        }
        if ( n->estimate()->calendar() ) {
            nodecalendarmap.insert( n, n->estimate()->calendar()->id() );
            n->estimate()->setCalendar( 0 );
        }
    }
    // get resources pointing to calendars and accounts
    QMap<Resource*, QString> resaccountmap;
    QMap<Resource*, QString> rescalendarmap;
    foreach ( Resource *r, project.resourceList() ) {
        if ( r->account() ) {
            resaccountmap.insert( r, r->account()->name() );
            r->setAccount( 0 );
        }
        if ( r->calendar() ) {
            rescalendarmap.insert( r, r->calendar()->id() );
            r->setCalendar( 0 );
        }
    }
    // create add account commands and keep track of used and unused accounts
    QList<Account*> unusedAccounts;
    QMap<QString, Account*> accountsmap;
    foreach ( Account *a,  m_project->accounts().allAccounts() ) {
        accountsmap.insert( a->name(), a );
    }
    foreach ( Account *a, project.accounts().accountList() ) {
        addAccounts( a, 0, unusedAccounts, accountsmap );
    }
    // create add calendar commands and keep track of used and unused calendars
    QList<Calendar*> unusedCalendars;
    QMap<QString, Calendar*> calendarsmap;
    foreach ( Calendar *c,  m_project->allCalendars() ) {
        calendarsmap.insert( c->id(), c );
    }
    foreach ( Calendar *c, project.calendars() ) {
        addCalendars( c, 0, unusedCalendars, calendarsmap );
    }
    // get all requests before resources are merged
    QMap<ResourceGroupRequest*, QPair<Node *, ResourceGroup*> > greqs;
    QMap<ResourceGroupRequest*, QPair<ResourceRequest*, Resource*> > rreqs;
    foreach ( Node *n, project.allNodes() ) {
        QList<ResourceRequest*> resReq;
        if ( n->type() != (int)Node::Type_Task || n->requests().isEmpty() ) {
            continue;
        }
        while ( ResourceGroupRequest *gr = n->requests().requests().value( 0 ) ) {
            while ( ResourceRequest *rr = gr->resourceRequests( false ).value( 0 ) ) {
                kDebug(insertProjectCmdDba())<<"Get resource request:"<<rr;
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
#ifdef NDEBUG
            Q_UNUSED(i);
#endif
        }
    }
    QList<ResourceGroup*> allGroups;
    QList<Resource*> allResources;
    QList<Resource*> newResources;
    QMap<ResourceGroup*, ResourceGroup*> existingGroups;
    QMap<Resource*, Resource*> existingResources;
    foreach ( ResourceGroup *g, project.resourceGroups() ) {
        ResourceGroup *gr = m_project->findResourceGroup( g->id() );
        if ( gr == 0 ) {
            addCommand( new AddResourceGroupCmd( m_project, g, QString("ResourceGroup") ) );
            gr = g;
            kDebug(insertProjectCmdDba())<<"AddResourceGroupCmd:"<<gr->name();
        } else {
            existingGroups[ gr ] = g;
        }
        allGroups << gr;
        foreach ( Resource *r, g->resources() ) {
            while ( Schedule *s = r->schedules().values().value( 0 ) ) {
                r->deleteSchedule( s ); // schedules not handled
            }
            Resource *res = m_project->findResource( r->id() );
            if ( res == 0 ) {
                addCommand( new AddResourceCmd( gr, r, "Resource" ) );
                allResources << r;
                newResources << r;
                kDebug(insertProjectCmdDba())<<"AddResourceCmd:"<<gr->name()<<r->name();
            } else {
                existingResources[ res ] = r;
                allResources << res;
            }
        }
    }
    // Update resource account
    {QMap<Resource*, QString>::const_iterator it = resaccountmap.constBegin();
    QMap<Resource*, QString>::const_iterator end = resaccountmap.constEnd();
    for ( ; it != end; ++it ) {
        Resource *r = it.key();
        if ( newResources.contains( r ) ) {
            Q_ASSERT( allResources.contains( r ) );
            addCommand( new ResourceModifyAccountCmd( *r, 0, accountsmap.value( it.value() ) ) );
        }
    }}
    // Update resource calendar
    {QMap<Resource*, QString>::const_iterator it = rescalendarmap.constBegin();
    QMap<Resource*, QString>::const_iterator end = rescalendarmap.constEnd();
    for ( ; it != end; ++it ) {
        Resource *r = it.key();
        if ( newResources.contains( r ) ) {
            Q_ASSERT( allResources.contains( r ) );
            addCommand( new ModifyResourceCalendarCmd( r, calendarsmap.value( it.value() ) ) );
        }
    }}
    // Requests: clean up requests to resources already in m_project
    int gi = 0;
    int ri = 0;
    foreach ( ResourceGroupRequest *gr, greqs.keys() ) {
        QPair<Node*, ResourceGroup*> pair = greqs[ gr ];
        Node *n = pair.first;
        ResourceGroup *newGroup = pair.second;
        if ( existingGroups.values().contains( newGroup ) ) {
            newGroup = existingGroups.key( newGroup );
        }
        Q_ASSERT( allGroups.contains( newGroup ) );
        gr->setGroup( newGroup );
        addCommand( new AddResourceGroupRequestCmd( static_cast<Task&>( *n ), gr, QString("Group %1").arg( ++gi ) ) );
        kDebug(insertProjectCmdDba())<<"Add resource group request:"<<n->name()<<":"<<newGroup->name();
        QMap<ResourceGroupRequest*, QPair<ResourceRequest*, Resource*> >::const_iterator i = rreqs.constFind( gr );
        for ( ; i != rreqs.constEnd() && i.key() == gr; ++i ) {
            ResourceRequest *rr = i.value().first;
            Resource *newRes = i.value().second;
            if ( existingResources.values().contains( newRes ) ) {
                newRes = existingResources.key( newRes );
            }
            kDebug(insertProjectCmdDba())<<"Add resource request:"<<n->name()<<":"<<newGroup->name()<<":"<<newRes->name();
            if ( ! rr->requiredResources().isEmpty() ) {
                // the resource request may have required resources that needs mapping
                QList<Resource*> required;
                foreach ( Resource *r, rr->requiredResources() ) {
                    if ( newResources.contains( r ) ) {
                        required << r;
                        kDebug(insertProjectCmdDba())<<"Request: required (new)"<<r->name();
                        continue;
                    }
                    Resource *r2 = existingResources.key( r );
                    Q_ASSERT( allResources.contains( r2 ) );
                    if ( r2 ) {
                        kDebug(insertProjectCmdDba())<<"Request: required (existing)"<<r2->name();
                        required << r2;
                    }
                }
                rr->setRequiredResources( required );
            }
            Q_ASSERT( allResources.contains( newRes ) );
            // all resource requests shall be reinserted
            rr->setResource( newRes );
            addCommand( new AddResourceRequestCmd( gr, rr, QString("Resource %1").arg( ++ri ) ) );
        }
    }
    // Add nodes ( ids are unique, no need to check )
    Node *node_after = after;
    for ( int i = 0; i < project.numChildren(); ++i ) {
        Node *n = project.childNode( i );
        Q_ASSERT( n );
        while ( Schedule *s = n->schedules().values().value( 0 ) ) {
            n->takeSchedule( s ); // schedules not handled
            delete s;
        }
        n->setParentNode( 0 );
        if ( node_after ) {
            addCommand( new TaskAddCmd( m_project, n, node_after, "Task" ) );
            node_after = n;
        } else {
            addCommand( new SubtaskAddCmd( m_project, n, parent, "Subtask" ) );
        }
        addChildNodes( n );
    }
    // Dependencies:
    foreach ( Node *n, project.allNodes() ) {
        while ( n->numDependChildNodes() > 0 ) {
            Relation *r = n->dependChildNodes().at( 0 );
            n->takeDependChildNode( r );
            r->child()->takeDependParentNode( r );
            addCommand( new AddRelationCmd( *m_project, r ) );
        }
    }
    // node calendar
    {QMap<Node*, QString>::const_iterator it = nodecalendarmap.constBegin();
    QMap<Node*, QString>::const_iterator end = nodecalendarmap.constEnd();
    for ( ; it != end; ++it ) {
        addCommand( new ModifyEstimateCalendarCmd( *(it.key()), 0, calendarsmap.value( it.value() ) ) );
    }}
    // node startup account
    {QMap<Node*, QString>::const_iterator it = startupaccountmap.constBegin();
    QMap<Node*, QString>::const_iterator end = startupaccountmap.constEnd();
    for ( ; it != end; ++it ) {
        addCommand( new NodeModifyStartupAccountCmd( *(it.key()), 0, accountsmap.value( it.value() ) ) );
    }}
    // node shutdown account
    {QMap<Node*, QString>::const_iterator it = shutdownaccountmap.constBegin();
    QMap<Node*, QString>::const_iterator end = shutdownaccountmap.constEnd();
    for ( ; it != end; ++it ) {
        addCommand( new NodeModifyShutdownAccountCmd( *(it.key()), 0, accountsmap.value( it.value() ) ) );
    }}
    // node running account
    {QMap<Node*, QString>::const_iterator it = runningaccountmap.constBegin();
    QMap<Node*, QString>::const_iterator end = runningaccountmap.constEnd();
    for ( ; it != end; ++it ) {
        addCommand( new NodeModifyRunningAccountCmd( *(it.key()), 0, accountsmap.value( it.value() ) ) );
    }}

    if ( ! defaultAccount.isEmpty() ) {
        Account *a = accountsmap.value( defaultAccount );
        if ( a && a->list() ) {
            addCommand( new ModifyDefaultAccountCmd( m_project->accounts(), 0, a ) );
        }
    }
    // Cleanup
    // Remove nodes from project so they are not deleted
    while ( Node *ch = project.childNode( 0 ) ) {
        project.takeChildNode( ch );
    }
    foreach ( Node *n, project.allNodes() ) {
        project.removeId( n->id() );
    }

    // Remove calendars from project
    while ( project.calendarCount() > 0 ) {
        project.takeCalendar( project.calendarAt( 0 ) );
    }
    qDeleteAll( unusedCalendars );

    // Remove accounts from project
    while ( project.accounts().accountCount() > 0 ) {
        project.accounts().take( project.accounts().accountAt( 0 ) );
    }
    qDeleteAll( unusedAccounts );

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

void InsertProjectCmd::addCalendars( Calendar *calendar, Calendar *parent, QList<Calendar*> &unused, QMap<QString, Calendar*> &calendarsmap ) {
    Calendar *par = 0;
    if ( parent ) {
        par = calendarsmap.value( parent->id() );
    }
    if ( par == 0 ) {
        par = parent;
    }
    Calendar *cal = calendarsmap.value( calendar->id() );
    if ( cal == 0 ) {
        calendarsmap.insert( calendar->id(), calendar );
        addCommand( new CalendarAddCmd( m_project, calendar, -1, par ) );
    } else {
        unused << calendar;
    }
    foreach ( Calendar *c, calendar->calendars() ) {
        addCalendars( c, calendar, unused, calendarsmap );
    }
}

void InsertProjectCmd::addAccounts( Account *account, Account *parent, QList<Account*>  &unused, QMap<QString, Account*>  &accountsmap ) {
    Account *par = 0;
    if ( parent ) {
        par = accountsmap.value( parent->name() );
    }
    if ( par == 0 ) {
        par = parent;
    }
    Account *acc = accountsmap.value( account->name() );
    if ( acc == 0 ) {
        kDebug(insertProjectCmdDba())<<"Move to new project:"<<account<<account->name();
        accountsmap.insert( account->name(), account );
        addCommand( new AddAccountCmd( *m_project, account, par, -1, QString( "Add account %1" ).arg( account->name() ) ) );
    } else {
        kDebug(insertProjectCmdDba())<<"Already exists:"<<account<<account->name();
        unused << account;
    }
    while ( ! account->accountList().isEmpty() ) {
        Account *a = account->accountList().first();
        account->list()->take( a );
        addAccounts( a, account, unused, accountsmap );
    }
}

void InsertProjectCmd::addChildNodes( Node *node ) {
    // schedules not handled
    while ( Schedule *s = node->schedules().values().value( 0 ) ) {
        node->takeSchedule( s ); // schedules not handled
        delete s;
    }
    foreach ( Node *n, node->childNodeIterator() ) {
        n->setParentNode( 0 );
        addCommand( new SubtaskAddCmd( m_project, n, node, "Subtask" ) );
        addChildNodes( n );
    }
    // Remove child nodes so they are not added twice
    while ( Node *ch = node->childNode( 0 ) ) {
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
#if KDE_IS_VERSION(4,4,0)
    m_oldvalue(locale->monetaryDecimalPlaces())
#else
    m_oldvalue( locale->fracDigits() )
#endif
{
};
void ModifyCurrencyFractionalDigitsCmd::execute()
{
#if KDE_IS_VERSION(4,4,0)
    m_locale->setMonetaryDecimalPlaces(m_newvalue);
#else
    m_locale->setFracDigits( m_newvalue );
#endif
}
void ModifyCurrencyFractionalDigitsCmd::unexecute()
{
#if KDE_IS_VERSION(4,4,0)
    m_locale->setMonetaryDecimalPlaces(m_oldvalue);
#else
    m_locale->setFracDigits( m_oldvalue );
#endif
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

AddExternalAppointmentCmd::AddExternalAppointmentCmd( Resource *resource, const QString &pid, const QString &pname, const QDateTime &start, const QDateTime &end, double load, const QString& name )
    : NamedCommand( name ),
    m_resource( resource ),
    m_pid( pid ),
    m_pname( pname ),
    m_start( start ),
    m_end( end ),
    m_load( load )
{
}

void AddExternalAppointmentCmd::execute()
{
    m_resource->addExternalAppointment( m_pid, m_pname, m_start, m_end, m_load );
}

void AddExternalAppointmentCmd::unexecute()
{
    m_resource->subtractExternalAppointment( m_pid, m_start, m_end, m_load );
    // FIXME do this smarter
    if ( ! m_resource->externalAppointments( m_pid ).isEmpty() ) {
        m_resource->takeExternalAppointment( m_pid );
    }
}

ClearExternalAppointmentCmd::ClearExternalAppointmentCmd( Resource *resource, const QString &pid, const QString &name )
    : NamedCommand( name ),
    m_resource( resource ),
    m_pid( pid ),
    m_appointments( 0 )
{
}

ClearExternalAppointmentCmd::~ClearExternalAppointmentCmd()
{
    delete m_appointments;
}

void ClearExternalAppointmentCmd::execute()
{
//     kDebug(planDbg())<<text()<<":"<<m_resource->name()<<m_pid;
    m_appointments = m_resource->takeExternalAppointment( m_pid );
}

void ClearExternalAppointmentCmd::unexecute()
{
//     kDebug(planDbg())<<text()<<":"<<m_resource->name()<<m_pid;
    if ( m_appointments ) {
        m_resource->addExternalAppointment( m_pid, m_appointments );
    }
    m_appointments = 0;
}

ClearAllExternalAppointmentsCmd::ClearAllExternalAppointmentsCmd( Project *project, const QString &name )
    : NamedCommand( name ),
    m_project( project )
{
    foreach ( Resource *r, project->resourceList() ) {
        foreach ( const QString &id, r->externalProjects().keys() ) {
            m_cmd.addCommand( new ClearExternalAppointmentCmd( r, id ) );
        }
    }
}

void ClearAllExternalAppointmentsCmd::execute()
{
    m_cmd.redo();
}

void ClearAllExternalAppointmentsCmd::unexecute()
{
    m_cmd.undo();
}

}  //KPlato namespace
