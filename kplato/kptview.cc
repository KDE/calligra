/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#include "kptview.h"
#include "kptfactory.h"
#include "kptpart.h"
#include "kptproject.h"
#include "kptprojectdialog.h"
#include "kptnodeitem.h"
#include "kpttask.h"
#include "kpttaskdialog.h"
#include "kptmilestone.h"
#include "kptmilestonedialog.h"

#include <koKoolBar.h>

#include <qpainter.h>
#include <qiconset.h>
#include <qlayout.h>
#include <qsplitter.h>

#include <kiconloader.h>
#include <kaction.h>
#include <kstdaction.h>
#include <klocale.h>
#include <kdebug.h>
#include <klistview.h>
#include <kstdaccel.h>
#include <kaccelgen.h>
#include <kdeversion.h>

KPTView::KPTView(KPTPart* part, QWidget* parent, const char* name)
    : KoView(part, parent, name)
{
    setInstance(KPTFactory::global());
    setXMLFile("kplato.rc");
  
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setAutoAdd( true );
    
    // Make a bar to the left
    KoKoolBar *bar = new KoKoolBar(this);
    bar->setFixedWidth( 65 );
    bar->setMinimumHeight( 200 );

    m_viewGrp = bar->insertGroup(i18n("View"));
    QPixmap pixmap = KPTBarIcon( "gantt_chart" );
    int id = bar->insertItem( m_viewGrp, pixmap,"Gantt",
                this, SLOT( slotKoolBar( int, int ) ) );
    pixmap = KPTBarIcon( "pert_chart" );
    id = bar->insertItem( m_viewGrp, pixmap,"PERT",
                this, SLOT( slotKoolBar( int, int ) ) );
    pixmap = KPTBarIcon( "resources" );
    id = bar->insertItem( m_viewGrp, pixmap,"Resources",
                this, SLOT( slotKoolBar( int, int ) ) );

    // Split the right side into a listview and data presentation view (Gantt, pert, etc)
    QSplitter *split = new QSplitter( this );
    m_listview = new KListView(split);
    QListBox *lb2 = new QListBox( split );
    
    // The main project view
    m_listview->setSelectionModeExt(KListView::Extended);
    //m_listview->setSorting(-1); // Off
    m_listview->setShowSortIndicator(true);
    m_listview->addColumn(i18n("Project"));
    m_listview->addColumn(i18n("Leader"));
    m_listview->addColumn(i18n("Description"));

    
    connect(m_listview, SIGNAL(selectionChanged()), this,
	    SLOT(slotSelectionChanged()));
        
    connect(m_listview, SIGNAL(doubleClicked(QListViewItem *)), this,
        SLOT(slotOpen(QListViewItem *)));

    // The menu items
    // ------ Edit
    actionEditCut = KStdAction::cut( this, SLOT( slotEditCut() ), actionCollection(), "edit_cut" );
    actionEditCut = KStdAction::copy( this, SLOT( slotEditCopy() ), actionCollection(), "edit_copy" );
    actionEditCut = KStdAction::paste( this, SLOT( slotEditPaste() ), actionCollection(), "edit_paste" );
    // ------ View
    new KAction(i18n("Gantt"), "gantt_chart", 0, this,
		SLOT(slotViewGantt()), actionCollection(), "view_gantt");
    new KAction(i18n("PERT"), "pert_chart", 0, this,
		SLOT(slotViewPert()), actionCollection(), "view_pert");
    new KAction(i18n("Resources"), "resources", 0, this,
		SLOT(slotViewResources()), actionCollection(), "view_resources");
    
    // ------ Insert
    new KAction(i18n("Sub-Project..."), "add_sub_project", 0, this,
		SLOT(slotAddSubProject()), actionCollection(), "add_sub_project");
    new KAction(i18n("Task..."), "add_task", 0, this,
		SLOT(slotAddTask()), actionCollection(), "add_task");
    new KAction(i18n("Milestone..."), "add_milestone", 0, this,
		SLOT(slotAddMilestone()), actionCollection(), "add_milestone");

    // ------ Tools
    new KAction(i18n("Edit..."), "edit_resource", 0, this,
		SLOT(slotEditResource()), actionCollection(), "edit_resource");
    
    // ------ Project
    new KAction(i18n("Edit Main Project..."), "project_edit", 0, this,
		SLOT(slotEditProject()), actionCollection(), "project_edit");
    
    // ------ Tools
    new KAction(i18n("Resource Editor..."), "edit_resource", 0, this,
		SLOT(slotEditResource()), actionCollection(), "edit_resource");

    // ------ Settings
    new KAction(i18n("Configure..."), "configure", 0, this,
		SLOT(slotConfigure()), actionCollection(), "configure");

            
    // ------------------- Actions with a key binding and no GUI item
#ifndef NDEBUG
    KAction* actPrintDebug = new KAction( i18n( "Print debug" ), CTRL+SHIFT+Key_P,
                        this, SLOT( slotPrintDebug() ), actionCollection(), "print_debug" );
#endif 
    // Necessary for the actions that are not plugged anywhere
    // Deprecated with KDE-3.1.
    // Not entirely sure it's necessary for 3.0, please test and report.
#if KDE_VERSION < 305
    KAccel * accel = new KAccel( this );
#ifndef NDEBUG
    actPrintDebug->plugAccel( accel );
#endif
#else
    // Stupid compilers ;)
#ifndef NDEBUG
    Q_UNUSED( actPrintDebug );
#endif
#endif
        
    // Show the project
    displayProject();

    // Save the un-zoomed font size
    m_defaultFontSize = m_listview->font().pointSize();
}


void KPTView::setZoom(double zoom) {
    QFont f = m_listview->font();
    f.setPointSize(qRound(m_defaultFontSize * zoom));
    m_listview->setFont(f);
}


void KPTView::displayProject() {
    // Clean old project display
    m_listview->clear();

    // Add the top level project and select it
    KPTProject &project = ((KPTPart *)koDocument())->getProject();
    KPTNodeItem *i = new KPTNodeItem(m_listview, project);
    i->setOpen(true);
    m_listview->setSelected(i, true);
    // Now recursively add all subitems
    displayChildren(project, i);
}


void KPTView::displayChildren(const KPTNode &node, KPTNodeItem *item) {
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

void KPTView::slotEditCut() {
}

void KPTView::slotEditCopy() {
}

void KPTView::slotEditPaste() {
}

void KPTView::slotViewGantt() {
}

void KPTView::slotViewPert() {
}

void KPTView::slotViewResources() {
}

void KPTView::slotEditProject() {
    ((KPTPart *)koDocument())->editProject();
    displayProject();
}


void KPTView::slotAddSubProject() {
    KPTProject *proj = new KPTProject();
    if (proj->openDialog()) {
        KPTNodeItem *curr = static_cast<KPTNodeItem *>(m_listview->currentItem());
        kdDebug()<<k_funcinfo<<" m_listview="<<m_listview<<" item="<<m_listview->currentItem()<<endl;
        
        // find last
        QListViewItem *last = curr->lastChild();
        KPTNodeItem *newItem;
        if (last) {
            newItem = new KPTNodeItem(curr, last, (KPTNode *)proj);
            kdDebug(42000) << "Added item '" << newItem->text(0) << "' after '"<< last->text(0)<< "' to parent '" << last->parent()->text(0) << "'"<< endl;
        } else {
            newItem = new KPTNodeItem(curr, (KPTNode *)proj);
        }
        newItem->setOpen(true);
        
        KPTNode &currNode = curr->getNode(); 
        currNode.addChildNode(proj);
        
        int n = currNode.numChildren();
        if (n > 1) {
            KPTNode &prev = currNode.getChildNode(n-2);
            kdDebug(42000) << "Added node[" << n-1 << "] '" << proj->name() <<  "' after '" << prev.name() << "' to '" << currNode.name().latin1() << "'" << endl;
        } else
            kdDebug(42000) << "Added node[" << n-1 << "] '" << proj->name() << "' to '" << currNode.name().latin1() << "'" << endl;
        
    } else
        delete proj;
}


void KPTView::slotAddTask() {
    KPTTask *task = new KPTTask();
    if (task->openDialog()) {
        KPTNodeItem *curr = static_cast<KPTNodeItem *>(m_listview->currentItem());
        kdDebug()<<k_funcinfo<<" m_listview="<<m_listview<<" item="<<m_listview->currentItem()<<endl;
        
        // find last
        QListViewItem *last = curr->lastChild();
        KPTNodeItem *newItem;
        if (last) {
            newItem = new KPTNodeItem(curr, last, (KPTNode *)task);
            kdDebug(42000) << "Added '" << newItem->text(0) << "' after '"<< last->text(0)<< "' to parent '" << last->parent()->text(0) << "'"<< endl;
        } else {
            newItem = new KPTNodeItem(curr, (KPTNode *)task);
        }
        newItem->setOpen(true);
        
        KPTNode &currNode = curr->getNode(); 
        kdDebug(42000) << "Adding '" << task->name() << "' to '" << currNode.name().latin1() << "'"<< endl;
        currNode.addChildNode(task);        
    } else
        delete task;
}

void KPTView::slotAddMilestone() {
    KPTMilestone *ms = new KPTMilestone();
    ms->setName(i18n("Milestone"));

    if (ms->openDialog()) {
        KPTNodeItem *curr = static_cast<KPTNodeItem *>(m_listview->currentItem());
        kdDebug()<<k_funcinfo<<" m_listview="<<m_listview<<" item="<<m_listview->currentItem()<<endl;
        
        // find last
        QListViewItem *last = curr->lastChild();
        KPTNodeItem *newItem;
        if (last) {
            newItem = new KPTNodeItem(curr, last, (KPTNode *)ms);
            kdDebug(42000) << "Added '" << newItem->text(0) << "' after '"<< last->text(0)<< "' to parent '" << last->parent()->text(0) << "'"<< endl;
        } else {
            newItem = new KPTNodeItem(curr, (KPTNode *)ms);
        }
        newItem->setOpen(true);
        
        KPTNode &currNode = curr->getNode(); 
        kdDebug(42000) << "Adding '" << ms->name() << "' to '" << currNode.name().latin1() << "'"<< endl;
        currNode.addChildNode(ms);        
    } else
        delete ms;
}

void KPTView::slotEditResource() {
    ((KPTPart *)koDocument())->openResourceDialog();
}

 void KPTView::slotConfigure() {
 
}

void KPTView::slotKoolBar( int _grp, int _item ) {
  kdDebug() <<k_funcinfo<< _grp << " " << _item << endl;
  if ( _grp == m_viewGrp )
  {
  }


}

void KPTView::slotSelectionChanged() {
    // TODO: Set available menu items according to the type of selected item
    KPTNodeItem *item = static_cast<KPTNodeItem *>(m_listview->currentItem());
    //kdDebug()<<k_funcinfo<<"Current item="<<item->getNode().name()<<endl;
}

void KPTView::slotOpen(QListViewItem *item) {
    if (item)
    {
        static_cast<KPTNodeItem *>(item)->openDialog();
    }
}

void KPTView::updateReadWrite(bool /*readwrite*/) {
}


#ifndef NDEBUG
void KPTView::slotPrintDebug() {
    KPTNodeItem *curr = static_cast<KPTNodeItem *>(m_listview->currentItem());
    if (curr) {
        kdDebug()<<"-------- Debug printout: Node list" <<endl;
        curr->getNode().printDebug(true,"");
    } else
        kdDebug()<<"-------- Debug printout: No current node"<<endl;
}
#endif
#include "kptview.moc"
