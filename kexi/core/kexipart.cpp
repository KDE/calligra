/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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
#include "kexiviewbase.h"

#include "kexipartguiclient.h"
#include "keximainwindow.h"
#include "kexipartdatasource.h"
#include "kexi.h"

#include <kexidb/connection.h>

#include <qwidgetstack.h>

#include <kiconloader.h>
#include <kdebug.h>

using namespace KexiPart;

Part::Part(QObject *parent, const char *name, const QStringList &)
: QObject(parent, name)
, m_guiClient(0)
{
	m_info = 0;
	m_supportedViewModes = Kexi::DataViewMode | Kexi::DesignViewMode;
	m_mainWin = 0;
}

Part::~Part()
{
}

void Part::createGUIClients(KexiMainWindow *win)
{
	m_mainWin = win;
	if (!m_guiClient) {
		//create part's gui client
		m_guiClient = new GUIClient(m_mainWin, this, false);

		const QString newobj_icon = info()->createItemIcon();
		Kexi::generateIconSetWithStar(info()->itemIcon(), newobj_icon);

		//default actions for part's gui client:
		KAction *act = new KAction(m_names["instance"]+"...", newobj_icon, 0, this, 
			SLOT(slotCreate()), this, (info()->objectName()+"part_create").latin1());
		act->plug( m_mainWin->findPopupMenu("create") );
//		new KAction(m_names["instance"]+"...", info()->itemIcon(), 0, this, 
//		SLOT(create()), m_guiClient->actionCollection(), (info()->objectName()+"part_create").latin1());
		//let init specific actions for parts
//		initPartActions( m_guiClient->actionCollection() );
		m_mainWin->guiFactory()->addClient(m_guiClient); //this client is added permanently

		//create part instance's gui client
//		m_instanceGuiClient = new GUIClient(win, this, true);

		//default actions for part instance's gui client:
		//NONE
		//let init specific actions for part instances
		for (int mode = 1; mode <= 0x01000; mode <<= 1) {
			if (m_supportedViewModes & mode) {
				GUIClient *instanceGuiClient = new GUIClient(m_mainWin, this, true);
				m_instanceGuiClients.insert(mode, instanceGuiClient);
//				initInstanceActions( mode, instanceGuiClient->actionCollection() );
			}
		}
		// also add an instance common for all modes (mode==0)
		GUIClient *instanceGuiClient = new GUIClient(m_mainWin, this, true);
		m_instanceGuiClients.insert(Kexi::AllViewModes, instanceGuiClient);
//		initInstanceActions( Kexi::AllViewModes , instanceGuiClient->actionCollection() );

//todo
		initActions();
	}
}

KActionCollection* Part::actionCollectionForMode(int viewMode) const
{
	KXMLGUIClient *cli = m_instanceGuiClients[viewMode];
	return cli ? cli->actionCollection() : 0;
}

KAction* Part::createSharedAction(int mode, const QString &text, 
	const QString &pix_name, const KShortcut &cut, const char *name,
	const char *subclassName)
{
	GUIClient *instanceGuiClient = m_instanceGuiClients[mode];
	if (!instanceGuiClient) {
		kdDebug() << "KexiPart::createSharedAction(): no gui client for mode " << mode << "!" << endl;
		return 0;
	}
	return m_mainWin->createSharedAction(text, pix_name, cut, name, 
		instanceGuiClient->actionCollection(), subclassName);
}

KAction* Part::createSharedPartAction(const QString &text, 
	const QString &pix_name, const KShortcut &cut, const char *name,
	const char *subclassName)
{
	if (!m_guiClient)
		return 0;
	return m_mainWin->createSharedAction(text, pix_name, cut, name, 
		m_guiClient->actionCollection(), subclassName);
}

/*KAction* Part::sharedAction(int mode, const char* name, const char *classname)
{
	GUIClient *instanceGuiClient = m_instanceGuiClients[mode];
	if (!instanceGuiClient) {
		kdDebug() << "KexiPart::createSharedAction(): no gui client for mode " << mode << "!" << endl;
		return 0;
	}
	return instanceGuiClient->actionCollection()->action(name, classname);
}

KAction* Part::sharedPartAction(int mode, const char* name, const char *classname)
{
	if (!m_guiClient)
		return 0;
	return m_guiClient->actionCollection()->action(name, classname);
}*/

void Part::setActionAvailable(const char *action_name, bool avail)
{
	QIntDictIterator<GUIClient> it( m_instanceGuiClients );
	for (;it.current();++it) {
		KAction *act = it.current()->actionCollection()->action(action_name);
		if (act) {
			act->setEnabled(avail);
			return;
		}
	}

	m_mainWin->setActionAvailable(action_name, avail);
}

KexiDialogBase* Part::openInstance(KexiMainWindow *win, KexiPart::Item &item, int viewMode )
{
	m_status.clearStatus();
//	KexiDialogBase *dlg = createInstance(win,item,viewMode);
//	if (!dlg)
//		return 0;
//	QString capt = QString("%1 : %2").arg(item.name()).arg(instanceName());
	KexiDialogBase *dlg = new KexiDialogBase(win);
	dlg->m_supportedViewModes = m_supportedViewModes;
//	dlg->m_neverSaved = item.neverSaved();
//	dlg->m_currentViewMode = viewMode;
	dlg->m_part = this;
	dlg->m_item = &item;
	dlg->updateCaption();

	KexiDB::SchemaData sdata(m_info->projectPartID());
	sdata.setName( item.name() );
	sdata.setCaption( item.caption() );
	sdata.setDescription( item.description() );

//js TODO: apply settings for caption displaying method; there can be option for
//- displaying item.caption() as caption, if not empty, without instanceName
//- displaying the same as above in tabCaption (or not)
//	dlg->setCaption( capt );
//	dlg->setTabCaption( item.name() );
	dlg->setId(item.identifier()); //not needed, but we did it
	dlg->registerDialog();
	dlg->setIcon( SmallIcon( dlg->itemIcon() ) );
	if (dlg->mdiParent())
		dlg->mdiParent()->setIcon( *dlg->icon() );
//	if (dlg->mainWidget())
//		dlg->mainWidget()->setIcon( *dlg->icon() );
	dlg->stack()->setIcon( *dlg->icon() );

	if (!item.neverSaved()) {
		//we have to load schema data for this dialog
		dlg->m_schemaData = loadSchemaData(dlg, sdata);
		if (!dlg->m_schemaData) {
			m_status = Kexi::ObjectStatus( i18n("Failed loading object's definition."), i18n("Data may be corrupted."));
			dlg->close(); //this will destroy dlg
			return 0;
		}
	}

	bool cancelled;
	bool switchingFailed = false;
	if (!dlg->switchToViewMode( viewMode, cancelled )) {
		//js TODO ERROR???
		switchingFailed = true;
	}
	if (cancelled)
		switchingFailed = true;

	if (switchingFailed) {
		m_status = dlg->status();
		dlg->close(); //this will destroy dlg
		return 0;
	}
	dlg->show();

	if (dlg->mdiParent() && dlg->mdiParent()->state()==KMdiChildFrm::Normal) //only resize dialog if it is in normal state
		dlg->resize(dlg->sizeHint());

	dlg->setMinimumSize(dlg->minimumSizeHint().width(),dlg->minimumSizeHint().height());

	if (dlg->selectedView())
		dlg->selectedView()->setDirty(false);
	
	return dlg;
}

void Part::slotCreate()
{
	emit newObjectRequest( m_info );
}

KexiDB::SchemaData* Part::loadSchemaData(KexiDialogBase * /*dlg*/, const KexiDB::SchemaData& sdata)
{
	KexiDB::SchemaData *new_schema = new KexiDB::SchemaData();
	*new_schema = sdata;
	return new_schema;
}

bool Part::loadDataBlock( KexiDialogBase *dlg, QString &dataString, const QString& dataID)
{
	if (!dlg->mainWin()->project()->dbConnection()->loadDataBlock( dlg->id(), dataString, dataID )) {
		m_status = Kexi::ObjectStatus( i18n("Failed loading object's data."), i18n("Data identifier: \"%1\".").arg(dataID) );
		m_status.append( *dlg );
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------


GUIClient::GUIClient(KexiMainWindow *win, Part* part, bool partInstanceClient)
 : QObject(part, part->info()->objectName().latin1()), KXMLGUIClient(win)
{
	if(!win->project()->final())
		setXMLFile(QString("kexi")+part->info()->objectName()+"part"+(partInstanceClient?"inst":"")+"ui.rc");

//	new KAction(part->m_names["new"], part->info()->itemIcon(), 0, this, 
//		SLOT(create()), actionCollection(), (part->info()->objectName()+"part_create").latin1());

//	new KAction(i18nInstanceName+"...", part->info()->itemIcon(), 0, this, 
//		SLOT(create()), actionCollection(), (part->info()->objectName()+"part_create").latin1());

//	win->guiFactory()->addClient(this);
}


#include "kexipart.moc"

