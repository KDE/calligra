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

#include <qcanvas.h>
#include <qrect.h>

class QPainter;

namespace KPlato
{

class Task;
class Relation;
class PertCanvas;
class PertRelationItem;
class PertNodeItem;
class GanttView;

class PertNodeItem : public QCanvasPolygon
{
private:
    class PertNodeRelation
    {
    public:
        PertNodeRelation(Relation *r, PertNodeItem *n) { relation = r; childItem = n; }
        ~PertNodeRelation() {}
        Relation *relation;
        PertNodeItem * childItem;
    };

public:
    PertNodeItem( PertCanvas *view, Node &node, int row, int col );
    virtual ~PertNodeItem();

    virtual int rtti() const;
    static int RTTI;

	void setVisible(bool yes);
    void move(PertCanvas *view, int row, int col);

    QPoint exitPoint(Relation::Type type) const;
	QPoint entryPoint(Relation::Type type) const;

	Node &node() const { return m_node; }

	QRect rect() const { return QRect(m_left, m_right); }
    void setRow(int row) { m_row = row; }
	int row() const { return m_row; }
    void setColumn(int col) { m_col = col; }
	int column() const { return m_col; }
	int x() const { return m_x; }
	int x(int col) const { return m_wgap + col*(m_width+m_wgap); }
	int y() const { return m_y; }
	int y(int row) const { return m_hgap + row*(m_height+m_hgap); }
	int width() const { return m_width; }
	int height() const { return m_height; }

    void addChildRelation(Relation *relation, PertNodeItem *node)
        { m_childRelations.append(new PertNodeRelation(relation, node)); }

    bool hasParent() { return m_node.numDependParentNodes(); }
    bool hasChild() { return m_node.numDependChildNodes(); }

protected:
    void drawShape(QPainter & p);

	int m_wgap;
	int m_hgap;
	int m_width;
	int m_height;
	int m_x;
	int m_y;

    QPtrList<PertNodeRelation> m_childRelations;

private:
    Node &m_node;
    int m_row, m_col;
	QPoint m_right; // Entry/exit point
	QPoint m_left;  // Entry/exit point
	QCanvasText *m_name;
	QCanvasText *m_leader;

#ifndef NDEBUG
    void printDebug( int );
#endif

};

class PertProjectItem : public PertNodeItem
{
public:
    PertProjectItem( PertCanvas *view, Node &node, int row=-1, int col=-1 );
    virtual ~PertProjectItem();

    virtual int rtti() const;
    static int RTTI;

#ifndef NDEBUG
    void printDebug( int );
#endif

};

class PertTaskItem : public PertNodeItem
{
public:
    PertTaskItem( PertCanvas *view, Node &node, int row=-1, int col=-1 );
    virtual ~PertTaskItem();

    virtual int rtti() const;
    static int RTTI;

#ifndef NDEBUG
    void printDebug( int );
#endif

};

class PertMilestoneItem : public PertNodeItem
{
public:
    PertMilestoneItem( PertCanvas *view, Node &node, int row=-1, int col=-1 );
    virtual ~PertMilestoneItem();

    virtual int rtti() const;
    static int RTTI;

	void draw();

#ifndef NDEBUG
    void printDebug( int );
#endif

};

/////////////////   PertRelationItem   ////////////////////

class PertRelationItem : public QCanvasPolygon
{
public:
    PertRelationItem(PertCanvas *view, PertNodeItem *parent, PertNodeItem *child, Relation *rel);
    virtual ~PertRelationItem();

    virtual int rtti() const;
    static int RTTI;

    Relation::Type type() { return m_rel->type(); }
    void draw();

	void setFinishStartPoints();
	void setFinishFinishPoints();
	void setStartStartPoints();
	QPointArray areaPoints() const;

	bool rowFree(int row, int startCol, int endCol);

protected:
    void drawShape(QPainter &p);

private:
    PertCanvas *m_view;
    Relation *m_rel;
    PertNodeItem *m_parentItem;
    PertNodeItem *m_childItem;
	int left, top, right, bottom;

	int parentTop;
	int parentBottom;
	int childTop;

	int childRow;
	int childCol;
	int parentRow;
	int parentCol;

	int wgap;
	int hgap;


#ifndef NDEBUG
    void printDebug( int );
#endif

};

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
