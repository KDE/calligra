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
 
#include "kptprojectlist.h"

#include "kptview.h"
#include "kptnodeitem.h"
#include "kptproject.h"
#include "kptnode.h"
#include "kptpart.h"

#include <kdebug.h>
#include <klocale.h>

#include <qsplitter.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qheader.h>


 KPTProjectList::KPTProjectList( KPTView *view, QWidget *parent )
    : KListView( parent ),
    m_mainview( view )
 { 
    setVScrollBarMode(QScrollView::AlwaysOff);
    setHScrollBarMode(QScrollView::AlwaysOn);
    setSelectionModeExt(KListView::Extended);
    setSorting(-1); // Off
    setShowSortIndicator(false);
    addColumn(i18n("Project"));
    addColumn(i18n("Responsible"));
    addColumn(i18n("Description"));
    displayProject();
    
    kdDebug()<<k_funcinfo<<"Listview geom: "<<geometry().x()<<","<<geometry().y()<<"  "<<geometry().width()<<"x"<<geometry().height()<<endl;
    QHeader *h = header();
    kdDebug()<<k_funcinfo<<"h geom: "<<h->geometry().x()<<","<<h->geometry().y()<<"  "<<h->geometry().width()<<"x"<<h->geometry().height()<<endl;
}

 KPTProjectList::~KPTProjectList()
{
}

void KPTProjectList::displayProject() {
    // Clean old project display
    clear();

    // Add the top level project and select it
    KPTProject &project = m_mainview->getPart()->getProject();
    KPTNodeItem *i = new KPTNodeItem(this, project);
    i->setOpen(true);
    setSelected(i, true);
    // Now recursively add all subitems
    displayChildren(project, i);
}


void KPTProjectList::displayChildren(const KPTNode &node, KPTNodeItem *item) {
    // Add all children of node to the view, and add all their children too
    for (int i=0; i<node.numChildren(); i++) {
    	// First add the child
	    KPTNode &n = node.getChildNode(i);
	    KPTNodeItem *ni = new KPTNodeItem(item, n);
    	ni->setOpen(true);

	    // Now add all it's children
    	displayChildren(n, ni);
    }
}

int KPTProjectList::canvasHeight() const
{
    kdDebug()<<k_funcinfo<<"tot height="<<height()<<" header height="<<header()->geometry().height()<<endl;
    return (height() - header()->geometry().height());
}

void KPTProjectList::slotSetContentsPos(int /*x*/, int y)
{
    setContentsPos(contentsX(), y);
}

#include "kptprojectlist.moc"
