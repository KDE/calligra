/* This file is part of the KDE project
  Copyright (C) 2001 Thomas Zander zander@kde.org
  Copyright (C) 2004 - 2010 Dag Andersen <danders@get2net.dk>
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

#ifndef KPTPROJECT_H
#define KPTPROJECT_H

#include "kplatokernel_export.h"

#include "kptnode.h"

#include "kptglobal.h"
#include "kptaccount.h"
#include "kptcalendar.h"
#include "kptdatetime.h"
#include "kptduration.h"
#include "kptresource.h"
#include "kptwbsdefinition.h"
#include "kptconfigbase.h"

#include <QMap>
#include <QList>
#include <QHash>
#include <QPointer>

#include <klocale.h>
#include <ktimezone.h>


/// The main namespace.
namespace KPlato
{

class Schedule;
class StandardWorktime;
class ScheduleManager;
class XMLLoaderObject;
class Task;
class SchedulerPlugin;
class KPlatoXmlLoaderBase;

/**
 * Project is the main node in a project, it contains child nodes and
 * possibly sub-projects. A sub-project is just another instantion of this
 * node however.
 *
 * A note on timezones:
 * To be able to handle resources working in diffierent timezones and
 * to facilitate data exchange with other applications like PIMs or
 * and groupware servers, the project has a timezone that is used for 
 * all datetimes in nodes and schedules.
 * By default the local timezone is used.
 *
 * A resources timezone is defined by the associated calendar.
 *
 * Note that a projects datetimes are always displayed/modified in the timezone
 * it was originally created, not necessarly in your current local timezone.
 */
class KPLATOKERNEL_EXPORT Project : public Node
{
    Q_OBJECT
public:
    explicit Project( Node *parent = 0 );
    explicit Project( ConfigBase &config, Node *parent = 0 );
    ~Project();

    /// Reference this project.
    void ref() { ++m_refCount; }
    /// De-reference this project. Deletes project of ref count <= 0
    void deref();

    /// Returns the node type. Can be Type_Project or Type_Subproject.
    virtual int type() const;

    /**
     * Calculate the schedules managed by the schedule manager
     *
     * @param sm Schedule manager
     */
    void calculate( ScheduleManager &sm );

    /**
     * Re-calculate the schedules managed by the schedule manager
     *
     * @param sm Schedule manager
     * @param dt The datetime from when the schedule shall be re-calculated
     */
    void calculate( ScheduleManager &sm, const DateTime &dt );

    virtual DateTime startTime( long id = -1 ) const;
    virtual DateTime endTime( long id = -1 ) const;

    /// Returns the calculated duration for schedule @p id
    Duration duration( long id = -1 ) const;
    using Node::duration;
    /**
     * Instead of using the expected duration, generate a random value using
     * the Distribution of each Task. This can be used for Monte-Carlo
     * estimation of Project duration.
     */
    Duration *getRandomDuration();

    virtual bool load( KoXmlElement &element, XMLLoaderObject &status );
    virtual void save( QDomElement &element ) const;
    
    using Node::saveWorkPackageXML;
    /// Save a workpackage document containing @node with schedule identity @p id
    void saveWorkPackageXML( QDomElement &element, const Node *node, long id ) const;
    
    /**
     * Add the node @p task to the project, after node @p position
     * If @p postition is zero or the project node, it will be added to this project.
     */
    bool addTask( Node* task, Node* position );
    /**
     * Add the node @p task to the @p parent
     */
    bool addSubTask( Node* task, Node* parent );
    /**
     * Add the node @p task to @p parent, in position @p index
     * If @p parent is zero, it will be added to this project.
     */
    bool addSubTask( Node* task, int index, Node* parent, bool emitSignal = true );
    /**
     * Remove the @p node.
     * The node is not deleted.
     */
    void takeTask( Node *node, bool emitSignal = true );
    bool canMoveTask( Node* node, Node *newParent );
    bool moveTask( Node* node, Node *newParent, int newPos );
    bool canIndentTask( Node* node );
    bool indentTask( Node* node, int index = -1 );
    bool canUnindentTask( Node* node );
    bool unindentTask( Node* node );
    bool canMoveTaskUp( Node* node );
    bool moveTaskUp( Node* node );
    bool canMoveTaskDown( Node* node );
    bool moveTaskDown( Node* node );
    /**
     * Create a task with a unique id.
     * The task is not added to the project. Do this with addSubTask().
     */
    Task *createTask();
    /**
     * Create a copy of @p def with a unique id.
     * The task is not added to the project. Do this with addSubTask().
     */
    Task *createTask( const Task &def );

    int resourceGroupCount() const { return m_resourceGroups.count(); }
    QList<ResourceGroup*> &resourceGroups();
    /// Adds the resource group to the project.
    virtual void addResourceGroup( ResourceGroup *resource, int index = -1 );
    /**
     * Removes the resource group @p resource from the project.
     * The resource group is not deleted.
     */
    ResourceGroup *takeResourceGroup( ResourceGroup *resource );
    int indexOf( ResourceGroup *resource ) const { return m_resourceGroups.indexOf( resource ); }
    ResourceGroup *resourceGroupAt( int pos ) const { return m_resourceGroups.value( pos ); }
    int numResourceGroups() const { return m_resourceGroups.count(); }
    
    /// Returns the resourcegroup with identity id.
    ResourceGroup *group( const QString& id );
    /// Returns the resource group with the matching name, 0 if no match is found.
    ResourceGroup *groupByName( const QString& name ) const;
    
    /**
     * Adds the resource to the project and resource group.
     * Always use this to add resources.
     */
    void addResource( ResourceGroup *group, Resource *resource, int index = -1 );
    /** 
     * Removes the resource from the project and resource group.
     * The resource is not deleted.
     * Always use this to remove resources.
     */ 
    Resource *takeResource( ResourceGroup *group, Resource *resource );
    /// Move @p resource to the new @p group. Requests are removed.
    void moveResource( ResourceGroup *group, Resource *resource );
    /// Returns the resource with identity id.
    Resource *resource( const QString& id );
    /// Returns the resource with matching name, 0 if no match is found.
    Resource *resourceByName( const QString& name ) const;
    QStringList resourceNameList() const;
    /// Returns a list of all resources
    QList<Resource*> resourceList() const { return resourceIdDict.values(); }

    virtual EffortCostMap plannedEffortCostPrDay( const QDate &start, const QDate &end, long id = CURRENTSCHEDULE, EffortCostCalculationType = ECCT_All ) const;
    virtual EffortCostMap plannedEffortCostPrDay(const Resource *resource, const QDate &start, const QDate &end, long id = CURRENTSCHEDULE, EffortCostCalculationType = ECCT_All ) const;

    using Node::plannedEffort;
    /// Returns the total planned effort for this project (or subproject)
    virtual Duration plannedEffort( long id = CURRENTSCHEDULE, EffortCostCalculationType = ECCT_All ) const;
    /// Returns the total planned effort for this project (or subproject) on date
    virtual Duration plannedEffort( const QDate &date, long id = CURRENTSCHEDULE, EffortCostCalculationType = ECCT_All  ) const;
    using Node::plannedEffortTo;
    /// Returns the planned effort up to and including date
    virtual Duration plannedEffortTo( const QDate &date, long id = CURRENTSCHEDULE, EffortCostCalculationType = ECCT_All  ) const;

    /// Returns the actual effort up to and including @p date
    virtual Duration actualEffortTo( const QDate &date ) const;
    /**
     * Planned cost up to and including date
     * @param date The cost is calculated from the start of the project upto including date.
     * @param id Identity of the schedule to be used.
     */
    virtual double plannedCostTo( const QDate &date, long id = CURRENTSCHEDULE, EffortCostCalculationType = ECCT_All  ) const;

    /**
     * Actual cost up to and including @p date
     * @param date The cost is calculated from the start of the project upto including date.
     */
    virtual EffortCost actualCostTo(  long int id, const QDate &date ) const;
    
    virtual EffortCostMap actualEffortCostPrDay( const QDate &start, const QDate &end, long id = CURRENTSCHEDULE, EffortCostCalculationType = ECCT_All ) const;

    virtual EffortCostMap actualEffortCostPrDay( const Resource *resource, const QDate &start, const QDate &end, long id = CURRENTSCHEDULE, EffortCostCalculationType = ECCT_All ) const;

    double effortPerformanceIndex( const QDate &date, long id ) const;
    
    double schedulePerformanceIndex( const QDate &date, long id ) const;
    
    /// Returns the effort planned to be used to reach the actual percent finished
    virtual Duration budgetedWorkPerformed( const QDate &date, long id = CURRENTSCHEDULE ) const;
    /// Returns the cost planned to be used to reach the actual percent finished
    virtual double budgetedCostPerformed( const QDate &date, long id = CURRENTSCHEDULE ) const;

    /// Budgeted Cost of Work Scheduled ( up to @p date )
    virtual double bcws( const QDate &date, long id = BASELINESCHEDULE ) const;
    /// Budgeted Cost of Work Performed
    virtual double bcwp( long id = BASELINESCHEDULE ) const;
    /// Budgeted Cost of Work Performed ( up to @p date )
    virtual double bcwp( const QDate &date, long id = BASELINESCHEDULE ) const;

    Calendar *defaultCalendar() const { return m_defaultCalendar; }
    void setDefaultCalendar( Calendar *cal );
    const QList<Calendar*> &calendars() const;
    void addCalendar( Calendar *calendar, Calendar *parent = 0, int index = -1 );
    void takeCalendar( Calendar *calendar );
    int indexOf( const Calendar *calendar ) const;
    /// Returns the calendar with identity id.
    Calendar *calendar( const QString& id ) const;
    /// Returns a list of all calendars
    QStringList calendarNames() const;
    /// Find calendar by name
    Calendar *calendarByName( const QString &name ) const;
    void changed( Calendar *cal );
    QList<Calendar*> allCalendars() const;
    /// Return number of calendars
    int calendarCount() const { return m_calendars.count(); }
    /// Return the calendar at @p index, 0 if index out of bounds
    Calendar *calendarAt( int index ) const { return m_calendars.value( index ); }
    /**
     * Defines the length of days, weeks, months and years
     * and the standard working week.
     * Used for estimation and calculation of effort, 
     * and presentation in gantt chart.
     */
    StandardWorktime *standardWorktime() { return m_standardWorktime; }
    void setStandardWorktime( StandardWorktime * worktime );
    void changed( StandardWorktime* );

    /// Check if a link exists between node @p par and @p child.
    bool linkExists( const Node *par, const Node *child ) const;
    /// Check if node @p par can be linked to node @p child.
    bool legalToLink( const Node *par, const Node *child ) const;
    using Node::legalToLink;

    virtual const QHash<QString, Node*> &nodeDict() { return nodeIdDict; }
    /// Return a list of all nodes in the project (exluding myself)
    QList<Node*> allNodes() const;
    /// Return the number of all nodes in the project (exluding myself)
    int nodeCount() const { return nodeIdDict.count() - 1; }

    /// Return a list of all tasks and milestones int the wbs order
    QList<Task*> allTasks( const Node *parent = 0 ) const;

    using Node::findNode;
    /// Find the node with identity id
    virtual Node *findNode( const QString &id ) const;

    using Node::removeId;
    /// Remove the node with identity id from the registers
    virtual bool removeId( const QString &id );
    
    /// Reserve @p id for the @p node
    virtual void reserveId( const QString &id, Node *node );
    /// Register @p node. The nodes id must be unique and non-empty.
    bool registerNodeId( Node *node );
    /// Create a unique id.
    QString uniqueNodeId( int seed = 1 ) const;
    /// Check if node @p id is used
    bool nodeIdentExists( const QString &id ) const;

    /// Create a unique id.
    QString uniqueNodeId( const QList<QString> &existingIds, int seed = 1 );

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
    /// Generate, set and insert unique id
    bool setResourceGroupId( ResourceGroup *group);
    /// returns a unique resourcegroup id
    QString uniqueResourceGroupId() const;

    /// Return a list of resources that will be allocated to new tasks
    QList<Resource*> autoAllocateResources() const;

    Resource *findResource( const QString &id ) const
    {
        if ( resourceIdDict.contains( id ) )
            return resourceIdDict[ id ];
        return 0;
    }
    /// Remove the resource with identity id from the register
    bool removeResourceId( const QString &id );
    /// Insert the resource with identity id
    void insertResourceId( const QString &id, Resource *resource );
    /// Generate, set and insert unique id
    bool setResourceId( Resource *resource );
    /// returns a unique resource id
    QString uniqueResourceId() const;

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
    /// Set and insert a unique id for calendar
    bool setCalendarId( Calendar *calendar );
    /// returns a unique calendar id
    QString uniqueCalendarId() const;
    /// Return reference to WBS Definition
    WBSDefinition &wbsDefinition();
    /// Set WBS Definition to @p def
    void setWbsDefinition( const WBSDefinition &def );
    /// Generate WBS Code
    virtual QString generateWBSCode( QList<int> &indexes ) const;
    
    Accounts &accounts() { return m_accounts; }
    const Accounts &accounts() const { return m_accounts; }

    /**
     * Set current schedule to the schedule with identity @p id, for me and my children
     * Note that this is used (and may be changed) when calculating schedules
     */
    virtual void setCurrentSchedule( long id );
    /// Create new schedule with unique name and id of type Expected.
    MainSchedule *createSchedule();
    /// Create new schedule with unique id.
    MainSchedule *createSchedule( const QString& name, Schedule::Type type );
    /// Add the schedule to the project. A fresh id will be generated for the schedule.
    void addMainSchedule( MainSchedule *schedule );
    /// Set parent schedule for my children
    virtual void setParentSchedule( Schedule *sch );
    
    /// Find the schedule manager that manages the Schedule with @p id
    ScheduleManager *scheduleManager( long id ) const;
    /// Find the schedule manager with @p id
    ScheduleManager *scheduleManager( const QString &id ) const;
    /// Create a unique schedule name (This may later be changed by the user)
    QString uniqueScheduleName() const;
    /// Create a unique schedule manager identity
    QString uniqueScheduleManagerId() const;
    ScheduleManager *createScheduleManager();
    ScheduleManager *createScheduleManager( const QString name );
    /// Returns a list of all top level schedule managers
    QList<ScheduleManager*> scheduleManagers() const { return m_managers; }
    int numScheduleManagers() const { return m_managers.count(); }
    int indexOf( const ScheduleManager *sm ) const { return m_managers.indexOf( const_cast<ScheduleManager*>(sm) ); }
    bool isScheduleManager( void* ptr ) const;
    void addScheduleManager( ScheduleManager *sm, ScheduleManager *parent = 0, int index = -1 );
    int takeScheduleManager( ScheduleManager *sm );
    void moveScheduleManager( ScheduleManager *sm, ScheduleManager *newparent = 0, int newindex = -1 );
    ScheduleManager *findScheduleManagerByName( const QString &name ) const;
    /// Returns a list of all schedule managers
    QList<ScheduleManager*> allScheduleManagers() const;
    /// Return true if schedule with identity @p id is baselined
    bool isBaselined( long id = ANYSCHEDULED ) const;
    
    void changed( ResourceGroup *group );
    void changed( Resource *resource );
    
    void changed( ScheduleManager *sm );
    void changed( MainSchedule *sch );
    void sendScheduleAdded( const MainSchedule *sch );
    void sendScheduleToBeAdded( const ScheduleManager *manager, int row );
    void sendScheduleRemoved( const MainSchedule *sch );
    void sendScheduleToBeRemoved( const MainSchedule *sch );

    /// Return the time spec used in this project
    const KDateTime::Spec &timeSpec() const { return m_spec; }
    /// Return the time zone used in this project
    KTimeZone timeZone() const { return m_spec.timeZone(); }
    /// Set the time zone to be used in this project
    void setTimeZone( const KTimeZone &tz ) { m_spec = KDateTime::Spec( tz ); }
    
    /**
     * Add a relation between the nodes specified in the relation rel.
     * Emits signals relationToBeAdded() before the relation is added,
     * and relationAdded() after it has been added.
     * @param rel The relation to be added.
     * @param check If true, the relation is checked for validity
     * @return true if successful.
     */
    bool addRelation( Relation *rel, bool check=true );
    /**
     * Removes the relation @p rel without deleting it.
     * Emits signals relationToBeRemoved() before the relation is removed,
     * and relationRemoved() after it has been removed.
     */
    void takeRelation( Relation *rel );
    
    /**
     * Modify the @p type of the @p relation.
     */
    void setRelationType( Relation *relation, Relation::Type type );
    /**
     * Modify the @p lag of the @p relation.
     */
    void setRelationLag( Relation *relation, const Duration &lag );
    
    void calcCriticalPathList( MainSchedule *cs );
    void calcCriticalPathList( MainSchedule *cs, Node *node );
    /**
     * Returns the list of critical paths for schedule @p id
     */
    const QList< QList<Node*> > *criticalPathList( long id = CURRENTSCHEDULE );
    QList<Node*> criticalPath( long id = CURRENTSCHEDULE, int index = 0 );

    /// Returns a flat list af all nodes
    QList<Node*> flatNodeList( Node *parent = 0 );
    
    void generateUniqueNodeIds();
    void generateUniqueIds();
    
    const ConfigBase &config() const { return m_config ? *m_config : emptyConfig; }
    /// Set configuration data
    void setConfig( ConfigBase *config ) { m_config = config; }

    const Task &taskDefaults() const { return config().taskDefaults(); }

    /// Return locale. (Used for currency, everything else is from KGlobal::locale)
    KLocale *locale() { return const_cast<ConfigBase&>(config()).locale(); }
    /// Return locale. (Used for currency, everything else is from KGlobal::locale)
    const KLocale *locale() const { return config().locale(); }
    /// Signal that locale data has changed
    void emitLocaleChanged();
    
    void setSchedulerPlugins( const QMap<QString, SchedulerPlugin*> &plugins );
    const QMap<QString, SchedulerPlugin*> &schedulerPlugins() const { return m_schedulerPlugins; }

    void initiateCalculation( MainSchedule &sch );
    void initiateCalculationLists( MainSchedule &sch );

    void finishCalculation( ScheduleManager &sm );
    void adjustSummarytask();

    /// Increments progress and emits signal sigProgress()
    void incProgress();
    /// Emits signal maxProgress()
    void emitMaxProgress( int value );

    bool stopcalculation;

    /// return a <id, name> map of all external projects
    QMap<QString, QString> externalProjects() const;

    void emitDocumentAdded( Node*, Document*, int index );
    void emitDocumentRemoved( Node*, Document*, int index );
    void emitDocumentChanged( Node*, Document*, int index );

public slots:
    /// Sets m_progress to @p progress and emits signal sigProgress()
    /// If @p sm is not 0, progress is also set for the schedule manager
    void setProgress( int progress, ScheduleManager *sm = 0 );
    /// Sets m_maxprogress to @p max and emits signal maxProgress()
    /// If @p sm is not 0, max progress is also set for the schedule manager
    void setMaxProgress( int max, ScheduleManager *sm = 0 );

signals:
    /// Emitted when anything in the project is changed (use with care)
    void changed();
    /// Emitted when the WBS code definition has changed. This may change all nodes.
    void wbsDefinitionChanged();
    /// Emitted when a schedule has been calculated
    void projectCalculated( ScheduleManager *sm );
    /// Emitted when the pointer to the current schedule has been changed
    void currentScheduleChanged();
    /// Use to show progress during calculation
    void sigProgress( int );
    /// Use to set the maximum progress (minimum always 0)
    void maxProgress( int );
    /// Emitted when calculation starts
    void sigCalculationStarted( Project *project, ScheduleManager *sm );
    /// Emitted when calculation is finished
    void sigCalculationFinished( Project *project, ScheduleManager *sm );
    /// This signal is emitted when one of the nodes members is changed.
    void nodeChanged( Node* );
    /// This signal is emitted when the node is to be added to the project.
    void nodeToBeAdded( Node*, int );
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

    /// This signal is emitted when a document is added
    void documentAdded( Node*, Document*, int index );
    /// This signal is emitted when a document is removed
    void documentRemoved( Node*, Document*, int index );
    /// This signal is emitted when a document is changed
    void documentChanged( Node*, Document*, int index );
    
    void resourceGroupChanged( ResourceGroup *group );
    void resourceGroupAdded( const ResourceGroup *group );
    void resourceGroupToBeAdded( const ResourceGroup *group, int row );
    void resourceGroupRemoved( const ResourceGroup *group );
    void resourceGroupToBeRemoved( const ResourceGroup *group );
    
    void resourceChanged( Resource *resource );
    void resourceAdded( const Resource *resource );
    void resourceToBeAdded( const ResourceGroup *group, int row );
    void resourceRemoved( const Resource *resource );
    void resourceToBeRemoved( const Resource *resource );

    void scheduleManagerChanged( ScheduleManager *sch );
    void scheduleManagerAdded( const ScheduleManager *sch );
    void scheduleManagerToBeAdded( const ScheduleManager *sch, int row );
    void scheduleManagerRemoved( const ScheduleManager *sch );
    void scheduleManagerToBeRemoved( const ScheduleManager *sch );
    void scheduleManagerMoved( const ScheduleManager *sch, int row );
    void scheduleManagerToBeMoved( const ScheduleManager *sch );

    void scheduleChanged( MainSchedule *sch );
    void scheduleToBeAdded( const ScheduleManager *manager, int row );
    void scheduleAdded( const MainSchedule *sch );
    void scheduleToBeRemoved( const MainSchedule *sch );
    void scheduleRemoved( const MainSchedule *sch );

//    void currentViewScheduleIdChanged( long id );
    
    void calendarChanged( Calendar *cal );
    void calendarToBeAdded( const Calendar *cal, int row );
    void calendarAdded( const Calendar *cal );
    void calendarToBeRemoved( const Calendar *cal );
    void calendarRemoved( const Calendar *cal );

    /**
     * Emitted when the the default calendar pointer has changed
     * @parem cal The new default calendar. May be 0.
     */
    void defaultCalendarChanged( Calendar *cal );
    /**
     * Emitted when the the standard worktime has been changed.
     */
    void standardWorktimeChanged( StandardWorktime* );
    
    /// Emitted when the relation @p rel is about to be added.
    void relationToBeAdded( Relation *rel, int parentIndex, int childIndex );
    /// Emitted when the relation @p rel has been added.
    void relationAdded( Relation *rel );
    /// Emitted when the relation @p rel is about to be removed.
    void relationToBeRemoved( Relation *rel );
    /// Emitted when the relation @p rel has been removed.
    void relationRemoved( Relation *rel );
    /// Emitted when the relation @p rel shall be modified.
    void relationToBeModified( Relation *rel );
    /// Emitted when the relation @p rel has been modified.
    void relationModified( Relation *rel );

    /// Emitted when locale data has changed
    void localeChanged();

protected:
    /// Calculate the schedule.
    void calculate( Schedule *scedule );
    /// Calculate current schedule
    void calculate();

    /// Re-calculate the schedule from @p dt
    void calculate( Schedule *scedule, const DateTime &dt );
    /// Calculate current schedule from @p dt (Always calculates forward)
    void calculate( const DateTime &dt );
    
    /// Calculate critical path
    virtual bool calcCriticalPath( bool fromEnd );

    /// Prepare task lists for scheduling
    void tasksForward();
    /// Prepare task lists for scheduling
    void tasksBackward();

protected:
    friend class KPlatoXmlLoaderBase;

    virtual void changed(Node *node);
    
    Accounts m_accounts;
    QList<ResourceGroup*> m_resourceGroups;

    QList<Calendar*> m_calendars;
    Calendar * m_defaultCalendar;

    StandardWorktime *m_standardWorktime;
    
    DateTime calculateForward( int use );
    DateTime calculateBackward( int use );
    DateTime scheduleForward( const DateTime &earliest, int use );
    DateTime scheduleBackward( const DateTime &latest, int use );
    DateTime checkStartConstraints( const DateTime &dt ) const;
    DateTime checkEndConstraints( const DateTime &dt ) const;

    bool legalParents( const Node *par, const Node *child ) const;
    bool legalChildren( const Node *par, const Node *child ) const;

private:
    void init();

    QHash<QString, ResourceGroup*> resourceGroupIdDict;
    QHash<QString, Resource*> resourceIdDict;
    QHash<QString, Node*> nodeIdDict;
    QMap<QString, Node*> nodeIdReserved;
    QMap<QString, Calendar*> calendarIdDict;
    QMap<QString, ScheduleManager*> m_managerIdMap;

    QList<ScheduleManager*> m_managers;
    KDateTime::Spec m_spec;
    
    WBSDefinition m_wbsDefinition;
    
    ConfigBase emptyConfig;
    QPointer<ConfigBase> m_config; // this one is not owned by me, don't delete

    int m_progress;

    QMap<QString, SchedulerPlugin*> m_schedulerPlugins;

    int m_refCount; // make it possible to use the project by different threads

    QList<Task*> m_hardConstraints;
    QList<Task*> m_softConstraints;
    QList<Task*> m_terminalNodes;
    
};


}  //KPlato namespace

#endif
