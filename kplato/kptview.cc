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
#include "kpttask.h"
#include "kptmilestone.h"
#include "kptganttview.h"
#include "kptpertview.h"

#include "kptcanvasitem.h"

#include "KDGanttView.h"
#include "KDGanttViewTaskItem.h"

#include <koKoolBar.h>
#include <koRect.h>

#include <qpainter.h>
#include <qiconset.h>
#include <qlayout.h>
#include <qsplitter.h>
#include <qcanvas.h>
#include <qscrollview.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qstring.h>
#include <qvbox.h>
#include <qgrid.h>
#include <qsize.h>
#include <qheader.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qwidgetstack.h>
#include <qtimer.h>

#include <kiconloader.h>
#include <kaction.h>
#include <kstdaction.h>
#include <klocale.h>
#include <kdebug.h>
#include <klistview.h>
#include <kstdaccel.h>
#include <kaccelgen.h>
#include <kdeversion.h>
#include <kxmlguifactory.h>

KPTView::KPTView(KPTPart* part, QWidget* parent, const char* /*name*/)
    : KoView(part, parent, "Main View"),
    m_ganttview(0),
    m_ganttlayout(0),
    m_pertview(0),
    m_pertlayout(0)
{
    kdDebug()<<k_funcinfo<<endl;
    setInstance(KPTFactory::global());
    setXMLFile("kplato.rc");

	m_tab = new QWidgetStack(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
	layout->add(m_tab);

	m_ganttview = new KPTGanttView( this, m_tab);
	m_tab->addWidget(m_ganttview);

	m_pertview = new KPTPertView( this, m_tab, layout );
    m_tab->addWidget(m_pertview);

	// HACK: Avoid crash
	// If gantt view is shown first, we have a crash...
	m_tab->raiseWidget(m_pertview);

    connect(m_tab, SIGNAL(aboutToShow(QWidget *)), this, SLOT(slotChanged(QWidget *)));

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

    // ------ Project
    new KAction(i18n("Edit Main Project..."), "project_edit", 0, this,
		SLOT(slotProjectEdit()), actionCollection(), "project_edit");
    new KAction(i18n("Calculate..."), "project_calculate", 0, this,
		SLOT(slotProjectCalculate()), actionCollection(), "project_calculate");

    // ------ Tools
    //new KAction(i18n("Resource Editor..."), "edit_resource", 0, this,
	//	SLOT(slotEditResource()), actionCollection(), "edit_resource");

    // ------ Settings
    new KAction(i18n("Configure..."), "configure", 0, this,
		SLOT(slotConfigure()), actionCollection(), "configure");

    // ------ Popup
    new KAction(i18n("Node Properties"), "node_properties", 0, this,
		SLOT(slotOpenNode()), actionCollection(), "node_properties");


    // ------------------- Actions with a key binding and no GUI item
#ifndef NDEBUG
    KAction* actPrintDebug = new KAction( i18n( "Print Debug" ), CTRL+SHIFT+Key_P,
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

}

void KPTView::setZoom(double zoom) {
    m_ganttview->zoom(zoom);
	m_pertview->zoom(zoom);
}


void KPTView::slotEditCut() {
    kdDebug()<<k_funcinfo<<endl;
}

void KPTView::slotEditCopy() {
    kdDebug()<<k_funcinfo<<endl;
}

void KPTView::slotEditPaste() {
    kdDebug()<<k_funcinfo<<endl;
}

void KPTView::slotViewGantt() {
    kdDebug()<<k_funcinfo<<endl;
	m_ganttview->clear();
    m_tab->raiseWidget(m_ganttview);
	m_ganttview->draw(getPart()->getProject());
}

void KPTView::slotViewPert() {
    kdDebug()<<k_funcinfo<<endl;
    m_tab->raiseWidget(m_pertview);
}

void KPTView::slotViewResources() {

}

void KPTView::slotProjectEdit() {
    if (getPart()->getProject().openDialog())
	    slotUpdate(true);
}

void KPTView::slotProjectCalculate() {
    getPart()->getProject().calculate();
    KPTDuration *t = getPart()->getProject().getStartTime();
    delete t;
}


void KPTView::slotAddSubProject() {
    KPTProject *node = new KPTProject(currentNode());
    if (node->openDialog()) {
		KPTNode *currNode = currentNode();
		if (currNode)
        {
			currNode->addChildNode(node);
    		slotUpdate(true);
			return;
	    }
		else
		    kdDebug()<<k_funcinfo<<"Cannot insert new project. Hmm, no current node!?"<<endl;
	}
    delete node;
}


void KPTView::slotAddTask() {
    KPTTask *node = new KPTTask(currentNode());
    if (node->openDialog()) {
		KPTNode *currNode = currentNode();
		if (currNode)
        {
			currNode->addChildNode(node);
    		slotUpdate(true);
			return;
	    }
		else
		    kdDebug()<<k_funcinfo<<"Cannot insert new task. Hmm, no current node!?"<<endl;
	}
    delete node;
}

void KPTView::slotAddMilestone() {
    KPTMilestone *node = new KPTMilestone(currentNode());
    node->setName(i18n("Milestone"));

    if (node->openDialog()) {
		KPTNode *currNode = currentNode();
		if (currNode)
        {
			currNode->addChildNode(node);
    		slotUpdate(true);
			return;
	    }
		else
		    kdDebug()<<k_funcinfo<<"Cannot insert new milestone. Hmm, no current node!?"<<endl;
	}
    delete node;
}

 void KPTView::slotConfigure() {

}

KPTNode *KPTView::currentNode()
{
    KPTNode *n = 0;
	if (m_tab->visibleWidget() == m_ganttview)
	    n = m_ganttview->currentNode();
	else if (m_tab->visibleWidget() == m_pertview)
	    n = m_pertview->currentNode();

	if (!n)
	    n = &(getPart()->getProject());
	return n;
}

void KPTView::slotOpenNode() {
    kdDebug()<<k_funcinfo<<endl;
	if (m_tab->visibleWidget() == m_ganttview)
	{
        KPTNode *node = m_ganttview->currentNode();
        if (node) {
		    if ( node->openDialog() ) {
				slotUpdate(true);
			}
		}
		return;
	}
	if (m_tab->visibleWidget() == m_pertview)
	{
	    if ( m_pertview->currentNode()->openDialog() ) {
			slotUpdate(true);
		}
		return;
	}
}

void KPTView::updateReadWrite(bool /*readwrite*/) {
}

KPTPart *KPTView::getPart()const {
    return (KPTPart *)koDocument();
}

void KPTView::slotConnectNode() {
    kdDebug()<<k_funcinfo<<endl;
/*    KPTNodeItem *curr = m_ganttview->currentItem();
    if (curr) {
        kdDebug()<<k_funcinfo<<"node="<<curr->getNode().name()<<endl;
    }*/
}

QPopupMenu * KPTView::popupMenu( const QString& name )
{
    kdDebug()<<k_funcinfo<<endl;
    Q_ASSERT(factory());
    if ( factory() )
        return ((QPopupMenu*)factory()->container( name, this ));
    return 0L;
}

void KPTView::slotChanged(QWidget *)
{
    kdDebug()<<k_funcinfo<<endl;
    slotUpdate(false);
}

void KPTView::slotChanged()
{
    kdDebug()<<k_funcinfo<<endl;
    slotUpdate(false);
}

void KPTView::slotUpdate(bool calculate)
{
    kdDebug()<<k_funcinfo<<"calculate="<<calculate<<endl;
    if (calculate)
	    slotProjectCalculate();
	if (m_tab->visibleWidget() == m_ganttview)
	{
	    //m_ganttview->hide();
    	m_ganttview->draw(getPart()->getProject());
	    m_ganttview->show();
	}
	else if (m_tab->visibleWidget() == m_pertview)
	{
    	m_pertview->draw();
	    m_pertview->show();
	}
}

#ifndef NDEBUG
void KPTView::slotPrintDebug() {
    kdDebug()<<"-------- Debug printout: Node list" <<endl;
    KPTNode *curr = m_ganttview->currentNode();
    if (curr) {
        curr->printDebug(true,"");
    } else
        getPart()->getProject().printDebug(true, "");
}
#endif
#include "kptview.moc"
