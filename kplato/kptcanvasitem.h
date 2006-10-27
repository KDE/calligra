/* This file is part of the KDE project
   Copyright (C) 2003 - 2004 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTCANVASITEM_H
#define KPTCANVASITEM_H

#include "kptnode.h"
#include "KDGanttView.h"
#include "KDGanttViewItem.h"
#include "KDGanttViewSummaryItem.h"
#include "KDGanttViewTaskItem.h"
#include "KDGanttViewEventItem.h"

#include <q3canvas.h>
#include <qrect.h>
//Added by qt3to4:
#include <Q3PointArray>
#include <Q3PtrList>

class QPainter;

namespace KPlato
{

class Task;
class Relation;
class GanttView;


class ItemBase
{
protected:
    KDGanttViewTaskLink::LinkType kdLinkType(int relationType);
};


/////////////////   GanttViewSummaryItem   ////////////////////

class GanttViewSummaryItem : public KDGanttViewSummaryItem, public ItemBase
{
public:
    GanttViewSummaryItem(KDGanttView *parent, Node *node);
    GanttViewSummaryItem(KDGanttViewItem *parent, Node *node);

    Node *getNode() { return m_node; }
    void insertRelations(GanttView *view);
    KDGanttViewItem *find(Node *node);
    KDGanttViewItem *find(KDGanttViewItem *item, Node *node);
    KDGanttView *ganttView() const { return m_view; }
    bool isDrawn() const { return m_drawn; }
    void setDrawn(bool drawn) { m_drawn = drawn; }

protected:
    Node *m_node;  // can be Project or Task
    KDGanttView *m_view;
    bool m_drawn;
};

/////////////////   GanttViewTaskItem   ////////////////////

class GanttViewTaskItem : public KDGanttViewTaskItem, public ItemBase
{
public:
    GanttViewTaskItem(KDGanttView *parent, KPlato::Task *task);
    GanttViewTaskItem(KDGanttViewItem *parent, KPlato::Task *task);

    KPlato::Task *getTask() const { return m_task; }
    void insertRelations(GanttView *view);
    KDGanttViewItem *find(Node *node);
    KDGanttViewItem *find(KDGanttViewItem *item, Node *node);
    KDGanttView *ganttView() const { return m_view; }
    bool isDrawn() const { return m_drawn; }
    void setDrawn(bool drawn) { m_drawn = drawn; }
    
protected:
    KPlato::Task *m_task;
    KDGanttView *m_view;
    bool m_drawn;
};

/////////////////   GanttViewEventItem   ////////////////////

class GanttViewEventItem : public KDGanttViewEventItem, public ItemBase
{
public:
    GanttViewEventItem(KDGanttView *parent, KPlato::Task *task);
    GanttViewEventItem(KDGanttViewItem *parent, KPlato::Task *task);

    KPlato::Task *getTask() { return m_task; }
    void insertRelations(GanttView *view);
    KDGanttViewItem *find(Node *node);
    KDGanttViewItem *find(KDGanttViewItem *item, Node *node);
    KDGanttView *ganttView() const { return m_view; }
    bool isDrawn() const { return m_drawn; }
    void setDrawn(bool drawn) { m_drawn = drawn; }
    
protected:
    KPlato::Task *m_task;
    KDGanttView *m_view;
    bool m_drawn;
};

}  //KPlato namespace

#endif
