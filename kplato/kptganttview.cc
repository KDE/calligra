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
 
#include "kptganttview.h"

#include "kpttimescale.h"
#include "kptprojectlist.h"
#include "kptview.h"
#include "kptganttcanvas.h"
#include "kptnodeitem.h"
#include "kptpart.h"
#include "kptproject.h"

#include <kdebug.h>

#include <qsplitter.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qheader.h>
#include <qpopupmenu.h>

 KPTGanttView::KPTGanttView( KPTView *view, QWidget *parent, QLayout *layout )
    : QSplitter( parent, "Gantt view" ),
    m_mainview( view )
 {
    init(layout);
    kdDebug()<<k_funcinfo<<" end"<<endl;
}

void KPTGanttView::init(QLayout *layout)
{
    // Split the right side into a listview and data presentation view (for gantt, pert, etc)
    
    m_projectlist = new KPTProjectList( m_mainview, this );

    QVBox *g = new QVBox( this, "Gantt QVBox" );
    QGridLayout *gl = new QGridLayout( g, 2, 1, -1, -1, "Gantt QGridLayout");
    
    m_timescale = new KPTTimeScale(g, m_mainview->getPart()->getProject().getEarliestStart(), 
                                                            m_mainview->getPart()->getProject().getLatestFinish(), 
                                                            m_projectlist->header()->height());
    gl->addWidget(m_timescale,0,0);

    m_canvasview = new KPTGanttCanvas(g);
    gl->addWidget(m_canvasview,1,0);
    draw();

    // Use m_canvasview's vert. scrollbar for the projectlist also (They should have the same height)
    connect(m_canvasview, SIGNAL(contentsMoving(int,int)), m_projectlist, SLOT(slotSetContentsPos(int,int)));
    //  Use m_canvasview's hor. scrollbar for the timescale also
    connect(m_canvasview, SIGNAL(contentsMoving(int,int)), m_timescale, SLOT(slotSetContents(int,int)));
    
    connect(m_projectlist, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(slotOpen(QListViewItem *)));
    
    connect(m_projectlist, SIGNAL(rightButtonPressed(QListViewItem *, const QPoint &, int)), this, SLOT(slotRMBPressed(QListViewItem *,const QPoint &, int)));
    
	connect(m_canvasview, SIGNAL(rightButtonPressed(QListViewItem *, const QPoint &, int)), this, SLOT(slotRMBPressed(QListViewItem *,const QPoint &, int)));
    connect(m_canvasview, SIGNAL(updateView(bool)), m_mainview, SLOT(slotUpdate(bool)));
	
    // Save the un-zoomed font size
	m_defaultFontSize = m_projectlist->font().pointSize();
}    

void KPTGanttView::zoom(double zoom)
{
    QFont f = m_projectlist->font();
    f.setPointSize(qRound(m_defaultFontSize * zoom));
    m_projectlist->setFont(f);
}

void KPTGanttView::draw() 
{
    kdDebug()<<k_funcinfo<<endl;
    m_timescale->draw(m_mainview->getPart()->getProject().getEarliestStart(), 
                                 m_mainview->getPart()->getProject().getLatestFinish(), 
                                 m_projectlist->header()->height());
                                 
    m_canvasview->draw( m_projectlist, m_timescale );
    m_canvasview->show();
    m_projectlist->show();
}

KPTNodeItem *KPTGanttView::currentItem()
{
    return (KPTNodeItem *)m_projectlist->currentItem();
}

KPTNodeItem *KPTGanttView::selectedItem()
{
    return (KPTNodeItem *)m_projectlist->selectedItem();
}

void KPTGanttView::slotOpen(QListViewItem *item) 
{
    if (item)
    {
        static_cast<KPTNodeItem *>(item)->openDialog();
    }
}

void KPTGanttView::slotRMBPressed(QListViewItem *item, const QPoint & point, int col)
{
    kdDebug()<<k_funcinfo<<endl;
    if (item)
    {
        m_projectlist->clearSelection();
        m_projectlist->setSelected(item,true);
        m_projectlist->setCurrentItem(item);
        QPopupMenu *menu = m_mainview->popupMenu("node_popup");
        if (menu)
        {
            int id = menu->exec(point);
            kdDebug()<<k_funcinfo<<"id="<<id<<endl;
        }
        else
            kdDebug()<<k_funcinfo<<"No menu!"<<endl;
    }
}

#include "kptganttview.moc"
