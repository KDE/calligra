/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002   Joseph Wenninger <jowenn@kde.org>

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

KexiQueryPartProxy::KexiQueryPartProxy(KexiQueryPart *part,KexiView *view)
 : KexiProjectHandlerProxy(part,view),KXMLGUIClient()
{
    setInstance(part->kexiProject()->instance());
	m_queryPart=part;
	kdDebug() << "KexiQueryPartProxy::KexiQueryPartProxy()" << endl;

    (void) new KAction(i18n("Create &Query..."), 0,
                       this,SLOT(slotCreateQuery()), actionCollection(), "querypart_create");

    setXMLFile("kexiquerypartui.rc");

    view->insertChildClient(this);
}


KexiPartPopupMenu*
KexiQueryPartProxy::groupContext()
{
	KexiPartPopupMenu *m = new KexiPartPopupMenu(this);
	m->insertAction(i18n("Create Query..."), SLOT(slotCreateQuery()));
	return m;
}

KexiPartPopupMenu*
KexiQueryPartProxy::itemContext(const QString& identifier)
{
	KexiPartPopupMenu *m = new KexiPartPopupMenu(this);
	m->insertAction(i18n("Open Query"), SLOT(slotOpen(const QString &)));
	m->insertAction(i18n("Edit Query"), SLOT(slotEdit(const QString &)));
	m->insertAction(i18n("Delete Query"), SLOT(slotDelete(const QString &)));

	return m;
}


void
KexiQueryPartProxy::executeItem(const QString& identifier)
{
	slotOpen(identifier);
}


void
KexiQueryPartProxy::slotCreateQuery()
{
    bool ok = false;
    QString name = KLineEditDlg::getText(i18n("New Query"), i18n("Query name:"), "", &ok, kexiView());

    if(ok && name.length() > 0)
    {
		KexiQueryPartItem *it;
        part()->items()->insert(name,it=new KexiQueryPartItem(part(), name, "kexi/query", name));
        KexiQueryDesigner *kqd = new KexiQueryDesigner(kexiView(), 0, "query",it, false);
        emit m_queryPart->itemListChanged(part());
        kexiView()->project()->addFileReference(FileReference("Queries",name,"/query/" + name + ".query"));
		kqd->show();
        kexiView()->project()->setModified(true);
    }
}

void
KexiQueryPartProxy::slotOpen(const QString& identifier)
{
	KexiProjectHandlerItem *it=(*(part()->items()))[part()->localIdentifier(identifier)];
	if (!it) return;

	if(kexiView()->activateWindow(it->identifier()))
		return;

	KexiQueryPartItem *it1=static_cast<KexiQueryPartItem*>(it->qt_cast("KexiQueryPartItem"));
	if (!it1) return;

	KexiQueryDesigner *kqd = new KexiQueryDesigner(kexiView(), 0, "oq", it1, true);
	kqd->show();
}

void
KexiQueryPartProxy::slotEdit(const QString &identifier)
{
	KexiProjectHandlerItem *it=(*(part()->items()))[part()->localIdentifier(identifier)];
	if (!it) return;

	if(kexiView()->activateWindow(it->identifier()))
		return;

	KexiQueryPartItem *it1=static_cast<KexiQueryPartItem*>(it->qt_cast("KexiQueryPartItem"));
	if (!it1) return;

	KexiQueryDesigner *kqd = new KexiQueryDesigner(kexiView(), 0, "oq", it1, false);
	kqd->show();
}

void
KexiQueryPartProxy::slotDelete(const QString &identifier)
{
	QString name = part()->localIdentifier(identifier);
	kdDebug() << "KexiQueryPartProxy::slotDelete() id: " << identifier << endl;
	kdDebug() << "KexiQueryPartProxy::slotDelete() name: " << name << endl;
	kexiView()->project()->removeFileReference("/query/" + name + ".query");
	part()->items()->remove(name);
	KexiQueryPart *npart = static_cast<KexiQueryPart *>(part());
	emit npart->itemListChanged(part());
}

#include "kexiquerypartproxy.moc"
