/*  This file is part of the KDE project
    Copyright (C) 2002,2003 Joseph Wenninger <jowenn@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Library General Public License version 2 as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include <qpixmap.h>

#include <koApplication.h>

#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <klineeditdlg.h>
#include <kmessagebox.h>
#include <kaction.h>

#include "kexikugarhandler.h"
#include "kexikugarwrapper.h"
#include "kexikugarhandlerproxy.h"
#include "kexikugarlicense.h"
#include "kexiview.h"

KexiKugarHandlerProxy::KexiKugarHandlerProxy(KexiKugarHandler *part,KexiView *view)
 : KexiProjectHandlerProxy(part,view)
 ,KXMLGUIClient()
 ,m_kugarManager(part)
{
	kdDebug() << "KexiKugarHandlerProxy::KexiKugarHandlerProxy()" << endl;

	m_createAction = new KAction(i18n("Create &Report..."), "report", "",
		this,SLOT(slotCreateReport()), actionCollection(), "kugarpart_create");

	m_openAction = new KexiPartItemAction(i18n("Open Report"), "", "",
		this,SLOT(executeItem(const QString &)), actionCollection(), "kugarpart_open");
	m_editAction = new KexiPartItemAction(i18n("Edit Report"), "edit", "",
		this,SLOT(slotEdit(const QString &)), actionCollection(), "kugarpart_edit");
	m_deleteAction = new KexiPartItemAction(i18n("Delete Report..."), "button_cancel", "",
		this,SLOT(slotDelete(const QString &)), actionCollection(), "kugarpart_delete");

	// actions in group menu
	m_createAction->plug(m_group_pmenu);

	// actions in item menu
	m_openAction->plug(m_item_pmenu);
	m_editAction->plug(m_item_pmenu);
	m_deleteAction->plug(m_item_pmenu);
	m_item_pmenu->insertSeparator();
	m_createAction->plug(m_item_pmenu);
	
	//other actions
	m_createAction = new KAction(i18n("Report Generator Licensing..."), "info", "",
		this,SLOT(slotGeneratorLicense()), actionCollection(), "kugar_show_license");

    setXMLFile("kexikugarhandlerui.rc");

    view->insertChildClient(this);
}

/*
KexiPartPopupMenu*
KexiKugarHandlerProxy::groupContext()
{
	KexiPartPopupMenu *m = new KexiPartPopupMenu(this);
	m->insertAction(i18n("Create Report..."), SLOT(slotCreateReport()));
	m->insertAction(i18n("Report Generator Licensing..."),SLOT(slotGeneratorLicense()));
	return m;
}

KexiPartPopupMenu*
KexiKugarHandlerProxy::itemContext(const QString& identifier)
{
	KexiPartPopupMenu *m = new KexiPartPopupMenu(this);
	m->insertAction(i18n("Open Report"), SLOT(executeItem(const QString&)));
	m->insertAction(i18n("Edit Report"), SLOT(slotEdit(const QString&)));
	m->insertAction(i18n("Delete Report..."), SLOT(slotDelete(const QString&)));
	m->insertAction(i18n("Create Report..."), SLOT(slotCreateReport()));
	m->insertAction(i18n("Report Generator Licensing..."),SLOT(slotGeneratorLicense()));
	return m;
}
*/

void KexiKugarHandlerProxy::slotGeneratorLicense()
{
	KexiKugarLicense lic(0,0,true);
	lic.exec();
}

void
KexiKugarHandlerProxy::executeItem(const QString& identifier)
{
	m_kugarManager->view(kexiView(),identifier);
}

void
KexiKugarHandlerProxy::slotEdit(const QString& identifier)
{
	m_kugarManager->editReport(kexiView(),identifier);
}

void
KexiKugarHandlerProxy::slotDelete(const QString& identifier)
{
}

void KexiKugarHandlerProxy::slotCreateReport()
{
	m_kugarManager->createReport(kexiView());
}

#include "kexikugarhandlerproxy.moc"
