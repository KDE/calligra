/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002   Joseph Wenninger <jowenn@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qpixmap.h>

#include <koApplication.h>

#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <klineeditdlg.h>
#include <kmessagebox.h>
#include <kaction.h>

#include "kexiquerydesigner.h"
#include "kexiquerypart.h"
#include "kexiquerypartproxy.h"
#include "kexiquerypartitem.h"
#include "kexiview.h"
#include "kexi_global.h"
#include "kexipartitemaction.h"

KexiQueryPartProxy::KexiQueryPartProxy(KexiQueryPart *part,KexiView *view)
 : KexiProjectHandlerProxy(part,view)
 ,KXMLGUIClient()
 ,m_queryPart(part)
{
	setInstance(part->kexiProject()->instance());
	
	kdDebug() << "KexiQueryPartProxy::KexiQueryPartProxy()" << endl;

//	m_openAction = new KAction(i18n("Open Query"), "", "",
//		this,SLOT(slotOpen()), actionCollection(), "querypart_open");

	m_createAction = new KAction(i18n("Create &Query..."), "query", "",
		this,SLOT(slotCreateQuery()), actionCollection(), "querypart_create");

	m_openAction = new KexiPartItemAction(i18n("Open Query"), "", "",
		this,SLOT(slotOpen(const QString &)), actionCollection(), "querypart_open");
	m_editAction = new KexiPartItemAction(i18n("Edit Query"), "edit", "",
		this,SLOT(slotEdit(const QString &)), actionCollection(), "querypart_edit");
	m_deleteAction = new KexiPartItemAction(i18n("Delete Query..."), "button_cancel", "",
		this,SLOT(slotDelete(const QString &)), actionCollection(), "querypart_delete");

	// actions in group menu
	m_createAction->plug(m_group_pmenu);

	// actions in item menu
	m_openAction->plug(m_item_pmenu);
	m_editAction->plug(m_item_pmenu);
	m_deleteAction->plug(m_item_pmenu);
	m_item_pmenu->insertSeparator();
	m_createAction->plug(m_item_pmenu);

	setXMLFile("kexiquerypartui.rc");

	view->insertChildClient(this);
}

/*KexiPartPopupMenu*
KexiQueryPartProxy::groupContext()
{
//	m->insertAction(i18n("Create Query..."), SLOT(slotCreateQuery()));
//	m_createAction->plug(m_group_pmenu);

	return m_group_pmenu;
}*/

/*KexiPartPopupMenu*
KexiQueryPartProxy::itemContext(const QString& identifier)
{
//	KexiPartPopupMenu *m = new KexiPartPopupMenu(this);
//	m->insertAction(i18n("Open Query"), SLOT(slotOpen(const QString &)));
//	m->insertAction(i18n("Edit Query"), SLOT(slotEdit(const QString &)));
//	m->insertAction(i18n("Delete Query..."), SLOT(slotDelete(const QString &)));


	return m_item_pmenu;
}*/


bool
KexiQueryPartProxy::executeItem(const QString& identifier)
{
	return slotOpen(identifier);
}


void
KexiQueryPartProxy::slotCreateQuery()
{
    bool ok = false;
    QString name = KLineEditDlg::getText(i18n("New Query"), i18n("Query name:"), "", &ok, kexiView());

    if (!ok || name.isEmpty())
		return;
	KexiQueryPartItem *it=new KexiQueryPartItem(part(), name, "kexi/query", name);
	KexiQueryDesigner *kqd = new KexiQueryDesigner(kexiView(), it, 0, false);
//	KexiQueryDesigner *kqd = new KexiQueryDesigner(kexiView(), 0, it, false);
	part()->items()->insert(it->fullIdentifier(),it);
//        KexiQueryDesigner *kqd = new KexiQueryDesigner(kexiView(), 0, "query",it, false);
	emit m_queryPart->itemListChanged(part());
	kexiView()->project()->addFileReference(FileReference("Queries",name,"/query/" + name + ".query"));
//	kqd->setIcon( it->handler()->itemPixmap() );
	kqd->show();
	kexiView()->project()->setModified(true);
}

bool
KexiQueryPartProxy::slotOpen(const QString& identifier)
{
	kdDebug() << "KexiQueryPartProxy::slotOpen(): id=" << identifier <<endl;

	part()->debug();
	KexiProjectHandlerItem *it=part()->items()->find(identifier);
	if (!it)
		return false;

	if(kexiView()->activateWindow(it->identifier()))
		return true;

	KexiQueryPartItem *it1=static_cast<KexiQueryPartItem*>(it->qt_cast("KexiQueryPartItem"));
	if (!it1)
		return false;

	KexiQueryDesigner *kqd = new KexiQueryDesigner(kexiView(), it1, 0, true);
//	KexiQueryDesigner *kqd = new KexiQueryDesigner(kexiView(), 0, it1, true);
//	kqd->setIcon( it->handler()->itemPixmap() );
	kqd->show();
	return true;
}

void
KexiQueryPartProxy::slotEdit(const QString &identifier)
{
	part()->debug();
	KexiProjectHandlerItem *it=part()->items()->find(identifier);
	if (!it) return;

	if(kexiView()->activateWindow(it->identifier()))
		return;

	KexiQueryPartItem *it1=static_cast<KexiQueryPartItem*>(it->qt_cast("KexiQueryPartItem"));
	if (!it1) return;

	KexiQueryDesigner *kqd = new KexiQueryDesigner(kexiView(), it1, 0, false);
//	KexiQueryDesigner *kqd = new KexiQueryDesigner(kexiView(), 0, it1, false);
//	kqd->setIcon( it->handler()->itemPixmap() );
	kqd->show();
}

void
KexiQueryPartProxy::slotDelete(const QString &identifier)
{
	part()->debug();

	KexiProjectHandlerItem *it=part()->items()->find(identifier);
	if (!it) {
		KMessageBox::sorry( 0, i18n( "Query not found" ) );
		return;
	}

	int ans = KMessageBox::questionYesNo(kexiView(),
		i18n("Do you want to delete \"%1\" query?").arg(it->title()), KEXI_APP_NAME);

	if (ans != KMessageBox::Yes)
		return;

	//close window if exists
	KexiDialogBase *w = m_view->findWindow(identifier);
	if (w && !w->close(true))
		return; //window do not want to be closed: give up!

	QString name = part()->localIdentifier(identifier);
	kdDebug() << "KexiQueryPartProxy::slotDelete() id: " << identifier << endl;
	kdDebug() << "KexiQueryPartProxy::slotDelete() name: " << name << endl;
	kexiView()->project()->removeFileReference("/query/" + name + ".query");

//	part()->items()->remove(name);
	part()->items()->remove(identifier);
	KexiQueryPart *npart = static_cast<KexiQueryPart *>(part());
	emit npart->itemListChanged(part());

	/* TODO: QUERY LIST VIEW REFRESHING DOESN'T WORK!!! 
	   TODO: SET DIRTY FLAG TO PROJECT*/
}

#include "kexiquerypartproxy.moc"
