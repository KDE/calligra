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

#include "keximainwindow.h"

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

#include "kexibrowser.h"
#include "kexiactionproxy.h"
#include "kexidialogbase.h"
#include "kexipartmanager.h"
#include "kexipart.h"
#include "kexipartinfo.h"
#include "kexipartguiclient.h"
#include "kexiproject.h"
#include "kexiprojectdata.h"
#include "kexi.h"
#include "kexistatusbar.h"

#include "startup/KexiStartupDialog.h"
#include "startup/KexiConnSelector.h"
#include "startup/KexiProjectSelectorBase.h"
#include "startup/KexiProjectSelector.h"
#include "startup/KexiProjectSet.h"
#include "startup/KexiNewProjectWizard.h"
#include "startup/KexiStartup.h"
#include "kexicontexthelp.h"
#include <unistd.h>

typedef QIntDict<KexiDialogBase> KexiDialogDict;

class KexiMainWindow::Private
{
	public:
		KexiProject	*prj;
#ifndef KEXI_NO_CTXT_HELP
		KexiContextHelp *ctxH;
#endif
		KexiBrowser *nav;
		KexiDialogDict dialogs;
		KXMLGUIClient *curDialogGUIClient, *closedDialogGUIClient;
		QGuardedPtr<KexiDialogBase> curDialog;

		QPtrDict<KexiActionProxy> actionProxies;

		QAsciiDict<QPopupMenu> popups; //list of menu popups

		QString appCaption; //<! original application's caption

		//! project menu
		KAction *action_save, *action_save_as, *action_close,
		 *action_project_properties;
		KActionMenu *action_open_recent, *action_show_other;
		KAction *action_open_recent_more;
		int action_open_recent_more_id;

		//! edit menu
		KAction *action_edit_remove, 
			*action_edit_cut, *action_edit_copy, *action_edit_paste;
		// view menu
		KAction *action_view_nav;
#ifndef KEXI_NO_CTXT_HELP
		KToggleAction *action_show_helper;
#endif

		KMdiToolViewAccessor* navToolWindow;

		QWidget *focus_before_popup;

		bool block_KMdiMainFrm_eventFilter : 1;
	Private()
		: dialogs(401)
		, actionProxies(401)
	{
		navToolWindow=0;
		prj = 0;
		curDialogGUIClient=0;
		closedDialogGUIClient=0;
		curDialog=0;
		block_KMdiMainFrm_eventFilter=false;
		focus_before_popup=0;
	}
};

//-------------------------------------------------

KexiMainWindow::KexiMainWindow()
 : KMdiMainFrm(0L, "keximainwindow")
 	,d(new KexiMainWindow::Private() )
{
	d->nav = 0;
	KGlobal::iconLoader()->addAppDir("kexi");
	setXMLFile("kexiui.rc");
	setManagedDockPositionModeEnabled(true);//TODO(js): remove this if will be default in kmdi :)
	setStandardMDIMenuEnabled();

	//get informed
	connect(&Kexi::partManager(),SIGNAL(partLoaded(KexiPart::Part*)),this,SLOT(slotPartLoaded(KexiPart::Part*)));
	connect( m_pMdi, SIGNAL(nowMaximized(bool)), this, SLOT(slotCaptionForCurrentMDIChild(bool)) ); 
	connect( m_pMdi, SIGNAL(noMaximizedChildFrmLeft(KMdiChildFrm*)), this, SLOT(slotNoMaximizedChildFrmLeft(KMdiChildFrm*)));
	connect( m_pMdi, SIGNAL(lastChildFrmClosed()), this, SLOT(slotLastChildFrmClosed()));
	connect( this, SIGNAL(childViewIsDetachedNow(QWidget*)), this, SLOT(slotChildViewIsDetachedNow(QWidget*)));

	initActions();
	createShellGUI(true);
	(void) new KexiStatusBar(this, "status_bar");

	d->appCaption = caption();
	initContextHelp();
	

//	initBrowser();
	//TODO:new KexiProject();
//	connect(d->prj, SIGNAL(dbAvailable()), this, SLOT(initBrowser()));

	restoreSettings();

	{//store menu popups list
		QObjectList *l = queryList( "QPopupMenu" );
		for (QObjectListIt it( *l ); it.current(); ++it ) {
			kdDebug() << "name=" <<it.current()->name() << " cname="<<it.current()->className()<<endl;
			d->popups.insert(it.current()->name(), static_cast<QPopupMenu*>(it.current()));
		}
		delete l;
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
//	QTimer::singleShot(0, this, SLOT(parseCmdLineOptions()));
}

KexiMainWindow::~KexiMainWindow()
{
	closeProject();
	delete d;
}

KexiProject	*KexiMainWindow::project()
{
	return d->prj;
}

void KexiMainWindow::setWindowMenu(QPopupMenu *menu)
{
	if (m_pWindowMenu)
		delete m_pWindowMenu;
	m_pWindowMenu = menu;
	m_pWindowMenu->setCheckable(TRUE);
	QObject::connect( m_pWindowMenu, SIGNAL(aboutToShow()), this, SLOT(fillWindowMenu()) );
}

QPopupMenu* KexiMainWindow::findPopupMenu(const char *popupName)
{
	return d->popups[popupName];
}

void
KexiMainWindow::initActions()
{
	// PROJECT MENU
	KAction *action = new KAction(i18n("&New..."), "filenew", KStdAccel::shortcut(KStdAccel::New), 
		this, SLOT(slotProjectNew()), actionCollection(), "project_new");
	action->setWhatsThis(i18n("Create a new project"));
	KStdAction::open( this, SLOT( slotProjectOpen() ), actionCollection(), "project_open" )
		->setWhatsThis(i18n("Open an existing project"));
	d->action_open_recent = new KActionMenu(i18n("Open Recent"), 
		actionCollection(), "project_open_recent");
	connect(d->action_open_recent->popupMenu(),SIGNAL(activated(int)),this,SLOT(slotProjectOpenRecent(int)));
	connect(d->action_open_recent->popupMenu(), SIGNAL(aboutToShow()),this,SLOT(slotProjectOpenRecentAboutToShow()));
	d->action_open_recent->popupMenu()->insertSeparator();
	d->action_open_recent_more_id = d->action_open_recent->popupMenu()
		->insertItem(i18n("&More projects..."), this, SLOT(slotProjectOpenRecentMore()), 0, 1000);
	
	d->action_save = new KAction(i18n("&Save"), "filesave", KStdAccel::shortcut(KStdAccel::Save), 
		this, SLOT(slotProjectSave()), actionCollection(), "project_save");
	d->action_project_properties = new KAction(i18n("Project properties"), "info", 0,
		this, SLOT(slotProjectProperties()), actionCollection(), "project_properties");
	d->action_save_as = new KAction(i18n("Save &As..."), "filesaveas", 0, 
		this, SLOT(slotProjectSaveAs()), actionCollection(), "project_saveas");
	d->action_close = new KAction(i18n("&Close"), 0, KStdAccel::shortcut(KStdAccel::Close),
		this, SLOT(slotProjectClose()), actionCollection(), "project_close" );
	d->action_close->setWhatsThis(i18n("Close the current project."));
	KStdAction::quit( this, SLOT(slotQuit()), actionCollection(), "quit");

	//EDIT MENU
	d->action_edit_cut = KStdAction::cut( this, SLOT( slotEditCut() ), actionCollection(), "edit_cut" );
	d->action_edit_copy = KStdAction::copy( this, SLOT( slotEditCopy() ), actionCollection(), "edit_copy" );
	d->action_edit_paste = KStdAction::paste( this, SLOT( slotEditPaste() ), actionCollection(), "edit_paste" );
	d->action_edit_remove = new KAction(i18n("&Remove"), SmallIcon("button_cancel"), Key_Delete, this, 
		SLOT(slotEditRemove()), actionCollection(), "edit_remove");

	//VIEW MENU
	d->action_view_nav = new KAction(i18n("Navigator"), "", ALT + Key_1,
		this, SLOT(slotViewNavigator()), actionCollection(), "view_navigator");

	new KAction(i18n("From File..."), "fileopen", 0, 
		this, SLOT(slotImportFile()), actionCollection(), "import_file");
	new KAction(i18n("From Server..."), "server", 0, 
		this, SLOT(slotImportServer()), actionCollection(), "import_server");

	//SETTINGS MENU
	setStandardToolBarMenuEnabled( true );
	KStdAction::keyBindings(this, SLOT( slotConfigureKeys() ), actionCollection() );
	KStdAction::configureToolbars( this, SLOT( slotConfigureToolbars() ), actionCollection() );
	(void*) KStdAction::preferences(this, SLOT(slotShowSettings()), actionCollection());

	d->action_show_other = new KActionMenu(i18n("Other"), 
		actionCollection(), "options_show_other");
//	d->action_show_nav = new KToggleAction(i18n("Show Navigator"), "", CTRL + Key_B,
//	 actionCollection(), "options_show_nav");
#ifndef KEXI_NO_CTXT_HELP
	d->action_show_helper = new KToggleAction(i18n("Show Context Help"), "", CTRL + Key_H,
	 actionCollection(), "options_show_contexthelp");
#endif
	
	KAction *actionSettings = new KAction(i18n("Configure Kexi..."), "configure", 0,
	 actionCollection(), "kexi_settings");
	connect(actionSettings, SIGNAL(activated()), this, SLOT(slotShowSettings()));

	invalidateActions();
}

void KexiMainWindow::invalidateActions()
{
	invalidateProjectWideActions();
	invalidateSharedActions();
}

void KexiMainWindow::invalidateSharedActions(QWidget *w)
{
	//TODO: enabling is more complex...
/*	d->action_edit_cut->setEnabled(true);
	d->action_edit_copy->setEnabled(true);
	d->action_edit_paste->setEnabled(true);*/

//	isActionAvailable

//js: THIS WILL BE SIMPLIFIED
	if (!w)
		w = focusWidget();
	if (w) {
		KexiActionProxy *p = d->actionProxies[ w ];
		setActionAvailable("edit_remove",p && p->isAvailable("edit_remove"));
	}
	/*d->action_edit_remove->setEnabled(true);
	}
	if (d->nav && d->nav->hasFocus()) {
		d->action_edit_remove->setEnabled( d->nav->actionAvailable("edit_remove") );
	}*/
}

void KexiMainWindow::invalidateProjectWideActions()
{
//	stateChanged("project_opened",d->prj ? StateNoReverse : StateReverse);

	d->action_save->setEnabled(d->prj);
	d->action_save_as->setEnabled(d->prj);
	d->action_project_properties->setEnabled(d->prj);
	d->action_close->setEnabled(d->prj);
	d->action_view_nav->setEnabled(d->prj);

#ifndef KEXI_NO_CTXT_HELP
	d->action_show_helper->setEnabled(d->prj);
#endif
}

void KexiMainWindow::setActionAvailable(const char *name, bool avail)
{
	KAction *act = actionCollection()->action(name);
	if (!act)
		return;
	act->setEnabled(avail);
}

void KexiMainWindow::updateActionAvailable(const char *action_name, bool set, QObject *obj)
{
	if (obj !=  static_cast<QObject*>(focusWidget()))
		return;
	setActionAvailable(action_name, set);
}

/*bool KexiMainWindow::isActionAvailable(const char *action_name)
{
//	KexiActionProxy *proxy = d->actionProxies[ QCString(action_name)+QCString().setNum((ulong)proxy->receiver()) ];
	KexiActionProxy *proxy = d->actionProxies[ QCString(action_name)+QCString().setNum((ulong)proxy->receiver()) ];
	return proxy && proxy->isAvailable(action_name);
}*/

void KexiMainWindow::plugActionProxy(KexiActionProxy *proxy)//, const char *action_name)
{
//	d->actionProxies[ QCString(action_name)+QCString().setNum((ulong)proxy->receiver()) ] = proxy;
	d->actionProxies.insert( proxy->receiver(), proxy);
}

void KexiMainWindow::startup(KexiProjectData *projectData)
{
	kdDebug() << "KexiMainWindow::startup()..." << endl;
	if (!projectData) {
//<TEMP>
		//some connection data
		KexiDB::ConnectionData *conndata;
		conndata = new KexiDB::ConnectionData();
			conndata->name = "My connection";
			conndata->driverName = "mysql";
			conndata->hostName = "myhost.org";
			conndata->userName = "otheruser";
			conndata->port = 53121;
		Kexi::connset().addConnectionData(conndata);
		conndata = new KexiDB::ConnectionData();
			conndata->name = "Local pgsql connection";
			conndata->driverName = "postgresql";
			conndata->hostName = "localhost"; // -- default //"host.net";
			conndata->userName = getlogin(); //-- temporary e.g."jarek"
		Kexi::connset().addConnectionData(conndata);

		//some recent projects data
		projectData = new KexiProjectData( *conndata, "bigdb", "Big DB" );
		projectData->setCaption("My Big Project");
		projectData->setHelpText("This is my first biger project started yesterday. Have fun!");
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

bool KexiMainWindow::openProject(KexiProjectData *projectData)
{
	if (!projectData)
		return false;
	d->prj = new KexiProject( projectData );
	connect(d->prj, SIGNAL(error(const QString&,KexiDB::Object*)), this, SLOT(showErrorMessage(const QString&,KexiDB::Object*)));
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
	for (QValueList< QPair<QString,QString> >::Iterator it = projectData->autoopenObjects.begin(); it != projectData->autoopenObjects.end(); ++it ) {
		openObject(QString("kexi/")+(*it).first,(*it).second);
		KexiPart::Info *i = Kexi::partManager().info( QString("kexi/")+(*it).first );
		if (!i) {
			not_found_msg += ( (*it).second + " - " + i18n("unknown object type \"%1\"").arg((*it).first)+"<br>" );
			continue;
		}

		KexiPart::Item *item = d->prj->item(i, (*it).second);

		if (!item) {
			not_found_msg += ( (*it).second + " - " + i18n("object not found") +"<br>" );
			continue;
		}
		if (!openObject(item)) {
			not_found_msg += ( (*it).second + " - " + i18n("cannot open object").arg((*it).first) +"<br>" );
			continue;
		}
	}
	if (!not_found_msg.isEmpty())
		showErrorMessage(i18n("You have requested selected objects to be opened automatically on startup. Several objects cannot be opened."),
			not_found_msg );

	return true;
}

KexiProjectData*
KexiMainWindow::selectProject(KexiDB::ConnectionData *cdata)
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

bool KexiMainWindow::closeProject()
{
	if (!d->prj)
		return true;
		
	//TODO: ask if user wants to close if project was changed
	delete d->prj;
	d->prj=0;
	return true;
}

void KexiMainWindow::initContextHelp() {
#ifndef KEXI_NO_CTXT_HELP
	d->ctxH=new KexiContextHelp(this,this);
	d->ctxH->setContextHelp(i18n("Welcome"),i18n("The <B>KEXI team</B> wishes you a lot of productive work, "
		"with this product. <BR><HR><BR>If you have found a <B>bug</B> or have a <B>feature</B> request, please don't "
		"hesitate to report it at our <A href=\"http://www.kexi-project.org/cgi-bin/bug.pl\"> issue "
		"tracking system </A>.<BR><HR><BR>If you would like to <B>join</B> our effort, the <B>development</B> documentation "
		"at <A href=\"http://www.kexi-project.org\">www.kexi-project.org</A> is a good starting point."),0);
	addToolWindow(d->ctxH,KDockWidget::DockRight,getMainDockWidget(),20);
#endif
}

void
KexiMainWindow::initNavigator()
{
	kdDebug() << "KexiMainWindow::initNavigator()" << endl;

	if(!d->nav)
	{
//		d->nav = new KexiBrowser(this, "kexi/db", 0);
		d->nav = new KexiBrowser(this);
		d->nav->installEventFilter(this);
		d->navToolWindow = addToolWindow(d->nav, KDockWidget::DockLeft, getMainDockWidget(), 20/*, lv, 35, "2"*/);
		connect(d->nav,SIGNAL(openItem(KexiPart::Item*,bool)),this,SLOT(openObject(KexiPart::Item*,bool)));
//		connect(d->nav,SIGNAL(actionAvailable(const char*,bool)),this,SLOT(actionAvailable(const char*,bool)));

	}
	if(d->prj->isConnected()) {
		d->nav->clear();

		KexiPart::PartInfoList *pl = Kexi::partManager().partInfoList(); //d->prj->partManager()->partList();
		for(KexiPart::Info *it = pl->first(); it; it = pl->next())
		{
			kdDebug() << "KexiMainWindow::initNavigator(): adding " << it->groupName() << endl;
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
	
//	d->action_show_nav->setChecked(d->nav->isVisible());
//TODO	d->nav->plugToggleAction(m_actionBrowser);

}

void KexiMainWindow::slotPartLoaded(KexiPart::Part* p)
{
	p->createGUIClient(this); //new KexiPart::GUIClient(this, p, p->instanceName());
}

//! internal
void KexiMainWindow::slotCaptionForCurrentMDIChild(bool childrenMaximized)
{
	//js todo: allow to set custom "static" app caption
	KMdiChildView *view;
	if (!d->curDialog)
		view = 0;
	else if (d->curDialog->isAttached())
		view = d->curDialog;
	else {
		//currend dialog isn't attached! - find top level child
		if (m_pMdi->topChild()) {
			view = m_pMdi->topChild()->m_pClient;
			childrenMaximized = view->mdiParent()->state()==KMdiChildFrm::Maximized;
		}
		else
			view = 0;
	}

	if (childrenMaximized && view) {
		setCaption( view->caption() );
	}
	else {
		setCaption( d->appCaption );
	}
}

void KexiMainWindow::slotNoMaximizedChildFrmLeft(KMdiChildFrm*)
{
	slotCaptionForCurrentMDIChild(false);
}

void KexiMainWindow::slotLastChildFrmClosed()
{
	slotCaptionForCurrentMDIChild(false);
}

void KexiMainWindow::slotChildViewIsDetachedNow(QWidget*)
{
	slotCaptionForCurrentMDIChild(false);
}

void
KexiMainWindow::closeEvent(QCloseEvent *ev)
{
	storeSettings();
	
	closeProject();
	
	ev->accept();
}

void
KexiMainWindow::restoreSettings()
{
	KConfig *config = kapp->config();
	config->setGroup("MainWindow");

	//small hack - set the default -- bottom
	config->setGroup(QString(name()) + " KMdiTaskBar Toolbar style");
	if (config->readEntry("Position").isEmpty()) {
		config->writeEntry("Position","Bottom");
		moveDockWindow(m_pTaskBar, DockBottom);
	}
	config->setGroup("MainWindow");

	// Saved settings
	applyMainWindowSettings( config );//, instance()->instanceName() );

	int mdimode = config->readNumEntry("MDIMode", -1);//KMdi::TabPageMode);

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

//	setGeometry(config->readRectEntry("Geometry", new QRect(150, 150, 400, 500)));

	if ( !initialGeometrySet() ) {
		// Default size
#if KDE_IS_VERSION(3,1,90)
		const int deskWidth = KGlobalSettings::desktopGeometry(this).width();
#else
		const int deskWidth = QApplication::desktop()->width();
#endif
		if (deskWidth > 1100) // very big desktop ?
			resize( 1000, 800 );
		if (deskWidth > 850) // big desktop ?
			resize( 800, 600 );
		else // small (800x600, 640x480) desktop
			resize( 600, 400 );
	}
}

void
KexiMainWindow::storeSettings()
{
	kdDebug() << "KexiMainWindow::storeSettings()" << endl;

	KConfig *config = kapp->config();
	config->setGroup("MainWindow");
	saveWindowSize( config ); //instance()->config() );
	saveMainWindowSettings( config );
	config->writeEntry("MDIMode", mdiMode());
	config->sync();
}

void
KexiMainWindow::registerChild(KexiDialogBase *dlg)
{
	kdDebug() << "KexiMainWindow::registerChild()" << endl;
	connect(dlg, SIGNAL(activated(KMdiChildView *)), this, SLOT(activeWindowChanged(KMdiChildView *)));
	connect(dlg, SIGNAL(childWindowCloseRequest(KMdiChildView *)), this, SLOT(childClosed(KMdiChildView *)));
	if(dlg->docID() != -1)
		d->dialogs.insert(dlg->docID(), dlg);
	kdDebug() << "KexiMainWindow::registerChild() docID = " << dlg->docID() << endl;

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
KexiMainWindow::activeWindowChanged(KMdiChildView *v)
{
	KexiDialogBase *dlg = static_cast<KexiDialogBase *>(v);
	kdDebug() << "KexiMainWindow::activeWindowChanged() to = " << (dlg ? dlg->caption() : "") << endl;

	KXMLGUIClient *client;

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
			kdDebug()<<"KexiMainWindow::activeWindowChanged(): old gui client:"<<d->curDialogGUIClient<<" new gui client: "<<client<<endl;
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

	d->curDialogGUIClient=client;
	bool dialogChanged = (KexiDialogBase*)d->curDialog!=dlg;
	d->curDialog=dlg;

	if (dialogChanged) {
//		invalidateSharedActions();
	}

	//update caption...
	if (update_dlg_caption) {
		slotCaptionForCurrentMDIChild(d->curDialog->mdiParent()->state()==KMdiChildFrm::Maximized);
	}
}

bool
KexiMainWindow::activateWindow(int id)
{
	kdDebug() << "KexiMainWindow::activateWindow()" << endl;
	KexiDialogBase *dlg = d->dialogs[id];
	if(!dlg)
		return false;

	dlg->activate();
	return true;
}

void
KexiMainWindow::childClosed(KMdiChildView *v)
{
	kdDebug() << "KexiMainWindow::unregisterWindow()" << endl;
	KexiDialogBase *dlg = static_cast<KexiDialogBase *>(v);
	d->dialogs.remove(dlg->docID());

	//focus navigator if nothing else available
	if (d->dialogs.isEmpty())
		d->nav->setFocus();
}

void
KexiMainWindow::slotShowSettings()
{
//TODO	KexiSettings s(this);
//	s.exec();
}

void
KexiMainWindow::slotConfigureKeys()
{
/*    KKeyDialog dlg;
    dlg.insert( actionCollection() );
    dlg.configure();*/
	KKeyDialog::configure( actionCollection() );
}

void
KexiMainWindow::slotConfigureToolbars()
{
    KEditToolbar edit(factory());
//    connect(&edit,SIGNAL(newToolbarConfig()),this,SLOT(slotNewToolbarConfig()));
    (void) edit.exec();
}

void 
KexiMainWindow::slotProjectNew()
{
	if (d->prj)//js: TODO: start new instance!
		return;
	createBlankDatabase();
}

bool
KexiMainWindow::createBlankDatabase()
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
		cdata.name = wiz.projectCaption();
		cdata.driverName = "sqlite";
		cdata.setFileName( wiz.projectDBName() );
		new_data = new KexiProjectData( cdata, wiz.projectDBName(), wiz.projectCaption() );
	}
	else
		return false;

	d->prj = new KexiProject( new_data );
	connect(d->prj, SIGNAL(error(const QString&,KexiDB::Object*)), this, SLOT(showErrorMessage(const QString&,KexiDB::Object*)));
	if (!d->prj->create()) {
		delete d->prj;
		d->prj = 0;
		return false;
	}
	kdDebug() << "KexiMainWindow::slotProjectNew(): new project created --- " << endl;
	initNavigator();
	Kexi::recentProjects().addProjectData( new_data );

	invalidateActions();
	return true;
}

void
KexiMainWindow::slotProjectOpen()
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
KexiMainWindow::slotProjectOpenRecentAboutToShow()
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
KexiMainWindow::slotProjectOpenRecent(int id)
{
	if (id<0 || id==d->action_open_recent_more_id)
		return;
	kdDebug() << "KexiMainWindow::slotProjectOpenRecent("<<id<<")"<<endl;
}

void
KexiMainWindow::slotProjectOpenRecentMore()
{
}

void
KexiMainWindow::slotProjectSave()
{
}

void
KexiMainWindow::slotProjectSaveAs()
{
}

void
KexiMainWindow::slotProjectProperties()
{
}

void
KexiMainWindow::slotProjectClose()
{
}

void KexiMainWindow::slotEditCut()
{
}

void KexiMainWindow::slotEditCopy()
{
}

void KexiMainWindow::slotEditPaste()
{
}

void KexiMainWindow::slotEditRemove()
{
	QWidget *w = focusWidget();
	if (!w)
		return;

//	KexiActionProxy * proxy = d->actionProxies[ QPair<QObject*,const char *>(w, "edit_remove") ];
	KexiActionProxy * proxy = d->actionProxies[ w ]; //QCString("edit_remove")+QCString().setNum((ulong)w) ];
	if (!proxy)
		return;
	proxy->activateAction("edit_remove");
/*	if (d->nav->hasFocus()) {
		//TODO
	}*/
}

void KexiMainWindow::slotImportFile() {
}

void
KexiMainWindow::slotQuit()
{
	//TODO
	close();
}

void KexiMainWindow::slotViewNavigator()
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

void
KexiMainWindow::showErrorMessage(const QString &title, const QString &details)
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
KexiMainWindow::showErrorMessage(const QString &title, KexiDB::Object *obj)
{
	QString msg = title;
	if (!obj) {
		showErrorMessage(msg);
		return;
	}
	msg += ("<p>"+obj->errorMsg());
	QString details;
	if (!obj->serverErrorMsg().isEmpty())
		details += "<p><b>" +i18n("Message from server:") + "</b> " + obj->serverErrorMsg();
	QString resname = obj->serverResultName();
	if (!resname.isEmpty())
		details += (QString("<p><b>")+i18n("Server result name:")+"</b> "+resname);
	if (!details.isEmpty()) {
		details += (QString("<p><b>")+i18n("Result number:")+"</b> "+QString::number(obj->serverResult()));
//		KMessageBox::detailedError(this, msg, details);
	}
	showErrorMessage(msg, details);
}

void
KexiMainWindow::closeWindow(KMdiChildView *pWnd, bool layoutTaskBar)
{
	if (!pWnd)
		return;
	KXMLGUIClient *client = static_cast<KexiDialogBase *>(pWnd)->guiClient();
	if (d->curDialogGUIClient==client) {
		d->curDialogGUIClient=0;
	}
	if (client) {
		if (d->closedDialogGUIClient && d->closedDialogGUIClient!=client) //sanity: ouch, it is not removed yet? - do it now
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
	KMdiMainFrm::closeWindow(pWnd, layoutTaskBar);
}

void KexiMainWindow::detachWindow(KMdiChildView *pWnd,bool bShow)
{
	KMdiMainFrm::detachWindow(pWnd,bShow);
	// update icon
	pWnd->setIcon( DesktopIcon( static_cast<KexiDialogBase *>(pWnd)->part()->info()->itemIcon() ) );
}

void KexiMainWindow::attachWindow(KMdiChildView *pWnd,bool bShow,bool bAutomaticResize)
{
	KMdiMainFrm::attachWindow(pWnd,true,bAutomaticResize);
	// update icon
	pWnd->mdiParent()->setIcon( SmallIcon( static_cast<KexiDialogBase *>(pWnd)->part()->info()->itemIcon() ) );
}

bool KexiMainWindow::eventFilter( QObject *obj, QEvent * e )
{
//	kdDebug() << "eventFilter: " <<e->type() << " " <<obj->name()<<endl;
	if (e->type()==QEvent::KeyPress) {
		kdDebug() << "KEY EVENT" << endl;
	}
	if (e->type()==QEvent::AccelOverride) {
		kdDebug() << "AccelOverride EVENT" << endl;
	}
	if (e->type()==QEvent::FocusIn || e->type()==QEvent::FocusOut) {
		kdDebug() << "Focus EVENT" << endl;
		QWidget *w = focusWidget();
		kdDebug() << (w ? w->name() : "" )  << endl;
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
		if (d->actionProxies[ obj ]) {
			invalidateSharedActions();
		}
		else {
			QObject* o = focusWidget();
			while (o && !o->inherits("KexiDialogBase"))
				o = o->parent();
			if (o)
				invalidateSharedActions(static_cast<QWidget*>(o));
		}
//		/*|| e->type()==QEvent::FocusOut*/) && /*(!obj->inherits("KexiDialogBase")) &&*/ d->actionProxies[ obj ]) {
//		invalidateSharedActions();
	}
	if ((e->type()==QEvent::FocusOut && focusWidget()==d->curDialog && !obj->inherits("KexiDialogBase")) && d->actionProxies[ obj ]) {
		invalidateSharedActions(d->curDialog);
	}

	//keep focus in main window:
	if (obj==d->nav) {
//		kdDebug() << "NAV" << endl;
		if (e->type()==QEvent::FocusIn) {
			return true;
		} else if (e->type()==QEvent::WindowActivate && obj==d->focus_before_popup) {
			d->focus_before_popup=0;
			d->nav->setFocus();
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

			}
		} else if (e->type()==QEvent::Hide) {
			setFocus();
			return false;
		}
	}
	if (d->block_KMdiMainFrm_eventFilter)//we don't want KMDI to eat our event!
		return false;
	return KMdiMainFrm::eventFilter(obj,e);//let KMDI do its work
}

bool
KexiMainWindow::openObject(const QString& mime, const QString& name, bool designMode)
{
	KexiPart::Item *item = d->prj->item(mime,name);
	if (!item)
		return false;
	return openObject(item, designMode);
}

bool
KexiMainWindow::openObject(KexiPart::Item* item, bool designMode)
{
	if (!item)
		return false;
	if (activateWindow(item->identifier()))
		return true;

	KexiPart::Part *part = Kexi::partManager().part(item->mime());
	if (!part) {
		//TODO js: error msg
		return false;
	}
	return part->openInstance(this, *item, designMode) != 0;
}

#include "keximainwindow.moc"

