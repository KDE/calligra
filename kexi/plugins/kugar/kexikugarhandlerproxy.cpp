/*  This file is part of the KDE project
    Copyright (C) 2002,2003 Joseph Wenninger <jowenn@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
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
 : KexiProjectHandlerProxy(part,view),KXMLGUIClient()
{
	m_kugarManager=part;
	kdDebug() << "KexiKugarHandlerProxy::KexiKugarHandlerProxy()" << endl;

    (void*) new KAction(i18n("Create &Report ..."), 0,
                        this,SLOT(slotCreateReport()), actionCollection(), "kugarhandler_create");

    setXMLFile("kexikugarhandlerui.rc");

    view->insertChildClient(this);
}


KexiPartPopupMenu*
KexiKugarHandlerProxy::groupContext()
{
	KexiPartPopupMenu *m = new KexiPartPopupMenu(this);
	m->insertAction(i18n("Create Report..."), SLOT(slotCreateReport()));
	m->insertAction(i18n("Report Generator Licensing"),SLOT(slotGeneratorLicense()));
	return m;
}

KexiPartPopupMenu*
KexiKugarHandlerProxy::itemContext(const QString& identifier)
{
	KexiPartPopupMenu *m = new KexiPartPopupMenu(this);
	m->insertAction(i18n("View Report"), SLOT(executeItem(const QString&)));
	m->insertAction(i18n("Edit Report"), SLOT(slotEdit(const QString&)));
	m->insertAction(i18n("Delete Report"), SLOT(slotDelete(const QString&)));
	m->insertAction(i18n("Create New Report"), SLOT(slotCreateReport()));
	m->insertAction(i18n("Report Generator Licensing"),SLOT(slotGeneratorLicense()));
	return m;
}


void KexiKugarHandlerProxy::slotGeneratorLicense()
{
	KexiKugarLicense *lic=new KexiKugarLicense(0,0,true);
	lic->exec();
	delete lic;
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
