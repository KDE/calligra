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

#include "kpttask.h"
#include "kptcanvasitem.h"
#include "kptrelation.h"
#include "kptpertcanvas.h"
#include "kptganttview.h"

#include <klocale.h>
#include <qpainter.h>
#include <q3pointarray.h>
#include <q3ptrlist.h>
#include <QPoint>

#include <kdebug.h>

namespace KPlato
{

PertNodeItem::PertNodeItem( PertCanvas *view, Node &node, int row, int col )
    : Q3CanvasPolygon(view->canvas()),
	m_node(node),
	m_row(row),
	m_col(col)
{
    m_x = m_y = 0;
    m_wgap = view->verticalGap();
    m_hgap = view->horizontalGap();
    m_width = view->itemSize().width();
    m_height = view->itemSize().height();

    m_name = new Q3CanvasText(node.name(), view->canvas());
    m_childRelations.setAutoDelete(true);
}

PertNodeItem::~PertNodeItem()
{
    Q3CanvasItemList list = canvas()->allItems();
    Q3CanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it)
    {
        if ( *it == m_name )
		    m_name->hide();
        if ( *it == m_leader )
		    m_leader->hide();
    }
   hide();
 }

int PertNodeItem::rtti() const { return RTTI; }
int PertNodeItem::RTTI = 2000;

void PertNodeItem::setVisible(bool yes)
{
    //kDebug()<<k_funcinfo<<m_node.name()<<endl;
	Q3CanvasPolygon::setVisible(yes);
    Q3CanvasItemList list = canvas()->allItems();
    Q3CanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it)
    {
        if ( *it == m_name )
            m_name->setVisible(yes);
        if ( *it == m_leader )
	        m_leader->setVisible(yes);
    }
}

void PertNodeItem::move(PertCanvas *view, int row, int col)
{
    //kDebug()<<k_funcinfo<<endl;
    m_row = row; m_col = col;
    view->mapNode(this);

    // Now map my children
    Q3PtrListIterator<PertNodeRelation> it(m_childRelations);
    for (; it.current(); ++it)
    {
        view->mapChildNode(this, it.current()->childItem, it.current()->relation->type());
    }

    // now move the item on the canvas
    m_x = x(col); m_y = y(row);
    m_left = QPoint(m_x, m_y + m_height/2);
    m_right = QPoint(m_x + m_width, m_y + m_height/2);
    Q3CanvasPolygon::move(m_x, m_y);
    if (m_name)
        m_name->move(m_x+5, m_y+2);

    setVisible(true);
    //kDebug()<<k_funcinfo<<m_node.name()<<" moved to row,col=("<<m_row<<","<<m_col<<")"<<endl;
}

void PertNodeItem::drawShape(QPainter &p)
{
    //QPen pen(pen());
	if (isSelected())
	    p.setPen(QPen(Qt::red, 2));
	Q3PointArray a = poly;
	int size = a.size()-1;
	for(int i = 0; i < size; ++i)
	{
        //kDebug()<<k_funcinfo<<" draw["<<i<<"]: "<<a[i].x()<<","<<a[i].y()<<" to "<<a[i+1].x()<<","<<a[i+1].y()<<endl;
	    p.drawLine(a[i], a[i+1]);
	}
	//setPen(pen);
}

QPoint PertNodeItem::exitPoint(Relation::Type type) const
{
    QPoint ret;
    switch(type)
	{
	    case Relation::FinishStart:
		case Relation::FinishFinish:
		    ret = m_right + QPoint(pen().width(), 0);
			break;
		case Relation::StartStart:
		    ret = m_left + QPoint(0, 4);
			break;
	}
	return ret;
}

QPoint PertNodeItem::entryPoint(Relation::Type type) const
{
    QPoint ret;
    switch(type)
	{
	    case Relation::FinishStart:
		    ret = m_left - QPoint(pen().width(), 0);
			break;
		case Relation::FinishFinish:
		    ret = m_right - QPoint(pen().width(), 4);
			break;
		case Relation::StartStart:
		    ret = m_left  - QPoint(pen().width(), 0);
			break;
	}
	return ret;
}

#ifndef NDEBUG
void PertNodeItem::printDebug( int /*info*/ )
{
}
#endif

////////////////////   PertProjectItem   //////////////////////////

PertProjectItem::PertProjectItem(PertCanvas *view, Node &node, int row, int col)
    : PertNodeItem(view, node, row, col)
{
    //kDebug()<<k_funcinfo<<"Node="<<node.name()<<" ("<<row<<","<<col<<")"<<endl;

	Q3PointArray a;
	a.putPoints(0, 5,
	    m_x+6, m_y, m_x+m_width, m_y, m_x+m_width-6, m_y+m_height, m_x, m_y+m_height, m_x+6, m_y);
	setPoints(a);

	setPen(QPen(Qt::cyan, 2));
}

PertProjectItem::~PertProjectItem()
{
}

int PertProjectItem::rtti() const { return RTTI; }
int PertProjectItem::RTTI = 2001;

#ifndef NDEBUG
void PertProjectItem::printDebug( int /*info*/ )
{
}
#endif


////////////////////   PertTaskItem   //////////////////////////

PertTaskItem::PertTaskItem(PertCanvas *view, Node &node, int row, int col)
    : PertNodeItem(view, node, row, col)
{
    //kDebug()<<k_funcinfo<<"Node="<<node.name()<<" ("<<row<<","<<col<<")"<<endl;
	Q3PointArray a;
	if (node.type() == Node::Type_Summarytask)
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

PertTaskItem::~PertTaskItem()
{
}

int PertTaskItem::rtti() const { return RTTI; }
int PertTaskItem::RTTI = 2002;


#ifndef NDEBUG
void PertTaskItem::printDebug( int /*info*/ )
{
}
#endif


////////////////////   PertMilestoneItem   //////////////////////////

PertMilestoneItem::PertMilestoneItem(PertCanvas *view, Node &node, int row, int col)
    : PertNodeItem(view, node, row, col)
{
    //kDebug()<<k_funcinfo<<"Node="<<node.name()<<" ("<<row<<","<<col<<")"<<endl;

	Q3PointArray a;
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

PertMilestoneItem::~PertMilestoneItem()
{
}

int PertMilestoneItem::rtti() const { return RTTI; }
int PertMilestoneItem::RTTI = 2003;

#ifndef NDEBUG
void PertMilestoneItem::printDebug( int /*info*/ )
{
}
#endif


////////////////////   PertRelationItem   //////////////////////////

PertRelationItem::PertRelationItem( PertCanvas *view, PertNodeItem *parent, PertNodeItem *child, Relation *rel)
    : Q3CanvasPolygon(view->canvas()),
    m_view(view),
    m_rel(rel),
    m_parentItem(parent),
    m_childItem(child)
{
    //kDebug()<<k_funcinfo<<"Parent="<<parent->node().name()<<" Child="<<child->node().name()<<endl;
    draw();
    setVisible(true);
}

PertRelationItem::~PertRelationItem()
{
    hide();
}

int PertRelationItem::rtti() const { return RTTI; }
int PertRelationItem::RTTI = 2020;

void PertRelationItem::draw()
{
    //kDebug()<<k_funcinfo<<endl;
	// Some "rules":
	//  a) Relation::FinishStart: child column > parent column
	//  b) Relation::FinishFinish: child column >= parent column
	//  c) Relation::StartStart: child column >= parent column
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
    //kDebug()<<k_funcinfo<<"Parent="<<m_parentItem->node().name()<<" ("<<parentRow<<","<<parentCol<<") Child="<<m_childItem->node().name()<<" ("<<childRow<<","<<childCol<<")"<<endl;

	switch (type())
	{
	    case Relation::FinishStart:
		    setFinishStartPoints();
		    break;
	    case Relation::FinishFinish:
		    setFinishFinishPoints();
		    break;
	    case Relation::StartStart:
		    setStartStartPoints();
		    break;
	}
	Q3PointArray a = poly;
	left = right = a[0].x();
	top = bottom = a[0].y();
    for (uint i = 0; i < a.size(); i++)
	{
	    left = qMin(a[i].x(), left);
		top = qMin(a[i].y(), top);
	    right = qMax(a[i].x(), right);
		bottom = qMax(a[i].y(), bottom);
	}
	top -= 3;
	bottom += 3;

	setPen(QPen(Qt::black));
	setZ(45);

/*#ifndef NDEBUG
	kDebug()<<" PertNodeRelation from parent: "<<m_rel->parent()->name()<<" to child: "<<m_rel->child()->name()<<endl;
	QPointArray pa = poly;
    for (int i = 0; i < pa.size(); ++i)
        kDebug()<<"            pa["<<i<<"]="<<pa[i].x()<<","<<pa[i].y()<<endl;
#endif*/

}

void PertRelationItem::setFinishStartPoints()
{
	QPoint parentPoint = m_parentItem->exitPoint(Relation::FinishStart);
	QPoint childPoint = m_childItem->entryPoint(Relation::FinishStart);

	Q3PointArray a;
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

void PertRelationItem::setFinishFinishPoints()
{
    //kDebug()<<k_funcinfo<<endl;
	QPoint parentPoint = m_parentItem->exitPoint(Relation::FinishFinish);
	QPoint childPoint = m_childItem->entryPoint(Relation::FinishFinish);

	Q3PointArray a;
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

void PertRelationItem::setStartStartPoints()
{
    //kDebug()<<k_funcinfo<<endl;
	QPoint parentPoint = m_parentItem->exitPoint(Relation::StartStart);
	QPoint childPoint = m_childItem->entryPoint(Relation::StartStart);

	Q3PointArray a;
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

void PertRelationItem::drawShape(QPainter &p)
{
    //kDebug()<<k_funcinfo<<" "<<m_rel->parent()->name()<<" to "<<m_rel->child()->name()<<endl;
    // cannot use polygon's drawShape() as it doesn't use the pen
	setBrush(Qt::NoBrush);
	Q3PointArray a = poly;
	int size = a.size()-1;
	for(int i = 0; i < size; ++i)
	{
        //kDebug()<<k_funcinfo<<" draw["<<i<<"]: "<<a[i].x()<<","<<a[i].y()<<" to "<<a[i+1].x()<<","<<a[i+1].y()<<endl;
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

Q3PointArray PertRelationItem::areaPoints () const
{
    Q3PointArray pa(4);
    int pw = (pen().width()+1)/2;
    if ( pw < 1 ) pw = 1;
    if ( pen() == Qt::NoPen ) pw = 0;
    pa[0] = QPoint(left-pw,top-pw);
    pa[1] = pa[0] + QPoint(right-left+pw*2,0);
    pa[2] = pa[1] + QPoint(0,bottom-top+pw*2);
    pa[3] = pa[0] + QPoint(0,bottom-top+pw*2);
/*    kDebug()<<k_funcinfo<<" areaPoints: "<<m_rel->parent()->name()<<" to "<<m_rel->child()->name()<<endl;
	kDebug()<<"      "<<pa[0].x()<<","<<pa[0].y()<<"      "<<pa[1].x()<<","<<pa[1].y()<<endl;
	kDebug()<<"      "<<pa[2].x()<<","<<pa[2].y()<<"      "<<pa[3].x()<<","<<pa[3].y()<<endl;*/
    return pa;
}

bool PertRelationItem::rowFree(int row, int startCol, int endCol)
{
    Q3CanvasItemList list = canvas()->allItems();
    Q3CanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it)
    {
		if ( (*it)->rtti() == PertProjectItem::RTTI ||
			(*it)->rtti() == PertTaskItem::RTTI  ||
			(*it)->rtti() == PertMilestoneItem::RTTI )
		{
		    PertNodeItem *item = (PertNodeItem *)(*it);
            if ( item->row() == row )
			{
			    int col = item->column();
                if (col >= startCol && col <= endCol)
				{
        			//kDebug()<<k_funcinfo<<"Hit on row,col="<<row<<","<<col<<endl;
                    return false;
				}
			}
		}
	}
	return true;
}

#ifndef NDEBUG
void PertRelationItem::printDebug( int /*info*/ )
{
}
#endif

////////////////////   ItemBase   //////////////////////////
KDGanttViewTaskLink::LinkType ItemBase::kdLinkType(int relationType) {
    switch (relationType) {
        case Relation::FinishStart:
            return KDGanttViewTaskLink::FinishStart;
            break;
        case Relation::FinishFinish:
            return KDGanttViewTaskLink::FinishFinish;
            break;
        case Relation::StartStart:
            return KDGanttViewTaskLink::StartStart;
            break;
        default:
            break;
    }
    return KDGanttViewTaskLink::None;
}

////////////////////   GanttViewSummaryItem   //////////////////////////


GanttViewSummaryItem::GanttViewSummaryItem(KDGanttView *parent, Node *node)
    : KDGanttViewSummaryItem(parent, node->name()),
      m_node(node),
      m_view(parent)
{
    setExpandable(true);
    setOpen(true);
}

GanttViewSummaryItem::GanttViewSummaryItem(KDGanttViewItem *parent, Node *node)
    : KDGanttViewSummaryItem(parent, node->name()),
      m_node(node),
      m_view(0)
{
    m_drawn = false;
    GanttViewSummaryItem *p = dynamic_cast<GanttViewSummaryItem*>(parent);
    if (p)
        m_view = p->ganttView();
    setExpandable(true);
    setOpen(true);
}

void GanttViewSummaryItem::insertRelations(GanttView *view)
{
    //kDebug()<<k_funcinfo<<endl;

    Q3PtrListIterator<Relation> it(m_node->dependChildNodes());
    for (; it.current(); ++it)
    {
        KDGanttViewItem *child = find(m_view->firstChild(), it.current()->child());
        if (child)
        {
            KDGanttViewTaskLink *link = new KDGanttViewTaskLink(this, child, kdLinkType(it.current()->type()));
            //TODO i18n
            QString t = i18n("From: %1", this->listViewText(0));
            t += "\n" + i18n("To: %1", child->listViewText(0));
            if (it.current()->lag() > Duration::zeroDuration) {
                t += "\n" + i18n("Lag:  %1", it.current()->lag().toString(Duration::Format_i18nDayTime));
            }
            link->setTooltipText(t);
            view->addTaskLink(link);
        }
    }
}

KDGanttViewItem *GanttViewSummaryItem::find(Node *node)
{
    //kDebug()<<k_funcinfo<<endl;
    if (m_node == node)
        return this;

    KDGanttViewItem *item = find(firstChild(), node);
    if (item)
        return item;

    return find(nextSibling(), node);
}


KDGanttViewItem *GanttViewSummaryItem::find(KDGanttViewItem *item, Node *node)
{
    if (!item)
        return 0;

    if (item->type() == Event)
    {
        GanttViewEventItem *i = static_cast<GanttViewEventItem *>(item);
        return i->find(node);
    }
    else if (item->type() == Task)
    {
        GanttViewTaskItem *i = static_cast<GanttViewTaskItem *>(item);
        return i->find(node);
    }
    else if (item->type() == Summary)
    {
        GanttViewSummaryItem *i = static_cast<GanttViewSummaryItem *>(item);
        return i->find(node);
    }
    return 0;
}

////////////////////   GanttViewTaskItem   //////////////////////////


GanttViewTaskItem::GanttViewTaskItem(KDGanttView *parent, KPlato::Task *task)
    : KDGanttViewTaskItem(parent, task->name()),
      m_task(task),
      m_view(parent)
{
}

GanttViewTaskItem::GanttViewTaskItem(KDGanttViewItem *parent, KPlato::Task *task)
    : KDGanttViewTaskItem(parent, task->name()),
      m_task(task),
      m_view()
{
    m_drawn = false;
    GanttViewSummaryItem *p = dynamic_cast<GanttViewSummaryItem*>(parent);
    if (p)
        m_view = p->ganttView();
}

void GanttViewTaskItem::insertRelations(GanttView *view)
{
    //kDebug()<<k_funcinfo<<endl;

    Q3PtrListIterator<Relation> it(m_task->dependChildNodes());
    for (; it.current(); ++it)
    {
        KDGanttViewItem *child = find(m_view->firstChild(), it.current()->child());
        if (child)
        {
            KDGanttViewTaskLink *link = new KDGanttViewTaskLink(this, child,  kdLinkType(it.current()->type()));
            //TODO i18n
            QString t = i18n("From: %1", this->listViewText(0));
            t += "\n" + i18n("To: %1", child->listViewText(0));
            if (it.current()->lag() > Duration::zeroDuration) {
                t += "\n" + i18n("Lag:  %1", it.current()->lag().toString(Duration::Format_i18nDayTime));
            }
            link->setTooltipText(t);
            view->addTaskLink(link);
        }
    }
}

KDGanttViewItem *GanttViewTaskItem::find(Node *node)
{
    //kDebug()<<k_funcinfo<<endl;
    if (m_task == node)
        return this;

    KDGanttViewItem *item = find(firstChild(), node);
    if (item)
        return item;

    return find(nextSibling(), node);
}


KDGanttViewItem *GanttViewTaskItem::find(KDGanttViewItem *item, Node *node)
{
    if (!item)
        return 0;

    if (item->type() == Event)
    {
        GanttViewEventItem *i = static_cast<GanttViewEventItem *>(item);
        return i->find(node);
    }
    else if (item->type() == Task)
    {
        GanttViewTaskItem *i= static_cast<GanttViewTaskItem *>(item);
        return i->find(node);
    }
    else if (item->type() == Summary)
    {
        GanttViewSummaryItem *i = static_cast<GanttViewSummaryItem *>(item);
        return i->find(node);
    }
    return 0; // avoid warning
}

////////////////////   GanttViewEventItem   //////////////////////////


GanttViewEventItem::GanttViewEventItem(KDGanttView *parent, KPlato::Task *task)
    : KDGanttViewEventItem(parent, task->name()),
      m_task(task),
      m_view(parent)
{
}

GanttViewEventItem::GanttViewEventItem(KDGanttViewItem *parent, KPlato::Task *task)
    : KDGanttViewEventItem(parent, task->name()),
      m_task(task),
      m_view()
{
    m_drawn = false;
    GanttViewSummaryItem *p = dynamic_cast<GanttViewSummaryItem*>(parent);
    if (p)
        m_view = p->ganttView();
}


void GanttViewEventItem::insertRelations(GanttView *view)
{
    //kDebug()<<k_funcinfo<<endl;

    Q3PtrListIterator<Relation> it(m_task->dependChildNodes());
    for (; it.current(); ++it)
    {
        KDGanttViewItem *child = find(m_view->firstChild(), it.current()->child());
        if (child)
        {
            KDGanttViewTaskLink *link = new KDGanttViewTaskLink(this, child, kdLinkType(it.current()->type()));

            QString t = i18n("From: %1",this->listViewText(0));
            t += "\n" + i18n("To: %1",child->listViewText(0));
            if (it.current()->lag() > Duration::zeroDuration) {
                t += "\n" + i18n("Lag:  %1",it.current()->lag().toString(Duration::Format_i18nDayTime));
            }
            link->setTooltipText(t);
            view->addTaskLink(link);
        }
    }
}

KDGanttViewItem *GanttViewEventItem::find(Node *node)
{
    //kDebug()<<k_funcinfo<<endl;
    if (m_task == node)
        return this;

    KDGanttViewItem *item = find(firstChild(), node);
    if (item)
        return item;

    return find(nextSibling(), node);
}


KDGanttViewItem *GanttViewEventItem::find(KDGanttViewItem *item, Node *node)
{
    if (!item)
        return 0;

    if (item->type() == Event)
    {
        GanttViewEventItem *i = static_cast<GanttViewEventItem *>(item);
        return i->find(node);
    }
    else if (item->type() == Task)
    {
        GanttViewTaskItem *i = static_cast<GanttViewTaskItem *>(item);
        return i->find(node);
    }
    else if (item->type() == Summary)
    {
        GanttViewSummaryItem *i = static_cast<GanttViewSummaryItem *>(item);
        return i->find(node);
    }
    return 0;
}

}  //KPlato namespace
