/***************************************************************************
 *   Copyright (C) 2003 by Lucijan Busch                                   *
 *   lucijan@kde.org                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 ***************************************************************************/

#include <qworkspace.h>

#include <kdeversion.h>
#include <kaction.h>
#include <klocale.h>
#include <kdebug.h>
#include <kstdaction.h>

#include "formmanager.h"
#include "objecttreeview.h"
#include "kexipropertyeditor.h"
#include "kfmview.h"

KFMView::KFMView()
 : KMainWindow()
{
	w = new QWorkspace(this);
	setCentralWidget(w);
	w->show();

	KFormDesigner::FormManager *manager = new KFormDesigner::FormManager(w, this, "manager");
	KexiPropertyEditor *editor = new KexiPropertyEditor();
	editor->show();
	KFormDesigner::ObjectTreeView *view = new KFormDesigner::ObjectTreeView();
	view->show();
	manager->setEditors(editor, view);
	connect(w, SIGNAL(windowActivated(QWidget*)), manager, SLOT(windowChanged(QWidget*)));

	new KAction(i18n("Print object tree"), "view_tree", KShortcut(0), manager, SLOT(debugTree()), actionCollection(), "dtree");
	KStdAction::save(manager, SLOT(saveForm()), actionCollection());
	KStdAction::saveAs(manager, SLOT(saveFormAs()), actionCollection());
	KStdAction::open(manager, SLOT(loadForm()), actionCollection());
	KStdAction::openNew(manager, SLOT(createBlankForm()), actionCollection());
	KStdAction::cut(manager, SLOT(cutWidget()), actionCollection());
	KStdAction::copy(manager, SLOT(copyWidget()), actionCollection());
	KStdAction::paste(manager, SLOT(pasteWidget()), actionCollection());
#if KDE_IS_VERSION(3,1,9) 
	KStdAction::clear(manager, SLOT(deleteWidget()), actionCollection());
#else
	//TODO
#endif
	manager->createActions(actionCollection());

	createGUI("kfmui.rc", true);
	setXMLFile("kfmui.rc", true);
}

KFMView::~KFMView()
{
}

#include "kfmview.moc"
