/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2007 Jaroslaw Staniek <js@iidea.pl>

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

#include "keximainwindowimpl.h"

#include <unistd.h>

#include <qapplication.h>
#include <qeventloop.h>
#include <qfile.h>
#include <qtimer.h>
#include <qobjectlist.h>
#include <qprocess.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qmutex.h>
#include <qwaitcondition.h>
#include <qfiledialog.h>
#include <qdockwindow.h>
#include <qdockarea.h>

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
#include <ktabwidget.h>
#include <kimageio.h>
#include <khelpmenu.h>
#include <kfiledialog.h>
#include <krecentdocument.h>

#include <kexidb/connection.h>
#include <kexidb/utils.h>
#include <kexidb/cursor.h>
#include <kexidb/dbobjectnamevalidator.h>
#include <kexidb/admin.h>
#include <kexiutils/utils.h>

//#include "projectsettingsui.h"
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
#include "kexistatusbar.h"
#include "kexiinternalpart.h"
#include "kexiactioncategories.h"
#include "kexifinddialog.h"
#include "kexisearchandreplaceiface.h"

#include "kde2_closebutton.xpm"

#include <widget/kexibrowser.h>
#include <widget/kexipropertyeditorview.h>
#include <widget/utils/kexirecordnavigator.h>
#include <koproperty/editor.h>
#include <koproperty/set.h>

#include "startup/KexiStartup.h"
#include "startup/KexiNewProjectWizard.h"
#include "startup/KexiStartupDialog.h"
#include "startup/KexiStartupFileDialog.h"
#include "kexinamedialog.h"
#include "printing/kexisimpleprintingpart.h"
#include "printing/kexisimpleprintingpagesetup.h"

//Extreme verbose debug
#if defined(Q_WS_WIN)
# include <krecentdirs.h>
# include <win32_utils.h>
//# define KexiVDebug kdDebug()
#endif

#if !defined(KexiVDebug)
# define KexiVDebug if (0) kdDebug()
#endif

//first fix the geometry
//#define KEXI_NO_CTXT_HELP 1

#ifndef KEXI_NO_CTXT_HELP
#include <kexicontexthelp.h>
#endif

#ifdef HAVE_KNEWSTUFF
#include <knewstuff/downloaddialog.h>
#include "kexinewstuff.h"
#endif

//! @todo REENABLE when blinking and dock
//! width changes will be removed in KMDI
//#define PROPEDITOR_VISIBILITY_CHANGES

//temporary fix to manage layout
#include "ksplitter.h"
#define KDOCKWIDGET_P 1

#ifndef KEXI_NO_FEEDBACK_AGENT
#ifdef FEEDBACK_INCLUDE
#include FEEDBACK_INCLUDE
#endif
#include <kapplication.h>
#include <kaboutdata.h>
#endif

#include "keximainwindowimpl_p.h"

//-------------------------------------------------

//static
int KexiMainWindowImpl::create(int argc, char *argv[], KAboutData* aboutdata)
{
	Kexi::initCmdLineArgs( argc, argv, aboutdata );

	bool GUIenabled = true;
	QWidget *dummyWidget = 0; //needed to have icon for dialogs before KexiMainWindowImpl is created
//! @todo switch GUIenabled off when needed
	KApplication* app = new KApplication(true, GUIenabled);

#ifdef KEXI_STANDALONE
	KGlobal::locale()->removeCatalogue("kexi");
	KGlobal::locale()->insertCatalogue("standalone_kexi");
#endif
	KGlobal::locale()->insertCatalogue("koffice");
	KGlobal::locale()->insertCatalogue("koproperty");

#ifdef CUSTOM_VERSION
# include "custom_exec.h"
#endif

#ifdef KEXI_DEBUG_GUI
	QWidget* debugWindow = 0;
#endif
	if (GUIenabled) {
		dummyWidget = new QWidget();
		dummyWidget->setIcon( DesktopIcon( "kexi" ) );
		app->setMainWidget(dummyWidget);
#ifdef KEXI_DEBUG_GUI
		app->config()->setGroup("General");
		if (app->config()->readBoolEntry("showInternalDebugger", false)) {
			debugWindow = KexiUtils::createDebugWindow(0);
		}
#endif
	}

	tristate res = Kexi::startupHandler().init(argc, argv);
	if (!res || ~res) {
#ifdef KEXI_DEBUG_GUI
		delete debugWindow;
#endif
		delete app;
		return (~res) ? 0 : 1;
	}
	
	kdDebug() << "startupActions OK" <<endl;

	/* Exit requested, e.g. after database removing. */
	if (Kexi::startupHandler().action() == KexiStartupData::Exit) {
#ifdef KEXI_DEBUG_GUI
		delete debugWindow;
#endif
		delete app;
		return 0;
	}

	KexiMainWindowImpl *win = new KexiMainWindowImpl();
	app->setMainWidget(win);
#ifdef KEXI_DEBUG_GUI
	//if (debugWindow)
		//debugWindow->reparent(win, QPoint(1,1));
#endif
	delete dummyWidget;

	if (true != win->startup()) {
		delete win;
		delete app;
		return 1;
	}

	win->show();
	app->processEvents();//allow refresh our app

//#ifdef KEXI_DEBUG_GUI
//	delete debugWindow;
//#endif
	return 0;
}

//-------------------------------------------------

KexiMainWindowImpl::KexiMainWindowImpl()
 : KexiMainWindow()
 , KexiGUIMessageHandler(this)
 , d(new KexiMainWindowImpl::Private(this) )
{
	KImageIO::registerFormats();
	KexiProjectData *pdata = Kexi::startupHandler().projectData();
	d->userMode = Kexi::startupHandler().forcedUserMode() /* <-- simply forced the user mode */
		/* project has 'user mode' set as default and not 'design mode' override is found: */
		|| (pdata && pdata->userMode() && !Kexi::startupHandler().forcedDesignMode());
	d->isProjectNavigatorVisible = Kexi::startupHandler().isProjectNavigatorVisible();

	if(userMode())
		kdDebug() << "KexiMainWindowImpl::KexiMainWindowImpl(): starting up in the User Mode" << endl;

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
	manager()->setSplitterHighResolution(true);
	manager()->setSplitterKeepSize(true);
	setStandardMDIMenuEnabled(false);
	setAsDefaultHost(); //this is default host now.
	KGlobal::iconLoader()->addAppDir("kexi");
	KGlobal::iconLoader()->addAppDir("koffice");

	//get informed
	connect(&Kexi::partManager(),SIGNAL(partLoaded(KexiPart::Part*)),this,SLOT(slotPartLoaded(KexiPart::Part*)));
	connect( m_pMdi, SIGNAL(nowMaximized(bool)), this, SLOT(slotCaptionForCurrentMDIChild(bool)) );
	connect( m_pMdi, SIGNAL(noMaximizedChildFrmLeft(KMdiChildFrm*)), this, SLOT(slotNoMaximizedChildFrmLeft(KMdiChildFrm*)));
//	connect( this, SIGNAL(lastChildFrmClosed()), this, SLOT(slotLastChildFrmClosed()));
	connect( this, SIGNAL(lastChildViewClosed()), this, SLOT(slotLastChildViewClosed()));

	connect( this, SIGNAL(childViewIsDetachedNow(QWidget*)), this, SLOT(slotChildViewIsDetachedNow(QWidget*)));
	connect( this, SIGNAL(mdiModeHasBeenChangedTo(KMdi::MdiMode)),
		this, SLOT(slotMdiModeHasBeenChangedTo(KMdi::MdiMode)));


	//if (!userMode()) {
		setXMLFile("kexiui.rc");
		setAcceptDrops(true);
		initActions();
		createShellGUI(true);
	//}

	d->statusBar = new KexiStatusBar(this, "status_bar");

	d->origAppCaption = caption();

	restoreSettings();
	(void)Kexi::smallFont(this/*init*/);

	if (!userMode()) {
		initContextHelp();
		initPropertyEditor();
	}

	{//store menu popups list
		QObjectList *l = queryList( "QPopupMenu" );
		for (QObjectListIt it( *l ); it.current(); ++it ) {
			//kdDebug() << "name=" <<it.current()->name() << " cname="<<it.current()->className()<<endl;
			//KexiMainWindowImpl::eventFilter() will filter our popups:
			it.current()->installEventFilter(this);
			d->popups.insert(it.current()->name(), static_cast<QPopupMenu*>(it.current()));
		}
		delete l;
		d->createMenu = d->popups["create"];

#ifdef KEXI_NO_REPORTBUG_COMMAND
		//remove "bug report" action to avoid confusion for supported with commercial technical support
		QPopupMenu *helpMenu = d->popups["help"];
		if (helpMenu) {
			//const int idx = helpMenu->indexOf( (int)KHelpMenu::menuReportBug );
			helpMenu->removeItemAt(int(KHelpMenu::menuReportBug)-1);
			helpMenu->removeItemAt(int(KHelpMenu::menuReportBug)-1); //separator
		}
#endif
	}

	//fix menus a bit more:
#ifndef KEXI_SHOW_UNIMPLEMENTED
//disabled (possible crash)	d->hideMenuItem("file", i18n("&Import"), true);
//disabled (possible crash)	d->hideMenuItem("help", i18n( "&Report Bug..." ), true);
#endif
	KAction *kmdi_tooldock_menu_action = childClients()->getFirst() ? childClients()->getFirst()->actionCollection()->action("kmdi_tooldock_menu") : 0;
	if (kmdi_tooldock_menu_action) {
		kmdi_tooldock_menu_action->setEnabled(false);
	}

	if (!isFakingSDIApplication()/* && !userMode()*/) {
//		QPopupMenu *menu = (QPopupMenu*) child( "window", "KPopupMenu" );
		QPopupMenu *menu = d->popups["window"];
		unsigned int count = menuBar()->count();
		if (menu)
			setWindowMenu(menu);
		else
			menuBar()->insertItem( i18n("&Window"), windowMenu(), -1, count-2); // standard position is left to the last ('Help')
	}
	if (userMode()) {
		//hide "insert" menu and disable "project_import", "edit_paste_special" menus
		QPopupMenu *menu = d->popups["insert"];
		if (menu) {
			for (uint i=0; i < menuBar()->count(); i++) {
				if (menuBar()->text( menuBar()->idAt(i) ) == i18n("&Insert")) {
					menuBar()->setItemVisible( menuBar()->idAt(i), false );
					break;
				}
			}
		}
		d->disableMenuItem("file", i18n("&Import"));
		d->disableMenuItem("edit", i18n("Paste &Special"));
	}

	m_pTaskBar->setCaption(i18n("Task Bar"));	//js TODO: move this to KMDIlib

//	if (!userMode()) {
		invalidateActions();
		d->timer.singleShot(0,this,SLOT(slotLastActions()));
//	}

	setTabWidgetVisibility(KMdi::AlwaysShowTabs);
	if (mdiMode()==KMdi::IDEAlMode) {
		d->config->setGroup("MainWindow");
		tabWidget()->setHoverCloseButton(d->config->readBoolEntry("HoverCloseButtonForTabs", false));
		//create special close button as corner widget for IDEAl mode
		QToolButton *closeButton = new QToolButton( tabWidget() );
		closeButton->setAutoRaise( true );
		closeButton->setPixmap( QPixmap( kde2_closebutton ) );
		closeButton->setPaletteBackgroundColor(closeButton->palette().active().background()); 
//		closeButton->setIconSet(SmallIconSet("tab_remove"));
		tabWidget()->setCornerWidget( closeButton, Qt::TopRight );
		closeButton->hide(); // hide until it's needed to avoid problems in "user mode" 
		                     // when initially the main window is empty
		QToolTip::add(closeButton, 
			i18n("Close the current tab page in Kexi tab interface", "Close the current tab"));
		QObject::connect( closeButton, SIGNAL( clicked() ), this, SLOT( closeActiveView() ) );
	}

#ifdef KEXI_ADD_CUSTOM_KEXIMAINWINDOWIMPL
# include "keximainwindowimpl_ctor.h"
#endif
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
		//kdDebug() << menuBar()->text( menuBar()->idAt(i) ) << endl;
		if (txt==menuBar()->text( menuBar()->idAt(i) ))
			break;
	}
	if (i<count) {
		const int id = menuBar()->idAt(i);
		menuBar()->removeItemAt(i);
		menuBar()->insertItem(txt, m_pWindowMenu, id, count-3);
	}
	m_pWindowMenu->setCheckable(true);
	QObject::connect( m_pWindowMenu, SIGNAL(aboutToShow()), this, SLOT(fillWindowMenu()) );
}

void KexiMainWindowImpl::fillWindowMenu()
{
	KexiMainWindow::fillWindowMenu();

/*	int i;
	for (i=0; i < (int)m_pWindowMenu->count(); i++) {
		if (m_pWindowMenu->text( m_pWindowMenu->idAt( i ) ) == i18n( "&MDI Mode" )) {
//			kdDebug() << m_pWindowMenu->text( m_pWindowMenu->idAt( i ) ) << endl;
			m_pWindowMenu->removeItem( m_pWindowMenu->idAt( i ) );
			break;
		}
	}*/

	m_pMdiModeMenu->removeItem( m_pMdiModeMenu->idAt( 0 ) ); //hide toplevel mode
	m_pMdiModeMenu->removeItem( m_pMdiModeMenu->idAt( 1 ) ); //hide tabbed mode
	//update
	if (d->mdiModeToSwitchAfterRestart != (KMdi::MdiMode)0) {
		m_pMdiModeMenu->setItemChecked( m_pMdiModeMenu->idAt( 0 ), 
			d->mdiModeToSwitchAfterRestart == KMdi::ChildframeMode );
		m_pMdiModeMenu->setItemChecked( m_pMdiModeMenu->idAt( 1 ), 
			d->mdiModeToSwitchAfterRestart == KMdi::IDEAlMode );
	}

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

void KexiMainWindowImpl::switchToIDEAlMode()
{
	switchToIDEAlMode(true);
}

void KexiMainWindowImpl::switchToIDEAlMode(bool showMessage)
{
	if (showMessage) {
		if ((int)d->mdiModeToSwitchAfterRestart == 0 && mdiMode()==KMdi::IDEAlMode)
			return;
		if (d->mdiModeToSwitchAfterRestart == KMdi::IDEAlMode)
			return;
		if (mdiMode()==KMdi::IDEAlMode) {//current mode
			d->mdiModeToSwitchAfterRestart = (KMdi::MdiMode)0;
		}
		else {
			KMessageBox::information(this,
				i18n("User interface mode will be switched to IDEAl at next %1 application startup.")
				.arg(KEXI_APP_NAME));
			//delayed
			d->mdiModeToSwitchAfterRestart = KMdi::IDEAlMode;
		}
	}
	else
		KexiMainWindow::switchToIDEAlMode();
}

void KexiMainWindowImpl::switchToChildframeMode()
{
	switchToChildframeMode(true);
}

void KexiMainWindowImpl::switchToChildframeMode(bool showMessage)
{
	if (showMessage) {
		if ((int)d->mdiModeToSwitchAfterRestart == 0 && mdiMode()==KMdi::ChildframeMode)
			return;
		if (d->mdiModeToSwitchAfterRestart == KMdi::ChildframeMode)
			return;
		if (mdiMode()==KMdi::ChildframeMode) {//current mode
			d->mdiModeToSwitchAfterRestart = (KMdi::MdiMode)0;
		}
		else {
			KMessageBox::information(this,
				i18n("User interface mode will be switched to Childframe at next %1 application startup.")
				.arg(KEXI_APP_NAME));
			//delayed
			d->mdiModeToSwitchAfterRestart = KMdi::ChildframeMode;
		}
	}
	else
		KexiMainWindow::switchToChildframeMode();
}

QPopupMenu* KexiMainWindowImpl::findPopupMenu(const char *popupName)
{
	return d->popups[popupName];
}

KActionPtrList KexiMainWindowImpl::allActions() const
{
	return actionCollection()->actions();
}

KexiDialogBase* KexiMainWindowImpl::currentDialog() const
{
	return d->curDialog;
}

void KexiMainWindowImpl::initActions()
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

#ifdef HAVE_KNEWSTUFF
	action = new KAction(i18n("&Download Example Databases..."), "kget", KShortcut(0),
		this, SLOT(slotGetNewStuff()), actionCollection(), "project_download_examples");
	action->setToolTip(i18n("Download example databases from the Internet"));
	action->setWhatsThis(i18n("Downloads example databases from the Internet."));
#endif

//	d->action_open_recent = KStdAction::openRecent( this, SLOT(slotProjectOpenRecent(const KURL&)), actionCollection(), "project_open_recent" );

//#ifdef KEXI_SHOW_UNIMPLEMENTED
#ifndef KEXI_NO_UNFINISHED
	d->action_open_recent = new KActionMenu(i18n("Open Recent"),
		actionCollection(), "project_open_recent");
	connect(d->action_open_recent->popupMenu(),SIGNAL(activated(int)),
		this,SLOT(slotProjectOpenRecent(int)));
	connect(d->action_open_recent->popupMenu(), SIGNAL(aboutToShow()),
		this,SLOT(slotProjectOpenRecentAboutToShow()));
//moved down		d->action_open_recent_projects_title_id = 
//		d->action_open_recent->popupMenu()->insertTitle(i18n("Recently Opened Databases"));
//moved down	d->action_open_recent_connections_title_id = 
//		d->action_open_recent->popupMenu()->insertTitle(i18n("Recently Connected Database Servers"));
//	d->action_open_recent->popupMenu()->insertSeparator();
//	d->action_open_recent_more_id = d->action_open_recent->popupMenu()
//		->insertItem(i18n("&More Projects..."), this, SLOT(slotProjectOpenRecentMore()), 0, 1000);
#else
	d->action_open_recent = d->dummy_action;
#endif

	d->action_save = KStdAction::save( 
		this, SLOT( slotProjectSave() ), actionCollection(), "project_save" );
//	d->action_save = new KAction(i18n("&Save"), "filesave", KStdAccel::shortcut(KStdAccel::Save),
//		this, SLOT(slotProjectSave()), actionCollection(), "project_save");
	d->action_save->setToolTip(i18n("Save object changes"));
	d->action_save->setWhatsThis(i18n("Saves object changes from currently selected window."));

#ifdef KEXI_SHOW_UNIMPLEMENTED
	d->action_save_as = new KAction(i18n("Save &As..."), "filesaveas", 0,
		this, SLOT(slotProjectSaveAs()), actionCollection(), "project_saveas");
	d->action_save_as->setToolTip(i18n("Save object as"));
	d->action_save_as->setWhatsThis(
		i18n("Saves object changes from currently selected window under a new name (within the same project)."));

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

	KStdAction::quit( this, SLOT(slotProjectQuit()), actionCollection(), "quit");

#ifdef KEXI_SHOW_UNIMPLEMENTED
	d->action_project_relations = new KAction(i18n("&Relationships..."), "relation", Qt::CTRL + Qt::Key_R,
		this, SLOT(slotProjectRelations()), actionCollection(), "project_relations");
	d->action_project_relations->setToolTip(i18n("Project relationships"));
	d->action_project_relations->setWhatsThis(i18n("Shows project relationships."));

#else
	d->action_project_relations = d->dummy_action;
#endif
	d->action_tools_data_migration = new KAction(
		i18n("&Import Database..."), "database_import", 0,
		this, SLOT(slotToolsProjectMigration()), actionCollection(), "tools_import_project");
	d->action_tools_data_migration->setToolTip(i18n("Import entire database as a Kexi project"));
	d->action_tools_data_migration->setWhatsThis(i18n("Imports entire database as a Kexi project."));

	d->action_tools_compact_database = new KAction(
		i18n("&Compact Database..."), "", 0,
		this, SLOT(slotToolsCompactDatabase()), actionCollection(), "tools_compact_database");
	d->action_tools_compact_database->setToolTip(i18n("Compact the current database project"));
	d->action_tools_compact_database->setWhatsThis(
		i18n("Compacts the current database project, so it will take less space and work faster."));

	if (userMode())
		d->action_project_import_data_table = 0;
	else {
		d->action_project_import_data_table = new KAction(
			i18n("Import->Table Data From File...", "Table Data From &File..."),
			"table"/*! @todo: change to "file_import" or so*/,
			0, this, SLOT(slotProjectImportDataTable()), actionCollection(), 
			"project_import_data_table");
		d->action_project_import_data_table->setToolTip(i18n("Import table data from a file"));
		d->action_project_import_data_table->setWhatsThis(i18n("Imports table data from a file."));
	}

	d->action_project_export_data_table = new KAction(i18n("Export->Table or Query Data to File...", 
		"Table or Query Data to &File..."),
		"table"/*! @todo: change to "file_export" or so*/,
		0, this, SLOT(slotProjectExportDataTable()), actionCollection(), 
		"project_export_data_table");
	d->action_project_export_data_table->setToolTip(
		i18n("Export data from the active table or query data to a file"));
	d->action_project_export_data_table->setWhatsThis(
		i18n("Exports data from the active table or query data to a file."));

//TODO	new KAction(i18n("From File..."), "fileopen", 0,
//TODO		this, SLOT(slotImportFile()), actionCollection(), "project_import_file");
//TODO	new KAction(i18n("From Server..."), "server", 0,
//TODO		this, SLOT(slotImportServer()), actionCollection(), "project_import_server");

	d->action_project_print = KStdAction::print(this, SLOT(slotProjectPrint()),
		actionCollection(), "project_print" ); 
	d->action_project_print->setToolTip(i18n("Print data from the active table or query"));
	d->action_project_print->setWhatsThis(i18n("Prints data from the active table or query."));

	d->action_project_print_preview = KStdAction::printPreview(
		this, SLOT(slotProjectPrintPreview()),
		actionCollection(), "project_print_preview" ); 
	d->action_project_print_preview->setToolTip(
		i18n("Show print preview for the active table or query"));
	d->action_project_print_preview->setWhatsThis(
		i18n("Shows print preview for the active table or query."));

	d->action_project_print_setup = new KAction(i18n("Page Set&up..."),
		"", 0, this, SLOT(slotProjectPageSetup()), actionCollection(), 
		"project_print_setup");
	d->action_project_print_setup->setToolTip(
		i18n("Show page setup for printing the active table or query"));
	d->action_project_print_setup->setWhatsThis(
		i18n("Shows page setup for printing the active table or query."));

	//EDIT MENU
	d->action_edit_cut = createSharedAction( KStdAction::Cut, "edit_cut");
	d->action_edit_copy = createSharedAction( KStdAction::Copy, "edit_copy");
	d->action_edit_paste = createSharedAction( KStdAction::Paste, "edit_paste");

	if (userMode())
		d->action_edit_paste_special_data_table = 0;
	else {
		d->action_edit_paste_special_data_table = 
			new KAction(i18n("Paste Special->As Data &Table...", "As Data &Table..."),
			"table", 0, this, SLOT(slotEditPasteSpecialDataTable()),
			actionCollection(), "edit_paste_special_data_table");
		d->action_edit_paste_special_data_table->setToolTip(
			i18n("Paste clipboard data as a table"));
		d->action_edit_paste_special_data_table->setWhatsThis(
			i18n("Pastes clipboard data to a table."));
	}

	d->action_edit_copy_special_data_table =
		new KAction(i18n("Copy Special->Table or Query Data...", 
			"Table or Query as Data Table..."),
		"table", 0, this, SLOT(slotEditCopySpecialDataTable()),
		actionCollection(), "edit_copy_special_data_table");
	d->action_edit_copy_special_data_table->setToolTip(
		i18n("Copy selected table or query data to clipboard"));
	d->action_edit_copy_special_data_table->setWhatsThis(
		i18n("Copies selected table or query data to clipboard."));

	d->action_edit_undo = createSharedAction( KStdAction::Undo, "edit_undo");
	d->action_edit_undo->setWhatsThis(i18n("Reverts the most recent editing action."));
	d->action_edit_redo = createSharedAction( KStdAction::Redo, "edit_redo");
	d->action_edit_redo->setWhatsThis(i18n("Reverts the most recent undo action."));

#if 0 //old
	d->action_edit_find = createSharedAction( KStdAction::Find, "edit_find");
	d->action_edit_findnext = createSharedAction( KStdAction::FindNext, "edit_findnext");
	d->action_edit_findprev = createSharedAction( KStdAction::FindPrev, "edit_findprevious");
//! @todo d->action_edit_paste = createSharedAction( KStdAction::Replace, "edit_replace");
#endif

	d->action_edit_find = KStdAction::find(
		this, SLOT(slotEditFind()), actionCollection(), "edit_find" );
//	d->action_edit_find = createSharedAction( KStdAction::Find, "edit_find");
	d->action_edit_findnext = KStdAction::findNext(
		this, SLOT(slotEditFindNext()), actionCollection(), "edit_findnext");
	d->action_edit_findprev = KStdAction::findPrev(
		this, SLOT(slotEditFindPrevious()), actionCollection(), "edit_findprevious");
	d->action_edit_replace = 0;
//! @todo	d->action_edit_replace = KStdAction::replace(
//!		this, SLOT(slotEditReplace()), actionCollection(), "project_print_preview" ); 
	d->action_edit_replace_all = 0;
//! @todo d->action_edit_replace_all = new KAction( i18n("Replace All"), "", 0, 
//!   this, SLOT(slotEditReplaceAll()), actionCollection(), "edit_replaceall");

	d->action_edit_select_all =  createSharedAction( KStdAction::SelectAll, "edit_select_all");

	d->action_edit_delete = createSharedAction(i18n("&Delete"), "editdelete",
		0/*Qt::Key_Delete*/, "edit_delete");
	d->action_edit_delete->setToolTip(i18n("Delete selected object"));
	d->action_edit_delete->setWhatsThis(i18n("Deletes currently selected object."));

	d->action_edit_delete_row = createSharedAction(i18n("Delete Row"), "delete_table_row",
		Qt::CTRL+Qt::Key_Delete, "edit_delete_row");
	d->action_edit_delete_row->setToolTip(i18n("Delete currently selected row"));
	d->action_edit_delete_row->setWhatsThis(i18n("Deletes currently selected row."));

	d->action_edit_clear_table = createSharedAction(i18n("Clear Table Contents"), "clear_table_contents",
		0, "edit_clear_table");
	d->action_edit_clear_table->setToolTip(i18n("Clear table contents"));
	d->action_edit_clear_table->setWhatsThis(i18n("Clears table contents."));
	setActionVolatile( d->action_edit_clear_table, true );

	d->action_edit_edititem = createSharedAction(i18n("Edit Item"), 0, 0, /* CONFLICT in TV: Qt::Key_F2,  */
		"edit_edititem");
	d->action_edit_edititem->setToolTip(i18n("Edit currently selected item"));
	d->action_edit_edititem->setWhatsThis(i18n("Edits currently selected item."));

	d->action_edit_insert_empty_row = createSharedAction(i18n("&Insert Empty Row"), "insert_table_row", 
		Qt::SHIFT | Qt::CTRL | Qt::Key_Insert, "edit_insert_empty_row");
	setActionVolatile( d->action_edit_insert_empty_row, true );
	d->action_edit_insert_empty_row->setToolTip(i18n("Insert one empty row above"));
	d->action_edit_insert_empty_row->setWhatsThis(i18n("Inserts one empty row above currently selected table row."));

	//VIEW MENU
	if (!userMode()) {
		d->action_view_data_mode = new KRadioAction(i18n("&Data View"), "state_data", Qt::Key_F6,
			this, SLOT(slotViewDataMode()), actionCollection(), "view_data_mode");
		d->actions_for_view_modes.insert( Kexi::DataViewMode, d->action_view_data_mode );
		d->action_view_data_mode->setExclusiveGroup("view_mode");
		d->action_view_data_mode->setToolTip(i18n("Switch to data view"));
		d->action_view_data_mode->setWhatsThis(i18n("Switches to data view."));
	}
	else
		d->action_view_data_mode = 0;

	if (!userMode()) {
		d->action_view_design_mode = new KRadioAction(i18n("D&esign View"), "state_edit", Qt::Key_F7,
			this, SLOT(slotViewDesignMode()), actionCollection(), "view_design_mode");
		d->actions_for_view_modes.insert( Kexi::DesignViewMode, d->action_view_design_mode );
		d->action_view_design_mode->setExclusiveGroup("view_mode");
		d->action_view_design_mode->setToolTip(i18n("Switch to design view"));
		d->action_view_design_mode->setWhatsThis(i18n("Switches to design view."));
	}
	else
		d->action_view_design_mode = 0;

	if (!userMode()) {
		d->action_view_text_mode = new KRadioAction(i18n("&Text View"), "state_sql", Qt::Key_F8,
			this, SLOT(slotViewTextMode()), actionCollection(), "view_text_mode");
		d->actions_for_view_modes.insert( Kexi::TextViewMode, d->action_view_text_mode );
		d->action_view_text_mode->setExclusiveGroup("view_mode");
		d->action_view_text_mode->setToolTip(i18n("Switch to text view"));
		d->action_view_text_mode->setWhatsThis(i18n("Switches to text view."));
	}
	else
		d->action_view_text_mode = 0;

	if (d->isProjectNavigatorVisible) {
		d->action_view_nav = new KAction(i18n("Project Navigator"), "", Qt::ALT + Qt::Key_1,
			this, SLOT(slotViewNavigator()), actionCollection(), "view_navigator");
		d->action_view_nav->setToolTip(i18n("Go to project navigator panel"));
		d->action_view_nav->setWhatsThis(i18n("Goes to project navigator panel."));
	}
	else
		d->action_view_nav = 0;

	d->action_view_mainarea = new KAction(i18n("Main Area"), "", Qt::ALT + Qt::Key_2,
		this, SLOT(slotViewMainArea()), actionCollection(), "view_mainarea");
	d->action_view_mainarea->setToolTip(i18n("Go to main area"));
	d->action_view_mainarea->setWhatsThis(i18n("Goes to main area."));

	if (!userMode()) {
		d->action_view_propeditor = new KAction(i18n("Property Editor"), "", Qt::ALT + Qt::Key_3,
			this, SLOT(slotViewPropertyEditor()), actionCollection(), "view_propeditor");
		d->action_view_propeditor->setToolTip(i18n("Go to property editor panel"));
		d->action_view_propeditor->setWhatsThis(i18n("Goes to property editor panel."));
	}
	else
		d->action_view_propeditor = 0;

	//DATA MENU
	d->action_data_save_row = createSharedAction(i18n("&Save Row"), "button_ok", 
		Qt::SHIFT | Qt::Key_Return, "data_save_row");
	d->action_data_save_row->setToolTip(i18n("Save changes made to the current row"));
	d->action_data_save_row->setWhatsThis(i18n("Saves changes made to the current row."));
//temp. disable because of problems with volatile actions	setActionVolatile( d->action_data_save_row, true );

	d->action_data_cancel_row_changes = createSharedAction(i18n("&Cancel Row Changes"), 
		"button_cancel", 0 , "data_cancel_row_changes");
	d->action_data_cancel_row_changes->setToolTip(i18n("Cancel changes made to the current row"));
	d->action_data_cancel_row_changes->setWhatsThis(i18n("Cancels changes made to the current row."));
//temp. disable because of problems with volatile actions	setActionVolatile( d->action_data_cancel_row_changes, true );

	d->action_data_execute = createSharedAction(i18n("&Execute"), "player_play", 0 , "data_execute");
	//d->action_data_execute->setToolTip(i18n("")); //TODO
	//d->action_data_execute->setWhatsThis(i18n("")); //TODO

#ifndef KEXI_NO_UNFINISHED
	action = createSharedAction(i18n("&Filter"), "filter", 0, "data_filter");
	setActionVolatile( action, true );
#endif
//	action->setToolTip(i18n("")); //todo
//	action->setWhatsThis(i18n("")); //todo

//	setSharedMenu("data_sort");
	action = createSharedAction(i18n("&Ascending"), "sort_az", 0, "data_sort_az");
//temp. disable because of problems with volatile actions	setActionVolatile( action, true );
	action->setToolTip(i18n("Sort data in ascending order"));
	action->setWhatsThis(i18n("Sorts data in ascending order (from A to Z and from 0 to 9). Data from selected column is used for sorting."));

	action = createSharedAction(i18n("&Descending"), "sort_za", 0, "data_sort_za");
//temp. disable because of problems with volatile actions	setActionVolatile( action, true );
	action->setToolTip(i18n("Sort data in descending order"));
	action->setWhatsThis(i18n("Sorts data in descending (from Z to A and from 9 to 0). Data from selected column is used for sorting."));

	// - record-navigation related actions
	createSharedAction( KexiRecordNavigator::Actions::moveToFirstRecord(), 0, "data_go_to_first_record");
	createSharedAction( KexiRecordNavigator::Actions::moveToPreviousRecord(), 0, "data_go_to_previous_record");
	createSharedAction( KexiRecordNavigator::Actions::moveToNextRecord(), 0, "data_go_to_next_record");
	createSharedAction( KexiRecordNavigator::Actions::moveToLastRecord(), 0, "data_go_to_last_record");
	createSharedAction( KexiRecordNavigator::Actions::moveToNewRecord(), 0, "data_go_to_new_record");

	//FORMAT MENU
	d->action_format_font = createSharedAction(i18n("&Font..."), "fonts", 0, "format_font");
	d->action_format_font->setToolTip(i18n("Change font for selected object"));
	d->action_format_font->setWhatsThis(i18n("Changes font for selected object."));

	//TOOLS MENU

	//WINDOW MENU
#ifndef Q_WS_WIN
	//KMDI <= 3.5.1 has no shortcut here:
	KAction *closeWindowAction = actionCollection()->action("window_close");
	if (closeWindowAction)
		closeWindowAction->setShortcut(KStdAccel::close());
#endif

	//additional 'Window' menu items
	d->action_window_next = new KAction( i18n("&Next Window"), "",
#ifdef Q_WS_WIN
		Qt::CTRL+Qt::Key_Tab,
#else
		Qt::ALT+Qt::Key_Right,
#endif
		this, SLOT(activateNextWin()), actionCollection(), "window_next");
	d->action_window_next->setToolTip( i18n("Next window") );
	d->action_window_next->setWhatsThis(i18n("Switches to the next window."));

	d->action_window_previous = new KAction( i18n("&Previous Window"), "",
#ifdef Q_WS_WIN
		Qt::CTRL+Qt::SHIFT+Qt::Key_Tab,
#else
		Qt::ALT+Qt::Key_Left,
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

	d->action_show_other = new KActionMenu(i18n("Other"),
		actionCollection(), "options_show_other");
#endif

#ifndef KEXI_NO_CTXT_HELP
	d->action_show_helper = new KToggleAction(i18n("Show Context Help"), "", Qt::CTRL + Qt::Key_H,
	 actionCollection(), "options_show_contexthelp");
#if KDE_IS_VERSION(3,2,90)
	d->action_show_helper->setCheckedState(i18n("Hide Context Help"));
#endif
#endif

#ifdef KEXI_FORMS_SUPPORT
	slotOptionsEnableForms(true, true);
#else
	slotOptionsEnableForms(false, true);
#endif

#ifdef KEXI_REPORTS_SUPPORT
	Kexi::tempShowReports() = true;
#else
	Kexi::tempShowReports() = false;
#endif

#ifdef KEXI_MACROS_SUPPORT
	Kexi::tempShowMacros() = true;
#else
	Kexi::tempShowMacros() = false;
#endif

#ifdef KEXI_SCRIPTS_SUPPORT
	Kexi::tempShowScripts() = true;
#else
	Kexi::tempShowScripts() = false;
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

#ifndef KEXI_NO_FEEDBACK_AGENT
#ifdef FEEDBACK_CLASS
	new KAction(i18n("Give Feedback..."), "messagebox_info", 0,
		this, SLOT(slotStartFeedbackAgent()), actionCollection(), "help_start_feedback_agent");
#endif
#endif
//	KAction *actionSettings = new KAction(i18n("Configure Kexi..."), "configure", 0,
//	 actionCollection(), "kexi_settings");
//	actionSettings->setWhatsThis(i18n("Lets you configure Kexi."));
//	connect(actionSettings, SIGNAL(activated()), this, SLOT(slotShowSettings()));

	// -- add a few missing tooltips (usable especially in Form's "Assign action" dialog)
	if ((action = actionCollection()->action("window_close")))
		action->setToolTip(i18n("Close the current window"));

	// ----- declare action categories, so form's "assign action to button"
	//       (and macros in the future) will be able to recognize category 
	//       of actions and filter them -----------------------------------
//! @todo shouldn't we move this to core?
	Kexi::ActionCategories *acat = Kexi::actionCategories();
	acat->addAction("data_execute", Kexi::PartItemActionCategory);

	//! @todo unused for now
	acat->addWindowAction("data_filter",
		KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

	acat->addWindowAction("data_save_row",
		KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

	acat->addWindowAction("data_cancel_row_changes",
		KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

	acat->addWindowAction("delete_table_row",
		KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

	//! @todo support this in KexiPart::FormObjectType as well
	acat->addWindowAction("data_sort_az",
		KexiPart::TableObjectType, KexiPart::QueryObjectType);

	//! @todo support this in KexiPart::FormObjectType as well
	acat->addWindowAction("data_sort_za",
		KexiPart::TableObjectType, KexiPart::QueryObjectType);

	//! @todo support this in KexiPart::FormObjectType as well
	acat->addWindowAction("edit_clear_table",
		KexiPart::TableObjectType, KexiPart::QueryObjectType);

	//! @todo support this in KexiPart::FormObjectType as well
	acat->addWindowAction("edit_copy_special_data_table", 
		KexiPart::TableObjectType, KexiPart::QueryObjectType);

	// GlobalActions, etc.
	acat->addAction("edit_copy", Kexi::GlobalActionCategory|Kexi::PartItemActionCategory);

	acat->addAction("edit_cut", Kexi::GlobalActionCategory|Kexi::PartItemActionCategory);

	acat->addAction("edit_paste", Kexi::GlobalActionCategory|Kexi::PartItemActionCategory);

	acat->addAction("edit_delete", Kexi::GlobalActionCategory|Kexi::PartItemActionCategory|Kexi::WindowActionCategory,
		KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

	acat->addAction("edit_delete_row", Kexi::GlobalActionCategory|Kexi::WindowActionCategory,
		KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

	acat->addAction("edit_edititem", Kexi::PartItemActionCategory|Kexi::WindowActionCategory,
		KexiPart::TableObjectType, KexiPart::QueryObjectType);

	acat->addAction("edit_find", Kexi::GlobalActionCategory|Kexi::WindowActionCategory,
		KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

	acat->addAction("edit_findnext", Kexi::GlobalActionCategory|Kexi::WindowActionCategory,
		KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

	acat->addAction("edit_findprevious", Kexi::GlobalActionCategory|Kexi::WindowActionCategory,
		KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

	acat->addAction("edit_replace", Kexi::GlobalActionCategory|Kexi::WindowActionCategory,
		KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

	acat->addAction("edit_paste_special_data_table", Kexi::GlobalActionCategory);

	acat->addAction("help_about_app", Kexi::GlobalActionCategory);

	acat->addAction("help_about_kde", Kexi::GlobalActionCategory);

	acat->addAction("help_contents", Kexi::GlobalActionCategory);

	acat->addAction("help_report_bug", Kexi::GlobalActionCategory);

	acat->addAction("help_whats_this", Kexi::GlobalActionCategory);

	acat->addAction("options_configure_keybinding", Kexi::GlobalActionCategory);

	acat->addAction("project_close", Kexi::GlobalActionCategory);

	//! @todo support this in FormObjectType as well
	acat->addAction("project_export_data_table", Kexi::GlobalActionCategory|Kexi::WindowActionCategory,
		KexiPart::TableObjectType, KexiPart::QueryObjectType);

	acat->addAction("project_import_data_table", Kexi::GlobalActionCategory);

	acat->addAction("project_new", Kexi::GlobalActionCategory);

	acat->addAction("project_open", Kexi::GlobalActionCategory);

	//! @todo support this in FormObjectType, ReportObjectType as well as others
	acat->addAction("project_print", Kexi::WindowActionCategory,
		KexiPart::TableObjectType, KexiPart::QueryObjectType);

	//! @todo support this in FormObjectType, ReportObjectType as well as others
	acat->addAction("project_print_preview", Kexi::WindowActionCategory,
		KexiPart::TableObjectType, KexiPart::QueryObjectType);

	//! @todo support this in FormObjectType, ReportObjectType as well as others
	acat->addAction("project_print_setup", Kexi::WindowActionCategory,
		KexiPart::TableObjectType, KexiPart::QueryObjectType);

	acat->addAction("quit", Kexi::GlobalActionCategory);

	acat->addAction("tools_compact_database", Kexi::GlobalActionCategory);

	acat->addAction("tools_import_project", Kexi::GlobalActionCategory);

	acat->addAction("view_data_mode", Kexi::GlobalActionCategory);

	acat->addAction("view_design_mode", Kexi::GlobalActionCategory);

	acat->addAction("view_text_mode", Kexi::GlobalActionCategory);

	acat->addAction("view_mainarea", Kexi::GlobalActionCategory);

	acat->addAction("view_navigator", Kexi::GlobalActionCategory);

	acat->addAction("view_propeditor", Kexi::GlobalActionCategory);

	acat->addAction("window_close", Kexi::GlobalActionCategory | Kexi::WindowActionCategory);
	acat->setAllObjectTypesSupported("window_close", true);

	acat->addAction("window_next", Kexi::GlobalActionCategory);

	acat->addAction("window_previous", Kexi::GlobalActionCategory);

	//skipped - design view only
	acat->addAction("format_font", Kexi::NoActionCategory);
	acat->addAction("project_save", Kexi::NoActionCategory);
	acat->addAction("edit_insert_empty_row", Kexi::NoActionCategory);
	//! @todo support this in KexiPart::TableObjectType, KexiPart::QueryObjectType later
	acat->addAction("edit_select_all", Kexi::NoActionCategory);
	//! @todo support this in KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType later
	acat->addAction("edit_redo", Kexi::NoActionCategory);
	//! @todo support this in KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType later
	acat->addAction("edit_undo", Kexi::NoActionCategory);

	//record-navigation related actions
	acat->addAction("data_go_to_first_record", Kexi::WindowActionCategory,
		KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);
	acat->addAction("data_go_to_previous_record", Kexi::WindowActionCategory,
		KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);
	acat->addAction("data_go_to_next_record", Kexi::WindowActionCategory,
		KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);
	acat->addAction("data_go_to_last_record", Kexi::WindowActionCategory,
		KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);
	acat->addAction("data_go_to_new_record", Kexi::WindowActionCategory,
		KexiPart::TableObjectType, KexiPart::QueryObjectType, KexiPart::FormObjectType);

	//skipped - internal: 
	acat->addAction("tablepart_create", Kexi::NoActionCategory);
	acat->addAction("querypart_create", Kexi::NoActionCategory);
	acat->addAction("formpart_create", Kexi::NoActionCategory);
	acat->addAction("reportpart_create", Kexi::NoActionCategory);
	acat->addAction("macropart_create", Kexi::NoActionCategory);
	acat->addAction("scriptpart_create", Kexi::NoActionCategory);
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

	const bool have_dialog = d->curDialog;
	const bool dialog_dirty = d->curDialog && d->curDialog->dirty();
	const bool readOnly = d->prj && d->prj->dbConnection() && d->prj->dbConnection()->isReadOnly();

	//PROJECT MENU
	d->action_save->setEnabled(have_dialog && dialog_dirty && !readOnly);
	d->action_save_as->setEnabled(have_dialog && !readOnly);
	d->action_project_properties->setEnabled(d->prj);
	d->action_close->setEnabled(d->prj);
	d->action_project_relations->setEnabled(d->prj);
	if (d->action_project_import_data_table)
		d->action_project_import_data_table->setEnabled(d->prj && !readOnly);
	d->action_project_export_data_table->setEnabled( 
		d->curDialog && d->curDialog->part()->info()->isDataExportSupported() 
		&& !d->curDialog->neverSaved() );

	const bool printingActionsEnabled = 
		d->curDialog && d->curDialog->part()->info()->isPrintingSupported()
		&& !d->curDialog->neverSaved();
	d->action_project_print->setEnabled( printingActionsEnabled );
	d->action_project_print_preview->setEnabled( printingActionsEnabled );
	d->action_project_print_setup->setEnabled( printingActionsEnabled );

	//EDIT MENU

	if (d->action_edit_paste_special_data_table)
		d->action_edit_paste_special_data_table->setEnabled(d->prj && !readOnly);

//! @todo "copy special" is currently enabled only for data view mode; 
//! 	what about allowing it to enable in design view for "kexi/table" ?
	if (d->curDialog && d->curDialog->currentViewMode()==Kexi::DataViewMode) {
		KexiPart::Info *activePartInfo = d->curDialog->part()->info();
		d->action_edit_copy_special_data_table->setEnabled(
			activePartInfo ? activePartInfo->isDataExportSupported() : false );
	}
	else
		d->action_edit_copy_special_data_table->setEnabled( false );

	//VIEW MENU
	if (d->action_view_nav)
		d->action_view_nav->setEnabled(d->prj);
	d->action_view_mainarea->setEnabled(d->prj);
	if (d->action_view_propeditor)
		d->action_view_propeditor->setEnabled(d->prj);
	if (d->action_view_data_mode) {
		d->action_view_data_mode->setEnabled( have_dialog && d->curDialog->supportsViewMode(Kexi::DataViewMode) );
		if (!d->action_view_data_mode->isEnabled())
			d->action_view_data_mode->setChecked(false);
	}
	if (d->action_view_design_mode) {
		d->action_view_design_mode->setEnabled( have_dialog && d->curDialog->supportsViewMode(Kexi::DesignViewMode) );
		if (!d->action_view_design_mode->isEnabled())
			d->action_view_design_mode->setChecked(false);
	}
	if (d->action_view_text_mode) {
		d->action_view_text_mode->setEnabled( have_dialog && d->curDialog->supportsViewMode(Kexi::TextViewMode) );
		if (!d->action_view_text_mode->isEnabled())
			d->action_view_text_mode->setChecked(false);
	}
#ifndef KEXI_NO_CTXT_HELP
	d->action_show_helper->setEnabled(d->prj);
#endif

	//CREATE MENU
	if (d->createMenu)
		d->createMenu->setEnabled(d->prj);

	// DATA MENU
	//d->action_data_execute->setEnabled( d->curDialog && d->curDialog->part()->info()->isExecuteSupported() );

	//TOOLS MENU
	// "compact db" supported if there's no db or the current db supports compacting and is opened r/w:
	d->action_tools_compact_database->setEnabled( 
		!d->prj || !readOnly && d->prj && d->prj->dbConnection() 
		&& (d->prj->dbConnection()->driver()->features() & KexiDB::Driver::CompactingDatabaseSupported) );

	//WINDOW MENU
	if (d->action_window_next) {
		d->action_window_next->setEnabled(!m_pDocumentViews->isEmpty());
		d->action_window_previous->setEnabled(!m_pDocumentViews->isEmpty());
	}

	//DOCKS
	if (d->nav)
		d->nav->setEnabled(d->prj);
	if (d->propEditor)
		d->propEditorTabWidget->setEnabled(d->prj);
}

void KexiMainWindowImpl::invalidateViewModeActions()
{
	if (d->curDialog) {
		//update toggle action
		if (d->curDialog->currentViewMode()==Kexi::DataViewMode) {
			if (d->action_view_data_mode)
				d->action_view_data_mode->setChecked( true );
		}
		else if (d->curDialog->currentViewMode()==Kexi::DesignViewMode) {
			if (d->action_view_design_mode)
				d->action_view_design_mode->setChecked( true );
		}
		else if (d->curDialog->currentViewMode()==Kexi::TextViewMode) {
			if (d->action_view_text_mode)
				d->action_view_text_mode->setChecked( true );
		}
	}
}

tristate KexiMainWindowImpl::startup()
{
	switch (Kexi::startupHandler().action()) {
	case KexiStartupHandler::CreateBlankProject:
		if (d->propEditor)
			makeDockInvisible( manager()->findWidgetParentDock(d->propEditorTabWidget) );
		return createBlankProject();
	case KexiStartupHandler::CreateFromTemplate:
		return createProjectFromTemplate(*Kexi::startupHandler().projectData());
	case KexiStartupHandler::OpenProject:
		return openProject(*Kexi::startupHandler().projectData());
	case KexiStartupHandler::ImportProject:
		return showProjectMigrationWizard(
			Kexi::startupHandler().importActionData().mimeType,
			Kexi::startupHandler().importActionData().fileName
		);
	default:;
		if (d->propEditor)
			makeDockInvisible( manager()->findWidgetParentDock(d->propEditorTabWidget) );
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

tristate KexiMainWindowImpl::openProject(const KexiProjectData& projectData)
{
	KexiProjectData *newProjectData = new KexiProjectData(projectData);
//	if (userMode()) {
		//TODO: maybe also auto allow to open objects...
//		return initUserModeMode(newProjectData);
//	}
	createKexiProject( newProjectData );
	if (!newProjectData->connectionData()->savePassword 
		&& newProjectData->connectionData()->password.isEmpty()
		&& newProjectData->connectionData()->fileName().isEmpty() //! @todo temp.: change this if there are file-based drivers requiring a password
	)
	{
		//ask for password
		KexiDBPasswordDialog pwdDlg(this, *newProjectData->connectionData(), 
			false /*!showDetailsButton*/);
		if (QDialog::Accepted!=pwdDlg.exec()) {
			delete d->prj;
			d->prj = 0;
			return cancelled;
		}
	}
	bool incompatibleWithKexi;
	tristate res = d->prj->open(incompatibleWithKexi);
	if (~res) {
		delete d->prj;
		d->prj = 0;
		return cancelled;
	}
	else if (!res) {
		delete d->prj;
		d->prj = 0;
		if (incompatibleWithKexi) {
			if (KMessageBox::Yes == KMessageBox::questionYesNo(this,
				i18n("<qt>Database project %1 does not appear to have been created using Kexi.<br><br>"
				"Do you want to import it as a new Kexi project?</qt>").arg(projectData.infoString()),
				0, KGuiItem(i18n("Import Database", "&Import..."), "database_import"),
				KStdGuiItem::quit()))
			{
				const bool anotherProjectAlreadyOpened = d->prj;
				tristate res = showProjectMigrationWizard("application/x-kexi-connectiondata", 
					projectData.databaseName(), projectData.constConnectionData());
				
				if (!anotherProjectAlreadyOpened) //the project could have been opened within this instance
					return res;

				//always return cancelled because even if migration succeeded, new Kexi instance 
				//will be started if user wanted to open the imported db
				return cancelled;
			}
			return cancelled;
		}
		return false;
	}
	initNavigator();
	Kexi::recentProjects().addProjectData( newProjectData );
	updateReadOnlyState();
	invalidateActions();
//	d->disableErrorMessages = true;
	enableMessages( false );

	QTimer::singleShot(1, this, SLOT(slotAutoOpenObjectsLater()));
	return true;
}

tristate KexiMainWindowImpl::createProjectFromTemplate(const KexiProjectData& projectData)
{
	QStringList mimetypes;
	mimetypes.append( KexiDB::Driver::defaultFileBasedDriverMimeType() );
	QString fname;
	const QString startDir(":OpenExistingOrCreateNewProject"/*as in KexiNewProjectWizard*/);
	const QString caption( i18n("Select New Project's Location") );
	
	while (true) {
#ifdef Q_WS_WIN
	//! @todo remove
		QString recentDir = KGlobalSettings::documentPath();
		if (fname.isEmpty() && !projectData.constConnectionData()->dbFileName().isEmpty()) //propose filename from db template name
		  fname = KFileDialog::getStartURL(startDir, recentDir).path() 
				+ '/' + projectData.constConnectionData()->dbFileName();
		fname = QFileDialog::getSaveFileName( 
			KFileDialog::getStartURL(fname.isEmpty() ? startDir : fname, recentDir).path(),
			KexiUtils::fileDialogFilterStrings(mimetypes, false),
			this, "CreateProjectFromTemplate", caption);
		if ( !fname.isEmpty() ) {
			//save last visited path
			KURL url;
			url.setPath( fname );
			if (url.isLocalFile())
				KRecentDirs::add(startDir, url.directory());
		}
#else
		Q_UNUSED(projectData);
		if (fname.isEmpty() &&
			!projectData.constConnectionData()->dbFileName().isEmpty())
		{
			//propose filename from db template name
		  fname = projectData.constConnectionData()->dbFileName();
		}
		const bool specialDir = fname.isEmpty();
	kdDebug() << fname << "............." << endl;
		KFileDialog dlg( specialDir ? startDir : QString::null, 
			mimetypes.join(" "), this, "filedialog", true);
		if ( !specialDir )
			dlg.setSelection( fname ); // may also be a filename
		dlg.setOperationMode( KFileDialog::Saving );
		dlg.setCaption( caption );
		dlg.exec();
		fname = dlg.selectedFile();
		if (!fname.isEmpty())
			KRecentDocument::add(fname);
//		fname = KFileDialog::getSaveFileName(fname.isEmpty() ? startDir : fname, 
	//		mimetypes.join(" "), this, caption);
#endif
		if ( fname.isEmpty() )
			return cancelled;
		if (KexiStartupFileDialog::askForOverwriting(fname, this))
			break;
	}

	if (KexiUtils::CopySuccess != KexiUtils::copyFile(
		projectData.constConnectionData()->fileName(), fname ))
	{
		return false;
	}

	return openProject(fname, 0, QString::null, projectData.autoopenObjects/*copy*/);
}

void KexiMainWindowImpl::updateReadOnlyState()
{
	const bool readOnly = d->prj && d->prj->dbConnection() && d->prj->dbConnection()->isReadOnly();
	d->statusBar->setReadOnlyFlag( readOnly );
	if (d->nav)
		d->nav->setReadOnly(readOnly);
	// update "insert ....." actions for every part
	KActionCollection *ac = actionCollection();
	for (KexiPart::PartInfoListIterator it(*Kexi::partManager().partInfoList()); it.current(); ++it) {
		KAction *a = ac->action( KexiPart::nameForCreateAction( *it.current() ) );
		if (a)
			a->setEnabled(!readOnly);
	}
}

void KexiMainWindowImpl::slotAutoOpenObjectsLater()
{
	QString not_found_msg;
	bool openingCancelled;
	//ok, now open "autoopen: objects
	if (d->prj) {
		for (QValueList<KexiProjectData::ObjectInfo>::ConstIterator it = 
				d->prj->data()->autoopenObjects.constBegin();
			it != d->prj->data()->autoopenObjects.constEnd(); ++it )
		{
			KexiProjectData::ObjectInfo info = *it;
			KexiPart::Info *i = Kexi::partManager().infoForMimeType( 
				QCString("kexi/")+info["type"].lower().latin1() );
			if (!i) {
				not_found_msg += "<li>";
				if (!info["name"].isEmpty())
					not_found_msg += (QString("\"") + info["name"] + "\" - ");
				if (info["action"]=="new")
					not_found_msg += i18n("cannot create object - unknown object type \"%1\"")
						.arg(info["type"]);
				else
					not_found_msg += i18n("unknown object type \"%1\"").arg(info["type"]);
				not_found_msg += internalReason(&Kexi::partManager())+"<br></li>";
				continue;
			}
			// * NEW
			if (info["action"]=="new") {
				if (!newObject( i, openingCancelled) && !openingCancelled) {
					not_found_msg += "<li>";
					not_found_msg += (i18n("cannot create object of type \"%1\"").arg(info["type"])+
						internalReason(d->prj)+"<br></li>");
				}
				else
					d->wasAutoOpen = true;
				continue;
			}

			KexiPart::Item *item = d->prj->item(i, info["name"]);

			if (!item) {
				QString taskName;
				if (info["action"]=="print-preview")
					taskName = i18n("making print preview for");
				else if (info["action"]=="print")
					taskName = i18n("printing");
				else if (info["action"]=="execute")
					taskName = i18n("\"executing object\" action", "executing");
				else 
					taskName = i18n("opening");

				not_found_msg += (QString("<li>")+ taskName + " \"" + info["name"] + "\" - ");
				if ("table"==info["type"].lower())
					not_found_msg += i18n("table not found");
				else if ("query"==info["type"].lower())
					not_found_msg += i18n("query not found");
				else if ("macro"==info["type"].lower())
					not_found_msg += i18n("macro not found");
				else if ("script"==info["type"].lower())
					not_found_msg += i18n("script not found");
				else
					not_found_msg += i18n("object not found");
				not_found_msg += (internalReason(d->prj)+"<br></li>");
				continue;
			}
			// * EXECUTE, PRINT, PRINT PREVIEW
			if (info["action"]=="execute") {
				tristate res = executeItem(item);
				if (false == res) {
					not_found_msg += ( QString("<li>\"")+ info["name"] + "\" - " + i18n("cannot execute object")+
					internalReason(d->prj)+"<br></li>" );
				}
				continue;
			}
			else if (info["action"]=="print") {
				tristate res = printItem(item);
				if (false == res) {
					not_found_msg += ( QString("<li>\"")+ info["name"] + "\" - " + i18n("cannot print object")+
					internalReason(d->prj)+"<br></li>" );
				}
				continue;
			}
			else if (info["action"]=="print-preview") {
				tristate res = printPreviewForItem(item);
				if (false == res) {
					not_found_msg += ( QString("<li>\"")+ info["name"] + "\" - " + i18n("cannot make print preview of object")+
					internalReason(d->prj)+"<br></li>" );
				}
				continue;
			}

			int viewMode;
			if (info["action"]=="open")
				viewMode = Kexi::DataViewMode;
			else if (info["action"]=="design")
				viewMode = Kexi::DesignViewMode;
			else if (info["action"]=="edittext")
				viewMode = Kexi::TextViewMode;
			else
				continue; //sanity

			QString openObjectMessage;
			if (!openObject(item, viewMode, openingCancelled, 0, &openObjectMessage) 
				&& (!openingCancelled || !openObjectMessage.isEmpty()))
			{
				not_found_msg += (QString("<li>\"")+ info["name"] + "\" - ");
				if (openObjectMessage.isEmpty())
					not_found_msg += i18n("cannot open object");
				else
					not_found_msg += openObjectMessage;
				not_found_msg += internalReason(d->prj) + "<br></li>";
				continue;
			}
			else {
				d->wasAutoOpen = true;
			}
		}
	}
	enableMessages( true );
//	d->disableErrorMessages = false;

	if (!not_found_msg.isEmpty())
		showErrorMessage(i18n("You have requested selected objects to be automatically opened "
			"or processed on startup. Several objects cannot be opened or processed."),
			QString("<ul>%1</ul>").arg(not_found_msg) );

	d->updatePropEditorVisibility(d->curDialog ? d->curDialog->currentViewMode() : 0);
#if defined(KDOCKWIDGET_P)
	if (d->propEditor) {
				KDockWidget *dw = (KDockWidget *)d->propEditorTabWidget->parentWidget();
				KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
				if(ds)
					ds->setSeparatorPosInPercent(d->config->readNumEntry("RightDockPosition", 80/* % */));
	}
#endif

	updateAppCaption();

//	d->navToolWindow->wrapperWidget()->setFixedWidth(200);
//js TODO: make visible FOR OTHER MODES if needed
	if (mdiMode()==KMdi::ChildframeMode || mdiMode()==KMdi::TabPageMode) {
		//make docks visible again
		if (!d->navToolWindow->wrapperWidget()->isVisible())
			static_cast<KDockWidget*>(d->navToolWindow->wrapperWidget())->makeDockVisible();
//		if (!d->propEditorToolWindow->wrapperWidget()->isVisible())
//			static_cast<KDockWidget*>(d->propEditorToolWindow->wrapperWidget())->makeDockVisible();
	}

	//	if (!d->prj->data()->autoopenObjects.isEmpty())
	d->restoreNavigatorWidth();

#ifndef PROPEDITOR_VISIBILITY_CHANGES
//	KDockWidget *dw = (KDockWidget *)d->nav->parentWidget();
//	KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
//	const int pos = ds->separatorPos();

	//if (!d->curDialog || d->curDialog->currentViewMode()==Kexi::DataViewMode)
//		d->propEditorToolWindow->hide();

//	ds->setSeparatorPos( pos, true );
#endif
	if (d->nav) {
		d->nav->updateGeometry();
	}
	qApp->processEvents();
	emit projectOpened();
}

tristate KexiMainWindowImpl::closeProject()
{
#ifndef KEXI_NO_PENDING_DIALOGS
	if (d->pendingDialogsExist()) {
		kdDebug() << "KexiMainWindowImpl::closeProject() pendingDialogsExist..." << endl;
		d->actionToExecuteWhenPendingJobsAreFinished = Private::CloseProjectAction;
		return cancelled;
	}
#endif

	//only save nav. visibility setting is project is opened
	d->saveSettingsForShowProjectNavigator = d->prj && d->isProjectNavigatorVisible;

	if (!d->prj)
		return true;

	{
		// make sure the project can be closed
		bool cancel = false;
		emit acceptProjectClosingRequested(cancel);
		if (cancel)
			return cancelled;
	}

	d->dialogExistedBeforeCloseProject = !d->curDialog.isNull();

#if defined(KDOCKWIDGET_P)
	//remember docks position - will be used on storeSettings()
	if (d->propEditor) {
		KDockWidget *dw = (KDockWidget *)d->propEditorTabWidget->parentWidget();
		KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
		if (ds)
			d->propEditorDockSeparatorPos = ds->separatorPosInPercent();
	}
	if (d->nav) {
//		makeDockInvisible( manager()->findWidgetParentDock(d->propEditor) );

		if (d->propEditor) {
			if (d->openedDialogsCount() == 0)
				makeWidgetDockVisible(d->propEditorTabWidget);
			KDockWidget *dw = (KDockWidget *)d->propEditorTabWidget->parentWidget();
			KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
			if(ds)
				ds->setSeparatorPosInPercent(80);
		}

		KDockWidget *dw = (KDockWidget *)d->nav->parentWidget();
		KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
		int dwWidth = dw->width();
		if (ds) {
				if (d->openedDialogsCount()!=0 && d->propEditorTabWidget && d->propEditorTabWidget->isVisible())
					d->navDockSeparatorPos = ds->separatorPosInPercent();
				else
					d->navDockSeparatorPos = (100 * dwWidth) / width();

//				int navDockSeparatorPosWithAutoOpen = (100 * dw->width()) / width() + 4;
//				d->navDockSeparatorPos = (100 * dw->width()) / width() + 1;
		}
	}
#endif

	//close each window, optionally asking if user wants to close (if data changed)
	while (!d->curDialog.isNull()) {
		tristate res = closeDialog( d->curDialog );
		if (!res || ~res)
			return res;
	}

	// now we will close for sure
	emit beforeProjectClosing();

	if (!d->prj->closeConnection())
		return false;

	if(d->nav)
	{
		d->navWasVisibleBeforeProjectClosing = manager()->findWidgetParentDock(d->nav)->isVisible();
		d->nav->clear();
#if 0 //do not confuse users 
		d->navToolWindow->hide();
#endif
	}

	if (d->propEditor)
		makeDockInvisible( manager()->findWidgetParentDock(d->propEditorTabWidget) );

//	if(d->propEditorToolWindow)
	//	d->propEditorToolWindow->hide();

	d->clearDialogs(); //sanity!
	delete d->prj;
	d->prj=0;

//	Kexi::partManager().unloadAllParts();

	updateReadOnlyState();
	invalidateActions();
//	if (!userMode())
		updateAppCaption();

	emit projectClosed();
	return true;
}

void KexiMainWindowImpl::initContextHelp() {
#ifndef KEXI_NO_CTXT_HELP
	d->ctxHelp=new KexiContextHelp(this,this);
/*todo
	d->ctxHelp->setContextHelp(i18n("Welcome"),i18n("The <B>KEXI team</B> wishes you a lot of productive work, "
		"with this product. <BR><HR><BR>If you have found a <B>bug</B> or have a <B>feature</B> request, please don't "
		"hesitate to report it at our <A href=\"http://www.kexi-project.org/cgi-bin/bug.pl\"> issue "
		"tracking system </A>.<BR><HR><BR>If you would like to <B>join</B> our effort, the <B>development</B> documentation "
		"at <A href=\"http://www.kexi-project.org\">www.kexi-project.org</A> is a good starting point."),0);
*/
	addToolWindow(d->ctxHelp,KDockWidget::DockBottom | KDockWidget::DockLeft,getMainDockWidget(),20);
#endif
}

void KexiMainWindowImpl::initNavigator()
{
	if (!d->isProjectNavigatorVisible)
		return;

	if(!d->nav)
	{
		d->nav = new KexiBrowser(this, this);
		d->nav->installEventFilter(this);
		d->navToolWindow = addToolWindow(d->nav, KDockWidget::DockLeft, getMainDockWidget(), 20/*, lv, 35, "2"*/);
//		d->navToolWindow->hide();

		connect(d->nav,SIGNAL(openItem(KexiPart::Item*,int)),this,SLOT(openObject(KexiPart::Item*,int)));
		connect(d->nav,SIGNAL(openOrActivateItem(KexiPart::Item*,int)),
			this,SLOT(openObjectFromNavigator(KexiPart::Item*,int)));
		connect(d->nav,SIGNAL(newItem( KexiPart::Info* )),
			this,SLOT(newObject(KexiPart::Info*)));
		connect(d->nav,SIGNAL(removeItem(KexiPart::Item*)),
			this,SLOT(removeObject(KexiPart::Item*)));
		connect(d->nav,SIGNAL(renameItem(KexiPart::Item*,const QString&, bool&)),
			this,SLOT(renameObject(KexiPart::Item*,const QString&, bool&)));
		connect(d->nav,SIGNAL(executeItem(KexiPart::Item*)),
			this,SLOT(executeItem(KexiPart::Item*)));
		connect(d->nav,SIGNAL(exportItemAsDataTable(KexiPart::Item*)),
			this,SLOT(exportItemAsDataTable(KexiPart::Item*)));
		connect(d->nav,SIGNAL(printItem( KexiPart::Item* )),
			this,SLOT(printItem(KexiPart::Item*)));
		connect(d->nav,SIGNAL(pageSetupForItem( KexiPart::Item*)),
			this,SLOT(showPageSetupForItem(KexiPart::Item*)));
		if (d->prj) {//connect to the project
			connect(d->prj, SIGNAL(itemRemoved(const KexiPart::Item&)),
				d->nav, SLOT(slotRemoveItem(const KexiPart::Item&)));
		}
		connect(d->nav,SIGNAL(selectionChanged(KexiPart::Item*)),
			this,SLOT(slotPartItemSelectedInNavigator(KexiPart::Item*)));

//		d->restoreNavigatorWidth();
	}
	if(d->prj->isConnected()) {
		QString partManagerErrorMessages;
		d->nav->setProject( d->prj, QString::null/*all mimetypes*/, &partManagerErrorMessages );
		if (!partManagerErrorMessages.isEmpty()) {
			showWarningContinueMessage(partManagerErrorMessages, QString::null,
				"dontShowWarningsRelatedToPluginsLoading");
		}
	}
	connect(d->prj, SIGNAL(newItemStored(KexiPart::Item&)), d->nav, SLOT(addItem(KexiPart::Item&)));
	d->nav->setFocus();

	if (d->forceShowProjectNavigatorOnCreation) {
		slotViewNavigator();
		d->forceShowProjectNavigatorOnCreation = false;
	}
	else if (d->forceHideProjectNavigatorOnCreation) {
		d->navToolWindow->hide();
//		makeDockInvisible( manager()->findWidgetParentDock(d->nav) );
		d->forceHideProjectNavigatorOnCreation = false;
	}

	invalidateActions();
}

void KexiMainWindowImpl::slotLastActions()
{
#if defined(KDOCKWIDGET_P)
	if (mdiMode()==KMdi::ChildframeMode || mdiMode()==KMdi::TabPageMode) {
//		KDockWidget *dw = (KDockWidget *)d->propEditor->parentWidget();
		//KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
//		Q_UNUSED(ds);
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
	if (!d->propEditor) {
//TODO: FIX LAYOUT PROBLEMS
		d->propEditorTabWidget = new KTabWidget(this);
		d->propEditorTabWidget->hide();
		d->propEditor = new KexiPropertyEditorView(this, d->propEditorTabWidget);
		d->propEditorTabWidget->setCaption(d->propEditor->caption());
		d->propEditorTabWidget->addTab(d->propEditor, i18n("Properties"));
		d->propEditor->installEventFilter(this);
		d->propEditorToolWindow = addToolWindow(d->propEditorTabWidget,
			KDockWidget::DockRight, getMainDockWidget(), 20);

		d->config->setGroup("PropertyEditor");
		int size = d->config->readNumEntry("FontSize", -1);
		QFont f( Kexi::smallFont() );
		if (size>0)
			f.setPixelSize( size );
		d->propEditorTabWidget->setFont(f);

		if (mdiMode()==KMdi::ChildframeMode || mdiMode()==KMdi::TabPageMode) {
		KDockWidget *dw = (KDockWidget *)d->propEditorTabWidget->parentWidget();
	#if defined(KDOCKWIDGET_P)
			KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
//			ds->setKeepSize(true);
			makeWidgetDockVisible(d->propEditorTabWidget);
	//		ds->show();
		//	ds->resize(400, ds->height());
	//		ds->setSeparatorPos(400, true);
	//		ds->setForcedFixedWidth( dw, 400 );
	//		ds->setSeparatorPos(600, true);


			d->config->setGroup("MainWindow");
			ds->setSeparatorPosInPercent(d->config->readNumEntry("RightDockPosition", 80/* % */));
//			makeDockInvisible( manager()->findWidgetParentDock(d->propEditor) );

	//		ds->setForcedFixedWidth( dw, d->config->readNumEntry("RightDockPosition", 80) );
		//	ds->resize(400, ds->height());
		//	dw->resize(400, dw->height());
	#endif

	//1		dw->setMinimumWidth(200);

	//	ds->setSeparatorPos(d->propEditor->sizeHint().width(), true);

			//heh, this is for IDEAl only, I suppose?
	//js		if (m_rightContainer) {
	//js			m_rightContainer->setForcedFixedWidth( 400 );
	//js		}
		}

	//	int w = d->propEditor->width();
	/*    KMdiToolViewAccessor *tmp=createToolWindow();
			tmp->setWidgetToWrap(d->propEditor);
		d->propEditor->show(); // I'm not sure, if this is a bug in kdockwidget, which I would better fix there
			tmp->show(KDockWidget::DockRight,getMainDockWidget(),20);
	*/
	}
//	makeDockInvisible(manager()->findWidgetParentDock(d->propEditorTabWidget));
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

void KexiMainWindowImpl::slotLastChildViewClosed() //slotLastChildFrmClosed()
{
	if (m_pDocumentViews->count()>0) //a fix for KMDI bug (will be fixed in KDE 3.4)
		return;

	slotCaptionForCurrentMDIChild(false);
	activeWindowChanged(0);

//js: too WEIRD	if (d->propEditor)
//js: too WEIRD		makeDockInvisible( manager()->findWidgetParentDock(d->propEditorTabWidget) );
//	if (d->propEditorToolWindow)
	//	d->propEditorToolWindow->hide();
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
#ifndef KEXI_NO_PENDING_DIALOGS
	if (d->pendingDialogsExist()) {
		kdDebug() << "KexiMainWindowImpl::queryClose() pendingDialogsExist..." << endl;
		d->actionToExecuteWhenPendingJobsAreFinished = Private::QuitAction;
		return false;
	}
#endif
//	storeSettings();
	const tristate res = closeProject();
	if (~res)
		return false;

	if (res==true)
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
	applyMainWindowSettings( d->config, "MainWindow" );

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

	const bool showProjectNavigator = d->config->readBoolEntry("ShowProjectNavigator", true);

	switch(mdimode)
	{
/*		case KMdi::ToplevelMode:
			switchToToplevelMode();
			m_pTaskBar->switchOn(true);
			break;*/
		case KMdi::ChildframeMode:
			switchToChildframeMode(false);
			m_pTaskBar->switchOn(true);

			// restore a possible maximized Childframe mode,
			// will be used in KexiMainWindowImpl::addWindow()
			d->maximizeFirstOpenedChildFrm = d->config->readBoolEntry("maximized childframes", true);
			setEnableMaximizedChildFrmMode(d->maximizeFirstOpenedChildFrm);

			if (!showProjectNavigator) {
				//it's visible by default but we want to hide it on navigator creation
				d->forceHideProjectNavigatorOnCreation = true;
			}

			break;

#define DEFAULT_MDI_MODE KMdi::IDEAlMode

		case DEFAULT_MDI_MODE:
		default:
			switchToIDEAlMode(false);
			if (showProjectNavigator) {
				//it's invisible by default but we want to show it on navigator creation
				d->forceShowProjectNavigatorOnCreation = true;
			}
			break;
/*		case KMdi::TabPageMode:
			switchToTabPageMode();
			break;
*/
	}

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
	KMdi::MdiMode modeToSave = mdiMode();
	if (d->mdiModeToSwitchAfterRestart!=(KMdi::MdiMode)0)
		modeToSave = d->mdiModeToSwitchAfterRestart;
	if (modeToSave == DEFAULT_MDI_MODE)
		d->config->deleteEntry("MDIMode");
	else
		d->config->writeEntry("MDIMode", modeToSave);
	d->config->writeEntry("maximized childframes", isInMaximizedChildFrmMode());

//	if (manager()->findWidgetParentDock(d->nav)->isVisible())
	if (d->saveSettingsForShowProjectNavigator) {
		if (d->navWasVisibleBeforeProjectClosing)
			d->config->deleteEntry("ShowProjectNavigator");
		else
			d->config->writeEntry("ShowProjectNavigator", false);
	}

	if (modeToSave==KMdi::ChildframeMode || modeToSave==KMdi::TabPageMode) {
		if (d->propEditor && d->propEditorDockSeparatorPos >= 0 && d->propEditorDockSeparatorPos <= 100) {
			d->config->setGroup("MainWindow");
			d->config->writeEntry("RightDockPosition", d->propEditorDockSeparatorPos);
		}
		else
			d->propEditorDockSeparatorPos = 80;
		if (d->nav && d->navDockSeparatorPos >= 0 && d->navDockSeparatorPos <= 100) {
			d->config->setGroup("MainWindow");
			//KDockWidget *dw = (KDockWidget *)d->nav->parentWidget();
			//int w = dw->width();
			//int ww = width();
			//int d1 = (100 * dw->width()) / width() + 1;
			//KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
			//int d2 = ds->separatorPosInPercent();
			if (d->wasAutoOpen && d->dialogExistedBeforeCloseProject) {
#ifdef Q_WS_WIN
				d->config->writeEntry("LeftDockPositionWithAutoOpen",
					d->navDockSeparatorPos);
#endif
//			d->config->writeEntry("LeftDockPosition", dw->width());
//			d->config->writeEntry("LeftDockPosition", d->nav->width());
			} else {
#ifdef Q_WS_WIN
				if (d->dialogExistedBeforeCloseProject)
					d->config->writeEntry("LeftDockPosition", d->navDockSeparatorPos);
				else
					d->config->writeEntry("LeftDockPosition", qRound(double(d->navDockSeparatorPos) / 0.77
					 / (double(d->propEditorDockSeparatorPos) / 80) ));
#endif
			}
		}
	}

	if (d->propEditor) {
		d->config->setGroup("PropertyEditor");
		d->config->writeEntry("FontSize", d->propEditorTabWidget->font().pixelSize());
	}
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
	if(dlg->id() != -1) {
		d->insertDialog(dlg);
	}
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

void KexiMainWindowImpl::updateCustomPropertyPanelTabs(KexiDialogBase *prevDialog, int prevViewMode)
{
	updateCustomPropertyPanelTabs(
		prevDialog ? prevDialog->part() : 0,
		prevDialog ? prevDialog->currentViewMode() : prevViewMode,
		d->curDialog ? d->curDialog->part() : 0,
		d->curDialog ? d->curDialog->currentViewMode() : Kexi::NoViewMode
	);
}

void KexiMainWindowImpl::updateCustomPropertyPanelTabs(
	KexiPart::Part *prevDialogPart, int prevViewMode, KexiPart::Part *curDialogPart, int curViewMode )
{
	if (!d->propEditorTabWidget)
		return;

	if (!curDialogPart
		|| (/*prevDialogPart &&*/ curDialogPart
		    && (prevDialogPart!=curDialogPart || prevViewMode!=curViewMode)
		 ))
	{
		if (d->partForPreviouslySetupPropertyPanelTabs) {
			//remember current page number for this part
			if (prevViewMode==Kexi::DesignViewMode && 
				((KexiPart::Part*)d->partForPreviouslySetupPropertyPanelTabs != curDialogPart) //part changed
				|| curViewMode!=Kexi::DesignViewMode) //..or switching to other view mode
			{
				d->recentlySelectedPropertyPanelPages.insert( d->partForPreviouslySetupPropertyPanelTabs, 
					d->propEditorTabWidget->currentPageIndex() );
			}
		}
		
		//delete old custom tabs (other than 'property' tab)
		const uint count = d->propEditorTabWidget->count();
		for (uint i=1; i < count; i++)
			d->propEditorTabWidget->removePage( d->propEditorTabWidget->page(1) );
	}

	//don't change anything if part is not switched nor view mode changed
	if ((!prevDialogPart && !curDialogPart)
		|| (prevDialogPart == curDialogPart && prevViewMode==curViewMode)
		|| (curDialogPart && curViewMode!=Kexi::DesignViewMode))
	{
		//new part for 'previously setup tabs'
		d->partForPreviouslySetupPropertyPanelTabs = curDialogPart;
		return;
	}

	if (curDialogPart) {
		//recreate custom tabs
		curDialogPart->setupCustomPropertyPanelTabs(d->propEditorTabWidget, this);

		//restore current page number for this part
		if (d->recentlySelectedPropertyPanelPages.contains( curDialogPart )) {
			d->propEditorTabWidget->setCurrentPage( 
				d->recentlySelectedPropertyPanelPages[ curDialogPart ] 
			);
		}
	}

	//new part for 'previously setup tabs'
	d->partForPreviouslySetupPropertyPanelTabs = curDialogPart;
}

void KexiMainWindowImpl::activeWindowChanged(KMdiChildView *v)
{
	KexiDialogBase *dlg = static_cast<KexiDialogBase *>(v);
	kdDebug() << "KexiMainWindowImpl::activeWindowChanged() to = " << (dlg ? dlg->caption() : "<none>") << endl;

	KXMLGUIClient *client=0; //common for all views
	KXMLGUIClient *viewClient=0; //specific for current dialog's view
	KexiDialogBase* prevDialog = d->curDialog;

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

//moved below:	propertySetSwitched(d->curDialog);

	updateCustomPropertyPanelTabs(prevDialog, prevDialog ? prevDialog->currentViewMode() : Kexi::NoViewMode);

	// inform the current view of the new dialog about property switching
	// (this will also call KexiMainWindowImpl::propertySetSwitched() to update the current property editor's set
	if (dialogChanged && d->curDialog)
		d->curDialog->selectedView()->propertySetSwitched();

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
	d->updateFindDialogContents();
	if (dlg)
		dlg->setFocus();
}

bool
KexiMainWindowImpl::activateWindow(int id)
{
	kdDebug() << "KexiMainWindowImpl::activateWindow()" << endl;
#ifndef KEXI_NO_PENDING_DIALOGS
	Private::PendingJobType pendingType;
	return activateWindow( d->openedDialogFor( id, pendingType ) );
#else
	return activateWindow( d->openedDialogFor( id ) );
#endif
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
	d->removeDialog(dlg->id());
#ifndef KEXI_NO_PENDING_DIALOGS
	d->removePendingDialog(dlg->id());
#endif

	//focus navigator if nothing else available
	if (d->openedDialogsCount() == 0)
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
	if (!d->prj) {
		//create within this instance
		createBlankProject();
		return;
	}
//TODO use KexiStartupDialog(KexiStartupDialog::Templates...)

	bool cancel;
	QString fileName;
	KexiProjectData *new_data = createBlankProjectData(
		cancel,
		false, /* do not confirm prj overwrites: user will be asked on process startup */
		&fileName //shortcut fname
	);
	if (!new_data)
		return;

  QStringList args;
	args << qApp->applicationFilePath() << "-create-opendb";
	if (new_data->connectionData()->fileName().isEmpty()) {
		//server based - pass .kexic file
		if (fileName.isEmpty())
			return;
		args << new_data->databaseName() << fileName;
		//args << "--skip-conn-dialog"; //user does not expect conn. dialog to be shown here
	}
	else {
		//file based
		fileName = new_data->connectionData()->fileName();
		args << fileName;
	}
//todo:			pass new_data->caption()
	//start new instance
//! @todo use KProcess?
	QProcess proc(args, this, "process");
	proc.setCommunication((QProcess::Communication)0);
//		proc.setWorkingDirectory( QFileInfo(new_data->connectionData()->fileName()).dir(true) );
	proc.setWorkingDirectory( QFileInfo(fileName).dir(true) );
	if (!proc.start()) {
			d->showStartProcessMsg(args);
	}
	delete new_data;
}

void
KexiMainWindowImpl::createKexiProject(KexiProjectData* new_data)
{
	d->prj = new KexiProject( new_data, this );
//	d->prj = ::createKexiProject(new_data);
//provided by KexiMessageHandler	connect(d->prj, SIGNAL(error(const QString&,KexiDB::Object*)), this, SLOT(showErrorMessage(const QString&,KexiDB::Object*)));
//provided by KexiMessageHandler	connect(d->prj, SIGNAL(error(const QString&,const QString&)), this, SLOT(showErrorMessage(const QString&,const QString&)));
	connect(d->prj, SIGNAL(itemRenamed(const KexiPart::Item&, const QCString&)), this, SLOT(slotObjectRenamed(const KexiPart::Item&, const QCString&)));

	if (d->nav)
		connect(d->prj, SIGNAL(itemRemoved(const KexiPart::Item&)), d->nav, SLOT(slotRemoveItem(const KexiPart::Item&)));
}

KexiProjectData*
KexiMainWindowImpl::createBlankProjectData(bool &cancelled, bool confirmOverwrites, 
	QString* shortcutFileName)
{
	cancelled = false;
	KexiNewProjectWizard wiz(Kexi::connset(), 0, "KexiNewProjectWizard", true);
	wiz.setConfirmOverwrites(confirmOverwrites);
	if (wiz.exec() != QDialog::Accepted) {
		cancelled=true;
		return 0;
	}

	KexiProjectData *new_data;

	if (shortcutFileName)
		*shortcutFileName = QString::null;
	if (wiz.projectConnectionData()) {
		//server-based project
		KexiDB::ConnectionData *cdata = wiz.projectConnectionData();
		kdDebug() << "DBNAME: " << wiz.projectDBName() << " SERVER: " << cdata->serverInfoString() << endl;
		new_data = new KexiProjectData( *cdata, wiz.projectDBName(), wiz.projectCaption() );
		if (shortcutFileName)
			*shortcutFileName = Kexi::connset().fileNameForConnectionData(cdata); 
	}
	else if (!wiz.projectDBName().isEmpty()) {
		//file-based project
		KexiDB::ConnectionData cdata;
		cdata.caption = wiz.projectCaption();
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

	tristate res = d->prj->create(true /*overwrite*/ );
	if (res != true) {
		delete d->prj;
		d->prj = 0;
		return res;
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

	openProject(dlg.selectedFileName(), dlg.selectedExistingConnection());
}

tristate KexiMainWindowImpl::openProject(const QString& aFileName, 
	const QString& fileNameForConnectionData, const QString& dbName)
{
	if (d->prj)
		return openProjectInExternalKexiInstance(aFileName, fileNameForConnectionData, dbName);

	KexiDB::ConnectionData *cdata = 0;
	if (!fileNameForConnectionData.isEmpty()) {
		cdata = Kexi::connset().connectionDataForFileName( fileNameForConnectionData );
		if (!cdata) {
			kdWarning() << "KexiMainWindowImpl::openProject() cdata?" << endl;
			return false;
		}
	}
	return openProject(aFileName, cdata, dbName);
}

tristate KexiMainWindowImpl::openProject(const QString& aFileName, 
	KexiDB::ConnectionData *cdata, const QString& dbName,
	const QValueList<KexiProjectData::ObjectInfo>& autoopenObjects)
{
	if (d->prj) {
		return openProjectInExternalKexiInstance(aFileName, cdata, dbName);
	}

	KexiProjectData* projectData = 0;
	bool deleteAfterOpen = false;
	if (cdata) {
		//server-based project
		if (dbName.isEmpty()) {//no database name given, ask user
			bool cancel;
			projectData = Kexi::startupHandler().selectProject( cdata, cancel, this );
			if (cancel)
				return cancelled;
		}
		else {
//! @todo caption arg?
			projectData = new KexiProjectData( *cdata, dbName );
			deleteAfterOpen = true;
		}
	}
	else {
//		QString selFile = dlg.selectedExistingFile();
		if (aFileName.isEmpty()) {
			kdWarning() << "KexiMainWindowImpl::openProject(): aFileName.isEmpty()" << endl;
			return false;
		}
		//file-based project
		kdDebug() << "Project File: " << aFileName << endl;
		KexiDB::ConnectionData cdata;
		cdata.setFileName( aFileName );
//			cdata.driverName = KexiStartupHandler::detectDriverForFile( cdata.driverName, fileName, this );
		QString detectedDriverName;
		KexiStartupData::Import importActionData;
		const tristate res = KexiStartupHandler::detectActionForFile( 
			importActionData, detectedDriverName, cdata.driverName, aFileName, this );
		if (true != res)
			return res;

		if (importActionData) { //importing requested
			return showProjectMigrationWizard( importActionData.mimeType, importActionData.fileName );
		}
		cdata.driverName = detectedDriverName;

		if (cdata.driverName.isEmpty())
			return false;

		//opening requested
		projectData = new KexiProjectData(cdata, aFileName);
		deleteAfterOpen = true;
	}
	if (!projectData)
		return false;
	projectData->autoopenObjects = autoopenObjects;
	const tristate res = openProject(*projectData);
	if (deleteAfterOpen) //projectData object has been copied
		delete projectData;
	return res;
}

tristate KexiMainWindowImpl::openProjectInExternalKexiInstance(const QString& aFileName, 
	KexiDB::ConnectionData *cdata, const QString& dbName)
{
	QString fileNameForConnectionData;
	if (aFileName.isEmpty()) { //try .kexic file
		if (cdata)
			fileNameForConnectionData = Kexi::connset().fileNameForConnectionData(cdata);
	}
	return openProjectInExternalKexiInstance(aFileName, fileNameForConnectionData, dbName);
}

tristate KexiMainWindowImpl::openProjectInExternalKexiInstance(const QString& aFileName, 
	const QString& fileNameForConnectionData, const QString& dbName)
{
	QString fileName(aFileName);
	QStringList args;
	args << qApp->applicationFilePath();
	// open a file-based project or a server connection provided as a .kexic file
	// (we have no other simple way to provide the startup data to a new process)
	if (fileName.isEmpty()) { //try .kexic file
		if (!fileNameForConnectionData.isEmpty())
			args << "--skip-conn-dialog"; //user does not expect conn. dialog to be shown here

		if (dbName.isEmpty()) { //use 'kexi --skip-conn-dialog file.kexic'
			fileName = fileNameForConnectionData;
		}
		else { //use 'kexi --skip-conn-dialog --connection file.kexic dbName'
			args << "--connection" << fileNameForConnectionData;
			fileName = dbName;
		}
	}
	if (fileName.isEmpty()) {
		kdWarning() << "KexiMainWindowImpl::openProjectInExternalKexiInstance() fileName?" << endl;
		return false;
	}
//! @todo use KRun
	args << fileName;
	QProcess proc(args, this, "process");
	proc.setWorkingDirectory( QFileInfo(fileName).dir(true) );
	const bool ok = proc.start();
	if (!ok) {
		d->showStartProcessMsg(args);
	}
	return ok;
}

void
KexiMainWindowImpl::slotProjectOpenRecentAboutToShow()
{
	/*
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
	*/

	//show recent databases
	KPopupMenu *popup = d->action_open_recent->popupMenu();
	popup->clear();
#if 0
	d->action_open_recent_projects_title_id = popup->insertTitle(i18n("Recently Opened Databases"));
#endif
//	int action_open_recent_projects_title_index = popup->indexOf(d->action_open_recent_projects_title_id);
//	int count = popup->count();
//	int action_open_recent_connections_title_index = popup->indexOf(d->action_open_recent_connections_title_id);
//	for (int i=action_open_recent_projects_title_index+1; 
//		i<action_open_recent_connections_title_index; i++)
//	{
//		popup->removeItemAt(action_open_recent_projects_title_index+1);
//	}

//	int cur_idx = action_open_recent_projects_title_index+1;
	popup->insertItem(SmallIconSet("kexiproject_sqlite"), "My project 1");
	popup->insertItem(SmallIconSet("kexiproject_sqlite"), "My project 2");
	popup->insertItem(SmallIconSet("kexiproject_sqlite"), "My project 3");

#if 0
	//show recent connections
	d->action_open_recent_connections_title_id = 
		d->action_open_recent->popupMenu()->insertTitle(i18n("Recently Connected Database Servers"));
	
//	cur_idx = popup->indexOf(d->action_open_recent_connections_title_id) + 1;
//	for (int i=cur_idx; i<count; i++) {
//		popup->removeItemAt(cur_idx);
//	}
	popup->insertItem(SmallIconSet("socket"), "My connection 1");
	popup->insertItem(SmallIconSet("socket"), "My connection 2");
	popup->insertItem(SmallIconSet("socket"), "My connection 3");
	popup->insertItem(SmallIconSet("socket"), "My connection 4");
#endif
}

void
KexiMainWindowImpl::slotProjectOpenRecent(int id)
{
	if (id<0) // || id==d->action_open_recent_more_id)
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
KexiMainWindowImpl::slotProjectPrint()
{
	if (d->curDialog && d->curDialog->partItem())
		printItem(d->curDialog->partItem());
}

void
KexiMainWindowImpl::slotProjectPrintPreview()
{
	if (d->curDialog && d->curDialog->partItem())
		printPreviewForItem(d->curDialog->partItem());
}

void
KexiMainWindowImpl::slotProjectPageSetup()
{
	if (d->curDialog && d->curDialog->partItem())
		showPageSetupForItem(d->curDialog->partItem());
}

void KexiMainWindowImpl::slotProjectExportDataTable()
{
	if (d->curDialog && d->curDialog->partItem())
		exportItemAsDataTable(d->curDialog->partItem());
}

void
KexiMainWindowImpl::slotProjectProperties()
{
	//TODO: load the implementation not the ui :)
//	ProjectSettingsUI u(this);
//	u.exec();
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
KexiMainWindowImpl::slotProjectQuit()
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

void KexiMainWindowImpl::slotViewMainArea()
{
	if (d->curDialog)
		d->curDialog->setFocus();
}

void KexiMainWindowImpl::slotViewPropertyEditor()
{
	if (!d->propEditor || !d->propEditorToolWindow)
		return;

//js		d->config->setGroup("MainWindow");
//js		ds->setSeparatorPos(d->config->readNumEntry("RightDockPosition", 80/* % */), true);

	if (!d->propEditorTabWidget->isVisible())
		makeWidgetDockVisible(d->propEditorTabWidget);


	d->propEditorToolWindow->wrapperWidget()->raise();

	d->block_KMdiMainFrm_eventFilter=true;
	if (d->propEditorTabWidget->currentPage())
		d->propEditorTabWidget->currentPage()->setFocus();
	d->block_KMdiMainFrm_eventFilter=false;

/*#if defined(KDOCKWIDGET_P)
		KDockWidget *dw = (KDockWidget *)d->propEditor->parentWidget();
		KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
		ds->setSeparatorPos(80,true);//d->config->readNumEntry("RightDockPosition", 80), true);
#endif*/
}

bool KexiMainWindowImpl::switchToViewMode(int viewMode)
{
	if (!d->curDialog) {
		d->toggleLastCheckedMode();
		return false;
	}
	if (!d->curDialog->supportsViewMode( viewMode )) {
		showErrorMessage(i18n("Selected view is not supported for \"%1\" object.")
			.arg(d->curDialog->partItem()->name()),
		i18n("Selected view (%1) is not supported by this object type (%2).")
			.arg(Kexi::nameForViewMode(viewMode))
			.arg(d->curDialog->part()->instanceCaption()) );
		d->toggleLastCheckedMode();
		return false;
	}
	int prevViewMode = d->curDialog->currentViewMode();
	updateCustomPropertyPanelTabs(d->curDialog->part(), prevViewMode,
		d->curDialog->part(), viewMode );
	tristate res = d->curDialog->switchToViewMode( viewMode );
	if (!res) {
		updateCustomPropertyPanelTabs(0, Kexi::NoViewMode); //revert
		showErrorMessage(i18n("Switching to other view failed (%1).").arg(Kexi::nameForViewMode(viewMode)),
			d->curDialog);
		d->toggleLastCheckedMode();
		return false;
	}
	if (~res) {
		updateCustomPropertyPanelTabs(0, Kexi::NoViewMode); //revert
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
	invalidateProjectWideActions();
	invalidateSharedActions();
	d->updateFindDialogContents();
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

void KexiMainWindowImpl::closeWindow(KMdiChildView *pWnd, bool layoutTaskBar)
{
	if (d->insideCloseDialog && dynamic_cast<KexiDialogBase *>(pWnd)) {
		d->windowsToClose.append(dynamic_cast<KexiDialogBase *>(pWnd));
		return;
	}
	/*moved to closeDialog()
	if (pWnd == d->curDialog && !pWnd->isAttached()) {
		if (d->propEditor) {
			// ah, closing detached window - better switch off property buffer right now...
			d->propBuffer = 0;
			d->propEditor->editor()->setBuffer( 0, false );
		}
	}
	*/
	closeDialog(dynamic_cast<KexiDialogBase *>(pWnd), layoutTaskBar);
}

tristate KexiMainWindowImpl::getNewObjectInfo(
	KexiPart::Item *partItem, KexiPart::Part *part,
	bool& allowOverwriting, const QString& messageWhenAskingForName )
{
	//data was never saved in the past -we need to create a new object at the backend
	KexiPart::Info *info = part->info();
#ifdef KEXI_ADD_CUSTOM_OBJECT_CREATION
# include "keximainwindowimpl_customobjcreation.h"
#endif
	if (!d->nameDialog) {
		d->nameDialog = new KexiNameDialog(
			messageWhenAskingForName, this, "nameDialog");
		//check if that name is allowed
		d->nameDialog->widget()->addNameSubvalidator(
			new KexiDB::ObjectNameValidator(project()->dbConnection()->driver(), 0, "sub"));
	}
	else {
		d->nameDialog->widget()->setMessageText( messageWhenAskingForName );
	}
	d->nameDialog->widget()->setCaptionText(partItem->caption());
	d->nameDialog->widget()->setNameText(partItem->name());
	d->nameDialog->setCaption(i18n("Save Object As"));
	d->nameDialog->setDialogIcon( DesktopIcon( info->itemIcon(), KIcon::SizeMedium ) );
	allowOverwriting = false;
	bool found;
	do {
		if (d->nameDialog->exec()!=QDialog::Accepted)
			return cancelled;
		//check if that name already exists
		KexiDB::SchemaData tmp_sdata;
		tristate result = project()->dbConnection()->loadObjectSchemaData(
			info->projectPartID(),
			d->nameDialog->widget()->nameText(), tmp_sdata );
		if (!result)
			return false;
		found = result==true;
		if (found) {
			if (allowOverwriting) {
				int res = KMessageBox::warningYesNoCancel(this,
					"<p>"+part->i18nMessage("Object \"%1\" already exists.", 0)
						.arg(d->nameDialog->widget()->nameText())
					+"</p><p>"+i18n("Do you want to replace it?")+"</p>", 0,
					KGuiItem(i18n("&Replace"), "button_yes"),
					KGuiItem(i18n("&Choose Other Name...")),
					QString::null, KMessageBox::Notify|KMessageBox::Dangerous);
				if (res == KMessageBox::No)
					continue;
				else if (res == KMessageBox::Cancel)
					return cancelled;
				else {//yes
					allowOverwriting = true;
					break;
				}
			}
			else {
				KMessageBox::information(this,
					"<p>"+part->i18nMessage("Object \"%1\" already exists.", 0)
						.arg(d->nameDialog->widget()->nameText())
					+"</p><p>"+i18n("Please choose other name.")+"</p>");
//				" For example: Table \"my_table\" already exists" ,
//				"%1 \"%2\" already exists.\nPlease choose other name.")
//				.arg(dlg->part()->instanceName()).arg(d->nameDialog->widget()->nameText()));
				continue;
			}
		}
	}
	while (found);

	//update name and caption
	partItem->setName( d->nameDialog->widget()->nameText() );
	partItem->setCaption( d->nameDialog->widget()->captionText() );
	return true;
}

tristate KexiMainWindowImpl::saveObject( KexiDialogBase *dlg, const QString& messageWhenAskingForName,
	bool dontAsk)
{
	tristate res;
	if (!dlg->neverSaved()) {
		//data was saved in the past -just save again
		res = dlg->storeData(dontAsk);
		if (!res)
			showErrorMessage(i18n("Saving \"%1\" object failed.").arg(dlg->partItem()->name()),
				d->curDialog);
		return res;
	}

	const int oldItemID = dlg->partItem()->identifier();

	bool allowOverwriting = false;
	res = getNewObjectInfo( dlg->partItem(), dlg->part(), allowOverwriting,
		messageWhenAskingForName );
	if (res != true)
		return res;

	res = dlg->storeNewData();
	if (~res)
		return cancelled;
	if (!res) {
		showErrorMessage(i18n("Saving new \"%1\" object failed.").arg(dlg->partItem()->name()),
			d->curDialog);
		return false;
	}

	//update navigator
//this is alreday done in KexiProject::addStoredItem():	d->nav->addItem(dlg->partItem());
	//item id changed to final one: update association in dialogs' dictionary
//	d->dialogs.take(oldItemID);
	d->updateDialogId(dlg, oldItemID);
	invalidateProjectWideActions();
	return true;
}

tristate KexiMainWindowImpl::closeDialog(KexiDialogBase *dlg)
{
	return closeDialog(dlg, true);
}

tristate KexiMainWindowImpl::closeDialog(KexiDialogBase *dlg, bool layoutTaskBar, bool doNotSaveChanges)
{
	if (!dlg) 
		return true;
	if (d->insideCloseDialog)
		return true;

#ifndef KEXI_NO_PENDING_DIALOGS
	d->addItemToPendingDialogs(dlg->partItem(), Private::DialogClosingJob);
#endif

	d->insideCloseDialog = true;

	if (dlg == d->curDialog && !dlg->isAttached()) {
		if (d->propEditor) {
			// ah, closing detached window - better switch off property buffer right now...
			d->propBuffer = 0;
			d->propEditor->editor()->changeSet( 0, false );
		}
	}

	bool remove_on_closing = dlg->partItem() ? dlg->partItem()->neverSaved() : false;
	if (dlg->dirty() && !d->forceDialogClosing && !doNotSaveChanges) {
		//more accurate tool tips and what's this
		KGuiItem saveChanges( KStdGuiItem::save() ); 
		saveChanges.setToolTip(i18n("Save changes"));
		saveChanges.setWhatsThis(
			i18n( "Pressing this button will save all recent changes made in \"%1\" object." )
			.arg(dlg->partItem()->name()) );
		KGuiItem discardChanges( KStdGuiItem::discard() );
		discardChanges.setWhatsThis(
			i18n( "Pressing this button will discard all recent changes made in \"%1\" object." )
			.arg(dlg->partItem()->name()) );

		//dialog's data is dirty:
		//--adidional message, e.g. table designer will return 
		//  "Note: This table is already filled with data which will be removed."
		//  if the dlg is in design view mode.
		QString additionalMessage = dlg->part()->i18nMessage(
			":additional message before saving design", dlg);
		if (additionalMessage.startsWith(":"))
			additionalMessage = QString::null;
		if (!additionalMessage.isEmpty())
			additionalMessage = "<p>"+additionalMessage+"</p>";

		const int questionRes = KMessageBox::warningYesNoCancel( this,
			"<p>"+dlg->part()->i18nMessage("Design of object \"%1\" has been modified.", dlg)
			.arg(dlg->partItem()->name())+"</p><p>"+i18n("Do you want to save changes?")+"</p>"
			+ additionalMessage /*may be empty*/,
			QString::null,
			saveChanges,
			discardChanges);
		if (questionRes==KMessageBox::Cancel) {
#ifndef KEXI_NO_PENDING_DIALOGS
			d->removePendingDialog(dlg->id());
#endif
			d->insideCloseDialog = false;
			d->windowsToClose.clear(); //give up with 'close all'
			return cancelled;
		}
		if (questionRes==KMessageBox::Yes) {
			//save it
//			if (!dlg->storeData())
			tristate res = saveObject( dlg, QString::null, true /*dontAsk*/ );
			if (!res || ~res) {
//js:TODO show error info; (retry/ignore/cancel)
#ifndef KEXI_NO_PENDING_DIALOGS
				d->removePendingDialog(dlg->id());
#endif
				d->insideCloseDialog = false;
				d->windowsToClose.clear(); //give up with 'close all'
				return res;
			}
			remove_on_closing = false;
		}
	}

	const int dlg_id = dlg->id(); //remember now, because removeObject() can destruct partitem object

	if (remove_on_closing) {
		//we won't save this object, and it was never saved -remove it
		if (!removeObject( dlg->partItem(), true )) {
#ifndef KEXI_NO_PENDING_DIALOGS
			d->removePendingDialog(dlg->id());
#endif
			//msg?
			//TODO: ask if we'd continue and return true/false
			d->insideCloseDialog = false;
			d->windowsToClose.clear(); //give up with 'close all'
			return false;
		}
	}
	else {
		//not dirty now
		if(d->nav)
			d->nav->updateItemName( *dlg->partItem(), false );
	}

	d->removeDialog(dlg_id); //don't remove -KMDI will do that
	//also remove from 'print setup dialogs' cache, if needed
	int printedObjectID = 0;
	if (d->pageSetupDialogItemID2dataItemID_map.contains(dlg_id))
		printedObjectID = d->pageSetupDialogItemID2dataItemID_map[ dlg_id ];
	d->pageSetupDialogs.take(printedObjectID);

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
		if (d->openedDialogsCount()==0) {//now there is no dialogs - remove client RIGHT NOW!
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
		if (d->openedDialogsCount()==0) {//now there is no dialogs - remove client RIGHT NOW!
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
	if (d->openedDialogsCount()==0) {
		d->maximizeFirstOpenedChildFrm = isInMaximizedChildFrmMode;
		if (d->nav)
			d->nav->setFocus();
		d->updatePropEditorVisibility(0);
	}

	invalidateActions();
	d->insideCloseDialog = false;
	if (!d->windowsToClose.isEmpty()) //continue 'close all'
		closeDialog(d->windowsToClose.take(0), true);

#ifndef KEXI_NO_PENDING_DIALOGS
	d->removePendingDialog( dlg_id );

	//perform pending global action that was suspended:
	if (!d->pendingDialogsExist()) {
		d->executeActionWhenPendingJobsAreFinished();
	}
#endif
	return true;
}

void KexiMainWindowImpl::detachWindow(KMdiChildView *pWnd,bool bShow)
{
	KMdiMainFrm::detachWindow(pWnd,bShow);
	// update icon - from small to large
	pWnd->setIcon( DesktopIcon( static_cast<KexiDialogBase *>(pWnd)->itemIcon() ) );
//	pWnd->setIcon( DesktopIcon( static_cast<KexiDialogBase *>(pWnd)->part()->info()->itemIcon() ) );
	if (dynamic_cast<KexiDialogBase*>(pWnd))
		dynamic_cast<KexiDialogBase*>(pWnd)->sendDetachedStateToCurrentView();
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
	if (dynamic_cast<KexiDialogBase*>(pWnd))
		dynamic_cast<KexiDialogBase*>(pWnd)->sendAttachedStateToCurrentView();
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
		if (static_cast<QKeyEvent*>(e)->key()==Qt::Key_Tab && static_cast<QKeyEvent*>(e)->state() == ControlButton) {
			if (d->action_window_next->shortcut().keyCodeQt()==Qt::Key_Tab+Qt::CTRL && d->action_window_next->shortcut().count()==1
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

/*	if (obj==d->propEditor) {
		if (e->type()==QEvent::Resize) {
			d->updatePropEditorDockWidthInfo();
		}
	}*/

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
#ifdef KEXI_STATUSBAR_DEBUG
		QWidget *focus_widget = focus_w ? focus_w->focusWidget() : 0;
		d->statusBar->setStatus(QString("FOCUS VIEW: %1 %2, FOCUS WIDGET: %3 %4")
			.arg(focus_w ? focus_w->className() : "").arg(focus_w ? focus_w->name() : "")
			.arg(focus_widget ? focus_widget->className() : "").arg(focus_widget ? focus_widget->name() : "")
			);
#endif
	}
	else if (e->type()==QEvent::FocusOut) {
		focus_w = focusWindow();
		KexiVDebug << "Focus OUT EVENT" << endl;
		KexiVDebug << (focus_w ? focus_w->name() : "" )  << endl;
		KexiVDebug << "eventFilter: " <<e->type() << " " <<obj->name() <<endl;
#ifdef KEXI_STATUSBAR_DEBUG
		QWidget *focus_widget = focus_w ? focus_w->focusWidget() : 0;
		d->statusBar->setStatus(QString("FOCUS VIEW: %1 %2, FOCUS WIDGET: %3 %4")
			.arg(focus_w ? focus_w->className() : "").arg(focus_w ? focus_w->name() : "")
			.arg(focus_widget ? focus_widget->className() : "").arg(focus_widget ? focus_widget->name() : "")
			);
#endif
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
			if (KexiUtils::hasParent(d->curDialog, focus_w)) {
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

bool KexiMainWindowImpl::openingAllowed(KexiPart::Item* item, int viewMode)
{
	//! @todo this can be more complex once we deliver ACLs...
	if (!userMode())
		return true;
	KexiPart::Part * part = Kexi::partManager().partForMimeType(item->mimeType());
	return part && (part->supportedUserViewModes() & viewMode);
}

KexiDialogBase *
KexiMainWindowImpl::openObject(const QCString& mimeType, const QString& name, 
	int viewMode, bool &openingCancelled, QMap<QString,QString>* staticObjectArgs)
{
	KexiPart::Item *item = d->prj->itemForMimeType(mimeType,name);
	if (!item)
		return 0;
	return openObject(item, viewMode, openingCancelled, staticObjectArgs);
}

KexiDialogBase *
KexiMainWindowImpl::openObject(KexiPart::Item* item, int viewMode, bool &openingCancelled,
	QMap<QString,QString>* staticObjectArgs, QString* errorMessage)
{
	if (!openingAllowed(item, viewMode)) {
		if (errorMessage)
			*errorMessage = i18n("opening is not allowed in \"data view/design view/text view\" mode", 
				"opening is not allowed in \"%1\" mode").arg(Kexi::nameForViewMode(viewMode));
		openingCancelled = true;
		return 0;
	}
	
	if (!d->prj || !item)
		return 0;
	KexiUtils::WaitCursor wait;
#ifndef KEXI_NO_PENDING_DIALOGS
	Private::PendingJobType pendingType;
	KexiDialogBase *dlg = d->openedDialogFor( item, pendingType );
	if (pendingType != Private::NoJob) {
		openingCancelled = true;
		return 0;
	}
#else
	KexiDialogBase *dlg = d->openedDialogFor( item );
#endif
	openingCancelled = false;

	bool needsUpdateViewGUIClient = true;
	if (dlg) {
		dlg->activate();
		if (viewMode!=dlg->currentViewMode()) {
			if (!switchToViewMode(viewMode))
				return 0;
		}
		needsUpdateViewGUIClient = false;
	}
	else {
		d->updatePropEditorVisibility(viewMode);
		KexiPart::Part *part = Kexi::partManager().partForMimeType(item->mimeType());
		//update tabs before opening
		updateCustomPropertyPanelTabs(d->curDialog ? d->curDialog->part() : 0,
			d->curDialog ? d->curDialog->currentViewMode() : Kexi::NoViewMode,
			part, viewMode);

#ifndef KEXI_NO_PENDING_DIALOGS
		d->addItemToPendingDialogs(item, Private::DialogOpeningJob);
#endif
		dlg = d->prj->openObject(this, *item, viewMode, staticObjectArgs);
	}

	if (!dlg || !activateWindow(dlg)) {
#ifndef KEXI_NO_PENDING_DIALOGS
		d->removePendingDialog(item->identifier());
#endif
		updateCustomPropertyPanelTabs(0, Kexi::NoViewMode); //revert
		//js TODO: add error msg...
		return 0;
	}

	if (needsUpdateViewGUIClient /*&& !userMode()*/) {
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

#ifndef KEXI_NO_PENDING_DIALOGS
	d->removePendingDialog( dlg->id() );

	//perform pending global action that was suspended:
	if (!d->pendingDialogsExist()) {
		d->executeActionWhenPendingJobsAreFinished();
	}
#endif
	return dlg;
}

KexiDialogBase *
KexiMainWindowImpl::openObjectFromNavigator(KexiPart::Item* item, int viewMode)
{
	bool openingCancelled;
	return openObjectFromNavigator(item, viewMode, openingCancelled);
}

KexiDialogBase *
KexiMainWindowImpl::openObjectFromNavigator(KexiPart::Item* item, int viewMode, 
	bool &openingCancelled)
{
	if (!openingAllowed(item, viewMode)) {
		openingCancelled = true;
		return 0;
	}
	if (!d->prj || !item)
		return false;
#ifndef KEXI_NO_PENDING_DIALOGS
	Private::PendingJobType pendingType;
	KexiDialogBase *dlg = d->openedDialogFor( item, pendingType );
	if (pendingType!=Private::NoJob) {
		openingCancelled = true;
		return 0;
	}
#else
	KexiDialogBase *dlg = d->openedDialogFor( item );
#endif
	openingCancelled = false;
	if (dlg) {
		if (activateWindow(dlg)) {//item->identifier())) {//just activate
			invalidateViewModeActions();
			return dlg;
		}
	}
	//if DataViewMode is not supported, try Design, then Text mode (currently useful for script part)
	KexiPart::Part *part = Kexi::partManager().partForMimeType(item->mimeType());
	if (!part)
		return 0;
	if (viewMode == Kexi::DataViewMode && !(part->supportedViewModes() & Kexi::DataViewMode)) {
		if (part->supportedViewModes() & Kexi::DesignViewMode)
			return openObjectFromNavigator( item, Kexi::DesignViewMode, openingCancelled );
		else if (part->supportedViewModes() & Kexi::TextViewMode)
			return openObjectFromNavigator( item, Kexi::TextViewMode, openingCancelled );
	}
	//do the same as in openObject()
	return openObject(item, viewMode, openingCancelled);
}

tristate KexiMainWindowImpl::closeObject(KexiPart::Item* item)
{
#ifndef KEXI_NO_PENDING_DIALOGS
	Private::PendingJobType pendingType;
	KexiDialogBase *dlg = d->openedDialogFor( item, pendingType );
	if (pendingType == Private::DialogClosingJob)
		return true;
	else if (pendingType == Private::DialogOpeningJob)
		return cancelled;
#else
	KexiDialogBase *dlg = d->openedDialogFor( item );
#endif
	if (!dlg)
		return cancelled;
	return closeDialog(dlg);
}

bool KexiMainWindowImpl::newObject( KexiPart::Info *info, bool& openingCancelled )
{
	if (userMode()) {
		openingCancelled = true;
		return false;
	}
	openingCancelled = false;
	if (!d->prj || !info)
		return false;
	KexiPart::Part *part = Kexi::partManager().partForMimeType(info->mimeType());
	if(!part)
		return false;

#ifdef KEXI_ADD_CUSTOM_OBJECT_CREATION
# include "keximainwindowimpl_customobjcreation.h"
#endif

	KexiPart::Item *it = d->prj->createPartItem(info); //this, *item, viewMode);
	if (!it) {
		//js: todo: err
		return false;
	}

	if (!it->neverSaved()) //only add stored objects to the browser
		d->nav->addItem(*it);
	return openObject(it, Kexi::DesignViewMode, openingCancelled);
}

tristate KexiMainWindowImpl::removeObject( KexiPart::Item *item, bool dontAsk )
{
	if (userMode())
		return cancelled;
	if (!d->prj || !item)
		return false;

	KexiPart::Part *part = Kexi::partManager().partForMimeType(item->mimeType());
	if (!part)
		return false;

	if (!dontAsk) {
		if (KMessageBox::No == KMessageBox::warningYesNo(this, 
			"<p>"+i18n("Do you want to permanently delete:\n"
			"%1\n"
			"If you click Yes, you will not be able to undo the deletion.")
				.arg( "</p><p>"+part->instanceCaption()+" \""+ item->name() + "\"?</p>" ),
			0, KGuiItem(i18n("Delete"), "editdelete"), KStdGuiItem::no()))
			return cancelled;
	}

	//also close 'print setup' dialog for this item, if any
	tristate res;
//	int printedObjectID = 0;
//	if (d->pageSetupDialogItemID2dataItemID_map.contains(item->identifier()))
//		printedObjectID = d->pageSetupDialogItemID2dataItemID_map[ item->identifier() ];
	KexiDialogBase * pageSetupDlg = d->pageSetupDialogs[ item->identifier() ];
	const bool oldInsideCloseDialog = d->insideCloseDialog;
	d->insideCloseDialog = false;
	 res = closeDialog(pageSetupDlg);
	d->insideCloseDialog = oldInsideCloseDialog;
	if (!res || ~res) {
		return res;
	}

#ifndef KEXI_NO_PENDING_DIALOGS
	Private::PendingJobType pendingType;
	KexiDialogBase *dlg = d->openedDialogFor( item, pendingType );
	if (pendingType!=Private::NoJob) {
		return cancelled;
	}
#else
	KexiDialogBase *dlg = d->openedDialogFor( item );
#endif

	if (dlg) {//close existing window
//		if (!dlg->tryClose(true))
		const bool tmp = d->forceDialogClosing;
		/*const bool remove_on_closing = */dlg->partItem()->neverSaved();
		d->forceDialogClosing = true;
		res = closeDialog(dlg);
		d->forceDialogClosing = tmp; //restore
		if (!res || ~res) {
			return res;
		}
//		if (remove_on_closing) //already removed
	//		return true;
//		if (!dlg->close(true))
//			return true; //ok - close cancelled
	}

	//in case the dialog is a 'print setup' dialog, also update d->pageSetupDialogs
	int dataItemID = d->pageSetupDialogItemID2dataItemID_map[item->identifier()];
	d->pageSetupDialogItemID2dataItemID_map.remove(item->identifier());
	d->pageSetupDialogs.take( dataItemID );

	if (!d->prj->removeObject(this, *item)) {
		//TODO(js) better msg
		showSorryMessage( i18n("Could not remove object.") );
		return false;
	}
	return true;
}

void KexiMainWindowImpl::renameObject( KexiPart::Item *item, const QString& _newName, bool &success )
{
	if (userMode()) {
		success = false;
		return;
	}
	d->pendingDialogsExist();
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
		showErrorMessage( d->prj, i18n("Renaming object \"%1\" failed.").arg(newName) );
		success = false;
		return;
	}
	d->pendingDialogsExist();
}

void KexiMainWindowImpl::slotObjectRenamed(const KexiPart::Item &item, const QCString& /*oldName*/)
{
#ifndef KEXI_NO_PENDING_DIALOGS
	Private::PendingJobType pendingType;
	KexiDialogBase *dlg = d->openedDialogFor( &item, pendingType );
	if (pendingType!=Private::NoJob)
		return;
#else
	KexiDialogBase *dlg = d->openedDialogFor( &item );
#endif
	if (!dlg)
		return;

	//change item
	dlg->updateCaption();
	if (static_cast<KexiDialogBase*>(d->curDialog)==dlg)//optionally, update app. caption
		updateAppCaption();
}

int KexiMainWindowImpl::generatePrivateID()
{
	return --d->privateIDCounter;
}

void KexiMainWindowImpl::acceptPropertySetEditing()
{
	if (d->propEditor)
		d->propEditor->editor()->acceptInput();
}

void KexiMainWindowImpl::propertySetSwitched(KexiDialogBase *dlg, bool force, 
	bool preservePrevSelection, const QCString& propertyToSelect)
{
	kdDebug() << "KexiMainWindowImpl::propertySetSwitched() d->curDialog: "
		<< (d->curDialog ? d->curDialog->caption() : QString("NULL")) << " dlg: " << (dlg ? dlg->caption() : QString("NULL"))<< endl;
	if ((KexiDialogBase*)d->curDialog!=dlg) {
		d->propBuffer = 0; //we'll need to move to another prop. set
		return;
	}
	if (d->propEditor) {
		KoProperty::Set *newBuf = d->curDialog ? d->curDialog->propertySet() : 0;
		if (!newBuf || (force || static_cast<KoProperty::Set*>(d->propBuffer) != newBuf)) {
			d->propBuffer = newBuf;
			if (preservePrevSelection) {
				if (propertyToSelect.isEmpty())
					d->propEditor->editor()->changeSet( d->propBuffer, preservePrevSelection );
				else
					d->propEditor->editor()->changeSet( d->propBuffer, propertyToSelect );
			}
		}
	}
}

void KexiMainWindowImpl::slotDirtyFlagChanged(KexiDialogBase* dlg)
{
	KexiPart::Item *item = dlg->partItem();
	//update text in navigator and app. caption
	if(!userMode())
		d->nav->updateItemName( *item, dlg->dirty() );

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

void KexiMainWindowImpl::slotStartFeedbackAgent()
{
#ifndef KEXI_NO_FEEDBACK_AGENT
#ifdef FEEDBACK_CLASS
	const KAboutData* about = KApplication::kApplication()->aboutData();
	FEEDBACK_CLASS* wizard = new FEEDBACK_CLASS( about->programName(),
		about->version(), 0, 0, 0, FEEDBACK_CLASS::AllPages );

	if ( wizard->exec() )
	{
		KApplication::kApplication()->invokeMailer( "kexi-reports-dummy@kexi.org",
			QString::null, QString::null,
			about->appName() + QCString( " [feedback]" ),
			wizard->feedbackDocument().toString( 2 ).local8Bit() );
	}

	delete wizard;
#endif
#endif
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
			QTextBrowser *tb = KexiUtils::findFirstChild<KTextBrowser>(&tipDialog,"KTextBrowser");
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
	Q_UNUSED(noMessage);
	Kexi::tempShowForms() = show;
}

bool KexiMainWindowImpl::userMode() const
{
	return d->userMode;
}

bool
KexiMainWindowImpl::initUserMode(KexiProjectData *projectData)
{
//	Kexi::tempShowForms() = true;
//	Kexi::tempShowReports() = true;
//	Kexi::tempShowMacros() = true;
//	Kexi::tempShowScripts() = true;
	if(!projectData)
		return false;

	createKexiProject(projectData); //initialize project
//	d->prj->setFinal(true);         //announce that we are in fianl mode

	tristate res = d->prj->open();             //try to open database
	if (!res || ~res) {
		delete d->prj;
		d->prj = 0;
		return false;
	}

#if 0 //todo reenable; autoopen objects are handled elsewhere
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

	KexiPart::Info *i = Kexi::partManager().infoForMimeType(startupPart.latin1());
	if (!i) {
		hide();
		showErrorMessage( err_msg, i18n("Specified plugin does not exist.") );
		return false;
	}

	Kexi::partManager().part(i);
	KexiPart::Item *item = d->prj->item(i, startupItem);
	bool openingCancelled;
	if(!openObject(item, Kexi::DataViewMode, openingCancelled) && !openingCancelled) {
		hide();
		showErrorMessage( err_msg, i18n("Specified object could not be opened.") );
		return false;
	}

	QWidget::setCaption("MyApp");//TODO
#endif
	return true;
}

void
KexiMainWindowImpl::initUserActions()
{
#if 0 //unused for now
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
#endif
}

void KexiMainWindowImpl::slotToolsProjectMigration()
{
	showProjectMigrationWizard(QString::null, QString::null);
}

void KexiMainWindowImpl::slotToolsCompactDatabase()
{
	KexiProjectData *data = 0;
	KexiDB::Driver *drv = 0;
	const bool projectWasOpened = d->prj;

	if (!d->prj) {
		KexiStartupDialog dlg(
			KexiStartupDialog::OpenExisting, 0, Kexi::connset(), Kexi::recentProjects(),
			this, "KexiOpenDialog");

		if (dlg.exec()!=QDialog::Accepted)
			return;

		if (dlg.selectedFileName().isEmpty()) {
//! @todo add support for server based if needed?
			return;
		}
		KexiDB::ConnectionData cdata;
		cdata.setFileName( dlg.selectedFileName() );
		
		//detect driver name for the selected file
		KexiStartupData::Import detectedImportAction;
		tristate res = KexiStartupHandler::detectActionForFile( 
			detectedImportAction, cdata.driverName,
			"" /*suggestedDriverName*/, cdata.fileName(), 0,
			KexiStartupHandler::SkipMessages | KexiStartupHandler::ThisIsAProjectFile 
			| KexiStartupHandler::DontConvert);

		if (true==res && !detectedImportAction)
			drv = Kexi::driverManager().driver( cdata.driverName );
		if (!drv || !(drv->features() & KexiDB::Driver::CompactingDatabaseSupported)) {
			KMessageBox::information(this, "<qt>"+
				i18n("Compacting database file <nobr>\"%1\"</nobr> is not supported.")
				.arg(QDir::convertSeparators(cdata.fileName())));
			return;
		}
		data = new KexiProjectData( cdata, cdata.fileName() );
	}
	else {
		//sanity
		if ( !(d->prj && d->prj->dbConnection() 
			&& (d->prj->dbConnection()->driver()->features() & KexiDB::Driver::CompactingDatabaseSupported) ))
			return;

		if (KMessageBox::Continue != KMessageBox::warningContinueCancel(this,
				i18n("The current project has to be closed before compacting the database. "
				 "It will be open again after compacting.\n\nDo you want to continue?")))
			return;

		data = new KexiProjectData(*d->prj->data()); // a copy
		drv = d->prj->dbConnection()->driver();
		const tristate res = closeProject();
		if (~res || !res)
			return;
	}

	if (!drv->adminTools().vacuum(*data->connectionData(), data->databaseName())) {
		//err msg
		showErrorMessage( &drv->adminTools() );
	}

	if (data && projectWasOpened) {
		openProject(*data);
		delete data;
	}
}

tristate KexiMainWindowImpl::showProjectMigrationWizard(
	const QString& mimeType, const QString& databaseName, const KexiDB::ConnectionData *cdata)
{
	//pass arguments
	QMap<QString,QString> args;
	args.insert("mimeType", mimeType);
	args.insert("databaseName", databaseName);
	if (cdata) { //pass ConnectionData serialized as a string...
		QString str;
		KexiUtils::serializeMap( KexiDB::toMap( *cdata ), str );
		args.insert("connectionData", str);
	}

	QDialog *dlg = KexiInternalPart::createModalDialogInstance("migration", this, this, 0, &args);
	if (!dlg)
		return false; //error msg has been shown by KexiInternalPart

	const int result = dlg->exec();
	delete dlg;
	//raise();
	if (result!=QDialog::Accepted)
		return cancelled;

	//open imported project in a new Kexi instance
	QString destinationDatabaseName( args["destinationDatabaseName"] );
	QString fileName, destinationConnectionShortcut, dbName;
	if (!destinationDatabaseName.isEmpty()) {
		if (args.contains("destinationConnectionShortcut")) {
			// server-based
			destinationConnectionShortcut = args["destinationConnectionShortcut"];
		}
		else {
			// file-based
			fileName = destinationDatabaseName;
			destinationDatabaseName = QString::null;
		}
		tristate res = openProject(fileName, destinationConnectionShortcut, 
			destinationDatabaseName);
		raise();
		return res;
//			KexiDB::ConnectionData *connData = new KexiDB::ConnectionData();
//			KexiDB::fromMap( KexiUtils::deserializeMap( args["destinationConnectionData"] ), *connData );
//		return openProject(destinationFileName, 0);
	}
	return true;
}

tristate KexiMainWindowImpl::executeItem(KexiPart::Item* item)
{
	KexiPart::Info *info = item ? Kexi::partManager().infoForMimeType(item->mimeType()) : 0;
	if ( (! info) || (! info->isExecuteSupported()) )
		return false;
	KexiPart::Part *part = Kexi::partManager().part(info);
	if (!part)
		return false;
	return part->execute(item);
}

void KexiMainWindowImpl::slotProjectImportDataTable()
{
//! @todo allow data appending (it is not possible now)
	if (userMode())
		return;
	QMap<QString,QString> args;
	args.insert("sourceType", "file");
	QDialog *dlg = KexiInternalPart::createModalDialogInstance(
		"csv_importexport", "KexiCSVImportDialog", this, this, 0, &args);
	if (!dlg)
		return; //error msg has been shown by KexiInternalPart
	dlg->exec();
	delete dlg;
}

tristate KexiMainWindowImpl::executeCustomActionForObject(KexiPart::Item* item, 
	const QString& actionName)
{
	if (actionName == "exportToCSV")
		return exportItemAsDataTable(item);
	else if (actionName == "copyToClipboardAsCSV")
		return copyItemToClipboardAsDataTable(item);

	kexiwarn << "KexiMainWindowImpl::executeCustomActionForObject(): no such action: " 
		<< actionName << endl;
	return false;
}

tristate KexiMainWindowImpl::exportItemAsDataTable(KexiPart::Item* item)
{
	if (!item)
		return false;
//! @todo: check if changes to this are saved, if not: ask for saving
//! @todo: accept row changes...

	QMap<QString,QString> args;
	args.insert("destinationType", "file");
	args.insert("itemId", QString::number(item->identifier()));
	QDialog *dlg = KexiInternalPart::createModalDialogInstance(
		"csv_importexport", "KexiCSVExportWizard", this, this, 0, &args);
	if (!dlg)
		return false; //error msg has been shown by KexiInternalPart
	int result = dlg->exec();
	delete dlg;
	return result == QDialog::Rejected ? cancelled : true;
}

bool KexiMainWindowImpl::printItem(KexiPart::Item* item, const QString& titleText)
{
	return printItem(item, KexiSimplePrintingSettings::load(), titleText);
}

tristate KexiMainWindowImpl::printItem(KexiPart::Item* item)
{
	return printItem(item, QString::null);
}

bool KexiMainWindowImpl::printItem(KexiPart::Item* item, const KexiSimplePrintingSettings& settings,
	const QString& titleText)
{
//! @todo: check if changes to this object's design are saved, if not: ask for saving
//! @todo: accept row changes...
	KexiSimplePrintingCommand cmd(this, item->identifier());
	//modal
	return cmd.print(settings, titleText);
}

bool KexiMainWindowImpl::printPreviewForItem(KexiPart::Item* item, const QString& titleText, bool reload)
{
	return printPreviewForItem(item, KexiSimplePrintingSettings::load(), titleText, reload);
}

tristate KexiMainWindowImpl::printPreviewForItem(KexiPart::Item* item)
{
	return printPreviewForItem(item, QString::null, 
//! @todo store cached row data?
		true/*reload*/);
}

bool KexiMainWindowImpl::printPreviewForItem(KexiPart::Item* item, 
	const KexiSimplePrintingSettings& settings, const QString& titleText, bool reload)
{
//! @todo: check if changes to this object's design are saved, if not: ask for saving
//! @todo: accept row changes...
	KexiSimplePrintingCommand* cmd = d->openedCustomObjectsForItem<KexiSimplePrintingCommand>(
		item, "KexiSimplePrintingCommand");
	if (!cmd) {
		d->addOpenedCustomObjectForItem(
			item,
			cmd = new KexiSimplePrintingCommand(this, item->identifier()), 
			"KexiSimplePrintingCommand"
		);
	}
	return cmd->showPrintPreview(settings, titleText, reload);
}

tristate KexiMainWindowImpl::showPageSetupForItem(KexiPart::Item* item)
{
//! @todo: check if changes to this object's design are saved, if not: ask for saving
//! @todo: accept row changes...
	return printActionForItem(item, PageSetupForItem);
}

tristate KexiMainWindowImpl::printActionForItem(KexiPart::Item* item, PrintActionType action)
{
	if (!item)
		return false;
	KexiPart::Info *info = Kexi::partManager().infoForMimeType( item->mimeType() );
	if (!info->isPrintingSupported())
		return false;

	KexiDialogBase *printingDialog = d->pageSetupDialogs[ item->identifier() ];
	if (printingDialog) {
		if (!activateWindow(printingDialog))
			return false;
		if (action == PreviewItem || action == PrintItem) {
			QTimer::singleShot(0,printingDialog->selectedView(),
				(action == PreviewItem) ? SLOT(printPreview()) : SLOT(print()));
		}
		return true;
	}

#ifndef KEXI_NO_PENDING_DIALOGS
	Private::PendingJobType pendingType;
	KexiDialogBase *dlg = d->openedDialogFor( item, pendingType );
	if (pendingType!=Private::NoJob)
		return cancelled;
#else
	KexiDialogBase *dlg = d->openedDialogFor( item );
#endif

	if (dlg) {
		// accept row changes
		QWidget *prevFocusWidget = focusWidget();
		dlg->setFocus();
		d->action_data_save_row->activate();
		if (prevFocusWidget)
			prevFocusWidget->setFocus();

		// opened: check if changes made to this dialog are saved, if not: ask for saving
		if (dlg->neverSaved()) //sanity check
			return false;
		if (dlg->dirty()) {
			KGuiItem saveChanges( KStdGuiItem::save() ); 
			saveChanges.setToolTip(i18n("Save changes"));
			saveChanges.setWhatsThis(
				i18n( "Pressing this button will save all recent changes made in \"%1\" object." )
				.arg(item->name()) );
			KGuiItem doNotSave( KStdGuiItem::no() );
			doNotSave.setWhatsThis(
				i18n( "Pressing this button will ignore all unsaved changes made in \"%1\" object." )
				.arg(dlg->partItem()->name()) );

			QString question;
			if (action == PrintItem)
				question = i18n("Do you want to save changes before printing?");
			else if (action == PreviewItem)
				question = i18n("Do you want to save changes before making print preview?");
			else if (action == PageSetupForItem)
				question = i18n("Do you want to save changes before showing page setup?");
			else
				return false;

			const int questionRes = KMessageBox::warningYesNoCancel( this,
				"<p>"+dlg->part()->i18nMessage("Design of object \"%1\" has been modified.", dlg)
				.arg(item->name()) + "</p><p>" + question + "</p>",
				QString::null, 
				saveChanges,
				doNotSave);
			if (KMessageBox::Cancel == questionRes)
				return cancelled;
			if (KMessageBox::Yes == questionRes) {
				tristate savingRes = saveObject( dlg, QString::null, true /*dontAsk*/ );
				if (true != savingRes)
					return savingRes;
			}
		}
	}
	KexiPart::Part * printingPart = Kexi::partManager().partForMimeType("kexi/simpleprinting");
	if (!printingPart)
		printingPart = new KexiSimplePrintingPart(); //hardcoded as there're no .desktop file
	KexiPart::Item* printingPartItem = d->prj->createPartItem(
		printingPart, item->name() //<-- this will look like "table1 : printing" on the window list
	);
	QMap<QString,QString> staticObjectArgs;
	staticObjectArgs["identifier"] = QString::number(item->identifier());
	if (action == PrintItem)
		staticObjectArgs["action"] = "print";
	else if (action == PreviewItem)
		staticObjectArgs["action"] = "printPreview";
	else if (action == PageSetupForItem)
		staticObjectArgs["action"] = "pageSetup";
	else
		return false;
	bool openingCancelled;
	printingDialog = openObject(printingPartItem, Kexi::DesignViewMode, 
		openingCancelled, &staticObjectArgs);
	if (openingCancelled)
		return cancelled;
	if (!printingDialog) //sanity
		return false;
	d->pageSetupDialogs.insert(item->identifier(), printingDialog);
	d->pageSetupDialogItemID2dataItemID_map.insert(
		printingDialog->partItem()->identifier(), item->identifier());

	return true;
}

void KexiMainWindowImpl::slotEditCopySpecialDataTable()
{
	KexiPart::Item* item = d->nav->selectedPartItem();
	if (item)
		exportItemAsDataTable(item);
}

tristate KexiMainWindowImpl::copyItemToClipboardAsDataTable(KexiPart::Item* item)
{
	if (!item)
		return false;

	QMap<QString,QString> args;
	args.insert("destinationType", "clipboard");
	args.insert("itemId", QString::number(item->identifier()));
	QDialog *dlg = KexiInternalPart::createModalDialogInstance(
		"csv_importexport", "KexiCSVExportWizard", this, this, 0, &args);
	if (!dlg)
		return false; //error msg has been shown by KexiInternalPart
	const int result = dlg->exec();
	delete dlg;
	return result == QDialog::Rejected ? cancelled : true;
}

void KexiMainWindowImpl::slotEditPasteSpecialDataTable()
{
//! @todo allow data appending (it is not possible now)
	if (userMode())
		return;
	QMap<QString,QString> args;
	args.insert("sourceType", "clipboard");
	QDialog *dlg = KexiInternalPart::createModalDialogInstance(
		"csv_importexport", "KexiCSVImportDialog", this, this, 0, &args);
	if (!dlg)
		return; //error msg has been shown by KexiInternalPart
	dlg->exec();
	delete dlg;
}

void KexiMainWindowImpl::slotEditFind()
{
//	KexiViewBase *view = d->currentViewSupportingAction("edit_findnext");
	KexiSearchAndReplaceViewInterface* iface = d->currentViewSupportingSearchAndReplaceInterface();
	if (!iface)
		return;
	d->updateFindDialogContents(true/*create if does not exist*/);
	d->findDialog()->setReplaceMode(false);

	d->findDialog()->show();
	d->findDialog()->setActiveWindow();
	d->findDialog()->raise();
}

void KexiMainWindowImpl::slotEditFind(bool next)
{
	KexiSearchAndReplaceViewInterface* iface = d->currentViewSupportingSearchAndReplaceInterface();
	if (!iface)
		return;
	tristate res = iface->find( 
		d->findDialog()->valueToFind(), d->findDialog()->options(), next);
	if (~res)
		return;
	d->findDialog()->updateMessage( true == res );
//! @todo result
}

void KexiMainWindowImpl::slotEditFindNext()
{
	slotEditFind( true );
}

void KexiMainWindowImpl::slotEditFindPrevious()
{
	slotEditFind( false );
}

void KexiMainWindowImpl::slotEditReplace()
{
	KexiSearchAndReplaceViewInterface* iface = d->currentViewSupportingSearchAndReplaceInterface();
	if (!iface)
		return;
	d->updateFindDialogContents(true/*create if does not exist*/);
	d->findDialog()->setReplaceMode(true);
//! @todo slotEditReplace()
	d->findDialog()->show();
	d->findDialog()->setActiveWindow();
}

void KexiMainWindowImpl::slotEditReplaceNext()
{
	slotEditReplace( false );
}

void KexiMainWindowImpl::slotEditReplace(bool all)
{
	KexiSearchAndReplaceViewInterface* iface = d->currentViewSupportingSearchAndReplaceInterface();
	if (!iface)
		return;
//! @todo add question: "Do you want to replace every occurence of \"%1\" with \"%2\"? 
//!       You won't be able to undo this." + "Do not ask again".
	tristate res = iface->findNextAndReplace( 
		d->findDialog()->valueToFind(), d->findDialog()->valueToReplaceWith(),
		d->findDialog()->options(), all);
	d->findDialog()->updateMessage( true == res );
//! @todo result
}

void KexiMainWindowImpl::slotEditReplaceAll()
{
	slotEditReplace( true );
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

/// TMP (until there's true template support)
void KexiMainWindowImpl::slotGetNewStuff()
{
#ifdef HAVE_KNEWSTUFF
	if(!d->newStuff)
		d->newStuff = new KexiNewStuff(this);
	d->newStuff->download();
	
	//KNS::DownloadDialog::open(newstuff->customEngine(), "kexi/template");
#endif
}

void KexiMainWindowImpl::highlightObject(const QCString& mime, const QCString& name)
{
	slotViewNavigator();
	if (!d->prj)
		return;
	KexiPart::Item *item = d->prj->itemForMimeType(mime, name);
	if (!item)
		return;
	if (d->nav) {
		d->nav->selectItem(*item);
	}
}

void KexiMainWindowImpl::slotPartItemSelectedInNavigator(KexiPart::Item* item)
{
	Q_UNUSED(item);
}

#include "keximainwindowimpl.moc"
