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

#include "widgetlibrary.h"
#include "container.h"
#include "form.h"

#include "kfmview.h"

KFMView::KFMView()
 : KMainWindow()
{
	KFormDesigner::WidgetLibrary *l = new KFormDesigner::WidgetLibrary();
//	l->createActions(actionCollection());
	createGUI("kfmui.rc", true);
	setXMLFile("kfmui.rc", true);
//	plugActionList("library_widgets", l->createActions(actionCollection()));


	//setXML(l->createXML(), true);

//	l->createActions(actionCollection(), toolBar("widgets"));

	QWorkspace *w = new QWorkspace(this);
	setCentralWidget(w);
	w->show();

	KFormDesigner::Form *form = new KFormDesigner::Form(this, "", l);
	QWidget *formV = new QWidget(w, "form1");
	form->createToplevel(formV);
	formV->setCaption("Form1");
	formV->show();
	formV->resize(350, 300);

	QTabWidget *tab = new QTabWidget(formV, "tabwidget1");
	QWidget *tabcontainer = new QWidget(tab, "tabc1");
	new KFormDesigner::Container(form->toplevelContainer(), tabcontainer);
	tab->addTab(tabcontainer, "Page 1");
	tab->show();
	form->toplevelContainer()->addWidget(tab, QRect(20, 20, 130, 140));
	QWidget *tabcontainer2 = new QWidget(tab, "tabc");
	new KFormDesigner::Container(form->toplevelContainer(), tabcontainer2);
	tab->addTab(tabcontainer2, "Page 2");

//	new KFormDesigner::ContainerFactory(0, form);

}

void
KFMView::slotWidget()
{
	kdDebug() << "KFMView::slotWidget()" << endl;
}

KFMView::~KFMView()
{
}

#include "kfmview.moc"
