/***************************************************************************
 *   Copyright (C) 2003 by Lucijan Busch                                   *
 *   lucijan@kde.org                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 ***************************************************************************/

#include <qbuttongroup.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qstring.h>
#include <qpopupmenu.h>

#include <kiconloader.h>
#include <kgenericfactory.h>
#include <klocale.h>
#include <kdebug.h>
#include <klineeditdlg.h>

#include "containerfactory.h"
#include "container.h"
#include "form.h"
#include "objecttree.h"

ContainerFactory::ContainerFactory(QObject *parent, const char *name, const QStringList &)
 : KFormDesigner::WidgetFactory(parent, name)
{
	KFormDesigner::Widget *wBtnGroup = new KFormDesigner::Widget(this);
	wBtnGroup->setPixmap("frame");
	wBtnGroup->setClassName("QButtonGroup");
	wBtnGroup->setName(i18n("Button Group"));
	wBtnGroup->setDescription(i18n("A simple container with a title"));
	m_classes.append(wBtnGroup);

	KFormDesigner::Widget *wTabWidget = new KFormDesigner::Widget(this);
	wTabWidget->setPixmap("tabwidget");
	wTabWidget->setClassName("QTabWidget");
	wTabWidget->setName(i18n("Tab Widget"));
	wTabWidget->setDescription(i18n("A widget to display multiple pages using tabs"));
	m_classes.append(wTabWidget);

	KFormDesigner::Widget *wWidget = new KFormDesigner::Widget(this);
	wWidget->setPixmap("widget");
	wWidget->setClassName("QWidget");
	wWidget->setName(i18n("Empty Widget"));
	wWidget->setDescription(i18n("An empty container with no frame"));
	m_classes.append(wWidget);
}

QString
ContainerFactory::name()
{
	return("containers");
}

KFormDesigner::WidgetList
ContainerFactory::classes()
{
	return m_classes;
}

QWidget*
ContainerFactory::create(const QString &c, QWidget *p, const char *n, KFormDesigner::Container *container)
{
	kdDebug() << "ContainerFactory::create() " << this << endl;

	if(c == "QButtonGroup")
	{
		QButtonGroup *w = new QButtonGroup("btn", p, n);
		new KFormDesigner::Container(container, w, container);
		return w;
	}
	else if(c == "QTabWidget")
	{
		QTabWidget *tab = new QTabWidget(p, n);
		container->form()->objectTree()->addChild(container->tree(), new KFormDesigner::ObjectTreeItem(c, n, tab));
		tab->installEventFilter(container);

		if(container->form()->interactiveMode())
		{
			m_widget=tab;
			m_container=container;
			AddTabPage();
		}

		return tab;
	}
	else if(c == "QWidget")
	{
		QWidget *w = new QWidget(p, n);
		new KFormDesigner::Container(container, w, container);
		return w;
	}

	return 0;
}

void
ContainerFactory::createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container)
{
	m_widget = w;
	m_container = container;

	if(classname == "QButtonGroup")
	{
		menu->insertItem(i18n("Change title"), this, SLOT(chTitle()) );
		return;
	}

	else if(classname == "QTabWidget")
	{
		menu->insertItem(i18n("Add Page"), this, SLOT(AddTabPage()) );
		return;
	}
	else if(classname == "QWidget")
	{
	}

	return;
}

void
ContainerFactory::startEditing(const QString &classname, QWidget *w, KFormDesigner::Container *container)
{
	m_container = container;
	if(classname == "QButtonGroup")
	{
		QButtonGroup *group = static_cast<QButtonGroup*>(w);
		QRect r = QRect(group->x()+2, group->y()-5, group->width()-10, 20);
		createEditor(group->title(), group, r, Qt::AlignAuto);
		return;
	}
	return;
}

void
ContainerFactory::changeText(const QString &text)
{
	changeProperty("title", text, m_container);
}

void ContainerFactory::AddTabPage()
{
	if (!m_widget->isA("QTabWidget")){ return ;}
	QString type = "QWidget";
	QString name = (m_container->form()->objectTree()->genName(type));

	QTabWidget *tab = (QTabWidget *)m_widget;
	QWidget *page = new QWidget(tab,name.latin1());
	new KFormDesigner::Container(m_container,page,tab);

	QString n;
	n.setNum(tab->count()+1);
	n.prepend("Page ");
	tab->addTab(page,n);
	tab->showPage(page);

	KFormDesigner::ObjectTreeItem *item = m_container->form()->objectTree()->lookup(name);
	item->addModProperty("title", n);
}

void ContainerFactory::chTitle()
{
	if(!m_widget->isA("QButtonGroup"))  { return; }
	QButtonGroup *btngrp = (QButtonGroup*)m_widget;

	bool ok;
	QString text = KLineEditDlg::getText(i18n("New Title"), i18n("Enter a new title for the button group"),
		btngrp->title(), &ok);
	if(ok)
		btngrp->setTitle(text);
}


ContainerFactory::~ContainerFactory()
{
}

K_EXPORT_COMPONENT_FACTORY(containers, KGenericFactory<ContainerFactory>)

#include "containerfactory.moc"
