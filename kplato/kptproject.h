/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org
   Copyright (C) 2004, 2005 Dag Andersen <danders@get2net.dk>

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

#ifndef kptproject_h
#define kptproject_h

#include "kptnode.h"

#include "kptcalendar.h"
#include "kptduration.h"
#include "kptresource.h"

#include <qmap.h>
#include <qptrlist.h>
#include <qdict.h>

namespace KPlato
{

class KPTPart;
class KPTStandardWorktime;

//#define DEBUGPERT
/**
 * KPTProject is the main node in a project, it contains child nodes and
 * possibly sub-projects. A sub-project is just another instantion of this
 * node however.
 */
class KPTProject : public KPTNode {
public:
    KPTProject(KPTNode *parent = 0);
    ~KPTProject();

    /// Returns the node type. Can be Type_Project or Type_Subproject.
    virtual int type() const;

    /**
     * Calculate the whole project.
     *
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     */
    void calculate(KPTEffort::Use use=KPTEffort::Use_Expected);
     
    virtual bool calcCriticalPath();
    
    ///Returns the duration calculated as latestFinish - earliestStart.
    KPTDuration *getExpectedDuration();

    /**
     * Instead of using the expected duration, generate a random value using
     * the Distribution of each Task. This can be used for Monte-Carlo
     * estimation of Project duration.
     */
    KPTDuration *getRandomDuration();

    virtual bool load(QDomElement &element);
    virtual void save(QDomElement &element) ;

    KPTDateTime getEarliestStart() const { return earliestStart; }
    KPTDateTime getLatestFinish() const { return latestFinish; }

    QPtrList<KPTResourceGroup> &resourceGroups();
    virtual void addResourceGroup(KPTResourceGroup *resource);
    virtual void insertResourceGroup(unsigned int index, KPTResourceGroup *resource);
    void removeResourceGroup(KPTResourceGroup *resource);
    void removeResourceGroup(int number);
    KPTResourceGroup *takeResourceGroup(KPTResourceGroup *resource)
        { return m_resourceGroups.take(m_resourceGroups.findRef(resource)); }

    bool addTask( KPTNode* task, KPTNode* position );
    bool addSubTask( KPTNode* task, KPTNode* position );
    bool canIndentTask(KPTNode* node);
    bool indentTask( KPTNode* node );
    bool canUnindentTask( KPTNode* node );
    bool unindentTask( KPTNode* node );
    bool canMoveTaskUp( KPTNode* node );
    bool moveTaskUp( KPTNode* node );
    bool canMoveTaskDown( KPTNode* node );
    bool moveTaskDown( KPTNode* node );

    /// Returns the resourcegroup with identity id.
    KPTResourceGroup *group(QString id);
    /// Returns the resource with identity id.
    KPTResource *resource(QString id);

    /// Returns the total planned effort for this project (or subproject) 
    virtual KPTDuration plannedEffort();
    /// Returns the total planned effort for this project (or subproject) on date
    virtual KPTDuration plannedEffort(const QDate &date);
    /// Returns the planned effort up to and including date
    virtual KPTDuration plannedEffortTo(const QDate &date);
    
    /// Returns the actual effort 
    virtual KPTDuration actualEffort();
    /// Returns the actual effort on date
    virtual KPTDuration actualEffort(const QDate &date);
    /// Returns the actual effort up to and including date
    virtual KPTDuration actualEffortTo(const QDate &date);
    /**
     * Returns the total planned cost for this project
     */
    virtual double plannedCost();
    /// Planned cost on date
    virtual double plannedCost(const QDate &date);
    /// Planned cost up to and including date
    virtual double plannedCostTo(const QDate &date);
    
    /**
     * Returns the actually reported cost for this project
     */
    virtual double actualCost();
    /// Actual cost on date
    virtual double actualCost(const QDate &date);
    /// Actual cost up to and including date
    virtual double actualCostTo(const QDate &date);

    KPTCalendar *defaultCalendar() { return m_defaultCalendar; }
    const QPtrList<KPTCalendar> &calendars() const { return m_calendars; }
    void addCalendar(KPTCalendar *calendar);
    /// Returns the calendar with identity id.
    KPTCalendar *calendar(const QString id) const;

    /**
     * Defines the length of days, weeks, months and years.
     * Used for estimation and calculation of effort, 
     * and presentation in gantt chart.
     */    
    KPTStandardWorktime *standardWorktime() { return m_standardWorktime; }
    void addStandardWorktime(KPTStandardWorktime * worktime); //FIXME
    void addDefaultCalendar(KPTStandardWorktime * worktime); //FIXME

    /// Check if node par can be linked to node child.
    bool legalToLink(KPTNode *par, KPTNode *child);
    
    /// Find the node with identity id
    virtual KPTNode *findNode(const QString &id) const 
        { return (m_parent ? m_parent->findNode(id) : nodeIdDict.find(id)); }
    /// Remove the node with identity id from the register
    virtual bool removeId(const QString &id) 
        { return (m_parent ? m_parent->removeId(id) : nodeIdDict.remove(id)); }
    /// Insert the node with identity id
    virtual void insertId(const QString &id, const KPTNode *node)
        { m_parent ? m_parent->insertId(id, node) : nodeIdDict.insert(id, node); }
    
    KPTResourceGroup *findResourceGroup(const QString &id) const 
        { return resourceGroupIdDict.find(id); }
    /// Remove the resourcegroup with identity id from the register
    bool removeResourceGroupId(const QString &id) 
        { return resourceGroupIdDict.remove(id); }
    /// Insert the resourcegroup with identity id
    void insertResourceGroupId(const QString &id, const KPTResourceGroup* group) 
        { resourceGroupIdDict.insert(id, group); }
    
    KPTResource *findResource(const QString &id) const 
        { return resourceIdDict.find(id); }
    /// Remove the resource with identity id from the register
    bool removeResourceId(const QString &id) 
        { return resourceIdDict.remove(id); }
    /// Insert the resource with identity id
    void insertResourceId(const QString &id, const KPTResource *resource) 
        { resourceIdDict.insert(id, resource); }

    /// Find the calendar with identity id
    virtual KPTCalendar *findCalendar(const QString &id) const 
        { return calendarIdDict.find(id); }
    /// Remove the calendar with identity id from the register
    virtual bool removeCalendarId(const QString &id) 
        { return calendarIdDict.remove(id); }
    /// Insert the calendar with identity id
    virtual void insertCalendarId(const QString &id, const KPTCalendar *calendar)
        { calendarIdDict.insert(id, calendar); }
    
    /**
     * Setting a project to be baselined means the project data can not be edited anymore.
     * @param on the new baseline value
     */
    void setBaselined(bool on) { m_baselined = on; }
    /**
     * @return if the project is baselined; a baselined project becomes uneditable.
     */
    bool isBaselined() const { return m_baselined; }
    
    void generateWBS(int count, KPTWBSDefinition &def, QString wbs=QString());

protected:
    QPtrList<KPTResourceGroup> m_resourceGroups;

    KPTCalendar *m_defaultCalendar;
    QPtrList<KPTCalendar> m_calendars;

    KPTStandardWorktime *m_standardWorktime;
        
    KPTDateTime calculateForward(int use);
    KPTDateTime calculateBackward(int use);
    KPTDateTime &scheduleForward(KPTDateTime &earliest, int use);
    KPTDateTime &scheduleBackward(KPTDateTime &latest, int use);
    void adjustSummarytask();

    void initiateCalculation();
    void initiateCalculationLists(QPtrList<KPTNode> &startnodes, QPtrList<KPTNode> &endnodes, QPtrList<KPTNode> &summarytasks);

    bool legalParents(KPTNode *par, KPTNode *child);
    bool legalChildren(KPTNode *par, KPTNode *child);

private:
    void init();
    
    QPtrList<KPTNode> m_startNodes;
    QPtrList<KPTNode> m_endNodes;
    QPtrList<KPTNode> m_summarytasks;
    
    bool m_baselined;
    
    QDict<KPTResourceGroup> resourceGroupIdDict;
    QDict<KPTResource> resourceIdDict;
    QDict<KPTNode> nodeIdDict;        
    QDict<KPTCalendar> calendarIdDict;
    
#ifndef NDEBUG
#include <qcstring.h>
public:
    void printDebug(bool children, QCString indent);
    void printCalendarDebug(QCString indent="");
#ifdef DEBUGPERT
    static void pert_test();
    static void printTree(KPTNode *n, QString s);
#endif
#endif
};

}  //KPlato namespace

#endif
