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

#ifndef KEXI_NO_PRINT
#include <kprinter.h>
#endif

#include "kexiview.h"
#include "kexisettings.h"
#include "kexiproject.h"
#include "kexitabbrowser.h"
#include "kexibrowser.h"
#include "kexibrowseritem.h"
#include "kexiworkspaceMDI.h"
#include "kexiworkspaceSDI.h"
#include "kexiprojectproperties.h"
#include "KexiViewIface.h"
#include "kexidbconnection.h"

#include "kexicontexthelp.h"
#include "kexi_factory.h"
#include "kexi_global.h"

KexiView::KexiView(KexiWindowMode winmode, KexiProject *part,QWidget *parent, const char *name ) : KoView(part,parent,name)
{
	m_project=part;
	m_windowMode=winmode;

	dcop = 0;
	m_browser = 0;
#ifndef KEXI_NO_CTXT_HELP
	m_help = 0;
#endif
	m_lastForm = NULL;
	dcopObject(); // build it
//	createGUI("kexiui.rc",false);

	setInstance(KexiFactory::global());
	setXMLFile("kexiui.rc");
	initActions();

	if(winmode != EmbeddedMode)
	{
		initMainDock();
		QTimer::singleShot(0,this,SLOT(finalizeInit()));
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


void KexiView::finalizeInit()
{
	initDocBrowser();
	initHelper();

	if(m_windowMode == MultipleWindowMode)
	{
		QDesktopWidget dw;
		QRect availGeom=dw.availableGeometry(this);
		move(availGeom.left(),availGeom.top());
		resize(availGeom.width()-(frameGeometry().width()-geometry().width()),
			height());
	}
}

void KexiView::initMainDock()
{
	(new QVBoxLayout (this))->setAutoAdd(true);
	m_workspace = new KexiWorkspaceMDI(this, "kexiworkspace", this);
}

void KexiView::initDocBrowser()
{
	m_browser = new KexiTabBrowser(this, m_workspace, "Document Browser");
	m_browser->show(); //remove later
	kdDebug() << "KexiView::initDocBrowser: done" << endl;
}

void KexiView::initHelper(bool h)
{
#ifndef KEXI_NO_CTXT_HELP
	if(h)
	{
		m_help=new KexiContextHelp(this,m_actionHelper,m_workspace, "Context Help");
//add that as we have a help!
//		m_help->setContextHelp(i18n("Welcome"), i18n("kexi is based on <a href=\"help://kexi#glossary-relationaldatabase\">relational databases</a>. Before you start creating tables you should think about the general database design.<br><br>Further readings:<br><ul><li><a href=\"help://kexi#databasedesign\">Relational Database Design</a></li><li><a href=\"help://kexi#entityrelationship\">The entity relationship model</a></li></ul>"));
		m_help->setContextHelp(i18n("Welcome"), i18n("%1 is in a early state of development, not all planed features are implemented<br>you can help with kexi development by filing <a href=\"http://bugs.kde.org/wizard.cgi?package=kexi\">feature-requests and bug reports</a><br><br><i>the kexi team wishes you fun and productive work</i>").arg(KEXI_APP_NAME));

	}
	else
	{
		if(m_help)
			delete m_help;
	}
#endif
}

void KexiView::initActions()
{
	//creating a list of actions for the form-designer
	m_formActionList = new QPtrList<KAction>;

	//standard actions
	(void*) KStdAction::preferences(this, SLOT(slotSettings()), actionCollection());

	KAction *actionProjectProps = new KAction(i18n("Project Properties"), "project_props", Key_F7,
	 actionCollection(), "project_props");
	connect(actionProjectProps, SIGNAL(activated()), this, SLOT(slotShowProjectProps()));

	KAction *actionSettings = new KAction(i18n("Configure Kexi..."), "", 0,
	 actionCollection(), "kexi_settings");
	connect(actionSettings, SIGNAL(activated()), this, SLOT(slotShowSettings()));

	KAction *actionImport = new KAction(i18n("Import Data..."), "", 0,
	 actionCollection(), "kexi_importdata");
	connect(actionImport, SIGNAL(activated()), m_project, SLOT(slotImportData()));


	KToggleAction *actionNav = new KToggleAction(i18n("Show Navigator"), "", CTRL + Key_B,
	 actionCollection(), "show_nav");

#ifndef KEXI_NO_CTXT_HELP
	m_actionHelper = new KToggleAction(i18n("Show Context Help"), "", CTRL + Key_H,
	 actionCollection(), "show_contexthelp");
	m_actionHelper->setChecked(true);
	connect(m_actionHelper, SIGNAL(toggled(bool)), this, SLOT(initHelper(bool)));
#endif

	connect(m_project, SIGNAL(dbAvaible()), this, SLOT(slotDBAvaible()));
	
	//TODO: disable some actions:
//	KAction *act = actionCollection()->action("file_import_file");
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
	KexiProjectProperties *p = new KexiProjectProperties(this, project()->dbConnection());
	if(p->exec())
	{
		project()->db()->setEncoding(p->encoding());
		project()->dbConnection()->setEncoding(p->encoding());
	}

	delete p;
}

void
KexiView::slotShowSettings()
{
	KexiSettings *s = new KexiSettings(this);
	s->exec();
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

/*
void
KexiView::slotShowRelations()
{
	KexiRelationDialog *r = new KexiRelationDialog(this,0, "relations");
	r->show();
}
*/

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

#ifndef KEXI_NO_PRINT
void
KexiView::setupPrinter(KPrinter &printer)
{
	//FIXME: we really shouldn't do it here!!!
	printer.setPageSelection(KPrinter::ApplicationSide);
	printer.setCurrentPage(1);
	printer.setMinMax(1, 1);
	printer.setPageSize(KPrinter::A4);
	printer.setOrientation(KPrinter::Portrait);

	KexiDialogBase  *active = dynamic_cast<KexiWorkspace*>(m_workspace)->activeDocumentView();
	if (active)
		active->setupPrinter(printer);
}

void
KexiView::print(KPrinter &printer)
{

	KexiDialogBase *active = dynamic_cast<KexiWorkspace*>(m_workspace)->activeDocumentView();
	kdDebug() << "KexiView::print: " << active << endl;

	if(active)
		active->print(printer);


/*	QPainter p(&printer);
	p.drawLine(60,60,120,120);
	p.end();
*/
}
#endif //!KEXI_NO_PRINT

bool
KexiView::activateWindow(const QString &id)
{
	kdDebug() << "KexiView::activateWindow()" << endl;
	KexiDialogBase *dlg = m_wins[id];
	kdDebug() << "KexiView::activateWindow(): dlg: " << dlg << endl;
	if(dlg)
	{
		workspace()->activateView(dlg);
		return true;
	}

	return false;
}

void
KexiView::registerDialog(KexiDialogBase *dlg, const QString &identifier)
{
	m_wins.insert(identifier, dlg);
}

void
KexiView::removeDialog(const QString &identifier)
{
//	if(m_wins)
	m_wins.remove(identifier);
}

#include "kexiview.moc"
