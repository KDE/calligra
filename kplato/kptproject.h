/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef kptproject_h
#define kptproject_h

#include "kptnode.h"
#include "kpttask.h"

#include "kptduration.h"
#include "kptresource.h"

#include <qmap.h>
#include <qptrlist.h>

#include <list>
#include <algorithm>

namespace KPlato
{

class KPTCalendar;
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

    virtual int type() const;

    /**
     * Calculate the whole project
     */
    void calculate(KPTEffort::Use use=KPTEffort::Use_Expected);
    /**
     * The expected Duration is the expected time to complete a Task, Project,
     * etc. For an individual Task, this will calculate the expected duration
     * by querying the Distribution of the Task. If the Distribution is a
     * simple RiskNone, the value will equal the mode Duration, but for other
     * Distributions like RiskHigh, the value will have to be calculated. For
     * a Project or Subproject, the expected Duration is calculated by
     * PERT/CPM.
     */
    KPTDuration *getExpectedDuration();

    /**
     * Instead of using the expected duration, generate a random value using
     * the Distribution of each Task. This can be used for Monte-Carlo
     * estimation of Project duration.
     */
    KPTDuration *getRandomDuration();

    /**
     * Retrieve the calculated float of this node
     */
    KPTDuration *getFloat();

    virtual bool load(QDomElement &element);
    virtual void save(QDomElement &element) ;

    KPTDateTime getEarliestStart() const { return earliestStart; }
    KPTDateTime getLatestFinish() const { return latestFinish; }

    QPtrList<KPTResourceGroup> &resourceGroups();
    virtual void addResourceGroup(KPTResourceGroup *resource);
    virtual void insertResourceGroup(unsigned int index, KPTResourceGroup *resource);
    void removeResourceGroup(KPTResourceGroup *resource);
    void removeResourceGroup(int number);

    QPtrList<KPTAppointment> appointments(const KPTTask *task);

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

    KPTResourceGroup *group(QString id);

    KPTResource *resource(QString id);

    /**
     * Returns the total planned cost for this project
     */
    virtual double plannedCost();
    /**
     * Returns the planned cost for this project upto date @dt
     */
    virtual double plannedCost(QDateTime &dt);
    /**
     * Returns the actually reported cost for this project
     */
    virtual double actualCost();

    QPtrList<KPTAppointment> appointments(const KPTNode *node);

    KPTCalendar *defaultCalendar() { return m_defaultCalendar; }
    const QPtrList<KPTCalendar> &calendars() const { return m_calendars; }
    void addCalendar(KPTCalendar *calendar);
    KPTCalendar *calendar(const QString id) const;

    /**
     * Used eg. for estimation and calculation of effort, 
     * and presentation in gantt chart.
     */    
    KPTStandardWorktime *standardWorktime() { return m_standardWorktime; }
    void addStandardWorktime(KPTStandardWorktime * worktime); //FIXME

    /// Check if node par can be linked to node child.
    bool legalToLink(KPTNode *par, KPTNode *child);

    KPTTask &defaultTask() { return m_defaultTask; }
    
protected:
    QPtrList<KPTResourceGroup> m_resourceGroups;

    KPTCalendar *m_defaultCalendar;
    QPtrList<KPTCalendar> m_calendars;

    KPTStandardWorktime *m_standardWorktime;
    KPTTask m_defaultTask;
        
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
    QPtrList<KPTNode> m_startNodes;
    QPtrList<KPTNode> m_endNodes;
    QPtrList<KPTNode> m_summarytasks;
    
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
