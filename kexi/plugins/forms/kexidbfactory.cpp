/***************************************************************************
 *   Copyright (C) 2003 by Lucijan Busch                                   *
 *   lucijan@kde.org                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 ***************************************************************************/

#include <qpopupmenu.h>

#include <kiconloader.h>
#include <kgenericfactory.h>
#include <klocale.h>
#include <kdebug.h>
#include <klineeditdlg.h>

#include "kexidbfactory.h"
//#include "kexidbform.h"
#include <container.h>
#include <form.h>
#include <objecttree.h>

KexiDBFactory::KexiDBFactory(QObject *parent, const char *name, const QStringList &)
 : KFormDesigner::WidgetFactory(parent, name)
{
	KFormDesigner::Widget *wView = new KFormDesigner::Widget(this);
	wView->setPixmap("frame");
	wView->setClassName("KexiDBForm");
	wView->setName(i18n("Database Form"));
	m_classes.append(wView);
}

QString
KexiDBFactory::name()
{
	return("containers");
}

KFormDesigner::WidgetList
KexiDBFactory::classes()
{
	return m_classes;
}

QWidget*
KexiDBFactory::create(const QString &c, QWidget *p, const char *n, KFormDesigner::Container *container)
{
	kdDebug() << "KexiDBFactory::create() " << this << endl;

	if(c == "KexiDBForm")
	{
		QWidget *w = new QWidget(p, n);
		new KFormDesigner::Container(container, w, container);
		return w;
	}

	return 0;
}

void
KexiDBFactory::createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container)
{
	m_widget = w;
	m_container = container;

	if(classname == "KexiDBForm")
	{
		menu->insertItem(i18n("Change Datasource"), this, SLOT(chDataSource()) );
		return;
	}

	return;
}

void
KexiDBFactory::startEditing(const QString &classname, QWidget *w, KFormDesigner::Container *container)
{
	m_container = container;
	return;
}

void KexiDBFactory::chDataSource()
{
/*
	if(!m_widget->isA("QButtonGroup"))  { return; }
	QButtonGroup *btngrp = (QButtonGroup*)m_widget;

	bool ok;
	QString text = KLineEditDlg::getText(i18n("New Title"), i18n("Enter a new title for the button group"),
		btngrp->title(), &ok);
	if(ok)
		btngrp->setTitle(text);
*/
}


KexiDBFactory::~KexiDBFactory()
{
}

K_EXPORT_COMPONENT_FACTORY(kexidbwidgets, KGenericFactory<KexiDBFactory>)

#include "kexidbfactory.moc"
