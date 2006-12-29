/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexipart.h"
#include "kexipartinfo.h"
#include "kexipartitem.h"
#include "kexistaticpart.h"
#include "kexidialogbase.h"
#include "kexiviewbase.h"

#include "kexipartguiclient.h"
#include "keximainwindow.h"
//#include "kexipartdatasource.h"
#include "kexi.h"

#include <kexidb/connection.h>
#include <kexiutils/identifier.h>
#include <kexiutils/utils.h>

#include <q3widgetstack.h>
//Added by qt3to4:
#include <Q3CString>

#include <kactioncollection.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kxmlguifactory.h>

namespace KexiPart {
//! @internal
class PartPrivate
{
public:
	PartPrivate()
	: instanceActionsInitialized(false)
	{
	}

	//! Helper, used in Part::openInstance()
	tristate askForOpeningInTextMode(KexiDialogBase *dlg, KexiPart::Item &item, 
		int supportedViewModes, int viewMode)
	{
		if (viewMode != Kexi::TextViewMode
			&& supportedViewModes & Kexi::TextViewMode 
			&& dlg->tempData()->proposeOpeningInTextViewModeBecauseOfProblems)
		{
			//ask
			KexiUtils::WaitCursorRemover remover;
	//! @todo use message handler for this to enable non-gui apps
			QString singleStatusString( dlg->singleStatusString() );
			if (!singleStatusString.isEmpty())
				singleStatusString.prepend(QString("\n\n")+i18n("Details:")+" ");
			if (KMessageBox::No==KMessageBox::questionYesNo(0, 
				((viewMode == Kexi::DesignViewMode) 
					? i18n("Object \"%1\" could not be opened in Design View.").arg(item.name())
					: i18n("Object could not be opened in Data View."))+"\n"
				+ i18n("Do you want to open it in Text View?") + singleStatusString, 0, 
				KStdGuiItem::open(), KStdGuiItem::cancel()))
			{
	//			dlg->close(); //this will destroy dlg
				return false;
			}
			return true;
		}
		return cancelled;
	}

	bool instanceActionsInitialized : 1;
};
}

//----------------------------------------------------------------

using namespace KexiPart;

Part::Part(QObject *parent, const char *name, const QStringList &)
: QObject(parent)
, m_guiClient(0)
, m_registeredPartID(-1) //no registered ID by default
, d(new PartPrivate())
{
	setObjectName(name);
	m_info = 0;
	m_supportedViewModes = Kexi::DataViewMode | Kexi::DesignViewMode;
	m_supportedUserViewModes = Kexi::DataViewMode;
	m_mainWin = 0;
	m_newObjectsAreDirty = false;
}

Part::Part(QObject* parent, StaticInfo *info)
: QObject(parent)
, m_guiClient(0)
, m_registeredPartID(-1) //no registered ID by default
, d(new PartPrivate())
{
	setObjectName("StaticPart");
	m_info = info;
	m_supportedViewModes = Kexi::DesignViewMode;
	m_supportedUserViewModes = 0;
	m_mainWin = 0;
	m_newObjectsAreDirty = false;
}

Part::~Part()
{
	delete d;
}

void Part::createGUIClients(KexiMainWindow *win)
{
	m_mainWin = win;
	if (!m_guiClient) {
		//create part's gui client
		m_guiClient = new GUIClient(m_mainWin, this, false, "part");

		//default actions for part's gui client:
		KAction *act = new KAction(
			m_names["instanceCaption"]+"...",
			info()->createItemIcon(),
			KShortcut(),
			this,
			SLOT(slotCreate()),
			m_mainWin->actionCollection(),
			KexiPart::nameForCreateAction(*info()));
		act->plug( m_mainWin->findPopupMenu("insert") );
//		new KAction(m_names["instance"]+"...", info()->itemIcon(), 0, this, 
//		SLOT(create()), m_guiClient->actionCollection(), (info()->objectName()+"part_create").toLatin1());
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
				GUIClient *instanceGuiClient = new GUIClient(m_mainWin, 
					this, true, Kexi::nameForViewMode(mode).toLatin1());
				m_instanceGuiClients.insert(mode, instanceGuiClient);
//				initInstanceActions( mode, instanceGuiClient->actionCollection() );
			}
		}
		// also add an instance common for all modes (mode==0)
		GUIClient *instanceGuiClient = new GUIClient(m_mainWin, this, true, "allViews");
		m_instanceGuiClients.insert(Kexi::AllViewModes, instanceGuiClient);
//		initInstanceActions( Kexi::AllViewModes , instanceGuiClient->actionCollection() );

//todo
		initPartActions();
//		initActions();
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
		kDebug() << "KexiPart::createSharedAction(): no gui client for mode " << mode << "!" << endl;
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

KAction* Part::createSharedToggleAction(int mode, const QString &text, 
	const QString &pix_name, const KShortcut &cut, const char *name)
{
	return createSharedAction(mode, text, pix_name, cut, name, "KToggleAction");
}

KAction* Part::createSharedPartToggleAction(const QString &text, 
	const QString &pix_name, const KShortcut &cut, const char *name)
{
	return createSharedPartAction(text, pix_name, cut, name, "KToggleAction");
}

/*KAction* Part::sharedAction(int mode, const char* name, const char *classname)
{
	GUIClient *instanceGuiClient = m_instanceGuiClients[mode];
	if (!instanceGuiClient) {
		kDebug() << "KexiPart::createSharedAction(): no gui client for mode " << mode << "!" << endl;
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
	for(QMap<int, GUIClient*>::Iterator it = m_instanceGuiClients.begin(); it != m_instanceGuiClients.end(); ++it) {
		QAction *act = it.value()->actionCollection()->action(action_name);
		if (act) {
			act->setEnabled(avail);
			return;
		}
	}
	m_mainWin->setActionAvailable(action_name, avail);
}

KexiDialogBase* Part::openInstance(KexiMainWindow *win, KexiPart::Item &item, int viewMode,
	QMap<QString,QString>* staticObjectArgs)
{
	//now it's the time for creating instance actions
	if (!d->instanceActionsInitialized) {
		initInstanceActions();
		d->instanceActionsInitialized = true;
	}

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

/*! @todo js: apply settings for caption displaying method; there can be option for
 - displaying item.caption() as caption, if not empty, without instanceName
 - displaying the same as above in tabCaption (or not) */
//	dlg->setCaption( capt );
//	dlg->setTabCaption( item.name() );
	dlg->setId(item.identifier()); //not needed, but we did it
//moved down	dlg->registerDialog();
	dlg->setIcon( SmallIcon( dlg->itemIcon() ) );
	if (dlg->mdiParent())
		dlg->mdiParent()->setIcon( *dlg->icon() );
//	if (dlg->mainWidget())
//		dlg->mainWidget()->setIcon( *dlg->icon() );
	dlg->stack()->setIcon( *dlg->icon() );
	dlg->m_tempData = createTempData(dlg);

	if (!item.neverSaved()) {
		//we have to load schema data for this dialog
		dlg->m_schemaData = loadSchemaData(dlg, sdata, viewMode);
		if (!dlg->m_schemaData) {
			//last chance:
			if (false == d->askForOpeningInTextMode(dlg, item, dlg->m_supportedViewModes, viewMode)) {
				delete dlg;
				return 0;
			}
			viewMode = Kexi::TextViewMode;
			dlg->m_schemaData = loadSchemaData(dlg, sdata, viewMode);
		}
		if (!dlg->m_schemaData) {
			if (!m_status.error())
				m_status = Kexi::ObjectStatus( dlg->mainWin()->project()->dbConnection(), 
					i18n("Could not load object's definition."), i18n("Object design may be corrupted."));
			m_status.append( 
				Kexi::ObjectStatus(i18n("You can delete \"%1\" object and create it again.")
				.arg(item.name()), QString::null) );

			dlg->close();
			delete dlg;
			return 0;
		}
	}

	bool switchingFailed = false;
	bool dummy;
	tristate res = dlg->switchToViewMode( viewMode, staticObjectArgs, dummy );
	if (!res) {
		tristate askForOpeningInTextModeRes
			= d->askForOpeningInTextMode(dlg, item, dlg->m_supportedViewModes, viewMode);
//		if (viewMode==Kexi::DesignViewMode && dlg->isDesignModePreloadedForTextModeHackUsed(Kexi::TextViewMode))
//			askForOpeningInTextModeRes = cancelled; //do not try
//		else
		if (true == askForOpeningInTextModeRes) {
			delete dlg->m_schemaData; //old one
			dlg->close();
			delete dlg;
			//try in text mode
			return openInstance(win, item, Kexi::TextViewMode, staticObjectArgs);
		}
		else if (false == askForOpeningInTextModeRes) {
			delete dlg->m_schemaData; //old one
			dlg->close();
			delete dlg;
			return 0;
		}
		//dlg has an error info
		switchingFailed = true;
	}
	if (~res)
		switchingFailed = true;

	if (switchingFailed) {
		m_status = dlg->status();
		dlg->close();
		delete dlg;
		return 0;
	}
	dlg->registerDialog(); //ok?
	dlg->show();

	if (dlg->mdiParent() && dlg->mdiParent()->state()==KexiMdiMainFrm::Normal) //only resize dialog if it is in normal state
		dlg->resize(dlg->sizeHint());

	dlg->setMinimumSize(dlg->minimumSizeHint().width(),dlg->minimumSizeHint().height());

	//dirty only if it's a new object
	if (dlg->selectedView())
		dlg->selectedView()->setDirty( m_newObjectsAreDirty ? item.neverSaved() : false );
	
	return dlg;
}

void Part::slotCreate()
{
	emit newObjectRequest( m_info );
}

KexiDB::SchemaData* Part::loadSchemaData(KexiDialogBase * /*dlg*/, const KexiDB::SchemaData& sdata, 
	int /*viewMode*/)
{
	KexiDB::SchemaData *new_schema = new KexiDB::SchemaData();
	*new_schema = sdata;
	return new_schema;
}

bool Part::loadDataBlock( KexiDialogBase *dlg, QString &dataString, const QString& dataID)
{
	if (!dlg->mainWin()->project()->dbConnection()->loadDataBlock( dlg->id(), dataString, dataID )) {
		m_status = Kexi::ObjectStatus( dlg->mainWin()->project()->dbConnection(), 
			i18n("Could not load object's data."), i18n("Data identifier: \"%1\".").arg(dataID) );
		m_status.append( *dlg );
		return false;
	}
	return true;
}

void Part::initPartActions()
{
}

void Part::initInstanceActions()
{
}

bool Part::remove(KexiMainWindow *win, KexiPart::Item &item)
{
	if (!win || !win->project() || !win->project()->dbConnection())
		return false;
	KexiDB::Connection *conn = win->project()->dbConnection();
	return conn->removeObject( item.identifier() );
}

KexiDialogTempData* Part::createTempData(KexiDialogBase* dialog)
{
	return new KexiDialogTempData(dialog);
}

QString Part::i18nMessage(const Q3CString& englishMessage, KexiDialogBase* dlg) const
{
	Q_UNUSED(dlg);
	if( QString(englishMessage).startsWith(":") )
		return QString();
	return englishMessage;
}

void Part::setupCustomPropertyPanelTabs(KTabWidget *, KexiMainWindow*)
{
}

Q3CString Part::instanceName() const
{
	// "instanceName" should be already valid identifier but we're using
	// KexiUtils::string2Identifier() to be sure translators did it right.
	return KexiUtils::string2Identifier(m_names["instanceName"]).toLower().toLatin1();
}

QString Part::instanceCaption() const
{
	return m_names["instanceCaption"];
}

tristate Part::rename(KexiMainWindow *win, KexiPart::Item &item, const QString& newName)
{
	Q_UNUSED(win);
	Q_UNUSED(item);
	Q_UNUSED(newName);
	return true;
}

//-------------------------------------------------------------------------


GUIClient::GUIClient(KexiMainWindow *win, Part* part, bool partInstanceClient, const char* nameSuffix)
 : QObject(part, 
   (part->info()->objectName() 
    + (nameSuffix ? QString(":%1").arg(nameSuffix) : QString())).toLatin1() )
 , KXMLGUIClient(win)
{
	if(!win->project()->final())
		setXMLFile(QString::fromLatin1("kexi")+part->info()->objectName()
			+"part"+(partInstanceClient?"inst":"")+"ui.rc");

//	new KAction(part->m_names["new"], part->info()->itemIcon(), 0, this, 
//		SLOT(create()), actionCollection(), (part->info()->objectName()+"part_create").toLatin1());

//	new KAction(i18nInstanceName+"...", part->info()->itemIcon(), 0, this, 
//		SLOT(create()), actionCollection(), (part->info()->objectName()+"part_create").toLatin1());

//	win->guiFactory()->addClient(this);
}


#include "kexipart.moc"

