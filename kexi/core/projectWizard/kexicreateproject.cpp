/***************************************************************************
                          kexicreateproject.cpp  -  description
                             -------------------
    begin                : Sun Jun 9 2002
    copyright            : (C) 2002 by lucijan busch
    email                : lucijan@gmx.at
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License, or (at your option) any later version.                   *
 *                                                                         *
 ***************************************************************************/

#include <klocale.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kwizard.h>
#include <qstringlist.h>
#include <klibloader.h>
#include <qpushbutton.h>

#include "kexicreateprojectpage.h"
#include "kexicreateprojectpageengine.h"
#include "kexicreateprojectpagelocation.h"
#include "kexicreateprojectpageauth.h"
#include "kexicreateprojectpagedb.h"
#include "kexicreateprojectpagefile.h"
#include <kexiprojecthandler.h>
#include <kgenericfactory.h>

#include <kapplication.h>
#include <kiconloader.h>

//#include "kexitabbrowser.h"
#include "kexicreateproject.h"
#include "kexidbconnection.h"
//#include "kexiview.h"
//#include "kexiproject.h"


KexiCreateProject::KexiCreateProject(QObject *project,const char* name, const QStringList &) : KWizard(0,name,true), KexiCreateProjectIface()
{
	m_project=KEXIPROJECT(project);
	setCaption(i18n("Create Project"));
	setIcon(kapp->iconLoader()->loadIcon("filenew", KIcon::Toolbar) );
	m_wpic = new QPixmap(locate("data","kexi/pics/cp-wiz.png"));
	m_pageEngine = new KexiCreateProjectPageEngine(this, m_wpic, "page_engine");
	addItem(m_pageEngine, i18n("Engine"));
	m_pageLocation = new KexiCreateProjectPageLocation(this, m_wpic, "page_location");
	addItem(m_pageLocation, i18n("Location"));
	m_pageAuth = new KexiCreateProjectPageAuth(this, m_wpic, "page_auth");
	addItem(m_pageAuth, i18n("Authentication"));
	m_pageDatabase = new KexiCreateProjectPageDB(this, m_wpic, "page_db");
	m_pageDatabase->hide();
	m_pageFile = new KexiCreateProjectPageFile(this, m_wpic, "page_file");
	m_pageFile->hide();
}

KexiProject *KexiCreateProject::project()const
{
    return m_project;
}

/*! adds the page to the pagelist
 *  which enables showing on demand
*/
void
KexiCreateProject::registerPage(KexiCreateProjectPage *page)
{
	m_pageList.append(page);
}

void
KexiCreateProject::addItem(KexiCreateProjectPage *page, QString title, int index)
{
	insertPage(page, title, index);
	connect(page, SIGNAL(valueChanged(KexiCreateProjectPage*, QString &)), this,
	   SLOT(slotValueChanged(KexiCreateProjectPage*, QString &)));
	connect(page, SIGNAL(acceptPage()), this, SLOT(slotPageAccepted()));
	page->m_loaded = true;

	if(page == m_pageFile)
	{
		kdDebug() << "KexiCreateProject::addItem(): finish" << endl;
		setFinishEnabled(page, true);
	}
}

void
KexiCreateProject::slotValueChanged(KexiCreateProjectPage *page, QString &data)
{
	kdDebug() << "KexiCreateProject::slotValueChanged() data: " << data << endl;

	if (!page)
		return;
	if(data == "finish") {
		setFinishEnabled(page, page->data("finish").toBool());
	}
	else if (data == "continue") {
		setNextEnabled(page, page->data("continue").toBool());
	}
}

/*! Page signals that we should accept it. If we are at the last page and Finish buton
	is enabled, we accpet our wizard dialog with accept(). Otherwise we try to
	move to next page if exists.
*/
void
KexiCreateProject::slotPageAccepted()
{
	if (finishButton()->isEnabled()) {
		accept();
	}
	else if (nextButton()->isEnabled()) {
		next();
	}
}

void
KexiCreateProject::next()
{
	kdDebug() << "KexiCreateProject::next()" << endl;
	if(currentPage() == m_pageEngine)
	{
		if(m_pageEngine->data("location").toString() == "RemoteDB")
		{
			requireSection("RemoteDB");
			KWizard::next();
			return;
		}
		else
		{
			requireSection("LocalDB");
			removePage(m_pageAuth);
			KWizard::next();
		}
	}
	else if(currentPage() == m_pageAuth)
	{
		kdDebug() << "KexiCreateProject::next(): time to connect..." << endl;
		QString engine = m_pageEngine->data("engine").toString();
		QString host = m_pageLocation->data("host").toString();
		QString port = m_pageLocation->data("port").toString();
		QString socket = m_pageLocation->data("socket").toString();
		QString user = m_pageAuth->data("user").toString();
		QString pass = m_pageAuth->data("password").toString();
		bool savePass = m_pageAuth->data("savePassword").toBool();

		kdDebug() << "User = " << user << endl;
		kdDebug() << "Socket = " << socket << " Port = " << port << endl;

		if (!static_cast<KexiCreateProjectPageDB*>(m_pageDatabase)
				->connectHost(engine, host, user, pass, socket, port, savePass)) {
			return;
		}
	}

	KWizard::next();
}

void
KexiCreateProject::accept()
{
	if(m_pageEngine->data("location").toString() != "RemoteDB")
	{
		kdDebug() << "KexiCreateProject::accept(): local engine..." << endl;
		//FIXME: care if user selects an existing location :)
		KexiDBConnection *c;
		if(m_pageFile->data("persistant").toBool())
		{
			c = new KexiDBConnection(m_pageEngine->data("engine").toString(),
			 m_pageFile->data("ref").toString(), true);
		}
		else
		{
			c = new KexiDBConnection(m_pageEngine->data("engine").toString(),
			 QString::null);
		}

		if(!project()->initDBConnection(c))
			return;
	}
	else
	{
		if(!static_cast<KexiCreateProjectPageDB*>(m_pageDatabase)->connectDB())
			return;
	}
	KWizard::accept();
}

/*! adds pages, needed for a section
 *  and removes pages, which are'n needed as well,
 *  note: it requeries that the pages are added in the right order)
 */
void
KexiCreateProject::requireSection(const QString &section)
{
	if(section == m_currentSection)
	{
		return;
	}
	else
	{
		for(KexiCreateProjectPage *page = m_pageList.first(); page; page = m_pageList.next())
		{
			if(page->data("section").toString() == m_currentSection)
			{
				//unload it in that case...
				removePage(page);
				kdDebug() << "KexiCreateProject::requireSection(): took page: " << page->data("caption").toString() << endl;
			}
			else if(page->data("section").toString() == section)
			{
				//we will need it...
				if(!page->m_loaded)
				{
					addItem(page, page->data("caption").toString());
					kdDebug() << "KexiCreateProject::requireSection(): added page: " << page->data("caption").toString() << endl;
					page->show();
				}
				else
				{
					page->show();
					kdDebug() << "KexiCreateProject::requireSection(): ignored: " << page->data("caption").toString() << endl;
				}
			}
		}
	}
}

QString KexiCreateProject::projectFileName()
{
#ifdef __GNUC__
#warning "TODO"
#endif
	return QString("/home/jowenn/kexidb");
}

KexiCreateProject::~KexiCreateProject()
{
}

/*extern "C" {
	void * init_kexiprojectwizard() {return new KexiCreateProjectFactory();}
}*/

K_EXPORT_COMPONENT_FACTORY( kexiprojectwizard, KexiCreateProjectFactory );

/*
K_EXPORT_COMPONENT_FACTORY( kexiprojectwizard, KGenericFactory<KexiCreateProject>)
*/

#include "kexicreateproject.moc"
