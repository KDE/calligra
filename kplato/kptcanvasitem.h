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

class KPTRelation;
class QPainter;

class KPTPertCanvasItem : public QCanvasRectangle
{
public:
    KPTPertCanvasItem( QCanvas *canvas, KPTNode &node, int row, int col );
    virtual ~KPTPertCanvasItem();

    virtual int rtti() const;
    static int RTTI;
	
	void setVisible(bool yes);
	
	QPoint exitPoint(TimingRelation type);
	QPoint entryPoint(TimingRelation type);
	
	KPTNode &node() { return m_node; }
	
	QRect rect() { return QRect(m_left, m_right); }
	int row() { return m_row; }
	int column() { return m_col; }
	
protected:
    void drawShape(QPainter & p);
	
private:
    KPTNode &m_node;
    int m_row, m_col;
	QPoint m_right;
	QPoint m_left;
	QCanvasText *m_name;
	QCanvasText *m_leader;
	
#ifndef NDEBUG
    void printDebug( int );
#endif

};

/////////////////   KPTRelationCanvasItem   ////////////////////

class KPTRelationCanvasItem : public QCanvasPolygon
{
public:
    KPTRelationCanvasItem( QCanvas *canvas, KPTRelation *rel );
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
    KPTGanttViewTaskItem(KDGanttView *parent, KPTNode &node);
    KPTGanttViewTaskItem(KDGanttViewItem *parent, KPTNode &node);
	
	KPTNode &getNode() { return m_node; }
	
protected:
    KPTNode & m_node;
};

/////////////////   KPTGanttViewEventItem   ////////////////////

class KPTGanttViewEventItem : public KDGanttViewEventItem
{
public:
    KPTGanttViewEventItem(KDGanttView *parent, KPTNode &node);
    KPTGanttViewEventItem(KDGanttViewItem *parent, KPTNode &node);
	
	KPTNode &getNode() { return m_node; }
	
protected:
    KPTNode & m_node;
};

#endif
