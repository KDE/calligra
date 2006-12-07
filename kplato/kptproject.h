/* This file is part of the KDE project
  Copyright (C) 2001 Thomas Zander zander@kde.org
  Copyright (C) 2004 - 2006 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTPROJECT_H
#define KPTPROJECT_H

#include "kptnode.h"

#include "kptaccount.h"
#include "kptcalendar.h"
#include "kptdatetime.h"
#include "kptduration.h"
#include "kptresource.h"

#include <QMap>
#include <QList>
#include <QHash>

#include <klocale.h>

namespace KPlato
{

class Part;
class Schedule;
class StandardWorktime;
class ScheduleManager;
class XMLLoaderObject;

/**
 * Project is the main node in a project, it contains child nodes and
 * possibly sub-projects. A sub-project is just another instantion of this
 * node however.
 */
class Project : public Node
{
    Q_OBJECT
public:
    Project( Node *parent = 0 );
    ~Project();

    /// Returns the node type. Can be Type_Project or Type_Subproject.
    virtual int type() const;

    /**
     * Calculate the schedules managed by the schedule manager
     *
     * @param sm Schedule manager
     */
    void calculate( ScheduleManager &sm );

    virtual DateTime startTime() const;
    virtual DateTime endTime() const;

    /**
     * Instead of using the expected duration, generate a random value using
     * the Distribution of each Task. This can be used for Monte-Carlo
     * estimation of Project duration.
     */
    Duration *getRandomDuration();

    virtual bool load( QDomElement &element, XMLLoaderObject &status );
    virtual void save( QDomElement &element ) const;

    QList<ResourceGroup*> &resourceGroups();
    virtual void addResourceGroup( ResourceGroup *resource );
    ResourceGroup *takeResourceGroup( ResourceGroup *resource );
    int indexOf( ResourceGroup *resource ) const { return m_resourceGroups.indexOf( resource ); }
    ResourceGroup *resourceGroupAt( int pos ) const { return m_resourceGroups.value( pos ); }
    int numResourceGroups() const { return m_resourceGroups.count(); }
    
    bool addTask( Node* task, Node* position );
    bool addSubTask( Node* task, Node* position );
    bool addSubTask( Node* task, int index, Node* parent );
    void delTask( Node *node );
    bool canIndentTask( Node* node );
    bool indentTask( Node* node, int index = -1 );
    bool canUnindentTask( Node* node );
    bool unindentTask( Node* node );
    bool canMoveTaskUp( Node* node );
    bool moveTaskUp( Node* node );
    bool canMoveTaskDown( Node* node );
    bool moveTaskDown( Node* node );
    Task *createTask( Node* parent );
    Task *createTask( Task &def, Node* parent );

    /// Returns the resourcegroup with identity id.
    ResourceGroup *group( const QString& id );
    /// Returns the resource with identity id.
    Resource *resource( const QString& id );

    virtual EffortCostMap plannedEffortCostPrDay( const QDate &start, const QDate &end, long id = -1 ) const;

    /// Returns the total planned effort for this project (or subproject)
    virtual Duration plannedEffort( long id = -1 );
    /// Returns the total planned effort for this project (or subproject) on date
    virtual Duration plannedEffort( const QDate &date, long id = -1  );
    /// Returns the planned effort up to and including date
    virtual Duration plannedEffortTo( const QDate &date, long id = -1  );

    /// Returns the actual effort
    virtual Duration actualEffort( long id = -1 );
    /// Returns the actual effort on date
    virtual Duration actualEffort( const QDate &date, long id = -1  );
    /// Returns the actual effort up to and including date
    virtual Duration actualEffortTo( const QDate &date, long id = -1  );
    /**
     * Returns the total planned cost for this project
     */
    virtual double plannedCost( long id = -1 );
    /// Planned cost on date
    virtual double plannedCost( const QDate &date, long id = -1  );
    /// Planned cost up to and including date
    virtual double plannedCostTo( const QDate &date, long id = -1  );

    /**
     * Returns the actually reported cost for this project
     */
    virtual double actualCost( long id = -1 );
    /// Actual cost on date
    virtual double actualCost( const QDate &date, long id = -1  );
    /// Actual cost up to and including date
    virtual double actualCostTo( const QDate &date, long id = -1  );

    Calendar *defaultCalendar() { return m_standardWorktime->calendar(); }
    QList<Calendar*> calendars();
    void addCalendar( Calendar *calendar );
    /// Returns the calendar with identity id.
    Calendar *calendar( const QString& id ) const;

    /**
     * Defines the length of days, weeks, months and years
     * and the standard working week.
     * Used for estimation and calculation of effort, 
     * and presentation in gantt chart.
     */
    StandardWorktime *standardWorktime() { return m_standardWorktime; }
    void setStandardWorktime( StandardWorktime * worktime );
    void setDefaultCalendar( Calendar *cal );

    /// Check if node par can be linked to node child.
    bool legalToLink( Node *par, Node *child );

    virtual const QHash<QString, Node*> &nodeDict() { return nodeIdDict; }

    /// Find the node with identity id
    virtual Node *findNode( const QString &id ) const
    {
        if ( m_parent == 0 ) {
            if ( nodeIdDict.contains( id ) )
                return nodeIdDict[ id ];
            return 0;
        }
        return m_parent->findNode( id );
    }
    /// Remove the node with identity id from the register
    virtual bool removeId( const QString &id );
    /// Insert the node with identity id
    virtual void insertId( const QString &id, Node *node );
    /// Register node. The nodes id must be unique and non-empty.
    bool registerNodeId( Node *node );
    /// Create a unique id.
    QString uniqueNodeId( int seed = 1 );

    ResourceGroup *findResourceGroup( const QString &id ) const
    {
        if ( resourceGroupIdDict.contains( id ) )
            return resourceGroupIdDict[ id ];
        return 0;
    }
    /// Remove the resourcegroup with identity id from the register
    bool removeResourceGroupId( const QString &id )
    {
        if ( resourceGroupIdDict.contains( id ) )
            return resourceGroupIdDict.remove( id );
        return false;
    }
    /// Insert the resourcegroup with identity id
    void insertResourceGroupId( const QString &id, ResourceGroup* group )
    {
        resourceGroupIdDict.insert( id, group );
    }

    Resource *findResource( const QString &id ) const
    {
        if ( resourceIdDict.contains( id ) )
            return resourceIdDict[ id ];
        return 0;
    }
    /// Remove the resource with identity id from the register
    bool removeResourceId( const QString &id ) { return resourceIdDict.remove( id ); }
    /// Insert the resource with identity id
    void insertResourceId( const QString &id, Resource *resource )
    { resourceIdDict.insert( id, resource ); }

    /// Find the calendar with identity id
    virtual Calendar *findCalendar( const QString &id ) const
    {
        if ( id.isEmpty() || !calendarIdDict.contains( id ) )
            return 0;
        return calendarIdDict[ id ];
    }
    /// Remove the calendar with identity id from the register
    virtual bool removeCalendarId( const QString &id );
    /// Insert the calendar with identity id
    virtual void insertCalendarId( const QString &id, Calendar *calendar );

    void generateWBS( int count, WBSDefinition &def, const QString& wbs = QString() );

    Accounts &accounts() { return m_accounts; }

    /// Set current schedule to schedule with identity id, for me and my children
    virtual void setCurrentSchedule( long id );
    /// Create new schedule with unique name and id of type Expected.
    MainSchedule *createSchedule();
    /// Create new schedule with unique id.
    MainSchedule *createSchedule( const QString& name, Schedule::Type type );
    /// Add the schedule to the project. A fresh id will be generated for the schedule.
    void addMainSchedule( MainSchedule *schedule );
    /// Set parent schedule for my children
    virtual void setParentSchedule( Schedule *sch );
    
    QString uniqueScheduleName() const;
    ScheduleManager *createScheduleManager();
    ScheduleManager *createScheduleManager( const QString name );
    QList<ScheduleManager*> scheduleManagers() const { return m_managers; }
    int numScheduleManagers() const { return m_managers.count(); }
    int indexOf( ScheduleManager *sm ) const { return m_managers.indexOf( sm ); }
    bool isScheduleManager( void* ptr ) const { return indexOf( static_cast<ScheduleManager*>( ptr ) ) >= 0; }
    void addScheduleManager( ScheduleManager *sm );
    void takeScheduleManager( ScheduleManager *sm );
    ScheduleManager *findScheduleManager( const QString name ) const;
    
    void changed( ResourceGroup *group );
    void sendResourceAdded( const ResourceGroup *group, const Resource *resource );
    void sendResourceToBeAdded( const ResourceGroup *group, const Resource *resource );
    void sendResourceRemoved( const ResourceGroup *group, const Resource *resource );
    void sendResourceToBeRemoved( const ResourceGroup *group, const Resource *resource );
    
    void changed( Resource *resource );
    
    void changed( ScheduleManager *sm, int type = 0 );
            
signals:
    void currentScheduleChanged();
    void sigProgress( int );
    
    /// This signal is emitted when one of the nodes members is changed.
    void nodeChanged( Node* );
    /// This signal is emitted when the node is to be added to the project.
    void nodeToBeAdded( Node* );
    /// This signal is emitted when the node has been added to the project.
    void nodeAdded( Node* );
    /// This signal is emitted when the node is to be removed from the project.
    void nodeToBeRemoved( Node* );
    /// This signal is emitted when the node has been removed from the project.
    void nodeRemoved( Node* );
    /// This signal is emitted when the node is to be moved up, moved down, indented or unindented.
    void nodeToBeMoved( Node* );
    /// This signal is emitted when the node has been moved up, moved down, indented or unindented.
    void nodeMoved( Node* );
    
    void resourceGroupChanged( ResourceGroup *group );
    void resourceGroupAdded( const ResourceGroup *group );
    void resourceGroupToBeAdded( const ResourceGroup *group );
    void resourceGroupRemoved( const ResourceGroup *group );
    void resourceGroupToBeRemoved( const ResourceGroup *group );
    
    void resourceChanged( Resource *resource );
    void resourceAdded( const ResourceGroup *group, const Resource *resource );
    void resourceToBeAdded( const ResourceGroup *group, const Resource *resource );
    void resourceRemoved( const ResourceGroup *group, const Resource *resource );
    void resourceToBeRemoved( const ResourceGroup *group, const Resource *resource );

    void scheduleManagerChanged( ScheduleManager *sch );
    void scheduleManagerAdded( const ScheduleManager *sch );
    void scheduleManagerToBeAdded( const ScheduleManager *sch );
    
    void scheduleChanged( MainSchedule *sch );
    void scheduleAdded( const MainSchedule *sch );
    void scheduleToBeAdded( const MainSchedule *sch );

protected:
    /**
     * Calculate the schedule.
     *
     * @param schedule Schedule to use
     */
    void calculate( Schedule *scedule );
    /// Calculate current schedule
    void calculate();

    virtual bool calcCriticalPath( bool fromEnd );

protected:
    virtual void changed(Node *node);
    
    Accounts m_accounts;
    QList<ResourceGroup*> m_resourceGroups;

    QList<Calendar*> m_calendars;

    StandardWorktime *m_standardWorktime;

    DateTime calculateForward( int use );
    DateTime calculateBackward( int use );
    DateTime scheduleForward( const DateTime &earliest, int use );
    DateTime scheduleBackward( const DateTime &latest, int use );
    void adjustSummarytask();

    void initiateCalculation( Schedule &sch );
    void initiateCalculationLists( QList<Node*> &startnodes, QList<Node*> &endnodes, QList<Node*> &summarytasks );

    bool legalParents( Node *par, Node *child );
    bool legalChildren( Node *par, Node *child );

private:
    void init();

    QList<Node*> m_startNodes;
    QList<Node*> m_endNodes;
    QList<Node*> m_summarytasks;

    QHash<QString, ResourceGroup*> resourceGroupIdDict;
    QHash<QString, Resource*> resourceIdDict;
    QHash<QString, Node*> nodeIdDict;
    QHash<QString, Calendar*> calendarIdDict;

    QList<ScheduleManager*> m_managers;
    
#ifndef NDEBUG
public:
    void printDebug( bool children, const QByteArray& indent );
    void printCalendarDebug( const QByteArray& indent = "" );
#endif
};


}  //KPlato namespace

#endif
