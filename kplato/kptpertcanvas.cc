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

#include <KoStore.h>
#include <ktempfile.h>
#include <klocale.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <assert.h>
#include <kmultipledrag.h>
#include <klistview.h>

namespace KPlato
{

PertCanvas::PertCanvas( QWidget *parent )
    : QCanvasView( parent, "Pert canvas" /*WNorthWestGravity WStaticContents| WResizeNoErase | WRepaintNoErase */),
	m_verticalGap(20),
	m_horizontalGap(10),
	m_itemSize(100,30)

{
    //setHScrollBarMode(QScrollView::AlwaysOn);
    m_canvas = new QCanvas( this );
    setCanvas( m_canvas );
}

PertCanvas::~PertCanvas()
{
}

void PertCanvas::draw(Project& project)
{
    //kdDebug()<<k_funcinfo<<endl;
    clear();
    updateContents();

    // First make node items
    QPtrListIterator<Node> nit(project.childNodeIterator());
    for ( ; nit.current(); ++nit ) {
        createChildItems(createNodeItem(nit.current()));
    }

    // First all items with relations
    QPtrDictIterator<PertNodeItem> it(m_nodes);
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
    QSize s = canvasSize();
    m_canvas->resize(s.width(), s.height());
    update();
}

PertNodeItem *PertCanvas::createNodeItem(Node *node)
{
    PertNodeItem *item = m_nodes.find(node);
    if (!item)
    {
        if ( node->type() == Node::Type_Project)
            kdDebug()<<k_funcinfo<<"Project nodes should not have relations"<<endl;
        else if (node->type() == Node::Type_Subproject)
            item  = new PertProjectItem(this, *node);
        else if (node->type()== Node::Type_Summarytask)
            item  = new PertTaskItem(this, *node);
        else if (node->type()== Node::Type_Task)
            item  = new PertTaskItem(this, *node);
        else if (node->type() == Node::Type_Milestone)
            item  = new PertMilestoneItem(this, *node);
        else
            kdDebug()<<k_funcinfo<<"Not implemented yet"<<endl;

        if (item)
            m_nodes.insert(node, item);
    }
    return item;
}

void PertCanvas::createChildItems(PertNodeItem *parentItem)
{
    //kdDebug()<<k_funcinfo<<"parentItem="<<(parentItem ? parentItem->node().name() : "nil")<<endl;
    if (!parentItem)
        return;

    QPtrListIterator<Relation> it(parentItem->node().dependChildNodes());
    for (; it.current(); ++it)
    {
        PertNodeItem *childItem = createNodeItem(it.current()->child());
        if (childItem)
            parentItem->addChildRelation(it.current(), childItem);
            m_relations.append(it.current());
    }

    // Now my children
	QPtrListIterator<Node> nit(parentItem->node().childNodeIterator());
    for ( ; nit.current(); ++nit ) {
        createChildItems(createNodeItem(nit.current()));
	}
}

void PertCanvas::drawRelations()
{
	//kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<Relation> it(m_relations);
    for (; it.current(); ++it)
    {
        PertNodeItem *parentItem = m_nodes.find(it.current()->parent());
        PertNodeItem *childItem = m_nodes.find(it.current()->child());
        if (parentItem && childItem)
        {
            PertRelationItem *item = new PertRelationItem(this, parentItem, childItem, it.current());
            item->show();
        }
    }
}

void PertCanvas::mapNode(PertNodeItem *item)
{
	//kdDebug()<<k_funcinfo<<endl;
    if (! m_rows.at(item->row()) || (item->column() >= 0 && m_rows.at(item->row())->count() <= uint(item->column())))
    {
        kdError()<<k_funcinfo<<item->node().name()<<": non existing map for: ("<<item->row()<<","<<item->column()<<")"<<endl;
        return;
    }
    m_rows.at(item->row())->at(item->column()) = true;
}

void PertCanvas::mapChildNode(PertNodeItem *parentItem, PertNodeItem *childItem, Relation::Type type)
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

    if (type == Relation::StartStart ||
        type == Relation::FinishFinish)
    {
        // node goes into row below parent, at least same col
        if (chMapped)
        {
            m_rows.at(chRow)->at(chCol) = false;
            //kdDebug()<<k_funcinfo<<" Moving "<<childItem->node().name()<<" from: "<<chRow<<","<<chCol<<endl;
            if (chRow <= row)
            {
                chRow = row+1;
                if (chRow >= 0 && m_rows.count() <= uint(chRow)) {
                    m_rows.append(new QMemArray<bool>(1)); // make a new row
                    chRow = m_rows.count()-1;  // to be safe
                }
                //kdDebug()<<k_funcinfo<<" Moving "<<childItem->node().name()<<" to row: "<<chRow<<endl;
            }
            if (chCol < col)
            {
                chCol = col;
                if (chCol >= 0 && m_rows.at(chRow)->count() <= uint(chCol))  // col does not exist
                    m_rows.at(chRow)->resize(chCol+1);
                
                //kdDebug()<<k_funcinfo<<" Moved "<<childItem->node().name()<<" to col: "<<chCol<<endl;
            }

        }
        else
        {
            if (!(m_rows.at(row+1)) ||                        // next row does not exists
                m_rows.at(row+1)->at(col) == true)  // col is not free
            {
                m_rows.append(new QMemArray<bool>(col+1)); // make a new row
            }
            else if (col >= 0 && m_rows.at(row+1)->count() <= uint(col))  // col does not exist
                m_rows.at(row)->resize(col+1);

            chRow = m_rows.count() -1;
            chCol = col;
        }
    }
    else if (type == Relation::FinishStart)
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
            if (chCol >= 0 && m_rows.at(chRow)->count() <= uint(chCol))  // col does not exist
                m_rows.at(chRow)->resize(chCol+1);
        }
        else
        {
            ++col;
            if (col >= 0 && m_rows.at(row)->count() <= uint(col))
                m_rows.at(row)->resize(col+1); // make new column
            else if (m_rows.at(row)->at(col) = true)
                m_rows.append(new QMemArray<bool>(col+1)); // col not free, so make a new row

            chRow = m_rows.count() -1;
            chCol = col;
        }
    }
    else
    {
        kdError()<<k_funcinfo<<"Unknow relation type"<<endl;
        return;
    }
    childItem->move(this, chRow, chCol);
}

QSize PertCanvas::canvasSize()
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

void PertCanvas::clear()
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

void PertCanvas::contentsMousePressEvent ( QMouseEvent * e )
{
    //kdDebug()<<k_funcinfo<<" gl.X,gl.Y="<<e->globalX()<<","<<e->globalY()<<" x,y="<<e->x()<<","<<e->y()<<endl;
    switch (e->button())
    {
        case QEvent::LeftButton:
        {
            break;
        }
        case QEvent::RightButton:
        {
            PertNodeItem *item = selectedItem();
            if (item)
                item->setSelected(false);
            canvas()->update();
            
            QCanvasItemList l = canvas()->collisions(e->pos());
            for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
            {
                if ( (*it)->rtti() == PertProjectItem::RTTI ||
                     (*it)->rtti() == PertTaskItem::RTTI  ||
                     (*it)->rtti() == PertMilestoneItem::RTTI )
                {
                    PertNodeItem *item = (PertNodeItem *)(*it);
                    {
                        item->setSelected(true);
                        canvas()->update();
                        emit rightButtonPressed(&(item->node()), e->globalPos());
                        if (item == selectedItem()) {
                            // item maybe deleted
                            item->setSelected(false);
                        }
                        canvas()->update();
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

void PertCanvas::contentsMouseReleaseEvent ( QMouseEvent * e )
{
    //kdDebug()<<k_funcinfo<<" gl.X,gl.Y="<<e->globalX()<<","<<e->globalY()<<" x,y="<<e->x()<<","<<e->y()<<endl;
    switch (e->button())
    {
        case QEvent::LeftButton:
        {
            bool hit = false;
            QCanvasItemList l = canvas()->collisions(e->pos());
            for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
		    {
	            if ( (*it)->rtti() == PertProjectItem::RTTI ||
	                 (*it)->rtti() == PertTaskItem::RTTI  ||
	                 (*it)->rtti() == PertMilestoneItem::RTTI )
				{
                    hit = true;
				    PertNodeItem *item = (PertNodeItem *)(*it);
					PertNodeItem *par = selectedItem();
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
					if (!par->node().legalToLink(&(item->node()))) {
                        KMessageBox::sorry(this, i18n("Cannot link these nodes"));
					} else {
						Relation *rel = item->node().findRelation(&(par->node()));
						if (rel)
                            emit modifyRelation(rel);
						else
							emit addRelation(&(par->node()), &(item->node()));
					}
					break;
				}
			}
            if (!hit) {
                PertNodeItem *i = selectedItem();
                if (i) i->setSelected(false);
            }
            canvas()->update();
            break;
        }
        case QEvent::RightButton:
        {
            break;
        }
        case QEvent::MidButton:
            break;
        default:
            break;
    }
}

PertNodeItem *PertCanvas::selectedItem()
{
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it)
    {
        if ( (*it)->isSelected() )
		{
		if ( (*it)->rtti() == PertProjectItem::RTTI ||
			(*it)->rtti() == PertTaskItem::RTTI  ||
			(*it)->rtti() == PertMilestoneItem::RTTI )
                return (PertNodeItem *)(*it);
		}
    }
	return 0;
}

Node *PertCanvas::selectedNode() { 
    return selectedItem() ? &(selectedItem()->node()) : 0; 
}
 
#ifndef NDEBUG
void PertCanvas::printDebug( int /*info*/ )
{
}
#endif

}  //KPlato namespace

#include "kptpertcanvas.moc"
