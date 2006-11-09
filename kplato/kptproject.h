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
     * Calculate the whole project.
     *
     * @param schedule Schedule to use
     */
    void calculate( Schedule *scedule );
    /**
     * Calculate the whole project.
     *
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     */
    void calculate( Effort::Use use );
    /// Calculate current schedule
    void calculate();

    virtual bool calcCriticalPath( bool fromEnd );

    virtual DateTime startTime() const;
    virtual DateTime endTime() const;

    /// Returns the duration calculated as latestFinish - earliestStart.
    Duration *getExpectedDuration();

    /**
     * Instead of using the expected duration, generate a random value using
     * the Distribution of each Task. This can be used for Monte-Carlo
     * estimation of Project duration.
     */
    Duration *getRandomDuration();

    virtual bool load( QDomElement &element );
    virtual void save( QDomElement &element ) const;

    QList<ResourceGroup*> &resourceGroups();
    virtual void addResourceGroup( ResourceGroup *resource );
    virtual void insertResourceGroup( unsigned int index, ResourceGroup *resource );
    void deleteResourceGroup( ResourceGroup *resource );
    void deleteResourceGroup( int number );
    ResourceGroup *takeResourceGroup( ResourceGroup *resource )
    {
        int i = m_resourceGroups.indexOf( resource );
        return i != -1 ? m_resourceGroups.takeAt( i ) : 0;
    }

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
    ResourceGroup *group( QString id );
    /// Returns the resource with identity id.
    Resource *resource( QString id );

    virtual EffortCostMap plannedEffortCostPrDay( const QDate &start, const QDate &end ) const;

    /// Returns the total planned effort for this project (or subproject)
    virtual Duration plannedEffort();
    /// Returns the total planned effort for this project (or subproject) on date
    virtual Duration plannedEffort( const QDate &date );
    /// Returns the planned effort up to and including date
    virtual Duration plannedEffortTo( const QDate &date );

    /// Returns the actual effort
    virtual Duration actualEffort();
    /// Returns the actual effort on date
    virtual Duration actualEffort( const QDate &date );
    /// Returns the actual effort up to and including date
    virtual Duration actualEffortTo( const QDate &date );
    /**
     * Returns the total planned cost for this project
     */
    virtual double plannedCost();
    /// Planned cost on date
    virtual double plannedCost( const QDate &date );
    /// Planned cost up to and including date
    virtual double plannedCostTo( const QDate &date );

    /**
     * Returns the actually reported cost for this project
     */
    virtual double actualCost();
    /// Actual cost on date
    virtual double actualCost( const QDate &date );
    /// Actual cost up to and including date
    virtual double actualCostTo( const QDate &date );

    Calendar *defaultCalendar() { return m_standardWorktime->calendar(); }
    QList<Calendar*> calendars();
    void addCalendar( Calendar *calendar );
    /// Returns the calendar with identity id.
    Calendar *calendar( const QString id ) const;

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

    void generateWBS( int count, WBSDefinition &def, QString wbs = QString() );

    Accounts &accounts() { return m_accounts; }

    /// Set current schedule to schedule with identity id, for me and my children
    virtual void setCurrentSchedule( long id );
    /// Create new schedule with unique id.
    MainSchedule *createSchedule( QString name, Schedule::Type type );
    /// Set parent schedule for my children
    virtual void setParentSchedule( Schedule *sch );

signals:
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

#ifndef NDEBUG
public:
    void printDebug( bool children, QByteArray indent );
    void printCalendarDebug( QByteArray indent = "" );
#endif
};

}  //KPlato namespace

#endif
