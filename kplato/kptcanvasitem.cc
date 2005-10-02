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

#include "kptcanvasitem.h"
#include "kptrelation.h"
#include "kptpertcanvas.h"
#include "kpttask.h"
#include "kptganttview.h"

#include <qpainter.h>
#include <qpointarray.h>
#include <qptrlist.h>
#include <qpoint.h>

#include <kdebug.h>
#include <koRect.h> //DEBUGRECT

namespace KPlato
{

KPTPertNodeItem::KPTPertNodeItem( KPTPertCanvas *view, KPTNode &node, int row, int col )
    : QCanvasPolygon(view->canvas()),
	m_node(node),
	m_row(row),
	m_col(col)
{
    m_x = m_y = 0;
    m_wgap = view->verticalGap();
    m_hgap = view->horizontalGap();
    m_width = view->itemSize().width();
    m_height = view->itemSize().height();

    m_name = new QCanvasText(node.name(), view->canvas());
    m_childRelations.setAutoDelete(true);
}

KPTPertNodeItem::~KPTPertNodeItem()
{
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it)
    {
        if ( *it == m_name )
		    m_name->hide();
        if ( *it == m_leader )
		    m_leader->hide();
    }
   hide();
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

void KPTPertNodeItem::move(KPTPertCanvas *view, int row, int col)
{
    //kdDebug()<<k_funcinfo<<endl;
    m_row = row; m_col = col;
    view->mapNode(this);

    // Now map my children
    QPtrListIterator<Relation> it(m_childRelations);
    for (; it.current(); ++it)
    {
        view->mapChildNode(this, it.current()->childItem, it.current()->relation->type());
    }

    // now move the item on the canvas
    m_x = x(col); m_y = y(row);
    m_left = QPoint(m_x, m_y + m_height/2);
    m_right = QPoint(m_x + m_width, m_y + m_height/2);
    QCanvasPolygon::move(m_x, m_y);
    if (m_name)
        m_name->move(m_x+5, m_y+2);

    setVisible(true);
    //kdDebug()<<k_funcinfo<<m_node.name()<<" moved to row,col=("<<m_row<<","<<m_col<<")"<<endl;
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

QPoint KPTPertNodeItem::exitPoint(KPTRelation::Type type) const
{
    QPoint ret;
    switch(type)
	{
	    case KPTRelation::FinishStart:
		case KPTRelation::FinishFinish:
		    ret = m_right + QPoint(pen().width(), 0);
			break;
		case KPTRelation::StartStart:
		    ret = m_left + QPoint(0, 4);
			break;
	}
	return ret;
}

QPoint KPTPertNodeItem::entryPoint(KPTRelation::Type type) const
{
    QPoint ret;
    switch(type)
	{
	    case KPTRelation::FinishStart:
		    ret = m_left - QPoint(pen().width(), 0);
			break;
		case KPTRelation::FinishFinish:
		    ret = m_right - QPoint(pen().width(), 4);
			break;
		case KPTRelation::StartStart:
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

KPTPertProjectItem::KPTPertProjectItem(KPTPertCanvas *view, KPTNode &node, int row, int col)
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

KPTPertTaskItem::KPTPertTaskItem(KPTPertCanvas *view, KPTNode &node, int row, int col)
    : KPTPertNodeItem(view, node, row, col)
{
    //kdDebug()<<k_funcinfo<<"Node="<<node.name()<<" ("<<row<<","<<col<<")"<<endl;
	QPointArray a;
	if (node.type() == KPTNode::Type_Summarytask)
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

KPTPertMilestoneItem::KPTPertMilestoneItem(KPTPertCanvas *view, KPTNode &node, int row, int col)
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


////////////////////   KPTPertRelationItem   //////////////////////////

KPTPertRelationItem::KPTPertRelationItem( KPTPertCanvas *view, KPTPertNodeItem *parent, KPTPertNodeItem *child, KPTRelation *rel)
    : QCanvasPolygon(view->canvas()),
    m_view(view),
    m_rel(rel),
    m_parentItem(parent),
    m_childItem(child)
{
    //kdDebug()<<k_funcinfo<<"Parent="<<parent->node().name()<<" Child="<<child->node().name()<<endl;
    draw();
    setVisible(true);
}

KPTPertRelationItem::~KPTPertRelationItem()
{
    hide();
}

int KPTPertRelationItem::rtti() const { return RTTI; }
int KPTPertRelationItem::RTTI = 2020;

void KPTPertRelationItem::draw()
{
    //kdDebug()<<k_funcinfo<<endl;
	// Some "rules":
	//  a) KPTRelation::FinishStart: child column > parent column
	//  b) KPTRelation::FinishFinish: child column >= parent column
	//  c) KPTRelation::StartStart: child column >= parent column
	//  d) Child row can be >= parent row

	wgap = m_view->verticalGap();
	hgap = m_view->horizontalGap();

	// could not use ...rect() here, don't know why
	parentTop = (int)(m_parentItem->y());
	parentBottom = parentTop + (int)(m_parentItem->height());
	childTop = (int)(m_childItem->y());

	childRow = m_childItem->row();
	childCol =  m_childItem->column();
	parentRow = m_parentItem->row();
	parentCol =  m_parentItem->column();
    //kdDebug()<<k_funcinfo<<"Parent="<<m_parentItem->node().name()<<" ("<<parentRow<<","<<parentCol<<") Child="<<m_childItem->node().name()<<" ("<<childRow<<","<<childCol<<")"<<endl;

	switch (type())
	{
	    case KPTRelation::FinishStart:
		    setFinishStartPoints();
		    break;
	    case KPTRelation::FinishFinish:
		    setFinishFinishPoints();
		    break;
	    case KPTRelation::StartStart:
		    setStartStartPoints();
		    break;
	}
	QPointArray a = poly;
	left = right = a[0].x();
	top = bottom = a[0].y();
    for (uint i = 0; i < a.size(); i++)
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

void KPTPertRelationItem::setFinishStartPoints()
{
	QPoint parentPoint = m_parentItem->exitPoint(KPTRelation::FinishStart);
	QPoint childPoint = m_childItem->entryPoint(KPTRelation::FinishStart);

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

void KPTPertRelationItem::setFinishFinishPoints()
{
    //kdDebug()<<k_funcinfo<<endl;
	QPoint parentPoint = m_parentItem->exitPoint(KPTRelation::FinishFinish);
	QPoint childPoint = m_childItem->entryPoint(KPTRelation::FinishFinish);

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

void KPTPertRelationItem::setStartStartPoints()
{
    //kdDebug()<<k_funcinfo<<endl;
	QPoint parentPoint = m_parentItem->exitPoint(KPTRelation::StartStart);
	QPoint childPoint = m_childItem->entryPoint(KPTRelation::StartStart);

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

void KPTPertRelationItem::drawShape(QPainter &p)
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

QPointArray KPTPertRelationItem::areaPoints () const
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

bool KPTPertRelationItem::rowFree(int row, int startCol, int endCol)
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
void KPTPertRelationItem::printDebug( int /*info*/ )
{
}
#endif

////////////////////   KPTItemBase   //////////////////////////
KDGanttViewTaskLink::LinkType KPTItemBase::kdLinkType(int relationType) {
    switch (relationType) {
        case KPTRelation::FinishStart:
            return KDGanttViewTaskLink::FinishStart;
            break;
        case KPTRelation::FinishFinish:
            return KDGanttViewTaskLink::FinishFinish;
            break;
        case KPTRelation::StartStart:
            return KDGanttViewTaskLink::StartStart;
            break;
        default:
            break;
    }
    return KDGanttViewTaskLink::None;
}

////////////////////   KPTGanttViewSummaryItem   //////////////////////////


KPTGanttViewSummaryItem::KPTGanttViewSummaryItem(KDGanttView *parent, KPTNode *node)
    : KDGanttViewSummaryItem(parent, node->name()),
      m_node(node),
      m_view(parent)
{
    setExpandable(true);
    setOpen(true);
}

KPTGanttViewSummaryItem::KPTGanttViewSummaryItem(KDGanttViewItem *parent, KPTNode *node)
    : KDGanttViewSummaryItem(parent, node->name()),
      m_node(node),
      m_view(0)
{
    m_drawn = false;
    KPTGanttViewSummaryItem *p = dynamic_cast<KPTGanttViewSummaryItem*>(parent);
    if (p)
        m_view = p->ganttView();
    setExpandable(true);
    setOpen(true);
}

void KPTGanttViewSummaryItem::insertRelations(KPTGanttView *view)
{
    //kdDebug()<<k_funcinfo<<endl;

    QPtrListIterator<KPTRelation> it(m_node->dependChildNodes());
    for (; it.current(); ++it)
    {
        KDGanttViewItem *child = find(m_view->firstChild(), it.current()->child());
        if (child)
        {
            KDGanttViewTaskLink *link = new KDGanttViewTaskLink(this, child, kdLinkType(it.current()->type()));
            //TODO i18n
            QString t = QString("From: %1").arg(this->listViewText(0));
            t += QString("\nTo: %1").arg(child->listViewText(0));
            t += QString("\nLag: %1").arg(it.current()->lag().toString(KPTDuration::Format_Hour));
            link->setTooltipText(t);
            view->addTaskLink(link);
        }
    }
}

KDGanttViewItem *KPTGanttViewSummaryItem::find(KPTNode *node)
{
    //kdDebug()<<k_funcinfo<<endl;
    if (m_node == node)
        return this;

    KDGanttViewItem *item = find(firstChild(), node);
    if (item)
        return item;

    return find(nextSibling(), node);
}


KDGanttViewItem *KPTGanttViewSummaryItem::find(KDGanttViewItem *item, KPTNode *node)
{
    if (!item)
        return 0;

    if (item->type() == Event)
    {
        KPTGanttViewEventItem *i = static_cast<KPTGanttViewEventItem *>(item);
        return i->find(node);
    }
    else if (item->type() == Task)
    {
        KPTGanttViewTaskItem *i = static_cast<KPTGanttViewTaskItem *>(item);
        return i->find(node);
    }
    else if (item->type() == Summary)
    {
        KPTGanttViewSummaryItem *i = static_cast<KPTGanttViewSummaryItem *>(item);
        return i->find(node);
    }
    return 0;
}

////////////////////   KPTGanttViewTaskItem   //////////////////////////


KPTGanttViewTaskItem::KPTGanttViewTaskItem(KDGanttView *parent, KPTTask *task)
    : KDGanttViewTaskItem(parent, task->name()),
      m_task(task),
      m_view(parent)
{
}

KPTGanttViewTaskItem::KPTGanttViewTaskItem(KDGanttViewItem *parent, KPTTask *task)
    : KDGanttViewTaskItem(parent, task->name()),
      m_task(task),
      m_view()
{
    m_drawn = false;
    KPTGanttViewSummaryItem *p = dynamic_cast<KPTGanttViewSummaryItem*>(parent);
    if (p)
        m_view = p->ganttView();
}

void KPTGanttViewTaskItem::insertRelations(KPTGanttView *view)
{
    //kdDebug()<<k_funcinfo<<endl;

    QPtrListIterator<KPTRelation> it(m_task->dependChildNodes());
    for (; it.current(); ++it)
    {
        KDGanttViewItem *child = find(m_view->firstChild(), it.current()->child());
        if (child)
        {
            KDGanttViewTaskLink *link = new KDGanttViewTaskLink(this, child, kdLinkType(it.current()->type()));
            //TODO i18n
            QString t = QString("From: %1").arg(this->listViewText(0));
            t += QString("\nTo: %1").arg(child->listViewText(0));
            t += QString("\nLag: %1").arg(it.current()->lag().toString(KPTDuration::Format_Hour));
            link->setTooltipText(t);
            view->addTaskLink(link);
        }
    }
}

KDGanttViewItem *KPTGanttViewTaskItem::find(KPTNode *node)
{
    //kdDebug()<<k_funcinfo<<endl;
    if (m_task == node)
        return this;

    KDGanttViewItem *item = find(firstChild(), node);
    if (item)
        return item;

    return find(nextSibling(), node);
}


KDGanttViewItem *KPTGanttViewTaskItem::find(KDGanttViewItem *item, KPTNode *node)
{
    if (!item)
        return 0;

    if (item->type() == Event)
    {
        KPTGanttViewEventItem *i = static_cast<KPTGanttViewEventItem *>(item);
        return i->find(node);
    }
    else if (item->type() == Task)
    {
        KPTGanttViewTaskItem *i= static_cast<KPTGanttViewTaskItem *>(item);
        return i->find(node);
    }
    else if (item->type() == Summary)
    {
        KPTGanttViewSummaryItem *i = static_cast<KPTGanttViewSummaryItem *>(item);
        return i->find(node);
    }
    return 0; // avoid warning
}

////////////////////   KPTGanttViewEventItem   //////////////////////////


KPTGanttViewEventItem::KPTGanttViewEventItem(KDGanttView *parent, KPTTask *task)
    : KDGanttViewEventItem(parent, task->name()),
      m_task(task),
      m_view(parent)
{
}

KPTGanttViewEventItem::KPTGanttViewEventItem(KDGanttViewItem *parent, KPTTask *task)
    : KDGanttViewEventItem(parent, task->name()),
      m_task(task),
      m_view()
{
    m_drawn = false;
    KPTGanttViewSummaryItem *p = dynamic_cast<KPTGanttViewSummaryItem*>(parent);
    if (p)
        m_view = p->ganttView();
}


void KPTGanttViewEventItem::insertRelations(KPTGanttView *view)
{
    //kdDebug()<<k_funcinfo<<endl;

    QPtrListIterator<KPTRelation> it(m_task->dependChildNodes());
    for (; it.current(); ++it)
    {
        KDGanttViewItem *child = find(m_view->firstChild(), it.current()->child());
        if (child)
        {
            KDGanttViewTaskLink *link = new KDGanttViewTaskLink(this, child, kdLinkType(it.current()->type()));
            //TODO i18n
            QString t = QString("From: %1").arg(this->listViewText(0));
            t += QString("\nTo: %1").arg(child->listViewText(0));
            t += QString("\nLag: %1").arg(it.current()->lag().toString(KPTDuration::Format_Hour));
            link->setTooltipText(t);
            view->addTaskLink(link);
        }
    }
}

KDGanttViewItem *KPTGanttViewEventItem::find(KPTNode *node)
{
    //kdDebug()<<k_funcinfo<<endl;
    if (m_task == node)
        return this;

    KDGanttViewItem *item = find(firstChild(), node);
    if (item)
        return item;

    return find(nextSibling(), node);
}


KDGanttViewItem *KPTGanttViewEventItem::find(KDGanttViewItem *item, KPTNode *node)
{
    if (!item)
        return 0;

    if (item->type() == Event)
    {
        KPTGanttViewEventItem *i = static_cast<KPTGanttViewEventItem *>(item);
        return i->find(node);
    }
    else if (item->type() == Task)
    {
        KPTGanttViewTaskItem *i = static_cast<KPTGanttViewTaskItem *>(item);
        return i->find(node);
    }
    else if (item->type() == Summary)
    {
        KPTGanttViewSummaryItem *i = static_cast<KPTGanttViewSummaryItem *>(item);
        return i->find(node);
    }
    return 0;
}

}  //KPlato namespace
