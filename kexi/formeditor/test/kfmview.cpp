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
#include <qtabwidget.h>

#include <kaction.h>
#include <klocale.h>
#include <kdebug.h>

#include "objecttree.h"
#include "widgetlibrary.h"
#include "container.h"
#include "form.h"

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

	m_form = new KFormDesigner::Form(this, "", l);
	QWidget *formV = new QWidget(w, "forms1");
	m_form->createToplevel(formV);
	formV->setCaption("Form1");
	formV->show();
	formV->resize(350, 300);

/*	QTabWidget *tab = new QTabWidget(formV, "tabwidget1");
	KFormDesigner::ObjectTree *ttab = new KFormDesigner::ObjectTree(tab->className(), tab->name());
	m_form->objectTree()->addChild(ttab);

	QWidget *tabcontainer = new QWidget(tab, "tabc1");
	KFormDesigner::Container *pc1 = new KFormDesigner::Container(m_form->toplevelContainer(), tabcontainer);
	KFormDesigner::ObjectTree *tt1 = new KFormDesigner::ObjectTree(tabcontainer->className(), tabcontainer->name());
	pc1->setObjectTree(tt1);
	m_form->objectTree()->addChild(ttab, tt1);
	tab->addTab(tabcontainer, "Page 1");
	tab->show();
	m_form->toplevelContainer()->addWidget(tab, QRect(20, 20, 130, 140));
	QWidget *tabcontainer2 = new QWidget(tab, "tabc");
	KFormDesigner::Container *pc2 = new KFormDesigner::Container(m_form->toplevelContainer(), tabcontainer2);
	KFormDesigner::ObjectTree *tt2 = new KFormDesigner::ObjectTree(tabcontainer2->className(), tabcontainer2->name());
	pc2->setObjectTree(tt2);
	m_form->objectTree()->addChild(ttab, tt2);
	tab->addTab(tabcontainer2, "Page 2");
*/
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
