/***************************************************************************
 *   Copyright (C) 2003 by Lucijan Busch                                   *
 *   lucijan@kde.org                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 ***************************************************************************/

#include <qlabel.h>
#include <klineedit.h>
#include <qpopupmenu.h>
#include <qcursor.h>

#include <kiconloader.h>
#include <kgenericfactory.h>
#include <klocale.h>
#include <kdebug.h>

#include "spacer.h"
#include "stdwidgetfactory.h"

StdWidgetFactory::StdWidgetFactory(QObject *parent, const char *name, const QStringList &)
 : KFormDesigner::WidgetFactory(parent, name)
{
	KFormDesigner::Widget *wLabel = new KFormDesigner::Widget(this);
	wLabel->setPixmap("label");
	wLabel->setClassName("QLabel");
	wLabel->setName(i18n("Text Label"));
//	wLabel->setDescription(i18n("A widget to display text or pixmaps"));
	m_classes.append(wLabel);

	KFormDesigner::Widget *wLineEdit = new KFormDesigner::Widget(this);
	wLineEdit->setPixmap("lineedit");
	wLineEdit->setClassName("QLineEdit");
	wLineEdit->setName(i18n("Line Edit"));
//	wLineEdit->setDescription(i18n("A widget to input text"));
	m_classes.append(wLineEdit);

	KFormDesigner::Widget *wSpacer = new KFormDesigner::Widget(this);
	wSpacer->setPixmap("kexi");
	wSpacer->setClassName("Spacer");
	wSpacer->setName(i18n("Spacer"));
//	wSpacer->setDescription(i18n("A widget to input text"));
	m_classes.append(wSpacer);
}

QString
StdWidgetFactory::name()
{
	return("stdwidgets");
}

KFormDesigner::WidgetList
StdWidgetFactory::classes()
{
	return m_classes;
}

QWidget*
StdWidgetFactory::create(const QString &c, QWidget *p, const char *n, KFormDesigner::Container *container)
{
	kdDebug() << "StdWidgetFactory::create() " << this << endl;

	if(c == "QLabel")
	{
		QWidget *w = new QLabel("Label", p, n);
		w->installEventFilter(container);
		return w;
	}
	else if(c == "QLineEdit")
	{
		QWidget *w = new QLineEdit(p, n);
		w->setCursor(QCursor(Qt::ArrowCursor));
		//((QLineEdit *)w)->setReadOnly(true);
		w->installEventFilter(container);
		return w;
	}
	else if(c == "Spacer")
	{
		QWidget *w = new KFormDesigner::Spacer(p, n);
		w->installEventFilter(container);
		return w;
	}

	return 0;
}


void
StdWidgetFactory::createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container)
{
	if(classname == "QLabel")
	{
		menu->insertItem(i18n("Change text"), this, SLOT(chText()) );
		return;
	}
	else if(classname == "QLineEdit")
	{
		menu->insertItem(i18n("Change text"), this, SLOT(chText()) );
		return;
	}

	return;
}

void
StdWidgetFactory::startEditing(const QString &classname, QWidget *w, KFormDesigner::Container *container)
{
	m_container = container;
	if(classname == "QLineEdit")
	{
		QLineEdit *lineedit = static_cast<QLineEdit*>(w);
		createEditor(lineedit->text(), lineedit, lineedit->geometry(), lineedit->alignment());
		return;
	}
	else if(classname == "QLabel")
	{
		m_widget = w;
		QLabel *label = static_cast<QLabel*>(w);
		createEditor(label->text(), label, label->geometry(), label->alignment());
		return;
	}
	return;
}

void
StdWidgetFactory::changeText(const QString &text)
{
	changeProperty("text", text, m_container);
}

bool
StdWidgetFactory::showProperty(const QString &classname, QWidget *w, const QString &property, bool multiple)
{
	if(classname == "Spacer")
	{
		return KFormDesigner::Spacer::showProperty(property);
	}
	return !multiple;
}

StdWidgetFactory::~StdWidgetFactory()
{
}

K_EXPORT_COMPONENT_FACTORY(stdwidgets, KGenericFactory<StdWidgetFactory>)

#include "stdwidgetfactory.moc"

