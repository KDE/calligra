/* This file is part of the KDE project
   Copyright (C) 2002 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTGANTTVIEW_H
#define KPTGANTTVIEW_H

#include <qsplitter.h>

class KPTView;
class KPTNode;
class KPTTask;
class KPTProject;
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

class KPTGanttView : public QSplitter
{
    Q_OBJECT

 public:

    KPTGanttView( KPTView *view, QWidget *parent, const char* name = 0  );

    //~KPTGanttView();

    void zoom(double zoom);

    void draw(KPTProject &project);
    void drawChanges(KPTProject &project);
    KPTView *mainView();

    KPTNode *currentNode();

    void clear();
    void print(KPrinter &printer);

    void addTaskLink(KDGanttViewTaskLink *link);
        
public slots:
    void popupMenuRequested(KDGanttViewItem * item, const QPoint & pos, int);

private slots:
    void currentItemChanged(KDGanttViewItem *);
    void slotItemDoubleClicked(KDGanttViewItem*);

private:
    KDGanttViewItem *findItem(KPTNode *node);
    KDGanttViewItem *findItem(KPTNode *node, KDGanttViewItem *item);
    KPTNode *getNode(KDGanttViewItem *item);
    void removeDeleted(KDGanttViewItem *item);
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

private:
    KPTView *m_mainview;
    int m_defaultFontSize;
    KDGanttViewItem *m_currentItem;
    KDGanttView *m_gantt;
    class KPTTaskAppointmentsView *m_taskView;
    bool m_showSlack;
    bool m_firstTime;
    QPtrList<KDGanttViewTaskLink> m_taskLinks;
};
 #endif
