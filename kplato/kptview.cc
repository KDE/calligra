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
#include "kptnodeitem.h"
#include "kpttask.h"
#include "kpttaskdialog.h"

#include <qpainter.h>
#include <qiconset.h>
#include <qlayout.h>
#include <kaction.h>
#include <kstdaction.h>
#include <klocale.h>
#include <kdebug.h>
#include <klistview.h>


KPTView::KPTView(KPTPart* part, QWidget* parent, const char* name)
    : KoView(part, parent, name)
{
    setInstance(KPTFactory::global());
    setXMLFile("kplato.rc");

    QBoxLayout *l = new QHBoxLayout(this);
    l->setAutoAdd(true);

    // The main project view
    m_listview = new KListView(this);
    m_listview->setSelectionModeExt(KListView::Extended);
    m_listview->addColumn(i18n("Project"));
    m_listview->addColumn(i18n("Leader"));
    m_listview->addColumn(i18n("Description"));

    connect(m_listview, SIGNAL(selectionChanged()), this,
	    SLOT(slotSelectionChanged()));

    // The menu items
    new KAction(i18n("Edit Main Project..."), "edit_project", 0, this,
		SLOT(slotEditProject()), actionCollection(), "edit_project");
    new KAction(i18n("Add Sub-Project..."), "add_sub_project", 0, this,
		SLOT(slotAddSubProject()), actionCollection(),
		"add_sub_project");
    new KAction(i18n("Add Task..."), "add_task", 0, this,
		SLOT(slotAddTask()), actionCollection(), "add_task");
    new KAction(i18n("Add Milestone..."), "add_milestone", 0, this,
		SLOT(slotAddMilestone()), actionCollection(), "add_milestone");

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
	KPTNodeItem *i = new KPTNodeItem(item, n);
	i->setOpen(true);

	// Now add all it's children
	displayChildren(n, i);
    }
}


void KPTView::slotEditProject() {
    ((KPTPart *)koDocument())->editProject();
    displayProject();
}


void KPTView::slotAddSubProject() {
}


void KPTView::slotAddTask() {
    KPTTask *task = new KPTTask();
    KPTTaskDialog *dialog = new KPTTaskDialog(*task, this);

    // Execute the dialog
    if (dialog->exec()) {
	KPTNode &node = ((KPTNodeItem *)m_listview->selectedItem())->getNode();
	cerr << "Adding child to " << node.name().latin1() << endl;
	node.addChildNode(task);
	
	displayProject();
    } else
	delete task;

    delete dialog;
}


void KPTView::slotAddMilestone() {
    KPTTask *task = 0;
    task->addChildNode(0);
}


void KPTView::slotSelectionChanged() {
    // TODO: Set available menu items according to the type of selected item
}


void KPTView::updateReadWrite(bool /*readwrite*/) {
}


#include "kptview.moc"
