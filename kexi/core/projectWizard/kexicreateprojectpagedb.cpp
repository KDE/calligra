/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
*/

#include <qlayout.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qframe.h>

#include <klocale.h>
#include <kdebug.h>
#include <klistview.h>
#include <klineedit.h>
#include <kdialog.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kiconloader.h>

#include "kexiDB/kexidb.h"
#include "kexiDB/kexidbinterfacemanager.h"

#include "kexiproject.h"
#include "kexitabbrowser.h"
#include "kexicreateprojectpagedb.h"
#include "kexicreateproject.h"
#include "kexiview.h"
#include "kexidbconnection.h"

KexiCreateProjectPageDB::KexiCreateProjectPageDB(KexiCreateProject *parent, QPixmap *wpic, const char *name)
 : KexiCreateProjectPage(parent, wpic, name),m_kcp(parent)
{
	//cool picture ;)
	QLabel *lPic = new QLabel("", this);
	lPic->setPixmap(*wpic);
	lPic->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
	lPic->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum));

	//default properties
	setProperty("section", QVariant("RemoteDB"));
	setProperty("caption", QVariant(i18n("Select Database")));

	//existing databases radiobutton
	m_existingRBtn = new QRadioButton(i18n("Select from existing databases:"), this);
	m_existingRBtn->setChecked(true);

	//existing databases list
	m_databases = new KListView(this);
	m_databases->addColumn(i18n("Database"));
	connect(m_databases, SIGNAL(selectionChanged()), this, SLOT(slotDatabaseChanged()));
	connect(m_databases, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(slotDatabaseDoubleClicked(QListViewItem*)));
	connect(m_existingRBtn, SIGNAL(toggled(bool)), m_databases, SLOT(setEnabled(bool)));

	//new database radiobutton
	m_newRBtn = new QRadioButton(i18n("Create new database:"), this);
	m_newRBtn->setChecked(false);
	connect(m_newRBtn, SIGNAL(toggled(bool)), this, SLOT(slotModeChanged(bool)));

	//new database lineedit
	m_newEdit = new KLineEdit(this);
	m_newEdit->setEnabled(false);
	connect(m_newRBtn, SIGNAL(toggled(bool)), m_newEdit, SLOT(setEnabled(bool)));
	connect(m_newEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotDatabaseChanged()));

	//layout once again...
	QHBoxLayout *g = new QHBoxLayout(this);
	g->addWidget(lPic);
	g->setSpacing(KDialog::spacingHint());

	//input widgets layout
	QVBoxLayout *iv = new QVBoxLayout(g);
	iv->addWidget(m_existingRBtn);
	iv->addWidget(m_databases);
	iv->setSpacing(KDialog::spacingHint());

	//new database layout
	QHBoxLayout *nh = new QHBoxLayout(iv);
	nh->addWidget(m_newRBtn);
	nh->addWidget(m_newEdit);
	nh->setSpacing(KDialog::spacingHint());

	//buttongroup
	QButtonGroup* selectBGrp = new QButtonGroup(this);
	selectBGrp->hide();
	selectBGrp->insert(m_existingRBtn);
	selectBGrp->insert(m_newRBtn);
}

bool
KexiCreateProjectPageDB::connectHost(const QString &driver, const QString &host, const QString &user, const QString &password,
	const QString &socket, const QString &port, bool savePass)
{
	m_driver = driver;
	m_host = host;
	m_user = user;
	m_pass = password;
	m_socket = socket;
	m_port = port;

	m_databases->clear();

	KexiDB *db = m_kcp->project()->manager()->newDBInstance(driver);

	if (!db || !db->connect(host, user, password, socket, port))
	{
		KMessageBox::detailedError(0, i18n("Error in databaseconnection"),
			db ? db->latestError()->message() : "", i18n("Database Connection"));
		return false;
	}

	QStringList databases = db->databases();
	QPixmap &db_pix = kapp->iconLoader()->loadIcon("db", KIcon::Small);
	for(QStringList::Iterator it = databases.begin(); it != databases.end(); it++)
	{
		if (!db->isSystemDatabase(*it)) {
			KListViewItem *item = new KListViewItem(m_databases, (*it));
			item->setPixmap(0, db_pix);
		}
	}
	return true;
}

bool
KexiCreateProjectPageDB::connectDB()
{
	QString database = data("database").toString();
	KexiDBConnection *c = new KexiDBConnection(m_driver, m_host, database, m_user,
                                             m_pass, m_socket, m_port,
                                             m_newRBtn->isChecked());
	if(project()->initDBConnection(c))
	{
		return true;
	}
	return false;
}

void
KexiCreateProjectPageDB::slotDatabaseChanged()
{
	if(!m_databases->currentItem() && m_existingRBtn->isChecked())
		return;

	if(!data("create").toBool())
	{
		setProperty("database", QVariant(m_databases->currentItem()->text(0)));
		setProperty("finish", QVariant(true));
	}
	else
	{
		setProperty("database", QVariant(m_newEdit->text()));

		if(m_newEdit->text().length() > 0)
		{
			setProperty("finish", QVariant(true));
		}
		else
		{
			setProperty("finish", QVariant(false));
		}
	}
}

void
KexiCreateProjectPageDB::slotDatabaseDoubleClicked(QListViewItem *item)
{
	kdDebug()<<item;
	slotDatabaseChanged();
	emit acceptPage(); //automatically select this database (like using Finish btn)
}

void
KexiCreateProjectPageDB::slotModeChanged(bool state)
{
	setProperty("create", QVariant(state));

	if(state)
	{
		setProperty("database", QVariant(m_newEdit->text()));

		if(m_newEdit->text().length() > 0)
		{
			setProperty("finish", QVariant(true));
		}
		else
		{
			setProperty("finish", QVariant(false));
		}
	}
	else
	{
		if(m_databases->currentItem())
		{
			setProperty("database", QVariant(m_databases->currentItem()->text(0)));
			setProperty("finish", QVariant(true));
		}
		else
		{
			setProperty("finish", QVariant(false));
		}
	}
}

KexiCreateProjectPageDB::~KexiCreateProjectPageDB()
{
}

#include "kexicreateprojectpagedb.moc"
