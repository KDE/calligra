/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
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
#include <qwidgetstack.h>

#include "kexiDB/kexidb.h"

#include "kexibrowser.h"
#include "kexibrowseritem.h"
#include "kexitabbrowser.h"
#include "kexiproject.h"
#include <kexiview.h>

KexiTabBrowser::KexiTabBrowser(KexiView *view,QWidget *parent, const char *name)
	: QDockWindow(view->mainWindow(), name)
{
	setCaption(i18n("Project"));
	m_project = view->project();
	
	kdDebug() << "KexiTabBrowser::KexiTabBrowser()" << endl;
	//QBoxLayout *layout = boxLayout();  that one changes the orientation dynamically :(

	setResizeEnabled(true);

	QWidget *box=new QWidget(this);
	QGridLayout *layout=new QGridLayout(box);
	m_tabBar = new KMultiTabBar(box, KMultiTabBar::Vertical);
	m_tabBar->setPosition(KMultiTabBar::Left);
	m_tabBar->showActiveTabTexts(true);

	m_stack = new QWidgetStack(box);

        layout->addWidget(m_tabBar,     0,      0);
        layout->addWidget(m_stack,      0,      1);
        layout->setColStretch(1, 1);
	box->show();
	m_stack->show();
	m_tabBar->show();
	setWidget(box);


	m_activeTab = -1;

	m_db = new KexiBrowser(view,m_stack, KexiBrowser::SectionDB);
	m_tables = new KexiBrowser(view,m_stack, KexiBrowser::SectionTable);
	m_forms = new KexiBrowser(view,m_stack, KexiBrowser::SectionForm);
	m_queries = new KexiBrowser(view,m_stack, KexiBrowser::SectionQuery);
	m_reports = new KexiBrowser(view,m_stack, KexiBrowser::SectionReport);

	addBrowser(m_db, "db",i18n("Database project"));
	addBrowser(m_tables, "tables",i18n("Tables"));
	addBrowser(m_forms, "forms",i18n("Forms"));
	addBrowser(m_queries, "queries",i18n("Queries"));
	addBrowser(m_reports, "reports",i18n("Reports"));

//	layout->addWidget(m_tabBar);
//	layout->addWidget(m_stack);
	
	
	view->mainWindow()->moveDockWindow(this, DockLeft);

	connect(kexiProject(),SIGNAL(updateBrowsers()),this,SLOT(generateView()));
	kdDebug() << "KexiTabBrowser::KexiTabBrowser(): connecting to " << kexiProject() << endl;

	if(kexiProject()->dbIsAvaible())
		generateView();
}

void
KexiTabBrowser::addBrowser(KexiBrowser *browser, QString icon, QString text)
{
	m_tabs++;
	m_tabBar->appendTab(kapp->iconLoader()->loadIcon(icon, KIcon::Small), m_tabs,text);

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
KexiTabBrowser::generateView()
{
	kdDebug() << "KexiTabBrowser::generateView()" << endl;
	m_db->generateView();

	generateTables();
}

void
KexiTabBrowser::generateTables()
{
	QStringList tables = kexiProject()->db()->tables();

	for ( QStringList::Iterator it = tables.begin(); it != tables.end(); ++it )
	{
		KexiBrowserItem *item = new KexiBrowserItem(KexiBrowserItem::Child, KexiBrowserItem::Table, m_tables, (*it) );
		item->setPixmap(0, kapp->iconLoader()->loadIcon("table", KIcon::Small));
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

KexiTabBrowser::~KexiTabBrowser()
{
}


KXMLGUIClient *KexiTabBrowser::guiClient()
{
	kdDebug()<<"KexiTabBrowser::guiClient() not implemented yet"<<endl;
	return new KXMLGUIClient();
}

#include "kexitabbrowser.moc"
