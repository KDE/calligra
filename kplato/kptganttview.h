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

class KPTTaskAppointmentsView;

class KPTView;
class KPTNode;
class KPTTask;
class KPTProject;
class KPTRelation;
class KPTContext;

class KPTGanttView : public QSplitter
{
    Q_OBJECT

 public:

    KPTGanttView(KPTView *view, QWidget *parent, bool readWrite=true, const char* name = 0  );

    //~KPTGanttView();

    void zoom(double zoom);

    void draw(KPTProject &project);
    void drawChanges(KPTProject &project);
    KPTView *mainView();

    KPTNode *currentNode() const;

    void clear();
    void print(KPrinter &prts);

    void addTaskLink(KDGanttViewTaskLink *link);
    
    bool exportGantt(QIODevice* device); // testing
    
    virtual bool setContext(KPTContext &context);
    virtual void getContext(KPTContext &context) const;

    void setReadWriteMode(bool on);
    bool isReadWriteMode() const { return m_readWrite; }
    
signals:
    void modifyRelation(KPTRelation *rel) ;
    void addRelation(KPTNode *par, KPTNode *child);
    void modifyRelation(KPTRelation *rel, int linkType) ;
    void addRelation(KPTNode *par, KPTNode *child, int linkType);
    
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
    KDGanttViewItem *findItem(KPTNode *node);
    KDGanttViewItem *findItem(KPTNode *node, KDGanttViewItem *item);
    KPTNode *getNode(KDGanttViewItem *item) const;
    bool isDrawn(KDGanttViewItem *item);
    void setDrawn(KDGanttViewItem *item, bool state);
    void resetDrawn(KDGanttViewItem *_item);
    void removeNotDrawn(KDGanttViewItem *_item);
    void deleteItem(KDGanttViewItem *item);
    KDGanttViewItem *correctType(KDGanttViewItem *item, KPTNode *node);
    void correctPosition(KDGanttViewItem *item, KPTNode *node);
    KDGanttViewItem *correctParent(KDGanttViewItem *item, KPTNode *node);

    void updateChildren(KPTNode *node);
    void updateNode(KPTNode *node);
    
    void modifyChildren(KPTNode *node);
    void modifyNode(KPTNode *node);    
    void modifyProject(KDGanttViewItem *item, KPTNode *node);
    void modifySummaryTask(KDGanttViewItem *item, KPTTask *task);
    void modifyTask(KDGanttViewItem *item, KPTTask *task);
    void modifyMilestone(KDGanttViewItem *item, KPTTask *task);
    
    KDGanttViewItem *addNode(KDGanttViewItem *parentItem, KPTNode *node,KDGanttViewItem *after=0);
    
    KDGanttViewItem *addProject(KDGanttViewItem *parentItem, KPTNode *node, KDGanttViewItem *after=0);
    KDGanttViewItem *addSubProject(KDGanttViewItem *parentItem, KPTNode *node, KDGanttViewItem *after=0);
    KDGanttViewItem *addSummaryTask(KDGanttViewItem *parentItem, KPTTask *task, KDGanttViewItem *after=0);
    KDGanttViewItem *addTask(KDGanttViewItem *parentItem, KPTTask *task, KDGanttViewItem *after=0);
    KDGanttViewItem *addMilestone(KDGanttViewItem *parentItem, KPTTask *task, KDGanttViewItem *after=0);
    
    void drawChildren(KDGanttViewItem *item, KPTNode &node);
    void drawProject(KDGanttViewItem *parentItem, KPTNode *node);
    void drawSubProject(KDGanttViewItem *parentItem, KPTNode *node);
    void drawSummaryTask(KDGanttViewItem *parentItem, KPTTask *task);
    void drawTask(KDGanttViewItem *parentItem, KPTTask *task);
    void drawMilestone(KDGanttViewItem *parentItem, KPTTask *task);

    void drawRelations();
    void drawRelations(KDGanttViewItem *item);
    void drawChildRelations(KDGanttViewItem *item);

    void getContextClosedNodes(KPTContext &context, KDGanttViewItem *item) const;
    
private:
    KPTView *m_mainview;
    bool m_readWrite;
    int m_defaultFontSize;
    KDGanttViewItem *m_currentItem;
    KDGanttView *m_gantt;
    KPTTaskAppointmentsView *m_taskView;
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
