//
//
// C++ Implementation: cpp
//
// Description:
//
//
// Author: Lucijan Busch <lucijan@kde.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <qlineedit.h>
#include <qpushbutton.h>

#include <kiconloader.h>
#include <kgenericfactory.h>

#include "stdwidgets.h"
#include "../formdesigner/container.h"


StdWidgets::StdWidgets(QObject *parent, const char *name, const QStringList &)
{
	KFormDesigner::Widget *wLineEdit = new KFormDesigner::Widget(this);
	wLineEdit->setPixmap(SmallIcon("lineedit"));
	wLineEdit->setClassName("QLineEdit");
	wLineEdit->setName("Line Edit");
	m_classes.append(wLineEdit);

	KFormDesigner::Widget *wPushButton = new KFormDesigner::Widget(this);
	wPushButton->setPixmap(SmallIcon("button"));
	wPushButton->setClassName("QPushButton");
	wPushButton->setName("PushButton");
	m_classes.append(wPushButton);

}

QString
StdWidgets::name()
{
	return QString("StdWidgets");
}

QWidget*
StdWidgets::create(const QString &c, QWidget *parent, const char *name, KFormDesigner::Container *container)
{
	if(c == "QLineEdit")
	{
		QLineEdit *w = new QLineEdit(parent, name);
		w->installEventFilter(container);
		return w;
	}
	else if(c == "QPushButton")
	{
		QPushButton *w = new QPushButton("Push Button", parent, name);
		w->installEventFilter(container);
		return w;
	}

	return 0;
}

StdWidgets::~StdWidgets()
{
}

K_EXPORT_COMPONENT_FACTORY(libstdwidgets, KGenericFactory<StdWidgets>)

#include "stdwidgets.moc"
