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
#include "KexiWindow.h"
#include "KexiWindowData.h"
#include "KexiView.h"

#include "kexipartguiclient.h"
#include "KexiMainWindowIface.h"
#include "kexi.h"

#include <kexidb/connection.h>
#include <kexiutils/identifier.h>
#include <kexiutils/utils.h>

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
	tristate askForOpeningInTextMode(KexiWindow *window, KexiPart::Item &item, 
		int supportedViewModes, int viewMode)
	{
		if (viewMode != Kexi::TextViewMode
			&& supportedViewModes & Kexi::TextViewMode 
			&& window->data()->proposeOpeningInTextViewModeBecauseOfProblems)
		{
			//ask
			KexiUtils::WaitCursorRemover remover;
	//! @todo use message handler for this to enable non-gui apps
			QString singleStatusString( window->singleStatusString() );
			if (!singleStatusString.isEmpty())
				singleStatusString.prepend(QString("\n\n")+i18n("Details:")+" ");
			if (KMessageBox::No==KMessageBox::questionYesNo(0, 
				((viewMode == Kexi::DesignViewMode) 
					? i18n("Object \"%1\" could not be opened in Design View.", item.name())
					: i18n("Object could not be opened in Data View."))+"\n"
				+ i18n("Do you want to open it in Text View?") + singleStatusString, 0, 
				KStandardGuiItem::open(), KStandardGuiItem::cancel()))
			{
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

Part::Part(QObject *parent, const QStringList &)
: QObject(parent)
, m_guiClient(0)
, m_registeredPartID(-1) //no registered ID by default
, d(new PartPrivate())
{
	m_info = 0;
	m_supportedViewModes = Kexi::DataViewMode | Kexi::DesignViewMode;
	m_supportedUserViewModes = Kexi::DataViewMode;
//	m_mainWin = 0;
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
//	m_mainWin = 0;
	m_newObjectsAreDirty = false;
}

Part::~Part()
{
	delete d;
}

void Part::createGUIClients()//KexiMainWindow *win)
{
//	m_mainWin = win;
	if (!m_guiClient) {
		//create part's gui client
		m_guiClient = new GUIClient(this, false, "part");

		//default actions for part's gui client:
		KAction *act = new KAction(
			KIcon(info()->createItemIcon()),
			m_names["instanceCaption"]+"...",
			KexiMainWindowIface::global()->actionCollection()
		);
		act->setObjectName(KexiPart::nameForCreateAction(*info()));
		connect(act, SIGNAL(triggered()), this, SLOT(slotCreate()));
		KexiMainWindowIface::global()->guiFactory()->addClient(m_guiClient); //this client is added permanently

		//default actions for part instance's gui client:
		//NONE
		//let init specific actions for part instances
		for (int mode = 1; mode <= 0x01000; mode <<= 1) {
			if (m_supportedViewModes & mode) {
				GUIClient *instanceGuiClient = new GUIClient( 
					this, true, Kexi::nameForViewMode(mode).toLatin1());
				m_instanceGuiClients.insert(mode, instanceGuiClient);
			}
		}
		// also add an instance common for all modes (mode==0)
		GUIClient *instanceGuiClient = new GUIClient(this, true, "allViews");
		m_instanceGuiClients.insert(Kexi::AllViewModes, instanceGuiClient);

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
	return KexiMainWindowIface::global()->createSharedAction(text, pix_name, cut, name, 
		instanceGuiClient->actionCollection(), subclassName);
}

KAction* Part::createSharedPartAction(const QString &text, 
	const QString &pix_name, const KShortcut &cut, const char *name,
	const char *subclassName)
{
	if (!m_guiClient)
		return 0;
	return KexiMainWindowIface::global()->createSharedAction(text, pix_name, cut, name, 
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
	KexiMainWindowIface::global()->setActionAvailable(action_name, avail);
}

KexiWindow* Part::openInstance(KexiPart::Item &item, int viewMode,
	QMap<QString,QString>* staticObjectArgs)
{
	//now it's the time for creating instance actions
	if (!d->instanceActionsInitialized) {
		initInstanceActions();
		d->instanceActionsInitialized = true;
	}

	m_status.clearStatus();
	KexiWindow *window = new KexiWindow(KexiMainWindowIface::global()->thisWidget(),
		m_supportedViewModes, *this, item);

	KexiDB::SchemaData sdata(m_info->projectPartID());
	sdata.setName( item.name() );
	sdata.setCaption( item.caption() );
	sdata.setDescription( item.description() );

/*! @todo js: apply settings for caption displaying method; there can be option for
 - displaying item.caption() as caption, if not empty, without instanceName
 - displaying the same as above in tabCaption (or not) */
	window->setId(item.identifier()); //not needed, but we did it
	window->setWindowIcon( SmallIcon( window->itemIcon() ) );
#warning todo
#if 0
	if (window->mdiParent())
		window->mdiParent()->setIcon( *window->icon() );
#endif
//	window->setWindowIcon( *window->icon() );
	window->setData( createWindowData(window) );

	if (!item.neverSaved()) {
		//we have to load schema data for this dialog
		window->setSchemaData( loadSchemaData(window, sdata, viewMode) );
		if (!window->schemaData()) {
			//last chance:
			if (false == d->askForOpeningInTextMode(
				window, item, window->supportedViewModes(), viewMode))
			{
				delete window;
				return 0;
			}
			viewMode = Kexi::TextViewMode;
			window->setSchemaData( loadSchemaData(window, sdata, viewMode) );
		}
		if (!window->schemaData()) {
			if (!m_status.error())
				m_status = Kexi::ObjectStatus( KexiMainWindowIface::global()->project()->dbConnection(), 
					i18n("Could not load object's definition."), i18n("Object design may be corrupted."));
			m_status.append( 
				Kexi::ObjectStatus(i18n("You can delete \"%1\" object and create it again.",
					item.name()), QString()) );

			window->close();
			delete window;
			return 0;
		}
	}

	bool switchingFailed = false;
	bool dummy;
	tristate res = window->switchToViewMode( viewMode, staticObjectArgs, dummy );
	if (!res) {
		tristate askForOpeningInTextModeRes
			= d->askForOpeningInTextMode(window, item, window->supportedViewModes(), viewMode);
		if (true == askForOpeningInTextModeRes) {
			delete window->schemaData(); //old one
			window->close();
			delete window;
			//try in text mode
			return openInstance(item, Kexi::TextViewMode, staticObjectArgs);
		}
		else if (false == askForOpeningInTextModeRes) {
			delete window->schemaData(); //old one
			window->close();
			delete window;
			return 0;
		}
		//the window has an error info
		switchingFailed = true;
	}
	if (~res)
		switchingFailed = true;

	if (switchingFailed) {
		m_status = window->status();
		window->close();
		delete window;
		return 0;
	}
	window->registerWindow(); //ok?
	window->show();

#warning Part::openInstance(): resize window->resize(window->sizeHint()) for standalone windows
#if 0
	if (window->mdiParent() && window->mdiParent()->state()==KexiMdiMainFrm::Normal) //only resize the window if it is in normal state
		window->resize(window->sizeHint());
#endif
	window->setMinimumSize(window->minimumSizeHint().width(), window->minimumSizeHint().height());

	//dirty only if it's a new object
	if (window->selectedView())
		window->selectedView()->setDirty( m_newObjectsAreDirty ? item.neverSaved() : false );
	
	return window;
}

void Part::slotCreate()
{
	emit newObjectRequest( m_info );
}

KexiDB::SchemaData* Part::loadSchemaData(KexiWindow *window, const KexiDB::SchemaData& sdata, 
	int viewMode)
{
	Q_UNUSED(window);
	Q_UNUSED(viewMode);
	KexiDB::SchemaData *new_schema = new KexiDB::SchemaData();
	*new_schema = sdata;
	return new_schema;
}

bool Part::loadDataBlock(KexiWindow *window, QString &dataString, const QString& dataID)
{
	if (!KexiMainWindowIface::global()->project()->dbConnection()->loadDataBlock( 
		window->id(), dataString, dataID ))
	{
		m_status = Kexi::ObjectStatus( KexiMainWindowIface::global()->project()->dbConnection(), 
			i18n("Could not load object's data."), i18n("Data identifier: \"%1\".", dataID) );
		m_status.append( *window );
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

bool Part::remove(KexiPart::Item &item)
{
	KexiDB::Connection *conn = KexiMainWindowIface::global()->project()->dbConnection();
	if (!conn)
		return false;
	return conn->removeObject( item.identifier() );
}

KexiWindowData* Part::createWindowData(KexiWindow* window)
{
	return new KexiWindowData(window);
}

KLocalizedString Part::i18nMessage(const QString& englishMessage, KexiWindow* window) const
{
	Q_UNUSED(window);
	if( QString(englishMessage).startsWith(":") )
		return KLocalizedString();
	return ki18n(englishMessage.toLatin1());
}

void Part::setupCustomPropertyPanelTabs(KTabWidget *)
{
}

QString Part::instanceName() const
{
	// "instanceName" should be already valid identifier but we're using
	// KexiUtils::string2Identifier() to be sure translators did it right.
	return KexiUtils::string2Identifier(m_names["instanceName"]).toLower();
}

QString Part::instanceCaption() const
{
	return m_names["instanceCaption"];
}

tristate Part::rename(KexiPart::Item &item, const QString& newName)
{
	Q_UNUSED(item);
	Q_UNUSED(newName);
	return true;
}

//-------------------------------------------------------------------------


GUIClient::GUIClient(Part* part, bool partInstanceClient, const char* nameSuffix)
 : QObject(part)
 , KXMLGUIClient(*KexiMainWindowIface::global()->guiClient())
{
	setObjectName(
		part->info()->objectName() 
    + (nameSuffix ? QString(":%1").arg(nameSuffix) : QString()) );
	
	if(!KexiMainWindowIface::global()->project()->data()->userMode())
		setXMLFile(QString::fromLatin1("kexi")+part->info()->objectName()
			+"part"+(partInstanceClient?"inst":"")+"ui.rc");

//	new KAction(part->m_names["new"], part->info()->itemIcon(), 0, this, 
//		SLOT(create()), actionCollection(), (part->info()->objectName()+"part_create").toLatin1());

//	new KAction(i18nInstanceName+"...", part->info()->itemIcon(), 0, this, 
//		SLOT(create()), actionCollection(), (part->info()->objectName()+"part_create").toLatin1());

//	win->guiFactory()->addClient(this);
}


#include "kexipart.moc"

