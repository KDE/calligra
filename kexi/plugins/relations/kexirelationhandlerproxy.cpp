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

#include "kexirelationdialog.h"
#include "kexirelationhandler.h"
#include "kexirelationhandlerproxy.h"
#include "kexiview.h"

KexiRelationHandlerProxy::KexiRelationHandlerProxy(KexiRelationHandler *part,KexiView *view)
 : KexiProjectHandlerProxy(part,view),KXMLGUIClient()
{
	kdDebug() << "KexiRelationHandlerProxy::KexiRelationHandlerProxy()" << endl;

    KAction *createAction = new KAction(i18n("Relations"), "relation", "",
                                        this,SLOT(slotShowRelationEditor()), actionCollection(), "relations");

    setXMLFile("kexirelationhandlerui.rc");

    view->insertChildClient(this);
}


KexiPartPopupMenu*
KexiRelationHandlerProxy::groupContext()
{
	return 0;
}

KexiPartPopupMenu*
KexiRelationHandlerProxy::itemContext(const QString&)
{
	return 0;
}


void
KexiRelationHandlerProxy::executeItem(const QString&)
{
}



void
KexiRelationHandlerProxy::slotShowRelationEditor()
{
    KexiRelationDialog *krd = new KexiRelationDialog(kexiView(), 0);
    krd->show();
}

#include "kexirelationhandlerproxy.moc"
