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
    listview = new KListView(this);
    listview->addColumn(i18n("Project"));
    KListViewItem *i = new KListViewItem(listview, i18n("item 1"));
    i->setOpen(true);
    new KListViewItem(i, i18n("item 2"));

    connect(listview, SIGNAL(selectionChanged(QListViewItem *)),
	    this, SLOT(slotSelectionChanged(QListViewItem *)));

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
}


void KPTView::slotEditProject() {
    ((KPTPart *)koDocument())->editProject();
}


void KPTView::slotAddSubProject() {
}


void KPTView::slotAddTask() {
}


void KPTView::slotAddMilestone() {
}


void KPTView::slotSelectionChanged(QListViewItem *item) {
    // TODO: Set available menu items according to the type of selected item
}


void KPTView::updateReadWrite(bool /*readwrite*/) {
}


#include "kptview.moc"
