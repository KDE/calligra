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

#include "kptcanvasitem.h"

#include <qpainter.h>
#include <qpointarray.h>

#include <kdebug.h>
#include <koRect.h> //DEBUGRECT

KPTPertCanvasItem::KPTPertCanvasItem( QCanvas *canvas, KPTNode &node, int row, int col )
    : QCanvasRectangle(canvas),
	m_node(node),
	m_row(row),
	m_col(col)
{
    //kdDebug()<<k_funcinfo<<"Node="<<node.name()<<" ("<<row<<","<<col<<")"<<endl;
	int wgap = 20;
	int hgap = 10;
	int w = 100;
	int h = 30;
	int left = 6 + col * (w + wgap);
	int top = 6 + row * (h + hgap);
	int bottom = top + h;
	int right = left + w;
    m_left = QPoint(left, top + h/2);
	m_right = QPoint(right, m_left.y());
	
	setX(left);
	setY(top);
	setSize(w,h);
	setBrush(Qt::yellow);
	setPen(Qt::black);
	setZ(50);
	
	m_name = new QCanvasText(node.name(), canvas);
	m_name->setX(left + 3);
	m_name->setY(top + 3);
	m_name->setZ(60);
	
	m_leader = new QCanvasText(node.leader(), canvas);
	m_leader->setX(left + 3);
	m_leader->setY(top + 15);
	m_leader->setZ(60);
    //kdDebug()<<k_funcinfo<<" end"<<endl;
}

KPTPertCanvasItem::~KPTPertCanvasItem()
{
    hide();
/*    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it) 
    {
        if ( *it == m_name )
		    delete m_name;
        if ( *it == m_leader )
		    delete m_leader;
    }*/
}

int KPTPertCanvasItem::rtti() const { return RTTI; }
int KPTPertCanvasItem::RTTI = 2001;

void KPTPertCanvasItem::setVisible(bool yes)
{
    kdDebug()<<k_funcinfo<<m_node.name()<<endl;
	QCanvasRectangle::setVisible(yes);
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it) 
    {
        if ( *it == m_name )
            m_name->setVisible(yes);
        if ( *it == m_leader )
	        m_leader->setVisible(yes);
    }
}

void KPTPertCanvasItem::drawShape(QPainter &p)
{
    kdDebug()<<k_funcinfo<<m_node.name()<<" areaPoints:"<<endl;
	QPointArray pa = areaPoints();
/*	kdDebug()<<"      "<<pa[0].x()<<","<<pa[0].y()<<"      "<<pa[1].x()<<","<<pa[1].y()<<endl;
	kdDebug()<<"      "<<pa[2].x()<<","<<pa[2].y()<<"      "<<pa[3].x()<<","<<pa[3].y()<<endl;*/
	QCanvasRectangle::drawShape(p);
}

QPoint KPTPertCanvasItem::exitPoint(TimingRelation type)
{
    QPoint ret;
    switch(type)
	{
	    case FINISH_START:
		case FINISH_FINISH:
		    ret = m_right;
			break;
		case START_START:
		    ret = m_left;
			break;
	}
	return ret;
}

QPoint KPTPertCanvasItem::entryPoint(TimingRelation type)
{
    QPoint ret;
    switch(type)
	{
	    case FINISH_START:
		    ret = m_left;
			break;
		case FINISH_FINISH:
		    ret = m_right;
			break;
		case START_START:
		    ret = m_left;
			break;
	}
	return ret;
}

#ifndef NDEBUG
void KPTPertCanvasItem::printDebug( int /*info*/ )
{
}
#endif


////////////////////   KPTRelationCanvasItem   //////////////////////////

KPTRelationCanvasItem::KPTRelationCanvasItem( QCanvas *canvas, KPTRelation *rel)
    : QCanvasPolygon(canvas),
	m_rel(rel)
{
    kdDebug()<<k_funcinfo<<endl;
	QPoint parentPoint = m_rel->parent()->pertItem()->exitPoint(FINISH_START);
	QRect parentRect = m_rel->parent()->pertItem()->rect();
	QPoint childPoint = m_rel->child()->pertItem()->entryPoint(FINISH_START);
	QRect childRect = m_rel->child()->pertItem()->rect();
	
	int childRow = m_rel->child()->pertItem()->row();
	int childCol =  m_rel->child()->pertItem()->column();
	int parentRow = m_rel->parent()->pertItem()->row();
	int parentCol =  m_rel->parent()->pertItem()->column();

	int wgap = 20;
	int hgap = 10;
	QPointArray a;
	a.putPoints(0, 1, parentPoint.x(), parentPoint.y());
	

    if ( parentRow == childRow )
	{
	    if (parentCol == childCol - 1)
		{
        	a.putPoints(1, 1, childPoint.x(), childPoint.y());
			
			left = a[0].x(); top = a[0].y(); right = a[1].x(); bottom = a[1].y();
		}
		else // go around
		{
    	    a.putPoints(1, 1, parentPoint.x()+wgap/2, parentPoint.y());
    	    a.putPoints(2, 1, parentPoint.x()+wgap/2, parentRect.top() - hgap/3);
    	    a.putPoints(3, 1, childPoint.x() - wgap/2, parentRect.top() - hgap/3);
    	    a.putPoints(4, 1, childPoint.x() - wgap/2, childPoint.y());
    	    a.putPoints(5, 1, childPoint.x(), childPoint.y());
			
			left = a[0].x(); top = a[2].y(); right = a[5].x(); bottom = a[5].y();
		}
	} 
	else if ( parentRow > childRow )
	{
	    if (parentCol == childCol - 1)
		{
            a.putPoints(1, 1, parentPoint.x()+wgap/2, parentPoint.y());
            a.putPoints(2, 1, parentPoint.x()+wgap/2, childPoint.y());
            a.putPoints(3, 1, childPoint.x(), childPoint.y());
	    
			left = a[0].x(); top = a[0].y(); right = a[3].x(); bottom = a[3].y();
		}
		else
		{
            a.putPoints(1, 1, parentPoint.x()+wgap/2, parentPoint.y());
            a.putPoints(2, 1, parentPoint.x()+wgap/2, childRect.top()-hgap/3);
            a.putPoints(3, 1, childPoint.x()-wgap/2, childRect.top()-hgap/3);
            a.putPoints(4, 1, childPoint.x()-wgap/2, childPoint.y());
    	    a.putPoints(5, 1, childPoint.x(), childPoint.y());		
		
			left = a[0].x(); top = a[0].y(); right = a[5].x(); bottom = a[5].y();
		}
	}
	else if ( parentRow < childRow )
	{
	    if (parentCol == childCol - 1)
		{
		    a.putPoints(1, 1, parentPoint.x()+wgap/2, parentPoint.y());
		    a.putPoints(2, 1, parentPoint.x()+wgap/2, childPoint.y());
    	    a.putPoints(3, 1, childPoint.x(), childPoint.y());		
			
			left = a[0].x(); top = a[3].y(); right = a[3].x(); bottom = a[3].y();
		}
		else
		{
	        a.putPoints(1, 1, parentPoint.x()+wgap/2, childRect.top()-hgap/3);	
	        a.putPoints(2, 1, parentPoint.x()+wgap/2, parentRect.top()-hgap/3);	
	        a.putPoints(3, 1, childPoint.x()-wgap/2, parentRect.top()-hgap/3);	
	        a.putPoints(4, 1, childPoint.x()-wgap/2, childPoint.y()-5);	
	        a.putPoints(5, 1, childPoint.x(), childPoint.y());	
			
			left = a[0].x(); top = a[0].y(); right = a[5].x(); bottom = a[5].y();
        }
	}
	setPoints(a);
	setPen(Qt::black);
	setZ(45);
}

KPTRelationCanvasItem::~KPTRelationCanvasItem()
{
    hide();
}

int KPTRelationCanvasItem::rtti() const { return RTTI; }
int KPTRelationCanvasItem::RTTI = 2002;

void KPTRelationCanvasItem::drawShape(QPainter &p)
{
    kdDebug()<<k_funcinfo<<" "<<m_rel->parent()->name()<<" to "<<m_rel->child()->name()<<endl;
    // cannot use polygon's drawShape() as it doesn't use the pen
	setBrush(Qt::NoBrush);
	QPointArray a = poly;
	int size = a.size()-1;
	for(int i = 0; i < size; ++i)
	{
        //kdDebug()<<k_funcinfo<<" draw["<<i<<"]: "<<a[i].x()<<","<<a[i].y()<<" to "<<a[i+1].x()<<","<<a[i+1].y()<<endl;
	    p.drawLine(a[i], a[i+1]);
	}
}

QPointArray KPTRelationCanvasItem::areaPoints () const
{
    QPointArray pa(4);
    int pw = (pen().width()+1)/2;
    if ( pw < 1 ) pw = 1;
    if ( pen() == NoPen ) pw = 0;
    pa[0] = QPoint(left-pw,top-pw);
    pa[1] = pa[0] + QPoint(right-left+pw*2,0);
    pa[2] = pa[1] + QPoint(0,bottom-top+pw*2);
    pa[3] = pa[0] + QPoint(0,bottom-top+pw*2);
/*    kdDebug()<<k_funcinfo<<" areaPoints: "<<m_rel->parent()->name()<<" to "<<m_rel->child()->name()<<endl;
	kdDebug()<<"      "<<pa[0].x()<<","<<pa[0].y()<<"      "<<pa[1].x()<<","<<pa[1].y()<<endl;
	kdDebug()<<"      "<<pa[2].x()<<","<<pa[2].y()<<"      "<<pa[3].x()<<","<<pa[3].y()<<endl;*/
    return pa;
}

#ifndef NDEBUG
void KPTRelationCanvasItem::printDebug( int /*info*/ )
{
}
#endif
