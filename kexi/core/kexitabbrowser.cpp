/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>

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

#include <koApplication.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <klistview.h>
#include <klocale.h>
#include "kmultitabbar.h"

#include <qtabwidget.h>
#include <qlayout.h>
#include <qptrlist.h>
#include <qwidgetstack.h>

#include "kexiDB/kexidb.h"

#include "kexibrowser.h"
#include "kexibrowseritem.h"
#include "kexitabbrowser.h"
#include "kexiproject.h"
#include "kexiprojecthandler.h"
#include <kexiview.h>

KexiTabBrowser::KexiTabBrowser(KexiView *view,QWidget *parent, const char *name)
	: KexiDialogBase(view,parent,name),
		m_project( view->project() ),
		m_tabBar( new KMultiTabBar(this, KMultiTabBar::Vertical) ),
		m_stack( new QWidgetStack(this) ),
		m_tabs( 0 ),
		m_activeTab( -1 ),
		m_db( new KexiBrowser(m_stack, "kexi/db", 0) )
{
	setCaption(i18n("Project"));

	QGridLayout *layout=new QGridLayout(this);
	m_tabBar->setPosition(KMultiTabBar::Left);
	m_tabBar->showActiveTabTexts(true);

	layout->addWidget(m_tabBar,     0,      0);
	layout->addWidget(m_stack,      0,      1);
	layout->setColStretch(1, 1);
	
//	m_stack->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum); //(JS)

	m_stack->show();
	m_tabBar->show();

	addBrowser(m_db, kapp->iconLoader()->loadIcon("db", KIcon::Small), i18n("Database"));

/*	m_db = new KexiBrowser(view,m_stack, KexiBrowser::SectionDB);
	m_tables = new KexiBrowser(view,m_stack, KexiBrowser::SectionTable);
	m_forms = new KexiBrowser(view,m_stack, KexiBrowser::SectionForm);
	m_queries = new KexiBrowser(view,m_stack, KexiBrowser::SectionQuery);
	m_reports = new KexiBrowser(view,m_stack, KexiBrowser::SectionReport);
*/

/*	addBrowser(m_db, "db",i18n("Database project"));
	addBrowser(m_tables, "tables",i18n("Tables"));
	addBrowser(m_forms, "forms",i18n("Forms"));
	addBrowser(m_queries, "queries",i18n("Queries"));
	addBrowser(m_reports, "reports",i18n("Reports"));
*/
//	layout->addWidget(m_tabBar);
//	layout->addWidget(m_stack);
	
	
//	view->mainWindow()->moveDockWindow(this, DockLeft);

	slotUpdateBrowsers();
	connect(kexiProject(),SIGNAL(partListUpdated()),this,SLOT(slotUpdateBrowsers()));

	registerAs(KexiDialogBase::ToolWindow);
}

void
KexiTabBrowser::addBrowser(KexiBrowser *browser, QPixmap icon, QString text)
{
	m_tabs++;
//	m_tabBar->appendTab(kapp->iconLoader()->loadIcon(icon, KIcon::Small), m_tabs,text);
	m_tabBar->appendTab(icon, m_tabs,text);

//	browser->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum); //(JS)

	connect(m_tabBar->getTab(m_tabs), SIGNAL(clicked(int)), this, SLOT(slotTabActivated(int)));
	m_stack->addWidget(browser);
	m_browserDict.insert(m_tabs, browser);

	if(m_activeTab == -1)
	{
		m_tabBar->setTab(m_tabs, true);
		m_activeTab = m_tabs;
		m_stack->raiseWidget(browser);
	}
}

void
KexiTabBrowser::slotTabActivated(int id)
{
	if(m_activeTab != id)
	{
		m_tabBar->setTab(m_activeTab, false);
		m_activeTab = id;
		m_stack->raiseWidget(m_browserDict[id]);
	}
	else
	{
		m_tabBar->setTab(id, true);
	}
}

void
KexiTabBrowser::slotUpdateBrowsers()
{
	kdDebug() << "KexiTabBrowser::slotUpdateBrowsers()" << endl;
	PartList *plist = m_project->getParts();
	kdDebug() << "KexiTabBrowser::slotUpdateBrowsers() found " << plist->count() << " items" << endl;

	
	
	for(KexiProjectHandler *part = plist->first(); part; part = plist->next())
	{
		kdDebug() << "KexiTabBrowser::slotUpdateBrowsers(): showing " << part->name() << endl;
		
		if(part->visible())
		{
			m_db->addGroup(part);
#ifndef KEXI_NO_MULTI_TABS
			addBrowser(new KexiBrowser(m_stack, part->mime(), part), part->groupPixmap(), part->name());
#endif
			kdDebug() << "KexiTabBrowser::slotUpdateBrowsers(): added " << part->name() << endl;
			// Update the databasetab
			connect(part, SIGNAL(itemListChanged(KexiProjectHandler*)), m_db, SLOT(slotItemListChanged(KexiProjectHandler*)));
		}
	}
	m_db->sort();
}

KexiTabBrowser::~KexiTabBrowser()
{
}


KXMLGUIClient *KexiTabBrowser::guiClient()
{
	kdDebug()<<"KexiTabBrowser::guiClient() not implemented yet"<<endl;
	return new KXMLGUIClient();
}

void KexiTabBrowser::resizeEvent( QResizeEvent *e )
{
}


#include "kexitabbrowser.moc"
