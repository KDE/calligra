/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>

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

#include <qlabel.h>
#include <qlineedit.h>
#include <qpopupmenu.h>

#include <kiconloader.h>
#include <kgenericfactory.h>
#include <kdebug.h>

#include "spacer.h"
#include "kexiwidgetfactory.h"

KexiWidgetFactory::KexiWidgetFactory(QObject *parent, const char *name, const QStringList &)
 : KFormDesigner::WidgetFactory(parent, name)
{
	KFormDesigner::Widget *wLabel = new KFormDesigner::Widget(this);
	wBase->setPixmap("widget");
	wBase->setClassName("KexiViewBase");
	wBase->setName("Kexi DB Container");
//	wLabel->setDescription(i18n("A widget to display text or pixmaps"));
	m_classes.append(wBase);
}

QString
KexiWidgetFactory::name()
{
	return("kexiwidgets");
}

KFormDesigner::WidgetList
KexiWidgetFactory::classes()
{
	return m_classes;
}

QWidget*
KexiWidgetFactory::create(const QString &c, QWidget *p, const char *n, KFormDesigner::Container *container)
{
	if(c == "KexiViewBase")
	{
		kdDebug() << "KexiWidgetFactory(): parent: " << parent->className() << endl;
//		QWidget *w = new QLabel("Label", p, n);
//		w->installEventFilter(container);
//		return w;
	}

	return 0;
}


void
KexiWidgetFactory::createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container)
{
	return;
}

KexiWidgetFactory::~KexiWidgetFactory()
{
}

K_EXPORT_COMPONENT_FACTORY(kexiwidgets, KGenericFactory<KexiWidgetFactory>("kexiwidgets"))

#include "kexiwidgetfactory.moc"

