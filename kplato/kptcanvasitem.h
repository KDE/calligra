/* This file is part of the KDE project
   Copyright (C) 2003 Dag Andersen <danders@get2net.dk>

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

class KPTTask;
class KPTRelation;
class KPTPertCanvas;
class QPainter;

class KPTPertNodeItem : public QCanvasPolygon
{
public:
    KPTPertNodeItem( KPTPertCanvas *view, KPTNode &node, int row, int col );
    virtual ~KPTPertNodeItem();

    virtual int rtti() const;
    static int RTTI;
	
	void setVisible(bool yes);
	
	QPoint exitPoint(TimingRelation type);
	QPoint entryPoint(TimingRelation type);
	
	KPTNode &node() { return m_node; }
	
	QRect rect() { return QRect(m_left, m_right); }
	int row() { return m_row; }
	int column() { return m_col; }
	int x() { return m_x; }
	int y() { return m_y; }
	int width() { return m_width; }
	int height() { return m_height; }
	
protected:
    void drawShape(QPainter & p);

	int m_wgap;
	int m_hgap;
	int m_width;
	int m_height;
	int m_x;
	int m_y;

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
    KPTPertProjectItem( KPTPertCanvas *view, KPTNode &node, int row, int col );
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
    KPTPertTaskItem( KPTPertCanvas *view, KPTNode &node, int row, int col );
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
    KPTPertMilestoneItem( KPTPertCanvas *view, KPTNode &node, int row, int col );
    virtual ~KPTPertMilestoneItem();

    virtual int rtti() const;
    static int RTTI;
	
	
#ifndef NDEBUG
    void printDebug( int );
#endif

};

/////////////////   KPTRelationCanvasItem   ////////////////////

class KPTRelationCanvasItem : public QCanvasPolygon
{
public:
    KPTRelationCanvasItem( KPTPertCanvas *view, KPTRelation *rel );
    virtual ~KPTRelationCanvasItem();

    virtual int rtti() const;
    static int RTTI;
	
	void setFinishStartPoints();
	void setFinishFinishPoints();
	void setStartStartPoints();
	QPointArray areaPoints() const;
	
	bool rowFree(int row, int startCol, int endCol);
	
protected:
    void drawShape(QPainter &p);

private:
    KPTRelation *m_rel;
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

/////////////////   KPTGanttViewSummaryItem   ////////////////////

class KPTGanttViewSummaryItem : public KDGanttViewSummaryItem
{
public:
    KPTGanttViewSummaryItem(KDGanttView *parent, KPTNode &node);
    KPTGanttViewSummaryItem(KDGanttViewItem *parent, KPTNode &node);
	
	KPTNode &getNode() { return m_node; }
	
protected:
    KPTNode & m_node;
};

/////////////////   KPTGanttViewTaskItem   ////////////////////

class KPTGanttViewTaskItem : public KDGanttViewTaskItem
{
public:
    KPTGanttViewTaskItem(KDGanttView *parent, KPTTask *task);
    KPTGanttViewTaskItem(KDGanttViewItem *parent, KPTTask *task);
	
	KPTTask *getTask() { return m_task; }
	
protected:
    KPTTask *m_task;
};

/////////////////   KPTGanttViewEventItem   ////////////////////

class KPTGanttViewEventItem : public KDGanttViewEventItem
{
public:
    KPTGanttViewEventItem(KDGanttView *parent, KPTTask *task);
    KPTGanttViewEventItem(KDGanttViewItem *parent, KPTTask *task);
	
	KPTTask *getTask() { return m_task; }

protected:
    KPTTask *m_task;
};

#endif
