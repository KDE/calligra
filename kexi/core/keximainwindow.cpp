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
#include "kexidialogbase.h"
#include "kexipartmanager.h"
#include "kexipart.h"
#include "kexistartupdlg.h"
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

#include <unistd.h>

class KexiMainWindow::Private
{
	public:
		//! project menu
		KAction *action_save, *action_save_as, *action_close,
		 *action_project_properties;
		KActionMenu *action_open_recent, *action_show_other;
		KAction *action_open_recent_more;
		int action_open_recent_more_id;
		// view menu
#ifndef KEXI_NO_CTXT_HELP
		KToggleAction *action_show_helper;
#endif
		KToggleAction *action_show_nav;
	Private()
	{
	}
};

//-------------------------------------------------

KexiMainWindow::KexiMainWindow()
 : KMdiMainFrm(0L, "keximainwindow")
	,m_currentDocumentGUIClient(0)
 	,d(new KexiMainWindow::Private() )
{
	m_nav = 0;
	m_project = 0;	
	KGlobal::iconLoader()->addAppDir("kexi");
	setXMLFile("kexiui.rc");
	setManagedDockPositionModeEnabled(true);//TODO(js): remove this if will be default in kmdi :)
	setStandardMDIMenuEnabled();

	initActions();
	createShellGUI(true);
	(void) new KexiStatusBar(this, "status_bar");

//	initBrowser();
	//TODO:new KexiProject();
//	connect(m_project, SIGNAL(dbAvailable()), this, SLOT(initBrowser()));

	restoreSettings();
	
	if (!isFakingSDIApplication()) {
		QPopupMenu *menu = (QPopupMenu*) child( "window", "KPopupMenu" );
		unsigned int count = menuBar()->count();
		if (menu)
			setWindowMenu(menu);
		else
			menuBar()->insertItem( i18n("&Window"), windowMenu(), -1, count-2); // standard position is left to the last ('Help')
	}
	
//	QTimer::singleShot(0, this, SLOT(parseCmdLineOptions()));
}

KexiMainWindow::~KexiMainWindow()
{
	delete d;
	closeProject();
}

void KexiMainWindow::setWindowMenu(QPopupMenu *menu)
{
    if (m_pWindowMenu)
        delete m_pWindowMenu;
    m_pWindowMenu = menu;
    m_pWindowMenu->setCheckable(TRUE);
    QObject::connect( m_pWindowMenu, SIGNAL(aboutToShow()), this, SLOT(fillWindowMenu()) );
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

	//SETTINGS MENU
	setStandardToolBarMenuEnabled( true );
    KStdAction::keyBindings(this, SLOT( slotConfigureKeys() ), actionCollection() );
    KStdAction::configureToolbars( this, SLOT( slotConfigureToolbars() ), actionCollection() );
	(void*) KStdAction::preferences(this, SLOT(slotShowSettings()), actionCollection());

	d->action_show_other = new KActionMenu(i18n("Other"), 
		actionCollection(), "options_show_other");
	d->action_show_nav = new KToggleAction(i18n("Show Navigator"), "", CTRL + Key_B,
	 actionCollection(), "options_show_nav");
#ifndef KEXI_NO_CTXT_HELP
	d->action_show_helper = new KToggleAction(i18n("Show Context Help"), "", CTRL + Key_H,
	 actionCollection(), "options_show_contexthelp");
#endif
	
	KAction *actionSettings = new KAction(i18n("Configure Kexi..."), "configure", 0,
	 actionCollection(), "kexi_settings");
	connect(actionSettings, SIGNAL(activated()), this, SLOT(slotShowSettings()));

	//VIEW MENU
//	d->action_show_browser = new KToggleAction(i18n("Show Navigator"), "", CTRL + Key_B,
//	 actionCollection(), "options_show_nav");

#ifndef KEXI_NO_CTXT_HELP
//	d->action_show_helper = new KToggleAction(i18n("Show Context Help"), "", CTRL + Key_H,
//	 actionCollection(), "options_show_contexthelp");
#endif

	invalidateActions();
}

void KexiMainWindow::invalidateActions()
{
	stateChanged("project_opened",m_project ? StateNoReverse : StateReverse);
	
	d->action_show_nav->setEnabled(m_project && m_nav);
#ifndef KEXI_NO_CTXT_HELP
	d->action_show_helper->setEnabled(m_project);
#endif
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
	m_project = new KexiProject( projectData );
	connect(m_project, SIGNAL(error(const QString&,KexiDB::Object*)), this, SLOT(slotShowErrorMessageFor(const QString&,KexiDB::Object*)));
	if (!m_project->open()) {
		delete m_project;
		m_project = 0;
		return false;
	}
	initNavigator();
	Kexi::recentProjects().addProjectData( projectData );
	invalidateActions();
	
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
		slotShowErrorMessageFor(i18n("Could not load list of available projects for connection \"%1\"")
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
	if (!m_project)
		return true;
		
	//TODO: ask if user wants to close if project was changed
	delete m_project;
	m_project=0;
	return true;
}

void
KexiMainWindow::initNavigator()
{
	kdDebug() << "KexiMainWindow::initNavigator()" << endl;

	if(!m_nav)
	{
		m_nav = new KexiBrowser(this, "kexi/db", 0);
		m_nav->setCaption(i18n("Navigator"));
		addToolWindow(m_nav, KDockWidget::DockLeft, getMainDockWidget(), 20/*, lv, 35, "2"*/);
	}

	if(m_project->isConnected())
	{
		m_nav->clear();

		KexiPart::PartList *pl = Kexi::partManager().partList(); //m_project->partManager()->partList();
		for(KexiPart::Info *it = pl->first(); it; it = pl->next())
		{
			kdDebug() << "KexiMainWindow::initNavigator(): adding " << it->groupName() << endl;
			m_nav->addGroup(it);
			KexiPart::Part *p=Kexi::partManager().part(it);
			if (p) p->createGUIClient(this);
		}
	}
	
	d->action_show_nav->setChecked(m_nav->isVisible());
//TODO	m_nav->plugToggleAction(m_actionBrowser);

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
	KConfig *config = KGlobal::config();
	config->setGroup("MainWindow");
	int mdimode = config->readNumEntry("MDIMode", KMdi::TabPageMode);

	switch(mdimode)
	{
		case KMdi::ToplevelMode:
			switchToToplevelMode();
			break;
		case KMdi::ChildframeMode:
			switchToChildframeMode();
			break;
		case KMdi::IDEAlMode:
			switchToIDEAlMode();
			break;
		default:
			switchToTabPageMode();
			break;
	}

//	setGeometry(config->readRectEntry("Geometry", new QRect(150, 150, 400, 500)));

    // Saved size
    restoreWindowSize( config );
    
	if ( !initialGeometrySet() )
    {
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

	KConfig *config = KGlobal::config();
	config->setGroup("MainWindow");
	saveWindowSize( instance()->config() );
	saveMainWindowSettings( KGlobal::config(), instance()->instanceName() );
//	config->writeEntry("Geometry", geometry());
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
		m_docs.insert(dlg->docID(), dlg);
	kdDebug() << "KexiMainWindow::registerChild() docID = " << dlg->docID() << endl;
}

void
KexiMainWindow::activeWindowChanged(KMdiChildView *v)
{
	kdDebug() << "KexiMainWindow::activeWindowChanged()" << endl;
	KexiDialogBase *dlg = static_cast<KexiDialogBase *>(v);
	kdDebug() << "KexiMainWindow::activeWindowChanged(): dlg = " << dlg << endl;

	if (dlg && (!dlg->isRegistered())) return;
	KXMLGUIClient *client=dlg->guiClient();
	if (client!=m_currentDocumentGUIClient) {
		if (m_currentDocumentGUIClient) guiFactory()->removeClient(m_currentDocumentGUIClient);
		if (client) guiFactory()->addClient(client);
	}
	m_currentDocumentGUIClient=client;
}

bool
KexiMainWindow::activateWindow(int id)
{
	kdDebug() << "KexiMainWindow::activateWindow()" << endl;
	KexiDialogBase *dlg = m_docs[id];
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
	m_docs.remove(dlg->docID());
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
	if (m_project)//js: TODO: start new instance!
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

	m_project = new KexiProject( new_data );
	connect(m_project, SIGNAL(error(const QString&,KexiDB::Object*)), this, SLOT(slotShowErrorMessageFor(const QString&,KexiDB::Object*)));
	if (!m_project->create()) {
		delete m_project;
		m_project = 0;
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

	if (m_project)//js: TODO: start new instance!
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

void
KexiMainWindow::slotQuit()
{
	//TODO
	close();
}

void
KexiMainWindow::slotShowErrorMessageFor(const QString &title, KexiDB::Object *obj)
{
	if (!obj)
		return;
	QString msg;
	if (!title.isEmpty())
		msg = "<qt><p><b>"+title+"</b><p>";
	else
		msg = "<qt><p>";
	if (!obj) {
		KMessageBox::error(this, msg);
		return;
	}
	msg += obj->errorMsg();
	QString details;
	if (!obj->serverErrorMsg().isEmpty())
		details += "<qt><p><b>" +i18n("Message from server:") + "</b> " + obj->serverErrorMsg();
	QString resname = obj->serverResultName();
	if (!resname.isEmpty())
		details += (QString("<p><b>")+i18n("Server result name:")+"</b> "+resname);
	if (!details.isEmpty()) {
		details += (QString("<p><b>")+i18n("Result number:")+"</b> "+QString::number(obj->serverResult()));
		KMessageBox::detailedError(this, msg, details);
	}
	else {
		KMessageBox::error(this, msg);
	}
}

#include "keximainwindow.moc"

