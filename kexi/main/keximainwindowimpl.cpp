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

#include "keximainwindowimpl.h"

#include <qapplication.h>
#include <qfile.h>
#include <qtimer.h>
#include <qobjectlist.h>

#include <kapplication.h>
#include <kmessagebox.h>
#include <kcmdlineargs.h>
#include <kaction.h>
#include <klocale.h>
#include <kstdaccel.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kkeydialog.h>
#include <kedittoolbar.h>
#include <kdeversion.h>
#include <kglobalsettings.h>
#include <kparts/componentfactory.h>

#include <kexidb/connection.h>
#include <kexidb/utils.h>

#include "kexibrowser.h"
#include "kexipropertyeditorview.h"
#include "kexipropertybuffer.h"
#include "kexiactionproxy.h"
#include "kexidialogbase.h"
#include "kexipartmanager.h"
#include "kexipart.h"
#include "kexipartinfo.h"
#include "kexipartguiclient.h"
#include "kexiproject.h"
#include "kexiprojectdata.h"
#include "kexiprojectset.h"
#include "kexi.h"
#include "kexi_utils.h"
#include "kexistatusbar.h"
#include "kexiinternalpart.h"

#include "startup/KexiStartupDialog.h"
#include "startup/KexiConnSelector.h"
#include "startup/KexiProjectSelectorBase.h"
#include "startup/KexiProjectSelector.h"
#include "startup/KexiNewProjectWizard.h"
#include "startup/KexiStartup.h"
#include "kexicontexthelp.h"
#include "kexinamedialog.h"

#if defined(Q_WS_WIN) || !KDE_IS_VERSION(3,1,9)
# include <unistd.h>
#else
# include <kuser.h>
#endif

//first fix the geometry
#define KEXI_NO_CTXT_HELP 1

//show property editor
#define KEXI_PROP_EDITOR 1

#if defined(KDOCKWIDGET_P)
#include <kdockwidget_private.h>
#endif

typedef QIntDict<KexiDialogBase> KexiDialogDict;

class KexiMainWindowImpl::Private
{
	public:
		KexiProject	*prj;
		KConfig *config;
#ifndef KEXI_NO_CTXT_HELP
		KexiContextHelp *ctxHelp;
#endif
		KexiBrowser *nav;
		QGuardedPtr<KexiPropertyEditorView> propEditor;
		QGuardedPtr<KexiPropertyBuffer> propBuffer;

		KexiDialogDict dialogs;
		KXMLGUIClient *curDialogGUIClient, *closedDialogGUIClient;
		QGuardedPtr<KexiDialogBase> curDialog;

		KexiNameDialog *nameDialog;

		QTimer timer; //helper timer
//		QPtrDict<KexiActionProxy> actionProxies;
//		KActionPtrList sharedActions;
//		QSignalMapper *actionMapper;

		QAsciiDict<QPopupMenu> popups; //list of menu popups
		QPopupMenu *createMenu;

		QString origAppCaption; //<! original application's caption (without project name)
		QString appCaptionPrefix; //<! application's caption prefix - prj name (if opened), else: null

		//! project menu
		KAction *action_save, *action_save_as, *action_close,
		 *action_project_properties;
		KActionMenu *action_open_recent, *action_show_other;
		KAction *action_open_recent_more;
		int action_open_recent_more_id;
		KAction *action_project_relations;

		//! edit menu
		KAction *action_edit_delete, *action_edit_delete_row,
			*action_edit_cut, *action_edit_copy, *action_edit_paste;
		// view menu
		KAction *action_view_nav, *action_view_propeditor;
		KRadioAction *action_view_data_mode, *action_view_design_mode, *action_view_text_mode;
#ifndef KEXI_NO_CTXT_HELP
		KToggleAction *action_show_helper;
#endif
		//data menu
		KAction *action_data_save_row;

		//for dock windows
		KMdiToolViewAccessor* navToolWindow;
		KMdiToolViewAccessor* propEditorToolWindow;

		QWidget *focus_before_popup;
//		KexiRelationPart *relationPart;

		int privateDocIDCounter; //!< counter: ID for private "document" like Relations window

		bool block_KMdiMainFrm_eventFilter : 1;

		//! Set to true only in destructor, used by closeDialog() to know if 
		//! user can cancel dialog closing. If true user even doesn't see any messages
		//! before closing a dialog. This is for extremely sanity... and shouldn't be even needed.
		bool forceDialogClosing : 1;
	Private()
		: dialogs(401)
//		, actionProxies(401)
	{
		nav=0;
		navToolWindow=0;
		prj = 0;
		curDialogGUIClient=0;
		closedDialogGUIClient=0;
		nameDialog=0;
		curDialog=0;
		block_KMdiMainFrm_eventFilter=false;
		focus_before_popup=0;
//		relationPart=0;
		privateDocIDCounter=0;
		action_view_nav=0;
		action_view_propeditor=0;
		forceDialogClosing=false;
		createMenu=0;
	}
};

//-------------------------------------------------

KexiMainWindowImpl::KexiMainWindowImpl()
 : KexiMainWindow()
// , KexiSharedActionHost(this)
 , d(new KexiMainWindowImpl::Private() )
{
	d->config = kapp->config();
	
	if ( !initialGeometrySet() ) {
		int scnum = QApplication::desktop()->screenNumber(parentWidget());
		QRect desk = QApplication::desktop()->screenGeometry(scnum);
		d->config->setGroup("MainWindow");
		QSize s ( d->config->readNumEntry( QString::fromLatin1("Width %1").arg(desk.width()), 700 ),
			d->config->readNumEntry( QString::fromLatin1("Height %1").arg(desk.height()), 480 ) );
		resize (kMin (s.width(), desk.width()), kMin(s.height(), desk.height()));
	}
	
	setManagedDockPositionModeEnabled(true);//TODO(js): remove this if will be default in kmdi :)
	setStandardMDIMenuEnabled();
	setAsDefaultHost(); //this is default host now.
	KGlobal::iconLoader()->addAppDir("kexi");
	setXMLFile("kexiui.rc");
	setAcceptDrops(true);

	//get informed
	connect(&Kexi::partManager(),SIGNAL(partLoaded(KexiPart::Part*)),this,SLOT(slotPartLoaded(KexiPart::Part*)));
	connect( m_pMdi, SIGNAL(nowMaximized(bool)), this, SLOT(slotCaptionForCurrentMDIChild(bool)) ); 
	connect( m_pMdi, SIGNAL(noMaximizedChildFrmLeft(KMdiChildFrm*)), this, SLOT(slotNoMaximizedChildFrmLeft(KMdiChildFrm*)));
	connect( m_pMdi, SIGNAL(lastChildFrmClosed()), this, SLOT(slotLastChildFrmClosed()));
	connect( this, SIGNAL(childViewIsDetachedNow(QWidget*)), this, SLOT(slotChildViewIsDetachedNow(QWidget*)));

	initActions();
	createShellGUI(true);
	(void) new KexiStatusBar(this, "status_bar");

	d->origAppCaption = caption();
	initContextHelp();
	initPropertyEditor();

	restoreSettings();

	{//store menu popups list
		QObjectList *l = queryList( "QPopupMenu" );
		for (QObjectListIt it( *l ); it.current(); ++it ) {
			kdDebug() << "name=" <<it.current()->name() << " cname="<<it.current()->className()<<endl;
			d->popups.insert(it.current()->name(), static_cast<QPopupMenu*>(it.current()));
		}
		delete l;
		d->createMenu = d->popups["create"];
	}

	if (!isFakingSDIApplication()) {
//		QPopupMenu *menu = (QPopupMenu*) child( "window", "KPopupMenu" );
		QPopupMenu *menu = d->popups["window"];
		unsigned int count = menuBar()->count();
		if (menu)
			setWindowMenu(menu);
		else
			menuBar()->insertItem( i18n("&Window"), windowMenu(), -1, count-2); // standard position is left to the last ('Help')
	}

	m_pTaskBar->setCaption(i18n("Task bar"));	//js TODO: move this to KMDIlib

	invalidateActions();

	d->timer.singleShot(0,this,SLOT(slotLastActions()));
}

KexiMainWindowImpl::~KexiMainWindowImpl()
{
	bool cancelled;
	d->forceDialogClosing=true;
	closeProject(cancelled);
	delete d;
}

KexiProject	*KexiMainWindowImpl::project()
{
	return d->prj;
}

void KexiMainWindowImpl::setWindowMenu(QPopupMenu *menu)
{
	if (m_pWindowMenu)
		delete m_pWindowMenu;
	m_pWindowMenu = menu;
	m_pWindowMenu->setCheckable(TRUE);
	QObject::connect( m_pWindowMenu, SIGNAL(aboutToShow()), this, SLOT(fillWindowMenu()) );
}

QPopupMenu* KexiMainWindowImpl::findPopupMenu(const char *popupName)
{
	return d->popups[popupName];
}

void
KexiMainWindowImpl::initActions()
{
//	d->actionMapper = new QSignalMapper(this, "act_map");
//	connect(d->actionMapper, SIGNAL(mapped(const QString &)), this, SLOT(slotAction(const QString &)));

	// PROJECT MENU
	KAction *action = new KAction(i18n("&New..."), "filenew", KStdAccel::shortcut(KStdAccel::New), 
		this, SLOT(slotProjectNew()), actionCollection(), "project_new");
	action->setToolTip(i18n("Create a new project"));
	action->setWhatsThis(i18n("Creates a new project. Currently opened project is not affected."));

	action = KStdAction::open( this, SLOT( slotProjectOpen() ), actionCollection(), "project_open" );
	action->setToolTip(i18n("Open an existing project"));
	action->setWhatsThis(i18n("Opens an existing project. Currently opened project is not affected."));

	d->action_open_recent = new KActionMenu(i18n("Open Recent"), 
		actionCollection(), "project_open_recent");
	connect(d->action_open_recent->popupMenu(),SIGNAL(activated(int)),this,SLOT(slotProjectOpenRecent(int)));
	connect(d->action_open_recent->popupMenu(), SIGNAL(aboutToShow()),this,SLOT(slotProjectOpenRecentAboutToShow()));
	d->action_open_recent->popupMenu()->insertSeparator();
	d->action_open_recent_more_id = d->action_open_recent->popupMenu()
		->insertItem(i18n("&More projects..."), this, SLOT(slotProjectOpenRecentMore()), 0, 1000);
	
	d->action_save = new KAction(i18n("&Save"), "filesave", KStdAccel::shortcut(KStdAccel::Save), 
		this, SLOT(slotProjectSave()), actionCollection(), "project_save");
	d->action_save->setToolTip(i18n("Save object changes"));
	d->action_save->setWhatsThis(i18n("Saves object changes from currently selected window."));

	d->action_save_as = new KAction(i18n("Save &As..."), "filesaveas", 0, 
		this, SLOT(slotProjectSaveAs()), actionCollection(), "project_saveas");
	d->action_save_as->setToolTip(i18n("Save object as"));
	d->action_save_as->setWhatsThis(i18n("Saves object changes from currently selected window under a new name (within the same project)."));

	d->action_project_properties = new KAction(i18n("Project properties"), "info", 0,
		this, SLOT(slotProjectProperties()), actionCollection(), "project_properties");

	d->action_close = new KAction(i18n("&Close Project"), 0, KStdAccel::shortcut(KStdAccel::Close),
		this, SLOT(slotProjectClose()), actionCollection(), "project_close" );
	d->action_close->setToolTip(i18n("Close the current project"));
	d->action_close->setWhatsThis(i18n("Closes the current project."));

	KStdAction::quit( this, SLOT(slotQuit()), actionCollection(), "quit");

	d->action_project_relations = new KAction(i18n("&Relationships..."), "relation", CTRL + Key_R, 
		this, SLOT(slotProjectRelations()), actionCollection(), "project_relations");
	d->action_project_relations->setToolTip(i18n("Project relationships"));
	d->action_project_relations->setWhatsThis(i18n("Shows project relationships."));

	//EDIT MENU
	d->action_edit_cut = createSharedAction( KStdAction::Cut, "edit_cut");
	d->action_edit_copy = createSharedAction( KStdAction::Copy, "edit_copy");
	d->action_edit_paste = createSharedAction( KStdAction::Paste, "edit_paste");

	d->action_edit_delete = createSharedAction(i18n("&Delete"), "button_cancel", Key_Delete, "edit_delete");
	d->action_edit_delete->setToolTip(i18n("Delete object"));
	d->action_edit_delete->setWhatsThis(i18n("Deletes currently selected object."));

	d->action_edit_delete_row = createSharedAction(i18n("Delete Row"), 0/*SmallIcon("button_cancel")*/, 
		SHIFT+Key_Delete, "edit_delete_row");
	d->action_edit_delete_row->setToolTip(i18n("Delete currently selected row from a table."));
	d->action_edit_delete_row->setWhatsThis(i18n("Deletes currently selected row from a table ."));

	//VIEW MENU
	d->action_view_data_mode = new KRadioAction(i18n("&Data View"), "table", KShortcut(), 
		this, SLOT(slotViewDataMode()), actionCollection(), "view_data_mode");
	d->action_view_data_mode->setExclusiveGroup("view_mode");
	d->action_view_data_mode->setToolTip(i18n("Switch to Data View mode"));
	d->action_view_data_mode->setWhatsThis(i18n("Switches to Data View mode."));

	d->action_view_design_mode = new KRadioAction(i18n("D&esign View"), "state_edit", KShortcut(), 
		this, SLOT(slotViewDesignMode()), actionCollection(), "view_design_mode");
	d->action_view_design_mode->setExclusiveGroup("view_mode");
	d->action_view_design_mode->setToolTip(i18n("Switch to Design View mode"));
	d->action_view_design_mode->setWhatsThis(i18n("Switches to Design View mode."));

	d->action_view_text_mode = new KRadioAction(i18n("&Text View"), "state_sql", KShortcut(), 
		this, SLOT(slotViewTextMode()), actionCollection(), "view_text_mode");
	d->action_view_text_mode->setExclusiveGroup("view_mode");
	d->action_view_text_mode->setToolTip(i18n("Switch to Text View mode"));
	d->action_view_text_mode->setWhatsThis(i18n("Switches to Text View mode."));

	d->action_view_nav = new KAction(i18n("Project navigator"), "", ALT + Key_1,
		this, SLOT(slotViewNavigator()), actionCollection(), "view_navigator");
	d->action_view_nav->setToolTip(i18n("Go to Project navigator panel"));
	d->action_view_nav->setWhatsThis(i18n("Goes to Project navigator panel."));

#ifdef KEXI_PROP_EDITOR
	d->action_view_propeditor = new KAction(i18n("Property editor"), "", ALT + Key_2,
		this, SLOT(slotViewPropertyEditor()), actionCollection(), "view_propeditor");
	d->action_view_propeditor->setToolTip(i18n("Go to Property editor panel"));
	d->action_view_propeditor->setWhatsThis(i18n("Goes to Property editor panel."));
#endif

	new KAction(i18n("From File..."), "fileopen", 0, 
		this, SLOT(slotImportFile()), actionCollection(), "import_file");
	new KAction(i18n("From Server..."), "server", 0, 
		this, SLOT(slotImportServer()), actionCollection(), "import_server");

	//DATA MENU
	d->action_data_save_row = createSharedAction(i18n("&Save Row"), "button_ok", SHIFT | Key_Return, "data_save_row");
	d->action_data_save_row->setToolTip(i18n("Save currently selected table row's data"));
	d->action_data_save_row->setWhatsThis(i18n("Saves currently selected table row's data."));

	//SETTINGS MENU
	setStandardToolBarMenuEnabled( true );
	action = KStdAction::keyBindings(this, SLOT( slotConfigureKeys() ), actionCollection() );
	action->setWhatsThis(i18n("Lets you configure shortcut keys."));

	action = KStdAction::configureToolbars( this, SLOT( slotConfigureToolbars() ), actionCollection() );
	action->setWhatsThis(i18n("Lets you configure toolbars."));

	d->action_show_other = new KActionMenu(i18n("Other"), 
		actionCollection(), "options_show_other");
#ifndef KEXI_NO_CTXT_HELP
	d->action_show_helper = new KToggleAction(i18n("Show Context Help"), "", CTRL + Key_H,
	 actionCollection(), "options_show_contexthelp");
#endif

	action =  KStdAction::preferences(this, SLOT(slotShowSettings()), actionCollection());
	action->setWhatsThis(i18n("Lets you configure Kexi."));

//	KAction *actionSettings = new KAction(i18n("Configure Kexi..."), "configure", 0,
//	 actionCollection(), "kexi_settings");
//	actionSettings->setWhatsThis(i18n("Lets you configure Kexi."));
//	connect(actionSettings, SIGNAL(activated()), this, SLOT(slotShowSettings()));
}

/* moved to host:*/
#if 0
KAction* KexiMainWindowImpl::createSharedActionInternal( KAction *action )
{
	connect(action,SIGNAL(activated()), d->actionMapper, SLOT(map()));
	d->actionMapper->setMapping(action, action->name());
	d->sharedActions.append( action );
	return action;
}

KAction* KexiMainWindowImpl::createSharedAction(const QString &text, const QString &pix_name, 
	const KShortcut &cut, const char *name)
{
	return createSharedActionInternal( 
		new KAction(text, (pix_name.isEmpty() ? QIconSet() : SmallIconSet(pix_name)),
		cut, 0/*receiver*/, 0/*slot*/, actionCollection(), name)
	);
}

KAction* KexiMainWindowImpl::createSharedAction( KStdAction::StdAction id, const char *name)
{
	return createSharedActionInternal( 
		KStdAction::create( id, name, 0/*receiver*/, 0/*slot*/, actionCollection() )
	);
}
#endif

void KexiMainWindowImpl::invalidateActions()
{
	invalidateProjectWideActions();
	invalidateSharedActions();
}

void KexiMainWindowImpl::invalidateSharedActions(QWidget *w)
{
	//TODO: enabling is more complex...
/*	d->action_edit_cut->setEnabled(true);
	d->action_edit_copy->setEnabled(true);
	d->action_edit_paste->setEnabled(true);*/

//	isActionAvailable

//js: THIS WILL BE SIMPLIFIED
	if (!w)
		w = focusWindow();
	KexiSharedActionHost::invalidateSharedActions(w);
/*	if (w) {
		KexiActionProxy *p = d->actionProxies[ w ];
		for (KActionPtrList::Iterator it=d->sharedActions.begin(); it!=d->sharedActions.end(); ++it) {
//			setActionAvailable((*it)->name(),p && p->isAvailable((*it)->name()));
			(*it)->setEnabled(p && p->isAvailable((*it)->name()));
		}
	}*/
}

void KexiMainWindowImpl::invalidateProjectWideActions()
{
//	stateChanged("project_opened",d->prj ? StateNoReverse : StateReverse);

	const bool have_dialog = d->curDialog;
	const bool dialog_dirty = d->curDialog && d->curDialog->dirty();

	//PROJECT MENU
	d->action_save->setEnabled(have_dialog && dialog_dirty);
	d->action_save_as->setEnabled(have_dialog);
	d->action_project_properties->setEnabled(d->prj);
	d->action_close->setEnabled(d->prj);
	d->action_project_relations->setEnabled(d->prj);

	//VIEW MENU
	d->action_view_nav->setEnabled(d->prj);
	d->action_view_propeditor->setEnabled(d->prj);
	d->action_view_data_mode->setEnabled( have_dialog && d->curDialog->supportsViewMode(Kexi::DataViewMode) );
	if (!d->action_view_data_mode->isEnabled())
		d->action_view_data_mode->setChecked(false);
	d->action_view_design_mode->setEnabled( have_dialog && d->curDialog->supportsViewMode(Kexi::DesignViewMode) );
	if (!d->action_view_design_mode->isEnabled())
		d->action_view_design_mode->setChecked(false);
	d->action_view_text_mode->setEnabled( have_dialog && d->curDialog->supportsViewMode(Kexi::TextViewMode) );
	if (!d->action_view_text_mode->isEnabled())
		d->action_view_text_mode->setChecked(false);
#ifndef KEXI_NO_CTXT_HELP
	d->action_show_helper->setEnabled(d->prj);
#endif

	//CREATE MENU
	if (d->createMenu)
		d->createMenu->setEnabled(d->prj);

	//DOCKS
	if (d->nav)
		d->nav->setEnabled(d->prj);
	if (d->propEditor)
		d->propEditor->setEnabled(d->prj);
}

void KexiMainWindowImpl::invalidateViewModeActions()
{
	if (d->curDialog) {
		//update toggle action
		if (d->curDialog->currentViewMode()==Kexi::DataViewMode)
			d->action_view_data_mode->setChecked( true );
		else if (d->curDialog->currentViewMode()==Kexi::DesignViewMode) 
			d->action_view_design_mode->setChecked( true );
		else if (d->curDialog->currentViewMode()==Kexi::TextViewMode) 
			d->action_view_text_mode->setChecked( true );
	}
}

#if 0
void KexiMainWindowImpl::setActionAvailable(const char *name, bool avail)
{
	KAction *act = actionCollection()->action(name);
	if (!act)
		return;
	act->setEnabled(avail);
}

void KexiMainWindowImpl::updateActionAvailable(const char *action_name, bool set, QObject *obj)
{
	QWidget *fw = focusWidget();
	while (fw && obj!=fw)
		fw = fw->parentWidget();

	if (!fw)
		return;

	setActionAvailable(action_name, set);
}

/*bool KexiMainWindowImpl::isActionAvailable(const char *action_name)
{
//	KexiActionProxy *proxy = d->actionProxies[ QCString(action_name)+QCString().setNum((ulong)proxy->receiver()) ];
	KexiActionProxy *proxy = d->actionProxies[ QCString(action_name)+QCString().setNum((ulong)proxy->receiver()) ];
	return proxy && proxy->isAvailable(action_name);
}*/

void KexiMainWindowImpl::plugActionProxy(KexiActionProxy *proxy)//, const char *action_name)
{
//	d->actionProxies[ QCString(action_name)+QCString().setNum((ulong)proxy->receiver()) ] = proxy;
	d->actionProxies.insert( proxy->receiver(), proxy);
}
#endif

void KexiMainWindowImpl::startup(KexiProjectData *projectData)
{
	kdDebug() << "KexiMainWindowImpl::startup()..." << endl;
	if (!projectData) {
//<TEMP>
		//some connection data
		KexiDB::ConnectionData *conndata;
		conndata = new KexiDB::ConnectionData();
			conndata->connName = "My connection";
			conndata->driverName = "mysql";
			conndata->hostName = "myhost.org";
			conndata->userName = "otheruser";
			conndata->port = 53121;
		Kexi::connset().addConnectionData(conndata);
		conndata = new KexiDB::ConnectionData();
			conndata->connName = "Local pgsql connection";
			conndata->driverName = "postgresql";
			conndata->hostName = "localhost"; // -- default //"host.net";
#if defined(Q_WS_WIN) || !KDE_IS_VERSION(3,1,9)
			conndata->userName = getlogin(); //-- temporary e.g."jarek"
#else
			conndata->userName = KUser().loginName(); //-- temporary e.g."jarek"
#endif
		Kexi::connset().addConnectionData(conndata);

		//some recent projects data
		projectData = new KexiProjectData( *conndata, "bigdb", "Big DB" );
		projectData->setCaption("My Big Project");
		projectData->setDescription("This is my first biger project started yesterday. Have fun!");
		Kexi::recentProjects().addProjectData(projectData);
	//</TEMP>

		if (!KexiStartupDialog::shouldBeShown())
			return;

		KexiStartupDialog dlg(KexiStartupDialog::Everything, KexiStartupDialog::CheckBoxDoNotShowAgain,
			Kexi::connset(), Kexi::recentProjects(), 0, "dlg");
		if (dlg.exec()!=QDialog::Accepted)
			return;
		
		projectData = 0;
		int r = dlg.result();
		if (r==KexiStartupDialog::TemplateResult) {
			kdDebug() << "Template key == " << dlg.selectedTemplateKey() << endl;
			if (dlg.selectedTemplateKey()=="blank") {
				createBlankDatabase();
				return;
			}
			return;//todo - templates
		}
		else if (r==KexiStartupDialog::OpenExistingResult) {
			kdDebug() << "Existing project --------" << endl;
			QString selFile = dlg.selectedExistingFile();
			if (!selFile.isEmpty()) {
				//file-based project
				kdDebug() << "Project File: " << selFile << endl;
				projectData = Kexi::detectProjectData( selFile, this );
			}
			else if (dlg.selectedExistingConnection()) {
				kdDebug() << "Existing connection: " << dlg.selectedExistingConnection()->serverInfoString() << endl;
				KexiDB::ConnectionData *cdata = dlg.selectedExistingConnection();
				//ok, now we will try to show projects for this connection to the user
				projectData = selectProject( cdata );
			}
		}
		else if (r==KexiStartupDialog::OpenRecentResult) {
			kdDebug() << "Recent project --------" << endl;
			const KexiProjectData *data = dlg.selectedProjectData();
			if (data) {
				kdDebug() << "Selected project: database=" << data->databaseName()
					<< " connection=" << data->constConnectionData()->serverInfoString() << endl;
			}
			//js: TODO
			return;
		}
	
		if (!projectData)
			return;
	}
	openProject(projectData);
}

bool KexiMainWindowImpl::openProject(KexiProjectData *projectData)
{
	if (!projectData)
		return false;
	createKexiProject( projectData );
//	d->prj = new KexiProject( projectData );
//	connect(d->prj, SIGNAL(error(const QString&,KexiDB::Object*)), this, SLOT(showErrorMessage(const QString&,KexiDB::Object*)));
	if (!d->prj->open()) {
		delete d->prj;
		d->prj = 0;
		return false;
	}
	initNavigator();
	Kexi::recentProjects().addProjectData( projectData );
	invalidateActions();

	QString not_found_msg;
	//ok, now open "autoopen: objects
//	for (QValueList< QPair<QString,QString> >::Iterator it = projectData->autoopenObjects.begin(); it != projectData->autoopenObjects.end(); ++it ) {
	for (QValueList<KexiProjectData::ObjectInfo>::Iterator it = projectData->autoopenObjects.begin(); it != projectData->autoopenObjects.end(); ++it ) {
//		openObject(QString("kexi/")+(*it).first,(*it).second);
		KexiProjectData::ObjectInfo info = *it;
		KexiPart::Info *i = Kexi::partManager().info( QCString("kexi/")+info["type"].latin1() );
		if (!i) {
			not_found_msg += ( info["name"] + " - " + i18n("unknown object type \"%1\"").arg(info["type"])+"<br>" );
			continue;
		}

		KexiPart::Item *item = d->prj->item(i, info["name"]);

		if (!item) {
			not_found_msg += ( info["name"] + " - " + i18n("object not found") +"<br>" );
			continue;
		}
		if (!openObject(item, info["action"]=="design" ? Kexi::DesignViewMode : Kexi::DataViewMode)) {
			not_found_msg += ( info["name"] + " - " + i18n("cannot open object") +"<br>" );
			continue;
		}
	}
	if (!not_found_msg.isEmpty())
		showErrorMessage(i18n("You have requested selected objects to be opened automatically on startup. Several objects cannot be opened."),
			not_found_msg );

	updateAppCaption();

	//make docks visible again
	if (!d->navToolWindow->wrapperWidget()->isVisible())
		static_cast<KDockWidget*>(d->navToolWindow->wrapperWidget())->makeDockVisible();
	if (!d->propEditorToolWindow->wrapperWidget()->isVisible())
		static_cast<KDockWidget*>(d->propEditorToolWindow->wrapperWidget())->makeDockVisible();
	return true;
}

KexiProjectData*
KexiMainWindowImpl::selectProject(KexiDB::ConnectionData *cdata)
{
	if (!cdata)
		return 0;
	KexiProjectData* projectData = 0;
	//dialog for selecting a project
	KexiProjectSelectorDialog prjdlg( this, "prjdlg", cdata, true, false );
	if (!prjdlg.projectSet() || prjdlg.projectSet()->error()) {
		showErrorMessage(i18n("Could not load list of available projects for connection \"%1\"")
		.arg(cdata->serverInfoString()), prjdlg.projectSet());
		return 0;
	}
	if (prjdlg.exec()!=QDialog::Accepted)
		return 0;
	if (prjdlg.selectedProjectData()) {
		//deep copy
		projectData = new KexiProjectData(*prjdlg.selectedProjectData());
	}
	return projectData;
}

bool KexiMainWindowImpl::closeProject(bool &cancelled)
{
	cancelled=false;
	if (!d->prj)
		return true;
		
	//close each window, optionally asking if user wants to close (if data changed)
	while (d->curDialog) {
		if (!closeDialog( d->curDialog, cancelled )) {
			return false;
		}
		if (cancelled)
			return true;
	}
	d->nav->clear();
	d->navToolWindow->hide();
	d->propEditorToolWindow->hide();

	delete d->prj;
	d->prj=0;

//	Kexi::partManager().unloadAllParts();
	invalidateActions();
	updateAppCaption();
	return true;
}

void KexiMainWindowImpl::initContextHelp() {
#ifndef KEXI_NO_CTXT_HELP
	d->ctxHelp=new KexiContextHelp(this,this);
	d->ctxHelp->setContextHelp(i18n("Welcome"),i18n("The <B>KEXI team</B> wishes you a lot of productive work, "
		"with this product. <BR><HR><BR>If you have found a <B>bug</B> or have a <B>feature</B> request, please don't "
		"hesitate to report it at our <A href=\"http://www.kexi-project.org/cgi-bin/bug.pl\"> issue "
		"tracking system </A>.<BR><HR><BR>If you would like to <B>join</B> our effort, the <B>development</B> documentation "
		"at <A href=\"http://www.kexi-project.org\">www.kexi-project.org</A> is a good starting point."),0);
	addToolWindow(d->ctxHelp,KDockWidget::DockBottom | KDockWidget::DockLeft,getMainDockWidget(),20);
#endif
}

void
KexiMainWindowImpl::initNavigator()
{
	kdDebug() << "KexiMainWindowImpl::initNavigator()" << endl;

	if(!d->nav)
	{
		d->nav = new KexiBrowser(this);
		d->nav->installEventFilter(this);
		d->navToolWindow = addToolWindow(d->nav, KDockWidget::DockLeft, getMainDockWidget(), 20/*, lv, 35, "2"*/);
		connect(d->nav,SIGNAL(openItem(KexiPart::Item*,int)),this,SLOT(openObject(KexiPart::Item*,int)));
		connect(d->nav,SIGNAL(openOrActivateItem(KexiPart::Item*,int)),
			this,SLOT(openObjectFromNavigator(KexiPart::Item*,int)));
		connect(d->nav,SIGNAL(newItem( KexiPart::Info* )),
			this,SLOT(newObject(KexiPart::Info*)));
		connect(d->nav,SIGNAL(removeItem(KexiPart::Item*)),
			this,SLOT(removeObject(KexiPart::Item*)));
		if (d->prj) {//connect to the project
			connect(d->prj, SIGNAL(itemRemoved(const KexiPart::Item&)), 
				d->nav, SLOT(slotRemoveItem(const KexiPart::Item&)));
		}
	}
	if(d->prj->isConnected()) {
		d->nav->clear();

		KexiPart::PartInfoList *pl = Kexi::partManager().partInfoList();
		for(KexiPart::Info *it = pl->first(); it; it = pl->next())
		{
			kdDebug() << "KexiMainWindowImpl::initNavigator(): adding " << it->groupName() << endl;
			d->nav->addGroup(it);

/*			KexiPart::Part *p=Kexi::partManager().part(it);
			if (!p) {
				//TODO: js - OPTIONALLY: show error
				continue;
			}
			p->createGUIClient(this);*/

			//load part - we need this to have GUI merged with part's actions
			//js: FUTURE TODO - don't do that when DESIGN MODE is OFF 
			KexiPart::Part *p=Kexi::partManager().part(it);
			if (!p) {
				//TODO: js - OPTIONALLY: show error
			}

			//lookup project's objects (part items)
			//js: FUTURE TODO - don't do that when DESIGN MODE is OFF 
			KexiPart::ItemDict *item_dict = d->prj->items(it);
			if (!item_dict)
				continue;
			for (KexiPart::ItemDictIterator item_it( *item_dict ); item_it.current(); ++item_it) {
				d->nav->addItem(item_it);
			}
		}
	}
	d->nav->setFocus();
	invalidateActions();
}

void KexiMainWindowImpl::slotLastActions()
{
#if defined(KEXI_PROP_EDITOR) && defined(KDOCKWIDGET_P)
	KDockWidget *dw = (KDockWidget *)d->propEditor->parentWidget();
	KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
	ds->resize(ds->width()*3, ds->height());
	ds->setSeparatorPos(30, true);
	ds->setForcedFixedWidth( dw, 200 );
#endif
#ifdef Q_WS_WIN
	showMaximized();//js: workaround for not yet completed layout settings storage on win32
#endif
}

void KexiMainWindowImpl::initPropertyEditor()
{
#ifdef KEXI_PROP_EDITOR
//TODO: FIX LAYOUT PROBLEMS
	d->propEditor = new KexiPropertyEditorView(this);
	d->propEditor->installEventFilter(this);
	d->propEditorToolWindow = addToolWindow(d->propEditor, 
		KDockWidget::DockRight, getMainDockWidget(), 20);

	KDockWidget *dw = (KDockWidget *)d->propEditor->parentWidget();
#if defined(KDOCKWIDGET_P)
	KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
	ds->show();
//	ds->resize(400, ds->height());
	ds->setSeparatorPos(400, true);
	ds->setForcedFixedWidth( dw, 400 );
//	ds->resize(400, ds->height());
//	dw->resize(400, dw->height());
#endif
	dw->setMinimumWidth(200);
//	ds->setMinimumWidth(200);
//	ds->setSeparatorPos(d->propEditor->sizeHint().width(), true);

	if (m_rightContainer) {
		m_rightContainer->setForcedFixedWidth( 400 );
	}
#endif
/*    KMdiToolViewAccessor *tmp=createToolWindow();
    tmp->setWidgetToWrap(d->propEditor);
	d->propEditor->show(); // I'm not sure, if this is a bug in kdockwidget, which I would better fix there
    tmp->show(KDockWidget::DockRight,getMainDockWidget(),20);
*/
}

void KexiMainWindowImpl::slotPartLoaded(KexiPart::Part* p)
{
	if (!p)
		return;
	connect(p, SIGNAL(newObjectRequest(KexiPart::Info*)), 
		this, SLOT(newObject(KexiPart::Info*)));
	p->createGUIClient(this);
}

//! internal
void KexiMainWindowImpl::slotCaptionForCurrentMDIChild(bool childrenMaximized)
{
	//js todo: allow to set custom "static" app caption

	KMdiChildView *view;
	if (!d->curDialog)
		view = 0;
	else if (d->curDialog->isAttached()) {
		view = d->curDialog;
	} else {
		//current dialog isn't attached! - find top level child
		if (m_pMdi->topChild()) {
			view = m_pMdi->topChild()->m_pClient;
			childrenMaximized = view->mdiParent()->state()==KMdiChildFrm::Maximized;
		}
		else
			view = 0;
	}

	if (childrenMaximized && view) {
		setCaption( d->curDialog->caption() 
			+ (d->appCaptionPrefix.isEmpty() ? "" : " - " + d->appCaptionPrefix) );
	}
	else {
		setCaption( (d->appCaptionPrefix.isEmpty() ? "" : d->appCaptionPrefix + " - ") 
			+ d->origAppCaption );
	}
}

void KexiMainWindowImpl::updateAppCaption()
{
	//js todo: allow to set custom "static" app caption

	d->appCaptionPrefix = "";
	if (d->prj && d->prj->data()) {//add project name
		d->appCaptionPrefix = d->prj->data()->caption();
		if (d->appCaptionPrefix.isEmpty())
			d->appCaptionPrefix = d->prj->data()->databaseName();
	}
	if (!d->appCaptionPrefix.isEmpty())
		d->appCaptionPrefix = d->appCaptionPrefix;

	bool max = false;
	if (d->curDialog && d->curDialog->mdiParent())
		max = d->curDialog->mdiParent()->state()==KMdiChildFrm::Maximized;

	slotCaptionForCurrentMDIChild(max);
/*
	KMdiChildView *view;
	if (!d->curDialog)
		view = 0;
	else if (d->curDialog->isAttached()) {
		view = d->curDialog;
	} else {
		//current dialog isn't attached! - find top level child
		if (m_pMdi->topChild()) {
			view = m_pMdi->topChild()->m_pClient;
		}
		else
			view = 0;
	}

	kApp->setCaption( d->appCaption );
	if (view && view->mdiParent()->state()==KMdiChildFrm::Maximized) {
		setCaption( view->caption() );
	}
	else {
		setCaption( d->appCaption );
	}*/
}

void KexiMainWindowImpl::slotNoMaximizedChildFrmLeft(KMdiChildFrm*)
{
	slotCaptionForCurrentMDIChild(false);
}

void KexiMainWindowImpl::slotLastChildFrmClosed()
{
	slotCaptionForCurrentMDIChild(false);
	activeWindowChanged(0);
}

void KexiMainWindowImpl::slotChildViewIsDetachedNow(QWidget*)
{
	slotCaptionForCurrentMDIChild(false);
}

/*void
KexiMainWindowImpl::closeEvent(QCloseEvent *ev)
{
	storeSettings();

	bool cancelled = false;
	if (!closeProject(cancelled)) {
		//todo: error message
		return;
	}
	if (cancelled) {
		ev->ignore();
		return;
	}
	
	ev->accept();
}*/

bool
KexiMainWindowImpl::queryClose()
{
//	storeSettings();
	bool cancelled;
	if (!closeProject(cancelled)) {
		//todo: error message
		return true;
	}
	return !cancelled;
}

bool
KexiMainWindowImpl::queryExit()
{
	storeSettings();
	return true;
}

void
KexiMainWindowImpl::restoreSettings()
{
	d->config->setGroup("MainWindow");

	// Saved settings
	applyMainWindowSettings( d->config, "MainWindow" );//, instance()->instanceName() );

	//small hack - set the default -- bottom
//	d->config->setGroup(QString(name()) + " KMdiTaskBar Toolbar style");
	d->config->setGroup("MainWindow Toolbar KMdiTaskBar");
	const bool tbe = d->config->readEntry("Position").isEmpty();
	if (tbe || d->config->readEntry("Position")=="Bottom") {
		if (tbe)
			d->config->writeEntry("Position","Bottom");
		moveDockWindow(m_pTaskBar, DockBottom);
	}

	int mdimode = d->config->readNumEntry("MDIMode", -1);//KMdi::TabPageMode);

	switch(mdimode)
	{
		case KMdi::ToplevelMode:
			switchToToplevelMode();
			m_pTaskBar->switchOn(true);
			break;
		case KMdi::ChildframeMode:
			switchToChildframeMode();
			m_pTaskBar->switchOn(true);
			break;
		case KMdi::IDEAlMode:
			switchToIDEAlMode();
			break;
		case KMdi::TabPageMode:
			switchToTabPageMode();
			break;
		default:;//-1
	}

	// restore a possible maximized Childframe mode
	bool maxChildFrmMode = d->config->readBoolEntry("maximized childframes", true);
	setEnableMaximizedChildFrmMode(maxChildFrmMode);

#if 0
	if ( !initialGeometrySet() ) {
		// Default size
//		int restoredWidth, restoredHeight;
    	int scnum = QApplication::desktop()->screenNumber(parentWidget());
		QRect desk = QApplication::desktop()->screenGeometry(scnum);
//#if KDE_IS_VERSION(3,1,90)
//		restoredWidth = KGlobalSettings::screenGeometry(scnum).width();
	//	restoredHeight = KGlobalSettings::screenGeometry(scnum).height();
//#else
//		restoredWidth = QApplication::desktop()->width();
//		restoredHeight = QApplication::desktop()->height();
//#endif
/*		if (restoredWidth > 1100) {// very big desktop ?
			restoredWidth = 1000;
			restoredHeight = 800;
		}
		if (restoredWidth > 850) {// big desktop ?
			restoredWidth = 800;
			restoredHeight = 600;
		}
		else {// small (800x600, 640x480) desktop
			restoredWidth = QMIN( restoredWidth, 600 );
			restoredHeight = QMIN( restoredHeight, 400 );
		}*/

		config->setGroup("MainWindow");
	    QSize s ( config->readNumEntry( QString::fromLatin1("Width %1").arg(desk.width()), 700 ),
              config->readNumEntry( QString::fromLatin1("Height %1").arg(desk.height()), 480 ) );
		resize (kMin (s.width(), desk.width()), kMin(s.height(), desk.height()));
	}
#endif
}

void
KexiMainWindowImpl::storeSettings()
{
	kdDebug() << "KexiMainWindowImpl::storeSettings()" << endl;

//	saveWindowSize( d->config ); //instance()->config() );
	saveMainWindowSettings( d->config, "MainWindow" );
	d->config->writeEntry("MDIMode", mdiMode());
//	config->sync();
	d->config->writeEntry("maximized childframes", isInMaximizedChildFrmMode());
}

void
KexiMainWindowImpl::restoreWindowConfiguration(KConfig *config)
{
	kdDebug()<<"preparing session restoring"<<endl;
	
	config->setGroup("MainWindow");

	QString dockGrp;

	if (kapp->isRestored())
		dockGrp=config->group()+"-Docking";
	else
		dockGrp="MainWindow0-Docking";

	if (config->hasGroup(dockGrp))
		readDockConfig(config,dockGrp);
}

void
KexiMainWindowImpl::storeWindowConfiguration(KConfig *config)
{
	kdDebug()<<"preparing session saving"<<endl;
	config->setGroup("MainWindow");
	QString dockGrp;

#if KDE_IS_VERSION(3,1,9)
	if (kapp->sessionSaving())
		dockGrp=config->group()+"-Docking";
	else
#endif
		dockGrp="MainWindow0-Docking";
  
	kdDebug()<<"Before write dock config"<<endl;
	writeDockConfig(config,dockGrp);
	kdDebug()<<"After write dock config"<<endl;
}

void
KexiMainWindowImpl::readProperties(KConfig *config) {
	restoreWindowConfiguration(config);
}

void
KexiMainWindowImpl::saveProperties(KConfig *config)
{
	storeWindowConfiguration(config);
//        m_docManager->saveDocumentList (config);
  //      m_projectManager->saveProjectList (config);
}

void
KexiMainWindowImpl::saveGlobalProperties( KConfig* sessionConfig ) {
	storeWindowConfiguration(sessionConfig);
}

void
KexiMainWindowImpl::registerChild(KexiDialogBase *dlg)
{
	kdDebug() << "KexiMainWindowImpl::registerChild()" << endl;
	connect(dlg, SIGNAL(activated(KMdiChildView *)), 
		this, SLOT(activeWindowChanged(KMdiChildView *)));
	connect(dlg, SIGNAL(dirtyChanged(KexiDialogBase*)), 
		this, SLOT(slotDirtyFlagChanged(KexiDialogBase*)));

//	connect(dlg, SIGNAL(childWindowCloseRequest(KMdiChildView *)), this, SLOT(childClosed(KMdiChildView *)));
	if(dlg->docID() != -1)
		d->dialogs.insert(dlg->docID(), dlg);
	kdDebug() << "KexiMainWindowImpl::registerChild() docID = " << dlg->docID() << endl;

	if (m_mdiMode==KMdi::ToplevelMode || m_mdiMode==KMdi::ChildframeMode) {//kmdi fix
		//js TODO: check if taskbar is switched in menu
		if (!m_pTaskBar->isSwitchedOn())
			m_pTaskBar->switchOn(true);
	}
	//KMdiChildFrm *frm = dlg->mdiParent();
	//if (frm) {
//		dlg->setMargin(20);
		//dlg->setLineWidth(20);
	//}
}

void
KexiMainWindowImpl::activeWindowChanged(KMdiChildView *v)
{
	KexiDialogBase *dlg = static_cast<KexiDialogBase *>(v);
	kdDebug() << "KexiMainWindowImpl::activeWindowChanged() to = " << (dlg ? dlg->caption() : "<none>") << endl;

	KXMLGUIClient *client=0;

	if (!dlg)
		client=0;
	else if ( dlg->isRegistered()) {
		client=dlg->guiClient();
		if (d->closedDialogGUIClient) {
			if (client!=d->closedDialogGUIClient) {
				//ooh, there is a client which dialog is already closed -- and we don't want it
				guiFactory()->removeClient(d->closedDialogGUIClient);
				d->closedDialogGUIClient=0;
			}
			else {
//				d->closedDialogGUIClient=0;
			}
		}
		if (client!=d->curDialogGUIClient) {
			kdDebug()<<"KexiMainWindowImpl::activeWindowChanged(): old gui client:"<<d->curDialogGUIClient<<" new gui client: "<<client<<endl;
			if (d->curDialogGUIClient) {
				guiFactory()->removeClient(d->curDialogGUIClient);
				d->curDialog->detachFromGUIClient();
			}
			if (client) {
				if (d->closedDialogGUIClient) {
					//ooh, there is a client which dialog is already closed -- BUT it is the same client as our
					//so: give up
				}
				else {
					guiFactory()->addClient(client);
				}
				dlg->attachToGUIClient();
			}
		} else {
			if ((KexiDialogBase*)d->curDialog!=dlg) {
				if (d->curDialog)
					d->curDialog->detachFromGUIClient();
				if (dlg)
					dlg->attachToGUIClient();
			}
		}
	}
	bool update_dlg_caption = dlg && dlg!=(KexiDialogBase*)d->curDialog && dlg->mdiParent();

	if (d->curDialogGUIClient && !client) 
		guiFactory()->removeClient(d->curDialogGUIClient);
	d->curDialogGUIClient=client;
	bool dialogChanged = ((KexiDialogBase*)d->curDialog)!=dlg;
	d->curDialog=dlg;

	propertyBufferSwitched(d->curDialog);

	if (dialogChanged) {
//		invalidateSharedActions();
		//update property editor's contents...
//		if ((KexiPropertyBuffer*)d->propBuffer!=d->curDialog->propertyBuffer()) {
//		propertyBufferSwitched();//d->curDialog);
//			d->propBuffer = d->curDialog->propertyBuffer();
//			d->propEditor->editor()->setBuffer( d->propBuffer );
//		}
	}

	//update caption...
	if (update_dlg_caption) {
		slotCaptionForCurrentMDIChild(d->curDialog->mdiParent()->state()==KMdiChildFrm::Maximized);
	}
	invalidateViewModeActions();
	invalidateActions();
}

bool
KexiMainWindowImpl::activateWindow(int id)
{
	kdDebug() << "KexiMainWindowImpl::activateWindow()" << endl;
	return activateWindow( d->dialogs[id] );
}

bool
KexiMainWindowImpl::activateWindow(KexiDialogBase *dlg)
{
	kdDebug() << "KexiMainWindowImpl::activateWindow(KexiDialogBase *)" << endl;
	if(!dlg)
		return false;

	dlg->activate();
	return true;
}

void
KexiMainWindowImpl::childClosed(KMdiChildView *v)
{
	kdDebug() << "KexiMainWindowImpl::unregisterWindow()" << endl;
	KexiDialogBase *dlg = static_cast<KexiDialogBase *>(v);
	d->dialogs.remove(dlg->docID());

	//focus navigator if nothing else available
	if (d->dialogs.isEmpty())
		d->nav->setFocus();
}

void
KexiMainWindowImpl::slotShowSettings()
{
//TODO	KexiSettings s(this);
//	s.exec();
}

void
KexiMainWindowImpl::slotConfigureKeys()
{
/*    KKeyDialog dlg;
    dlg.insert( actionCollection() );
    dlg.configure();*/
	KKeyDialog::configure( actionCollection() );
}

void
KexiMainWindowImpl::slotConfigureToolbars()
{
    KEditToolbar edit(factory());
//    connect(&edit,SIGNAL(newToolbarConfig()),this,SLOT(slotNewToolbarConfig()));
    (void) edit.exec();
}

void 
KexiMainWindowImpl::slotProjectNew()
{
	if (d->prj)//js: TODO: start new instance!
		return;
	createBlankDatabase();
}

void
KexiMainWindowImpl::createKexiProject(KexiProjectData* new_data)
{
	d->prj = new KexiProject( new_data );
	connect(d->prj, SIGNAL(error(const QString&,KexiDB::Object*)), this, SLOT(showErrorMessage(const QString&,KexiDB::Object*)));
	if (d->nav)
		connect(d->prj, SIGNAL(itemRemoved(const KexiPart::Item&)), d->nav, SLOT(slotRemoveItem(const KexiPart::Item&)));
}

bool
KexiMainWindowImpl::createBlankDatabase()
{
	KexiNewProjectWizard wiz(Kexi::connset(), 0, "KexiNewProjectWizard", true);
	if (wiz.exec() != QDialog::Accepted)
		return false;
	
	KexiProjectData *new_data = 0;
	if (wiz.projectConnectionData()) {
		//server-based project
		KexiDB::ConnectionData *cdata = wiz.projectConnectionData();
		kdDebug() << "DBNAME: " << wiz.projectDBName() << " SERVER: " << cdata->serverInfoString() << endl;
		new_data = new KexiProjectData( *cdata, wiz.projectDBName(), wiz.projectCaption() );
	}
	else if (!wiz.projectDBName().isEmpty()) {
		//file-based project
		KexiDB::ConnectionData cdata;
		cdata.connName = wiz.projectCaption();
		cdata.driverName = "sqlite";
		cdata.setFileName( wiz.projectDBName() );
		new_data = new KexiProjectData( cdata, wiz.projectDBName(), wiz.projectCaption() );
	}
	else
		return false;

	createKexiProject( new_data );
//	d->prj = new KexiProject( new_data );
//	connect(d->prj, SIGNAL(error(const QString&,KexiDB::Object*)), this, SLOT(showErrorMessage(const QString&,KexiDB::Object*)));
	if (!d->prj->create()) {
		delete d->prj;
		d->prj = 0;
		return false;
	}
	kdDebug() << "KexiMainWindowImpl::slotProjectNew(): new project created --- " << endl;
	initNavigator();
	Kexi::recentProjects().addProjectData( new_data );

	invalidateActions();
	updateAppCaption();
	return true;
}

void
KexiMainWindowImpl::slotProjectOpen()
{
	KexiStartupDialog dlg(
		KexiStartupDialog::OpenExisting, 0, Kexi::connset(), Kexi::recentProjects(),
		this, "KexiOpenDialog");
	
	if (dlg.exec()!=QDialog::Accepted)
		return;

	if (d->prj)//js: TODO: start new instance!
		return;

	KexiProjectData* projectData = 0;
	KexiDB::ConnectionData *cdata = dlg.selectedExistingConnection();
	if (cdata) {
		projectData = selectProject( cdata );
	}
	else {
		QString selFile = dlg.selectedExistingFile();
		if (!selFile.isEmpty()) {
			//file-based project
			kdDebug() << "Project File: " << selFile << endl;
			projectData = Kexi::detectProjectData( selFile, this );
		}
	}
	
	if (!projectData)
		return;
	openProject(projectData);
}

void
KexiMainWindowImpl::slotProjectOpenRecentAboutToShow()
{
	//setup
	KPopupMenu *popup = d->action_open_recent->popupMenu();
	const int cnt = popup->count();
	//remove older
	for (int i = 0; i<cnt; i++) {
		int id = popup->idAt(0);
		if (id==d->action_open_recent_more_id)
			break;
		if (id>=0) {
			popup->removeItem(id);
		}
	}
	//insert current items
	int cur_id = 0, cur_idx = 0;
	//TODO:
	cur_id = popup->insertItem("My example project 1", ++cur_id, cur_idx++);
	cur_id = popup->insertItem("My example project 2", ++cur_id, cur_idx++);
	cur_id = popup->insertItem("My example project 3", ++cur_id, cur_idx++);
}

void
KexiMainWindowImpl::slotProjectOpenRecent(int id)
{
	if (id<0 || id==d->action_open_recent_more_id)
		return;
	kdDebug() << "KexiMainWindowImpl::slotProjectOpenRecent("<<id<<")"<<endl;
}

void
KexiMainWindowImpl::slotProjectOpenRecentMore()
{
	KEXI_UNFINISHED(i18n("Open Recent"));
}

void
KexiMainWindowImpl::slotProjectSave()
{
	if (!d->curDialog)
		return;
	bool cancelled;
	saveObject( d->curDialog, cancelled );
}

void
KexiMainWindowImpl::slotProjectSaveAs()
{
	KEXI_UNFINISHED(i18n("Save object as"));
}

void
KexiMainWindowImpl::slotProjectProperties()
{
	KEXI_UNFINISHED(i18n("Project properties"));
}

void
KexiMainWindowImpl::slotProjectClose()
{
	bool cancelled;
	closeProject(cancelled);
}

void KexiMainWindowImpl::slotProjectRelations()
{
	if (!d->prj)
		return;
	KexiDialogBase *d = KexiInternalPart::createDialogInstance("relation", this);
	activateWindow(d);
/*	KexiRelationPart *p = relationPart();
	if(!p)
		return;
	
	p->createWindow(this);*/
}

/*
void KexiMainWindowImpl::slotAction(const QString& act_id)
{
	QWidget *w = focusWindow(); //focusWidget();
//	while (w && !w->inherits("KexiDialogBase") && !w->inherits("KexiDockBase"))
//		w = w->parentWidget();

	if (!w)
		return;

	KexiActionProxy * proxy = d->actionProxies[ w ];
	if (!proxy)
		return;
	proxy->activateSharedAction(act_id.latin1());
}*/

void KexiMainWindowImpl::slotImportFile()
{
	KEXI_UNFINISHED("Import: " + i18n("From File..."));
}

void KexiMainWindowImpl::slotImportServer()
{
	KEXI_UNFINISHED("Import: " + i18n("From Server..."));
}

void
KexiMainWindowImpl::slotQuit()
{
	bool cancelled;
	closeProject(cancelled);
	if (cancelled)
		return;
	close();
}

void KexiMainWindowImpl::slotViewNavigator()
{
	if (!d->nav || !d->navToolWindow)
		return;
	if (!d->nav->isVisible())
		makeWidgetDockVisible(d->nav);
//		makeDockVisible(dynamic_cast<KDockWidget*>(d->navToolWindow->wrapperWidget()));
//		d->navToolWindow->wrapperWidget()->show();
//		d->navToolWindow->show(KDockWidget::DockLeft, getMainDockWidget());

	d->navToolWindow->wrapperWidget()->raise();
//
	d->block_KMdiMainFrm_eventFilter=true;
		d->nav->setFocus();
	d->block_KMdiMainFrm_eventFilter=false;
}

void KexiMainWindowImpl::slotViewPropertyEditor()
{
	if (!d->propEditor || !d->propEditorToolWindow)
		return;

	if (!d->propEditor->isVisible())
		makeWidgetDockVisible(d->propEditor);

	d->propEditorToolWindow->wrapperWidget()->raise();

	d->block_KMdiMainFrm_eventFilter=true;
	d->propEditor->setFocus();
	d->block_KMdiMainFrm_eventFilter=false;
}

void KexiMainWindowImpl::slotViewDataMode()
{
	if (!d->curDialog)
		return;
	if (!d->curDialog->supportsViewMode( Kexi::DataViewMode )) {
		// js TODO error...
		return;
	}
	if (!d->curDialog->switchToViewMode( Kexi::DataViewMode )) {
		// js TODO error...
		return;
	}
}

void KexiMainWindowImpl::slotViewDesignMode()
{
	if (!d->curDialog)
		return;
	if (!d->curDialog->supportsViewMode( Kexi::DesignViewMode )) {
		// js TODO error...
		return;
	}
	if (!d->curDialog->switchToViewMode( Kexi::DesignViewMode )) {
		// js TODO error...
		return;
	}
}

void KexiMainWindowImpl::slotViewTextMode()
{
	if (!d->curDialog)
		return;
	if (!d->curDialog->supportsViewMode( Kexi::TextViewMode )) {
		// js TODO error...
		return;
	}
	if (!d->curDialog->switchToViewMode( Kexi::TextViewMode )) {
		// js TODO error...
		return;
	}
}

void
KexiMainWindowImpl::showErrorMessage(const QString &title, const QString &details)
{
	QString msg = title;
	if (title.isEmpty())
		msg = i18n("Unknown error");
	msg = "<qt><p>"+msg+"</p>";
	if (!details.isEmpty()) {
		KMessageBox::detailedError(this, msg, details);
	}
	else {
		KMessageBox::error(this, msg);
	}
}

void
KexiMainWindowImpl::showErrorMessage(const QString &title, KexiDB::Object *obj)
{
	QString msg = title;
	if (!obj) {
		showErrorMessage(msg);
		return;
	}
	QString details;
	KexiDB::getHTMLErrorMesage(obj, msg, details);
	
/*	msg += ("<p>"+obj->errorMsg());
	QString details;
	if (!obj->serverErrorMsg().isEmpty())
		details += "<p><b>" +i18n("Message from server:") + "</b> " + obj->serverErrorMsg();
	QString resname = obj->serverResultName();
	if (!resname.isEmpty())
		details += (QString("<p><b>")+i18n("Server result name:")+"</b> "+resname);
	if (!details.isEmpty()) {
		details += (QString("<p><b>")+i18n("Result number:")+"</b> "+QString::number(obj->serverResult()));
//		KMessageBox::detailedError(this, msg, details);
	}*/
	showErrorMessage(msg, details);
}

void KexiMainWindowImpl::closeWindow(KMdiChildView *pWnd, bool layoutTaskBar)
{
	bool cancelled;
	closeDialog(static_cast<KexiDialogBase *>(pWnd), cancelled, layoutTaskBar);
}

bool KexiMainWindowImpl::saveObject( KexiDialogBase *dlg, bool &cancelled ) //, bool dontAsk = true )
{
	cancelled=false;
	if (dlg->neverSaved()) {
		if (!d->nameDialog) {
			d->nameDialog = new KexiNameDialog(QString::null,
				this, "nameDialog");
		}
		d->nameDialog->widget()->setCaptionText(dlg->partItem()->caption());
		d->nameDialog->widget()->setNameText(dlg->partItem()->name());
		d->nameDialog->setCaption(i18n("Save Object As"));
		d->nameDialog->setDialogIcon( DesktopIcon( dlg->itemIcon(), KIcon::SizeMedium ) ); 
		bool found;
		do {
			if (d->nameDialog->exec()!=QDialog::Accepted) {
				cancelled=true;
				return true;
			}
			//check if that name already exists
			KexiDB::SchemaData tmp_sdata;
			found = project()->dbConnection()->findObjectSchemaData( 
					dlg->part()->info()->projectPartID(), 
					d->nameDialog->widget()->nameText(), tmp_sdata );
			if (found) {
				KMessageBox::information(this, i18n("%1 \"%2\" already exists.\nPlease choose other name.")
					.arg(dlg->part()->instanceName()).arg(d->nameDialog->widget()->nameText()));
			}
		}
		while (found);

		return dlg->storeNewData();
	}
	return dlg->storeData();
}

bool KexiMainWindowImpl::closeDialog(KexiDialogBase *dlg, bool &cancelled, bool layoutTaskBar)
{
	cancelled = false;
	if (!dlg)
		return true;
	bool remove_on_closing = dlg->partItem()->neverSaved();
	if (dlg->dirty() && !d->forceDialogClosing) {
		//dialog's data is dirty:
		const int res = KMessageBox::warningYesNoCancel( this,
			i18n( "<p>The object has been modified: %1 \"%2\".</p><p>Do you want to save it?</p>" )
			.arg(dlg->part()->instanceName()).arg(dlg->partItem()->name()),
			QString::null,
			KStdGuiItem::save(),
			KStdGuiItem::discard());
		if (res==KMessageBox::Cancel) {
			cancelled=true;
			return true;
		}
		if (res==KMessageBox::Yes) {
			//save it
//			if (!dlg->storeData())
			if (!saveObject( dlg, cancelled )) {
//js:TODO show error info; (retry/ignore/cancel)
				return false;
			}
			if (cancelled)
				return true;
			remove_on_closing = false;
		}
	}

	if (remove_on_closing) {
		//we won't save this object, and it was never saved -remove it
		if (!removeObject( dlg->partItem(), false )) {
			//msg?
			//TODO: ask if we'd continue and return true/false
			return false;
		}
	}
	else {
		//not dirty now
		d->nav->updateItemName( dlg->partItem(), false );
	}

	d->dialogs.take(dlg->docID()); //don't remove -KMDI will do that

	KXMLGUIClient *client = dlg->guiClient();
	if (d->curDialogGUIClient==client) {
		d->curDialogGUIClient=0;
	}
	if (client) {
		//sanity: ouch, it is not removed yet? - do it now
		if (d->closedDialogGUIClient && d->closedDialogGUIClient!=client) 
			guiFactory()->removeClient(d->closedDialogGUIClient);
		if (d->dialogs.isEmpty()) {//now there is no dialogs - remove client RIGHT NOW!
			d->closedDialogGUIClient=0;
			guiFactory()->removeClient(client);
		}
		else {
			//remember this - and MAYBE remove later, if needed
			d->closedDialogGUIClient=client; 
		}
	}

	KMdiMainFrm::closeWindow(dlg, layoutTaskBar);

	//focus navigator if nothing else available
	if (d->dialogs.isEmpty())
		d->nav->setFocus();

	invalidateActions();
	return true;
}

/*
KexiRelationPart *
KexiMainWindowImpl::relationPart()
{
	if(d->relationPart)
		return d->relationPart;

	d->relationPart = KParts::ComponentFactory::createInstanceFromLibrary<KexiRelationPart>("kexihandler_relation", this, "prel");
	return d->relationPart;
}*/

void KexiMainWindowImpl::detachWindow(KMdiChildView *pWnd,bool bShow)
{
	KMdiMainFrm::detachWindow(pWnd,bShow);
	// update icon - from small to large
	pWnd->setIcon( DesktopIcon( static_cast<KexiDialogBase *>(pWnd)->itemIcon() ) );
//	pWnd->setIcon( DesktopIcon( static_cast<KexiDialogBase *>(pWnd)->part()->info()->itemIcon() ) );
}

void KexiMainWindowImpl::attachWindow(KMdiChildView *pWnd, bool /*bShow*/, bool bAutomaticResize)
{
//	if (bAutomaticResize || w->size().isEmpty() || (w->size() == QSize(1,1))) {
	KMdiMainFrm::attachWindow(pWnd,true,bAutomaticResize);
	//for dialogs in normal state: decrease dialog's height if it exceeds area contents
	if (pWnd->mdiParent()->state() == KMdiChildFrm::Normal 
		&& pWnd->geometry().bottom() > pWnd->mdiParent()->mdiAreaContentsRect().bottom()) 
	{
		QRect r = pWnd->geometry();
		r.setBottom( pWnd->mdiParent()->mdiAreaContentsRect().bottom() - 5 );
		pWnd->setGeometry( r );
	}
	// update icon - from large to small
	pWnd->mdiParent()->setIcon( SmallIcon( static_cast<KexiDialogBase *>(pWnd)->itemIcon() ) );
//	pWnd->mdiParent()->setIcon( SmallIcon( static_cast<KexiDialogBase *>(pWnd)->part()->info()->itemIcon() ) );
}

/*
bool KexiMainWindowImpl::isWindow(QObject *o)
{
	return o->inherits("KexiDialogBase") || o->inherits("KexiDockBase");
}*/

QWidget* KexiMainWindowImpl::findWindow(QWidget *w)
{
	while (w && !acceptsSharedActions(w))
		w = w->parentWidget();
	return w;
}

bool KexiMainWindowImpl::acceptsSharedActions(QObject *w)
{
	return w->inherits("KexiDialogBase") || w->inherits("KexiViewBase");
}

/*moved to host
QWidget* KexiMainWindowImpl::focusWindow() const
{
	QWidget* fw = focusWidget();
	while (fw && !fw->inherits("KexiDialogBase") && !fw->inherits("KexiDockBase"))
		fw = fw->parentWidget();
	return fw;
}*/

bool KexiMainWindowImpl::eventFilter( QObject *obj, QEvent * e )
{
//	kdDebug() << "eventFilter: " <<e->type() << " " <<obj->name()<<endl;
	if (e->type()==QEvent::KeyPress) {
		kdDebug() << "KEY EVENT" << endl;
	}
	if (e->type()==QEvent::AccelOverride) {
		kdDebug() << "AccelOverride EVENT" << endl;
	}
	if (e->type()==QEvent::Close) {
		kdDebug() << "Close EVENT" << endl;
	}
	if (e->type()==QEvent::Resize) {
		kdDebug() << "Resize EVENT" << endl;
	}
	if (e->type()==QEvent::ShowMaximized) {
		kdDebug() << "ShowMaximized EVENT" << endl;
	}
	QWidget *focus_w = 0;
	QWidget *w = findWindow(static_cast<QWidget*>(obj));
	if (e->type()==QEvent::FocusIn || e->type()==QEvent::FocusOut) {
		focus_w = focusWindow();
		kdDebug() << "Focus EVENT" << endl;
		kdDebug() << (focus_w ? focus_w->name() : "" )  << endl;
		kdDebug() << "eventFilter: " <<e->type() << " " <<obj->name() <<endl;
	}
	if (e->type()==QEvent::WindowActivate) {
		kdDebug() << "WindowActivate EVENT" << endl;
		kdDebug() << "eventFilter: " <<e->type() << " " <<obj->name()<<endl;
	}
/*	if (e->type()==QEvent::FocusOut) {//after leaving focus from the menu, put it on prev. focused widget
		if (static_cast<QFocusEvent*>(e)->reason()==QFocusEvent::Popup && !obj->inherits("QMenuBar")) {
			if (static_cast<QWidget*>(obj)->hasFocus())
				d->focus_before_popup = static_cast<QWidget*>(obj);
		}
		else if (obj->inherits("QMenuBar") && d->focus_before_popup) {
			d->focus_before_popup->setFocus();
			d->focus_before_popup=0;
			return true;
		}
	}
	if (e->type()==QEvent::WindowDeactivate) {
		if (static_cast<QWidget*>(obj)->hasFocus())
			d->focus_before_popup = static_cast<QWidget*>(obj);
	}*/
//	if ((e->type()==QEvent::FocusIn /*|| e->type()==QEvent::FocusOut*/) && /*(!obj->inherits("KexiDialogBase")) &&*/ d->actionProxies[ obj ]) {
	if (e->type()==QEvent::FocusIn) {
		if (focus_w) {
//			if (d->actionProxies[ w ])
//			if (d->actionProxies[ focus_w ]) {
			if (actionProxyFor( focus_w )) {
				invalidateSharedActions();
			}
			else {
/*			QObject* o = focusWidget();
			while (o && !o->inherits("KexiDialogBase") && !o->inherits("KexiDockBase"))
				o = o->parent();*/
				invalidateSharedActions(focus_w);
			}
		}
//		/*|| e->type()==QEvent::FocusOut*/) && /*(!obj->inherits("KexiDialogBase")) &&*/ d->actionProxies[ obj ]) {
//		invalidateSharedActions();
	}
//	if ((e->type()==QEvent::FocusOut && focusWidget()==d->curDialog && !obj->inherits("KexiDialogBase")) && d->actionProxies[ obj ]) {
//	if (e->type()==QEvent::FocusOut && focus_w && focus_w==d->curDialog && d->actionProxies[ obj ]) {
	if (e->type()==QEvent::FocusOut && focus_w && focus_w==d->curDialog && actionProxyFor( obj )) {
		invalidateSharedActions(d->curDialog);
	}

	if (d->focus_before_popup && e->type()==QEvent::FocusOut && obj->inherits("KMenuBar")) {
		d->nav->setFocus();
		d->focus_before_popup=0;
		return true;
	}
	//keep focus in main window:
	if (w && w==d->nav) {
//		kdDebug() << "NAV" << endl;
		if (e->type()==QEvent::FocusIn) {
			return true;
		} else if (e->type()==QEvent::WindowActivate && w==d->focus_before_popup) {
			d->nav->setFocus();
			d->focus_before_popup=0;
			return true;
		} else if (e->type()==QEvent::FocusOut) {
			if (static_cast<QFocusEvent*>(e)->reason()==QFocusEvent::Tab) {
				//activate current child:
				if (d->curDialog) {
					d->curDialog->activate();
					return true;
				}
			}
			else if (static_cast<QFocusEvent*>(e)->reason()==QFocusEvent::Popup) {
				d->focus_before_popup=w;
			}
			invalidateSharedActions();
		} else if (e->type()==QEvent::Hide) {
			setFocus();
			return false;
		}
	}
	if (d->block_KMdiMainFrm_eventFilter)//we don't want KMDI to eat our event!
		return false;
	return KMdiMainFrm::eventFilter(obj,e);//let KMDI do its work
}

KexiDialogBase *
KexiMainWindowImpl::openObject(const QCString& mime, const QString& name, int viewMode)
{
	KexiPart::Item *item = d->prj->item(mime,name);
	if (!item)
		return 0;
	return openObject(item, viewMode);
}

KexiDialogBase *
KexiMainWindowImpl::openObject(KexiPart::Item* item, int viewMode)
{
	if (!d->prj || !item)
		return 0;
	KexiDialogBase *dlg = d->dialogs[ item->identifier() ];
	if (dlg) {
		if (dlg->currentViewMode()!=viewMode) {
			//try to switch
			if (!dlg->switchToViewMode(viewMode)) {
				//js TODO: add error msg...
				return 0;
			}
		}
		if (!activateWindow(dlg)) {
			//js TODO: add error msg...
			return 0;
		}
	}
	else {
		dlg = d->prj->openObject(this, *item, viewMode);
	}
	invalidateViewModeActions();
	return dlg;
}

/*! this slot handles event when user double clicked (or single -depending on settings)
 or pressed return ky on the part item in the navigator.
 This differs from openObject() signal in that if the object is already opened
 in view mode other than \a viewMode, the mode is not changed. */
KexiDialogBase *
KexiMainWindowImpl::openObjectFromNavigator(KexiPart::Item* item, int viewMode)
{
	if (!d->prj || !item)
		return false;
	KexiDialogBase *dlg = d->dialogs[ item->identifier() ];
	if (dlg) {
		if (activateWindow(dlg)) {//item->identifier())) {//just activate
			invalidateViewModeActions();
			return dlg;
		}
	}
	//do the same as in openObject()
	return openObject(item, viewMode);
}

bool KexiMainWindowImpl::newObject( KexiPart::Info *info )
{
	if (!d->prj || !info)
		return false;
	KexiPart::Part *part = Kexi::partManager().part(info->mime());
	if(!part)
		return false;

//js TODO: move this code
	if(info->projectPartID() == -1)
	{
		KexiDB::TableSchema *ts = project()->dbConnection()->tableSchema("kexi__parts");
		kdDebug() << "KexiMainWindowImpl::newObject(): schema: " << ts << endl;
		if (!ts)
			return false;
			
		KexiDB::FieldList *fl = ts->subList("p_name", "p_mime", "p_url");
		kdDebug() << "KexiMainWindowImpl::newObject(): fieldlist: " << fl << endl;
		if (!fl)
			return false;

		if (!project()->dbConnection()->insertRecord(*fl, QVariant(info->groupName()), QVariant(info->mime()), QVariant("http://")))
			return false;

		kdDebug() << "KexiMainWindowImpl::newObject(): insert success!" << endl;
		info->setProjectPartID(project()->dbConnection()->lastInsertedAutoIncValue("p_id", "kexi__parts"));
		kdDebug() << "KexiMainWindowImpl::newObject(): new id is: " << info->projectPartID()  << endl;
	}

#if 0 //js: this will be performed after saving confirmation
	KexiCreateItemDlg *dlg = new KexiCreateItemDlg(this, info->objectName(), "citem");
	if(!dlg->exec())
		return false;

//Ahh, Lucijan, what about using kexiDB API, not hardcoding? (js)
//aah, js, good idea! (lucijan)
//
//	if(!project()->dbConnection()->executeQuery(QString("INSERT INTO kexi__objects VALUES(NULL, %1, '%2', '%3', NULL)")
//		.arg(info->projectPartID()).arg(dlg->name()).arg(dlg->caption())))
//		return false;

	KexiDB::TableSchema *ts = project()->dbConnection()->tableSchema("kexi__objects");
	if (!ts)
		return false;
		
	KexiDB::FieldList *fl = ts->subList("o_type", "o_name", "o_caption");//TODO: "o_help");
	if (!fl)
		return false;
		
	if (!project()->dbConnection()->insertRecord(
		*fl, QVariant(info->projectPartID()), 
		QVariant(dlg->name()), QVariant(dlg->caption()) ))
		return false;

	delete fl;

	KexiPart::Item *it = new KexiPart::Item();
	it->setIdentifier(project()->dbConnection()->lastInsertedAutoIncValue("o_id", "kexi__objects"));
	it->setMime(info->mime());
	it->setName(dlg->name());
	it->setCaption(dlg->caption());

	kdDebug() << "KexiMainWindowImpl::newObject(): id = " << it->identifier() << endl;

	d->nav->addItem(it);

	openObject(it, Kexi::DesignViewMode);

	delete dlg;
#endif //0
	KexiPart::Item *it = d->prj->createPartItem(info); //this, *item, viewMode);
	if (!it) {
		//js: todo: err
		return false;
	}
/*	KexiPart::Item *it = new KexiPart::Item();
	it->setMime(info->mime());
	it->setName(dlg->name());
	it->setNeverSaved(true);*/

	if (!it->neverSaved()) //only add stored objects to the browser
		d->nav->addItem(it);
	return openObject(it, Kexi::DesignViewMode);
}

bool KexiMainWindowImpl::removeObject( KexiPart::Item *item, bool dontAsk )
{
	if (!d->prj || !item)
		return false;

	KexiPart::Part *part = Kexi::partManager().part(item->mime());
	if (!part)
		return false;

	if (dontAsk) {
		if (KMessageBox::questionYesNo(this, "<p>"+i18n("Do you want to remove:")
			+"</p><p>"+part->instanceName()+" \""+ item->name() + "\"?</p>",
			0, KStdGuiItem::yes(), KStdGuiItem::no(), "askBeforeDeletePartItem"/*config entry*/)==KMessageBox::No)
			return true;//cancelled
	}

	/*KexiDialogBase *dlg = d->dialogs[item->identifier()];
	if (dlg) {//close existing window
//		if (!dlg->tryClose(true))
		if (!dlg->close(true))
			return true; //ok - close cancelled
	}*/

	if (!d->prj->removeObject(this, *item)) {
		//TODO(js) some msg
		return false;
	}
	return true;
}

int KexiMainWindowImpl::generatePrivateDocID()
{
	return --d->privateDocIDCounter;
}

void KexiMainWindowImpl::propertyBufferSwitched(KexiDialogBase *dlg)
{
	kdDebug() << "KexiMainWindowImpl::propertyBufferSwitched()" << endl;
	if ((KexiDialogBase*)d->curDialog!=dlg)
		return;
	if (d->propEditor) {
		KexiPropertyBuffer *newBuf = d->curDialog ? d->curDialog->propertyBuffer() : 0;
		if (!newBuf || (KexiPropertyBuffer *)d->propBuffer != newBuf) {
			d->propBuffer = newBuf;
			d->propEditor->editor()->setBuffer( d->propBuffer );
		}
	}
}

void KexiMainWindowImpl::slotDirtyFlagChanged(KexiDialogBase* dlg)
{
	KexiPart::Item *item = dlg->partItem();
	//update text in navigator and app. caption
	d->nav->updateItemName( item, dlg->dirty() );
	updateAppCaption();
	invalidateActions();
}

#include "keximainwindowimpl.moc"

