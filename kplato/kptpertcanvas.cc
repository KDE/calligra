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
#include "kptnodeitem.h"
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
    : QCanvasView( parent, "Pert canvas" /*WNorthWestGravity WStaticContents| WResizeNoErase | WRepaintNoErase */)
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
    updateContents();
	m_rows.resize(0);
	project.drawPert(this, m_canvas, 0);
	project.drawPertRelations(m_canvas);
	QSize s = canvasSize();
	m_canvas->resize(s.width(), s.height());
    update();
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
	            if ( (*it)->rtti() == KPTPertCanvasItem::RTTI )
				{
				    KPTPertCanvasItem *item = (KPTPertCanvasItem *)(*it);
					if (m_linkMode)
					{
						m_linkParentNode->pertItem()->setSelected(false);
						if (item->node().name() == m_linkParentNode->name())
						{
							m_linkMode = false;
							break;
						}
						kdDebug()<<k_funcinfo<<" Second node="<<item->node().name()<<endl;
						// open relation dialog
						KPTRelationDialog *dia;
						KPTRelation *rel = item->node().findRelation(m_linkParentNode);
						if (rel)
							dia = new KPTRelationDialog(rel, this);
						else
							dia = new KPTRelationDialog(m_linkParentNode, &(item->node()), this);

						if (dia->exec())
						{
							kdDebug()<<k_funcinfo<<" Linked node="<<item->node().name()<<" to "<<m_linkParentNode->name()<<endl; 
							emit updateView(true);
						}
						delete dia;
						m_linkMode = false;
						break;
					}
					else
					{
						kdDebug()<<k_funcinfo<<" First node="<<item->node().name()<<endl;
						item->setSelected(true);
						m_linkParentNode = &(item->node());
						m_linkMode = true;
						canvas()->update();
						return;
					}
				}
				m_linkMode = false;
			}
            if (m_linkMode)
                m_linkParentNode->pertItem()->setSelected(false);
            canvas()->update();
            break;
        }
        case QEvent::RightButton:
        {
            QCanvasItemList l = canvas()->collisions(e->pos());
            for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) 
		    {
	            if ( (*it)->rtti() == KPTPertCanvasItem::RTTI )
				{
				    KPTPertCanvasItem *item = (KPTPertCanvasItem *)(*it);
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
    }
}

void KPTPertCanvas::setRow(int row, int col)
{
    if (m_rows.count() <= col)
	{
	    m_rows.resize(col+1);
		m_rows[col] = 0;
	}
	if (m_rows[col] < row);
	    m_rows[col] = row;
	kdDebug()<<k_funcinfo<<"m_rows["<<col<<"]="<<m_rows[col]<<" row="<<row<<endl;
}

int KPTPertCanvas::row(int col)
{
    if (m_rows.size() <= col)
	    setRow(0, col);
	kdDebug()<<k_funcinfo<<"m_rows["<<col<<"]="<<m_rows[col]<<endl;
    return m_rows[col];
}

#ifndef NDEBUG
void KPTPertCanvas::printDebug( int /*info*/ )
{
}
#endif

#include "kptpertcanvas.moc"
