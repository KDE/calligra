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
 
#include "kptpertview.h"

#include "kptview.h"
#include "kptpertcanvas.h"
#include "kptpart.h"
#include "kptproject.h"

#include <kdebug.h>

#include <qsplitter.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qheader.h>
#include <qpopupmenu.h>

#include <kprinter.h>

 KPTPertView::KPTPertView( KPTView *view, QWidget *parent, QLayout *layout )
    : QWidget( parent, "Pert view" ),
    m_mainview( view ),
	m_node( 0 )
 {
    init(layout);
}

 KPTPertView::~KPTPertView()
{
}

void KPTPertView::init(QLayout *layout)
{
    //kdDebug()<<k_funcinfo<<endl;
    QGridLayout *gl = new QGridLayout(this, 1, 1, -1, -1, "Pert QGridLayout");
    m_canvasview = new KPTPertCanvas(this);
	gl->addWidget(m_canvasview, 0, 0);
    draw();
    connect(m_canvasview, SIGNAL(rightButtonPressed(KPTNode *, const QPoint &)), this, SLOT(slotRMBPressed(KPTNode *,const QPoint &)));
    connect(m_canvasview, SIGNAL(updateView(bool)), m_mainview, SLOT(slotUpdate(bool)));
}    

void KPTPertView::draw() 
{
    //kdDebug()<<k_funcinfo<<endl;
    m_canvasview->draw(m_mainview->getPart()->getProject());
    m_canvasview->show();
}

void KPTPertView::slotRMBPressed(KPTNode *node, const QPoint & point)
{
    //kdDebug()<<k_funcinfo<<" node: "<<node->name()<<endl;
	m_node = node;
	QPopupMenu *menu = m_mainview->popupMenu("node_popup");
	if (menu)
	{
		int id = menu->exec(point);
	}
}

void KPTPertView::print(KPrinter &printer) {
    kdDebug()<<k_funcinfo<<endl;

}

KPTNode *KPTPertView::currentNode() {
    return m_canvasview->selectedNode(); 
}
 
#include "kptpertview.moc"
