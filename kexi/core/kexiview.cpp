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

#include <qsqlquery.h>
#include <qsqldatabase.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qapplication.h>
#include <qglobal.h>
#include <qtimer.h>
#include <qlayout.h>
#include <qpainter.h>

#include <klocale.h>
#include <kinstance.h>
#include <kaction.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>
#include <kkeydialog.h>
#include <kedittoolbar.h>
#include <kcmdlineargs.h>
#include <kparts/event.h>
#include <kocontexthelp.h>
#include <kprinter.h>

#include "kexiview.h"
#include "kexiproject.h"
#include "kexitabbrowser.h"
#include "kexibrowser.h"
#include "kexibrowseritem.h"
#include "kexiworkspaceMDI.h"
//#include "kexicreateproject.h"
#include "kexiprojectproperties.h"
#include "KexiViewIface.h"

#include "kexicontexthelp.h"

KexiView::KexiView(KexiWindowMode winmode, KexiProject *part,QWidget *parent, const char *name ) : KoView(part,parent,name)
{
	m_project=part;
	m_windowMode=winmode;
	initActions();
	dcop = 0;
	m_browser = 0;
	dcopObject(); // build it
//	createGUI("kexiui.rc",false);
	setXMLFile("kexiui.rc");

	initMainDock();
//	setAutoSaveSettings();

	m_lastForm = NULL;
	QTimer::singleShot(0,this,SLOT(finalizeInit()));
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

#if QT_VERSION >= 0x030100
	if(m_windowMode == MultipleWindowMode)
	{
		QDesktopWidget dw;
		QRect availGeom=dw.availableGeometry(this);
		move(availGeom.left(),availGeom.top());
		resize(availGeom.width()-(frameGeometry().width()-geometry().width()),
			height());
	}
#endif
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

void KexiView::initHelper()
{
	m_help=new KexiContextHelp(this,m_workspace,"Context Help");

/*	m_help = new KoContextHelpDocker(mainWindow());
        m_help->setResizeEnabled(true);
        m_help->setCloseMode(QDockWindow::Always);
//	mainWindow()->moveDockWindow(help,DockLeft);
	mainWindow()->moveDockWindow(m_help, DockLeft);*/
	m_help->setContextHelp(i18n("Welcome"), i18n("kexi is based on <a href=\"help://kexi#glossary-relationaldatabase\">relational databases</a>. Before you start creating tables you should think about the general database design.<br><br>Further readings:<br><ul><li><a href=\"help://kexi#databasedesign\">Relational Database Design</a></li><li><a href=\"help://kexi#entityrelationship\">The entity relationship model</a></li></ul>"));
}

void KexiView::initActions()
{
	//creating a list of actions for the form-designer
	m_formActionList = new QPtrList<KAction>;

	//standard actions
	(void*) KStdAction::preferences(this, SLOT(slotSettings()), actionCollection());

//	setStandardToolBarMenuEnabled( true );

	KAction *actionProjectProps = new KAction(i18n("Project Properties"), "project_props", Key_F7, actionCollection(), "project_props");
	connect(actionProjectProps, SIGNAL(activated()), this, SLOT(slotShowProjectProps()));

//	m_actionRelations = new KAction(i18n("Table Relations"), "relation", Key_F8, actionCollection(), "relations");
//	connect(m_actionRelations, SIGNAL(activated()), this, SLOT(slotShowRelations()));
	connect(m_project, SIGNAL(dbAvaible()), this, SLOT(slotDBAvaible()));

//	m_actionRelations->setEnabled(m_project->dbIsAvaible());

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
	KexiProjectProperties *p = new KexiProjectProperties(this);
	p->exec();
	delete p;
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
	//FIXME: we really shouldn't do it here!!!
	printer.setPageSelection(KPrinter::ApplicationSide);
	printer.setCurrentPage(1);
	printer.setMinMax(1, 1);
	printer.setPageSize(KPrinter::A4);
	printer.setOrientation(KPrinter::Portrait);
}

void
KexiView::print(KPrinter &printer)
{
	//FIXME: IT IS GETTING REALLY; REALLY HACKY!!!!
	//kill me. lucijan

	KexiDialogBase *active = static_cast<KexiWorkspaceMDI *>(m_workspace)->activeDocumentView();
	kdDebug() << "KexiView::print: " << active << endl;

	if(active)
		active->print(printer);
/*	QPainter p(&printer);
	p.drawLine(60,60,120,120);
	p.end();
*/
}

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
