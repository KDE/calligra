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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "keximainwindowimpl.h"

#include <qapplication.h>
#include <qfile.h>
#include <qtimer.h>
#include <qobjectlist.h>
#include <qprocess.h>

#include <kapplication.h>
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
#include <ktip.h>
#include <kstandarddirs.h>
#include <kpushbutton.h>
#include <ktextbrowser.h>
#include <kiconloader.h>

#include <kexidb/connection.h>
#include <kexidb/utils.h>
#include <kexidb/cursor.h>

#include "projectsettingsui.h"
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
#include "kexiuseraction.h"

#include "startup/KexiStartup.h"
#include "startup/KexiNewProjectWizard.h"
#include "startup/KexiStartupDialog.h"
/*
#include "startup/KexiConnSelector.h"
#include "startup/KexiProjectSelectorBase.h"
#include "startup/KexiProjectSelector.h"
*/
#include "kexinamedialog.h"

//Extreme verbose debug
#if defined(Q_WS_WIN)
# define KexiVDebug kdDebug()
#endif
#if !defined(KexiVDebug)
# define KexiVDebug if (0) kdDebug()
#endif

//first fix the geometry
#define KEXI_NO_CTXT_HELP 1

#ifndef KEXI_NO_CTXT_HELP
#include "kexicontexthelp.h"
#endif

//show property editor
#define KEXI_PROP_EDITOR 1

//! @todo REENABLE when blinking and dock
//! width changes will be removed in KMDI
//#define PROPEDITOR_VISIBILITY_CHANGES

//#undef KDOCKWIDGET_P
#if defined(KDOCKWIDGET_P)
#include <kdockwidget_private.h>
#endif

//#ifndef KEXI_NO_MIGRATION
//#include "migration/importwizard.h"
//#endif

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
		KXMLGUIClient *curDialogGUIClient, *curDialogViewGUIClient,
			*closedDialogGUIClient, *closedDialogViewGUIClient;
		QGuardedPtr<KexiDialogBase> curDialog;

		KexiNameDialog *nameDialog;

		QTimer timer; //helper timer
//		QSignalMapper *actionMapper;

		QAsciiDict<QPopupMenu> popups; //list of menu popups
		QPopupMenu *createMenu;

		QString origAppCaption; //<! original application's caption (without project name)
		QString appCaptionPrefix; //<! application's caption prefix - prj name (if opened), else: null

#ifndef KEXI_SHOW_UNIMPLEMENTED
		KActionMenu *dummy_action;
#endif

		//! project menu
		KAction *action_save, *action_save_as, *action_close,
		 *action_project_properties;
		KActionMenu *action_open_recent, *action_show_other;
		KAction *action_open_recent_more;
		int action_open_recent_more_id;
		KAction *action_project_relations;

		//! edit menu
		KAction *action_edit_delete, *action_edit_delete_row,
			*action_edit_cut, *action_edit_copy, *action_edit_paste,
			*action_edit_undo, *action_edit_redo,
			*action_edit_insert_empty_row,
			*action_edit_edititem, *action_edit_clear_table;

		//! view menu
		KAction *action_view_nav, *action_view_propeditor;
		KRadioAction *action_view_data_mode, *action_view_design_mode, *action_view_text_mode;
		QIntDict<KRadioAction> actions_for_view_modes;
//		KRadioAction *last_checked_mode;
#ifndef KEXI_NO_CTXT_HELP
		KToggleAction *action_show_helper;
#endif
		//! data menu
		KAction *action_data_save_row;
		KAction *action_data_cancel_row_changes;

		//! tools menu
#ifndef KEXI_NO_MIGRATION
		KAction *action_tools_data_migration;
#endif

		//! window menu
		KAction *action_window_next, *action_window_previous;

		//! settings menu
		KAction *action_configure;

		//! for dock windows
		KMdiToolViewAccessor* navToolWindow;
		KMdiToolViewAccessor* propEditorToolWindow;

		QGuardedPtr<QWidget> focus_before_popup;
//		KexiRelationPart *relationPart;

		int privateIDCounter; //!< counter: ID for private "document" like Relations window

		bool block_KMdiMainFrm_eventFilter : 1;

		//! Set to true only in destructor, used by closeDialog() to know if
		//! user can cancel dialog closing. If true user even doesn't see any messages
		//! before closing a dialog. This is for extremely sanity... and shouldn't be even needed.
		bool forceDialogClosing : 1;

		//! Indicates that we're inside closeDialog() method - to avoid inf. recursion
		//! on dialog removing
		bool insideCloseDialog : 1;

		//! Used in several places to show info dialog at startup (only once per session)
		//! before displaying other stuff
		bool showImportantInfoOnStartup : 1;

//		//! Used sometimes to block showErrorMessage()
//		bool disableErrorMessages : 1;

		//! Indicates if project is started in --final mode
		bool final : 1;

		//! Used on opening 1st child window
		bool maximizeFirstOpenedChildFrm : 1;

	Private(KexiMainWindowImpl* w)
		: dialogs(401)
		, wnd(w)
	{
		propEditorToolWindow=0;
		final = false;
		nav=0;
		navToolWindow=0;
		prj = 0;
		curDialogGUIClient=0;
		curDialogViewGUIClient=0;
		closedDialogGUIClient=0;
		closedDialogViewGUIClient=0;
		nameDialog=0;
		curDialog=0;
		block_KMdiMainFrm_eventFilter=false;
		focus_before_popup=0;
//		relationPart=0;
		privateIDCounter=0;
		action_view_nav=0;
		action_view_propeditor=0;
		forceDialogClosing=false;
		insideCloseDialog=false;
		createMenu=0;
		showImportantInfoOnStartup=true;
//		disableErrorMessages=false;
//		last_checked_mode=0;
		propEditorDockSeparatorPos=-1;
#ifndef KEXI_SHOW_UNIMPLEMENTED
		dummy_action = new KActionMenu("", wnd);
#endif
		maximizeFirstOpenedChildFrm = false;
	}
	~Private() {
	}

	KexiMainWindowImpl *wnd;

	/*! Toggles last checked view mode radio action, if available. */
	void toggleLastCheckedMode()
	{
		if (curDialog.isNull())
			return;
		KRadioAction *ra = actions_for_view_modes[ curDialog->currentViewMode() ];
		if (ra)
			ra->setChecked(true);
//		if (!last_checked_mode)
//			return;
//		last_checked_mode->setChecked(true);
	}

	int propEditorDockSeparatorPos;

	void updatePropEditorDockWidthInfo() {
		if (propEditor) {
			KDockWidget *dw = (KDockWidget *)propEditor->parentWidget();
#if defined(KDOCKWIDGET_P)
			KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
			if (ds)
				propEditorDockSeparatorPos = ds->separatorPos();
#endif
		}
	}

	void showStartProcessMsg(const QStringList& args)
	{
		wnd->showErrorMessage(i18n("Could not start %1 application.").arg(KEXI_APP_NAME), 
			i18n("Command \"%1\" failed.").arg(args.join(" ")));
	}

	void hideMenuItem(const QString& menuName, const QString& itemText, bool alsoSeparator)
	{
		QPopupMenu *pm = popups[menuName.ascii()];
		if (!pm)
			return;
		uint i=0;
		const uint c = pm->count();
		for (;i<c;i++) {
			kdDebug() << pm->text( pm->idAt(i) ) <<endl;
			if (pm->text( pm->idAt(i) ).lower().stripWhiteSpace()==itemText.lower().stripWhiteSpace())
				break;
		}
		if (i<c) {
			pm->setItemVisible( pm->idAt(i), false );
			if (alsoSeparator)
				pm->setItemVisible( pm->idAt(i+1), false ); //also separator
		}
	}

	void updatePropEditorVisibility(int viewMode)
	{
		if (propEditorToolWindow) {
			if (viewMode==0 || viewMode==Kexi::DataViewMode) {
#ifdef PROPEDITOR_VISIBILITY_CHANGES
				propEditorToolWindow->hide();
#endif
			} else {
				propEditorToolWindow->show();
			}
		}
	}
};

//-------------------------------------------------

KexiMainWindowImpl::KexiMainWindowImpl()
 : KexiMainWindow()
 , KexiGUIMessageHandler(this)
 , d(new KexiMainWindowImpl::Private(this) )
{
	KexiProjectData *pdata = Kexi::startupHandler().projectData();
	d->final = Kexi::startupHandler().forcedFinalMode() /* <-- simply forced final mode */
		/* project has 'final mode' set as default and not 'design mode' override is found: */
		|| (pdata && pdata->finalMode() && !Kexi::startupHandler().forcedDesignMode()); 

	if(d->final)
		kdDebug() << "KexiMainWindowImpl::KexiMainWindowImpl(): starting up in final mode" << endl;

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
	setStandardMDIMenuEnabled(false);
	setAsDefaultHost(); //this is default host now.
	KGlobal::iconLoader()->addAppDir("kexi");

	//get informed
	connect(&Kexi::partManager(),SIGNAL(partLoaded(KexiPart::Part*)),this,SLOT(slotPartLoaded(KexiPart::Part*)));
	connect( m_pMdi, SIGNAL(nowMaximized(bool)), this, SLOT(slotCaptionForCurrentMDIChild(bool)) );
	connect( m_pMdi, SIGNAL(noMaximizedChildFrmLeft(KMdiChildFrm*)), this, SLOT(slotNoMaximizedChildFrmLeft(KMdiChildFrm*)));
	connect( this, SIGNAL(lastChildFrmClosed()), this, SLOT(slotLastChildFrmClosed()));
//no such signal	connect( m_pMdi, SIGNAL(lastChildViewClosed()), this, SLOT(slotLastChildViewClosed()));

	connect( this, SIGNAL(childViewIsDetachedNow(QWidget*)), this, SLOT(slotChildViewIsDetachedNow(QWidget*)));
	connect( this, SIGNAL(mdiModeHasBeenChangedTo(KMdi::MdiMode)),
		this, SLOT(slotMdiModeHasBeenChangedTo(KMdi::MdiMode)));


	if(!d->final)
	{
		setXMLFile("kexiui.rc");
		setAcceptDrops(true);
		initActions();
		createShellGUI(true);
	}

	(void) new KexiStatusBar(this, "status_bar");

	d->origAppCaption = caption();

	restoreSettings();

	if(!d->final)
	{
		initContextHelp();
		initPropertyEditor();
	}

	{//store menu popups list
		QObjectList *l = queryList( "QPopupMenu" );
		for (QObjectListIt it( *l ); it.current(); ++it ) {
			kdDebug() << "name=" <<it.current()->name() << " cname="<<it.current()->className()<<endl;
			//KexiMainWindowImpl::eventFilter() will filter our popups:
			it.current()->installEventFilter(this);
			d->popups.insert(it.current()->name(), static_cast<QPopupMenu*>(it.current()));
		}
		delete l;
		d->createMenu = d->popups["create"];
	}

	//fix menus a bit more:
#ifndef KEXI_SHOW_UNIMPLEMENTED
//disabled (possible crash)	d->hideMenuItem("file", i18n("&Import"), true);
//disabled (possible crash)	d->hideMenuItem("help", i18n( "&Report Bug..." ), true);
#endif

	if (!isFakingSDIApplication() && !d->final) {
//		QPopupMenu *menu = (QPopupMenu*) child( "window", "KPopupMenu" );
		QPopupMenu *menu = d->popups["window"];
		unsigned int count = menuBar()->count();
		if (menu)
			setWindowMenu(menu);
		else
			menuBar()->insertItem( i18n("&Window"), windowMenu(), -1, count-2); // standard position is left to the last ('Help')
	}

	m_pTaskBar->setCaption(i18n("Task Bar"));	//js TODO: move this to KMDIlib

	if (!d->final) {
		invalidateActions();
		d->timer.singleShot(0,this,SLOT(slotLastActions()));
	}
}

KexiMainWindowImpl::~KexiMainWindowImpl()
{
	d->forceDialogClosing=true;
	closeProject();
	delete d;
}

KexiProject	*KexiMainWindowImpl::project()
{
	return d->prj;
}

void KexiMainWindowImpl::setWindowMenu(QPopupMenu *menu)
{
	delete m_pWindowMenu;
	m_pWindowMenu = menu;
	int count = menuBar()->count();
	//try to move "window" menu just before "Settings" menu (count-3)
	const QString txt = i18n("&Window");
	int i;
	for (i=0; i<count; i++) {
		kdDebug() << menuBar()->text( menuBar()->idAt(i) ) << endl;
		if (txt==menuBar()->text( menuBar()->idAt(i) ))
			break;
	}
	if (i<count) {
		const int id = menuBar()->idAt(i);
		menuBar()->removeItemAt(i);
		menuBar()->insertItem(txt, m_pWindowMenu, id, count-3);
	}
	m_pWindowMenu->setCheckable(TRUE);
	QObject::connect( m_pWindowMenu, SIGNAL(aboutToShow()), this, SLOT(fillWindowMenu()) );
}

void KexiMainWindowImpl::fillWindowMenu()
{
	KexiMainWindow::fillWindowMenu();

	//insert window_next, window_previous actions:
//	const QString t = i18n("&Dock/Undock...");
	int i = m_pWindowMenu->count()-1;
	for (int index;; i--) {
		index = m_pWindowMenu->idAt(i);
		if (index==-1 || m_pWindowMenu->text(index).isNull())
				break;
	}
	i++;
	d->action_window_next->plug( m_pWindowMenu, i++ );
	d->action_window_previous->plug( m_pWindowMenu, i++ );
	if (!m_pDocumentViews->isEmpty())
		m_pWindowMenu->insertSeparator( i++ );
}

QPopupMenu* KexiMainWindowImpl::findPopupMenu(const char *popupName)
{
	return d->popups[popupName];
}

void
KexiMainWindowImpl::initActions()
{
//	setupGUI(KMainWindow::Keys|KMainWindow::StatusBar|KMainWindow::Save|KMainWindow::Create);

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

#ifdef KEXI_SHOW_UNIMPLEMENTED
	d->action_open_recent = new KActionMenu(i18n("Open Recent"),
		actionCollection(), "project_open_recent");
	connect(d->action_open_recent->popupMenu(),SIGNAL(activated(int)),this,SLOT(slotProjectOpenRecent(int)));
	connect(d->action_open_recent->popupMenu(), SIGNAL(aboutToShow()),this,SLOT(slotProjectOpenRecentAboutToShow()));
	d->action_open_recent->popupMenu()->insertSeparator();
	d->action_open_recent_more_id = d->action_open_recent->popupMenu()
		->insertItem(i18n("&More Projects..."), this, SLOT(slotProjectOpenRecentMore()), 0, 1000);
#else
	d->action_open_recent = d->dummy_action;
#endif

	d->action_save = KStdAction::save( this, SLOT( slotProjectSave() ), actionCollection(), "project_save" );
//	d->action_save = new KAction(i18n("&Save"), "filesave", KStdAccel::shortcut(KStdAccel::Save),
//		this, SLOT(slotProjectSave()), actionCollection(), "project_save");
	d->action_save->setToolTip(i18n("Save object changes"));
	d->action_save->setWhatsThis(i18n("Saves object changes from currently selected window."));

#ifdef KEXI_SHOW_UNIMPLEMENTED
	d->action_save_as = new KAction(i18n("Save &As..."), "filesaveas", 0,
		this, SLOT(slotProjectSaveAs()), actionCollection(), "project_saveas");
	d->action_save_as->setToolTip(i18n("Save object as"));
	d->action_save_as->setWhatsThis(i18n("Saves object changes from currently selected window under a new name (within the same project)."));

	d->action_project_properties = new KAction(i18n("Project Properties"), "info", 0,
		this, SLOT(slotProjectProperties()), actionCollection(), "project_properties");
#else
	d->action_save_as = d->dummy_action;
	d->action_project_properties = d->dummy_action;
#endif

	d->action_close = new KAction(i18n("&Close Project"), "fileclose", 0,
		this, SLOT(slotProjectClose()), actionCollection(), "project_close" );
	d->action_close->setToolTip(i18n("Close the current project"));
	d->action_close->setWhatsThis(i18n("Closes the current project."));

	KStdAction::quit( this, SLOT(slotQuit()), actionCollection(), "quit");

#ifdef KEXI_SHOW_UNIMPLEMENTED
	d->action_project_relations = new KAction(i18n("&Relationships..."), "relation", CTRL + Key_R,
		this, SLOT(slotProjectRelations()), actionCollection(), "project_relations");
	d->action_project_relations->setToolTip(i18n("Project relationships"));
	d->action_project_relations->setWhatsThis(i18n("Shows project relationships."));

//TODO	new KAction(i18n("From File..."), "fileopen", 0,
//TODO		this, SLOT(slotImportFile()), actionCollection(), "project_import_file");
//TODO	new KAction(i18n("From Server..."), "server", 0,
//TODO		this, SLOT(slotImportServer()), actionCollection(), "project_import_server");
#else
	d->action_project_relations = d->dummy_action;
#endif
#ifndef KEXI_NO_MIGRATION
	d->action_tools_data_migration = new KAction(i18n("Project..."), "", 0, this, SLOT(slotImportProject()), actionCollection(), "project_import_project");
#endif	

	//EDIT MENU
	d->action_edit_cut = createSharedAction( KStdAction::Cut, "edit_cut");
	d->action_edit_copy = createSharedAction( KStdAction::Copy, "edit_copy");
	d->action_edit_paste = createSharedAction( KStdAction::Paste, "edit_paste");

	d->action_edit_undo = createSharedAction( KStdAction::Undo, "edit_undo");
	d->action_edit_redo = createSharedAction( KStdAction::Redo, "edit_redo");

	d->action_edit_delete = createSharedAction(i18n("&Delete"), "editdelete", 
		0/*Key_Delete*/, "edit_delete");
	d->action_edit_delete->setToolTip(i18n("Delete object"));
	d->action_edit_delete->setWhatsThis(i18n("Deletes currently selected object."));

	d->action_edit_delete_row = createSharedAction(i18n("Delete Row"), "delete_table_row",
		CTRL+Key_Delete, "edit_delete_row");
	d->action_edit_delete_row->setToolTip(i18n("Delete currently selected row from a table"));
	d->action_edit_delete_row->setWhatsThis(i18n("Deletes currently selected row from a table."));

	d->action_edit_clear_table = createSharedAction(i18n("Clear Table Contents"), "clear_table_contents",
		0, "edit_clear_table");
	d->action_edit_clear_table->setToolTip(i18n("Clear table contents"));
	d->action_edit_clear_table->setWhatsThis(i18n("Clears table contents."));
	setActionVolatile( d->action_edit_clear_table, true );

	d->action_edit_edititem = createSharedAction(i18n("Edit Item"), 0, Key_F2, "edit_edititem");
	d->action_edit_edititem->setToolTip(i18n("Edit currently selected item"));
	d->action_edit_edititem->setWhatsThis(i18n("Edits currently selected item."));

	d->action_edit_insert_empty_row = createSharedAction(i18n("&Insert Empty Row"), "insert_table_row", SHIFT | CTRL | Key_Insert, "edit_insert_empty_row");
	setActionVolatile( d->action_edit_insert_empty_row, true );
	d->action_edit_insert_empty_row->setToolTip(i18n("Insert one empty row above"));
	d->action_edit_insert_empty_row->setWhatsThis(i18n("Inserts one empty row above currently selected table row."));

	//VIEW MENU
	d->action_view_data_mode = new KRadioAction(i18n("&Data View"), "state_data", Key_F6,
		this, SLOT(slotViewDataMode()), actionCollection(), "view_data_mode");
	d->actions_for_view_modes.insert( Kexi::DataViewMode, d->action_view_data_mode );
	d->action_view_data_mode->setExclusiveGroup("view_mode");
	d->action_view_data_mode->setToolTip(i18n("Switch to Data View mode"));
	d->action_view_data_mode->setWhatsThis(i18n("Switches to Data View mode."));

	d->action_view_design_mode = new KRadioAction(i18n("D&esign View"), "state_edit", Key_F7,
		this, SLOT(slotViewDesignMode()), actionCollection(), "view_design_mode");
	d->actions_for_view_modes.insert( Kexi::DesignViewMode, d->action_view_design_mode );
	d->action_view_design_mode->setExclusiveGroup("view_mode");
	d->action_view_design_mode->setToolTip(i18n("Switch to Design View mode"));
	d->action_view_design_mode->setWhatsThis(i18n("Switches to Design View mode."));

	d->action_view_text_mode = new KRadioAction(i18n("&Text View"), "state_sql", Key_F8,
		this, SLOT(slotViewTextMode()), actionCollection(), "view_text_mode");
	d->actions_for_view_modes.insert( Kexi::TextViewMode, d->action_view_text_mode );
	d->action_view_text_mode->setExclusiveGroup("view_mode");
	d->action_view_text_mode->setToolTip(i18n("Switch to Text View mode"));
	d->action_view_text_mode->setWhatsThis(i18n("Switches to Text View mode."));

	d->action_view_nav = new KAction(i18n("Project Navigator"), "", ALT + Key_1,
		this, SLOT(slotViewNavigator()), actionCollection(), "view_navigator");
	d->action_view_nav->setToolTip(i18n("Go to Project navigator panel"));
	d->action_view_nav->setWhatsThis(i18n("Goes to Project navigator panel."));

#ifdef KEXI_PROP_EDITOR
	d->action_view_propeditor = new KAction(i18n("Property Editor"), "", ALT + Key_2,
		this, SLOT(slotViewPropertyEditor()), actionCollection(), "view_propeditor");
	d->action_view_propeditor->setToolTip(i18n("Go to Property editor panel"));
	d->action_view_propeditor->setWhatsThis(i18n("Goes to Property editor panel."));
#endif

	//DATA MENU
	d->action_data_save_row = createSharedAction(i18n("&Save Row"), "button_ok", SHIFT | Key_Return, "data_save_row");
	d->action_data_save_row->setToolTip(i18n("Save currently selected table row's data"));
	d->action_data_save_row->setWhatsThis(i18n("Saves currently selected table row's data."));
	setActionVolatile( d->action_data_save_row, true );

	d->action_data_cancel_row_changes = createSharedAction(i18n("&Cancel Row Changes"), "button_cancel", 0 , "data_cancel_row_changes");
	d->action_data_cancel_row_changes->setToolTip(i18n("Cancel changes made to currently selected table row"));
	d->action_data_cancel_row_changes->setWhatsThis(i18n("Cancels changes made to currently selected table row."));
	setActionVolatile( d->action_data_cancel_row_changes, true );

	action = createSharedAction(i18n("&Filter"), "filter", 0, "data_filter");
	setActionVolatile( action, true );
//	action->setToolTip(i18n("")); //todo
//	action->setWhatsThis(i18n("")); //todo

//	setSharedMenu("data_sort");
	action = createSharedAction(i18n("&Ascending"), "sort_az", 0, "data_sort_az");
	setActionVolatile( action, true );
	action->setToolTip(i18n("Sort data in ascending order"));
	action->setWhatsThis(i18n("Sorts data in ascending order (from A to Z and from 0 to 9). Data from selected column is used for sorting."));

	action = createSharedAction(i18n("&Descending"), "sort_za", 0, "data_sort_za");
	setActionVolatile( action, true );
	action->setToolTip(i18n("Sort data in descending order"));
	action->setWhatsThis(i18n("Sorts data in descending (from Z to A and from 9 to 0). Data from selected column is used for sorting."));

	//TOOLS MENU

	//additional 'Window' menu items
	d->action_window_next = new KAction( i18n("&Next Window"), "", 
#ifdef Q_WS_WIN
		CTRL+Key_Tab,
#else
		ALT+Key_Right,
#endif
		this, SLOT(activateNextWin()), actionCollection(), "window_next");
	d->action_window_next->setToolTip( i18n("Next window") );
	d->action_window_next->setWhatsThis(i18n("Switches to the next window."));

	d->action_window_previous = new KAction( i18n("&Previous Window"), "", 
#ifdef Q_WS_WIN
		CTRL+SHIFT+Key_Tab,
#else
		ALT+Key_Left,
#endif
		this, SLOT(activatePrevWin()), actionCollection(), "window_previous");
	d->action_window_previous->setToolTip( i18n("Previous window") );
	d->action_window_previous->setWhatsThis(i18n("Switches to the previous window."));

	//SETTINGS MENU
	setStandardToolBarMenuEnabled( true );
	action = KStdAction::keyBindings(this, SLOT( slotConfigureKeys() ), actionCollection() );
	action->setWhatsThis(i18n("Lets you configure shortcut keys."));

#ifdef KEXI_SHOW_UNIMPLEMENTED
	action = KStdAction::configureToolbars( this, SLOT( slotConfigureToolbars() ), actionCollection() );
	action->setWhatsThis(i18n("Lets you configure toolbars."));
#endif

	d->action_show_other = new KActionMenu(i18n("Other"),
		actionCollection(), "options_show_other");
#ifndef KEXI_NO_CTXT_HELP
	d->action_show_helper = new KToggleAction(i18n("Show Context Help"), "", CTRL + Key_H,
	 actionCollection(), "options_show_contexthelp");
#if KDE_IS_VERSION(3,2,90)
	d->action_show_helper->setCheckedState(i18n("Hide Context Help"));
#endif
#endif

#ifdef KEXI_FORMS_SUPPORT
	slotOptionsEnableForms(true, true);
#else
	slotOptionsEnableForms(false, true);

# if 0
	KToggleAction *toggleaction = new KToggleAction(i18n("Enable Forms"), "", 0, actionCollection(), "options_enable_forms");
#  if KDE_IS_VERSION(3,2,90)
	toggleaction->setCheckedState(i18n("Disable Forms"));
#  endif
	d->config->setGroup("Unfinished");
	if (d->config->readBoolEntry("EnableForms", false)) {
		slotOptionsEnableForms( true, true );
		toggleaction->setChecked(true);
	}
	connect(toggleaction, SIGNAL(toggled(bool)), this, SLOT(slotOptionsEnableForms(bool)));
# endif //0
#endif

#ifdef KEXI_REPORTS_SUPPORT
	Kexi::tempShowReports() = true;
#else
	Kexi::tempShowReports() = false;
#endif

#ifdef KEXI_SHOW_UNIMPLEMENTED
	d->action_configure = KStdAction::preferences(this, SLOT(slotShowSettings()), actionCollection());
	action->setWhatsThis(i18n("Lets you configure Kexi."));
#endif

	//HELP MENU
#if 0//js: todo reenable later
	KStdAction::tipOfDay( this, SLOT( slotTipOfTheDayAction() ), actionCollection() )
		->setWhatsThis(i18n("This shows useful tips on the use of this application."));
#endif
#if 0 //we don't have a time for updating info text for each new version
	new KAction(i18n("Important Information"), "messagebox_info", 0,
		this, SLOT(slotImportantInfo()), actionCollection(), "help_show_important_info");
#endif
//TODO: UNCOMMENT TO REMOVE MDI MODES SETTING	m_pMdiModeMenu->hide();

//	KAction *actionSettings = new KAction(i18n("Configure Kexi..."), "configure", 0,
//	 actionCollection(), "kexi_settings");
//	actionSettings->setWhatsThis(i18n("Lets you configure Kexi."));
//	connect(actionSettings, SIGNAL(activated()), this, SLOT(slotShowSettings()));
}

void KexiMainWindowImpl::invalidateActions()
{
	invalidateProjectWideActions();
	invalidateSharedActions();
}

void KexiMainWindowImpl::invalidateSharedActions(QObject *o)
{
	//TODO: enabling is more complex...
/*	d->action_edit_cut->setEnabled(true);
	d->action_edit_copy->setEnabled(true);
	d->action_edit_paste->setEnabled(true);*/

	if (!o)
		o = focusWindow();
	KexiSharedActionHost::invalidateSharedActions(o);
}

void KexiMainWindowImpl::invalidateSharedActions()
{
	invalidateSharedActions(0);
}

// unused, I think
void KexiMainWindowImpl::invalidateSharedActionsLater()
{
	QTimer::singleShot(1, this, SLOT(invalidateSharedActions()));
}

void KexiMainWindowImpl::invalidateProjectWideActions()
{
//	stateChanged("project_opened",d->prj ? StateNoReverse : StateReverse);

	if(d->final)
		return;

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

	//TOOLS MENU
#ifndef KEXI_NO_MIGRATION
	d->action_tools_data_migration->setEnabled(d->prj);
#endif

	//WINDOW MENU
	if (d->action_window_next) {
		d->action_window_next->setEnabled(!m_pDocumentViews->isEmpty());
		d->action_window_previous->setEnabled(!m_pDocumentViews->isEmpty());
	}

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

tristate KexiMainWindowImpl::startup()
{
	switch (Kexi::startupHandler().action()) {
	case KexiStartupHandler::CreateBlankProject:
		return createBlankProject();
	case KexiStartupHandler::UseTemplate:
		return cancelled;
		//TODO
		break;
	case KexiStartupHandler::OpenProject:
		if (!openProject(Kexi::startupHandler().projectData())) {
			if (d->final)
				return false;
		}
		break;
	default:;
	}
	return true;
}

static QString internalReason(KexiDB::Object *obj)
{
	const QString &s = obj->errorMsg();
	if (s.isEmpty())
		return s;
	return QString("<br>(%1) ").arg(i18n("reason:")+" <i>"+s+"</i>");
}

bool KexiMainWindowImpl::openProject(KexiProjectData *projectData)
{
	if (!projectData)
		return false;
	if (d->final) {
		//TODO: maybe also auto allow to open objects...
		return initFinalMode(projectData);
	}
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

//	d->disableErrorMessages = true;
	enableMessages( false );

	QString not_found_msg;
	//ok, now open "autoopen: objects
	for (QValueList<KexiProjectData::ObjectInfo>::ConstIterator it = projectData->autoopenObjects.constBegin();
		it != projectData->autoopenObjects.constEnd(); ++it )
	{
		KexiProjectData::ObjectInfo info = *it;
		KexiPart::Info *i = Kexi::partManager().info( QCString("kexi/")+info["type"].lower().latin1() );
		if (!i) {
			not_found_msg += "<li>";
			if (!info["name"].isEmpty())
				not_found_msg += (QString("\"") + info["name"] + "\" - ");
			if (info["action"]=="new")
				not_found_msg += i18n("cannot create object - ");
			not_found_msg += (i18n("unknown object type \"%1\"").arg(info["type"])+
				internalReason(&Kexi::partManager())+"<br></li>");
			continue;
		}
		if (info["action"]=="new") {
			if (!newObject( i )) {
				not_found_msg += "<li>";
				not_found_msg += (i18n("cannot create object of type \"%1\"").arg(info["type"])+
					internalReason(d->prj)+"<br></li>");
			}
			continue;
		}

		KexiPart::Item *item = d->prj->item(i, info["name"]);

		if (!item) {
			not_found_msg += "<li>";
			not_found_msg += ( QString("<li>\"")+ info["name"] + "\" - " + i18n("object not found")+
				internalReason(d->prj)+"<br></li>" );
			continue;
		}
		int viewMode;
		if (info["action"]=="design")
			viewMode = Kexi::DesignViewMode;
		else if (info["action"]=="edittext")
			viewMode = Kexi::TextViewMode;
		else
			viewMode = Kexi::DataViewMode;
		if (!openObject(item, viewMode)) {
			not_found_msg += "<li>";
			not_found_msg += ( QString("<li>\"")+ info["name"] + "\" - " + i18n("cannot open object")+
				internalReason(d->prj)+"<br></li>" );
			continue;
		}
	}
	enableMessages( true );
//	d->disableErrorMessages = false;

	if (!not_found_msg.isEmpty())
		showErrorMessage(i18n("You have requested selected objects to be opened automatically on startup. Several objects cannot be opened."),
			QString("<ul>%1</ul>").arg(not_found_msg) );

	d->updatePropEditorVisibility(d->curDialog ? d->curDialog->currentViewMode() : 0);
#ifndef PROPEDITOR_VISIBILITY_CHANGES
	if (!d->curDialog)
		d->propEditorToolWindow->hide();
#endif

	updateAppCaption();

//	d->navToolWindow->wrapperWidget()->setFixedWidth(200);
//js TODO: make visible FOR OTHER MODES if needed
	if (mdiMode()==KMdi::ChildframeMode) {
		//make docks visible again
		if (!d->navToolWindow->wrapperWidget()->isVisible())
			static_cast<KDockWidget*>(d->navToolWindow->wrapperWidget())->makeDockVisible();
//		if (!d->propEditorToolWindow->wrapperWidget()->isVisible())
//			static_cast<KDockWidget*>(d->propEditorToolWindow->wrapperWidget())->makeDockVisible();
	}
	return true;
}

tristate KexiMainWindowImpl::closeProject()
{
	if (!d->prj)
		return true;

	//close each window, optionally asking if user wants to close (if data changed)
	while (!d->curDialog.isNull()) {
		tristate res = closeDialog( d->curDialog );
		if (!res || ~res)
			return res;
	}

	if(d->nav)
	{
		d->nav->clear();
		d->navToolWindow->hide();
	}

	if(d->propEditorToolWindow)
		d->propEditorToolWindow->hide();

	d->dialogs.clear(); //sanity!
	delete d->prj;
	d->prj=0;

//	Kexi::partManager().unloadAllParts();

	invalidateActions();
	if(!d->final)
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
		connect(d->nav,SIGNAL(renameItem(KexiPart::Item*,const QString&, bool&)),
			this,SLOT(renameObject(KexiPart::Item*,const QString&, bool&)));
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
			if (!it->addTree())
				continue;
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
	if (mdiMode()==KMdi::ChildframeMode) {
		KDockWidget *dw = (KDockWidget *)d->propEditor->parentWidget();
		KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
		Q_UNUSED(ds);
//1		ds->resize(ds->width()*3, ds->height());
//1		ds->setSeparatorPos(30, true);
//1		ds->setForcedFixedWidth( dw, 200 );
	}
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

	d->config->setGroup("PropertyEditor");
	int size = d->config->readNumEntry("FontSize", -1);
	QFont f(d->propEditor->font());
	if (size<0) {
		//this gives:
		// -2/3 of base font size (6 point minimum)
		// if the current screen width is > 1100, +1 point is added to every 100 points greater than 1300
		// for resolutions below 1100 in width, 7 is the minimum
		// maximum size is the base size
		const int wdth = KGlobalSettings::desktopGeometry(this).width();
		size = QMAX( 6 + QMAX(0, wdth - 1100) / 100 , f.pointSize()*2/3 );
		if (wdth<1100)
			size = QMAX( size, 7 );
		size = QMIN( size, f.pointSize() );
	}
	f.setPointSize( size );
	d->propEditor->setFont(f);

	if (mdiMode()==KMdi::ChildframeMode) {
	KDockWidget *dw = (KDockWidget *)d->propEditor->parentWidget();
#if defined(KDOCKWIDGET_P)
		KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
		ds->show();
	//	ds->resize(400, ds->height());
//		ds->setSeparatorPos(400, true);
//		ds->setForcedFixedWidth( dw, 400 );
//		ds->setSeparatorPos(600, true);

		
		d->config->setGroup("MainWindow");
		ds->setSeparatorPos(d->config->readNumEntry("RightDockPosition", 80/* % */), true);
//1		ds->setForcedFixedWidth( dw, 600 );
	//	ds->resize(400, ds->height());
	//	dw->resize(400, dw->height());
#endif

//1		dw->setMinimumWidth(200);

//	ds->setSeparatorPos(d->propEditor->sizeHint().width(), true);

		//heh, this is for IDEAl only, I suppose?
		if (m_rightContainer) {
			m_rightContainer->setForcedFixedWidth( 400 );
		}
	}
#endif

//	int w = d->propEditor->width();
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
	p->createGUIClients(this);
}

//! internal
void KexiMainWindowImpl::slotCaptionForCurrentMDIChild(bool childrenMaximized)
{
	//js todo: allow to set custom "static" app caption

	KMdiChildView *view = 0L;
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
			+ (d->appCaptionPrefix.isEmpty() ? QString::null : (QString::fromLatin1(" - ") + d->appCaptionPrefix)) );
	}
	else {
		setCaption( (d->appCaptionPrefix.isEmpty() ? QString::null : (d->appCaptionPrefix + QString::fromLatin1(" - ")))
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
//	if (!d->appCaptionPrefix.isEmpty())
//		d->appCaptionPrefix = d->appCaptionPrefix;

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
	const tristate res = closeProject();
	if (~res)
		return false;
	
	if (res)
		storeSettings();

	return ! ~res;
}

bool
KexiMainWindowImpl::queryExit()
{
//	storeSettings();
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

	d->config->setGroup("MainWindow");
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

	// restore a possible maximized Childframe mode, 
	// will be used in KexiMainWindowImpl::addWindow()
	d->maximizeFirstOpenedChildFrm = d->config->readBoolEntry("maximized childframes", true);
	setEnableMaximizedChildFrmMode(d->maximizeFirstOpenedChildFrm);

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
	d->config->setGroup("MainWindow");
	d->config->writeEntry("MDIMode", mdiMode());
//	config->sync();
	d->config->writeEntry("maximized childframes", isInMaximizedChildFrmMode());

	if (mdiMode()==KMdi::ChildframeMode) {
		if (d->propEditorDockSeparatorPos > 0 && d->propEditorDockSeparatorPos <= 100) {
			d->config->setGroup("MainWindow");
			d->config->writeEntry("RightDockPosition", d->propEditorDockSeparatorPos);
		}
	}

	d->config->setGroup("PropertyEditor");
	d->config->writeEntry("FontSize", d->propEditor->font().pointSize());
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

#if KDE_IS_VERSION(3,1,9) && !defined(Q_WS_WIN)
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
	if(dlg->id() != -1)
		d->dialogs.insert(dlg->id(), dlg);
	kdDebug() << "KexiMainWindowImpl::registerChild() ID = " << dlg->id() << endl;

	if (m_mdiMode==KMdi::ToplevelMode || m_mdiMode==KMdi::ChildframeMode) {//kmdi fix
		//js TODO: check if taskbar is switched in menu
		if (m_pTaskBar && !m_pTaskBar->isSwitchedOn())
			m_pTaskBar->switchOn(true);
	}
	//KMdiChildFrm *frm = dlg->mdiParent();
	//if (frm) {
//		dlg->setMargin(20);
		//dlg->setLineWidth(20);
	//}
}

void
KexiMainWindowImpl::updateDialogViewGUIClient(KXMLGUIClient *viewClient)
{
	if (viewClient!=d->curDialogViewGUIClient) {
		//view clients differ
		kdDebug()<<"KexiMainWindowImpl::activeWindowChanged(): old view gui client:"
			<<(d->curDialogViewGUIClient ? d->curDialogViewGUIClient->xmlFile() : "")
			<<" new view gui client: "<<( viewClient ? viewClient->xmlFile() : "") <<endl;
		if (d->curDialogViewGUIClient) {
			guiFactory()->removeClient(d->curDialogViewGUIClient);
		}
		if (viewClient) {
			if (d->closedDialogViewGUIClient) {
				//ooh, there is a client which dialog is already closed -- BUT it is the same client as our
				//so: give up
			}
			else {
				guiFactory()->addClient(viewClient);
			}
		}
	}
}

void
KexiMainWindowImpl::activeWindowChanged(KMdiChildView *v)
{
	KexiDialogBase *dlg = static_cast<KexiDialogBase *>(v);
	kdDebug() << "KexiMainWindowImpl::activeWindowChanged() to = " << (dlg ? dlg->caption() : "<none>") << endl;

	KXMLGUIClient *client=0; //common for all views
	KXMLGUIClient *viewClient=0; //specific for current dialog's view

	if (!dlg)
		client=0;
	else if ( dlg->isRegistered()) {
//		client=dlg->guiClient();
		client=dlg->commonGUIClient();
		viewClient=dlg->guiClient();
		if (d->closedDialogGUIClient) {
			if (client!=d->closedDialogGUIClient) {
				//ooh, there is a client which dialog is already closed -- and we don't want it
				guiFactory()->removeClient(d->closedDialogGUIClient);
				d->closedDialogGUIClient=0;
			}
		}
		if (d->closedDialogViewGUIClient) {
			if (viewClient!=d->closedDialogViewGUIClient) {
				//ooh, there is a client which dialog is already closed -- and we don't want it
				guiFactory()->removeClient(d->closedDialogViewGUIClient);
				d->closedDialogViewGUIClient=0;
			}
		}
		if (client!=d->curDialogGUIClient) {
			//clients differ
			kdDebug()<<"KexiMainWindowImpl::activeWindowChanged(): old gui client:"
				<<(d->curDialogGUIClient ? d->curDialogGUIClient->xmlFile() : "")
				<<" new gui client: "<<( client ? client->xmlFile() : "") <<endl;
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
			//clients are the same
			if ((KexiDialogBase*)d->curDialog!=dlg) {
				if (d->curDialog)
					d->curDialog->detachFromGUIClient();
				if (dlg)
					dlg->attachToGUIClient();
			}
		}
		updateDialogViewGUIClient(viewClient);
/*		if (viewClient!=d->curDialogViewGUIClient) {
			//view clients differ
			kdDebug()<<"KexiMainWindowImpl::activeWindowChanged(): old view gui client:"
				<<d->curDialogViewGUIClient<<" new view gui client: "<<viewClient<<endl;
			if (d->curDialogViewGUIClient) {
				guiFactory()->removeClient(d->curDialogViewGUIClient);
			}
			if (viewClient) {
				if (d->closedDialogViewGUIClient) {
					//ooh, there is a client which dialog is already closed -- BUT it is the same client as our
					//so: give up
				}
				else {
					guiFactory()->addClient(viewClient);
				}
			}
		}*/
	}
	bool update_dlg_caption = dlg && dlg!=(KexiDialogBase*)d->curDialog && dlg->mdiParent();

	if (d->curDialogGUIClient && !client)
		guiFactory()->removeClient(d->curDialogGUIClient);
	d->curDialogGUIClient=client;

	if (d->curDialogViewGUIClient && !viewClient)
		guiFactory()->removeClient(d->curDialogViewGUIClient);
	d->curDialogViewGUIClient=viewClient;

	bool dialogChanged = ((KexiDialogBase*)d->curDialog)!=dlg;

	if (dialogChanged) {
		if (d->curDialog) {
			//inform previously activated dialog about deactivation
			d->curDialog->deactivate();
		}
	}
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
		if (d->curDialog && d->curDialog->currentViewMode()!=0) //on opening new dialog it can be 0; we don't want this
			d->updatePropEditorVisibility(d->curDialog->currentViewMode());
	}

	//update caption...
	if (update_dlg_caption) {//d->curDialog is != null for sure
		slotCaptionForCurrentMDIChild(d->curDialog->mdiParent()->state()==KMdiChildFrm::Maximized);
	}
//	if (!d->curDialog.isNull())
//		d->last_checked_mode = d->actions_for_view_modes[ d->curDialog->currentViewMode() ];
	invalidateViewModeActions();
	invalidateActions();
	if (dlg)
		dlg->setFocus();
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

	d->focus_before_popup = dlg;
	dlg->activate();
	return true;
}

void
KexiMainWindowImpl::childClosed(KMdiChildView *v)
{
	KexiDialogBase *dlg = static_cast<KexiDialogBase *>(v);
	d->dialogs.remove(dlg->id());

	//focus navigator if nothing else available
	if (d->dialogs.isEmpty())
		d->nav->setFocus();
}

void
KexiMainWindowImpl::slotShowSettings()
{
	KEXI_UNFINISHED(d->action_configure->text());
//TODO	KexiSettings s(this);
//	s.exec();
}

void
KexiMainWindowImpl::slotConfigureKeys()
{
/*    KKeyDialog dlg;
    dlg.insert( actionCollection() );
    dlg.configure();*/
	KKeyDialog::configure( actionCollection(), false/*bAllowLetterShortcuts*/, this );
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
	if (d->prj) {
//TODO use KexiStartupDialog(KexiStartupDialog::Templates...)

		bool cancel;
		KexiProjectData *new_data = createBlankProjectData(
			cancel, 
			false /* do not confirm prj overwrites: user will be asked on process startup */
		);
		if (!new_data)
			return;
		//start new instance

//TODO use KProcess?
    QStringList args;
		QProcess *proc = 0;
		if (!new_data->connectionData()->fileName().isEmpty()) {
			//file based
			args << qApp->applicationFilePath() << "-create-opendb" 
				<< new_data->connectionData()->fileName();
			proc = new QProcess(args, this, "process");
			proc->setCommunication((QProcess::Communication)0);
			proc->setWorkingDirectory( QFileInfo(new_data->connectionData()->fileName()).dir(true) );
		}
		else {
			//server based
			//TODO
			return;
		}
		if (!proc->start()) {
				d->showStartProcessMsg(args);
		}
		delete proc;
		delete new_data;
//		KEXI_UNFINISHED(i18n("Create another project"));
		return;
	}
	//create within this instance
	createBlankProject();
}

void
KexiMainWindowImpl::createKexiProject(KexiProjectData* new_data)
{
	d->prj = new KexiProject( new_data, this );
//	d->prj = ::createKexiProject(new_data);
//provided by KexiMessageHandler	connect(d->prj, SIGNAL(error(const QString&,KexiDB::Object*)), this, SLOT(showErrorMessage(const QString&,KexiDB::Object*)));
//provided by KexiMessageHandler	connect(d->prj, SIGNAL(error(const QString&,const QString&)), this, SLOT(showErrorMessage(const QString&,const QString&)));
	connect(d->prj, SIGNAL(itemRenamed(const KexiPart::Item&)), this, SLOT(slotObjectRenamed(const KexiPart::Item&)));

	if (d->nav)
		connect(d->prj, SIGNAL(itemRemoved(const KexiPart::Item&)), d->nav, SLOT(slotRemoveItem(const KexiPart::Item&)));
}

KexiProjectData*
KexiMainWindowImpl::createBlankProjectData(bool &cancelled, bool confirmOverwrites)
{
	cancelled = false;
	KexiNewProjectWizard wiz(Kexi::connset(), 0, "KexiNewProjectWizard", true);
	wiz.setConfirmOverwrites(confirmOverwrites);
	if (wiz.exec() != QDialog::Accepted) {
		cancelled=true;
		return 0;
	}

	KexiProjectData *new_data;

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
		cdata.driverName = KexiDB::Driver::defaultFileBasedDriverName();
		cdata.setFileName( wiz.projectDBName() );
		new_data = new KexiProjectData( cdata, wiz.projectDBName(), wiz.projectCaption() );
	}
	else {
		cancelled = true;
		return 0;
	}
	return new_data;
}

tristate
KexiMainWindowImpl::createBlankProject()
{
	bool cancel;
	KexiProjectData *new_data = createBlankProjectData(cancel);
	if (cancel)
		return cancelled;
	if (!new_data)
		return false;

	createKexiProject( new_data );

	bool ok = d->prj->create(true /*overwrite*/ );
	if (!ok) {
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

/* moved to kexiproject
tristate
KexiMainWindowImpl::createBlankProject()
{
	KexiProjectData *new_data = Kexi::startupHandler().projectData(); // true, if project data was provided from command line
	const bool dataAlreadyProvided = new_data;

	if (!new_data) {// project not provided, ask using the wizard
		KexiNewProjectWizard wiz(Kexi::connset(), 0, "KexiNewProjectWizard", true);
		if (wiz.exec() != QDialog::Accepted)
			return cancelled;

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
			return cancelled;
	}
	createKexiProject( new_data );

//todo: move this method outside keximainwindowimpl, so the window wont be visible if not needed

	bool ok = true;
	if (dataAlreadyProvided) {
		tristate res = d->prj->create(false);
		if (~res) {
			if (KMessageBox::Yes != KMessageBox::warningYesNo(qApp->desktop(), i18n(
				"The project \"%1\" already exists.\n"
				"Do you want to replace it with a new, blank one?")
				.arg(new_data->name()))) ///connectionData().dbFileName())))
//todo add serverInfoString() for server-based prj
			{
				return cancelled;
			}
		}
		ok = res;
	}
	if (ok) {
		tristate res = d->prj->create(true);
		ok = res;
	}
	if (!ok) {
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
}*/

void
KexiMainWindowImpl::slotProjectOpen()
{
	KexiStartupDialog dlg(
		KexiStartupDialog::OpenExisting, 0, Kexi::connset(), Kexi::recentProjects(),
		this, "KexiOpenDialog");

	if (dlg.exec()!=QDialog::Accepted)
		return;

	if (d->prj) {//js: TODO: start new instance!
		QProcess *proc;
    QStringList args;
		if (!dlg.selectedExistingFile().isEmpty()) {
//TODO use KRun
			args << qApp->applicationFilePath() << dlg.selectedExistingFile();
			proc = new QProcess(args, this, "process");
			proc->setWorkingDirectory( QFileInfo(dlg.selectedExistingFile()).dir(true) );
		}
		//TODO: server-based
		if (!proc->start()) {
			d->showStartProcessMsg(args);
		}
		delete proc;
		return;
	}

	KexiProjectData* projectData = 0;
	KexiDB::ConnectionData *cdata = dlg.selectedExistingConnection();
	if (cdata) {
		projectData = Kexi::startupHandler().selectProject( cdata, this );
		if (!projectData && Kexi::startupHandler().error()) {
			showErrorMessage(&Kexi::startupHandler());
		}
	}
	else {
		QString selFile = dlg.selectedExistingFile();
		if (!selFile.isEmpty()) {
			//file-based project
			kdDebug() << "Project File: " << selFile << endl;
			KexiDB::ConnectionData cdata;
			cdata.setFileName( selFile );
			cdata.driverName = KexiStartupHandler::detectDriverForFile( cdata.driverName, selFile, this );
			if (cdata.driverName.isEmpty())
				return;
			projectData = new KexiProjectData(cdata, selFile);
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
	saveObject( d->curDialog );
	updateAppCaption();
	invalidateActions();
}

void
KexiMainWindowImpl::slotProjectSaveAs()
{
	KEXI_UNFINISHED(i18n("Save object as"));
}

void
KexiMainWindowImpl::slotProjectProperties()
{
	//TODO: load the implementation not the ui :)
	ProjectSettingsUI u(this);
	u.exec();
}

void
KexiMainWindowImpl::slotProjectClose()
{
	closeProject();
}

void KexiMainWindowImpl::slotProjectRelations()
{
	if (!d->prj)
		return;
	KexiDialogBase *d = KexiInternalPart::createKexiDialogInstance("relation", this, this);
	activateWindow(d);
/*	KexiRelationPart *p = relationPart();
	if(!p)
		return;

	p->createWindow(this);*/
}

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
	if (~ closeProject())
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

bool KexiMainWindowImpl::switchToViewMode(int viewMode)
{
	if (!d->curDialog) {
		d->toggleLastCheckedMode();
		return false;
	}
	if (!d->curDialog->supportsViewMode( viewMode )) {
		showErrorMessage(i18n("Selected view mode is not supported for \"%1\" object.")
			.arg(d->curDialog->partItem()->name()),
		i18n("Selected view mode (%1) is not supported by this object type (%2)")
			.arg(Kexi::nameForViewMode(viewMode))
			.arg(d->curDialog->part()->instanceName()) );
		d->toggleLastCheckedMode();
		return false;
	}
//	bool cancelled;
	tristate res = d->curDialog->switchToViewMode( viewMode );
	if (!res) {
		showErrorMessage(i18n("Switching to other view failed (%1).").arg(Kexi::nameForViewMode(viewMode)),
			d->curDialog);
		d->toggleLastCheckedMode();
		return false;
	}
	if (~res) {
		d->toggleLastCheckedMode();
		return false;
	}

	//view changed: switch to this view's gui client
	KXMLGUIClient *viewClient=d->curDialog->guiClient();
	updateDialogViewGUIClient(viewClient);
	if (d->curDialogViewGUIClient && !viewClient)
		guiFactory()->removeClient(d->curDialogViewGUIClient);
	d->curDialogViewGUIClient=viewClient; //remember

	d->updatePropEditorVisibility(viewMode);
	invalidateSharedActions();
	return true;
}


void KexiMainWindowImpl::slotViewDataMode()
{
	switchToViewMode(Kexi::DataViewMode);
}

void KexiMainWindowImpl::slotViewDesignMode()
{
	switchToViewMode(Kexi::DesignViewMode);
}

void KexiMainWindowImpl::slotViewTextMode()
{
	switchToViewMode(Kexi::TextViewMode);
}

/*
void
KexiMainWindowImpl::showSorryMessage(const QString &title, const QString &details)
{
	showMessage(KMessageBox::Sorry, title, details);
}

void
KexiMainWindowImpl::showErrorMessage(const QString &title, const QString &details)
{
	showMessage(KMessageBox::Error, title, details);
}

void
KexiMainWindowImpl::showMessage(KMessageBox::DialogType dlgType,
	const QString &title, const QString &details)
{
	if (d->disableErrorMessages)
		return;
	QString msg = title;
	if (title.isEmpty())
		msg = i18n("Unknown error");
	msg = "<qt><p>"+msg+"</p>";
	if (!details.isEmpty()) {
		switch (dlgType) {
		case KMessageBox::Error:
			KMessageBox::detailedError(this, msg, details);
			break;
		default:
			KMessageBox::detailedSorry(this, msg, details);
		}
	}
	else {
		KMessageBox::messageBox(this, dlgType, msg);
	}
}

void
KexiMainWindowImpl::showErrorMessage(const QString &msg, KexiDB::Object *obj)
{
	QString _msg = msg;
	if (!obj) {
		showErrorMessage(_msg);
		return;
	}
	QString details;
	KexiDB::getHTMLErrorMesage(obj, _msg, details);
	showErrorMessage(_msg, details);
}

void KexiMainWindowImpl::showErrorMessage(const QString &msg, const QString &details, KexiDB::Object *obj)
{
	QString _msg = msg;
	if (!obj) {
		showErrorMessage(_msg, details);
		return;
	}
	QString _details;
	KexiDB::getHTMLErrorMesage(obj, _msg, _details);
	showErrorMessage(_msg, _details);
}

void
KexiMainWindowImpl::showErrorMessage(Kexi::ObjectStatus *status)
{
	showErrorMessage("", status);
}

void
KexiMainWindowImpl::showErrorMessage(const QString &message, Kexi::ObjectStatus *status)
{
	if (status && status->error()) {
		QString msg = message;
		if (msg.isEmpty()) {
			msg = status->message;
			status->message = status->description;
			status->description = "";
		}
		QString desc;
		if (!status->message.isEmpty()) {
			if (status->description.isEmpty()) {
				desc = status->message;
			} else {
				msg += (QString("<br><br>") + status->message);
				desc = status->description;
			}
		}
		showErrorMessage(message, desc, status->dbObject());
	}
	else {
		showErrorMessage(message);
	}
	status->clearStatus();
}
*/

void KexiMainWindowImpl::closeWindow(KMdiChildView *pWnd, bool layoutTaskBar)
{
	closeDialog(static_cast<KexiDialogBase *>(pWnd), layoutTaskBar);
}

tristate KexiMainWindowImpl::saveObject( KexiDialogBase *dlg, const QString& messageWhenAskingForName )
{
	if (!dlg->neverSaved()) {
		//data was saved in the past -just save again
		const tristate res = dlg->storeData();
		if (!res)
			showErrorMessage(i18n("Saving \"%1\" object failed.").arg(dlg->partItem()->name()),
				d->curDialog);
		return res;
	}

	//data was never saved in the past -we need to create a new object at the backend
#ifdef KEXI_ADD_CUSTOM_OBJECT_CREATION
	KexiPart::Info *info = dlg->part()->info();
# include "keximainwindowimpl_customobjcreation.h"
#endif
	if (!d->nameDialog) {
		d->nameDialog = new KexiNameDialog(
			messageWhenAskingForName, this, "nameDialog");
		//check if that name is allowed
		d->nameDialog->widget()->addNameSubvalidator(
			new Kexi::KexiDBObjectNameValidator(project()->dbConnection()->driver(), 0, "sub"));
	}
	else {
		d->nameDialog->widget()->setMessageText( messageWhenAskingForName );
	}
	d->nameDialog->widget()->setCaptionText(dlg->partItem()->caption());
	d->nameDialog->widget()->setNameText(dlg->partItem()->name());
	d->nameDialog->setCaption(i18n("Save Object As"));
	d->nameDialog->setDialogIcon( DesktopIcon( dlg->itemIcon(), KIcon::SizeMedium ) );
	bool found;
	do {
		if (d->nameDialog->exec()!=QDialog::Accepted)
			return cancelled;
		//check if that name already exists
		KexiDB::SchemaData tmp_sdata;
		found = project()->dbConnection()->loadObjectSchemaData(
				dlg->part()->info()->projectPartID(),
				d->nameDialog->widget()->nameText(), tmp_sdata );
		if (found) {
			KMessageBox::information(this, i18n("%1 \"%2\" already exists.\nPlease choose other name.")
				.arg(dlg->part()->instanceName()).arg(d->nameDialog->widget()->nameText()));
			continue;
		}
	}
	while (found);

	const int oldItemID = dlg->partItem()->identifier();
	//update name and caption
	dlg->partItem()->setName( d->nameDialog->widget()->nameText() );
	dlg->partItem()->setCaption( d->nameDialog->widget()->captionText() );

	const tristate res = dlg->storeNewData();
	if (~res)
		return cancelled;
	if (!res) {
		showErrorMessage(i18n("Saving new \"%1\" object failed.").arg(dlg->partItem()->name()),
			d->curDialog);
		return false;
	}

	//update navigator
	d->nav->addItem(dlg->partItem());
	//item id changed to final one: update association in dialogs' dictionary
	d->dialogs.take(oldItemID);
	d->dialogs.insert(dlg->partItem()->identifier(), dlg);
	return true;
}

tristate KexiMainWindowImpl::closeDialog(KexiDialogBase *dlg)
{
	return closeDialog(dlg, true);
}

tristate KexiMainWindowImpl::closeDialog(KexiDialogBase *dlg, bool layoutTaskBar)
{
	if (!dlg)
		return true;
	if (d->insideCloseDialog)
		return true;
	d->insideCloseDialog = true;

/*this crashes but is nice:
	QWidget *www = guiFactory()->container("query", dlg->commonGUIClient()); 
	delete www;*/
	
	bool remove_on_closing = dlg->partItem() ? dlg->partItem()->neverSaved() : false;
	if (dlg->dirty() && !d->forceDialogClosing) {
		//dialog's data is dirty:
		const int quertionRes = KMessageBox::questionYesNoCancel( this,
			i18n( "<p>The object has been modified: %1 \"%2\".</p><p>Do you want to save it?</p>" )
			.arg(dlg->part()->instanceName()).arg(dlg->partItem()->name()),
			QString::null,
			KStdGuiItem::save(),
			KStdGuiItem::discard());
		if (quertionRes==KMessageBox::Cancel) {
			d->insideCloseDialog = false;
			return cancelled;
		}
		if (quertionRes==KMessageBox::Yes) {
			//save it
//			if (!dlg->storeData())
			tristate res = saveObject( dlg );
			if (!res || ~res) {
//js:TODO show error info; (retry/ignore/cancel)
				d->insideCloseDialog = false;
				return res;
			}
			remove_on_closing = false;
		}
	}

	const int dlg_id = dlg->id(); //remember now, because removeObject() can destruct partitem object

	if (remove_on_closing) {
		//we won't save this object, and it was never saved -remove it
		if (!removeObject( dlg->partItem(), true )) {
			//msg?
			//TODO: ask if we'd continue and return true/false
			d->insideCloseDialog = false;
			return false;
		}
	}
	else {
		//not dirty now
		if(d->nav)
			d->nav->updateItemName( dlg->partItem(), false );
	}

	d->dialogs.take(dlg_id); //don't remove -KMDI will do that

	KXMLGUIClient *client = dlg->commonGUIClient();
	KXMLGUIClient *viewClient = dlg->guiClient();
	if (d->curDialogGUIClient==client) {
		d->curDialogGUIClient=0;
	}
	if (d->curDialogViewGUIClient==viewClient) {
		d->curDialogViewGUIClient=0;
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
	if (viewClient) {
		//sanity: ouch, it is not removed yet? - do it now
		if (d->closedDialogViewGUIClient && d->closedDialogViewGUIClient!=viewClient)
			guiFactory()->removeClient(d->closedDialogViewGUIClient);
		if (d->dialogs.isEmpty()) {//now there is no dialogs - remove client RIGHT NOW!
			d->closedDialogViewGUIClient=0;
			guiFactory()->removeClient(viewClient);
		}
		else {
			//remember this - and MAYBE remove later, if needed
			d->closedDialogViewGUIClient=viewClient;
		}
	}

	const bool isInMaximizedChildFrmMode = this->isInMaximizedChildFrmMode();

	KMdiMainFrm::closeWindow(dlg, layoutTaskBar);

	//focus navigator if nothing else available
	if (d->dialogs.isEmpty()) {
		d->maximizeFirstOpenedChildFrm = isInMaximizedChildFrmMode;
		if (d->nav)
			d->nav->setFocus();
		d->updatePropEditorVisibility(0);
	}

	invalidateActions();
	d->insideCloseDialog = false;
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
}

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

bool KexiMainWindowImpl::eventFilter( QObject *obj, QEvent * e )
{
	//KexiVDebug << "eventFilter: " <<e->type() << " " <<obj->name()<<endl;
	if (e->type()==QEvent::KeyPress) {
		KexiVDebug << "KEY EVENT " << QString::number(static_cast<QKeyEvent*>(e)->key(), 16) << endl;
		KexiVDebug << endl;
	}
	if (e->type()==QEvent::AccelOverride) {
		//KexiVDebug << "AccelOverride EVENT " << static_cast<QKeyEvent*>(e)->key() << " " << static_cast<QKeyEvent*>(e)->state() == ControlButton << endl;

		//avoid sending CTRL+Tab key twice for tabbed/ideal mode, epecially for win32
		if (static_cast<QKeyEvent*>(e)->key()==Key_Tab && static_cast<QKeyEvent*>(e)->state() == ControlButton) {
			if (d->action_window_next->shortcut().keyCodeQt()==Key_Tab+CTRL && d->action_window_next->shortcut().count()==1
				&& (mdiMode()==KMdi::TabPageMode || mdiMode()==KMdi::IDEAlMode))
			{
				static_cast<QKeyEvent*>(e)->accept();
			}
		}
	}
	if (e->type()==QEvent::Close) {
		KexiVDebug << "Close EVENT" << endl;
	}
	if (e->type()==QEvent::Resize) {
		KexiVDebug << "Resize EVENT" << endl;
	}
	if (e->type()==QEvent::ShowMaximized) {
		KexiVDebug << "ShowMaximized EVENT" << endl;
	}

	if (obj==d->propEditor) {
		if (e->type()==QEvent::Resize) {
			d->updatePropEditorDockWidthInfo();
		}
	}

	QWidget *focus_w = 0;
	if (obj->inherits("QPopupMenu")) {
		/* Fixes for popup menus behaviour:
		 For hiding/showing: focus previously (d->focus_before_popup)
		 focused window, if known, otherwise focus currently focused one.
		 And: just invalidate actions.
		*/
		if (e->type()==QEvent::Hide || e->type()==QEvent::Show) {
			KexiVDebug << e->type() << endl;
			focus_w = focusWindow();
			if (!d->focus_before_popup.isNull()) {
				d->focus_before_popup->setFocus();
				d->focus_before_popup=0;
				invalidateSharedActions();
			} else {
				if (focus_w) {
					focus_w->setFocus();
					invalidateSharedActions();
				}
			}
		}
		return false;
	}

	/*! On mouse click on the findow, make sure it's focused and actions are invalidated */
	if (e->type()==QEvent::MouseButtonPress) {
		QWidget *w = findWindow(static_cast<QWidget*>(obj));
		KexiVDebug << "MouseButtonPress EVENT " <<  (w ? w->name() : 0) << endl;
		if (w) {
			w->setFocus();
			invalidateSharedActions(d->curDialog);
		}
	}
	QWidget *w = findWindow(static_cast<QWidget*>(obj));
	if (e->type()==QEvent::FocusIn) {
		focus_w = focusWindow();
		KexiVDebug << "Focus EVENT" << endl;
		KexiVDebug << (focus_w ? focus_w->name() : "" )  << endl;
		KexiVDebug << "eventFilter: " <<e->type() << " " <<obj->name() <<endl;
	}
	else if (e->type()==QEvent::FocusOut) {
		focus_w = focusWindow();
		KexiVDebug << "Focus EVENT" << endl;
		KexiVDebug << (focus_w ? focus_w->name() : "" )  << endl;
		KexiVDebug << "eventFilter: " <<e->type() << " " <<obj->name() <<endl;
	}
	if (e->type()==QEvent::WindowActivate) {
		KexiVDebug << "WindowActivate EVENT" << endl;
		KexiVDebug << "eventFilter: " <<e->type() << " " <<obj->name()<<endl;
	}
#if 0
	if (e->type()==QEvent::FocusIn) {
		if (focus_w) {
//			if (d->actionProxies[ w ])
//			if (d->actionProxies[ focus_w ]) {
			if (actionProxyFor( focus_w )) {
//				invalidateSharedActions();
			}
			else {
/*			QObject* o = focusWidget();
			while (o && !o->inherits("KexiDialogBase") && !o->inherits("KexiDockBase"))
				o = o->parent();*/
//js				invalidateSharedActions(focus_w);
			}
		}
//		/*|| e->type()==QEvent::FocusOut*/) && /*(!obj->inherits("KexiDialogBase")) &&*/ d->actionProxies[ obj ]) {
//		invalidateSharedActions();
	}
	if (e->type()==QEvent::FocusOut && focus_w && focus_w==d->curDialog && actionProxyFor( obj )) {
		invalidateSharedActions(d->curDialog);
	}
#endif

	if (!d->focus_before_popup.isNull() && e->type()==QEvent::FocusOut && obj->inherits("KMenuBar")) {
		//d->nav->setFocus();
		d->focus_before_popup->setFocus();
		d->focus_before_popup=0;
		invalidateSharedActions(d->curDialog);
		return true;
	}

	//remember currently focued window invalidate act.
	if (e->type()==QEvent::FocusOut) {
		if (static_cast<QFocusEvent*>(e)->reason()==QFocusEvent::Popup) {
			if (Kexi::hasParent(d->curDialog, focus_w)) {
				invalidateSharedActions(d->curDialog);
				d->focus_before_popup=d->curDialog;
			}
			else {
//not needed???			invalidateSharedActions(focus_w);
				d->focus_before_popup=focus_w;
			}
		}
	}

	//keep focus in main window:
	if (w && w==d->nav) {
//		kdDebug() << "NAV" << endl;
		if (e->type()==QEvent::FocusIn) {
			return true;
		} else if (e->type()==QEvent::WindowActivate && w==d->focus_before_popup) {
//			d->nav->setFocus();
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
			//invalidateSharedActions();
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
	Kexi::WaitCursor wait;
	KexiDialogBase *dlg = d->dialogs[ item->identifier() ];
	bool needsUpdateViewGUIClient = true;
	if (dlg) {
		dlg->activate();
		if (viewMode!=dlg->currentViewMode()) {
			if (!switchToViewMode(viewMode))
				return 0;
		}
/*		if (dlg->currentViewMode()!=viewMode) {
			//try to switch
			bool cancelled;
			if (!dlg->switchToViewMode(viewMode, cancelled)) {
				//js TODO: add error msg...
				return 0;
			}
			if (cancelled)
				return 0;
			needsUpdateViewGUIClient = false;
		}*/
		needsUpdateViewGUIClient = false;
	}
	else {
		d->updatePropEditorVisibility(viewMode);
		dlg = d->prj->openObject(this, *item, viewMode);
//moved up		if (dlg)
//			d->updatePropEditorVisibility(dlg->currentViewMode());
	}

	if (!dlg || !activateWindow(dlg)) {
		//js TODO: add error msg...
		return 0;
	}

	if (needsUpdateViewGUIClient && !d->final) {
		//view changed: switch to this view's gui client
		KXMLGUIClient *viewClient=dlg->guiClient();
		updateDialogViewGUIClient(viewClient);
		if (d->curDialogViewGUIClient && !viewClient)
			guiFactory()->removeClient(d->curDialogViewGUIClient);
		d->curDialogViewGUIClient=viewClient; //remember
	}

	invalidateViewModeActions();
	if (viewMode!=dlg->currentViewMode())
		invalidateSharedActions();

	return dlg;
}

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

		//temp. hack: avoid problems with autonumber
		// see http://bugs.kde.org/show_bug.cgi?id=89381
		int p_id = KexiPart::LastObjectType+1; //min is == 3+1
		if (project()->dbConnection()->querySingleNumber("SELECT max(p_id) FROM kexi__parts", p_id))
			p_id++;

//		KexiDB::FieldList *fl = ts->subList("p_name", "p_mime", "p_url");
		KexiDB::FieldList *fl = ts->subList("p_id", "p_name", "p_mime", "p_url");
		kexidbg << "KexiMainWindowImpl::newObject(): fieldlist: " << (fl ? fl->debugString() : QString::null) << endl;
		if (!fl)
			return false;

//		kdDebug() << info->ptr()->genericName() << endl;
		kdDebug() << info->ptr()->untranslatedGenericName() << endl;
//		QStringList sl = info->ptr()->propertyNames();
//		for (QStringList::ConstIterator it=sl.constBegin();it!=sl.constEnd();++it)
//			kexidbg << *it << " " << info->ptr()->property(*it).toString() <<  endl;
		if (!project()->dbConnection()->insertRecord(*fl, 
				QVariant(p_id),
				QVariant(info->ptr()->untranslatedGenericName()), 
				QVariant(info->mime()), QVariant("http://www.koffice.org/kexi/")))
			return false;

		kdDebug() << "KexiMainWindowImpl::newObject(): insert success!" << endl;
		info->setProjectPartID(project()->dbConnection()->lastInsertedAutoIncValue("p_id", "kexi__parts"));
		kdDebug() << "KexiMainWindowImpl::newObject(): new id is: " << info->projectPartID()  << endl;
	}

#ifdef KEXI_ADD_CUSTOM_OBJECT_CREATION
# include "keximainwindowimpl_customobjcreation.h"
#endif

	KexiPart::Item *it = d->prj->createPartItem(info); //this, *item, viewMode);
	if (!it) {
		//js: todo: err
		return false;
	}

	if (!it->neverSaved()) //only add stored objects to the browser
		d->nav->addItem(it);
	return openObject(it, Kexi::DesignViewMode);
}

tristate KexiMainWindowImpl::removeObject( KexiPart::Item *item, bool dontAsk )
{
	if (!d->prj || !item)
		return false;

	KexiPart::Part *part = Kexi::partManager().part(item->mime());
	if (!part)
		return false;

	if (!dontAsk) {
		if (KMessageBox::No == KMessageBox::warningYesNo(this, "<p>"+i18n("Do you want to remove:")
			+"</p><p>"+part->instanceName()+" \""+ item->name() + "\"?</p>",
			0, KStdGuiItem::yes(), KStdGuiItem::no()))//, "askBeforeDeletePartItem"/*config entry*/))
			return cancelled;
	}

	KexiDialogBase *dlg = d->dialogs[item->identifier()];
	if (dlg) {//close existing window
//		if (!dlg->tryClose(true))
		const bool tmp = d->forceDialogClosing;
		/*const bool remove_on_closing = */dlg->partItem()->neverSaved();
		d->forceDialogClosing = true;
		const tristate res = closeDialog(dlg);
		d->forceDialogClosing = tmp; //restore
		if (!res || ~res) {
			return res;
		}
//		if (remove_on_closing) //already removed
	//		return true;
//		if (!dlg->close(true))
//			return true; //ok - close cancelled
	}

	if (!d->prj->removeObject(this, *item)) {
		//TODO(js) better msg
		showSorryMessage( i18n("Could not remove object.") );
		return false;
	}
	return true;
}

void KexiMainWindowImpl::renameObject( KexiPart::Item *item, const QString& _newName, bool &success )
{
	QString newName = _newName.stripWhiteSpace();
	if (newName.isEmpty()) {
		showSorryMessage( i18n("Could not set empty name for this object.") );
		success = false;
		return;
	}
	enableMessages(false); //to avoid double messages
	const bool res = d->prj->renameObject(this, *item, newName);
	enableMessages(true);
	if (!res) {
		showErrorMessage( i18n("Renaming object \"%1\" failed.").arg(newName), d->prj );
		success = false;
		return;
	}
}

void KexiMainWindowImpl::slotObjectRenamed(const KexiPart::Item &item)
{
	KexiDialogBase *dlg = d->dialogs[item.identifier()];
	if (dlg) {//change item
		dlg->updateCaption();
		if (static_cast<KexiDialogBase*>(d->curDialog)==dlg)//optionally, update app. caption
			updateAppCaption();
	}
}

int KexiMainWindowImpl::generatePrivateID()
{
	return --d->privateIDCounter;
}

void KexiMainWindowImpl::propertyBufferSwitched(KexiDialogBase *dlg, bool force, bool preservePrevSelection)
{
	kdDebug() << "KexiMainWindowImpl::propertyBufferSwitched()" << endl;
	if ((KexiDialogBase*)d->curDialog!=dlg)
		return;
	if (d->propEditor) {
		KexiPropertyBuffer *newBuf = d->curDialog ? d->curDialog->propertyBuffer() : 0;
		if (!newBuf || (force || static_cast<KexiPropertyBuffer*>(d->propBuffer) != newBuf)) {
			d->propBuffer = newBuf;
			d->propEditor->editor()->setBuffer( d->propBuffer, preservePrevSelection );
		}
	}
}

void KexiMainWindowImpl::slotDirtyFlagChanged(KexiDialogBase* dlg)
{
	KexiPart::Item *item = dlg->partItem();
	//update text in navigator and app. caption
	if(!d->final)
		d->nav->updateItemName( item, dlg->dirty() );

	invalidateActions();
	updateAppCaption();
}

void KexiMainWindowImpl::slotMdiModeHasBeenChangedTo(KMdi::MdiMode)
{
	//after switching to other MDI mode, pointer to current dialog needs to be updated
	activateFirstWin();
	activeWindowChanged(activeWindow());
}

void KexiMainWindowImpl::slotTipOfTheDay()
{
	//todo
}

void KexiMainWindowImpl::slotImportantInfo()
{
	importantInfo(false);
}

void KexiMainWindowImpl::importantInfo(bool /*onStartup*/)
{
#if 0
	if (onStartup && !d->showImportantInfoOnStartup)
		return;

	QString key = QString("showImportantInfo %1").arg(KEXI_VERSION_STRING);
	d->config->setGroup("Startup");
	bool show = d->config->readBoolEntry(key,true);

	if (show || !onStartup) { //if !onStartup - dialog is always shown
		d->config->setGroup("TipOfDay");
		if (!d->config->hasKey("RunOnStart"))
			d->config->writeEntry("RunOnStart",true);

		QString lang = KGlobal::locale()->language();
		QString fname = locate("data", QString("kexi/readme_")+lang);
		if (fname.isEmpty())//back to default
			fname = locate("data", "kexi/readme_en");
		KTipDialog tipDialog(new KTipDatabase(QString::null), 0);
		tipDialog.setCaption(i18n("Important Information"));
		QObjectList *l = tipDialog.queryList( "KPushButton" );//hack: hide <- -> buttons
		int i=0;
		for (QObjectListIt it( *l ); it.current() && i<2; ++it, i++ )
			static_cast<KPushButton*>(it.current())->hide();
		QFile f(fname);
		if ( f.open( IO_ReadOnly ) ) {
			QTextStream ts(&f);
			ts.setCodec( KGlobal::locale()->codecForEncoding() );
			QTextBrowser *tb = Kexi::findFirstChild<KTextBrowser>(&tipDialog,"KTextBrowser");
			if (tb) {
				tb->setText( QString("<qt>%1</qt>").arg(ts.read()) );
			}
			f.close();
		}

		tipDialog.adjustSize();
		QRect desk = QApplication::desktop()->screenGeometry( QApplication::desktop()->screenNumber(this) );
		tipDialog.resize( QMAX(tipDialog.width(),desk.width()*3/5), QMAX(tipDialog.height(),desk.height()*3/5) );
		KDialog::centerOnScreen(&tipDialog);
		tipDialog.setModal ( true );
		tipDialog.exec();
		//a hack: get user's settings
		d->config->setGroup("TipOfDay");
		show = d->config->readBoolEntry("RunOnStart", show);
	}

	//write our settings back
	d->config->setGroup("Startup");
	d->config->writeEntry(key,show);
	d->showImportantInfoOnStartup = false;
#endif
}

void KexiMainWindowImpl::slotOptionsEnableForms(bool show, bool noMessage)
{
	Kexi::tempShowForms() = show;
	d->config->setGroup("Unfinished");
	d->config->writeEntry("EnableForms", Kexi::tempShowForms());
	if (noMessage)
		return;
	QString note = i18n("Please note that forms are currently unstable functionality, provided <u>only for your preview</u>.");
	if (show) {
		KMessageBox::information(this,
			"<p>"+i18n("Forms will be available after restarting Kexi application.")+"</p>"+note+"<p>");
	}
	else {
		KMessageBox::information(this,
			"<p>"+i18n("Forms will be hidden after restarting Kexi application.")+"</p><p>"+note+"<p>");
	}
}

bool KexiMainWindowImpl::inFinalMode() const
{
	return d->final;
}

bool
KexiMainWindowImpl::initFinalMode(KexiProjectData *projectData)
{
//TODO
	Kexi::tempShowForms() = true;
	Kexi::tempShowReports() = true;
	if(!projectData)
		return false;

	createKexiProject(projectData); //initialize project
	d->prj->m_final = true;         //announce that we are in fianl mode

	if(!d->prj->open())             //try to open database
		return false;

	KexiDB::TableSchema *sch = d->prj->dbConnection()->tableSchema("kexi__final");
	QString err_msg = i18n("Could not start project \"%1\" in Final Mode.")
		.arg(static_cast<KexiDB::SchemaData*>(projectData)->name());
	if(!sch)
	{
		hide();
		showErrorMessage( err_msg, i18n("No Final Mode data found.") );
		return false;
	}

	KexiDB::Cursor *c = d->prj->dbConnection()->executeQuery(*sch);
	if(!c)
	{
		hide();
		showErrorMessage( err_msg, i18n("Error reading Final Mode data.") );
		return false;
	}

	QString startupPart;
	QString startupItem;
	while(c->moveNext())
	{
		kdDebug() << "KexiMainWinImpl::initFinalMode(): property: [" << c->value(1).toString() << "] " << c->value(2).toString() << endl;
		if(c->value(1).toString() == "startup-part")
			startupPart = c->value(2).toString();
		else if(c->value(1).toString() == "startup-item")
			startupItem = c->value(2).toString();
		else if(c->value(1).toString() == "mainxmlui")
			setXML(c->value(2).toString());
	}
	d->prj->dbConnection()->deleteCursor(c);

	kdDebug() << "KexiMainWinImpl::initFinalMode(): part: " << startupPart << endl;
	kdDebug() << "KexiMainWinImpl::initFinalMode(): item: " << startupItem << endl;

	initActions();
	initUserActions();
	guiFactory()->addClient(this);
	setStandardToolBarMenuEnabled(false);
	setHelpMenuEnabled(false);

	KexiPart::Info *i = Kexi::partManager().info(startupPart.latin1());
	if (!i) {
		hide();
		showErrorMessage( err_msg, i18n("Specified plugin does not exist.") );
		return false;
	}

	Kexi::partManager().part(i);
	KexiPart::Item *item = d->prj->item(i, startupItem);
	if(!openObject(item, Kexi::DataViewMode)) {
		hide();
		showErrorMessage( err_msg, i18n("Specified object could not be opened.") );
		return false;
	}

	QWidget::setCaption("MyApp");//TODO

	return true;
}

void
KexiMainWindowImpl::initUserActions()
{
	KexiDB::Cursor *c = d->prj->dbConnection()->executeQuery("SELECT p_id, name, text, icon, method, arguments FROM kexi__useractions WHERE scope = 0");
	if(!c)
		return;

	while(c->moveNext())
	{
		KexiUserAction::fromCurrentRecord(this, actionCollection(), c);
	}
	d->prj->dbConnection()->deleteCursor(c);
/*
	KexiUserAction *a1 = new KexiUserAction(this, actionCollection(), "user_dataview", "Change to dataview", "table");
	Arguments args;
	args.append(QVariant("kexi/table"));
	args.append(QVariant("persons"));
	a1->setMethod(KexiUserAction::OpenObject, args);
*/
}

#include <kreplacedialog.h>

void KexiMainWindowImpl::slotImportProject()
{
#ifndef KEXI_NO_MIGRATION
	QDialog *d = KexiInternalPart::createModalDialogInstance("migration", this, this);
	if (!d)
		return;
	d->exec();
//	KexiMigration::importWizard* iw = new KexiMigration::importWizard();
//	iw->setGeometry(300,300,400,300);
//	iw->show();
#endif
}

void KexiMainWindowImpl::addWindow( KMdiChildView* pView, int flags )
{
	//maximize this window, if it's 
//!@todo Certain windows' sizes, e.g. forms could have own size configation specified!
//!      Query for this, and if so: give up.
	if (d->maximizeFirstOpenedChildFrm) {
		flags |= KMdi::Maximize;
		d->maximizeFirstOpenedChildFrm = false;
	}
	KexiMainWindow::addWindow( pView, flags );
}

#include "keximainwindowimpl.moc"

