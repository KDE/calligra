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

class KPTTask;
class KPTRelation;
class KPTPertCanvas;
class KPTPertRelationItem;
class KPTPertNodeItem;
class KPTGanttView;

class KPTPertNodeItem : public QCanvasPolygon
{
private:
    class Relation
    {
    public:
        Relation(KPTRelation *r, KPTPertNodeItem *n) { relation = r; childItem = n; }
        ~Relation() {}
        KPTRelation *relation;
        KPTPertNodeItem * childItem;
    };

public:
    KPTPertNodeItem( KPTPertCanvas *view, KPTNode &node, int row, int col );
    virtual ~KPTPertNodeItem();

    virtual int rtti() const;
    static int RTTI;

	void setVisible(bool yes);
    void move(KPTPertCanvas *view, int row, int col);

    QPoint exitPoint(KPTRelation::Type type) const;
	QPoint entryPoint(KPTRelation::Type type) const;

	KPTNode &node() const { return m_node; }

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

    void addChildRelation(KPTRelation *relation, KPTPertNodeItem *node)
        { m_childRelations.append(new Relation(relation, node)); }

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

    QPtrList<Relation> m_childRelations;

private:
    KPTNode &m_node;
    int m_row, m_col;
	QPoint m_right; // Entry/exit point
	QPoint m_left;  // Entry/exit point
	QCanvasText *m_name;
	QCanvasText *m_leader;

#ifndef NDEBUG
    void printDebug( int );
#endif

};

class KPTPertProjectItem : public KPTPertNodeItem
{
public:
    KPTPertProjectItem( KPTPertCanvas *view, KPTNode &node, int row=-1, int col=-1 );
    virtual ~KPTPertProjectItem();

    virtual int rtti() const;
    static int RTTI;

#ifndef NDEBUG
    void printDebug( int );
#endif

};

class KPTPertTaskItem : public KPTPertNodeItem
{
public:
    KPTPertTaskItem( KPTPertCanvas *view, KPTNode &node, int row=-1, int col=-1 );
    virtual ~KPTPertTaskItem();

    virtual int rtti() const;
    static int RTTI;

#ifndef NDEBUG
    void printDebug( int );
#endif

};

class KPTPertMilestoneItem : public KPTPertNodeItem
{
public:
    KPTPertMilestoneItem( KPTPertCanvas *view, KPTNode &node, int row=-1, int col=-1 );
    virtual ~KPTPertMilestoneItem();

    virtual int rtti() const;
    static int RTTI;

	void draw();

#ifndef NDEBUG
    void printDebug( int );
#endif

};

/////////////////   KPTPertRelationItem   ////////////////////

class KPTPertRelationItem : public QCanvasPolygon
{
public:
    KPTPertRelationItem(KPTPertCanvas *view, KPTPertNodeItem *parent, KPTPertNodeItem *child, KPTRelation *rel);
    virtual ~KPTPertRelationItem();

    virtual int rtti() const;
    static int RTTI;

    KPTRelation::Type type() { return m_rel->type(); }
    void draw();

	void setFinishStartPoints();
	void setFinishFinishPoints();
	void setStartStartPoints();
	QPointArray areaPoints() const;

	bool rowFree(int row, int startCol, int endCol);

protected:
    void drawShape(QPainter &p);

private:
    KPTPertCanvas *m_view;
    KPTRelation *m_rel;
    KPTPertNodeItem *m_parentItem;
    KPTPertNodeItem *m_childItem;
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

class KPTItemBase
{
protected:
    KDGanttViewTaskLink::LinkType kdLinkType(int relationType);
};


/////////////////   KPTGanttViewSummaryItem   ////////////////////

class KPTGanttViewSummaryItem : public KDGanttViewSummaryItem, public KPTItemBase
{
public:
    KPTGanttViewSummaryItem(KDGanttView *parent, KPTNode *node);
    KPTGanttViewSummaryItem(KDGanttViewItem *parent, KPTNode *node);

	KPTNode *getNode() { return m_node; }
    void insertRelations(KPTGanttView *view);
    KDGanttViewItem *find(KPTNode *node);
    KDGanttViewItem *find(KDGanttViewItem *item, KPTNode *node);
    KDGanttView *ganttView() const { return m_view; }
    bool isDrawn() const { return m_drawn; }
    void setDrawn(bool drawn) { m_drawn = drawn; }

protected:
    KPTNode *m_node;  // can be KPTProject or KPTTask
    KDGanttView *m_view;
    bool m_drawn;
};

/////////////////   KPTGanttViewTaskItem   ////////////////////

class KPTGanttViewTaskItem : public KDGanttViewTaskItem, public KPTItemBase
{
public:
    KPTGanttViewTaskItem(KDGanttView *parent, KPTTask *task);
    KPTGanttViewTaskItem(KDGanttViewItem *parent, KPTTask *task);

	KPTTask *getTask() const { return m_task; }
    void insertRelations(KPTGanttView *view);
    KDGanttViewItem *find(KPTNode *node);
    KDGanttViewItem *find(KDGanttViewItem *item, KPTNode *node);
    KDGanttView *ganttView() const { return m_view; }
    bool isDrawn() const { return m_drawn; }
    void setDrawn(bool drawn) { m_drawn = drawn; }
    
protected:
    KPTTask *m_task;
    KDGanttView *m_view;
    bool m_drawn;
};

/////////////////   KPTGanttViewEventItem   ////////////////////

class KPTGanttViewEventItem : public KDGanttViewEventItem, public KPTItemBase
{
public:
    KPTGanttViewEventItem(KDGanttView *parent, KPTTask *task);
    KPTGanttViewEventItem(KDGanttViewItem *parent, KPTTask *task);

	KPTTask *getTask() { return m_task; }
    void insertRelations(KPTGanttView *view);
    KDGanttViewItem *find(KPTNode *node);
    KDGanttViewItem *find(KDGanttViewItem *item, KPTNode *node);
    KDGanttView *ganttView() const { return m_view; }
    bool isDrawn() const { return m_drawn; }
    void setDrawn(bool drawn) { m_drawn = drawn; }
    
protected:
    KPTTask *m_task;
    KDGanttView *m_view;
    bool m_drawn;
};

}  //KPlato namespace

#endif
