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

#include "kexicreateprojectpage.h"
#include "kexicreateprojectpageengine.h"
#include "kexicreateprojectpageconnection.h"
#include "kexicreateprojectpagedb.h"
#include "kexicreateprojectpagefile.h"

#include "kexicreateproject.h"

KexiCreateProject::KexiCreateProject(QWidget *parent, const char *name, bool modal, WFlags f) : KWizard(parent,name,modal,f)
{
	setCaption(i18n("Create Project"));

	m_wpic = new QPixmap(locate("data","kexi/pics/cp-wiz.png"));

	m_pageEngine = new KexiCreateProjectPageEngine(this, m_wpic, "page_engine");
	addItem(m_pageEngine, i18n("Engine"));
	m_pageConnection = new KexiCreateProjectPageConnection(this, m_wpic, "page_connection");
	addItem(m_pageConnection, i18n("Connection"));
	m_pageDatabase = new KexiCreateProjectPageDB(this, m_wpic, "page_db");
	m_pageDatabase->hide();
	m_pageFile = new KexiCreateProjectPageFile(this, m_wpic, "page_file");
	m_pageFile->hide();
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
}

void
KexiCreateProject::slotValueChanged(KexiCreateProjectPage *page, QString &data)
{
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
			KWizard::next();
		}
	}
	else if(currentPage() == m_pageConnection)
	{
		kdDebug() << "KexiCreateProject::next(): time to connect..." << endl;
		QString engine = m_pageEngine->data("engine").toString();
		QString host = m_pageConnection->data("host").toString();
		QString user = m_pageConnection->data("user").toString();
		QString pass = m_pageConnection->data("password").toString();
		
		static_cast<KexiCreateProjectPageDB*>(m_pageDatabase)->connectHost(engine, host, user, pass);
	}
	KWizard::next();
}

void
KexiCreateProject::accept()
{
	if(static_cast<KexiCreateProjectPageDB*>(m_pageDatabase)->connectDB())
		KWizard::accept();
}

void
KexiCreateProject::requireSection(QString section)
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

#include "kexicreateproject.moc"
