/***************************************************************************
                          kexiview.cpp  -  description
                             -------------------
    begin                : Sun Jun 9 2002
    copyright            : (C) 2002 by lucijan busch
			   (C) 2002 by Joseph Wenninger <jowenn@kde.org>
    email                : lucijan@gmx.at
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qsqlquery.h>
#include <qsqldatabase.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qapplication.h>
#include <qglobal.h>
#include <qtimer.h>
#include <qlayout.h>

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

#include "kexiview.h"
#include "kexiproject.h"
#include "kexitabbrowser.h"
#include "kexibrowser.h"
#include "kexibrowseritem.h"
#include "kexiworkspaceMDI.h"
#include "kexiworkspaceSDI.h"
#include "kexicreateproject.h"
#include "kexiformbase.h"
#include "kexiprojectproperties.h"
#include "kexiproject.h"
#include "kexirelation.h"
#include "KexiViewIface.h"

KexiView::KexiView(KexiWindowMode winmode, KexiProject *part,QWidget *parent, const char *name ) : KoView(part,parent,name)
{
	m_project=part;
	m_windowMode=winmode;
	initActions();
        dcop = 0;
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

	// Open file given as commandline argument
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

}

void KexiView::initMainDock()
{
	(new QVBoxLayout (this))->setAutoAdd(true);
	m_workspace = new KexiWorkspaceMDI(this,"kexiworkspace",this);

#if 0
	m_mainDock = createDockWidget(i18n("no document"), 0, 0L, "");

	if (m_windowMode==MDIWindowMode)
	m_workspace = new KexiWorkspaceMDI(m_mainDock,"kexiworkspace",this);
	else
	m_workspace = new KexiWorkspaceSDI(m_mainDock,"kexiworkspace",this);
//	connect(m_workspace, SIGNAL(windowActivated(QWidget *)), this, SLOT(slotActiveWindowChanged(QWidget *)));
	m_mainDock->setWidget(m_workspace);
	if (m_windowMode == MultipleWindowMode)
		m_mainDock->setDockSite(KDockWidget::DockNone);
	else
	if (m_windowMode == MDIWindowMode)
		m_mainDock->setDockSite(KDockWidget::DockCorner);
	else
		m_mainDock->setDockSite(KDockWidget::DockTop);
	m_mainDock->setEnableDocking(KDockWidget::DockNone);
	setView(m_mainDock);
	setMainDockWidget(m_mainDock);
#endif
}

void KexiView::initDocBrowser()
{
//	KDockWidget* dockBrowser;
//	dockBrowser = createDockWidget(i18n("Database Browser"), 0, 0L, i18n("db"));
	m_browser = new KexiTabBrowser(this,m_workspace,"Document Browser");//dockBrowser);
	m_browser->show(); //remove later
//	dockBrowser->setWidget(m_browser);
//	dockBrowser->manualDock(getMainDockWidget(), KDockWidget::DockLeft);
//	dockBrowser->toDesktop();
	kdDebug() << "KexiView::initDocBrowser: done" << endl;

//	KexiTabBrowser *tab = new KexiTabBrowser(0, 0);
//	tab->show();
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

	m_actionRelations = new KAction(i18n("Table Relations"), "relation", Key_F8, actionCollection(), "relations");
	connect(m_actionRelations, SIGNAL(activated()), this, SLOT(slotShowRelations()));

	m_actionRelations->setEnabled(false);

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
	p->show();
}

KexiView::~KexiView(){
    delete m_formActionList;
    delete dcop;
}

void
KexiView::slotSettings()
{
}


void
KexiView::slotShowRelations()
{
	KexiRelation *r = new KexiRelation(this,workspaceWidget(), "relations");
	r->show();
}

#include "kexiview.moc"
