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

#include <qlayout.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qdict.h>
#include <qstringlist.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>

#include <klocale.h>
#include <kdebug.h>
#include <kcombobox.h>
#include <klineedit.h>
#include <ktextbrowser.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <klistview.h>

#include "kexiglobal.h"
#include "kexi.h"
#include "kexidb.h"
#include "kexibrowser.h"

#include "kexicreateproject.h"

KexiCreateProject::KexiCreateProject(Kexi *main, QWidget *parent, const char *name, bool modal, WFlags f) : KWizard(parent,name,modal,f)
{
	m_main = main;
	
	setCaption(i18n("create project"));

	m_wpic = QPixmap(locate("data","kexi/createproject.png"));
	
	connect(this, SIGNAL(selected(const QString &)), this, SLOT(nextClicked(const QString &)));
	
	QWidget *page0 = generatePage0();
	addPage(page0, i18n("New Project"));

	m_page1 = generatePage1();
	addPage(m_page1, i18n("Database Location"));

	m_page2 = generatePage2();
	addPage(m_page2, i18n("Connecting"));
		
	if(m_cEngine->count() > 0)
	{
		engineSelectionChanged(m_cEngine->text(m_cEngine->currentItem()));
		kdDebug() << "item: " << m_cEngine->currentItem() << endl;
	}
}

QWidget *KexiCreateProject::generatePage0()
{
	QWidget *p0 = new QWidget(this);
	QGridLayout *g0 = new QGridLayout(p0);
	
	QLabel *pic0 = new QLabel("", p0);
	pic0->setPixmap(m_wpic);
	
	QLabel *lEngine = new QLabel(i18n("Engine:"), p0);
	m_cEngine = new KComboBox(p0);
	
	connect(m_cEngine, SIGNAL(activated(const QString &)), this, SLOT(engineSelectionChanged(const QString &)));

	QLabel *lName = new QLabel(i18n("Database Name:"), p0);
	m_dbName = new KLineEdit(p0);
	
	KTextBrowser *iEngine = new KTextBrowser(p0);
	iEngine->setText("<i>no information avaible</i>");
	
	
	//checking drivers and making them avaible	
	QStringList engines = g_Global->g_db->engines();
	for(QStringList::Iterator it = engines.begin(); it != engines.end(); ++it)
	{
		kdDebug() << "found engine: " << *it << endl;
		m_cEngine->insertItem(*it);
	}
	
	QString description = "<b>";
	description += m_cEngine->currentText();
	description += "</b><br><hr><br>";
	//description += g_Global->g_manager->m_interfaceList->find(m_cEngine->currentText())->description();
	iEngine->setText(description);
	
	g0->addMultiCellWidget(pic0,	0,	2,	0,	0);
	g0->addWidget(lEngine,		0,	1);
	g0->addWidget(m_cEngine,		0,	2);
	g0->addWidget(lName,		1,	1);
	g0->addWidget(m_dbName,		1,	2);
	g0->addMultiCellWidget(iEngine,	2,	2,	1,	2);
	
//	#warning "TODO: don't load here!"
	return p0;
}

QWidget *KexiCreateProject::generatePage1()
{
//	kdDebug() << "KexiCreateProject::generatePage1()" << endl;
	QWidget *p1 = new QWidget(this);
	QGridLayout *g1 = new QGridLayout(p1);
	QLabel *pic1 = new QLabel("", p1);
	pic1->setPixmap(m_wpic);
	QLabel *lHost = new QLabel(i18n("Host:"), p1);
	m_dbHost = new KLineEdit(p1);
	QLabel *lUser = new QLabel(i18n("User:"), p1);
	m_dbUser = new KLineEdit(p1);
	QLabel *lPass = new QLabel(i18n("Password:"), p1);
	m_dbPass = new KLineEdit(p1);
		
	QSpacerItem *s1 = new QSpacerItem(40, 40);
	
	g1->addMultiCellWidget(pic1,	0,	3,	0,	0);
	g1->addWidget(lHost,		0,	1);
	g1->addWidget(m_dbHost,		0,	2);
	g1->addWidget(lUser,		1,	1);
	g1->addWidget(m_dbUser,		1,	2);
	g1->addWidget(lPass,		2,	1);
	g1->addWidget(m_dbPass,		2,	2);
	g1->addItem(s1,			3,	1);
	kdDebug() << "KexiCreateProject::generatePage1(): 11" << endl;
	
	return p1;
}


QWidget *KexiCreateProject::generatePage2()
{
	QWidget *p2 = new QWidget(this);
	QGridLayout *g2 = new QGridLayout(p2);
	
	QLabel *pic2 = new QLabel("", p2);
	pic2->setPixmap(m_wpic);
	
	//QLabel *lLog = new QLabel(i18n("connection log"), p2);
	m_connectionLog = new KListView(p2);
	m_connectionLog->addColumn(i18n("log message"));
	
	g2->addWidget(pic2, 0, 0);
	g2->addWidget(m_connectionLog, 0, 1);
	
	return p2;
}

void KexiCreateProject::engineSelectionChanged(const QString &engineName)
{
	kdDebug() << "engine is changeing to " << engineName << endl;
}


void KexiCreateProject::nextClicked(const QString &pageTitle)
{
	if(pageTitle == i18n("Connecting"))
	{
		kdDebug() << "it's time to connect to the db..." << endl;
		
		m_connectionLog->clear();
		if(g_Global->g_db->connectDB(m_cEngine->currentText(), m_dbHost->text(), m_dbName->text(), m_dbUser->text(), m_dbPass->text()))
		{
			KListViewItem *i = new KListViewItem(m_connectionLog, i18n("1. connected to the database"));
			KListViewItem *i2 = new KListViewItem(m_connectionLog, i18n("2. checking content"));
			QSqlQuery query("show tables;", g_Global->g_db->m_db);
			while(query.next())
			{
				QString tblName = query.value(0).toString();
				kdDebug() << "table: " << tblName << endl;
				m_main->m_browser->addTableItem(tblName);
			}
		}
		else
		{
			QString msg = i18n("connection faild: ");
			msg += g_Global->g_db->m_db->lastError().databaseText();
			KListViewItem *i = new KListViewItem(m_connectionLog, msg);
		}
		
	}
}

KexiCreateProject::~KexiCreateProject()
{
}

#include "kexicreateproject.moc"
