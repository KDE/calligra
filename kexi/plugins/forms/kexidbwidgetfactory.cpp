/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kdebug.h>

#include "kexidbwidgetcontainer.h"
#include "kexidbwidgetfactory.h"

KexiDBWidgetFactory::KexiDBWidgetFactory(QObject *parent, const char *name)
 : KFormDesigner::WidgetFactory(parent, name)
{
	KFormDesigner::Widget *wContainer = new KFormDesigner::Widget(this);
//	wTabWidget->setPixmap(SmallIcon("container"));
	wContainer->setClassName("KexiDBWidgetContainer");
	wContainer->setName("DB Container");
	m_classes.append(wContainer);
}

QString
KexiDBWidgetFactory::name()
{
	return("DBWidgets");
}

KFormDesigner::WidgetList
KexiDBWidgetFactory::classes()
{
	return m_classes;
}

QWidget*
KexiDBWidgetFactory::create(const QString &c, QWidget *p, const char *n, KFormDesigner::Container *container)
{
	if(c == "KexiDBWidgetContainer")
	{
		return new KexiDBWidgetContainer(p, n);
	}

	return 0;
}


KexiDBWidgetFactory::~KexiDBWidgetFactory()
{
}

#include "kexidbwidgetfactory.moc"
