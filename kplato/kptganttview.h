/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTGANTTVIEW_H
#define KPTGANTTVIEW_H

#include <qsplitter.h>
#include <qcursor.h>

class QLayout;
class QListViewItem;
class QPoint;
class QListView;
class QLineEdit;
class QSpinBox;

class KDGanttView;
class KDGanttViewSummaryItem;
class KDGanttViewTaskItem;
class KDGanttViewEventItem;
class KDGanttViewItem;
class KDGanttViewTaskLink;

class KPrinter;

namespace KPlato
{

class TaskAppointmentsView;

class View;
class Node;
class Task;
class Project;
class Relation;
class Context;

class GanttView : public QSplitter
{
    Q_OBJECT

 public:

    GanttView(View *view, QWidget *parent, bool readWrite=true, const char* name = 0  );

    //~GanttView();

    void zoom(double zoom);

    void draw(Project &project);
    void drawChanges(Project &project);
    View *mainView();

    Node *currentNode() const;

    void clear();
    void print(KPrinter &prts);

    void addTaskLink(KDGanttViewTaskLink *link);
    
    bool exportGantt(QIODevice* device); // testing
    
    virtual bool setContext(Context &context);
    virtual void getContext(Context &context) const;

    void setReadWriteMode(bool on);
    bool isReadWriteMode() const { return m_readWrite; }
    
signals:
    void modifyRelation(Relation *rel) ;
    void addRelation(Node *par, Node *child);
    void modifyRelation(Relation *rel, int linkType) ;
    void addRelation(Node *par, Node *child, int linkType);
    
public slots:
    void popupMenuRequested(KDGanttViewItem * item, const QPoint & pos, int);

    void setShowResources(bool on) { m_showResources = on; }
    void setShowTaskName(bool on) { m_showTaskName = on; }
    void setShowTaskLinks(bool on) { m_showTaskLinks = on; }
    void setShowProgress(bool on) { m_showProgress = on; }
    void setShowPositiveFloat(bool on) { m_showPositiveFloat = on; }
    void setShowCriticalTasks(bool on) { m_showCriticalTasks = on; }
    void setShowCriticalPath(bool on) { m_showCriticalPath = on; }

private slots:
    void currentItemChanged(KDGanttViewItem *);
    void slotItemDoubleClicked(KDGanttViewItem*);
    void slotItemRenamed(KDGanttViewItem*, int, const QString&);
    
    void slotLinkItems(KDGanttViewItem* from, KDGanttViewItem* to, int linkType);
    
    void slotGvItemClicked(KDGanttViewItem*);
    
    void slotModifyLink(KDGanttViewTaskLink* link);

protected:
    int linkTypeToRelation(int linkType);
    void setRenameEnabled(QListViewItem *item, bool on);
private:
    KDGanttViewItem *findItem(Node *node);
    KDGanttViewItem *findItem(Node *node, KDGanttViewItem *item);
    Node *getNode(KDGanttViewItem *item) const;
    bool isDrawn(KDGanttViewItem *item);
    void setDrawn(KDGanttViewItem *item, bool state);
    void resetDrawn(KDGanttViewItem *_item);
    void removeNotDrawn(KDGanttViewItem *_item);
    void deleteItem(KDGanttViewItem *item);
    KDGanttViewItem *correctType(KDGanttViewItem *item, Node *node);
    void correctPosition(KDGanttViewItem *item, Node *node);
    KDGanttViewItem *correctParent(KDGanttViewItem *item, Node *node);

    void updateChildren(Node *node);
    void updateNode(Node *node);
    
    void modifyChildren(Node *node);
    void modifyNode(Node *node);    
    void modifyProject(KDGanttViewItem *item, Node *node);
    void modifySummaryTask(KDGanttViewItem *item, Task *task);
    void modifyTask(KDGanttViewItem *item, Task *task);
    void modifyMilestone(KDGanttViewItem *item, Task *task);
    
    KDGanttViewItem *addNode(KDGanttViewItem *parentItem, Node *node,KDGanttViewItem *after=0);
    
    KDGanttViewItem *addProject(KDGanttViewItem *parentItem, Node *node, KDGanttViewItem *after=0);
    KDGanttViewItem *addSubProject(KDGanttViewItem *parentItem, Node *node, KDGanttViewItem *after=0);
    KDGanttViewItem *addSummaryTask(KDGanttViewItem *parentItem, Task *task, KDGanttViewItem *after=0);
    KDGanttViewItem *addTask(KDGanttViewItem *parentItem, Task *task, KDGanttViewItem *after=0);
    KDGanttViewItem *addMilestone(KDGanttViewItem *parentItem, Task *task, KDGanttViewItem *after=0);
    
    void drawChildren(KDGanttViewItem *item, Node &node);
    void drawProject(KDGanttViewItem *parentItem, Node *node);
    void drawSubProject(KDGanttViewItem *parentItem, Node *node);
    void drawSummaryTask(KDGanttViewItem *parentItem, Task *task);
    void drawTask(KDGanttViewItem *parentItem, Task *task);
    void drawMilestone(KDGanttViewItem *parentItem, Task *task);

    void drawRelations();
    void drawRelations(KDGanttViewItem *item);
    void drawChildRelations(KDGanttViewItem *item);

    void getContextClosedNodes(Context &context, KDGanttViewItem *item) const;
    
private:
    View *m_mainview;
    bool m_readWrite;
    int m_defaultFontSize;
    KDGanttViewItem *m_currentItem;
    KDGanttView *m_gantt;
    TaskAppointmentsView *m_taskView;
    bool m_showResources;
    bool m_showTaskName;
    bool m_showTaskLinks;
    bool m_showProgress;
    bool m_showPositiveFloat;
    bool m_showCriticalTasks;
    bool m_showCriticalPath;
    bool m_firstTime;
    QPtrList<KDGanttViewTaskLink> m_taskLinks;
};

}  //KPlato namespace

#endif
