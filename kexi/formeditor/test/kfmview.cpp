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

#include <kaction.h>
#include <klocale.h>
#include <kdebug.h>

#include "objecttree.h"
#include "widgetlibrary.h"
#include "container.h"
#include "form.h"
#include "kexipropertyeditor.h"
#include "objpropbuffer.h"

#include "kfmview.h"

KFMView::KFMView()
 : KMainWindow()
{
	KFormDesigner::WidgetLibrary *l = new KFormDesigner::WidgetLibrary();
//	l->createActions(actionCollection());
//	plugActionList("library_widgets", l->createActions(actionCollection()));


	//setXML(l->createXML(), true);

//	l->createActions(actionCollection(), toolBar("widgets"));

	QWorkspace *w = new QWorkspace(this);
	setCentralWidget(w);
	w->show();

	KFormDesigner::ObjectPropertyBuffer *buff = new KFormDesigner::ObjectPropertyBuffer(this, 0);
	KexiPropertyEditor *editor = new KexiPropertyEditor(0, true, 0);
	buff->setList(editor);

	m_form = new KFormDesigner::Form(this, "", l, buff);
	QWidget *formV = new QWidget(w, "forms1");
	m_form->createToplevel(formV);
	formV->setCaption("Form1");
	formV->show();
	formV->resize(350, 300);
	
	buff->setObject(formV);
	editor->setBuffer(buff);
	editor->show();

	new KAction(i18n("Print object tree"), "view_tree", KShortcut(0), this, SLOT(debugTree()), actionCollection(), "dtree");

	m_form->createActions(actionCollection());
	createGUI("kfmui.rc", true);
	setXMLFile("kfmui.rc", true);

	
//	l->createActions(actionCollection(), toolBar("widgets"));
//	new KFormDesigner::ContainerFactory(0, form);

}

void
KFMView::slotWidget()
{
	kdDebug() << "KFMView::slotWidget()" << endl;
}

void
KFMView::debugTree()
{
	m_form->objectTree()->debug();
}

KFMView::~KFMView()
{
}

#include "kfmview.moc"
