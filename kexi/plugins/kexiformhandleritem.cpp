/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

#include <koStore.h>

#include "kexiformhandler.h"
#include "kexiformhandleritem.h"
#include "kexiprojecthandler.h"
#include "kexidbwidgetcontainer.h"
#include "kexiwidgetprovider.h"
#include "formeditor/widgetwatcher.h"


KexiFormHandlerItem::KexiFormHandlerItem(KexiProjectHandler *parent, const QString &name, const QString &identifier)
 : KexiProjectHandlerItem(parent, name, "kexi/form", identifier)
{
	m_propertyBuffer = new PropertyBuffer(this, "pb");
//	m_widgetList = new WidgetList();
	m_widgetWatcher = new KFormEditor::WidgetWatcher(this, m_propertyBuffer);

	m_container = 0;
}

void
KexiFormHandlerItem::store(KoStore *store)
{
	if(m_container)
	{
		QByteArray data = m_widgetWatcher->store(m_container);

		store->open("/form/" + name() + ".ui");
		store->write(data);
		store->close();
	}
}

void
KexiFormHandlerItem::load(KoStore *store)
{
	store->open("/form/" + name() + ".ui");
	QByteArray data = store->read(store->size());
	store->close();
	m_container = new KexiDBWidgetContainer(0, "foo", "bar");
	KexiWidgetProvider *provider = new KexiWidgetProvider();
	m_widgetWatcher->load(m_container, provider, data);
}

KexiFormHandlerItem::~KexiFormHandlerItem()
{
}

#include "kexiformhandleritem.moc"
