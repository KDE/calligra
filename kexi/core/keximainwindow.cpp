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
#include "kexistartupdlg.h"
#include "kexiproject.h"
#include "startup/KexiStartupDialog.h"
#include "startup/KexiConnSelector.h"
#include "startup/KexiProjectSet.h"
#include "kexiprojectdata.h"

#include "kexi.h"

//-------------------------------------------------

class KexiMainWindow::Private
{
	public:
		//! project menu
		KAction *action_save, *action_save_as, *action_close,
		 *action_project_properties;
		KActionMenu *action_open_recent;
		KAction *action_open_recent_more;
		int action_open_recent_more_id;
		// view menu
#ifndef KEXI_NO_CTXT_HELP
		KToggleAction *action_show_helper;
#endif
		KToggleAction *action_show_browser;
	Private()
	{
	}
};

//-------------------------------------------------

KexiMainWindow::KexiMainWindow()
 : KMdiMainFrm(0L, "keximain")
	,m_currentDocumentGUIClient(0)
 	,d(new KexiMainWindow::Private() )
{
	m_browser = 0;
	m_project = 0;	
	setXMLFile("kexiui.rc");
	setStandardMDIMenuEnabled();
	setManagedDockPositionModeEnabled(true);//TODO(js): remove this if will be default in kmdi :)

	initActions();
	createShellGUI(true);
//	createGUI(0);

//	initBrowser();
	//TODO:new KexiProject();
//	connect(m_project, SIGNAL(dbAvailable()), this, SLOT(initBrowser()));

	restoreSettings();
//	QTimer::singleShot(0, this, SLOT(parseCmdLineOptions()));
}

KexiMainWindow::~KexiMainWindow()
{
	delete d;
	closeProject();
}

void
KexiMainWindow::initActions()
{
	// PROJECT MENU
	new KAction(i18n("&New"), "filenew", KStdAccel::shortcut(KStdAccel::New), 
		this, SLOT(slotProjectNew()), actionCollection(), "project_new");
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
//	new KAction(i18n("&Quit"), "exit", KStdAccel::shortcut(KStdAccel::Quit), qApp, SLOT(slotFileQuit()), actionCollection(), "file_quit");

	//SETTINGS MENU
	setStandardToolBarMenuEnabled( true );
    KStdAction::keyBindings(this, SLOT( slotConfigureKeys() ), actionCollection() );
    KStdAction::configureToolbars( this, SLOT( slotConfigureToolbars() ), actionCollection() );
	(void*) KStdAction::preferences(this, SLOT(slotShowSettings()), actionCollection());
	
	KAction *actionSettings = new KAction(i18n("Configure Kexi..."), "configure", 0,
	 actionCollection(), "kexi_settings");
	connect(actionSettings, SIGNAL(activated()), this, SLOT(slotShowSettings()));

	//VIEW MENU
	d->action_show_browser = new KToggleAction(i18n("Show Navigator"), "", CTRL + Key_B,
	 actionCollection(), "show_nav");

#ifndef KEXI_NO_CTXT_HELP
	d->action_show_helper = new KToggleAction(i18n("Show Context Help"), "", CTRL + Key_H,
	 actionCollection(), "show_contexthelp");
#endif

	invalidateActions();
}

void KexiMainWindow::invalidateActions()
{
	stateChanged("project_opened",m_project ? StateNoReverse : StateReverse);
	
/*	d->action_save->setEnabled(m_project);
	d->action_save_as->setEnabled(m_project);
	d->action_project_properties->setEnabled(m_project);
	d->action_close->setEnabled(m_project);*/
	d->action_show_browser->setEnabled(m_project && m_browser);
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
			conndata->name = "My connection 1";
			conndata->driverName = "mysql";
			conndata->hostName = "host.net";
			conndata->userName = "user";
		Kexi::connset.addConnectionData(conndata);
		conndata = new KexiDB::ConnectionData();
			conndata->name = "My connection 2";
			conndata->driverName = "mysql";
			conndata->hostName = "myhost.org";
			conndata->userName = "otheruser";
			conndata->port = 53121;
		Kexi::connset.addConnectionData(conndata);

		//some recent projects data
		KexiProjectData *prjdata;
		prjdata = new KexiProjectData( *conndata, "bigdb" );
		prjdata->setCaption("My Big Project");
		prjdata->setHelpText("This is my first biger project started yesterday. Have fun!");
		Kexi::recentProjects.addProjectData(prjdata);
	//</TEMP>

		KexiStartupDialog dlg(KexiStartupDialog::Everything, KexiStartupDialog::CheckBoxDoNotShowAgain,
			Kexi::connset, Kexi::recentProjects, 0, "dlg");
		int e=dlg.exec();
		kdDebug() << (e==QDialog::Accepted ? "Accepted" : "Rejected") << endl;
	
		if (e==QDialog::Accepted) {
			int r = dlg.result();
			if (r==KexiStartupDialog::TemplateResult) {
				kdDebug() << "Template key == " << dlg.selectedTemplateKey() << endl;
				if (dlg.selectedTemplateKey()=="blank") {
					createBlankDatabase();
				}
			}
			else if (r==KexiStartupDialog::OpenExistingResult) {
				kdDebug() << "Existing project --------" << endl;
				QString selFile = dlg.selectedExistingFile();
				if (!selFile.isEmpty())
					kdDebug() << "Project File: " << selFile << endl;
				else if (dlg.selectedExistingConnection()) {
					kdDebug() << "Existing connection: " << dlg.selectedExistingConnection()->serverInfoString() << endl;
					//ok, now we are trying to show daabases for this conenction to this user
					//todo
				}
			}
			else if (r==KexiStartupDialog::OpenRecentResult) {
				kdDebug() << "Recent project --------" << endl;
				const KexiProjectData *data = dlg.selectedProjectData();
				if (data) {
					kdDebug() << "Selected project: database=" << data->databaseName()
						<< " connection=" << data->constConnectionData()->serverInfoString() << endl;
				}
			}
		}
	}
	
	//we have got a project to open
	if (projectData) { //.databaseName().isEmpty()) {
		openProject( projectData );
	}
	
	invalidateActions();
}

bool KexiMainWindow::createBlankDatabase()
{
	KexiDB::ConnectionData *cdata = 0;
	if (!KexiConnSelectorDialog::alwaysUseFilesForNewProjects()) {
		KexiConnSelectorDialog sel(Kexi::connset, 0,"sel");
		int e = sel.exec();
		kdDebug() << (e==QDialog::Accepted ? "Accepted" : "Rejected") << endl;
		if (e==QDialog::Accepted) {
			if (sel.selectedConnectionType()==KexiConnSelectorWidget::FileBased) {
				kdDebug() << "Selected conn. type: File based" << endl;
			}
			else if (sel.selectedConnectionType()==KexiConnSelectorWidget::ServerBased) {
				kdDebug() << "Selected conn. type: Server based" << endl;
				cdata = sel.selectedConnectionData();
				kdDebug() << "SERVER: " << cdata->serverInfoString() << endl;
			}
			else
				return false;
		}
		else
			return false;
	}
		
}

void KexiMainWindow::openProject(KexiProjectData *pdata)
{
	if (!closeProject())
		return;
	
	m_project = new KexiProject(pdata);
	if (!m_project->open()) {
		KMessageBox::sorry(this, i18n("<qt>Could not open project:<p>%1</qt>").arg(m_project->errorMsg()));
		closeProject();
		return;
	}
	initBrowser();
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
KexiMainWindow::initBrowser()
{
	kdDebug() << "KexiMainWindow::initBrowser()" << endl;

	if(!m_browser)
	{
		m_browser = new KexiBrowser(this, "kexi/db", 0);
		addToolWindow(m_browser, KDockWidget::DockLeft, getMainDockWidget(), 20/*, lv, 35, "2"*/);
	}

	if(m_project->isConnected())
	{
		m_browser->clear();

		KexiPart::Parts *pl = m_project->partManager()->partList();
		for(KexiPart::Info *it = pl->first(); it; it = pl->next())
		{
			kdDebug() << "KexiMainWindow::initBrowser(): adding " << it->groupName() << endl;
			m_browser->addGroup(it);
		}
	}
	
	d->action_show_browser->setChecked(m_browser->isVisible());
//TODO	m_browser->plugToggleAction(m_actionBrowser);

}

#if 0
void
KexiMainWindow::parseCmdLineOptions()
{
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if(args->count() > 0 && m_project->open(QFile::decodeName(args->arg(0))))
	{
		kdDebug() << "KexiMainWindow::parseCmdLineOptions(): opened" << endl;
	}
	else
	{
		KexiStartupDlg *dlg = new KexiStartupDlg(this);
		int res = dlg->exec();
		switch(res)
		{
			case KexiStartupDlg::Cancel:
				qApp->quit();
			case KexiStartupDlg::OpenExisting:
				if(!m_project->open(dlg->fileName()))
					KMessageBox::error(this, m_project->errorMsg());
				break;
			default:
				return;
		}
	}

}
#endif
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
	kdDebug() << "KexiMainWindow::registerChild()" << endl;
}

void
KexiMainWindow::activeWindowChanged(KMdiChildView *v)
{
	kdDebug() << "KexiMainWindow::activeWindowChanged()" << endl;
	KexiDialogBase *dlg = static_cast<KexiDialogBase *>(v);
	KXMLGUIClient *client=dynamic_cast<KXMLGUIClient*>(dlg);
	kdDebug() << "KexiMainWindow::activeWindowChanged(): dlg = " << dlg << endl;
	if (client!=m_currentDocumentGUIClient) {
		if (m_currentDocumentGUIClient) guiFactory()->removeClient(m_currentDocumentGUIClient);
		if (client) guiFactory()->addClient(client);
	}
	m_currentDocumentGUIClient=client;
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
}

void
KexiMainWindow::slotProjectOpen()
{
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


#include "keximainwindow.moc"

