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
	m_summaryColumn(0),
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
    kdDebug()<<k_funcinfo<<endl;
    clear();
	m_summaryColumn = 0;
    updateContents();
	project.setDrawn(false, true);
	//project.drawPert(this);

	QPtrListIterator<KPTNode> nit(project.childNodeIterator());
    for ( ; nit.current(); ++nit ) {
        drawChildren(nit.current());
	}

	QPtrListIterator<KPTNode> it(project.childNodeIterator());
    for ( ; it.current(); ++it ) {
        drawChildren(it.current());
	}

	drawRelations(); // done _after_ all nodes are drawn
	QSize s = canvasSize();
	m_canvas->resize(s.width(), s.height());
    update();
}

void KPTPertCanvas::drawChildren(KPTNode* node)
{
    kdDebug()<<k_funcinfo<<"node="<<node->name()<<endl;

	if ( node->type() == KPTNode::Type_Project)
		drawProject( node );
	else if (node->type() == KPTNode::Type_Subproject)
		drawSubproject( node );
	else if (node->type() == KPTNode::Type_Task)
		drawTask( node );
	else if (node->type() == KPTNode::Type_Milestone)
		drawMilestone( node );
	else
		kdDebug()<<k_funcinfo<<"Not implemented yet"<<endl;

	QPtrListIterator<KPTNode> nit(node->childNodeIterator());
	for ( ; nit.current(); ++nit ) {
		drawChildren(nit.current());
	}
}

void KPTPertCanvas::drawProject( KPTNode *node)
{

}

void KPTPertCanvas::drawSubproject( KPTNode *node)
{
    kdDebug()<<k_funcinfo<<endl;
	if ( !node->isDrawn()) {
	    if (node->numDependChildNodes() == 0 &&
		    node->numDependParentNodes() == 0)
		{
			int col = summaryColumn();
			KPTPertProjectItem* pertItem = new KPTPertProjectItem(this, *node, 0, col);
			pertItem->show();
			node->setPertItem( pertItem );
			node->setDrawn( true, false);
			kdDebug()<<k_funcinfo<<" draw project("<<0<<","<<col<<"): "<<node->name()<<endl;
			return;
		}
		if (!node->allParentsDrawn()) {
			return;
		}
		int col = node->getColumn(node);
		int rowx = row(node->getRow(node), col);
		KPTPertTaskItem* pertItem = new KPTPertTaskItem(this,*node, rowx, col);
		node->setPertItem( pertItem );
		pertItem->show();
		node->setDrawn( true,false);
		//kdDebug()<<k_funcinfo<<" draw ("<<row<<","<<col<<"): "<<m_name<<endl;
	}
}


void KPTPertCanvas::drawMilestone( KPTNode *node)
{
	kdDebug()<< "draw task " << node->name()<<endl;
	if ( !node->isDrawn()) {
		if ( node->numChildren() > 0 ) {
			int col = summaryColumn();
			KPTPertMilestoneItem* pertItem = new KPTPertMilestoneItem(this, *node, 0, col);
			node->setPertItem( pertItem );
			pertItem->show();
			node->setDrawn( true, false ); // drawn, but not recursive
			kdDebug()<<k_funcinfo<<" drawn milestone(?)("<<0<<","<<col<<"): "<<node->name()<<endl;
			return;
		}
		if (!node->allParentsDrawn()) {
			return;
		}
		int col = node->getColumn();
		int rowx = row(node->getRow(), col);
		KPTPertMilestoneItem* pertItem = new KPTPertMilestoneItem(this, *node, rowx, col);
		pertItem->show();
		node->setPertItem( pertItem );
		node->setDrawn( true, false ); // drawn, but not recursive
	}
}


void KPTPertCanvas::drawTask( KPTNode *node)
{
	kdDebug()<< "draw task " << node->name()<<endl;
	if ( !node->isDrawn()) {
		if ( node->numChildren() > 0  &&
	         node->numDependChildNodes() == 0 &&
		     node->numDependParentNodes() == 0)
		{
			int col = summaryColumn();
			KPTPertTaskItem* pertItem = new KPTPertTaskItem(this, *node, 0, col);
			node->setPertItem( pertItem );
			pertItem->show();
			node->setDrawn( true, false );
			kdDebug()<<k_funcinfo<<" drawn summary task("<<0<<","<<col<<"): "<< node->name() <<endl;
			return;
		}
		if (!node->allParentsDrawn()) {
			return;
		}
		int col = node->getColumn(node);
		int xrow = row(node->getRow(node), col);

		KPTPertTaskItem* pertItem = new KPTPertTaskItem(this, *node, xrow, col);
		node->setPertItem( pertItem );
		pertItem->show();
		node->setDrawn( true,false );
		//kdDebug()<<k_funcinfo<<" draw ("<<row<<","<<col<<"): "<<m_name<<endl;
	}

}

void KPTPertCanvas::drawRelations()
{
	kdDebug()<<k_funcinfo<<endl;
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it)
    {
	    if ( (*it)->rtti() == KPTPertTaskItem::RTTI ||
			 (*it)->rtti() == KPTPertMilestoneItem::RTTI )
		{
			KPTPertNodeItem *item = dynamic_cast<KPTPertNodeItem *>(*it);
			int numRelations = item->node().numDependChildNodes();
			for (int i=0; i < numRelations; ++i)
			{
				drawRelation(item->node().getDependChildNode(i));
			}
		}
	}
}

void KPTPertCanvas::drawRelation( KPTRelation* relation)
{
	kdDebug()<<k_funcinfo<<endl;
	//Only draw if both nodes are shown on canvas
	if (relation->parent()->isDrawn() && relation->child()->isDrawn())
	{
		KPTRelationCanvasItem *item = new KPTRelationCanvasItem(this, relation);
		item->show();
	}
}


QSize KPTPertCanvas::canvasSize()
{
    kdDebug()<<k_funcinfo<<endl;
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
    kdDebug()<<k_funcinfo<<" gl.X,gl.Y="<<e->globalX()<<","<<e->globalY()<<" x,y="<<e->x()<<","<<e->y()<<endl;
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
						kdDebug()<<k_funcinfo<<" First node="<<item->node().name()<<endl;
						item->setSelected(true);
						canvas()->update();
						return;
					}
					par->setSelected(false);
					if (&(item->node()) == &(par->node()))
					{
						break;
					}
					kdDebug()<<k_funcinfo<<" Second node="<<item->node().name()<<endl;
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
    kdDebug()<<k_funcinfo<<par.name()<<" ("<<par.numDependParentNodes()<<" parents) "<<child.name()<<" ("<<child.numDependChildNodes()<<" children)"<<endl;
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
	        kdDebug()<<k_funcinfo<<"Found: "<<pNode->name()<<" is related to "<<child->name()<<endl;
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
			kdDebug()<<k_funcinfo<<"Found: "<<par->name()<<" is related to "<<cNode->name()<<endl;
			legal = false;
		}
		else
		    legal = legalChildren(par, cNode);
	}
	return legal;
}

int KPTPertCanvas::row(int minrow, int col)
{
	QValueList<int> rows;
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it)
    {
		if ( (*it)->rtti() == KPTPertProjectItem::RTTI ||
			(*it)->rtti() == KPTPertTaskItem::RTTI  ||
			(*it)->rtti() == KPTPertMilestoneItem::RTTI )
		{
		    KPTPertNodeItem *item = (KPTPertNodeItem *)(*it);
            if ( item->column() == col )
			{
    		    rows.append(item->row());
			}
		}
    }
	int row = minrow;
	if (!rows.empty())
	{
    	qHeapSort(rows);
    	bool found = false;
		for (uint i=0; i < rows.size(); ++i)
		{
			if ( rows[i] < minrow)
			    continue;
			if ( rows[i] == minrow)
			    found = true;
            if (found)
			{
			    if (row !=rows[i])
				    break;          // free row
			    ++row;
			}
		}
	}
    //kdDebug()<<k_funcinfo<<" col="<<col<<" free row="<<row<<endl;
	return row;
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

#ifndef NDEBUG
void KPTPertCanvas::printDebug( int /*info*/ )
{
}
#endif

#include "kptpertcanvas.moc"
