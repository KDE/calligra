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
	setStandardMDIMenuEnabled();
	setXMLFile("kexiui.rc");
	initActions();
	createGUI(0);
	restoreSettings();

	initBrowser();
	connect(m_project, SIGNAL(dbAvailable()), this, SLOT(initBrowser()));

	QTimer::singleShot(0, this, SLOT(parseCmdLineOptions()));
}

void
KexiMainWindow::initActions()
{
	new KAction(i18n("&New"), "filenew", KStdAccel::shortcut(KStdAccel::New), this, SLOT(fileNew()), actionCollection(), "file_new");
	new KAction(i18n("&Open"), "fileopen", KStdAccel::shortcut(KStdAccel::Open), this, SLOT(fileNew()), actionCollection(), "file_open");
	new KAction(i18n("&Save"), "filesave", KStdAccel::shortcut(KStdAccel::Save), this, SLOT(fileNew()), actionCollection(), "file_save");
	new KAction(i18n("Save &As"), "filesaveas", 0, this, SLOT(fileNew()), actionCollection(), "file_save_as");
	new KAction(i18n("&Quit"), "exit", KStdAccel::shortcut(KStdAccel::Quit), qApp, SLOT(quit()), actionCollection(), "file_quit");
}

void
KexiMainWindow::initBrowser()
{
	kdDebug() << "KexiMainWindow::initBrowser()" << endl;

	if(!m_browser)
	{
		m_browser = new KexiBrowser(this, "kexi/db", 0);
		addToolWindow(m_browser, KDockWidget::DockLeft, m_pMdi, 20/*, lv, 35, "2"*/);
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

KexiMainWindow::~KexiMainWindow()
{
}

#include "keximainwindow.moc"

