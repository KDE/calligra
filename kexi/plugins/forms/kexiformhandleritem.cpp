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

#include <kdebug.h>
#include <koStore.h>

#include "formeditor/form.h"

#include "kexiview.h"
#include "kexiformhandler.h"
#include "kexiformhandleritem.h"
#include "kexidbwidgetcontainer.h"

KexiFormHandlerItem::KexiFormHandlerItem(KexiFormHandler *parent, const QString &name, const QString &identifier)
 : KexiProjectHandlerItem(parent, name, "kexi/form", identifier)
{
	kdDebug() << "KexiFormHandlerItem::KexiFormHandlerItem()" << endl;
	m_form = new KFormDesigner::Form(this, "form", parent->library());
	kdDebug() << "KexiFormHandlerItem::KexiFormHandlerItem() basic" << endl;
}

void
KexiFormHandlerItem::store(KoStore *store)
{
}

void
KexiFormHandlerItem::load(KoStore *store)
{
}

void
KexiFormHandlerItem::designForm(KexiView *v)
{
	kdDebug() << "KexiFormHandlerItem::designForm(this=" << this << ", KexiView *v)" << endl;
	QWidget *w = m_form->createEmptyInstance("KexiDBWidgetContainer", v);

	if(!w)
		return;

	kdDebug() << "KexiFormHandlerItem::designForm(): widget '" << w->name() << "' is up." << endl;
	KexiDBWidgetContainer *c = static_cast<KexiDBWidgetContainer *>(w);
	c->init(this, "a");
	w->setGeometry(0, 0, 255, 255);
	w->show();
}

KexiFormHandlerItem::~KexiFormHandlerItem()
{
}

#include "kexiformhandleritem.moc"
