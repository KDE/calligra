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

#include "kptganttcanvas.h"
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

KPTGanttCanvas::KPTGanttCanvas( QWidget *parent )
    : QCanvasView( parent, "Gantt canvas" )
{
    setHScrollBarMode(QScrollView::AlwaysOn);
    m_canvas = new QCanvas( this );
    setCanvas( m_canvas );
    m_projectList = 0;
}

KPTGanttCanvas::~KPTGanttCanvas()
{
}

/*
void KPTGanttCanvas::keyPressEvent( QKeyEvent *e )
{
    if( true ) {
        switch( e->key() ) {
        case Key_Down:
            break;
        default:
            break;
        }
    }
}


void KPTGanttCanvas::contentsMousePressEvent( QMouseEvent *e )
{
}


void KPTGanttCanvas::contentsMouseMoveEvent( QMouseEvent *e )
{
}


void KPTGanttCanvas::contentsMouseReleaseEvent( QMouseEvent * e )
{
    if ( m_printing )
        return;
}

void KPTGanttCanvas::contentsMouseDoubleClickEvent( QMouseEvent * e )
{
    if ( m_printing )
        return;
}

void KPTGanttCanvas::contentsDragEnterEvent( QDragEnterEvent *e )
{
}

void KPTGanttCanvas::contentsDragMoveEvent( QDragMoveEvent *e )
{
}

void KPTGanttCanvas::contentsDragLeaveEvent( QDragLeaveEvent *e )
{
}

void KPTGanttCanvas::contentsDropEvent( QDropEvent *e )
{
}


void KPTGanttCanvas::slotContentsMoving( int cx, int cy )
{
}

void KPTGanttCanvas::resizeEvent( QResizeEvent *e )
{
}

bool KPTGanttCanvas::eventFilter( QObject *o, QEvent *e )
{
    if ( !o || !e )
        return TRUE;

    return QCanvasView::eventFilter( o, e );
}
*/

void KPTGanttCanvas::draw( const KPTProjectList* list,  KPTTimeScale *timescale )
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

void KPTGanttCanvas::clear()
{
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it) 
    {
        if ( *it )
            delete *it;
    }
}

void KPTGanttCanvas::setContentsPos( int x, int y )
{
    kdDebug()<<k_funcinfo<<endl;
    QCanvasView::setContentsPos( x, y );
}
void KPTGanttCanvas::contentsMouseReleaseEvent ( QMouseEvent * e )
{
    kdDebug()<<k_funcinfo<<" gl.X,gl.Y="<<e->globalX()<<","<<e->globalY()<<" x,y="<<e->x()<<","<<e->y()<<endl;
    switch (e->button())
    {
        case QEvent::LeftButton:
        {
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
void KPTGanttCanvas::printDebug( int /*info*/ )
{
}
#endif

#include "kptganttcanvas.moc"
