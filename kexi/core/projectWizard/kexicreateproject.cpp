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
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <klocale.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kwizard.h>
#include <qstringlist.h>
#include <klibloader.h>

#include "kexicreateprojectpage.h"
#include "kexicreateprojectpageengine.h"
#include "kexicreateprojectpagelocation.h"
#include "kexicreateprojectpageauth.h"
#include "kexicreateprojectpagedb.h"
#include "kexicreateprojectpagefile.h"
#include <kexiprojecthandler.h>

#include <kgenericfactory.h>

//#include "kexitabbrowser.h"
#include "kexicreateproject.h"
//#include "kexiview.h"
//#include "kexiproject.h"

KexiCreateProject::KexiCreateProject(QObject *project,const char* name, const QStringList &) : KWizard(0,name,true), KexiCreateProjectIface()
{
	m_project=KEXIPROJECT(project);
	setCaption(i18n("Create Project"));

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

	if(page && data == "finish")
	{
		setFinishEnabled(page, page->data("finish").toBool());
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
		
		static_cast<KexiCreateProjectPageDB*>(m_pageDatabase)->connectHost(engine, host, user, pass, socket,
			port, savePass);
	}

	KWizard::next();
}

void
KexiCreateProject::accept()
{
	if(m_pageEngine->data("location").toString() != "RemoteDB")
	{
		kdDebug() << "KexiCreateProject::accept(): local engine..." << endl;
		project()->initFileConnection(m_pageEngine->data("engine").toString(), QString::null);
	}
	else
	{
		static_cast<KexiCreateProjectPageDB*>(m_pageDatabase)->connectDB();
	}
	KWizard::accept();
}

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

KexiCreateProject::~KexiCreateProject()
{
}

extern "C" {
	void * init_kexiprojectwizard() {return new KexiCreateProjectFactory();}
}

/*
K_EXPORT_COMPONENT_FACTORY( kexiprojectwizard, KGenericFactory<KexiCreateProject>)
*/

#include "kexicreateproject.moc"
