/* This file is part of the KDE project
Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

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

#include <qpixmap.h>

#include <kdebug.h>
#include <klocale.h>
#include <kaction.h>
#include <klineeditdlg.h>

#include "kexiscripthandlerproxy.h"
#include "kexiscripthandler.h"
#include "kexiprojecthandleritem.h"
#include "kexiqsahost.h"

KexiScriptHandlerProxy::KexiScriptHandlerProxy(KexiScriptHandler *handler, KexiView *view)
 : KexiProjectHandlerProxy(handler, view), KXMLGUIClient()
{
	view->insertChildClient(this);

	m_scriptHandler = handler;
	m_host = handler->host();
}

KexiPartPopupMenu *
KexiScriptHandlerProxy::groupContext()
{
	KexiPartPopupMenu *m = new KexiPartPopupMenu(this);
	m->insertAction(i18n("Create Sourcefile..."), SLOT(slotCreate()));
	return m;
}

KexiPartPopupMenu *
KexiScriptHandlerProxy::itemContext(const QString &identifier)
{
	KexiPartPopupMenu *m = new KexiPartPopupMenu(this);
	m->insertAction(i18n("Open Sourcefile"), SLOT(slotOpen(const QString &)));
	return m;
}

void
KexiScriptHandlerProxy::slotCreate()
{
	bool ok;
	QString name = KLineEditDlg::getText(i18n("New Code"), i18n("Sourcefile name:"), "", &ok, kexiView());
	if(ok)
	{
		KexiProjectHandlerItem *i = new KexiProjectHandlerItem(part(), name + ".qs", "kexi/script", "kexi/form" + name);
		part()->items()->insert(name, i);
		emit m_scriptHandler->itemListChanged(part());
		m_host->createFile(kexiView(), name);
	}
}

void
KexiScriptHandlerProxy::slotOpen(const QString &identifier)
{
	kdDebug() << "KexiScriptHandlerProxy::slotOpen(): id: " << identifier << endl;

	KexiProjectHandlerItem *i = part()->items()->find(identifier);
	kdDebug() << "KexiScriptHandlerProxy::slotOpen(): item: " << i << endl;

	if(i)
		m_host->openFile(kexiView(), i->name());
}

void
KexiScriptHandlerProxy::executeItem(const QString &identifier)
{
	slotOpen(identifier);
}


#include "kexiscripthandlerproxy.moc"
