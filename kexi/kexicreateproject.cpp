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
	addItem(m_pageDatabase, i18n("Database"));
}

void
KexiCreateProject::addItem(KexiCreateProjectPage *page, QString title)
{
	addPage(page, title);
	connect(page, SIGNAL(valueChanged(KexiCreateProjectPage*, QString &)), this,
	   SLOT(slotValueChanged(KexiCreateProjectPage*, QString &)));
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
	if(currentPage() == m_pageConnection)
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
	static_cast<KexiCreateProjectPageDB*>(m_pageDatabase)->connectDB();
	KWizard::accept();
}

KexiCreateProject::~KexiCreateProject()
{
}

#include "kexicreateproject.moc"
