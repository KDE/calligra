/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexipart.h"

#include "kexipartinfo.h"
#include "kexipartitem.h"
#include "kexidialogbase.h"

#include "kexipartguiclient.h"
#include "keximainwindow.h"

#include <qwidgetstack.h>

#include <kiconloader.h>

using namespace KexiPart;

Part::Part(QObject *parent, const char *name, const QStringList &)
: QObject(parent, name)
, m_guiClient(0)
{
	m_info = 0;
	m_supportedViewModes = Kexi::DataViewMode | Kexi::DesignViewMode;
}

Part::~Part()
{
}

void Part::createGUIClient(KexiMainWindow *win)
{
	if (!m_guiClient) {
		//create part's gui client
		m_guiClient = new GUIClient(win, this, false);
		//default actions for part's gui client:
		KAction *act = new KAction(m_names["instance"]+"...", info()->itemIcon(), 0, this, 
			SLOT(slotCreate()), this, (info()->objectName()+"part_create").latin1());
		act->plug( win->findPopupMenu("create") );
//		new KAction(m_names["instance"]+"...", info()->itemIcon(), 0, this, 
//		SLOT(create()), m_guiClient->actionCollection(), (info()->objectName()+"part_create").latin1());
		//let init specific actions for parts
		initPartActions( m_guiClient->actionCollection() );
		win->guiFactory()->addClient(m_guiClient); //this client is added premanently

		//create part instance's gui client
		m_instanceGuiClient = new GUIClient(win, this, true);
		//default actions for part instance's gui client:
		//NONE
		//let init specific actions for part instances
		initInstanceActions( m_instanceGuiClient->actionCollection() );
	}
}

KexiDialogBase* Part::openInstance(KexiMainWindow *win, const KexiPart::Item &item, int viewMode )
{
//	KexiDialogBase *dlg = createInstance(win,item,viewMode);
//	if (!dlg)
//		return 0;
	QString capt = QString("%1 : %2").arg(item.name()).arg(instanceName());
	KexiDialogBase *dlg = new KexiDialogBase(win, capt);
	dlg->m_supportedViewModes = m_supportedViewModes;
	dlg->m_neverSaved = item.neverSaved();
//	dlg->m_currentViewMode = viewMode;
	dlg->m_part = this;
	dlg->m_item = &item;

//js TODO: apply settings for caption displaying menthod; there can be option for
//- displaying item.caption() as caption, if not empty, without instanceName
//- displaying the same as above in tabCaption (or not)
	dlg->setCaption( capt );
//	dlg->setTabCaption( dlg->caption() );
	dlg->setTabCaption( item.name() );
	dlg->setDocID(item.identifier());
	dlg->registerDialog();
	dlg->setIcon( SmallIcon( dlg->itemIcon() ) );
	if (dlg->mdiParent())
		dlg->mdiParent()->setIcon( *dlg->icon() );
//	if (dlg->mainWidget())
//		dlg->mainWidget()->setIcon( *dlg->icon() );
	dlg->stack()->setIcon( *dlg->icon() );

	if (!dlg->switchToViewMode( viewMode )) {
		//js TODO ERROR???
	}

	if (dlg->mdiParent() && dlg->mdiParent()->state()==KMdiChildFrm::Normal) //only resize dialog if it is in normal state
		dlg->resize(dlg->sizeHint());

	dlg->setMinimumSize(dlg->minimumSizeHint().width(),dlg->minimumSizeHint().height());

//	QWidget *view = createView(dlg->stack(), dlg, item, viewMode);
/*
	if (!view) {
		//js TODO ERROR???
	}
	else {
		dlg->addView(view, viewMode);
	}*/
	
	return dlg;
}

void Part::slotCreate()
{
	emit newObjectRequest( m_info );
}

//-------------------------------------------------------------------------


GUIClient::GUIClient(KexiMainWindow *win, Part* part, bool partInstanceClient)
 : QObject(part, part->info()->objectName().latin1()), KXMLGUIClient(win)
{
	setXMLFile(QString("kexi")+part->info()->objectName()+"part"+(partInstanceClient?"inst":"")+"ui.rc");

//	new KAction(part->m_names["new"], part->info()->itemIcon(), 0, this, 
//		SLOT(create()), actionCollection(), (part->info()->objectName()+"part_create").latin1());

//	new KAction(i18nInstanceName+"...", part->info()->itemIcon(), 0, this, 
//		SLOT(create()), actionCollection(), (part->info()->objectName()+"part_create").latin1());

//	win->guiFactory()->addClient(this);
}


#include "kexipart.moc"

