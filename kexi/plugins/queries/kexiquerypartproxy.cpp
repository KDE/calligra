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
#include "kexiview.h"

KexiQueryPartProxy::KexiQueryPartProxy(KexiQueryPart *part,KexiView *view)
 : KexiProjectHandlerProxy(part,view),KXMLGUIClient()
{
	m_queryPart=part;
	kdDebug() << "KexiQueryPartProxy::KexiQueryPartProxy()" << endl;

        (void*) new KAction(i18n("Create &Query ..."), 0,
                this,SLOT(slotCreateQuery()), actionCollection(), "querypart_create");

        setXMLFile("kexiquerypartui.rc");

        view->insertChildClient(this);

}


KexiPartPopupMenu*
KexiQueryPartProxy::groupContext()
{
	KexiPartPopupMenu *m = new KexiPartPopupMenu(this);
	m->insertAction(i18n("Create Query"), SLOT(slotCreateQuery()));
	return m;
}

KexiPartPopupMenu*
KexiQueryPartProxy::itemContext(const QString& identifier)
{
	KexiPartPopupMenu *m = new KexiPartPopupMenu(this);
	m->insertAction(i18n("Open Query"), SLOT(slotOpen(QString)));
	m->insertAction(i18n("Delete Query"), SLOT(slotDelete(QString)));
	
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
        QString name = KLineEditDlg::getText(i18n("New Query"), i18n("Query Name:"), "", &ok, kexiView());

	KexiProjectHandler::ItemList *list=part()->items();

        if(ok && name.length() > 0)
        {
                KexiQueryDesigner *kqd = new KexiQueryDesigner(kexiView(), 0, name, "query");
                list->append(new KexiProjectHandlerItem(part(), name, "kexi/query", name));
                emit m_queryPart->itemListChanged(part());
//              project()->addFileReference("/query/" + name + ".query");

                kqd->show();
                kexiView()->project()->setModified(true);
        }
}

void
KexiQueryPartProxy::slotOpen(QString identifier)
{
        KexiQueryDesigner *kqd = new KexiQueryDesigner(kexiView(), 0, identifier, "oq");
        kqd->show();
}

void
KexiQueryPartProxy::slotDelete(QString identifier)
{
}

#include "kexiquerypartproxy.moc"
