/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>

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

#include <qptrlist.h>
#include <qapplication.h>
#include <qtimer.h>
#include <qlayout.h>

#include <klocale.h>
#include <kinstance.h>
#include <kaction.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kstdguiitem.h>
#include <kcmdlineargs.h>
#include <kparts/event.h>
#include <koMainWindow.h>
#include <kprinter.h>

#include "kexiview.h"
#include "kexisettings.h"
#include "kexiproject.h"
#include "kexitabbrowser.h"
#include "kexibrowser.h"
#include "kexibrowseritem.h"
#include "kexiworkspaceMDI.h"
#include "kexiworkspaceTabbedMDI.h"
#include "kexiworkspaceSDI.h"
#include "kexiprojectproperties.h"
#include "KexiViewIface.h"
#include "kexidbconnection.h"

#include "kexicontexthelp.h"
#include "kexi_factory.h"
#include "kexi_global.h"
#include "kexi_utils.h"

KexiView::KexiView(KexiWindowMode winmode, KexiProject *part, QWidget *parent, const char *name )
: KoView(part,parent,name ? name : "kexi_view")
	,m_browser(0)
#ifndef KEXI_NO_CTXT_HELP
	,m_helper(0)
#endif
	,m_windowMode(winmode)
	,m_lastForm(0)
	,m_project(part)
	,dcop(0)
{
	dcopObject(); // build it
	setInstance(KexiFactory::global());
	setXMLFile("kexiui.rc");

	//IMPORTANT !!!!!!!!!! This has to be in the same order as the WindowMode enum
	m_modeDescriptions<<i18n("Child frames");
	m_modeDescriptions<<i18n("Tabbed");
//	m_modeDescriptions<<i18n("Single document");
	m_possibleModes=m_modeDescriptions;
#if !KDE_IS_VERSION(3,1,9)
	m_possibleModes.remove(m_possibleModes.at(1));
#endif
	initActions();

	if(winmode != EmbeddedMode)
	{
		initMainDock();
//		QTimer::singleShot(0,this,SLOT(finalizeInit()));
	}
	else
	{
		(new QVBoxLayout (this))->setAutoAdd(true);
		m_workspace = new KexiWorkspaceSDI(this, 0, this);
	}
}

DCOPObject* KexiView::dcopObject()
{
	if ( !dcop )
		dcop = new KexiViewIface( this );

	return dcop;
}


KexiProject *KexiView::project() const
{
	return m_project;
}

void KexiView::solveDeps()
{
#if 0
	connect(kexi->project(), SIGNAL(dbAvaible()), this, SLOT(slotDBAvaible()));
#endif
}

void KexiView::updateReadWrite( bool /*readwrite*/ )
{
#ifdef __GNUC__
#warning TODO
#endif
}

/*!	This function should contain initialization done after hooking parts into view.
	It is done in KexiProject::createViewInstance() automatically. */
void KexiView::finalizeInit()
{
	if (m_windowMode != EmbeddedMode)
	{
		initBrowser();
		initHelper();
/*
		if(m_windowMode == MultipleWindowMode)
		{
			QDesktopWidget dw;
			QRect availGeom=dw.availableGeometry(this);
			move(availGeom.left(),availGeom.top());
			resize(availGeom.width()-(frameGeometry().width()-geometry().width()),
				height());
		}*/
	}
}

void KexiView::initMainDock()
{
	(new QVBoxLayout (this))->setAutoAdd(true);
	m_workspace=0;
	if (m_windowMode==MDIMode)
		m_workspace = new KexiWorkspaceMDI(this, "kexiworkspace", this);
	else if (m_windowMode==TabbedMDIMode) 
		m_workspace = new KexiWorkspaceTabbedMDI(this, "kexiworkspace", this);
	else
		kdDebug()<<"Not implemented yet";
	if (m_workspace) m_workspace->show();
}

void KexiView::initBrowser()
{
//	QDockWindow *dock = new QDockWindow(m_workspace, "Browser Dock");
//	dock->setResizeEnabled(true);
//	dock->setCloseMode(QDockWindow::Always);
//		 reparent(w,QPoint(0,0),true);
//	mainWindow()->moveDockWindow(dock, DockLeft);
//	w->setCaption(this->caption());
//	addQDockWindow(dock);

	m_browser = new KexiTabBrowser(this, 0, "Document Browser");
//	dock->setCaption(m_browser->caption());
//	dock->setWidget(m_browser);
	m_browser->show(); //TODO: read settings
//	dock->show();
	m_actionBrowser->setChecked(m_browser->isVisible());
	m_browser->plugToggleAction(m_actionBrowser);

	kdDebug() << "KexiView::initBrowser: done" << endl;
}

void KexiView::initHelper()
{
#ifndef KEXI_NO_CTXT_HELP
	m_helper=new KexiContextHelp(this, 0, "Context Help");
	//add that as we have a help!
	m_helper->setContextHelp(i18n("Welcome"), i18n("%1 is in a early state of development, not all planed features are implemented<br>you can help with kexi development by filing <a href=\"http://bugs.kde.org/wizard.cgi?package=kexi\">feature-requests and bug reports</a><br><br><i>the kexi team wishes you fun and productive work</i>").arg(KEXI_APP_NAME));
	m_actionHelper->setChecked(m_helper->isVisible());
	connect(m_actionHelper, SIGNAL(toggled(bool)), m_helper, SLOT(setVisible(bool)));
#endif
}

void KexiView::initActions()
{
	//creating a list of actions for the form-designer
	m_formActionList = new QPtrList<KAction>;

	//standard actions
	(void*) KStdAction::preferences(this, SLOT(slotSettings()), actionCollection());

//	KAction *actionProjectProps = new KAction(i18n("Project Properties"), "project_props", Key_F7,
	KAction *actionProjectProps = new KAction(i18n("Project Properties"), "edit", Key_F7,
	 actionCollection(), "project_props");
	connect(actionProjectProps, SIGNAL(activated()), this, SLOT(slotShowProjectProps()));

	KSelectAction *actionViewMode = new KSelectAction(i18n("View mode"), KShortcut(),
		actionCollection(),"view_viewmode");

	actionViewMode->setItems(m_possibleModes);
	connect(actionViewMode,SIGNAL(activated(const QString&)),this,SLOT(changeViewMode(const QString&)));

#ifndef KEXI_NO_UNFINISHED
	KAction *actionSettings = new KAction(i18n("Configure Kexi..."), "configure", 0,
	 actionCollection(), "kexi_settings");
	connect(actionSettings, SIGNAL(activated()), this, SLOT(slotShowSettings()));

	KAction *actionFileImport = new KAction(i18n("Import file based data..."), "", "",
	 actionCollection(), "kexi_importfiledata");
	connect(actionFileImport, SIGNAL(activated()), m_project, SLOT(slotImportFileData()));

	KAction *actionServerImport = new KAction(i18n("Import remote server based data..."), "", "",
	 actionCollection(), "kexi_importserverdata");
	connect(actionServerImport, SIGNAL(activated()), m_project, SLOT(slotImportServerData()));
#endif

	m_actionBrowser = new KToggleAction(i18n("Show Navigator"), "", CTRL + Key_B,
	 actionCollection(), "show_nav");

#ifndef KEXI_NO_CTXT_HELP
	m_actionHelper = new KToggleAction(i18n("Show Context Help"), "", CTRL + Key_H,
	 actionCollection(), "show_contexthelp");
#endif

	connect(m_project, SIGNAL(dbAvaible()), this, SLOT(slotDBAvaible()));
	
	//disable some actions:
#ifdef KEXI_NO_UNFINISHED
	KActionCollection *parentCollection = shell()->actionCollection();
	for (int i=0;i<(int)parentCollection->count();i++)
		kdDebug() << parentCollection->action(i)->name() << endl;
	kdDebug() << "--" << endl;
	KActionCollection *myCollection = actionCollection();
	for (int i=0;i<(int)myCollection->count();i++)
		kdDebug() << myCollection->action(i)->name() << endl;

//	parentCollection->action("file_import_file")->setEnabled(false);
#define INIT_UNF_ACT(act) \
	{ if (act) connect(act, SIGNAL(activated()), this, SLOT(slotInfoUnfinished())); }
#define INIT_UNF(act_name) \
	{ KAction *act = parentCollection->action(act_name); \
		if (act) connect(act, SIGNAL(activated()), this, SLOT(slotInfoUnfinished())); \
	}
	INIT_UNF("file_print");
	INIT_UNF("file_print_preview");
	INIT_UNF("file_import_file");
	INIT_UNF("file_export_file");
	INIT_UNF("file_send_file");
	INIT_UNF("help_contents");
//	INIT_UNF_ACT(actionProjectProps);

#undef INIT_UNF
#endif
}

void KexiView::slotInfoUnfinished()
{
#ifdef KEXI_NO_UNFINISHED
	if (sender() && sender()->isA("KAction")) {
		const KAction *act = dynamic_cast<const KAction*>(sender());
		QString txt = act->plainText();
		txt.replace( '.', QString::null );
		KEXI_UNFINISHED(txt);
	}
#endif
}

void KexiView::slotActiveWindowChanged(QWidget *w)
{
#if 0
	if(m_lastForm)
	{
		m_lastForm->unregisterActions(m_formActionList);
	}

	if(w && m_workspace->count() != 0)
	{
		if(!strcmp(w->name(),"form")==1)
		{
			QPtrListIterator<KAction> it(*m_formActionList);
			for(; it.current() != 0; ++it)
			{
				it.current()->setEnabled(true);
			}
			KexiFormBase *b = (KexiFormBase *)w;
			b->setActions(m_formActionList);
			m_lastForm = b;
		}
		else
		{
			QPtrListIterator<KAction> it(*m_formActionList);
			for(; it.current() != 0; ++it)
			{
				it.current()->setEnabled(false);
			}
		}
	}
	else
	{
		QPtrListIterator<KAction> it(*m_formActionList);
		for(; it.current() != 0; ++it)
		{
			it.current()->setEnabled(false);
		}
	}
#endif
}


void
KexiView::slotProjectModified()
{
//	m_actionSave->setEnabled(project()->modified());
//	m_actionSaveAs->setEnabled(project()->modified());
}

void
KexiView::slotDBAvaible()
{
	kdDebug() << "KexiView::slotDBAvaible()" << endl;
	m_actionRelations->setEnabled(true);
}

void
KexiView::slotShowProjectProps()
{
	KexiProjectProperties p(this, project()->dbConnection());
	if(p.exec())
	{
		project()->db()->setEncoding(p.encoding());
		project()->dbConnection()->setEncoding(p.encoding());
	}
}

void
KexiView::slotShowSettings()
{
	KexiSettings s(this);
	s.exec();
}


KexiView::~KexiView(){
	m_dockWins.setAutoDelete(true);
	m_dockWins.clear();
	delete m_formActionList;
	delete dcop;
}

void
KexiView::slotSettings()
{
}

void KexiView::addQDockWindow(QDockWindow *w) {
	m_dockWins.append(w);
}

void KexiView::removeQDockWindow(QDockWindow * w) {
	w->hide();
	m_dockWins.remove(w);
}

void KexiView::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
	if ( ev->activated() ) {
		for (QDockWindow *w=m_dockWins.first();w;w=m_dockWins.next()) {
			w->show();
		}
	}
	else
	{
		for (QDockWindow *w=m_dockWins.first();w;w=m_dockWins.next()) {
			w->hide();
		}

	}
    KoView::guiActivateEvent( ev );
}

void
KexiView::setupPrinter(KPrinter &printer)
{
#ifndef KEXI_NO_PRINT
	//FIXME: we really shouldn't do it here!!!
	printer.setPageSelection(KPrinter::ApplicationSide);
	printer.setCurrentPage(1);
	printer.setMinMax(1, 1);
	printer.setPageSize(KPrinter::A4);
	printer.setOrientation(KPrinter::Portrait);

	KexiDialogBase  *active = dynamic_cast<KexiWorkspace*>(m_workspace)->activeDocumentView();
	if (active)
		active->setupPrinter(printer);
#else
	KEXI_UNFINISHED(i18n("Setup printer"));
#endif //!KEXI_NO_PRINT
}

void
KexiView::print(KPrinter &printer)
{
#ifndef KEXI_NO_PRINT
	KexiDialogBase *active = dynamic_cast<KexiWorkspace*>(m_workspace)->activeDocumentView();
	kdDebug() << "KexiView::print: " << active << endl;

	if(active)
		active->print(printer);


/*	QPainter p(&printer);
	p.drawLine(60,60,120,120);
	p.end();
*/
#else
	KEXI_UNFINISHED(i18n("Print"));
#endif //!KEXI_NO_PRINT
}

bool
KexiView::activateWindow(const QString &id)
{
	kdDebug() << "KexiView::activateWindow()" << endl;
	KexiDialogBase *dlg = m_wins[id];
	kdDebug() << "KexiView::activateWindow(): dlg: " << dlg << endl;
	if (!dlg)
		return false;
	
	workspace()->activateView(dlg);
	return true;
}

void
KexiView::registerDialog(KexiDialogBase *dlg, const QString &identifier)
{
	m_wins.insert(identifier, dlg);
}

bool
KexiView::removeDialog(const QString &identifier)
{
	Windows::const_iterator it = m_wins.find(identifier);
	if (it==m_wins.end())
		return false;
	m_wins.remove(identifier);
	return true;
}

KexiDialogBase *
KexiView::findWindow(const QString &id)
{
	return m_wins[ id ];
}

/*! Method used to connect windows that are before closing when
	we want to do something in this situation.
*/
void KexiView::slotAboutCloseWindow( KexiDialogBase *w )
{
	//UNUSED
}


void KexiView::changeViewMode(const QString& newMode) {
	kdDebug()<<"KexiView::changeViewMode"<<endl;
	m_windowMode=(KexiWindowMode) m_modeDescriptions.findIndex(newMode);

	delete m_workspace;
	initMainDock();
}

#include "kexiview.moc"
