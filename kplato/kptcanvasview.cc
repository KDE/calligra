/* This file is part of the KDE project
   Copyright (C) 2002 The Koffice Team <koffice@kde.org>

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

#include "kptcanvasview.h"
#include "kptprojectlist.h"
#include "kpttimescale.h"
#include "kptnode.h"
#include "kptnodeitem.h"
#include "kptrelation.h"
#include "kptrelationdialog.h"

#include <qbuffer.h>
#include <qtimer.h>
#include <qclipboard.h>
#include <qprogressdialog.h>
#include <qobjectlist.h>
#include <qpainter.h>
#include <qheader.h>
#include <qcursor.h>

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

KPTCanvasView::KPTCanvasView( QWidget *parent )
    : QCanvasView( parent, "Gantt canvas" /*WNorthWestGravity WStaticContents| WResizeNoErase | WRepaintNoErase */)
{
    setHScrollBarMode(QScrollView::AlwaysOn);
    m_canvas = new QCanvas( this );
    setCanvas( m_canvas );
    m_projectList = 0;
    m_linkMode = false;
    m_linkParentNode = 0;
}

KPTCanvasView::~KPTCanvasView()
{
}
/*
void KPTCanvasView::keyPressEvent( QKeyEvent *e )
{
    if( true ) {
        switch( e->key() ) {
        case Key_Down:
            setContentsPos( contentsX(), contentsY() + 10 );
            break;
        case Key_Up:
            setContentsPos( contentsX(), contentsY() - 10 );
            break;
        case Key_Left:
            setContentsPos( contentsX() - 10, contentsY() );
            break;
        case Key_Right:
            setContentsPos( contentsX() + 10, contentsY() );
            break;
        case Key_PageUp:
            setContentsPos( contentsX(), contentsY() - visibleHeight() );
            break;
        case Key_PageDown:
            setContentsPos( contentsX(), contentsY() + visibleHeight() );
            break;
        case Key_Home:
            setContentsPos( contentsX(), 0 );
            break;
        case Key_End:
            setContentsPos( contentsX(), contentsHeight() - visibleHeight() );
            break;
        default:
            break;
        }
    }
    // The key events in read-write mode are handled by eventFilter(), otherwise
    // we don't get <Tab> key presses.
}


void KPTCanvasView::contentsMousePressEvent( QMouseEvent *e )
{
}


void KPTCanvasView::contentsMouseMoveEvent( QMouseEvent *e )
{
}


void KPTCanvasView::contentsMouseReleaseEvent( QMouseEvent * e )
{
    if ( m_printing )
        return;
}

void KPTCanvasView::contentsMouseDoubleClickEvent( QMouseEvent * e )
{
    if ( m_printing )
        return;
}

void KPTCanvasView::contentsDragEnterEvent( QDragEnterEvent *e )
{
}

void KPTCanvasView::contentsDragMoveEvent( QDragMoveEvent *e )
{
}

void KPTCanvasView::contentsDragLeaveEvent( QDragLeaveEvent *e )
{
}

void KPTCanvasView::contentsDropEvent( QDropEvent *e )
{
}


void KPTCanvasView::slotContentsMoving( int cx, int cy )
{
}

void KPTCanvasView::resizeEvent( QResizeEvent *e )
{
}

bool KPTCanvasView::eventFilter( QObject *o, QEvent *e )
{
    if ( !o || !e )
        return TRUE;

    return QCanvasView::eventFilter( o, e );
}
*/
void KPTCanvasView::drawGantt( const KPTProjectList* list,  KPTTimeScale *timescale )
{
    kdDebug()<<k_funcinfo<<endl;
    m_projectList = list;
    clear();
    m_canvas->resize( timescale->totalWidth()+2, m_canvas->height() );
    updateContents();
    int y=0, h=0;
    // Draw bars
    QListViewItem *item = list->firstChild(); 
    kdDebug()<<k_funcinfo<<"First child="<<item->text(0)<<endl;        
    for ( ; item; item = item->itemBelow() )
    {
        KPTNodeItem *ni = (KPTNodeItem *)item;
        KPTNode& node = ni->getNode();
        kdDebug()<<k_funcinfo<<"Draw bar: Node="<<node.name()<<endl;        
        y = item->itemPos()+4;
        h = item->height()-8;
        node.drawGanttBar(m_canvas, timescale, y, h);
    }
    m_canvas->resize( timescale->totalWidth(), y + h + 8 );
    update();
}

void KPTCanvasView::drawPert()
{
    kdDebug()<<k_funcinfo<<endl;
    clear();
        
//     // Draw relations
//     for ( item = list->firstChild(); item; item = item->itemBelow() )
//     {
//         KPTNodeItem *ni = (KPTNodeItem *)item;
//         KPTNode& node = ni->getNode();
//         kdDebug()<<k_funcinfo<<"Draw relations: Node="<<node.name()<<endl;        
//         node.drawPerttRelations(m_canvas);
//     }
//    
}

void KPTCanvasView::clear()
{
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it) 
    {
        if ( *it )
            delete *it;
    }
}

void KPTCanvasView::setContentsPos( int x, int y )
{
    kdDebug()<<k_funcinfo<<endl;
    QCanvasView::setContentsPos( x, y );
}
void KPTCanvasView::contentsMouseReleaseEvent ( QMouseEvent * e )
{
    kdDebug()<<k_funcinfo<<" gl.X,gl.Y="<<e->globalX()<<","<<e->globalY()<<" x,y="<<e->x()<<","<<e->y()<<endl;
    switch (e->button())
    {
        case QEvent::LeftButton:
        {
            if (m_projectList)
            {
                QListViewItem * item = m_projectList->itemAt(QPoint(0,e->y()));
                if (!item)
                {
                    m_linkMode = false;
                    return;
                }
                kdDebug()<<k_funcinfo<<" item="<<item->text(0)<<endl;
                KPTNodeItem *ni = (KPTNodeItem *)item;
                if (!m_linkMode)
                {
                    kdDebug()<<k_funcinfo<<" First node="<<ni->getNode().name()<<endl;
                    m_linkParentNode = &(ni->getNode());
                    m_linkMode = true;
                    return;
                }
                if (m_linkParentNode->name() == ni->getNode().name())
                {
                    kdDebug()<<k_funcinfo<<" Same node"<<endl;
                    m_linkMode = false;
                    return;
                }
                kdDebug()<<k_funcinfo<<" Second node="<<ni->getNode().name()<<endl;
                // open relation dialog
                KPTRelationDialog *dia;
                KPTRelation *rel = ni->getNode().findRelation(m_linkParentNode);
                if (rel)
                    dia = new KPTRelationDialog(rel, this);
                else
                    dia = new KPTRelationDialog(m_linkParentNode, &(ni->getNode()), this);
                
                if (dia->exec())
                    kdDebug()<<k_funcinfo<<" Linked node="<<ni->getNode().name()<<" to "<<m_linkParentNode->name()<<endl; 
                delete dia;
                m_linkMode = false;
            }
            break;
        }
        case QEvent::RightButton:
        {
            if (m_projectList)
            {
                QListViewItem * item = m_projectList->itemAt(QPoint(0,e->y()));
                emit rightButtonPressed(item, e->globalPos(), -1);
            }
            break;
        }
        case QEvent::MidButton:
            break;
    }
}
#ifndef NDEBUG
void KPTCanvasView::printDebug( int /*info*/ )
{
}
#endif

#include "kptcanvasview.moc"
