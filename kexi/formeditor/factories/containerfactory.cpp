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

#include <kiconloader.h>
#include <kgenericfactory.h>
#include <kdebug.h>

#include "containerfactory.h"

ContainerFactory::ContainerFactory(QObject *parent, const char *name, const QStringList &)
 : KFormDesigner::WidgetFactory(parent, name)
{
	KFormDesigner::Widget *wBtnGroup = new KFormDesigner::Widget(this);
	wBtnGroup->setPixmap(SmallIcon("frame"));
	wBtnGroup->setClassName("QButtonGroup");
	wBtnGroup->setName("Button Group");
	m_classes.append(wBtnGroup);

	KFormDesigner::Widget *wTabWidget = new KFormDesigner::Widget(this);
	wTabWidget->setPixmap(SmallIcon("tabwidget"));
	wTabWidget->setClassName("QTabWidget");
	wTabWidget->setName("Tab Widget");
	m_classes.append(wTabWidget);
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
		kdDebug() << "ContainerFactory::create(): container=" << container << endl;
		if(container)
			new KFormDesigner::Container(container->toplevel(), w);
		return w;
	}
	else if(c == "QTabWidget")
	{
		QTabWidget *tab = new QTabWidget(p, n);
		QWidget *p1 = new QWidget(tab);
		new KFormDesigner::Container(container->toplevel(), p1);
		tab->addTab(p1, "Page 1");

		return tab;
	}

	return 0;
}

ContainerFactory::~ContainerFactory()
{
}

K_EXPORT_COMPONENT_FACTORY(containers, KGenericFactory<ContainerFactory>)

#include "containerfactory.moc"
