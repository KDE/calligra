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

#include "defs.h"
#include "kptcanvasitem.h"
#include "kptrelation.h"
#include "kptpertcanvas.h"
#include "kpttask.h"

#include <qpainter.h>
#include <qpointarray.h>

#include <kdebug.h>
#include <koRect.h> //DEBUGRECT

KPTPertNodeItem::KPTPertNodeItem( KPTPertCanvas *view, KPTNode &node, int row, int col )
    : QCanvasPolygon(view->canvas()),
	m_node(node),
	m_row(row),
	m_col(col)
{
    //kdDebug()<<k_funcinfo<<"Node="<<node.name()<<" ("<<row<<","<<col<<")"<<endl;
	
	m_wgap = view->verticalGap();
	m_hgap = view->horizontalGap();
	m_width = view->itemSize().width();
	m_height = view->itemSize().height();
	
	m_x = 12 + col * (m_width + m_wgap);
	m_y = 6 + row * (m_height + m_hgap);
    m_left = QPoint(m_x, m_y + m_height/2);
	m_right = QPoint(m_x+m_width, m_left.y());
	
	setPen(Qt::black);
	setZ(50);
	
	m_name = new QCanvasText(node.name(), view->canvas());
	m_name->setX(m_x + 6);
	m_name->setY(m_y + 3);
	m_name->setZ(60);
	
	m_leader = new QCanvasText(node.leader(), view->canvas());
	m_leader->setX(m_x + 6);
	m_leader->setY(m_y + 15);
	m_leader->setZ(60);
}

KPTPertNodeItem::~KPTPertNodeItem()
{
    hide();
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it) 
    {
        if ( *it == m_name )
		    m_name->hide();
        if ( *it == m_leader )
		    m_leader->hide();
    }
}

int KPTPertNodeItem::rtti() const { return RTTI; }
int KPTPertNodeItem::RTTI = 2000;

void KPTPertNodeItem::setVisible(bool yes)
{
    //kdDebug()<<k_funcinfo<<m_node.name()<<endl;
	QCanvasPolygon::setVisible(yes);
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

void KPTPertNodeItem::drawShape(QPainter &p)
{
    //QPen pen(pen());
	if (isSelected())
	    p.setPen(QPen(Qt::red, 2));
	QPointArray a = poly;
	int size = a.size()-1;
	for(int i = 0; i < size; ++i)
	{
        //kdDebug()<<k_funcinfo<<" draw["<<i<<"]: "<<a[i].x()<<","<<a[i].y()<<" to "<<a[i+1].x()<<","<<a[i+1].y()<<endl;
	    p.drawLine(a[i], a[i+1]);
	}
	//setPen(pen);
}

QPoint KPTPertNodeItem::exitPoint(TimingRelation type)
{
    QPoint ret;
    switch(type)
	{
	    case FINISH_START:
		case FINISH_FINISH:
		    ret = m_right + QPoint(pen().width(), 0);
			break;
		case START_START:
		    ret = m_left + QPoint(0, 4);
			break;
	}
	return ret;
}

QPoint KPTPertNodeItem::entryPoint(TimingRelation type)
{
    QPoint ret;
    switch(type)
	{
	    case FINISH_START:
		    ret = m_left - QPoint(pen().width(), 0);
			break;
		case FINISH_FINISH:
		    ret = m_right - QPoint(pen().width(), 4);
			break;
		case START_START:
		    ret = m_left  - QPoint(pen().width(), 0);
			break;
	}
	return ret;
}

#ifndef NDEBUG
void KPTPertNodeItem::printDebug( int /*info*/ )
{
}
#endif

////////////////////   KPTPertProjectItem   //////////////////////////

KPTPertProjectItem::KPTPertProjectItem( KPTPertCanvas *view, KPTNode &node, int row, int col )
    : KPTPertNodeItem(view, node, row, col)
{
    //kdDebug()<<k_funcinfo<<"Node="<<node.name()<<" ("<<row<<","<<col<<")"<<endl;
	
	QPointArray a;
	a.putPoints(0, 5, 
	    m_x+6, m_y, m_x+m_width, m_y, m_x+m_width-6, m_y+m_height, m_x, m_y+m_height, m_x+6, m_y);
	setPoints(a);
	
	setPen(QPen(Qt::cyan, 2));
}

KPTPertProjectItem::~KPTPertProjectItem()
{
}

int KPTPertProjectItem::rtti() const { return RTTI; }
int KPTPertProjectItem::RTTI = 2001;


#ifndef NDEBUG
void KPTPertProjectItem::printDebug( int /*info*/ )
{
}
#endif


////////////////////   KPTPertTaskItem   //////////////////////////

KPTPertTaskItem::KPTPertTaskItem( KPTPertCanvas *view, KPTNode &node, int row, int col )
    : KPTPertNodeItem(view, node, row, col)
{
    //kdDebug()<<k_funcinfo<<"Node="<<node.name()<<" ("<<row<<","<<col<<")"<<endl;
	
	QPointArray a;
	if (node.numChildren() > 0)
	{
	    a.putPoints(0, 5, m_x+6, m_y, m_x+m_width, m_y, m_x+m_width-6, m_y+m_height, m_x, m_y+m_height, m_x+6, m_y);
    	setPen(QPen(Qt::cyan, 2));
	}
	else
	{
    	a.putPoints(0, 5, m_x, m_y, m_x+m_width, m_y, m_x+m_width, m_y+m_height, m_x, m_y+m_height, m_x, m_y);
    	setPen(QPen(Qt::green, 2));
	}
	setPoints(a);
}

KPTPertTaskItem::~KPTPertTaskItem()
{
}

int KPTPertTaskItem::rtti() const { return RTTI; }
int KPTPertTaskItem::RTTI = 2002;


#ifndef NDEBUG
void KPTPertTaskItem::printDebug( int /*info*/ )
{
}
#endif


////////////////////   KPTPertMilestoneItem   //////////////////////////

KPTPertMilestoneItem::KPTPertMilestoneItem( KPTPertCanvas *view, KPTNode &node, int row, int col )
    : KPTPertNodeItem(view, node, row, col)
{
    //kdDebug()<<k_funcinfo<<"Node="<<node.name()<<" ("<<row<<","<<col<<")"<<endl;
	
	QPointArray a;
	a.putPoints(0, 7, 
	    m_x, m_y+m_height/2,
	    m_x+6, m_y, 
		m_x+m_width-6, m_y, 
		m_x+m_width, m_y+m_height/2,
		m_x+m_width-6, m_y+m_height, 
	    m_x+6, m_y+m_height, 
	    m_x, m_y+m_height/2);
	
	setPoints(a);
	
	setPen(QPen(Qt::blue, 2));
}

KPTPertMilestoneItem::~KPTPertMilestoneItem()
{
}

int KPTPertMilestoneItem::rtti() const { return RTTI; }
int KPTPertMilestoneItem::RTTI = 2003;


#ifndef NDEBUG
void KPTPertMilestoneItem::printDebug( int /*info*/ )
{
}
#endif


////////////////////   KPTRelationCanvasItem   //////////////////////////

KPTRelationCanvasItem::KPTRelationCanvasItem( KPTPertCanvas *view, KPTRelation *rel)
    : QCanvasPolygon(view->canvas()),
	m_rel(rel)
{
    //kdDebug()<<k_funcinfo<<endl;
	// Some "rules":
	//  a) FINISH_START: child column > parent column
	//  b) FINISH_FINISH: child column >= parent column
	//  c) START_START: child column >= parent column
	//  d) Child row can be >= than parent row

	wgap = view->verticalGap();
	hgap = view->horizontalGap();

	// could not use ...rect() here, don't know why
	parentTop = (int)(rel->parent()->pertItem()->y());
	parentBottom = parentTop + (int)(rel->parent()->pertItem()->height());
	childTop = (int)(rel->child()->pertItem()->y());
	
	childRow = rel->child()->pertItem()->row();
	childCol =  rel->child()->pertItem()->column();
	parentRow = rel->parent()->pertItem()->row();
	parentCol =  rel->parent()->pertItem()->column();
    //kdDebug()<<k_funcinfo<<"Parent="<<rel->parent()->name()<<" ("<<parentRow<<","<<parentCol<<") Child="<<rel->child()->name()<<" ("<<childRow<<","<<childCol<<")"<<endl;

	switch (rel->timingRelation())
	{
	    case FINISH_START:
		    setFinishStartPoints();
		    break;
	    case FINISH_FINISH:
		    setFinishFinishPoints();
		    break;
	    case START_START:
		    setStartStartPoints();
		    break;
	}
	QPointArray a = poly;
	left = right = a[0].x();
	top = bottom = a[0].y();
    for (int i = 0; i < a.size(); i++)
	{
	    left = QMIN(a[i].x(), left);
		top = QMIN(a[i].y(), top);
	    right = QMAX(a[i].x(), right);
		bottom = QMAX(a[i].y(), bottom);
	}
	top -= 3;
	bottom += 3;

	setPen(Qt::black);
	setZ(45);

/*#ifndef NDEBUG
	kdDebug()<<" Relation from parent: "<<m_rel->parent()->name()<<" to child: "<<m_rel->child()->name()<<endl;
	QPointArray pa = poly;
    for (int i = 0; i < pa.size(); ++i)
        kdDebug()<<"            pa["<<i<<"]="<<pa[i].x()<<","<<pa[i].y()<<endl;
#endif*/
}

KPTRelationCanvasItem::~KPTRelationCanvasItem()
{
    hide();
}

int KPTRelationCanvasItem::rtti() const { return RTTI; }
int KPTRelationCanvasItem::RTTI = 2020;
	
void KPTRelationCanvasItem::setFinishStartPoints()
{
	QPoint parentPoint = m_rel->parent()->pertItem()->exitPoint(FINISH_START);
	QPoint childPoint = m_rel->child()->pertItem()->entryPoint(FINISH_START);
	
	QPointArray a;
	a.putPoints(0, 1, parentPoint.x(), parentPoint.y());
    
	if ( parentRow == childRow )
	{
	    if (parentCol == childCol - 1 || rowFree(parentRow, parentCol+1, childCol-1))
		{
        	a.putPoints(1, 1, childPoint.x(), childPoint.y());
		}
		else // go around below
		{
    	    a.putPoints(1, 9, 
			        parentPoint.x()+(wgap/2)-3, parentPoint.y(),                // stop short
			        parentPoint.x()+(wgap/2), parentPoint.y()+3,                // right/down
					parentPoint.x()+(wgap/2), parentBottom+(hgap/2)-3,  // stop short
    	            parentPoint.x()+(wgap/2)+3, parentBottom+(hgap/2),  // right/down
    	            childPoint.x()-(wgap/2)-3, parentBottom+(hgap/2),      // stop short
    	            childPoint.x()-(wgap/2), parentBottom+(hgap/2)-3,      // right/up
    	            childPoint.x()-(wgap/2), childPoint.y()+3,                      // stop short
    	            childPoint.x()-(wgap/2)+3, childPoint.y(),                      // right/up
    	            childPoint.x(), childPoint.y());
		}
	} 
	else if ( parentRow > childRow )
	{
	    if (parentCol == childCol - 1)
		{
            a.putPoints(1, 5, 
			        parentPoint.x()+(wgap/2)-3, parentPoint.y(),
			        parentPoint.x()+(wgap/2), parentPoint.y()-3, 
                    parentPoint.x()+wgap/2, childPoint.y()+3,
    	            parentPoint.x()+(wgap/2)+3, childPoint.y(), 
                    childPoint.x(), childPoint.y());
		}
		else // go around above
		{
            a.putPoints(1, 9,
			        parentPoint.x()+(wgap/2)-3, parentPoint.y(),
                    parentPoint.x()+(wgap/2), parentPoint.y()-3,
                    parentPoint.x()+wgap/2, parentTop-(hgap/2)+3,
                    parentPoint.x()+(wgap/2)+3, parentTop-(hgap/2),
                    childPoint.x()-(wgap/2)-3, parentTop-hgap/2,
                    childPoint.x()-(wgap/2), parentTop-(hgap/2)-3,
                    childPoint.x()-wgap/2, childPoint.y()+3,
				    childPoint.x()-(wgap/2)+3, childPoint.y(),
    	            childPoint.x(), childPoint.y());
		}
	}
	else if ( parentRow < childRow )
	{
	    if (parentCol == childCol - 1)
		{
		    a.putPoints(1, 5, 
			        parentPoint.x()+(wgap/2)-3, parentPoint.y(),
			        parentPoint.x()+(wgap/2), parentPoint.y()+3, 
		            parentPoint.x()+wgap/2, childPoint.y()-3,
    	            parentPoint.x()+(wgap/2)+3, childPoint.y(),
    	            childPoint.x(), childPoint.y());
		
	    }
		else
		{
	        a.putPoints(1, 9, 
			        parentPoint.x()+(wgap/2)-3, parentPoint.y(),
			        parentPoint.x()+(wgap/2), parentPoint.y()+3,
					parentPoint.x()+wgap/2, childTop-(hgap/2)-3,
					parentPoint.x()+(wgap/2)+3, childTop-(hgap/2),
	                childPoint.x()-(wgap/2)-3, childTop-(hgap/2),
	                childPoint.x()-(wgap/2), childTop-(hgap/2)+3,
	                childPoint.x()-wgap/2, childPoint.y()-3,
	                childPoint.x()-wgap/2+3, childPoint.y(),
	                childPoint.x(), childPoint.y());
        }
	}
	setPoints(a);
}

void KPTRelationCanvasItem::setFinishFinishPoints()
{
    //kdDebug()<<k_funcinfo<<endl;
	QPoint parentPoint = m_rel->parent()->pertItem()->exitPoint(FINISH_FINISH);
	QPoint childPoint = m_rel->child()->pertItem()->entryPoint(FINISH_FINISH);
	
	QPointArray a;
	a.putPoints(0, 1, parentPoint.x(), parentPoint.y());
	

    if ( parentRow >= childRow )
	{
	    if (parentCol == childCol)
		{
		    a.putPoints(1, 5, 
			        childPoint.x()+(wgap/2)-3, parentPoint.y(),
			        childPoint.x()+(wgap/2), parentPoint.y()-3, 
		            childPoint.x()+wgap/2, childPoint.y()+3,
    	            childPoint.x()+(wgap/2)-3, childPoint.y(),
    	            childPoint.x(), childPoint.y());
		}
		else if (parentCol < childCol)
		{
			a.putPoints(1, 9, 
					parentPoint.x()+(wgap/2)-3, parentPoint.y(),                // stop short
					parentPoint.x()+(wgap/2), parentPoint.y()+3,                // right/down
					parentPoint.x()+(wgap/2), parentBottom+(hgap/2)-3,  // stop short
					parentPoint.x()+(wgap/2)+3, parentBottom+(hgap/2),  // right/down
					childPoint.x()+(wgap/2)-3, parentBottom+(hgap/2),      // stop short
					childPoint.x()+(wgap/2), parentBottom+(hgap/2)-3,      // right/up
					childPoint.x()+(wgap/2), childPoint.y()+3,                      // stop short
					childPoint.x()+(wgap/2)-3, childPoint.y(),                      // left/up
					childPoint.x(), childPoint.y());
		}
	} 
	else // parentRow < choldRow
	{
	    if (parentCol == childCol)
		{
		    a.putPoints(1, 5, 
			        parentPoint.x()+(wgap/2)-3, parentPoint.y(),
			        parentPoint.x()+(wgap/2), parentPoint.y()+3, 
		            parentPoint.x()+wgap/2, childPoint.y()-3,
    	            parentPoint.x()+(wgap/2)-3, childPoint.y(),
    	            childPoint.x(), childPoint.y());
		}
		else if (parentCol < childCol)
		{
		    if (rowFree(parentRow, parentCol+1, childCol))
				a.putPoints(1, 5,
						childPoint.x()+(wgap/2)-3, parentPoint.y(),
						childPoint.x()+(wgap/2), parentPoint.y()+3,
						childPoint.x()+(wgap/2), childPoint.y()-3,
						childPoint.x()+(wgap/2)-3, childPoint.y(),
						childPoint.x(), childPoint.y());			
			else
				a.putPoints(1, 9, 
						parentPoint.x()+(wgap/2)-3, parentPoint.y(),
						parentPoint.x()+(wgap/2), parentPoint.y()+3,
						parentPoint.x()+wgap/2, childTop-(hgap/2)-3,
						parentPoint.x()+(wgap/2)+3, childTop-(hgap/2),
						childPoint.x()+(wgap/2)-3, childTop-(hgap/2),
						childPoint.x()+(wgap/2), childTop-(hgap/2)+3,
						childPoint.x()+(wgap/2), childPoint.y()-3,
						childPoint.x()+(wgap/2)-3, childPoint.y(),
						childPoint.x(), childPoint.y());
        }
		else 
		{
	        a.putPoints(1, 9, 
			        parentPoint.x()+(wgap/2)-3, parentPoint.y(),
			        parentPoint.x()+(wgap/2), parentPoint.y()+3,
					parentPoint.x()+wgap/2, childTop-(hgap/2)-3,
					parentPoint.x()+(wgap/2)+3, childTop-(hgap/2),
	                childPoint.x()+(wgap/2)-3, childTop-(hgap/2),
	                childPoint.x()+(wgap/2), childTop-(hgap/2)+3,
	                childPoint.x()+wgap/2, childPoint.y()-3,
	                childPoint.x()+wgap/2-3, childPoint.y(),
	                childPoint.x(), childPoint.y());
        }
	}
	setPoints(a);
}

void KPTRelationCanvasItem::setStartStartPoints()
{
    //kdDebug()<<k_funcinfo<<endl;
	QPoint parentPoint = m_rel->parent()->pertItem()->exitPoint(START_START);
	QPoint childPoint = m_rel->child()->pertItem()->entryPoint(START_START);
	
	QPointArray a;
	a.putPoints(0, 1, parentPoint.x(), parentPoint.y());
	
    if ( parentRow > childRow )
	{
	    if (parentCol == childCol) // go up
		{
    		a.putPoints(1, 4,
			    parentPoint.x()-(wgap/2)+3, parentPoint.y(),
				parentPoint.x()-(wgap/2), parentPoint.y()-3,
				parentPoint.x()-(wgap/2), childPoint.y()+3,
    		    parentPoint.x()-(wgap/2)+3, childPoint.y());
		}
		else // go above myself
		{
    		a.putPoints(1, 8,
			    parentPoint.x()-(wgap/2)+3, parentPoint.y(),
			    parentPoint.x()-(wgap/2), parentPoint.y()-3,
    		    parentPoint.x()-(wgap/2), parentTop-(hgap/2)+3,
    		    parentPoint.x()-(wgap/2)+3, parentTop-(hgap/2),
    		    childPoint.x()-(wgap/2)-3, parentTop-(hgap/2),
    		    childPoint.x()-(wgap/2), parentTop-(hgap/2)-3,
    		    childPoint.x()-(wgap/2), childPoint.y()+3,
    		    childPoint.x()-(wgap/2)+3, childPoint.y());
		}
	}
	else // go left/down
	{
		a.putPoints(1, 2, 
		    parentPoint.x()-(wgap/2)+3, parentPoint.y(),
		    parentPoint.x()-(wgap/2), parentPoint.y()+3);
						  
	    if (parentCol == childCol)
		{
		    a.putPoints(3, 2, 
			    parentPoint.x()-(wgap/2), childPoint.y()-3,
		        parentPoint.x()-(wgap/2)+3, childPoint.y());
		}
		else // go below myself
		{
            if (parentRow == childRow) // go up
			{
				a.putPoints(3, 6, 
					parentPoint.x()-(wgap/2), parentBottom+hgap/2-3,
					parentPoint.x()-(wgap/2)+3, parentBottom+hgap/2,
					childPoint.x()-(wgap/2)-3, parentBottom+hgap/2,
				    childPoint.x()-(wgap/2), parentBottom+hgap/2-3,
				    childPoint.x()-(wgap/2), childPoint.y()+3,
				    childPoint.x()-(wgap/2)+3, childPoint.y());
			}
			else // go down
			{
				a.putPoints(3, 6,
					parentPoint.x()-(wgap/2), childTop-(hgap/2)-3,
					parentPoint.x()-(wgap/2)+3, childTop-hgap/2,
					childPoint.x()-(wgap/2)-3, childTop-hgap/2,
				    childPoint.x()-(wgap/2), childTop-(hgap/2)+3,
				    childPoint.x()-(wgap/2), childPoint.y()-3,
				    childPoint.x()-(wgap/2)+3, childPoint.y());
			}			
		}
	}
    a.putPoints(a.size(), 1, childPoint.x(), childPoint.y());
	setPoints(a);
}

void KPTRelationCanvasItem::drawShape(QPainter &p)
{
    //kdDebug()<<k_funcinfo<<" "<<m_rel->parent()->name()<<" to "<<m_rel->child()->name()<<endl;
    // cannot use polygon's drawShape() as it doesn't use the pen
	setBrush(Qt::NoBrush);
	QPointArray a = poly;
	int size = a.size()-1;
	for(int i = 0; i < size; ++i)
	{
        //kdDebug()<<k_funcinfo<<" draw["<<i<<"]: "<<a[i].x()<<","<<a[i].y()<<" to "<<a[i+1].x()<<","<<a[i+1].y()<<endl;
	    p.drawLine(a[i], a[i+1]);
	}
	// Draw arrow
    int pos = a.size()-1;
	int xoffset = -3;
	if ( pos > 1&& a[pos-1].x() > a[pos].x())
	    xoffset = 3;
	QPoint pnt(a[pos].x()+xoffset, a[pos].y()-3);
	p.drawLine(a[pos], pnt);
	pnt.setY(a[pos].y()+3);
	p.drawLine(a[pos], pnt);
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

bool KPTRelationCanvasItem::rowFree(int row, int startCol, int endCol)
{
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it) 
    {
		if ( (*it)->rtti() == KPTPertProjectItem::RTTI ||
			(*it)->rtti() == KPTPertTaskItem::RTTI  ||
			(*it)->rtti() == KPTPertMilestoneItem::RTTI )
		{
		    KPTPertNodeItem *item = (KPTPertNodeItem *)(*it);
            if ( item->row() == row )
			{
			    int col = item->column();
                if (col >= startCol && col <= endCol)
				{
        			//kdDebug()<<k_funcinfo<<"Hit on row,col="<<row<<","<<col<<endl;
                    return false;
				}
			}
		}
	}
	return true;
}

#ifndef NDEBUG
void KPTRelationCanvasItem::printDebug( int /*info*/ )
{
}
#endif


////////////////////   KPTGanttViewSummaryItem   //////////////////////////


KPTGanttViewSummaryItem::KPTGanttViewSummaryItem(KDGanttView *parent, KPTNode &node)
    : KDGanttViewSummaryItem(parent, node.name()),
	m_node(node)
{}

KPTGanttViewSummaryItem::KPTGanttViewSummaryItem(KDGanttViewItem *parent, KPTNode &node)
    : KDGanttViewSummaryItem(parent, node.name()),
	m_node(node)
{}

////////////////////   KPTGanttViewTaskItem   //////////////////////////


KPTGanttViewTaskItem::KPTGanttViewTaskItem(KDGanttView *parent, KPTTask *task)
    : KDGanttViewTaskItem(parent, task->name()),
	m_task(task)
{}

KPTGanttViewTaskItem::KPTGanttViewTaskItem(KDGanttViewItem *parent, KPTTask *task)
    : KDGanttViewTaskItem(parent, task->name()),
	m_task(task)
{}

////////////////////   KPTGanttViewEventItem   //////////////////////////


KPTGanttViewEventItem::KPTGanttViewEventItem(KDGanttView *parent, KPTTask *task)
    : KDGanttViewEventItem(parent, task->name()),
	m_task(task)
{}

KPTGanttViewEventItem::KPTGanttViewEventItem(KDGanttViewItem *parent, KPTTask *task)
    : KDGanttViewEventItem(parent, task->name()),
	m_task(task)
{}

