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

#include "kptpertcanvas.h"
#include "kptnode.h"
#include "kptrelation.h"
#include "kptrelationdialog.h"
#include "kptcanvasitem.h"

#include <qbuffer.h>
#include <qtimer.h>
#include <qclipboard.h>
#include <qprogressdialog.h>
#include <qobjectlist.h>
#include <qpainter.h>
#include <qheader.h>
#include <qcursor.h>
#include <qrect.h>
#include <qsize.h>
#include <qptrlist.h>

#include <koStore.h>
#include <koStoreDrag.h>
#include <ktempfile.h>
#include <klocale.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <assert.h>
#include <kmultipledrag.h>
#include <klistview.h>

#include <koRect.h> //DEBUGRECT

KPTPertCanvas::KPTPertCanvas( QWidget *parent )
    : QCanvasView( parent, "Pert canvas" /*WNorthWestGravity WStaticContents| WResizeNoErase | WRepaintNoErase */),
	m_verticalGap(20),
	m_horizontalGap(10),
	m_itemSize(100,30)

{
    //setHScrollBarMode(QScrollView::AlwaysOn);
    m_canvas = new QCanvas( this );
    setCanvas( m_canvas );
}

KPTPertCanvas::~KPTPertCanvas()
{
}

void KPTPertCanvas::draw(KPTProject& project)
{
    //kdDebug()<<k_funcinfo<<endl;
    clear();
    updateContents();

    if (!project.isDeleted()) {
    
        // First make node items
        QPtrListIterator<KPTNode> nit(project.childNodeIterator());
        for ( ; nit.current(); ++nit ) {
            if (!nit.current()->isDeleted()) {
                createChildItems(createNodeItem(nit.current()));
            }
        }
    
        // First all items with relations
        QPtrDictIterator<KPTPertNodeItem> it(m_nodes);
        for(; it.current(); ++it)
        {
            if (!(it.current()->hasParent()) && it.current()->hasChild())
            {
                m_rows.append(new QMemArray<bool>(1)); // New node always goes into new row, first column
                it.current()->move(this, m_rows.count()-1, 0); // item also moves it's children
            }
        }
        // now items without relations
        for(it.toFirst(); it.current(); ++it)
        {
            if (!(it.current()->hasParent() || it.current()->hasChild()))
            {
                m_rows.append(new QMemArray<bool>(1)); // New node always goes into new row, first column
                it.current()->move(this, m_rows.count()-1, 0);
            }
        }
        drawRelations(); // done _after_ all nodes are drawn
    }
    QSize s = canvasSize();
    m_canvas->resize(s.width(), s.height());
    update();
}

KPTPertNodeItem *KPTPertCanvas::createNodeItem(KPTNode *node)
{
    if (node->isDeleted())
        return 0;
    KPTPertNodeItem *item = m_nodes.find(node);
    if (!item)
    {
        if ( node->type() == KPTNode::Type_Project)
            kdDebug()<<k_funcinfo<<"Project nodes should not have relations"<<endl;
        else if (node->type() == KPTNode::Type_Subproject)
            item  = new KPTPertProjectItem(this, *node);
        else if (node->type()== KPTNode::Type_Summarytask)
            item  = new KPTPertTaskItem(this, *node);
        else if (node->type()== KPTNode::Type_Task)
            item  = new KPTPertTaskItem(this, *node);
        else if (node->type() == KPTNode::Type_Milestone)
            item  = new KPTPertMilestoneItem(this, *node);
        else
            kdDebug()<<k_funcinfo<<"Not implemented yet"<<endl;

        if (item)
            m_nodes.insert(node, item);
    }
    return item;
}

void KPTPertCanvas::createChildItems(KPTPertNodeItem *parentItem)
{
    //kdDebug()<<k_funcinfo<<"parentItem="<<(parentItem ? parentItem->node().name() : "nil")<<endl;
    if (!parentItem)
        return;

    QPtrListIterator<KPTRelation> it(parentItem->node().dependChildNodes());
    for (; it.current(); ++it)
    {
        KPTPertNodeItem *childItem = createNodeItem(it.current()->child());
        if (childItem)
            parentItem->addChildRelation(it.current(), childItem);
            m_relations.append(it.current());
    }

    // Now my children
	QPtrListIterator<KPTNode> nit(parentItem->node().childNodeIterator());
    for ( ; nit.current(); ++nit ) {
        createChildItems(createNodeItem(nit.current()));
	}
}

void KPTPertCanvas::drawRelations()
{
	//kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<KPTRelation> it(m_relations);
    for (; it.current(); ++it)
    {
        KPTPertNodeItem *parentItem = m_nodes.find(it.current()->parent());
        KPTPertNodeItem *childItem = m_nodes.find(it.current()->child());
        if (parentItem && childItem)
        {
            KPTPertRelationItem *item = new KPTPertRelationItem(this, parentItem, childItem, it.current());
            item->show();
        }
    }
}

void KPTPertCanvas::mapNode(KPTPertNodeItem *item)
{
	//kdDebug()<<k_funcinfo<<endl;
    if (! m_rows.at(item->row()) || m_rows.at(item->row())->count() <= item->column())
    {
        kdError()<<k_funcinfo<<item->node().name()<<": non existing map for: ("<<item->row()<<","<<item->column()<<")"<<endl;
        return;
    }
    m_rows.at(item->row())->at(item->column()) = true;
}

void KPTPertCanvas::mapChildNode(KPTPertNodeItem *parentItem, KPTPertNodeItem *childItem, TimingRelation type)
{
	//kdDebug()<<k_funcinfo<<"Parent: "<<parentItem->node().name()<<" to child: "<<(childItem ? childItem->node().name() : "None")<<endl;
    if (!childItem)
    {   // shouldn't happen...
        kdError()<<k_funcinfo<<"No childItem"<<endl;
        return;
    }
    int row = parentItem->row();
    int col = parentItem->column();
    int chRow = childItem->row();
    int chCol = childItem->column();
    bool chMapped = (chRow > -1 && chCol > -1);
	//kdDebug()<<k_funcinfo<<"Parent: "<<parentItem->node().name()<<" at ("<<row<<","<<col<<"): Moving "<<childItem->node().name()<<" from: "<<chRow<<","<<chCol<<endl;

    if (type == START_START ||
        type == FINISH_FINISH)
    {
        // node goes into row below parent, at least same col
        if (chMapped)
        {
            m_rows.at(chRow)->at(chCol) = false;
            if (chRow <= row)
            {
                chRow = row+1;
                if (!(m_rows.count() <= chRow))
                    m_rows.append(new QMemArray<bool>(1)); // make a new row
            }
            if (chCol < col)
            {
                chCol = col;
                if (m_rows.at(chRow)->count() <= chCol)  // col does not exist
                    m_rows.at(chRow)->resize(chCol+1);
            }

        }
        else
        {
            if (!(m_rows.at(row+1)) ||                        // next row does not exists
                m_rows.at(row+1)->at(col) == true)  // col is not free
            {
                m_rows.append(new QMemArray<bool>(col+1)); // make a new row
            }
            else if (m_rows.at(row+1)->count() <= col)  // col does not exist
                m_rows.at(row)->resize(col+1);

            chRow = m_rows.count() -1;
            chCol = col;
        }
    }
    else if (type == FINISH_START)
    {
        // node goes into same row, next col if col free
        if (chMapped)
        {
            m_rows.at(chRow)->at(chCol) = false;
            if (chRow < row)
                chRow = row;
            if (chCol <= col)
            {
                chCol = col+1;
            }
            if (m_rows.at(chRow)->count() <= chCol)  // col does not exist
                m_rows.at(chRow)->resize(chCol+1);
        }
        else
        {
            ++col;
            if (m_rows.at(row)->count() <= col)
                m_rows.at(row)->resize(col+1); // make new column
            else if (m_rows.at(row)->at(col) = true)
                m_rows.append(new QMemArray<bool>(col+1)); // col not free, so make a new row

            chRow = m_rows.count() -1;
            chCol = col;
        }
    }
    else
    {
        kdError()<<k_funcinfo<<"Unknow TimingRelation"<<endl;
        return;
    }
    childItem->move(this, chRow, chCol);
}

QSize KPTPertCanvas::canvasSize()
{
    //kdDebug()<<k_funcinfo<<endl;
	QSize s(0,0);
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it)
    {
	    QRect r = (*it)->boundingRect();
		s.setWidth(QMAX(s.width(), r.right()));
		s.setHeight(QMAX(s.height(), r.bottom()));
	}
	s.setWidth(s.width()+20);
	s.setHeight(s.height()+20);
	return s;
}

void KPTPertCanvas::clear()
{
    m_nodes.clear();
    m_relations.clear();
    m_rows.clear();
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it)
    {
        if ( *it )
            delete *it;
    }
}

void KPTPertCanvas::contentsMouseReleaseEvent ( QMouseEvent * e )
{
    //kdDebug()<<k_funcinfo<<" gl.X,gl.Y="<<e->globalX()<<","<<e->globalY()<<" x,y="<<e->x()<<","<<e->y()<<endl;
    switch (e->button())
    {
        case QEvent::LeftButton:
        {
            QCanvasItemList l = canvas()->collisions(e->pos());
            for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
		    {
	            if ( (*it)->rtti() == KPTPertProjectItem::RTTI ||
	                 (*it)->rtti() == KPTPertTaskItem::RTTI  ||
	                 (*it)->rtti() == KPTPertMilestoneItem::RTTI )
				{
				    KPTPertNodeItem *item = (KPTPertNodeItem *)(*it);
					KPTPertNodeItem *par = selectedItem();
					if ( !par)
					{
						//kdDebug()<<k_funcinfo<<" First node="<<item->node().name()<<endl;
						item->setSelected(true);
						canvas()->update();
						return;
					}
					par->setSelected(false);
					if (&(item->node()) == &(par->node()))
					{
						break;
					}
					//kdDebug()<<k_funcinfo<<" Second node="<<item->node().name()<<endl;
					// open relation dialog
					if (legalToLink(par->node(), item->node()))
					{
						KPTRelationDialog *dia;
						KPTRelation *rel = item->node().findRelation(&(par->node()));
						if (rel)
							dia = new KPTRelationDialog(rel, this);
						else
							dia = new KPTRelationDialog(&(par->node()), &(item->node()), this);

						if (dia->exec())
						{
							//kdDebug()<<k_funcinfo<<" Linked node="<<item->node().name()<<" to "<<par->node().name()<<endl;
							emit updateView(true);
						}
						delete dia;
					}
					break;
				}
			}
            canvas()->update();
            break;
        }
        case QEvent::RightButton:
        {
            QCanvasItemList l = canvas()->collisions(e->pos());
            for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
		    {
	            if ( (*it)->rtti() == KPTPertProjectItem::RTTI ||
	                 (*it)->rtti() == KPTPertTaskItem::RTTI  ||
	                 (*it)->rtti() == KPTPertMilestoneItem::RTTI )
				{
				    KPTPertNodeItem *item = (KPTPertNodeItem *)(*it);
					{
					    emit rightButtonPressed(&(item->node()), e->globalPos());
						break;
					}
				}
			}
            break;
        }
        case QEvent::MidButton:
            break;
        default:
            break;
    }
}

bool KPTPertCanvas::legalToLink(KPTNode &par, KPTNode &child)
{
    //kdDebug()<<k_funcinfo<<par.name()<<" ("<<par.numDependParentNodes()<<" parents) "<<child.name()<<" ("<<child.numDependChildNodes()<<" children)"<<endl;
	if (par.isDependChildOf(&(child)))
	{
		KMessageBox::sorry(this, i18n("Cannot link already dependent nodes"));
		return false;
	}
    bool legal = true;
	// see if par/child is related
    if (par.isParentOf(&child) || child.isParentOf(&par))
	{
	    legal = false;
	}
	if (legal)
	    legal = legalChildren(&par, &child);
	if (legal)
		legal = legalParents(&par, &child);

	if (!legal)
	{
		KMessageBox::sorry(this, i18n("Cannot link parent/child nodes"));
	}
	return legal;
}

bool KPTPertCanvas::legalParents(KPTNode *par, KPTNode *child)
{
    bool legal = true;
    //kdDebug()<<k_funcinfo<<par->name()<<" ("<<par->numDependParentNodes()<<" parents) "<<child->name()<<" ("<<child->numDependChildNodes()<<" children)"<<endl;
	for (int i=0; i < par->numDependParentNodes() && legal; ++i)
	{
		KPTNode *pNode = par->getDependParentNode(i)->parent();
		if (child->isParentOf(pNode) || pNode->isParentOf(child))
		{
	        //kdDebug()<<k_funcinfo<<"Found: "<<pNode->name()<<" is related to "<<child->name()<<endl;
			legal = false;
		}
		else
		    legal = legalChildren(pNode, child);
		if (legal)
		    legal = legalParents(pNode, child);
	}
	return legal;
}

bool KPTPertCanvas::legalChildren(KPTNode *par, KPTNode *child)
{
    bool legal = true;
    //kdDebug()<<k_funcinfo<<par->name()<<" ("<<par->numDependParentNodes()<<" parents) "<<child->name()<<" ("<<child->numDependChildNodes()<<" children)"<<endl;
	for (int j=0; j < child->numDependChildNodes() && legal; ++j)
	{
		KPTNode *cNode = child->getDependChildNode(j)->child();
		if (par->isParentOf(cNode) || cNode->isParentOf(par))
		{
			//kdDebug()<<k_funcinfo<<"Found: "<<par->name()<<" is related to "<<cNode->name()<<endl;
			legal = false;
		}
		else
		    legal = legalChildren(par, cNode);
	}
	return legal;
}

KPTPertNodeItem *KPTPertCanvas::selectedItem()
{
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it)
    {
        if ( (*it)->isSelected() )
		{
		if ( (*it)->rtti() == KPTPertProjectItem::RTTI ||
			(*it)->rtti() == KPTPertTaskItem::RTTI  ||
			(*it)->rtti() == KPTPertMilestoneItem::RTTI )
                return (KPTPertNodeItem *)(*it);
		}
    }
	return 0;
}

KPTNode *KPTPertCanvas::selectedNode() { 
    return selectedItem() ? &(selectedItem()->node()) : 0; 
}
 
#ifndef NDEBUG
void KPTPertCanvas::printDebug( int /*info*/ )
{
}
#endif

#include "kptpertcanvas.moc"
