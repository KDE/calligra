/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>

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

#include "kexibrowser.h"
#include "kexidialogbase.h"
#include "kexipartmanager.h"
#include "kexistartupdlg.h"
#include "kexiproject.h"
#include "keximainwindow.h"

KexiMainWindow::KexiMainWindow()
 : KMdiMainFrm(0L, "keximain")
{
	m_browser = 0;
	m_project = new KexiProject();
	setManagedDockPositionModeEnabled(true);
	setStandardMDIMenuEnabled();
	setXMLFile("kexiui.rc");

	initActions();
	createGUI(0);

	initBrowser();
	connect(m_project, SIGNAL(dbAvailable()), this, SLOT(initBrowser()));

	restoreSettings();


	QTimer::singleShot(0, this, SLOT(parseCmdLineOptions()));
}


KexiMainWindow::~KexiMainWindow()
{
}

void
KexiMainWindow::initActions()
{
	//FILE MENU
	new KAction(i18n("&New"), "filenew", KStdAccel::shortcut(KStdAccel::New), this, SLOT(fileNew()), actionCollection(), "file_new");
	new KAction(i18n("&Open"), "fileopen", KStdAccel::shortcut(KStdAccel::Open), this, SLOT(fileNew()), actionCollection(), "file_open");
	new KAction(i18n("&Save"), "filesave", KStdAccel::shortcut(KStdAccel::Save), this, SLOT(fileNew()), actionCollection(), "file_save");
	new KAction(i18n("Save &As"), "filesaveas", 0, this, SLOT(fileNew()), actionCollection(), "file_save_as");
	new KAction(i18n("&Quit"), "exit", KStdAccel::shortcut(KStdAccel::Quit), qApp, SLOT(quit()), actionCollection(), "file_quit");

	//SETTINGS MENU
	(void*) KStdAction::preferences(this, SLOT(slotSettings()), actionCollection());

    KStdAction::keyBindings( this, SLOT( slotConfigureKeys() ), actionCollection() );
    KStdAction::configureToolbars( this, SLOT( slotConfigureToolbars() ), actionCollection() );
	
	KAction *actionSettings = new KAction(i18n("Configure Kexi..."), "configure", 0,
	 actionCollection(), "kexi_settings");
	connect(actionSettings, SIGNAL(activated()), this, SLOT(slotShowSettings()));

	//VIEW MENU
	m_actionBrowser = new KToggleAction(i18n("Show Navigator"), "", CTRL + Key_B,
	 actionCollection(), "show_nav");

#ifndef KEXI_NO_CTXT_HELP
	m_actionHelper = new KToggleAction(i18n("Show Context Help"), "", CTRL + Key_H,
	 actionCollection(), "show_contexthelp");
#endif

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
	
	m_actionBrowser->setChecked(m_browser->isVisible());
//TODO	m_browser->plugToggleAction(m_actionBrowser);

}

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
					KMessageBox::error(this, m_project->error(), i18n("Connection Error"));
				break;
			default:
				return;
		}
	}

}

void
KexiMainWindow::closeEvent(QCloseEvent *ev)
{
	storeSettings();
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
		default:
			switchToTabPageMode();
			break;
	}

	setGeometry(config->readRectEntry("Geometry", new QRect(150, 150, 400, 500)));
}

void
KexiMainWindow::storeSettings()
{
	kdDebug() << "KexiMainWindow::storeSettings()" << endl;

	KConfig *config = KGlobal::config();
	config->setGroup("MainWindow");
	config->writeEntry("Geometry", geometry());
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
	kdDebug() << "KexiMainWindow::activeWindowChanged(): dlg = " << dlg << endl;
	factory()->addClient(dlg);
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
    KKeyDialog dlg;
    dlg.insert( actionCollection() );
    dlg.configure();
}

void
KexiMainWindow::slotConfigureToolbars()
{
    KEditToolbar edit(factory());
//    connect(&edit,SIGNAL(newToolbarConfig()),this,SLOT(slotNewToolbarConfig()));
    (void) edit.exec();
}


#include "keximainwindow.moc"

